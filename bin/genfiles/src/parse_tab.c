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
x->r; struct Cyc_Absyn_Vardecl* _temp116; int _temp118; struct Cyc_Absyn_Exp*
_temp120; struct Cyc_Core_Opt* _temp122; struct Cyc_Core_Opt* _temp124; struct
Cyc_Absyn_Pat* _temp126; struct Cyc_List_List* _temp128; struct Cyc_Absyn_Fndecl*
_temp130; struct Cyc_Absyn_Structdecl* _temp132; struct Cyc_Absyn_Uniondecl*
_temp134; struct Cyc_Absyn_Tuniondecl* _temp136; struct Cyc_Absyn_Typedefdecl*
_temp138; struct Cyc_Absyn_Enumdecl* _temp140; struct Cyc_List_List* _temp142;
struct _tagged_arr* _temp144; struct Cyc_List_List* _temp146; struct _tuple1*
_temp148; struct Cyc_List_List* _temp150; _LL92: if(*(( int*) _temp90) == Cyc_Absyn_Var_d){
_LL117: _temp116=(( struct Cyc_Absyn_Var_d_struct*) _temp90)->f1; goto _LL93;}
else{ goto _LL94;} _LL94: if(*(( int*) _temp90) == Cyc_Absyn_Let_d){ _LL127:
_temp126=(( struct Cyc_Absyn_Let_d_struct*) _temp90)->f1; goto _LL125; _LL125:
_temp124=(( struct Cyc_Absyn_Let_d_struct*) _temp90)->f2; goto _LL123; _LL123:
_temp122=(( struct Cyc_Absyn_Let_d_struct*) _temp90)->f3; goto _LL121; _LL121:
_temp120=(( struct Cyc_Absyn_Let_d_struct*) _temp90)->f4; goto _LL119; _LL119:
_temp118=(( struct Cyc_Absyn_Let_d_struct*) _temp90)->f5; goto _LL95;} else{
goto _LL96;} _LL96: if(*(( int*) _temp90) == Cyc_Absyn_Letv_d){ _LL129: _temp128=((
struct Cyc_Absyn_Letv_d_struct*) _temp90)->f1; goto _LL97;} else{ goto _LL98;}
_LL98: if(*(( int*) _temp90) == Cyc_Absyn_Fn_d){ _LL131: _temp130=(( struct Cyc_Absyn_Fn_d_struct*)
_temp90)->f1; goto _LL99;} else{ goto _LL100;} _LL100: if(*(( int*) _temp90) ==
Cyc_Absyn_Struct_d){ _LL133: _temp132=(( struct Cyc_Absyn_Struct_d_struct*)
_temp90)->f1; goto _LL101;} else{ goto _LL102;} _LL102: if(*(( int*) _temp90) ==
Cyc_Absyn_Union_d){ _LL135: _temp134=(( struct Cyc_Absyn_Union_d_struct*)
_temp90)->f1; goto _LL103;} else{ goto _LL104;} _LL104: if(*(( int*) _temp90) ==
Cyc_Absyn_Tunion_d){ _LL137: _temp136=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp90)->f1; goto _LL105;} else{ goto _LL106;} _LL106: if(*(( int*) _temp90) ==
Cyc_Absyn_Typedef_d){ _LL139: _temp138=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp90)->f1; goto _LL107;} else{ goto _LL108;} _LL108: if(*(( int*) _temp90) ==
Cyc_Absyn_Enum_d){ _LL141: _temp140=(( struct Cyc_Absyn_Enum_d_struct*) _temp90)->f1;
goto _LL109;} else{ goto _LL110;} _LL110: if(*(( int*) _temp90) == Cyc_Absyn_Namespace_d){
_LL145: _temp144=(( struct Cyc_Absyn_Namespace_d_struct*) _temp90)->f1; goto
_LL143; _LL143: _temp142=(( struct Cyc_Absyn_Namespace_d_struct*) _temp90)->f2;
goto _LL111;} else{ goto _LL112;} _LL112: if(*(( int*) _temp90) == Cyc_Absyn_Using_d){
_LL149: _temp148=(( struct Cyc_Absyn_Using_d_struct*) _temp90)->f1; goto _LL147;
_LL147: _temp146=(( struct Cyc_Absyn_Using_d_struct*) _temp90)->f2; goto _LL113;}
else{ goto _LL114;} _LL114: if(*(( int*) _temp90) == Cyc_Absyn_ExternC_d){
_LL151: _temp150=(( struct Cyc_Absyn_ExternC_d_struct*) _temp90)->f1; goto
_LL115;} else{ goto _LL91;} _LL93: if( _temp116->initializer != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);}{ void* _temp152=(* _temp116->name).f1; struct
Cyc_List_List* _temp162; struct Cyc_List_List* _temp164; _LL154: if( _temp152 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL155;} else{ goto _LL156;} _LL156: if((
unsigned int) _temp152 > 1u?*(( int*) _temp152) == Cyc_Absyn_Rel_n: 0){ _LL163:
_temp162=(( struct Cyc_Absyn_Rel_n_struct*) _temp152)->f1; if( _temp162 == 0){
goto _LL157;} else{ goto _LL158;}} else{ goto _LL158;} _LL158: if(( unsigned int)
_temp152 > 1u?*(( int*) _temp152) == Cyc_Absyn_Abs_n: 0){ _LL165: _temp164=((
struct Cyc_Absyn_Abs_n_struct*) _temp152)->f1; if( _temp164 == 0){ goto _LL159;}
else{ goto _LL160;}} else{ goto _LL160;} _LL160: goto _LL161; _LL155: goto
_LL153; _LL157: goto _LL153; _LL159: goto _LL153; _LL161: Cyc_Parse_err(
_tag_arr("module names not allowed on parameter declarations", sizeof(
unsigned char), 51u), x->loc); goto _LL153; _LL153:;}{ int found= 0; for( 0;
params != 0; params=(( struct Cyc_List_List*) _check_null( params))->tl){ if(
Cyc_String_zstrptrcmp((* _temp116->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( params))->hd) == 0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct _tagged_arr _temp166=*(* _temp116->name).f2;
xprintf("%.*s is not listed as a parameter", _get_arr_size( _temp166, 1u),
_temp166.curr);}), x->loc);} return;} _LL95: decl_kind= _tag_arr("let declaration",
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
struct _tagged_arr _temp167= decl_kind; xprintf("%.*s appears in parameter type",
_get_arr_size( _temp167, 1u), _temp167.curr);}), x->loc); return;} struct
_tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_arr* x){ struct _tuple12 _temp170; struct Cyc_Position_Segment* _temp171;
struct Cyc_List_List* _temp173; struct _tuple12* _temp168= env; _temp170=*
_temp168; _LL174: _temp173= _temp170.f1; goto _LL172; _LL172: _temp171= _temp170.f2;
goto _LL169; _LL169: if( _temp173 == 0){ return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct _tagged_arr _temp175=* x; xprintf("missing type for parameter %.*s",
_get_arr_size( _temp175, 1u), _temp175.curr);}), _temp171);}{ void* _temp176=(
void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp173))->hd)->r;
struct Cyc_Absyn_Vardecl* _temp182; _LL178: if(*(( int*) _temp176) == Cyc_Absyn_Var_d){
_LL183: _temp182=(( struct Cyc_Absyn_Var_d_struct*) _temp176)->f1; goto _LL179;}
else{ goto _LL180;} _LL180: goto _LL181; _LL179:{ void* _temp184=(* _temp182->name).f1;
struct Cyc_List_List* _temp194; struct Cyc_List_List* _temp196; _LL186: if(
_temp184 ==( void*) Cyc_Absyn_Loc_n){ goto _LL187;} else{ goto _LL188;} _LL188:
if(( unsigned int) _temp184 > 1u?*(( int*) _temp184) == Cyc_Absyn_Rel_n: 0){
_LL195: _temp194=(( struct Cyc_Absyn_Rel_n_struct*) _temp184)->f1; if( _temp194
== 0){ goto _LL189;} else{ goto _LL190;}} else{ goto _LL190;} _LL190: if((
unsigned int) _temp184 > 1u?*(( int*) _temp184) == Cyc_Absyn_Abs_n: 0){ _LL197:
_temp196=(( struct Cyc_Absyn_Abs_n_struct*) _temp184)->f1; if( _temp196 == 0){
goto _LL191;} else{ goto _LL192;}} else{ goto _LL192;} _LL192: goto _LL193;
_LL187: goto _LL185; _LL189: goto _LL185; _LL191: goto _LL185; _LL193: Cyc_Parse_err(
_tag_arr("module name not allowed on parameter", sizeof( unsigned char), 37u),
_temp171); goto _LL185; _LL185:;} if( Cyc_String_zstrptrcmp((* _temp182->name).f2,
x) == 0){ return({ struct _tuple2* _temp198=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2) * 1); _temp198[ 0]=({ struct _tuple2 _temp199; _temp199.f1=({
struct Cyc_Core_Opt* _temp200=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) * 1); _temp200[ 0]=({ struct Cyc_Core_Opt _temp201; _temp201.v=(
void*)(* _temp182->name).f2; _temp201;}); _temp200;}); _temp199.f2= _temp182->tq;
_temp199.f3=( void*) _temp182->type; _temp199;}); _temp198;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple12* _temp202=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp202->f1=(( struct Cyc_List_List*) _check_null( _temp173))->tl; _temp202->f2=
_temp171; _temp202;}), x);} _LL181: return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration",
sizeof( unsigned char), 25u),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp173))->hd)->loc); _LL177:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp203= tm; int _temp209; struct Cyc_Position_Segment*
_temp211; struct Cyc_List_List* _temp213; _LL205: if(( unsigned int) _temp203 >
1u?*(( int*) _temp203) == Cyc_Absyn_TypeParams_mod: 0){ _LL214: _temp213=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp203)->f1; goto _LL212; _LL212:
_temp211=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp203)->f2; goto _LL210;
_LL210: _temp209=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp203)->f3; goto
_LL206;} else{ goto _LL207;} _LL207: goto _LL208; _LL206: return 1; _LL208:
return 0; _LL204:;} static void* Cyc_Parse_id2type( struct _tagged_arr s, struct
Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s, _tag_arr("`H", sizeof(
unsigned char), 3u)) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{ return(
void*)({ struct Cyc_Absyn_VarType_struct* _temp215=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp215[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp216; _temp216.tag= Cyc_Absyn_VarType; _temp216.f1=({ struct Cyc_Absyn_Tvar*
_temp217=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp217->name=({ struct _tagged_arr* _temp218=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp218[ 0]= s; _temp218;}); _temp217->identity=
0; _temp217->kind= k; _temp217;}); _temp216;}); _temp215;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp219=
t; struct Cyc_Absyn_Tvar* _temp225; _LL221: if(( unsigned int) _temp219 > 4u?*((
int*) _temp219) == Cyc_Absyn_VarType: 0){ _LL226: _temp225=(( struct Cyc_Absyn_VarType_struct*)
_temp219)->f1; goto _LL222;} else{ goto _LL223;} _LL223: goto _LL224; _LL222:
return _temp225; _LL224: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL220:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp227=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp227[ 0]=({ struct Cyc_Absyn_VarType_struct _temp228; _temp228.tag= Cyc_Absyn_VarType;
_temp228.f1= pr; _temp228;}); _temp227;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp229=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp235; _LL231:
if(( unsigned int) _temp229 > 1u?*(( int*) _temp229) == Cyc_Absyn_Function_mod:
0){ _LL236: _temp235=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp229)->f1;
goto _LL232;} else{ goto _LL233;} _LL233: goto _LL234; _LL232: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp237= _temp235; struct Cyc_List_List* _temp243; struct Cyc_Core_Opt*
_temp245; struct Cyc_Absyn_VarargInfo* _temp247; int _temp249; struct Cyc_List_List*
_temp251; struct Cyc_Position_Segment* _temp253; struct Cyc_List_List* _temp255;
_LL239: if(*(( int*) _temp237) == Cyc_Absyn_WithTypes){ _LL252: _temp251=((
struct Cyc_Absyn_WithTypes_struct*) _temp237)->f1; goto _LL250; _LL250: _temp249=((
struct Cyc_Absyn_WithTypes_struct*) _temp237)->f2; goto _LL248; _LL248: _temp247=((
struct Cyc_Absyn_WithTypes_struct*) _temp237)->f3; goto _LL246; _LL246: _temp245=((
struct Cyc_Absyn_WithTypes_struct*) _temp237)->f4; goto _LL244; _LL244: _temp243=((
struct Cyc_Absyn_WithTypes_struct*) _temp237)->f5; goto _LL240;} else{ goto
_LL241;} _LL241: if(*(( int*) _temp237) == Cyc_Absyn_NoTypes){ _LL256: _temp255=((
struct Cyc_Absyn_NoTypes_struct*) _temp237)->f1; goto _LL254; _LL254: _temp253=((
struct Cyc_Absyn_NoTypes_struct*) _temp237)->f2; goto _LL242;} else{ goto _LL238;}
_LL240: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL242:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp255,
tds);{ struct _tuple12* _temp257=({ struct _tuple12* _temp263=( struct _tuple12*)
GC_malloc( sizeof( struct _tuple12)); _temp263->f1= tds; _temp263->f2= loc;
_temp263;}); return({ struct Cyc_List_List* _temp258=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp258->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp259=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp259[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp260; _temp260.tag= Cyc_Absyn_Function_mod;
_temp260.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp261=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp261[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp262; _temp262.tag= Cyc_Absyn_WithTypes;
_temp262.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp257, _temp255); _temp262.f2= 0; _temp262.f3= 0;
_temp262.f4= 0; _temp262.f5= 0; _temp262;}); _temp261;})); _temp260;}); _temp259;}));
_temp258->tl= 0; _temp258;});} _LL238:;} else{ return({ struct Cyc_List_List*
_temp264=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp264->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp264->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp264;});} _LL234: return({ struct Cyc_List_List*
_temp265=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp265->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp265->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp265;}); _LL230:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp266=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp266->id= d->id; _temp266->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp266;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp267=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL269: if( _temp267 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL270;} else{ goto _LL271;} _LL271: if(
_temp267 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL272;} else{ goto _LL273;}
_LL273: if( _temp267 ==( void*) Cyc_Parse_Static_sc){ goto _LL274;} else{ goto
_LL275;} _LL275: goto _LL276; _LL270: sc=( void*) Cyc_Absyn_Extern; goto _LL268;
_LL272: sc=( void*) Cyc_Absyn_ExternC; goto _LL268; _LL274: sc=( void*) Cyc_Absyn_Static;
goto _LL268; _LL276: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL268; _LL268:;}}{ struct Cyc_Core_Opt*
_temp279; void* _temp281; struct _tuple5 _temp277= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL282: _temp281= _temp277.f1; goto _LL280; _LL280: _temp279=
_temp277.f2; goto _LL278; _LL278: { struct Cyc_List_List* _temp285; struct Cyc_List_List*
_temp287; void* _temp289; struct Cyc_Absyn_Tqual _temp291; struct _tuple6
_temp283= Cyc_Parse_apply_tms( tq, _temp281, atts, d->tms); _LL292: _temp291=
_temp283.f1; goto _LL290; _LL290: _temp289= _temp283.f2; goto _LL288; _LL288:
_temp287= _temp283.f3; goto _LL286; _LL286: _temp285= _temp283.f4; goto _LL284;
_LL284: if( _temp279 != 0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp287 != 0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp293= _temp289; struct Cyc_Absyn_FnInfo _temp299; struct Cyc_List_List*
_temp301; struct Cyc_List_List* _temp303; struct Cyc_Absyn_VarargInfo* _temp305;
int _temp307; struct Cyc_List_List* _temp309; void* _temp311; struct Cyc_Core_Opt*
_temp313; struct Cyc_List_List* _temp315; _LL295: if(( unsigned int) _temp293 >
4u?*(( int*) _temp293) == Cyc_Absyn_FnType: 0){ _LL300: _temp299=(( struct Cyc_Absyn_FnType_struct*)
_temp293)->f1; _LL316: _temp315= _temp299.tvars; goto _LL314; _LL314: _temp313=
_temp299.effect; goto _LL312; _LL312: _temp311=( void*) _temp299.ret_typ; goto
_LL310; _LL310: _temp309= _temp299.args; goto _LL308; _LL308: _temp307= _temp299.c_varargs;
goto _LL306; _LL306: _temp305= _temp299.cyc_varargs; goto _LL304; _LL304:
_temp303= _temp299.rgn_po; goto _LL302; _LL302: _temp301= _temp299.attributes;
goto _LL296;} else{ goto _LL297;} _LL297: goto _LL298; _LL296: { struct Cyc_List_List*
_temp317=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp309); return({ struct Cyc_Absyn_Fndecl*
_temp318=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp318->sc=( void*) sc; _temp318->name= d->id; _temp318->tvs= _temp315;
_temp318->is_inline= is_inline; _temp318->effect= _temp313; _temp318->ret_type=(
void*) _temp311; _temp318->args= _temp317; _temp318->c_varargs= _temp307;
_temp318->cyc_varargs= _temp305; _temp318->rgn_po= _temp303; _temp318->body=
body; _temp318->cached_typ= 0; _temp318->param_vardecls= 0; _temp318->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp301, _temp285); _temp318;});} _LL298: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL294:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp319=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp319->f1=({ struct _tagged_arr*
_temp320=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp320[ 0]= _tag_arr("?", sizeof( unsigned char), 2u); _temp320;}); _temp319->f2=(*
t).f2; _temp319->f3=(* t).f3; _temp319;});} else{ return({ struct _tuple4*
_temp321=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp321->f1=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v; _temp321->f2=(*
t).f2; _temp321->f3=(* t).f3; _temp321;});}} static unsigned char _temp322[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp322, _temp322, _temp322 + 52u};
static unsigned char _temp323[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp323, _temp323, _temp323 + 63u};
static unsigned char _temp324[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp324, _temp324, _temp324 + 50u};
static unsigned char _temp325[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp325, _temp325, _temp325 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp326=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp340; void* _temp342; struct Cyc_Position_Segment*
_temp344; struct Cyc_Position_Segment* _temp346; struct Cyc_Position_Segment*
_temp348; struct Cyc_Position_Segment* _temp350; struct Cyc_Absyn_Decl* _temp352;
_LL328: if(*(( int*) _temp326) == Cyc_Parse_Type_spec){ _LL343: _temp342=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp326)->f1; goto _LL341; _LL341: _temp340=((
struct Cyc_Parse_Type_spec_struct*) _temp326)->f2; goto _LL329;} else{ goto
_LL330;} _LL330: if(*(( int*) _temp326) == Cyc_Parse_Signed_spec){ _LL345:
_temp344=(( struct Cyc_Parse_Signed_spec_struct*) _temp326)->f1; goto _LL331;}
else{ goto _LL332;} _LL332: if(*(( int*) _temp326) == Cyc_Parse_Unsigned_spec){
_LL347: _temp346=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp326)->f1; goto
_LL333;} else{ goto _LL334;} _LL334: if(*(( int*) _temp326) == Cyc_Parse_Short_spec){
_LL349: _temp348=(( struct Cyc_Parse_Short_spec_struct*) _temp326)->f1; goto
_LL335;} else{ goto _LL336;} _LL336: if(*(( int*) _temp326) == Cyc_Parse_Long_spec){
_LL351: _temp350=(( struct Cyc_Parse_Long_spec_struct*) _temp326)->f1; goto
_LL337;} else{ goto _LL338;} _LL338: if(*(( int*) _temp326) == Cyc_Parse_Decl_spec){
_LL353: _temp352=(( struct Cyc_Parse_Decl_spec_struct*) _temp326)->f1; goto
_LL339;} else{ goto _LL327;} _LL329: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp340);} last_loc= _temp340; seen_type= 1; t= _temp342; goto _LL327; _LL331:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp344);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp344);} last_loc= _temp344; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL327; _LL333: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp346);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp346);} last_loc= _temp346; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL327; _LL335: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp348);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp348);} last_loc= _temp348; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL327; _LL337: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp350);} if( seen_size){ void* _temp354= sz; _LL356: if( _temp354 ==( void*)
Cyc_Absyn_B4){ goto _LL357;} else{ goto _LL358;} _LL358: goto _LL359; _LL357: sz=(
void*) Cyc_Absyn_B8; goto _LL355; _LL359: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp350); goto _LL355; _LL355:;} last_loc=
_temp350; seen_size= 1; goto _LL327; _LL339: last_loc= _temp352->loc; if(
declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp360=( void*) _temp352->r;
struct Cyc_Absyn_Structdecl* _temp372; struct Cyc_Absyn_Tuniondecl* _temp374;
struct Cyc_Absyn_Uniondecl* _temp376; struct Cyc_Absyn_Enumdecl* _temp378;
_LL362: if(*(( int*) _temp360) == Cyc_Absyn_Struct_d){ _LL373: _temp372=((
struct Cyc_Absyn_Struct_d_struct*) _temp360)->f1; goto _LL363;} else{ goto
_LL364;} _LL364: if(*(( int*) _temp360) == Cyc_Absyn_Tunion_d){ _LL375: _temp374=((
struct Cyc_Absyn_Tunion_d_struct*) _temp360)->f1; goto _LL365;} else{ goto
_LL366;} _LL366: if(*(( int*) _temp360) == Cyc_Absyn_Union_d){ _LL377: _temp376=((
struct Cyc_Absyn_Union_d_struct*) _temp360)->f1; goto _LL367;} else{ goto _LL368;}
_LL368: if(*(( int*) _temp360) == Cyc_Absyn_Enum_d){ _LL379: _temp378=(( struct
Cyc_Absyn_Enum_d_struct*) _temp360)->f1; goto _LL369;} else{ goto _LL370;}
_LL370: goto _LL371; _LL363: { struct Cyc_List_List* _temp380=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp372->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp381=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp381[ 0]=({ struct Cyc_Absyn_StructType_struct _temp382; _temp382.tag= Cyc_Absyn_StructType;
_temp382.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp372->name))->v); _temp382.f2= _temp380; _temp382.f3= 0;
_temp382;}); _temp381;}); if( _temp372->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp383=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp383->v=( void*) _temp352; _temp383;});} goto _LL361;} _LL365: { struct Cyc_List_List*
_temp384=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp374->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp385=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp385[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp386; _temp386.tag= Cyc_Absyn_TunionType;
_temp386.f1=({ struct Cyc_Absyn_TunionInfo _temp387; _temp387.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp388=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp388[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp389; _temp389.tag= Cyc_Absyn_KnownTunion;
_temp389.f1= _temp374; _temp389;}); _temp388;})); _temp387.targs= _temp384;
_temp387.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp387;}); _temp386;});
_temp385;}); if( _temp374->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp390=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp390->v=( void*) _temp352; _temp390;});} goto _LL361;} _LL367: { struct Cyc_List_List*
_temp391=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp376->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp392=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp392[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp393; _temp393.tag= Cyc_Absyn_UnionType; _temp393.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp376->name))->v); _temp393.f2= _temp391; _temp393.f3= 0; _temp393;});
_temp392;}); if( _temp376->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp394=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp394->v=( void*) _temp352; _temp394;});} goto _LL361;} _LL369: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp395=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp395[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp396; _temp396.tag= Cyc_Absyn_EnumType; _temp396.f1=
_temp378->name; _temp396.f2= 0; _temp396;}); _temp395;}); declopt=({ struct Cyc_Core_Opt*
_temp397=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp397->v=( void*) _temp352; _temp397;}); goto _LL361; _LL371:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp352->loc); goto _LL361; _LL361:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp352->loc);} goto _LL327; _LL327:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp398=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp398[ 0]=({ struct Cyc_Absyn_IntType_struct _temp399; _temp399.tag= Cyc_Absyn_IntType;
_temp399.f1=( void*) sgn; _temp399.f2=( void*) sz; _temp399;}); _temp398;});}
else{ if( seen_sign){ void* _temp400= t; void* _temp406; void* _temp408; _LL402:
if(( unsigned int) _temp400 > 4u?*(( int*) _temp400) == Cyc_Absyn_IntType: 0){
_LL409: _temp408=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp400)->f1;
goto _LL407; _LL407: _temp406=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp400)->f2; goto _LL403;} else{ goto _LL404;} _LL404: goto _LL405; _LL403: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp410=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp410[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp411; _temp411.tag= Cyc_Absyn_IntType; _temp411.f1=( void*) sgn; _temp411.f2=(
void*) _temp406; _temp411;}); _temp410;}); goto _LL401; _LL405: Cyc_Parse_err(
_tag_arr("sign specification on non-integral type", sizeof( unsigned char), 40u),
last_loc); goto _LL401; _LL401:;} if( seen_size){ void* _temp412= t; void*
_temp418; void* _temp420; _LL414: if(( unsigned int) _temp412 > 4u?*(( int*)
_temp412) == Cyc_Absyn_IntType: 0){ _LL421: _temp420=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp412)->f1; goto _LL419; _LL419: _temp418=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp412)->f2; goto _LL415;} else{ goto _LL416;} _LL416: goto _LL417; _LL415: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp422=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp422[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp423; _temp423.tag= Cyc_Absyn_IntType; _temp423.f1=( void*) _temp420;
_temp423.f2=( void*) sz; _temp423;}); _temp422;}); goto _LL413; _LL417: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL413; _LL413:;}} return({ struct _tuple5 _temp424; _temp424.f1=
t; _temp424.f2= declopt; _temp424;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp425=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp426= _temp425->id; struct Cyc_List_List* _temp429; struct
Cyc_List_List* _temp431; void* _temp433; struct Cyc_Absyn_Tqual _temp435; struct
_tuple6 _temp427= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp425->tms);
_LL436: _temp435= _temp427.f1; goto _LL434; _LL434: _temp433= _temp427.f2; goto
_LL432; _LL432: _temp431= _temp427.f3; goto _LL430; _LL430: _temp429= _temp427.f4;
goto _LL428; _LL428: return({ struct Cyc_List_List* _temp437=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp437->hd=( void*)({ struct
_tuple7* _temp438=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp438->f1= _temp426; _temp438->f2= _temp435; _temp438->f3= _temp433; _temp438->f4=
_temp431; _temp438->f5= _temp429; _temp438;}); _temp437->tl= Cyc_Parse_apply_tmss(
_temp435, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp437;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp439; _temp439.f1= tq; _temp439.f2=
t; _temp439.f3= 0; _temp439.f4= atts; _temp439;});}{ void* _temp440=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp454;
void* _temp456; int _temp458; struct Cyc_Position_Segment* _temp460; struct Cyc_List_List*
_temp462; struct Cyc_Absyn_Tqual _temp464; void* _temp466; void* _temp468;
struct Cyc_List_List* _temp470; struct Cyc_Position_Segment* _temp472; _LL442:
if( _temp440 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL443;} else{ goto _LL444;}
_LL444: if(( unsigned int) _temp440 > 1u?*(( int*) _temp440) == Cyc_Absyn_ConstArray_mod:
0){ _LL455: _temp454=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp440)->f1;
goto _LL445;} else{ goto _LL446;} _LL446: if(( unsigned int) _temp440 > 1u?*((
int*) _temp440) == Cyc_Absyn_Function_mod: 0){ _LL457: _temp456=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp440)->f1; goto _LL447;} else{ goto
_LL448;} _LL448: if(( unsigned int) _temp440 > 1u?*(( int*) _temp440) == Cyc_Absyn_TypeParams_mod:
0){ _LL463: _temp462=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp440)->f1;
goto _LL461; _LL461: _temp460=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp440)->f2; goto _LL459; _LL459: _temp458=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp440)->f3; goto _LL449;} else{ goto _LL450;} _LL450: if(( unsigned int)
_temp440 > 1u?*(( int*) _temp440) == Cyc_Absyn_Pointer_mod: 0){ _LL469: _temp468=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp440)->f1; goto _LL467;
_LL467: _temp466=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp440)->f2;
goto _LL465; _LL465: _temp464=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp440)->f3;
goto _LL451;} else{ goto _LL452;} _LL452: if(( unsigned int) _temp440 > 1u?*((
int*) _temp440) == Cyc_Absyn_Attributes_mod: 0){ _LL473: _temp472=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp440)->f1; goto _LL471; _LL471: _temp470=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp440)->f2; goto _LL453;} else{ goto _LL441;} _LL443: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp474=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp474[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp475; _temp475.tag= Cyc_Absyn_ArrayType;
_temp475.f1=( void*) t; _temp475.f2= tq; _temp475.f3= 0; _temp475;}); _temp474;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL445: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp476=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp476[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp477; _temp477.tag= Cyc_Absyn_ArrayType;
_temp477.f1=( void*) t; _temp477.f2= tq; _temp477.f3=( struct Cyc_Absyn_Exp*)
_temp454; _temp477;}); _temp476;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL447: { void* _temp478= _temp456; struct Cyc_List_List* _temp484;
struct Cyc_Core_Opt* _temp486; struct Cyc_Absyn_VarargInfo* _temp488; int
_temp490; struct Cyc_List_List* _temp492; struct Cyc_Position_Segment* _temp494;
struct Cyc_List_List* _temp496; _LL480: if(*(( int*) _temp478) == Cyc_Absyn_WithTypes){
_LL493: _temp492=(( struct Cyc_Absyn_WithTypes_struct*) _temp478)->f1; goto
_LL491; _LL491: _temp490=(( struct Cyc_Absyn_WithTypes_struct*) _temp478)->f2;
goto _LL489; _LL489: _temp488=(( struct Cyc_Absyn_WithTypes_struct*) _temp478)->f3;
goto _LL487; _LL487: _temp486=(( struct Cyc_Absyn_WithTypes_struct*) _temp478)->f4;
goto _LL485; _LL485: _temp484=(( struct Cyc_Absyn_WithTypes_struct*) _temp478)->f5;
goto _LL481;} else{ goto _LL482;} _LL482: if(*(( int*) _temp478) == Cyc_Absyn_NoTypes){
_LL497: _temp496=(( struct Cyc_Absyn_NoTypes_struct*) _temp478)->f1; goto _LL495;
_LL495: _temp494=(( struct Cyc_Absyn_NoTypes_struct*) _temp478)->f2; goto _LL483;}
else{ goto _LL479;} _LL481: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as != 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp498=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp498->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp498->tl= fn_atts; _temp498;});} else{ new_atts=({ struct Cyc_List_List*
_temp499=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp499->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp499->tl= new_atts; _temp499;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp500=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; int _temp506; struct Cyc_Position_Segment*
_temp508; struct Cyc_List_List* _temp510; _LL502: if(( unsigned int) _temp500 >
1u?*(( int*) _temp500) == Cyc_Absyn_TypeParams_mod: 0){ _LL511: _temp510=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp500)->f1; goto _LL509; _LL509:
_temp508=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp500)->f2; goto _LL507;
_LL507: _temp506=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp500)->f3; goto
_LL503;} else{ goto _LL504;} _LL504: goto _LL505; _LL503: typvars= _temp510; tms=((
struct Cyc_List_List*) _check_null( tms))->tl; goto _LL501; _LL505: goto _LL501;
_LL501:;} if((((( ! _temp490? _temp488 == 0: 0)? _temp492 != 0: 0)?(( struct Cyc_List_List*)
_check_null( _temp492))->tl == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp492))->hd)).f1 == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp492))->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp492= 0;}
t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp492); return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ( typvars, _temp486, t, _temp492,
_temp490, _temp488, _temp484, fn_atts), new_atts,(( struct Cyc_List_List*)
_check_null( tms))->tl);} _LL483:( int) _throw((( void*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("function declaration without parameter types",
sizeof( unsigned char), 45u), _temp494)); _LL479:;} _LL449: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0){ return({ struct _tuple6 _temp512; _temp512.f1= tq;
_temp512.f2= t; _temp512.f3= _temp462; _temp512.f4= atts; _temp512;});}( int)
_throw((( void*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)( _tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp460)); _LL451: { void* _temp513= _temp468;
struct Cyc_Absyn_Exp* _temp521; struct Cyc_Absyn_Exp* _temp523; _LL515: if((
unsigned int) _temp513 > 1u?*(( int*) _temp513) == Cyc_Absyn_NonNullable_ps: 0){
_LL522: _temp521=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp513)->f1; goto
_LL516;} else{ goto _LL517;} _LL517: if(( unsigned int) _temp513 > 1u?*(( int*)
_temp513) == Cyc_Absyn_Nullable_ps: 0){ _LL524: _temp523=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp513)->f1; goto _LL518;} else{ goto _LL519;} _LL519: if( _temp513 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL520;} else{ goto _LL514;} _LL516: return Cyc_Parse_apply_tms(
_temp464, Cyc_Absyn_atb_typ( t, _temp466, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp525=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp525[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp526; _temp526.tag= Cyc_Absyn_Upper_b;
_temp526.f1= _temp521; _temp526;}); _temp525;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL518: return Cyc_Parse_apply_tms( _temp464, Cyc_Absyn_starb_typ(
t, _temp466, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp527=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp527[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp528; _temp528.tag= Cyc_Absyn_Upper_b;
_temp528.f1= _temp523; _temp528;}); _temp527;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL520: return Cyc_Parse_apply_tms( _temp464, Cyc_Absyn_tagged_typ(
t, _temp466, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL514:;}
_LL453: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp470),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL441:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp531; void* _temp533; struct _tuple5 _temp529= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL534: _temp533= _temp529.f1; goto _LL532; _LL532: _temp531=
_temp529.f2; goto _LL530; _LL530: if( _temp531 != 0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp533;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp535=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp535[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp536; _temp536.tag= Cyc_Absyn_Decl_s;
_temp536.f1= d; _temp536.f2= s; _temp536;}); _temp535;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc != 0){ void* _temp537=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL539: if( _temp537 ==(
void*) Cyc_Parse_Typedef_sc){ goto _LL540;} else{ goto _LL541;} _LL541: if(
_temp537 ==( void*) Cyc_Parse_Extern_sc){ goto _LL542;} else{ goto _LL543;}
_LL543: if( _temp537 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL544;} else{ goto
_LL545;} _LL545: if( _temp537 ==( void*) Cyc_Parse_Static_sc){ goto _LL546;}
else{ goto _LL547;} _LL547: if( _temp537 ==( void*) Cyc_Parse_Auto_sc){ goto
_LL548;} else{ goto _LL549;} _LL549: if( _temp537 ==( void*) Cyc_Parse_Register_sc){
goto _LL550;} else{ goto _LL551;} _LL551: if( _temp537 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL552;} else{ goto _LL538;} _LL540: istypedef= 1; goto _LL538; _LL542: s=(
void*) Cyc_Absyn_Extern; goto _LL538; _LL544: s=( void*) Cyc_Absyn_ExternC; goto
_LL538; _LL546: s=( void*) Cyc_Absyn_Static; goto _LL538; _LL548: s=( void*) Cyc_Absyn_Public;
goto _LL538; _LL550: s=( void*) Cyc_Absyn_Public; goto _LL538; _LL552: s=( void*)
Cyc_Absyn_Abstract; goto _LL538; _LL538:;}{ struct Cyc_List_List* _temp555;
struct Cyc_List_List* _temp557; struct _tuple0 _temp553=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL558: _temp557= _temp553.f1;
goto _LL556; _LL556: _temp555= _temp553.f2; goto _LL554; _LL554: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp555; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp559= Cyc_Parse_collapse_type_specifiers( tss, loc); if(
_temp557 == 0){ struct Cyc_Core_Opt* _temp562; void* _temp564; struct _tuple5
_temp560= _temp559; _LL565: _temp564= _temp560.f1; goto _LL563; _LL563: _temp562=
_temp560.f2; goto _LL561; _LL561: if( _temp562 != 0){ struct Cyc_Absyn_Decl* d=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null( _temp562))->v;{
void* _temp566=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp578; struct Cyc_Absyn_Structdecl*
_temp580; struct Cyc_Absyn_Uniondecl* _temp582; struct Cyc_Absyn_Tuniondecl*
_temp584; _LL568: if(*(( int*) _temp566) == Cyc_Absyn_Enum_d){ _LL579: _temp578=((
struct Cyc_Absyn_Enum_d_struct*) _temp566)->f1; goto _LL569;} else{ goto _LL570;}
_LL570: if(*(( int*) _temp566) == Cyc_Absyn_Struct_d){ _LL581: _temp580=((
struct Cyc_Absyn_Struct_d_struct*) _temp566)->f1; goto _LL571;} else{ goto
_LL572;} _LL572: if(*(( int*) _temp566) == Cyc_Absyn_Union_d){ _LL583: _temp582=((
struct Cyc_Absyn_Union_d_struct*) _temp566)->f1; goto _LL573;} else{ goto _LL574;}
_LL574: if(*(( int*) _temp566) == Cyc_Absyn_Tunion_d){ _LL585: _temp584=((
struct Cyc_Absyn_Tunion_d_struct*) _temp566)->f1; goto _LL575;} else{ goto
_LL576;} _LL576: goto _LL577; _LL569:( void*)( _temp578->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} goto _LL567; _LL571:( void*)( _temp580->sc=( void*)
s); _temp580->attributes= atts; goto _LL567; _LL573:( void*)( _temp582->sc=(
void*) s); _temp582->attributes= atts; goto _LL567; _LL575:( void*)( _temp584->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL567; _LL577: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL567:;}
return({ struct Cyc_List_List* _temp586=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp586->hd=( void*) d; _temp586->tl= 0;
_temp586;});} else{ void* _temp587= _temp564; struct Cyc_Absyn_Structdecl**
_temp601; struct Cyc_List_List* _temp603; struct _tuple1* _temp605; struct Cyc_Absyn_TunionInfo
_temp607; void* _temp609; struct Cyc_List_List* _temp611; void* _temp613; struct
Cyc_Absyn_Tuniondecl* _temp615; struct Cyc_Absyn_TunionInfo _temp617; void*
_temp619; struct Cyc_List_List* _temp621; void* _temp623; struct Cyc_Absyn_UnknownTunionInfo
_temp625; int _temp627; struct _tuple1* _temp629; struct Cyc_Absyn_Uniondecl**
_temp631; struct Cyc_List_List* _temp633; struct _tuple1* _temp635; struct Cyc_Absyn_Enumdecl*
_temp637; struct _tuple1* _temp639; _LL589: if(( unsigned int) _temp587 > 4u?*((
int*) _temp587) == Cyc_Absyn_StructType: 0){ _LL606: _temp605=(( struct Cyc_Absyn_StructType_struct*)
_temp587)->f1; goto _LL604; _LL604: _temp603=(( struct Cyc_Absyn_StructType_struct*)
_temp587)->f2; goto _LL602; _LL602: _temp601=(( struct Cyc_Absyn_StructType_struct*)
_temp587)->f3; goto _LL590;} else{ goto _LL591;} _LL591: if(( unsigned int)
_temp587 > 4u?*(( int*) _temp587) == Cyc_Absyn_TunionType: 0){ _LL608: _temp607=((
struct Cyc_Absyn_TunionType_struct*) _temp587)->f1; _LL614: _temp613=( void*)
_temp607.tunion_info; if(*(( int*) _temp613) == Cyc_Absyn_KnownTunion){ _LL616:
_temp615=(( struct Cyc_Absyn_KnownTunion_struct*) _temp613)->f1; goto _LL612;}
else{ goto _LL593;} _LL612: _temp611= _temp607.targs; goto _LL610; _LL610:
_temp609=( void*) _temp607.rgn; goto _LL592;} else{ goto _LL593;} _LL593: if((
unsigned int) _temp587 > 4u?*(( int*) _temp587) == Cyc_Absyn_TunionType: 0){
_LL618: _temp617=(( struct Cyc_Absyn_TunionType_struct*) _temp587)->f1; _LL624:
_temp623=( void*) _temp617.tunion_info; if(*(( int*) _temp623) == Cyc_Absyn_UnknownTunion){
_LL626: _temp625=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp623)->f1;
_LL630: _temp629= _temp625.name; goto _LL628; _LL628: _temp627= _temp625.is_xtunion;
goto _LL622;} else{ goto _LL595;} _LL622: _temp621= _temp617.targs; goto _LL620;
_LL620: _temp619=( void*) _temp617.rgn; goto _LL594;} else{ goto _LL595;} _LL595:
if(( unsigned int) _temp587 > 4u?*(( int*) _temp587) == Cyc_Absyn_UnionType: 0){
_LL636: _temp635=(( struct Cyc_Absyn_UnionType_struct*) _temp587)->f1; goto
_LL634; _LL634: _temp633=(( struct Cyc_Absyn_UnionType_struct*) _temp587)->f2;
goto _LL632; _LL632: _temp631=(( struct Cyc_Absyn_UnionType_struct*) _temp587)->f3;
goto _LL596;} else{ goto _LL597;} _LL597: if(( unsigned int) _temp587 > 4u?*((
int*) _temp587) == Cyc_Absyn_EnumType: 0){ _LL640: _temp639=(( struct Cyc_Absyn_EnumType_struct*)
_temp587)->f1; goto _LL638; _LL638: _temp637=(( struct Cyc_Absyn_EnumType_struct*)
_temp587)->f2; goto _LL598;} else{ goto _LL599;} _LL599: goto _LL600; _LL590: {
struct Cyc_List_List* _temp641=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp603);
struct Cyc_Absyn_Structdecl* _temp642=({ struct Cyc_Absyn_Structdecl* _temp646=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp646->sc=( void*) s; _temp646->name=({ struct Cyc_Core_Opt* _temp647=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp647->v=(
void*)(( struct _tuple1*) _check_null( _temp605)); _temp647;}); _temp646->tvs=
_temp641; _temp646->fields= 0; _temp646->attributes= 0; _temp646;}); if( atts !=
0){ Cyc_Parse_err( _tag_arr("bad attributes on struct", sizeof( unsigned char),
25u), loc);} return({ struct Cyc_List_List* _temp643=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp643->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp644=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp644[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp645; _temp645.tag= Cyc_Absyn_Struct_d; _temp645.f1=
_temp642; _temp645;}); _temp644;}), loc); _temp643->tl= 0; _temp643;});} _LL592:
if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion", sizeof(
unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp648=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp648->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp649=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp649[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp650; _temp650.tag= Cyc_Absyn_Tunion_d;
_temp650.f1= _temp615; _temp650;}); _temp649;}), loc); _temp648->tl= 0; _temp648;});
_LL594: { struct Cyc_List_List* _temp651=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp621);
struct Cyc_Absyn_Decl* _temp652= Cyc_Absyn_tunion_decl( s, _temp629, _temp651, 0,
_temp627, loc); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp653=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp653->hd=(
void*) _temp652; _temp653->tl= 0; _temp653;});} _LL596: { struct Cyc_List_List*
_temp654=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp633); struct Cyc_Absyn_Uniondecl* _temp655=({
struct Cyc_Absyn_Uniondecl* _temp660=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp660->sc=( void*) s; _temp660->name=({
struct Cyc_Core_Opt* _temp661=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp661->v=( void*)(( struct _tuple1*) _check_null( _temp635));
_temp661;}); _temp660->tvs= _temp654; _temp660->fields= 0; _temp660->attributes=
0; _temp660;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp656=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp656->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp657=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp657->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp658=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp658[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp659; _temp659.tag= Cyc_Absyn_Union_d;
_temp659.f1= _temp655; _temp659;}); _temp658;})); _temp657->loc= loc; _temp657;});
_temp656->tl= 0; _temp656;});} _LL598: { struct Cyc_Absyn_Enumdecl* _temp662=({
struct Cyc_Absyn_Enumdecl* _temp667=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp667->sc=( void*) s; _temp667->name=
_temp639; _temp667->fields= 0; _temp667;}); if( atts != 0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp663=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp663->hd=( void*)({ struct Cyc_Absyn_Decl* _temp664=(
struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp664->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp665=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp665[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp666; _temp666.tag= Cyc_Absyn_Enum_d; _temp666.f1= _temp662; _temp666;});
_temp665;})); _temp664->loc= loc; _temp664;}); _temp663->tl= 0; _temp663;});}
_LL600: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL588:;}} else{ void* t= _temp559.f1; struct Cyc_List_List*
_temp668= Cyc_Parse_apply_tmss( tq, t, _temp557, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp668); if( _temp559.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp559.f2))->v;{ void* _temp669=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp681; struct Cyc_Absyn_Tuniondecl* _temp683;
struct Cyc_Absyn_Uniondecl* _temp685; struct Cyc_Absyn_Enumdecl* _temp687;
_LL671: if(*(( int*) _temp669) == Cyc_Absyn_Struct_d){ _LL682: _temp681=((
struct Cyc_Absyn_Struct_d_struct*) _temp669)->f1; goto _LL672;} else{ goto
_LL673;} _LL673: if(*(( int*) _temp669) == Cyc_Absyn_Tunion_d){ _LL684: _temp683=((
struct Cyc_Absyn_Tunion_d_struct*) _temp669)->f1; goto _LL674;} else{ goto
_LL675;} _LL675: if(*(( int*) _temp669) == Cyc_Absyn_Union_d){ _LL686: _temp685=((
struct Cyc_Absyn_Union_d_struct*) _temp669)->f1; goto _LL676;} else{ goto _LL677;}
_LL677: if(*(( int*) _temp669) == Cyc_Absyn_Enum_d){ _LL688: _temp687=(( struct
Cyc_Absyn_Enum_d_struct*) _temp669)->f1; goto _LL678;} else{ goto _LL679;}
_LL679: goto _LL680; _LL672:( void*)( _temp681->sc=( void*) s); _temp681->attributes=
atts; atts= 0; goto _LL670; _LL674:( void*)( _temp683->sc=( void*) s); goto
_LL670; _LL676:( void*)( _temp685->sc=( void*) s); goto _LL670; _LL678:( void*)(
_temp687->sc=( void*) s); goto _LL670; _LL680: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL670; _LL670:;} decls=({ struct Cyc_List_List*
_temp689=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp689->hd=( void*) d; _temp689->tl= decls; _temp689;});} if( atts != 0){ Cyc_Parse_err((
struct _tagged_arr)({ struct _tagged_arr _temp690= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s in typedef",
_get_arr_size( _temp690, 1u), _temp690.curr);}), loc);} return decls;}} else{
if( _temp559.f2 != 0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp691= _temp668; for( 0; _temp691 != 0;( _temp691=(( struct
Cyc_List_List*) _check_null( _temp691))->tl, _temp555=(( struct Cyc_List_List*)
_check_null( _temp555))->tl)){ struct _tuple7 _temp694; struct Cyc_List_List*
_temp695; struct Cyc_List_List* _temp697; void* _temp699; struct Cyc_Absyn_Tqual
_temp701; struct _tuple1* _temp703; struct _tuple7* _temp692=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp691))->hd; _temp694=* _temp692; _LL704:
_temp703= _temp694.f1; goto _LL702; _LL702: _temp701= _temp694.f2; goto _LL700;
_LL700: _temp699= _temp694.f3; goto _LL698; _LL698: _temp697= _temp694.f4; goto
_LL696; _LL696: _temp695= _temp694.f5; goto _LL693; _LL693: if( _temp697 != 0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp555 == 0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected null in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp705=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp555))->hd; struct Cyc_Absyn_Vardecl*
_temp706= Cyc_Absyn_new_vardecl( _temp703, _temp699, _temp705); _temp706->tq=
_temp701;( void*)( _temp706->sc=( void*) s); _temp706->attributes= _temp695;{
struct Cyc_Absyn_Decl* _temp707=({ struct Cyc_Absyn_Decl* _temp709=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp709->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp710=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp710[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp711; _temp711.tag= Cyc_Absyn_Var_d; _temp711.f1= _temp706; _temp711;});
_temp710;})); _temp709->loc= loc; _temp709;}); decls=({ struct Cyc_List_List*
_temp708=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp708->hd=( void*) _temp707; _temp708->tl= decls; _temp708;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_String_strlen( s) != 1){ Cyc_Parse_err(( struct _tagged_arr)({
struct _tagged_arr _temp712= s; xprintf("bad kind: %.*s", _get_arr_size(
_temp712, 1u), _temp712.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;} else{
switch(*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), 0))){ case 'A': _LL713: return( void*) Cyc_Absyn_AnyKind; case 'M':
_LL714: return( void*) Cyc_Absyn_MemKind; case 'B': _LL715: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL716: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL717: return(
void*) Cyc_Absyn_EffKind; default: _LL718: Cyc_Parse_err(( struct _tagged_arr)({
struct _tagged_arr _temp720= s; xprintf("bad kind: %.*s", _get_arr_size(
_temp720, 1u), _temp720.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp721=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp721->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp722=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp722[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp723; _temp723.tag= Cyc_Absyn_Attributes_mod;
_temp723.f1= loc; _temp723.f2= atts; _temp723;}); _temp722;})); _temp721->tl=
tms; _temp721;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(( struct
_tagged_arr)({ struct _tagged_arr _temp724= Cyc_Absyn_attribute2string(( void*)((
struct Cyc_List_List*) _check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_get_arr_size( _temp724, 1u), _temp724.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp725=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp725[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp726; _temp726.tag= Cyc_Absyn_Typedef_d; _temp726.f1=({
struct Cyc_Absyn_Typedefdecl* _temp727=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp727->name= x; _temp727->tvs=(*
t).f4; _temp727->defn=( void*)(* t).f3; _temp727;}); _temp726;}); _temp725;}),
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
unsigned char _temp729[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp729, _temp729, _temp729 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp730= yy1; struct _tuple16* _temp736; _LL732: if(*(( void**)
_temp730) == Cyc_Int_tok){ _LL737: _temp736=(( struct Cyc_Int_tok_struct*)
_temp730)->f1; goto _LL733;} else{ goto _LL734;} _LL734: goto _LL735; _LL733:
return _temp736; _LL735:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL731:;}
static unsigned char _temp739[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp739, _temp739, _temp739 + 11u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ void* _temp740= yy1; struct
_tagged_arr _temp746; _LL742: if(*(( void**) _temp740) == Cyc_String_tok){
_LL747: _temp746=(( struct Cyc_String_tok_struct*) _temp740)->f1; goto _LL743;}
else{ goto _LL744;} _LL744: goto _LL745; _LL743: return _temp746; _LL745:( int)
_throw(( void*)& Cyc_yyfail_String_tok); _LL741:;} static unsigned char _temp749[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp749, _temp749, _temp749 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp750= yy1; unsigned char _temp756; _LL752: if(*(( void**) _temp750) ==
Cyc_Char_tok){ _LL757: _temp756=(( struct Cyc_Char_tok_struct*) _temp750)->f1;
goto _LL753;} else{ goto _LL754;} _LL754: goto _LL755; _LL753: return _temp756;
_LL755:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL751:;} static
unsigned char _temp759[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp759, _temp759, _temp759 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp760= yy1; void*
_temp766; _LL762: if(*(( void**) _temp760) == Cyc_Pointer_Sort_tok){ _LL767:
_temp766=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp760)->f1; goto
_LL763;} else{ goto _LL764;} _LL764: goto _LL765; _LL763: return _temp766;
_LL765:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL761:;} static
unsigned char _temp769[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp769, _temp769, _temp769 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp770= yy1; struct Cyc_Absyn_Exp*
_temp776; _LL772: if(*(( void**) _temp770) == Cyc_Exp_tok){ _LL777: _temp776=((
struct Cyc_Exp_tok_struct*) _temp770)->f1; goto _LL773;} else{ goto _LL774;}
_LL774: goto _LL775; _LL773: return _temp776; _LL775:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL771:;} static unsigned char _temp779[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{ _temp779, _temp779, _temp779 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( void* yy1){ void* _temp780= yy1;
struct Cyc_List_List* _temp786; _LL782: if(*(( void**) _temp780) == Cyc_ExpList_tok){
_LL787: _temp786=(( struct Cyc_ExpList_tok_struct*) _temp780)->f1; goto _LL783;}
else{ goto _LL784;} _LL784: goto _LL785; _LL783: return _temp786; _LL785:( int)
_throw(( void*)& Cyc_yyfail_ExpList_tok); _LL781:;} static unsigned char
_temp789[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure,{ _temp789, _temp789, _temp789 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp790= yy1; struct Cyc_List_List*
_temp796; _LL792: if(*(( void**) _temp790) == Cyc_InitializerList_tok){ _LL797:
_temp796=(( struct Cyc_InitializerList_tok_struct*) _temp790)->f1; goto _LL793;}
else{ goto _LL794;} _LL794: goto _LL795; _LL793: return _temp796; _LL795:( int)
_throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL791:;} static unsigned char
_temp799[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{ _temp799, _temp799, _temp799 + 11u}}; void* Cyc_yyget_Primop_tok(
void* yy1){ void* _temp800= yy1; void* _temp806; _LL802: if(*(( void**) _temp800)
== Cyc_Primop_tok){ _LL807: _temp806=( void*)(( struct Cyc_Primop_tok_struct*)
_temp800)->f1; goto _LL803;} else{ goto _LL804;} _LL804: goto _LL805; _LL803:
return _temp806; _LL805:( int) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL801:;}
static unsigned char _temp809[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp809, _temp809, _temp809 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp810= yy1;
struct Cyc_Core_Opt* _temp816; _LL812: if(*(( void**) _temp810) == Cyc_Primopopt_tok){
_LL817: _temp816=(( struct Cyc_Primopopt_tok_struct*) _temp810)->f1; goto _LL813;}
else{ goto _LL814;} _LL814: goto _LL815; _LL813: return _temp816; _LL815:( int)
_throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL811:;} static unsigned char
_temp819[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{ _temp819, _temp819, _temp819 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok(
void* yy1){ void* _temp820= yy1; struct _tuple1* _temp826; _LL822: if(*(( void**)
_temp820) == Cyc_QualId_tok){ _LL827: _temp826=(( struct Cyc_QualId_tok_struct*)
_temp820)->f1; goto _LL823;} else{ goto _LL824;} _LL824: goto _LL825; _LL823:
return _temp826; _LL825:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL821:;}
static unsigned char _temp829[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp829, _temp829, _temp829 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp830= yy1;
struct Cyc_Absyn_Stmt* _temp836; _LL832: if(*(( void**) _temp830) == Cyc_Stmt_tok){
_LL837: _temp836=(( struct Cyc_Stmt_tok_struct*) _temp830)->f1; goto _LL833;}
else{ goto _LL834;} _LL834: goto _LL835; _LL833: return _temp836; _LL835:( int)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL831:;} static unsigned char _temp839[
14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure,{ _temp839, _temp839, _temp839 + 14u}}; void* Cyc_yyget_BlockItem_tok(
void* yy1){ void* _temp840= yy1; void* _temp846; _LL842: if(*(( void**) _temp840)
== Cyc_BlockItem_tok){ _LL847: _temp846=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp840)->f1; goto _LL843;} else{ goto _LL844;} _LL844: goto _LL845; _LL843:
return _temp846; _LL845:( int) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL841:;} static unsigned char _temp849[ 21u]="SwitchClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{
_temp849, _temp849, _temp849 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp850= yy1; struct Cyc_List_List* _temp856; _LL852: if(*((
void**) _temp850) == Cyc_SwitchClauseList_tok){ _LL857: _temp856=(( struct Cyc_SwitchClauseList_tok_struct*)
_temp850)->f1; goto _LL853;} else{ goto _LL854;} _LL854: goto _LL855; _LL853:
return _temp856; _LL855:( int) _throw(( void*)& Cyc_yyfail_SwitchClauseList_tok);
_LL851:;} static unsigned char _temp859[ 22u]="SwitchCClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{
_temp859, _temp859, _temp859 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok(
void* yy1){ void* _temp860= yy1; struct Cyc_List_List* _temp866; _LL862: if(*((
void**) _temp860) == Cyc_SwitchCClauseList_tok){ _LL867: _temp866=(( struct Cyc_SwitchCClauseList_tok_struct*)
_temp860)->f1; goto _LL863;} else{ goto _LL864;} _LL864: goto _LL865; _LL863:
return _temp866; _LL865:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL861:;} static unsigned char _temp869[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp869, _temp869, _temp869 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp870= yy1;
struct Cyc_Absyn_Pat* _temp876; _LL872: if(*(( void**) _temp870) == Cyc_Pattern_tok){
_LL877: _temp876=(( struct Cyc_Pattern_tok_struct*) _temp870)->f1; goto _LL873;}
else{ goto _LL874;} _LL874: goto _LL875; _LL873: return _temp876; _LL875:( int)
_throw(( void*)& Cyc_yyfail_Pattern_tok); _LL871:;} static unsigned char
_temp879[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp879, _temp879, _temp879 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp880= yy1; struct Cyc_List_List*
_temp886; _LL882: if(*(( void**) _temp880) == Cyc_PatternList_tok){ _LL887:
_temp886=(( struct Cyc_PatternList_tok_struct*) _temp880)->f1; goto _LL883;}
else{ goto _LL884;} _LL884: goto _LL885; _LL883: return _temp886; _LL885:( int)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL881:;} static unsigned char
_temp889[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp889, _temp889, _temp889 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp890= yy1; struct _tuple14* _temp896; _LL892: if(*(( void**)
_temp890) == Cyc_FieldPattern_tok){ _LL897: _temp896=(( struct Cyc_FieldPattern_tok_struct*)
_temp890)->f1; goto _LL893;} else{ goto _LL894;} _LL894: goto _LL895; _LL893:
return _temp896; _LL895:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL891:;} static unsigned char _temp899[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp899, _temp899, _temp899 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp900= yy1; struct Cyc_List_List* _temp906; _LL902: if(*((
void**) _temp900) == Cyc_FieldPatternList_tok){ _LL907: _temp906=(( struct Cyc_FieldPatternList_tok_struct*)
_temp900)->f1; goto _LL903;} else{ goto _LL904;} _LL904: goto _LL905; _LL903:
return _temp906; _LL905:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL901:;} static unsigned char _temp909[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp909, _temp909, _temp909 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp910= yy1;
struct Cyc_Absyn_Fndecl* _temp916; _LL912: if(*(( void**) _temp910) == Cyc_FnDecl_tok){
_LL917: _temp916=(( struct Cyc_FnDecl_tok_struct*) _temp910)->f1; goto _LL913;}
else{ goto _LL914;} _LL914: goto _LL915; _LL913: return _temp916; _LL915:( int)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL911:;} static unsigned char _temp919[
13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{ _temp919, _temp919, _temp919 + 13u}}; struct Cyc_List_List*
Cyc_yyget_DeclList_tok( void* yy1){ void* _temp920= yy1; struct Cyc_List_List*
_temp926; _LL922: if(*(( void**) _temp920) == Cyc_DeclList_tok){ _LL927:
_temp926=(( struct Cyc_DeclList_tok_struct*) _temp920)->f1; goto _LL923;} else{
goto _LL924;} _LL924: goto _LL925; _LL923: return _temp926; _LL925:( int) _throw((
void*)& Cyc_yyfail_DeclList_tok); _LL921:;} static unsigned char _temp929[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{
_temp929, _temp929, _temp929 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
void* yy1){ void* _temp930= yy1; struct Cyc_Parse_Declaration_spec* _temp936;
_LL932: if(*(( void**) _temp930) == Cyc_DeclSpec_tok){ _LL937: _temp936=((
struct Cyc_DeclSpec_tok_struct*) _temp930)->f1; goto _LL933;} else{ goto _LL934;}
_LL934: goto _LL935; _LL933: return _temp936; _LL935:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL931:;} static unsigned char _temp939[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp939, _temp939, _temp939 + 13u}};
struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp940= yy1; struct
_tuple15* _temp946; _LL942: if(*(( void**) _temp940) == Cyc_InitDecl_tok){
_LL947: _temp946=(( struct Cyc_InitDecl_tok_struct*) _temp940)->f1; goto _LL943;}
else{ goto _LL944;} _LL944: goto _LL945; _LL943: return _temp946; _LL945:( int)
_throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL941:;} static unsigned char
_temp949[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={
Cyc_Core_Failure,{ _temp949, _temp949, _temp949 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp950= yy1; struct Cyc_List_List*
_temp956; _LL952: if(*(( void**) _temp950) == Cyc_InitDeclList_tok){ _LL957:
_temp956=(( struct Cyc_InitDeclList_tok_struct*) _temp950)->f1; goto _LL953;}
else{ goto _LL954;} _LL954: goto _LL955; _LL953: return _temp956; _LL955:( int)
_throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL951:;} static unsigned char
_temp959[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={
Cyc_Core_Failure,{ _temp959, _temp959, _temp959 + 17u}}; void* Cyc_yyget_StorageClass_tok(
void* yy1){ void* _temp960= yy1; void* _temp966; _LL962: if(*(( void**) _temp960)
== Cyc_StorageClass_tok){ _LL967: _temp966=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp960)->f1; goto _LL963;} else{ goto _LL964;} _LL964: goto _LL965; _LL963:
return _temp966; _LL965:( int) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL961:;} static unsigned char _temp969[ 18u]="TypeSpecifier_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp969, _temp969, _temp969 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp970= yy1; void* _temp976; _LL972: if(*(( void**) _temp970) ==
Cyc_TypeSpecifier_tok){ _LL977: _temp976=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp970)->f1; goto _LL973;} else{ goto _LL974;} _LL974: goto _LL975; _LL973:
return _temp976; _LL975:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL971:;} static unsigned char _temp979[ 18u]="StructOrUnion_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp979, _temp979, _temp979 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp980= yy1; void* _temp986; _LL982: if(*(( void**) _temp980) ==
Cyc_StructOrUnion_tok){ _LL987: _temp986=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp980)->f1; goto _LL983;} else{ goto _LL984;} _LL984: goto _LL985; _LL983:
return _temp986; _LL985:( int) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL981:;} static unsigned char _temp989[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp989, _temp989, _temp989 + 13u}};
struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ void* _temp990= yy1;
struct Cyc_Absyn_Tqual _temp996; _LL992: if(*(( void**) _temp990) == Cyc_TypeQual_tok){
_LL997: _temp996=(( struct Cyc_TypeQual_tok_struct*) _temp990)->f1; goto _LL993;}
else{ goto _LL994;} _LL994: goto _LL995; _LL993: return _temp996; _LL995:( int)
_throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL991:;} static unsigned char
_temp999[ 24u]="StructFieldDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{ _temp999, _temp999,
_temp999 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok( void*
yy1){ void* _temp1000= yy1; struct Cyc_List_List* _temp1006; _LL1002: if(*((
void**) _temp1000) == Cyc_StructFieldDeclList_tok){ _LL1007: _temp1006=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp1000)->f1; goto _LL1003;} else{ goto
_LL1004;} _LL1004: goto _LL1005; _LL1003: return _temp1006; _LL1005:( int)
_throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL1001:;} static
unsigned char _temp1009[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp1009, _temp1009,
_temp1009 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp1010= yy1; struct Cyc_List_List* _temp1016; _LL1012: if(*((
void**) _temp1010) == Cyc_StructFieldDeclListList_tok){ _LL1017: _temp1016=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp1010)->f1; goto _LL1013;}
else{ goto _LL1014;} _LL1014: goto _LL1015; _LL1013: return _temp1016; _LL1015:(
int) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL1011:;} static
unsigned char _temp1019[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp1019, _temp1019,
_temp1019 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp1020= yy1; struct Cyc_List_List* _temp1026; _LL1022: if(*((
void**) _temp1020) == Cyc_TypeModifierList_tok){ _LL1027: _temp1026=(( struct
Cyc_TypeModifierList_tok_struct*) _temp1020)->f1; goto _LL1023;} else{ goto
_LL1024;} _LL1024: goto _LL1025; _LL1023: return _temp1026; _LL1025:( int)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1021:;} static
unsigned char _temp1029[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure,{ _temp1029, _temp1029, _temp1029 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1030= yy1; void* _temp1036;
_LL1032: if(*(( void**) _temp1030) == Cyc_Rgn_tok){ _LL1037: _temp1036=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1030)->f1; goto _LL1033;} else{ goto _LL1034;}
_LL1034: goto _LL1035; _LL1033: return _temp1036; _LL1035:( int) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1031:;} static unsigned char _temp1039[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{
_temp1039, _temp1039, _temp1039 + 15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ void* _temp1040= yy1; struct Cyc_Parse_Declarator* _temp1046;
_LL1042: if(*(( void**) _temp1040) == Cyc_Declarator_tok){ _LL1047: _temp1046=((
struct Cyc_Declarator_tok_struct*) _temp1040)->f1; goto _LL1043;} else{ goto
_LL1044;} _LL1044: goto _LL1045; _LL1043: return _temp1046; _LL1045:( int)
_throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1041:;} static unsigned char
_temp1049[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{ _temp1049, _temp1049,
_temp1049 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok( void* yy1){
void* _temp1050= yy1; struct _tuple13* _temp1056; _LL1052: if(*(( void**)
_temp1050) == Cyc_DeclaratorExpopt_tok){ _LL1057: _temp1056=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1050)->f1; goto _LL1053;} else{ goto _LL1054;} _LL1054: goto _LL1055;
_LL1053: return _temp1056; _LL1055:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL1051:;} static unsigned char _temp1059[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp1059, _temp1059, _temp1059 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1060= yy1; struct Cyc_List_List* _temp1066; _LL1062: if(*((
void**) _temp1060) == Cyc_DeclaratorExpoptList_tok){ _LL1067: _temp1066=((
struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1060)->f1; goto _LL1063;}
else{ goto _LL1064;} _LL1064: goto _LL1065; _LL1063: return _temp1066; _LL1065:(
int) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL1061:;} static
unsigned char _temp1069[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1069, _temp1069,
_temp1069 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1070= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1076;
_LL1072: if(*(( void**) _temp1070) == Cyc_AbstractDeclarator_tok){ _LL1077:
_temp1076=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1070)->f1; goto
_LL1073;} else{ goto _LL1074;} _LL1074: goto _LL1075; _LL1073: return _temp1076;
_LL1075:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1071:;}
static unsigned char _temp1079[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp1079, _temp1079, _temp1079 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1080= yy1; int _temp1086; _LL1082:
if(*(( void**) _temp1080) == Cyc_Bool_tok){ _LL1087: _temp1086=(( struct Cyc_Bool_tok_struct*)
_temp1080)->f1; goto _LL1083;} else{ goto _LL1084;} _LL1084: goto _LL1085;
_LL1083: return _temp1086; _LL1085:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1081:;} static unsigned char _temp1089[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1089, _temp1089, _temp1089 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1090= yy1; void* _temp1096;
_LL1092: if(*(( void**) _temp1090) == Cyc_Scope_tok){ _LL1097: _temp1096=( void*)((
struct Cyc_Scope_tok_struct*) _temp1090)->f1; goto _LL1093;} else{ goto _LL1094;}
_LL1094: goto _LL1095; _LL1093: return _temp1096; _LL1095:( int) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1091:;} static unsigned char _temp1099[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1099, _temp1099, _temp1099 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1100= yy1; struct Cyc_Absyn_Tunionfield* _temp1106;
_LL1102: if(*(( void**) _temp1100) == Cyc_TunionField_tok){ _LL1107: _temp1106=((
struct Cyc_TunionField_tok_struct*) _temp1100)->f1; goto _LL1103;} else{ goto
_LL1104;} _LL1104: goto _LL1105; _LL1103: return _temp1106; _LL1105:( int)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1101:;} static unsigned char
_temp1109[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1109, _temp1109, _temp1109 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1110= yy1; struct Cyc_List_List*
_temp1116; _LL1112: if(*(( void**) _temp1110) == Cyc_TunionFieldList_tok){
_LL1117: _temp1116=(( struct Cyc_TunionFieldList_tok_struct*) _temp1110)->f1;
goto _LL1113;} else{ goto _LL1114;} _LL1114: goto _LL1115; _LL1113: return
_temp1116; _LL1115:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1111:;} static unsigned char _temp1119[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1119, _temp1119, _temp1119 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1120= yy1; struct _tuple18* _temp1126; _LL1122: if(*((
void**) _temp1120) == Cyc_QualSpecList_tok){ _LL1127: _temp1126=(( struct Cyc_QualSpecList_tok_struct*)
_temp1120)->f1; goto _LL1123;} else{ goto _LL1124;} _LL1124: goto _LL1125;
_LL1123: return _temp1126; _LL1125:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1121:;} static unsigned char _temp1129[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1129, _temp1129, _temp1129 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1130= yy1;
struct Cyc_List_List* _temp1136; _LL1132: if(*(( void**) _temp1130) == Cyc_IdList_tok){
_LL1137: _temp1136=(( struct Cyc_IdList_tok_struct*) _temp1130)->f1; goto
_LL1133;} else{ goto _LL1134;} _LL1134: goto _LL1135; _LL1133: return _temp1136;
_LL1135:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1131:;} static
unsigned char _temp1139[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1139, _temp1139, _temp1139 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1140= yy1;
struct _tuple2* _temp1146; _LL1142: if(*(( void**) _temp1140) == Cyc_ParamDecl_tok){
_LL1147: _temp1146=(( struct Cyc_ParamDecl_tok_struct*) _temp1140)->f1; goto
_LL1143;} else{ goto _LL1144;} _LL1144: goto _LL1145; _LL1143: return _temp1146;
_LL1145:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1141:;} static
unsigned char _temp1149[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1149, _temp1149,
_temp1149 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1150= yy1; struct Cyc_List_List* _temp1156; _LL1152: if(*(( void**)
_temp1150) == Cyc_ParamDeclList_tok){ _LL1157: _temp1156=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1150)->f1; goto _LL1153;} else{ goto _LL1154;} _LL1154: goto _LL1155;
_LL1153: return _temp1156; _LL1155:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1151:;} static unsigned char _temp1159[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1159, _temp1159, _temp1159 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1160= yy1; struct _tuple17* _temp1166; _LL1162: if(*((
void**) _temp1160) == Cyc_ParamDeclListBool_tok){ _LL1167: _temp1166=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1160)->f1; goto _LL1163;} else{ goto
_LL1164;} _LL1164: goto _LL1165; _LL1163: return _temp1166; _LL1165:( int)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1161:;} static
unsigned char _temp1169[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1169, _temp1169, _temp1169 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1170=
yy1; struct Cyc_List_List* _temp1176; _LL1172: if(*(( void**) _temp1170) == Cyc_TypeList_tok){
_LL1177: _temp1176=(( struct Cyc_TypeList_tok_struct*) _temp1170)->f1; goto
_LL1173;} else{ goto _LL1174;} _LL1174: goto _LL1175; _LL1173: return _temp1176;
_LL1175:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1171:;} static
unsigned char _temp1179[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1179, _temp1179,
_temp1179 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1180= yy1; struct Cyc_List_List* _temp1186; _LL1182: if(*(( void**)
_temp1180) == Cyc_DesignatorList_tok){ _LL1187: _temp1186=(( struct Cyc_DesignatorList_tok_struct*)
_temp1180)->f1; goto _LL1183;} else{ goto _LL1184;} _LL1184: goto _LL1185;
_LL1183: return _temp1186; _LL1185:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1181:;} static unsigned char _temp1189[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1189,
_temp1189, _temp1189 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1190= yy1; void* _temp1196; _LL1192: if(*(( void**) _temp1190) == Cyc_Designator_tok){
_LL1197: _temp1196=( void*)(( struct Cyc_Designator_tok_struct*) _temp1190)->f1;
goto _LL1193;} else{ goto _LL1194;} _LL1194: goto _LL1195; _LL1193: return
_temp1196; _LL1195:( int) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1191:;}
static unsigned char _temp1199[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1199, _temp1199, _temp1199 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1200= yy1; void* _temp1206;
_LL1202: if(*(( void**) _temp1200) == Cyc_Kind_tok){ _LL1207: _temp1206=( void*)((
struct Cyc_Kind_tok_struct*) _temp1200)->f1; goto _LL1203;} else{ goto _LL1204;}
_LL1204: goto _LL1205; _LL1203: return _temp1206; _LL1205:( int) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1201:;} static unsigned char _temp1209[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1209, _temp1209, _temp1209 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1210= yy1; void* _temp1216; _LL1212: if(*(( void**) _temp1210) == Cyc_Type_tok){
_LL1217: _temp1216=( void*)(( struct Cyc_Type_tok_struct*) _temp1210)->f1; goto
_LL1213;} else{ goto _LL1214;} _LL1214: goto _LL1215; _LL1213: return _temp1216;
_LL1215:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1211:;} static
unsigned char _temp1219[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1219, _temp1219,
_temp1219 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1220= yy1; struct Cyc_List_List* _temp1226; _LL1222: if(*(( void**)
_temp1220) == Cyc_AttributeList_tok){ _LL1227: _temp1226=(( struct Cyc_AttributeList_tok_struct*)
_temp1220)->f1; goto _LL1223;} else{ goto _LL1224;} _LL1224: goto _LL1225;
_LL1223: return _temp1226; _LL1225:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1221:;} static unsigned char _temp1229[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1229,
_temp1229, _temp1229 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1230= yy1; void* _temp1236; _LL1232: if(*(( void**) _temp1230) == Cyc_Attribute_tok){
_LL1237: _temp1236=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1230)->f1;
goto _LL1233;} else{ goto _LL1234;} _LL1234: goto _LL1235; _LL1233: return
_temp1236; _LL1235:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1231:;}
static unsigned char _temp1239[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1239, _temp1239, _temp1239 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1240= yy1; struct Cyc_Absyn_Enumfield* _temp1246; _LL1242: if(*(( void**)
_temp1240) == Cyc_Enumfield_tok){ _LL1247: _temp1246=(( struct Cyc_Enumfield_tok_struct*)
_temp1240)->f1; goto _LL1243;} else{ goto _LL1244;} _LL1244: goto _LL1245;
_LL1243: return _temp1246; _LL1245:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1241:;} static unsigned char _temp1249[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1249, _temp1249, _temp1249 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1250= yy1; struct Cyc_List_List* _temp1256; _LL1252: if(*((
void**) _temp1250) == Cyc_EnumfieldList_tok){ _LL1257: _temp1256=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1250)->f1; goto _LL1253;} else{ goto _LL1254;} _LL1254: goto _LL1255;
_LL1253: return _temp1256; _LL1255:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1251:;} static unsigned char _temp1259[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1259, _temp1259, _temp1259 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1260= yy1;
struct Cyc_Core_Opt* _temp1266; _LL1262: if(*(( void**) _temp1260) == Cyc_TypeOpt_tok){
_LL1267: _temp1266=(( struct Cyc_TypeOpt_tok_struct*) _temp1260)->f1; goto
_LL1263;} else{ goto _LL1264;} _LL1264: goto _LL1265; _LL1263: return _temp1266;
_LL1265:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1261:;} static
unsigned char _temp1269[ 13u]="Rgnorder_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{ _temp1269, _temp1269, _temp1269 +
13u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok( void* yy1){ void* _temp1270=
yy1; struct Cyc_List_List* _temp1276; _LL1272: if(*(( void**) _temp1270) == Cyc_Rgnorder_tok){
_LL1277: _temp1276=(( struct Cyc_Rgnorder_tok_struct*) _temp1270)->f1; goto
_LL1273;} else{ goto _LL1274;} _LL1274: goto _LL1275; _LL1273: return _temp1276;
_LL1275:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1271:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_arr text; } ; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct
Cyc_Yyltype _temp1278; _temp1278.timestamp= 0; _temp1278.first_line= 0;
_temp1278.first_column= 0; _temp1278.last_line= 0; _temp1278.last_column= 0;
_temp1278.text= _tag_arr("", sizeof( unsigned char), 1u); _temp1278;});} static
unsigned char _temp1279[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0,{
_temp1279, _temp1279, _temp1279 + 1u}}; static short Cyc_yytranslate[ 353u]={ 0,
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
unsigned char _temp1280[ 2u]="$"; static unsigned char _temp1281[ 6u]="error";
static unsigned char _temp1282[ 12u]="$undefined."; static unsigned char
_temp1283[ 5u]="AUTO"; static unsigned char _temp1284[ 9u]="REGISTER"; static
unsigned char _temp1285[ 7u]="STATIC"; static unsigned char _temp1286[ 7u]="EXTERN";
static unsigned char _temp1287[ 8u]="TYPEDEF"; static unsigned char _temp1288[ 5u]="VOID";
static unsigned char _temp1289[ 5u]="CHAR"; static unsigned char _temp1290[ 6u]="SHORT";
static unsigned char _temp1291[ 4u]="INT"; static unsigned char _temp1292[ 5u]="LONG";
static unsigned char _temp1293[ 6u]="FLOAT"; static unsigned char _temp1294[ 7u]="DOUBLE";
static unsigned char _temp1295[ 7u]="SIGNED"; static unsigned char _temp1296[ 9u]="UNSIGNED";
static unsigned char _temp1297[ 6u]="CONST"; static unsigned char _temp1298[ 9u]="VOLATILE";
static unsigned char _temp1299[ 9u]="RESTRICT"; static unsigned char _temp1300[
7u]="STRUCT"; static unsigned char _temp1301[ 6u]="UNION"; static unsigned char
_temp1302[ 5u]="CASE"; static unsigned char _temp1303[ 8u]="DEFAULT"; static
unsigned char _temp1304[ 7u]="INLINE"; static unsigned char _temp1305[ 7u]="SIZEOF";
static unsigned char _temp1306[ 9u]="OFFSETOF"; static unsigned char _temp1307[
3u]="IF"; static unsigned char _temp1308[ 5u]="ELSE"; static unsigned char
_temp1309[ 7u]="SWITCH"; static unsigned char _temp1310[ 6u]="WHILE"; static
unsigned char _temp1311[ 3u]="DO"; static unsigned char _temp1312[ 4u]="FOR";
static unsigned char _temp1313[ 5u]="GOTO"; static unsigned char _temp1314[ 9u]="CONTINUE";
static unsigned char _temp1315[ 6u]="BREAK"; static unsigned char _temp1316[ 7u]="RETURN";
static unsigned char _temp1317[ 5u]="ENUM"; static unsigned char _temp1318[ 8u]="NULL_kw";
static unsigned char _temp1319[ 4u]="LET"; static unsigned char _temp1320[ 6u]="THROW";
static unsigned char _temp1321[ 4u]="TRY"; static unsigned char _temp1322[ 6u]="CATCH";
static unsigned char _temp1323[ 4u]="NEW"; static unsigned char _temp1324[ 9u]="ABSTRACT";
static unsigned char _temp1325[ 9u]="FALLTHRU"; static unsigned char _temp1326[
6u]="USING"; static unsigned char _temp1327[ 10u]="NAMESPACE"; static
unsigned char _temp1328[ 7u]="TUNION"; static unsigned char _temp1329[ 8u]="XTUNION";
static unsigned char _temp1330[ 5u]="FILL"; static unsigned char _temp1331[ 8u]="CODEGEN";
static unsigned char _temp1332[ 4u]="CUT"; static unsigned char _temp1333[ 7u]="SPLICE";
static unsigned char _temp1334[ 7u]="PRINTF"; static unsigned char _temp1335[ 8u]="FPRINTF";
static unsigned char _temp1336[ 8u]="XPRINTF"; static unsigned char _temp1337[ 6u]="SCANF";
static unsigned char _temp1338[ 7u]="FSCANF"; static unsigned char _temp1339[ 7u]="SSCANF";
static unsigned char _temp1340[ 7u]="MALLOC"; static unsigned char _temp1341[ 9u]="REGION_T";
static unsigned char _temp1342[ 7u]="REGION"; static unsigned char _temp1343[ 5u]="RNEW";
static unsigned char _temp1344[ 8u]="RMALLOC"; static unsigned char _temp1345[ 7u]="PTR_OP";
static unsigned char _temp1346[ 7u]="INC_OP"; static unsigned char _temp1347[ 7u]="DEC_OP";
static unsigned char _temp1348[ 8u]="LEFT_OP"; static unsigned char _temp1349[ 9u]="RIGHT_OP";
static unsigned char _temp1350[ 6u]="LE_OP"; static unsigned char _temp1351[ 6u]="GE_OP";
static unsigned char _temp1352[ 6u]="EQ_OP"; static unsigned char _temp1353[ 6u]="NE_OP";
static unsigned char _temp1354[ 7u]="AND_OP"; static unsigned char _temp1355[ 6u]="OR_OP";
static unsigned char _temp1356[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1357[ 11u]="DIV_ASSIGN"; static unsigned char _temp1358[ 11u]="MOD_ASSIGN";
static unsigned char _temp1359[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1360[ 11u]="SUB_ASSIGN"; static unsigned char _temp1361[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1362[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1363[ 11u]="AND_ASSIGN"; static unsigned char _temp1364[ 11u]="XOR_ASSIGN";
static unsigned char _temp1365[ 10u]="OR_ASSIGN"; static unsigned char _temp1366[
9u]="ELLIPSIS"; static unsigned char _temp1367[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1368[ 12u]="COLON_COLON"; static unsigned char _temp1369[ 11u]="IDENTIFIER";
static unsigned char _temp1370[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1371[ 7u]="STRING"; static unsigned char _temp1372[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1373[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1374[ 9u]="TYPE_VAR"; static unsigned char _temp1375[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1376[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1377[ 10u]="ATTRIBUTE"; static unsigned char _temp1378[ 4u]="';'"; static
unsigned char _temp1379[ 4u]="'{'"; static unsigned char _temp1380[ 4u]="'}'";
static unsigned char _temp1381[ 4u]="'='"; static unsigned char _temp1382[ 4u]="'('";
static unsigned char _temp1383[ 4u]="')'"; static unsigned char _temp1384[ 4u]="','";
static unsigned char _temp1385[ 4u]="'_'"; static unsigned char _temp1386[ 4u]="'$'";
static unsigned char _temp1387[ 4u]="'<'"; static unsigned char _temp1388[ 4u]="'>'";
static unsigned char _temp1389[ 4u]="':'"; static unsigned char _temp1390[ 4u]="'.'";
static unsigned char _temp1391[ 4u]="'['"; static unsigned char _temp1392[ 4u]="']'";
static unsigned char _temp1393[ 4u]="'*'"; static unsigned char _temp1394[ 4u]="'@'";
static unsigned char _temp1395[ 4u]="'?'"; static unsigned char _temp1396[ 4u]="'+'";
static unsigned char _temp1397[ 4u]="'-'"; static unsigned char _temp1398[ 4u]="'&'";
static unsigned char _temp1399[ 4u]="'|'"; static unsigned char _temp1400[ 4u]="'^'";
static unsigned char _temp1401[ 4u]="'/'"; static unsigned char _temp1402[ 4u]="'%'";
static unsigned char _temp1403[ 4u]="'~'"; static unsigned char _temp1404[ 4u]="'!'";
static unsigned char _temp1405[ 5u]="prog"; static unsigned char _temp1406[ 17u]="translation_unit";
static unsigned char _temp1407[ 21u]="external_declaration"; static
unsigned char _temp1408[ 20u]="function_definition"; static unsigned char
_temp1409[ 21u]="function_definition2"; static unsigned char _temp1410[ 13u]="using_action";
static unsigned char _temp1411[ 15u]="unusing_action"; static unsigned char
_temp1412[ 17u]="namespace_action"; static unsigned char _temp1413[ 19u]="unnamespace_action";
static unsigned char _temp1414[ 12u]="declaration"; static unsigned char
_temp1415[ 17u]="declaration_list"; static unsigned char _temp1416[ 23u]="declaration_specifiers";
static unsigned char _temp1417[ 24u]="storage_class_specifier"; static
unsigned char _temp1418[ 15u]="attributes_opt"; static unsigned char _temp1419[
11u]="attributes"; static unsigned char _temp1420[ 15u]="attribute_list"; static
unsigned char _temp1421[ 10u]="attribute"; static unsigned char _temp1422[ 15u]="type_specifier";
static unsigned char _temp1423[ 5u]="kind"; static unsigned char _temp1424[ 15u]="type_qualifier";
static unsigned char _temp1425[ 15u]="enum_specifier"; static unsigned char
_temp1426[ 11u]="enum_field"; static unsigned char _temp1427[ 22u]="enum_declaration_list";
static unsigned char _temp1428[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1429[ 16u]="type_params_opt"; static unsigned char _temp1430[
16u]="struct_or_union"; static unsigned char _temp1431[ 24u]="struct_declaration_list";
static unsigned char _temp1432[ 25u]="struct_declaration_list0"; static
unsigned char _temp1433[ 21u]="init_declarator_list"; static unsigned char
_temp1434[ 22u]="init_declarator_list0"; static unsigned char _temp1435[ 16u]="init_declarator";
static unsigned char _temp1436[ 19u]="struct_declaration"; static unsigned char
_temp1437[ 25u]="specifier_qualifier_list"; static unsigned char _temp1438[ 23u]="struct_declarator_list";
static unsigned char _temp1439[ 24u]="struct_declarator_list0"; static
unsigned char _temp1440[ 18u]="struct_declarator"; static unsigned char
_temp1441[ 17u]="tunion_specifier"; static unsigned char _temp1442[ 18u]="tunion_or_xtunion";
static unsigned char _temp1443[ 17u]="tunionfield_list"; static unsigned char
_temp1444[ 18u]="tunionfield_scope"; static unsigned char _temp1445[ 12u]="tunionfield";
static unsigned char _temp1446[ 11u]="declarator"; static unsigned char
_temp1447[ 18u]="direct_declarator"; static unsigned char _temp1448[ 8u]="pointer";
static unsigned char _temp1449[ 13u]="pointer_char"; static unsigned char
_temp1450[ 8u]="rgn_opt"; static unsigned char _temp1451[ 4u]="rgn"; static
unsigned char _temp1452[ 20u]="type_qualifier_list"; static unsigned char
_temp1453[ 20u]="parameter_type_list"; static unsigned char _temp1454[ 16u]="optional_effect";
static unsigned char _temp1455[ 19u]="optional_rgn_order"; static unsigned char
_temp1456[ 10u]="rgn_order"; static unsigned char _temp1457[ 16u]="optional_inject";
static unsigned char _temp1458[ 11u]="effect_set"; static unsigned char
_temp1459[ 14u]="atomic_effect"; static unsigned char _temp1460[ 11u]="region_set";
static unsigned char _temp1461[ 15u]="parameter_list"; static unsigned char
_temp1462[ 22u]="parameter_declaration"; static unsigned char _temp1463[ 16u]="identifier_list";
static unsigned char _temp1464[ 17u]="identifier_list0"; static unsigned char
_temp1465[ 12u]="initializer"; static unsigned char _temp1466[ 18u]="array_initializer";
static unsigned char _temp1467[ 17u]="initializer_list"; static unsigned char
_temp1468[ 12u]="designation"; static unsigned char _temp1469[ 16u]="designator_list";
static unsigned char _temp1470[ 11u]="designator"; static unsigned char
_temp1471[ 10u]="type_name"; static unsigned char _temp1472[ 14u]="any_type_name";
static unsigned char _temp1473[ 15u]="type_name_list"; static unsigned char
_temp1474[ 20u]="abstract_declarator"; static unsigned char _temp1475[ 27u]="direct_abstract_declarator";
static unsigned char _temp1476[ 10u]="statement"; static unsigned char _temp1477[
18u]="labeled_statement"; static unsigned char _temp1478[ 21u]="expression_statement";
static unsigned char _temp1479[ 19u]="compound_statement"; static unsigned char
_temp1480[ 16u]="block_item_list"; static unsigned char _temp1481[ 11u]="block_item";
static unsigned char _temp1482[ 20u]="selection_statement"; static unsigned char
_temp1483[ 15u]="switch_clauses"; static unsigned char _temp1484[ 16u]="switchC_clauses";
static unsigned char _temp1485[ 20u]="iteration_statement"; static unsigned char
_temp1486[ 15u]="jump_statement"; static unsigned char _temp1487[ 8u]="pattern";
static unsigned char _temp1488[ 19u]="tuple_pattern_list"; static unsigned char
_temp1489[ 20u]="tuple_pattern_list0"; static unsigned char _temp1490[ 14u]="field_pattern";
static unsigned char _temp1491[ 19u]="field_pattern_list"; static unsigned char
_temp1492[ 20u]="field_pattern_list0"; static unsigned char _temp1493[ 11u]="expression";
static unsigned char _temp1494[ 22u]="assignment_expression"; static
unsigned char _temp1495[ 20u]="assignment_operator"; static unsigned char
_temp1496[ 23u]="conditional_expression"; static unsigned char _temp1497[ 20u]="constant_expression";
static unsigned char _temp1498[ 22u]="logical_or_expression"; static
unsigned char _temp1499[ 23u]="logical_and_expression"; static unsigned char
_temp1500[ 24u]="inclusive_or_expression"; static unsigned char _temp1501[ 24u]="exclusive_or_expression";
static unsigned char _temp1502[ 15u]="and_expression"; static unsigned char
_temp1503[ 20u]="equality_expression"; static unsigned char _temp1504[ 22u]="relational_expression";
static unsigned char _temp1505[ 17u]="shift_expression"; static unsigned char
_temp1506[ 20u]="additive_expression"; static unsigned char _temp1507[ 26u]="multiplicative_expression";
static unsigned char _temp1508[ 16u]="cast_expression"; static unsigned char
_temp1509[ 17u]="unary_expression"; static unsigned char _temp1510[ 14u]="format_primop";
static unsigned char _temp1511[ 15u]="unary_operator"; static unsigned char
_temp1512[ 19u]="postfix_expression"; static unsigned char _temp1513[ 19u]="primary_expression";
static unsigned char _temp1514[ 25u]="argument_expression_list"; static
unsigned char _temp1515[ 26u]="argument_expression_list0"; static unsigned char
_temp1516[ 9u]="constant"; static unsigned char _temp1517[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 238u]={{ _temp1280, _temp1280, _temp1280
+ 2u},{ _temp1281, _temp1281, _temp1281 + 6u},{ _temp1282, _temp1282, _temp1282
+ 12u},{ _temp1283, _temp1283, _temp1283 + 5u},{ _temp1284, _temp1284, _temp1284
+ 9u},{ _temp1285, _temp1285, _temp1285 + 7u},{ _temp1286, _temp1286, _temp1286
+ 7u},{ _temp1287, _temp1287, _temp1287 + 8u},{ _temp1288, _temp1288, _temp1288
+ 5u},{ _temp1289, _temp1289, _temp1289 + 5u},{ _temp1290, _temp1290, _temp1290
+ 6u},{ _temp1291, _temp1291, _temp1291 + 4u},{ _temp1292, _temp1292, _temp1292
+ 5u},{ _temp1293, _temp1293, _temp1293 + 6u},{ _temp1294, _temp1294, _temp1294
+ 7u},{ _temp1295, _temp1295, _temp1295 + 7u},{ _temp1296, _temp1296, _temp1296
+ 9u},{ _temp1297, _temp1297, _temp1297 + 6u},{ _temp1298, _temp1298, _temp1298
+ 9u},{ _temp1299, _temp1299, _temp1299 + 9u},{ _temp1300, _temp1300, _temp1300
+ 7u},{ _temp1301, _temp1301, _temp1301 + 6u},{ _temp1302, _temp1302, _temp1302
+ 5u},{ _temp1303, _temp1303, _temp1303 + 8u},{ _temp1304, _temp1304, _temp1304
+ 7u},{ _temp1305, _temp1305, _temp1305 + 7u},{ _temp1306, _temp1306, _temp1306
+ 9u},{ _temp1307, _temp1307, _temp1307 + 3u},{ _temp1308, _temp1308, _temp1308
+ 5u},{ _temp1309, _temp1309, _temp1309 + 7u},{ _temp1310, _temp1310, _temp1310
+ 6u},{ _temp1311, _temp1311, _temp1311 + 3u},{ _temp1312, _temp1312, _temp1312
+ 4u},{ _temp1313, _temp1313, _temp1313 + 5u},{ _temp1314, _temp1314, _temp1314
+ 9u},{ _temp1315, _temp1315, _temp1315 + 6u},{ _temp1316, _temp1316, _temp1316
+ 7u},{ _temp1317, _temp1317, _temp1317 + 5u},{ _temp1318, _temp1318, _temp1318
+ 8u},{ _temp1319, _temp1319, _temp1319 + 4u},{ _temp1320, _temp1320, _temp1320
+ 6u},{ _temp1321, _temp1321, _temp1321 + 4u},{ _temp1322, _temp1322, _temp1322
+ 6u},{ _temp1323, _temp1323, _temp1323 + 4u},{ _temp1324, _temp1324, _temp1324
+ 9u},{ _temp1325, _temp1325, _temp1325 + 9u},{ _temp1326, _temp1326, _temp1326
+ 6u},{ _temp1327, _temp1327, _temp1327 + 10u},{ _temp1328, _temp1328, _temp1328
+ 7u},{ _temp1329, _temp1329, _temp1329 + 8u},{ _temp1330, _temp1330, _temp1330
+ 5u},{ _temp1331, _temp1331, _temp1331 + 8u},{ _temp1332, _temp1332, _temp1332
+ 4u},{ _temp1333, _temp1333, _temp1333 + 7u},{ _temp1334, _temp1334, _temp1334
+ 7u},{ _temp1335, _temp1335, _temp1335 + 8u},{ _temp1336, _temp1336, _temp1336
+ 8u},{ _temp1337, _temp1337, _temp1337 + 6u},{ _temp1338, _temp1338, _temp1338
+ 7u},{ _temp1339, _temp1339, _temp1339 + 7u},{ _temp1340, _temp1340, _temp1340
+ 7u},{ _temp1341, _temp1341, _temp1341 + 9u},{ _temp1342, _temp1342, _temp1342
+ 7u},{ _temp1343, _temp1343, _temp1343 + 5u},{ _temp1344, _temp1344, _temp1344
+ 8u},{ _temp1345, _temp1345, _temp1345 + 7u},{ _temp1346, _temp1346, _temp1346
+ 7u},{ _temp1347, _temp1347, _temp1347 + 7u},{ _temp1348, _temp1348, _temp1348
+ 8u},{ _temp1349, _temp1349, _temp1349 + 9u},{ _temp1350, _temp1350, _temp1350
+ 6u},{ _temp1351, _temp1351, _temp1351 + 6u},{ _temp1352, _temp1352, _temp1352
+ 6u},{ _temp1353, _temp1353, _temp1353 + 6u},{ _temp1354, _temp1354, _temp1354
+ 7u},{ _temp1355, _temp1355, _temp1355 + 6u},{ _temp1356, _temp1356, _temp1356
+ 11u},{ _temp1357, _temp1357, _temp1357 + 11u},{ _temp1358, _temp1358,
_temp1358 + 11u},{ _temp1359, _temp1359, _temp1359 + 11u},{ _temp1360, _temp1360,
_temp1360 + 11u},{ _temp1361, _temp1361, _temp1361 + 12u},{ _temp1362, _temp1362,
_temp1362 + 13u},{ _temp1363, _temp1363, _temp1363 + 11u},{ _temp1364, _temp1364,
_temp1364 + 11u},{ _temp1365, _temp1365, _temp1365 + 10u},{ _temp1366, _temp1366,
_temp1366 + 9u},{ _temp1367, _temp1367, _temp1367 + 11u},{ _temp1368, _temp1368,
_temp1368 + 12u},{ _temp1369, _temp1369, _temp1369 + 11u},{ _temp1370, _temp1370,
_temp1370 + 17u},{ _temp1371, _temp1371, _temp1371 + 7u},{ _temp1372, _temp1372,
_temp1372 + 19u},{ _temp1373, _temp1373, _temp1373 + 18u},{ _temp1374, _temp1374,
_temp1374 + 9u},{ _temp1375, _temp1375, _temp1375 + 16u},{ _temp1376, _temp1376,
_temp1376 + 18u},{ _temp1377, _temp1377, _temp1377 + 10u},{ _temp1378, _temp1378,
_temp1378 + 4u},{ _temp1379, _temp1379, _temp1379 + 4u},{ _temp1380, _temp1380,
_temp1380 + 4u},{ _temp1381, _temp1381, _temp1381 + 4u},{ _temp1382, _temp1382,
_temp1382 + 4u},{ _temp1383, _temp1383, _temp1383 + 4u},{ _temp1384, _temp1384,
_temp1384 + 4u},{ _temp1385, _temp1385, _temp1385 + 4u},{ _temp1386, _temp1386,
_temp1386 + 4u},{ _temp1387, _temp1387, _temp1387 + 4u},{ _temp1388, _temp1388,
_temp1388 + 4u},{ _temp1389, _temp1389, _temp1389 + 4u},{ _temp1390, _temp1390,
_temp1390 + 4u},{ _temp1391, _temp1391, _temp1391 + 4u},{ _temp1392, _temp1392,
_temp1392 + 4u},{ _temp1393, _temp1393, _temp1393 + 4u},{ _temp1394, _temp1394,
_temp1394 + 4u},{ _temp1395, _temp1395, _temp1395 + 4u},{ _temp1396, _temp1396,
_temp1396 + 4u},{ _temp1397, _temp1397, _temp1397 + 4u},{ _temp1398, _temp1398,
_temp1398 + 4u},{ _temp1399, _temp1399, _temp1399 + 4u},{ _temp1400, _temp1400,
_temp1400 + 4u},{ _temp1401, _temp1401, _temp1401 + 4u},{ _temp1402, _temp1402,
_temp1402 + 4u},{ _temp1403, _temp1403, _temp1403 + 4u},{ _temp1404, _temp1404,
_temp1404 + 4u},{ _temp1405, _temp1405, _temp1405 + 5u},{ _temp1406, _temp1406,
_temp1406 + 17u},{ _temp1407, _temp1407, _temp1407 + 21u},{ _temp1408, _temp1408,
_temp1408 + 20u},{ _temp1409, _temp1409, _temp1409 + 21u},{ _temp1410, _temp1410,
_temp1410 + 13u},{ _temp1411, _temp1411, _temp1411 + 15u},{ _temp1412, _temp1412,
_temp1412 + 17u},{ _temp1413, _temp1413, _temp1413 + 19u},{ _temp1414, _temp1414,
_temp1414 + 12u},{ _temp1415, _temp1415, _temp1415 + 17u},{ _temp1416, _temp1416,
_temp1416 + 23u},{ _temp1417, _temp1417, _temp1417 + 24u},{ _temp1418, _temp1418,
_temp1418 + 15u},{ _temp1419, _temp1419, _temp1419 + 11u},{ _temp1420, _temp1420,
_temp1420 + 15u},{ _temp1421, _temp1421, _temp1421 + 10u},{ _temp1422, _temp1422,
_temp1422 + 15u},{ _temp1423, _temp1423, _temp1423 + 5u},{ _temp1424, _temp1424,
_temp1424 + 15u},{ _temp1425, _temp1425, _temp1425 + 15u},{ _temp1426, _temp1426,
_temp1426 + 11u},{ _temp1427, _temp1427, _temp1427 + 22u},{ _temp1428, _temp1428,
_temp1428 + 26u},{ _temp1429, _temp1429, _temp1429 + 16u},{ _temp1430, _temp1430,
_temp1430 + 16u},{ _temp1431, _temp1431, _temp1431 + 24u},{ _temp1432, _temp1432,
_temp1432 + 25u},{ _temp1433, _temp1433, _temp1433 + 21u},{ _temp1434, _temp1434,
_temp1434 + 22u},{ _temp1435, _temp1435, _temp1435 + 16u},{ _temp1436, _temp1436,
_temp1436 + 19u},{ _temp1437, _temp1437, _temp1437 + 25u},{ _temp1438, _temp1438,
_temp1438 + 23u},{ _temp1439, _temp1439, _temp1439 + 24u},{ _temp1440, _temp1440,
_temp1440 + 18u},{ _temp1441, _temp1441, _temp1441 + 17u},{ _temp1442, _temp1442,
_temp1442 + 18u},{ _temp1443, _temp1443, _temp1443 + 17u},{ _temp1444, _temp1444,
_temp1444 + 18u},{ _temp1445, _temp1445, _temp1445 + 12u},{ _temp1446, _temp1446,
_temp1446 + 11u},{ _temp1447, _temp1447, _temp1447 + 18u},{ _temp1448, _temp1448,
_temp1448 + 8u},{ _temp1449, _temp1449, _temp1449 + 13u},{ _temp1450, _temp1450,
_temp1450 + 8u},{ _temp1451, _temp1451, _temp1451 + 4u},{ _temp1452, _temp1452,
_temp1452 + 20u},{ _temp1453, _temp1453, _temp1453 + 20u},{ _temp1454, _temp1454,
_temp1454 + 16u},{ _temp1455, _temp1455, _temp1455 + 19u},{ _temp1456, _temp1456,
_temp1456 + 10u},{ _temp1457, _temp1457, _temp1457 + 16u},{ _temp1458, _temp1458,
_temp1458 + 11u},{ _temp1459, _temp1459, _temp1459 + 14u},{ _temp1460, _temp1460,
_temp1460 + 11u},{ _temp1461, _temp1461, _temp1461 + 15u},{ _temp1462, _temp1462,
_temp1462 + 22u},{ _temp1463, _temp1463, _temp1463 + 16u},{ _temp1464, _temp1464,
_temp1464 + 17u},{ _temp1465, _temp1465, _temp1465 + 12u},{ _temp1466, _temp1466,
_temp1466 + 18u},{ _temp1467, _temp1467, _temp1467 + 17u},{ _temp1468, _temp1468,
_temp1468 + 12u},{ _temp1469, _temp1469, _temp1469 + 16u},{ _temp1470, _temp1470,
_temp1470 + 11u},{ _temp1471, _temp1471, _temp1471 + 10u},{ _temp1472, _temp1472,
_temp1472 + 14u},{ _temp1473, _temp1473, _temp1473 + 15u},{ _temp1474, _temp1474,
_temp1474 + 20u},{ _temp1475, _temp1475, _temp1475 + 27u},{ _temp1476, _temp1476,
_temp1476 + 10u},{ _temp1477, _temp1477, _temp1477 + 18u},{ _temp1478, _temp1478,
_temp1478 + 21u},{ _temp1479, _temp1479, _temp1479 + 19u},{ _temp1480, _temp1480,
_temp1480 + 16u},{ _temp1481, _temp1481, _temp1481 + 11u},{ _temp1482, _temp1482,
_temp1482 + 20u},{ _temp1483, _temp1483, _temp1483 + 15u},{ _temp1484, _temp1484,
_temp1484 + 16u},{ _temp1485, _temp1485, _temp1485 + 20u},{ _temp1486, _temp1486,
_temp1486 + 15u},{ _temp1487, _temp1487, _temp1487 + 8u},{ _temp1488, _temp1488,
_temp1488 + 19u},{ _temp1489, _temp1489, _temp1489 + 20u},{ _temp1490, _temp1490,
_temp1490 + 14u},{ _temp1491, _temp1491, _temp1491 + 19u},{ _temp1492, _temp1492,
_temp1492 + 20u},{ _temp1493, _temp1493, _temp1493 + 11u},{ _temp1494, _temp1494,
_temp1494 + 22u},{ _temp1495, _temp1495, _temp1495 + 20u},{ _temp1496, _temp1496,
_temp1496 + 23u},{ _temp1497, _temp1497, _temp1497 + 20u},{ _temp1498, _temp1498,
_temp1498 + 22u},{ _temp1499, _temp1499, _temp1499 + 23u},{ _temp1500, _temp1500,
_temp1500 + 24u},{ _temp1501, _temp1501, _temp1501 + 24u},{ _temp1502, _temp1502,
_temp1502 + 15u},{ _temp1503, _temp1503, _temp1503 + 20u},{ _temp1504, _temp1504,
_temp1504 + 22u},{ _temp1505, _temp1505, _temp1505 + 17u},{ _temp1506, _temp1506,
_temp1506 + 20u},{ _temp1507, _temp1507, _temp1507 + 26u},{ _temp1508, _temp1508,
_temp1508 + 16u},{ _temp1509, _temp1509, _temp1509 + 17u},{ _temp1510, _temp1510,
_temp1510 + 14u},{ _temp1511, _temp1511, _temp1511 + 15u},{ _temp1512, _temp1512,
_temp1512 + 19u},{ _temp1513, _temp1513, _temp1513 + 19u},{ _temp1514, _temp1514,
_temp1514 + 25u},{ _temp1515, _temp1515, _temp1515 + 26u},{ _temp1516, _temp1516,
_temp1516 + 9u},{ _temp1517, _temp1517, _temp1517 + 20u}}; static short Cyc_yyr1[
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
yyss=( short*)({ unsigned int _temp3367= 10000u; short* _temp3368=( short*)
GC_malloc_atomic( sizeof( short) * _temp3367);{ unsigned int _temp3369=
_temp3367; unsigned int i; for( i= 0; i < _temp3369; i ++){ _temp3368[ i]=(
short) 0;}}; _temp3368;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3364= 10000u; void** _temp3365=( void**) GC_malloc( sizeof(
void*) * _temp3364);{ unsigned int _temp3366= _temp3364; unsigned int i; for( i=
0; i < _temp3366; i ++){ _temp3365[ i]= Cyc_yylval;}}; _temp3365;}); int
yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({ unsigned int
_temp3361= 10000u; struct Cyc_Yyltype* _temp3362=( struct Cyc_Yyltype*)
GC_malloc( sizeof( struct Cyc_Yyltype) * _temp3361);{ unsigned int _temp3363=
_temp3361; unsigned int i; for( i= 0; i < _temp3363; i ++){ _temp3362[ i]= Cyc_yynewloc();}};
_temp3362;}); int yystacksize= 10000; void* yyval= Cyc_yylval; int yylen;
yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1;
yyvsp_offset= 0; yylsp_offset= 0; yynewstate: yyss[
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
yyn){ case 1: _LL1518: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1519:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1521=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1521[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1522; _temp1522.tag= Cyc_DeclList_tok; _temp1522.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1522;});
_temp1521;}); break; case 3: _LL1520: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1524=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1524[ 0]=({ struct Cyc_DeclList_tok_struct _temp1525; _temp1525.tag= Cyc_DeclList_tok;
_temp1525.f1=({ struct Cyc_List_List* _temp1526=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1526->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1527=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1527->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1528=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1528[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1529; _temp1529.tag= Cyc_Absyn_Using_d;
_temp1529.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1529.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1529;}); _temp1528;}));
_temp1527->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1527;});
_temp1526->tl= 0; _temp1526;}); _temp1525;}); _temp1524;}); Cyc_Lex_leave_using();
break; case 4: _LL1523: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1531=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1531[ 0]=({ struct Cyc_DeclList_tok_struct _temp1532; _temp1532.tag= Cyc_DeclList_tok;
_temp1532.f1=({ struct Cyc_List_List* _temp1533=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1533->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1534=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1534->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1535=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1535[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1536; _temp1536.tag= Cyc_Absyn_Using_d;
_temp1536.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp1536.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1536;});
_temp1535;})); _temp1534->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1534;}); _temp1533->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1533;}); _temp1532;});
_temp1531;}); break; case 5: _LL1530: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1538=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1538[ 0]=({ struct Cyc_DeclList_tok_struct _temp1539; _temp1539.tag= Cyc_DeclList_tok;
_temp1539.f1=({ struct Cyc_List_List* _temp1540=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1540->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1541=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1541->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1542=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1542[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1543; _temp1543.tag=
Cyc_Absyn_Namespace_d; _temp1543.f1=({ struct _tagged_arr* _temp1544=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1544[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1544;});
_temp1543.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1543;}); _temp1542;})); _temp1541->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1541;}); _temp1540->tl= 0; _temp1540;}); _temp1539;}); _temp1538;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1537: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1546=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1546[ 0]=({ struct Cyc_DeclList_tok_struct _temp1547; _temp1547.tag= Cyc_DeclList_tok;
_temp1547.f1=({ struct Cyc_List_List* _temp1548=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1548->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1549=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1549->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1550=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1550[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1551; _temp1551.tag=
Cyc_Absyn_Namespace_d; _temp1551.f1=({ struct _tagged_arr* _temp1552=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1552[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1552;});
_temp1551.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1551;}); _temp1550;})); _temp1549->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1549;}); _temp1548->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1548;}); _temp1547;});
_temp1546;}); break; case 7: _LL1545: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1554=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1554[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1555; _temp1555.tag= Cyc_DeclList_tok; _temp1555.f1=({ struct Cyc_List_List*
_temp1556=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1556->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1557=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1557->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1558=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1558[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1559; _temp1559.tag= Cyc_Absyn_ExternC_d;
_temp1559.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1559;}); _temp1558;})); _temp1557->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1557;}); _temp1556->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1556;}); _temp1555;});
_temp1554;}); break; case 8: _LL1553: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1561=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1561[ 0]=({ struct Cyc_DeclList_tok_struct _temp1562; _temp1562.tag= Cyc_DeclList_tok;
_temp1562.f1= 0; _temp1562;}); _temp1561;}); break; case 9: _LL1560: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1564=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1564[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1565; _temp1565.tag= Cyc_DeclList_tok; _temp1565.f1=({ struct Cyc_List_List*
_temp1566=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1566->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1567=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1567[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1568; _temp1568.tag= Cyc_Absyn_Fn_d;
_temp1568.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1568;}); _temp1567;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1566->tl=
0; _temp1566;}); _temp1565;}); _temp1564;}); break; case 10: _LL1563: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1569: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1571=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1571[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1572; _temp1572.tag= Cyc_FnDecl_tok; _temp1572.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1572;});
_temp1571;}); break; case 12: _LL1570: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1574=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1574[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1575; _temp1575.tag= Cyc_FnDecl_tok;
_temp1575.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1576=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1576->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1576;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1575;}); _temp1574;}); break; case 13: _LL1573: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1578=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1578[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1579; _temp1579.tag= Cyc_FnDecl_tok;
_temp1579.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1579;}); _temp1578;}); break; case 14: _LL1577: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1581=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1581[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1582; _temp1582.tag= Cyc_FnDecl_tok;
_temp1582.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1583=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1583->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1583;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1582;}); _temp1581;}); break; case 15: _LL1580: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1585=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1585[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1586; _temp1586.tag= Cyc_FnDecl_tok;
_temp1586.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1587=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1587->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1587;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1586;}); _temp1585;}); break; case 16: _LL1584: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1589=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1589[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1590; _temp1590.tag= Cyc_FnDecl_tok;
_temp1590.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1591=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1591->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1591;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1590;}); _temp1589;}); break; case 17: _LL1588: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1592:
Cyc_Lex_leave_using(); break; case 19: _LL1593: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1595=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp1595[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1595;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1594: Cyc_Lex_leave_namespace(); break; case 21: _LL1596: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1598=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1598[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1599; _temp1599.tag= Cyc_DeclList_tok; _temp1599.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp1599;}); _temp1598;}); break; case 22: _LL1597: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1601=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1601[ 0]=({ struct Cyc_DeclList_tok_struct _temp1602; _temp1602.tag= Cyc_DeclList_tok;
_temp1602.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1602;}); _temp1601;}); break; case 23: _LL1600: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1604=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1604[ 0]=({ struct Cyc_DeclList_tok_struct _temp1605; _temp1605.tag= Cyc_DeclList_tok;
_temp1605.f1=({ struct Cyc_List_List* _temp1606=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1606->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1606->tl=
0; _temp1606;}); _temp1605;}); _temp1604;}); break; case 24: _LL1603: { struct
Cyc_List_List* _temp1608= 0;{ struct Cyc_List_List* _temp1609= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); for( 0;
_temp1609 != 0; _temp1609=(( struct Cyc_List_List*) _check_null( _temp1609))->tl){
struct _tagged_arr* _temp1610=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1609))->hd; struct _tuple1* _temp1611=({ struct _tuple1*
_temp1614=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1614->f1=
Cyc_Absyn_rel_ns_null; _temp1614->f2= _temp1610; _temp1614;}); struct Cyc_Absyn_Vardecl*
_temp1612= Cyc_Absyn_new_vardecl( _temp1611, Cyc_Absyn_wildtyp( 0), 0);
_temp1608=({ struct Cyc_List_List* _temp1613=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1613->hd=( void*) _temp1612; _temp1613->tl=
_temp1608; _temp1613;});}} _temp1608=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1608); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1615=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1615[ 0]=({ struct Cyc_DeclList_tok_struct _temp1616; _temp1616.tag= Cyc_DeclList_tok;
_temp1616.f1=({ struct Cyc_List_List* _temp1617=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1617->hd=( void*) Cyc_Absyn_letv_decl(
_temp1608, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1617->tl= 0; _temp1617;}); _temp1616;});
_temp1615;}); break;} case 25: _LL1607: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1618:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1620=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1620[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1621; _temp1621.tag= Cyc_DeclList_tok; _temp1621.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1621;});
_temp1620;}); break; case 27: _LL1619: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1623=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1623[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1624; _temp1624.tag= Cyc_DeclSpec_tok;
_temp1624.f1=({ struct Cyc_Parse_Declaration_spec* _temp1625=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1625->sc=({ struct
Cyc_Core_Opt* _temp1626=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1626->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1626;});
_temp1625->tq= Cyc_Absyn_empty_tqual(); _temp1625->type_specs= 0; _temp1625->is_inline=
0; _temp1625->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1625;}); _temp1624;});
_temp1623;}); break; case 28: _LL1622: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc != 0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1628=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1628[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1629; _temp1629.tag= Cyc_DeclSpec_tok; _temp1629.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1630=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1630->sc=({ struct Cyc_Core_Opt* _temp1631=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1631->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1631;});
_temp1630->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1630->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1630->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1630->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes); _temp1630;});
_temp1629;}); _temp1628;}); break; case 29: _LL1627: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1633=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1633[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1634; _temp1634.tag= Cyc_DeclSpec_tok;
_temp1634.f1=({ struct Cyc_Parse_Declaration_spec* _temp1635=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1635->sc= 0;
_temp1635->tq= Cyc_Absyn_empty_tqual(); _temp1635->type_specs=({ struct Cyc_List_List*
_temp1636=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1636->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1636->tl= 0;
_temp1636;}); _temp1635->is_inline= 0; _temp1635->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1635;});
_temp1634;}); _temp1633;}); break; case 30: _LL1632: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1638=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1638[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1639; _temp1639.tag= Cyc_DeclSpec_tok;
_temp1639.f1=({ struct Cyc_Parse_Declaration_spec* _temp1640=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1640->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1640->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1640->type_specs=({ struct Cyc_List_List* _temp1641=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1641->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1641->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1641;});
_temp1640->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1640->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes); _temp1640;});
_temp1639;}); _temp1638;}); break; case 31: _LL1637: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1643=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1643[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1644; _temp1644.tag= Cyc_DeclSpec_tok;
_temp1644.f1=({ struct Cyc_Parse_Declaration_spec* _temp1645=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1645->sc= 0;
_temp1645->tq= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1645->type_specs= 0; _temp1645->is_inline= 0;
_temp1645->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1645;}); _temp1644;});
_temp1643;}); break; case 32: _LL1642: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1647=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1647[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1648; _temp1648.tag= Cyc_DeclSpec_tok;
_temp1648.f1=({ struct Cyc_Parse_Declaration_spec* _temp1649=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1649->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1649->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1649->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1649->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1649->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1649;}); _temp1648;}); _temp1647;}); break; case 33: _LL1646: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1651=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1651[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1652; _temp1652.tag= Cyc_DeclSpec_tok; _temp1652.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1653=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1653->sc= 0; _temp1653->tq= Cyc_Absyn_empty_tqual(); _temp1653->type_specs=
0; _temp1653->is_inline= 1; _temp1653->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1653;});
_temp1652;}); _temp1651;}); break; case 34: _LL1650: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1655=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1655[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1656; _temp1656.tag= Cyc_DeclSpec_tok;
_temp1656.f1=({ struct Cyc_Parse_Declaration_spec* _temp1657=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1657->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1657->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1657->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1657->is_inline=
1; _temp1657->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1657;}); _temp1656;}); _temp1655;}); break; case 35: _LL1654: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1659=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1659[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1660; _temp1660.tag= Cyc_StorageClass_tok;
_temp1660.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1660;}); _temp1659;});
break; case 36: _LL1658: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1662=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1662[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1663; _temp1663.tag=
Cyc_StorageClass_tok; _temp1663.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1663;}); _temp1662;}); break; case 37: _LL1661: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1665=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1665[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1666; _temp1666.tag=
Cyc_StorageClass_tok; _temp1666.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1666;}); _temp1665;}); break; case 38: _LL1664: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1668=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1668[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1669; _temp1669.tag=
Cyc_StorageClass_tok; _temp1669.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1669;}); _temp1668;}); break; case 39: _LL1667: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1671=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1671[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1672; _temp1672.tag= Cyc_StorageClass_tok;
_temp1672.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1672;}); _temp1671;});
break; case 40: _LL1670: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1674=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1674[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1675; _temp1675.tag=
Cyc_StorageClass_tok; _temp1675.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1675;}); _temp1674;}); break; case 41: _LL1673: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1677=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1677[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1678; _temp1678.tag=
Cyc_StorageClass_tok; _temp1678.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1678;}); _temp1677;}); break; case 42: _LL1676: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1680=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1680[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1681; _temp1681.tag=
Cyc_AttributeList_tok; _temp1681.f1= 0; _temp1681;}); _temp1680;}); break; case
43: _LL1679: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1682: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1684=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1684[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1685; _temp1685.tag=
Cyc_AttributeList_tok; _temp1685.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1685;});
_temp1684;}); break; case 45: _LL1683: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1687=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1687[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1688; _temp1688.tag=
Cyc_AttributeList_tok; _temp1688.f1=({ struct Cyc_List_List* _temp1689=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1689->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1689->tl= 0; _temp1689;}); _temp1688;}); _temp1687;});
break; case 46: _LL1686: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1691=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1691[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1692; _temp1692.tag=
Cyc_AttributeList_tok; _temp1692.f1=({ struct Cyc_List_List* _temp1693=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1693->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1693->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1693;}); _temp1692;});
_temp1691;}); break; case 47: _LL1690: { struct _tagged_arr _temp1695= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1695, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1695, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1695, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1695,
sizeof( unsigned char),( int)( _get_arr_size( _temp1695, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1695,
sizeof( unsigned char),( int)( _get_arr_size( _temp1695, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1695=( struct _tagged_arr) Cyc_String_substring(
_temp1695, 2,( int)( _get_arr_size( _temp1695, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1695,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1696=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1696[ 0]=({ struct Cyc_Attribute_tok_struct _temp1697; _temp1697.tag= Cyc_Attribute_tok;
_temp1697.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1697;}); _temp1696;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1698=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1698[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1699; _temp1699.tag= Cyc_Attribute_tok; _temp1699.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1699;}); _temp1698;});} break;}} case
48: _LL1694: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1701=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1701[ 0]=({ struct Cyc_Attribute_tok_struct _temp1702; _temp1702.tag= Cyc_Attribute_tok;
_temp1702.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1702;}); _temp1701;});
break; case 49: _LL1700: { struct _tagged_arr _temp1704= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct
_tuple16 _temp1707; int _temp1708; void* _temp1710; struct _tuple16* _temp1705=
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp1707=* _temp1705; _LL1711: _temp1710= _temp1707.f1; goto _LL1709;
_LL1709: _temp1708= _temp1707.f2; goto _LL1706; _LL1706: { void* a; if( Cyc_String_zstrcmp(
_temp1704, _tag_arr("regparm", sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp(
_temp1704, _tag_arr("__regparm__", sizeof( unsigned char), 12u)) == 0){ if(
_temp1708 <= 0? 1: _temp1708 > 3){ Cyc_Parse_err( _tag_arr("regparm requires value between 1 and 3",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp1712=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp1712[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp1713; _temp1713.tag= Cyc_Absyn_Regparm_att;
_temp1713.f1= _temp1708; _temp1713;}); _temp1712;});} else{ if( Cyc_String_zstrcmp(
_temp1704, _tag_arr("aligned", sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp(
_temp1704, _tag_arr("__aligned__", sizeof( unsigned char), 12u)) == 0){ if(
_temp1708 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
unsigned int j=( unsigned int) _temp1708; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1714=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1714[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1715; _temp1715.tag= Cyc_Absyn_Aligned_att;
_temp1715.f1= _temp1708; _temp1715;}); _temp1714;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1716=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1716[ 0]=({ struct Cyc_Attribute_tok_struct _temp1717; _temp1717.tag= Cyc_Attribute_tok;
_temp1717.f1=( void*) a; _temp1717;}); _temp1716;}); break;}} case 50: _LL1703: {
struct _tagged_arr _temp1719= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct _tagged_arr
_temp1720= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); void* a; if( Cyc_String_zstrcmp( _temp1719, _tag_arr("section",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1719, _tag_arr("__section__",
sizeof( unsigned char), 12u))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1721=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1721[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1722; _temp1722.tag=
Cyc_Absyn_Section_att; _temp1722.f1= _temp1720; _temp1722;}); _temp1721;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1723=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1723[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1724; _temp1724.tag= Cyc_Attribute_tok; _temp1724.f1=(
void*) a; _temp1724;}); _temp1723;}); break;} case 51: _LL1718: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1726=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1726[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1727; _temp1727.tag= Cyc_TypeSpecifier_tok;
_temp1727.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1727;});
_temp1726;}); break; case 52: _LL1725: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1729=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1729[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1730; _temp1730.tag=
Cyc_TypeSpecifier_tok; _temp1730.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1730;}); _temp1729;}); break; case 53: _LL1728:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1732=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1732[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1733; _temp1733.tag= Cyc_TypeSpecifier_tok;
_temp1733.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1733;});
_temp1732;}); break; case 54: _LL1731: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1735=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1735[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1736; _temp1736.tag=
Cyc_TypeSpecifier_tok; _temp1736.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1737=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1737[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1738; _temp1738.tag=
Cyc_Parse_Short_spec; _temp1738.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1738;});
_temp1737;})); _temp1736;}); _temp1735;}); break; case 55: _LL1734: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1740=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1740[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1741; _temp1741.tag= Cyc_TypeSpecifier_tok;
_temp1741.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1741;});
_temp1740;}); break; case 56: _LL1739: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1743=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1743[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1744; _temp1744.tag=
Cyc_TypeSpecifier_tok; _temp1744.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1745=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1745[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1746; _temp1746.tag= Cyc_Parse_Long_spec;
_temp1746.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1746;}); _temp1745;})); _temp1744;}); _temp1743;});
break; case 57: _LL1742: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1748=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1748[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1749; _temp1749.tag=
Cyc_TypeSpecifier_tok; _temp1749.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1749;}); _temp1748;}); break; case 58: _LL1747:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1751=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1751[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1752; _temp1752.tag= Cyc_TypeSpecifier_tok;
_temp1752.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1752;});
_temp1751;}); break; case 59: _LL1750: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1754=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1754[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1755; _temp1755.tag=
Cyc_TypeSpecifier_tok; _temp1755.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1756=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1756[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1757; _temp1757.tag=
Cyc_Parse_Signed_spec; _temp1757.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1757;});
_temp1756;})); _temp1755;}); _temp1754;}); break; case 60: _LL1753: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1759=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1759[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1760; _temp1760.tag= Cyc_TypeSpecifier_tok;
_temp1760.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1761=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1761[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1762; _temp1762.tag= Cyc_Parse_Unsigned_spec; _temp1762.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1762;});
_temp1761;})); _temp1760;}); _temp1759;}); break; case 61: _LL1758: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 62: _LL1763:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
63: _LL1764: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 64: _LL1765: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1767=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1767[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1768; _temp1768.tag=
Cyc_TypeSpecifier_tok; _temp1768.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1768;});
_temp1767;}); break; case 65: _LL1766: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1770=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1770[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1771; _temp1771.tag=
Cyc_TypeSpecifier_tok; _temp1771.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1771;}); _temp1770;}); break; case 66: _LL1769: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1773=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1773[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1774; _temp1774.tag=
Cyc_TypeSpecifier_tok; _temp1774.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1775=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1775[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1776; _temp1776.tag= Cyc_Absyn_TypedefType;
_temp1776.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1776.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1776.f3= 0;
_temp1776;}); _temp1775;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1774;});
_temp1773;}); break; case 67: _LL1772: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1778=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1778[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1779; _temp1779.tag=
Cyc_TypeSpecifier_tok; _temp1779.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1780=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1780[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1781; _temp1781.tag= Cyc_Absyn_TupleType;
_temp1781.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); _temp1781;});
_temp1780;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1779;}); _temp1778;}); break; case 68:
_LL1777: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1783=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1783[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1784; _temp1784.tag=
Cyc_TypeSpecifier_tok; _temp1784.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1785=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1785[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1786; _temp1786.tag= Cyc_Absyn_RgnHandleType;
_temp1786.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1786;}); _temp1785;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1784;}); _temp1783;}); break; case 69: _LL1782: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1788=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1788[ 0]=({ struct Cyc_Kind_tok_struct _temp1789; _temp1789.tag= Cyc_Kind_tok;
_temp1789.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1789;});
_temp1788;}); break; case 70: _LL1787: { struct _tagged_arr* _temp1793; void*
_temp1795; struct _tuple1 _temp1791=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1796: _temp1795=
_temp1791.f1; goto _LL1794; _LL1794: _temp1793= _temp1791.f2; goto _LL1792;
_LL1792: if( _temp1795 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1797=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1797[ 0]=({ struct Cyc_Kind_tok_struct
_temp1798; _temp1798.tag= Cyc_Kind_tok; _temp1798.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1793, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1798;}); _temp1797;}); break;} case 71:
_LL1790: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1800=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1800[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1801; _temp1801.tag= Cyc_TypeQual_tok; _temp1801.f1=({ struct Cyc_Absyn_Tqual
_temp1802; _temp1802.q_const= 1; _temp1802.q_volatile= 0; _temp1802.q_restrict=
0; _temp1802;}); _temp1801;}); _temp1800;}); break; case 72: _LL1799: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1804=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1804[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1805; _temp1805.tag= Cyc_TypeQual_tok; _temp1805.f1=({ struct Cyc_Absyn_Tqual
_temp1806; _temp1806.q_const= 0; _temp1806.q_volatile= 1; _temp1806.q_restrict=
0; _temp1806;}); _temp1805;}); _temp1804;}); break; case 73: _LL1803: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1808=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1808[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1809; _temp1809.tag= Cyc_TypeQual_tok; _temp1809.f1=({ struct Cyc_Absyn_Tqual
_temp1810; _temp1810.q_const= 0; _temp1810.q_volatile= 0; _temp1810.q_restrict=
1; _temp1810;}); _temp1809;}); _temp1808;}); break; case 74: _LL1807: yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1812=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1812[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1813; _temp1813.tag= Cyc_TypeSpecifier_tok;
_temp1813.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1814=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1814[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1815; _temp1815.tag= Cyc_Parse_Decl_spec;
_temp1815.f1=({ struct Cyc_Absyn_Decl* _temp1816=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1816->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1817=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1817[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1818; _temp1818.tag= Cyc_Absyn_Enum_d; _temp1818.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1819=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1819->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1819->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1819->fields=({
struct Cyc_Core_Opt* _temp1820=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1820->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1820;});
_temp1819;}); _temp1818;}); _temp1817;})); _temp1816->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1816;}); _temp1815;}); _temp1814;})); _temp1813;}); _temp1812;}); break;
case 75: _LL1811: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1822=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1822[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1823; _temp1823.tag=
Cyc_TypeSpecifier_tok; _temp1823.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1824=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1824[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1825; _temp1825.tag= Cyc_Absyn_EnumType;
_temp1825.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1825.f2= 0; _temp1825;}); _temp1824;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1823;}); _temp1822;}); break; case 76: _LL1821: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1827=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1827[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1828; _temp1828.tag= Cyc_Enumfield_tok;
_temp1828.f1=({ struct Cyc_Absyn_Enumfield* _temp1829=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1829->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1829->tag= 0;
_temp1829->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1829;});
_temp1828;}); _temp1827;}); break; case 77: _LL1826: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1831=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1831[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1832; _temp1832.tag= Cyc_Enumfield_tok;
_temp1832.f1=({ struct Cyc_Absyn_Enumfield* _temp1833=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1833->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1833->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1833->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1833;});
_temp1832;}); _temp1831;}); break; case 78: _LL1830: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1835=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1835[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1836; _temp1836.tag=
Cyc_EnumfieldList_tok; _temp1836.f1=({ struct Cyc_List_List* _temp1837=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1837->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1837->tl= 0; _temp1837;}); _temp1836;}); _temp1835;});
break; case 79: _LL1834: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1839=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1839[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1840; _temp1840.tag=
Cyc_EnumfieldList_tok; _temp1840.f1=({ struct Cyc_List_List* _temp1841=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1841->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1841->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1841;}); _temp1840;});
_temp1839;}); break; case 80: _LL1838: { void* t;{ void* _temp1843= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1845: if(
_temp1843 ==( void*) Cyc_Parse_Struct_su){ goto _LL1846;} else{ goto _LL1847;}
_LL1847: if( _temp1843 ==( void*) Cyc_Parse_Union_su){ goto _LL1848;} else{ goto
_LL1844;} _LL1846: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp1849=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp1849[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp1850; _temp1850.tag= Cyc_Absyn_AnonStructType; _temp1850.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1850;});
_temp1849;}); goto _LL1844; _LL1848: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1851=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1851[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1852; _temp1852.tag= Cyc_Absyn_AnonUnionType; _temp1852.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1852;});
_temp1851;}); goto _LL1844; _LL1844:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1853=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1853[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1854; _temp1854.tag=
Cyc_TypeSpecifier_tok; _temp1854.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1855=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1855[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1856; _temp1856.tag= Cyc_Parse_Type_spec;
_temp1856.f1=( void*) t; _temp1856.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1856;});
_temp1855;})); _temp1854;}); _temp1853;}); break;} case 81: _LL1842: { struct
Cyc_List_List* _temp1858=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1859= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1861: if(
_temp1859 ==( void*) Cyc_Parse_Struct_su){ goto _LL1862;} else{ goto _LL1863;}
_LL1863: if( _temp1859 ==( void*) Cyc_Parse_Union_su){ goto _LL1864;} else{ goto
_LL1860;} _LL1862: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1865=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1865->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1865;}), _temp1858,({ struct Cyc_Core_Opt*
_temp1866=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1866->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1866;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1860; _LL1864: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1867=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1867->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1867;}),
_temp1858,({ struct Cyc_Core_Opt* _temp1868=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1868->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1868;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1860; _LL1860:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1869=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1869[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1870; _temp1870.tag= Cyc_TypeSpecifier_tok;
_temp1870.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1871=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1871[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1872; _temp1872.tag= Cyc_Parse_Decl_spec;
_temp1872.f1= d; _temp1872;}); _temp1871;})); _temp1870;}); _temp1869;}); break;}
case 82: _LL1857: { struct Cyc_List_List* _temp1874=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1875= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1877: if(
_temp1875 ==( void*) Cyc_Parse_Struct_su){ goto _LL1878;} else{ goto _LL1879;}
_LL1879: if( _temp1875 ==( void*) Cyc_Parse_Union_su){ goto _LL1880;} else{ goto
_LL1876;} _LL1878: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1881=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1881->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1881;}), _temp1874,({ struct Cyc_Core_Opt*
_temp1882=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1882->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1882;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1876; _LL1880: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1883=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1883->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1883;}),
_temp1874,({ struct Cyc_Core_Opt* _temp1884=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1884->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1884;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1876; _LL1876:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1885=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1885[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1886; _temp1886.tag= Cyc_TypeSpecifier_tok;
_temp1886.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1887=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1887[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1888; _temp1888.tag= Cyc_Parse_Decl_spec;
_temp1888.f1= d; _temp1888;}); _temp1887;})); _temp1886;}); _temp1885;}); break;}
case 83: _LL1873:{ void* _temp1890= Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL1892: if(
_temp1890 ==( void*) Cyc_Parse_Struct_su){ goto _LL1893;} else{ goto _LL1894;}
_LL1894: if( _temp1890 ==( void*) Cyc_Parse_Union_su){ goto _LL1895;} else{ goto
_LL1891;} _LL1893: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
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
_temp1900;}); goto _LL1891; _LL1891:;} break; case 84: _LL1889:{ void* _temp1905=
Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL1907: if( _temp1905 ==( void*) Cyc_Parse_Struct_su){
goto _LL1908;} else{ goto _LL1909;} _LL1909: if( _temp1905 ==( void*) Cyc_Parse_Union_su){
goto _LL1910;} else{ goto _LL1906;} _LL1908: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1911=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1911[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1912; _temp1912.tag=
Cyc_TypeSpecifier_tok; _temp1912.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1913=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1913[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1914; _temp1914.tag= Cyc_Absyn_StructType;
_temp1914.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1914.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1914.f3= 0;
_temp1914;}); _temp1913;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1912;});
_temp1911;}); goto _LL1906; _LL1910: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1915=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1915[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1916; _temp1916.tag=
Cyc_TypeSpecifier_tok; _temp1916.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1917=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1917[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1918; _temp1918.tag= Cyc_Absyn_UnionType;
_temp1918.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1918.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1918.f3= 0;
_temp1918;}); _temp1917;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1916;});
_temp1915;}); goto _LL1906; _LL1906:;} break; case 85: _LL1904: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp1920=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1920[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1921; _temp1921.tag= Cyc_TypeList_tok; _temp1921.f1= 0; _temp1921;});
_temp1920;}); break; case 86: _LL1919: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1923=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1923[ 0]=({ struct Cyc_TypeList_tok_struct _temp1924; _temp1924.tag= Cyc_TypeList_tok;
_temp1924.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])); _temp1924;}); _temp1923;}); break; case 87: _LL1922:((
struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1; yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp1926=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1926[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1927; _temp1927.tag= Cyc_TypeList_tok; _temp1927.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1927;});
_temp1926;}); break; case 88: _LL1925: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1929=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1929[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1930; _temp1930.tag=
Cyc_StructOrUnion_tok; _temp1930.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp1930;}); _temp1929;}); break; case 89: _LL1928: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1932=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1932[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1933; _temp1933.tag=
Cyc_StructOrUnion_tok; _temp1933.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp1933;}); _temp1932;}); break; case 90: _LL1931: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1935=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1935[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1936; _temp1936.tag= Cyc_StructFieldDeclList_tok; _temp1936.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1936;});
_temp1935;}); break; case 91: _LL1934: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1938=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1938[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1939; _temp1939.tag= Cyc_StructFieldDeclListList_tok; _temp1939.f1=({
struct Cyc_List_List* _temp1940=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1940->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1940->tl= 0;
_temp1940;}); _temp1939;}); _temp1938;}); break; case 92: _LL1937: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1942=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1942[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1943; _temp1943.tag= Cyc_StructFieldDeclListList_tok;
_temp1943.f1=({ struct Cyc_List_List* _temp1944=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1944->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1944->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1944;}); _temp1943;}); _temp1942;}); break;
case 93: _LL1941: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1946=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1946[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1947; _temp1947.tag=
Cyc_InitDeclList_tok; _temp1947.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1947;});
_temp1946;}); break; case 94: _LL1945: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1949=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1949[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1950; _temp1950.tag=
Cyc_InitDeclList_tok; _temp1950.f1=({ struct Cyc_List_List* _temp1951=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1951->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1951->tl= 0; _temp1951;}); _temp1950;}); _temp1949;});
break; case 95: _LL1948: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1953=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1953[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1954; _temp1954.tag=
Cyc_InitDeclList_tok; _temp1954.f1=({ struct Cyc_List_List* _temp1955=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1955->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1955->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1955;});
_temp1954;}); _temp1953;}); break; case 96: _LL1952: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1957=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1957[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1958; _temp1958.tag= Cyc_InitDecl_tok;
_temp1958.f1=({ struct _tuple15* _temp1959=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp1959->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1959->f2= 0;
_temp1959;}); _temp1958;}); _temp1957;}); break; case 97: _LL1956: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1961=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1961[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1962; _temp1962.tag= Cyc_InitDecl_tok; _temp1962.f1=({ struct _tuple15*
_temp1963=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1963->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1963->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1963;});
_temp1962;}); _temp1961;}); break; case 98: _LL1960: { struct Cyc_Absyn_Tqual tq=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])).f1; struct Cyc_List_List* _temp1965=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3; void*
_temp1966= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
struct Cyc_List_List* _temp1969; struct Cyc_List_List* _temp1971; struct _tuple0
_temp1967=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _LL1972:
_temp1971= _temp1967.f1; goto _LL1970; _LL1970: _temp1969= _temp1967.f2; goto
_LL1968; _LL1968: { struct Cyc_List_List* _temp1973=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, _temp1966, _temp1971, _temp1965), _temp1969); yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1974=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1974[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1975; _temp1975.tag= Cyc_StructFieldDeclList_tok; _temp1975.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),
_temp1973); _temp1975;}); _temp1974;}); break;}} case 99: _LL1964: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp1977=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1977[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1978; _temp1978.tag= Cyc_QualSpecList_tok;
_temp1978.f1=({ struct _tuple18* _temp1979=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1979->f1= Cyc_Absyn_empty_tqual(); _temp1979->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1980=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1980->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1980->tl=
0; _temp1980;}); _temp1979->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1979;}); _temp1978;});
_temp1977;}); break; case 100: _LL1976: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1982=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1982[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1983; _temp1983.tag=
Cyc_QualSpecList_tok; _temp1983.f1=({ struct _tuple18* _temp1984=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1984->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1984->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1985=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1985->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1985->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1985;}); _temp1984->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1984;});
_temp1983;}); _temp1982;}); break; case 101: _LL1981: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1987=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1987[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1988; _temp1988.tag= Cyc_QualSpecList_tok;
_temp1988.f1=({ struct _tuple18* _temp1989=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1989->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1989->f2= 0;
_temp1989->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1989;}); _temp1988;}); _temp1987;}); break; case
102: _LL1986: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1991=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1991[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1992; _temp1992.tag=
Cyc_QualSpecList_tok; _temp1992.f1=({ struct _tuple18* _temp1993=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1993->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1993->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1993->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1993;});
_temp1992;}); _temp1991;}); break; case 103: _LL1990: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp1995=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1995[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1996; _temp1996.tag= Cyc_DeclaratorExpoptList_tok;
_temp1996.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1996;}); _temp1995;}); break; case 104: _LL1994: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp1998=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1998[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1999; _temp1999.tag= Cyc_DeclaratorExpoptList_tok;
_temp1999.f1=({ struct Cyc_List_List* _temp2000=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2000->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2000->tl= 0;
_temp2000;}); _temp1999;}); _temp1998;}); break; case 105: _LL1997: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp2002=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2002[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2003; _temp2003.tag= Cyc_DeclaratorExpoptList_tok;
_temp2003.f1=({ struct Cyc_List_List* _temp2004=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2004->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2004->tl=
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2004;}); _temp2003;}); _temp2002;}); break; case 106:
_LL2001: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2006=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2006[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2007; _temp2007.tag=
Cyc_DeclaratorExpopt_tok; _temp2007.f1=({ struct _tuple13* _temp2008=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2008->f1= Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2008->f2= 0;
_temp2008;}); _temp2007;}); _temp2006;}); break; case 107: _LL2005: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp2010=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2010[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2011; _temp2011.tag= Cyc_DeclaratorExpopt_tok;
_temp2011.f1=({ struct _tuple13* _temp2012=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2012->f1=({ struct Cyc_Parse_Declarator*
_temp2014=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp2014->id=({ struct _tuple1* _temp2015=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2015->f1= Cyc_Absyn_rel_ns_null; _temp2015->f2=
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp2015;}); _temp2014->tms= 0; _temp2014;});
_temp2012->f2=({ struct Cyc_Core_Opt* _temp2013=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2013->v=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2013;});
_temp2012;}); _temp2011;}); _temp2010;}); break; case 108: _LL2009: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp2017=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2017[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2018; _temp2018.tag= Cyc_DeclaratorExpopt_tok;
_temp2018.f1=({ struct _tuple13* _temp2019=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2019->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2019->f2=({
struct Cyc_Core_Opt* _temp2020=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2020->v=( void*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2020;}); _temp2019;});
_temp2018;}); _temp2017;}); break; case 109: _LL2016: { struct Cyc_List_List*
_temp2022=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2023=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2023[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2024; _temp2024.tag= Cyc_TypeSpecifier_tok;
_temp2024.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2025=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2025[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2026; _temp2026.tag= Cyc_Parse_Decl_spec;
_temp2026.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _temp2022,({
struct Cyc_Core_Opt* _temp2027=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2027->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2027;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2026;}); _temp2025;})); _temp2024;}); _temp2023;}); break;} case 110:
_LL2021: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2029=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2029[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2030; _temp2030.tag=
Cyc_TypeSpecifier_tok; _temp2030.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2031=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2031[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2032; _temp2032.tag= Cyc_Absyn_TunionType;
_temp2032.f1=({ struct Cyc_Absyn_TunionInfo _temp2033; _temp2033.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2034=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2034[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2035; _temp2035.tag= Cyc_Absyn_UnknownTunion;
_temp2035.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2036; _temp2036.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp2036.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2036;});
_temp2035;}); _temp2034;})); _temp2033.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2033.rgn=( void*)((
void*) Cyc_Absyn_HeapRgn); _temp2033;}); _temp2032;}); _temp2031;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2030;}); _temp2029;}); break; case 111: _LL2028: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2038=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2038[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2039; _temp2039.tag= Cyc_TypeSpecifier_tok;
_temp2039.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2040=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2040[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2041; _temp2041.tag=
Cyc_Absyn_TunionType; _temp2041.f1=({ struct Cyc_Absyn_TunionInfo _temp2042;
_temp2042.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp2043=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp2043[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp2044; _temp2044.tag= Cyc_Absyn_UnknownTunion; _temp2044.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp2045; _temp2045.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2045.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2045;}); _temp2044;}); _temp2043;})); _temp2042.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2042.rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2042;}); _temp2041;}); _temp2040;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2039;}); _temp2038;}); break; case 112: _LL2037: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2047=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2047[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2048; _temp2048.tag= Cyc_TypeSpecifier_tok;
_temp2048.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2049=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2049[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2050; _temp2050.tag= Cyc_Absyn_TunionFieldType; _temp2050.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2051; _temp2051.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2052=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2052[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2053; _temp2053.tag= Cyc_Absyn_UnknownTunionfield; _temp2053.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2054; _temp2054.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2054.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp2054.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2054;});
_temp2053;}); _temp2052;})); _temp2051.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2051;}); _temp2050;});
_temp2049;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2048;}); _temp2047;}); break; case 113:
_LL2046: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2056=( struct Cyc_Bool_tok_struct*)
GC_malloc( sizeof( struct Cyc_Bool_tok_struct)); _temp2056[ 0]=({ struct Cyc_Bool_tok_struct
_temp2057; _temp2057.tag= Cyc_Bool_tok; _temp2057.f1= 0; _temp2057;}); _temp2056;});
break; case 114: _LL2055: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2059=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2059[ 0]=({ struct Cyc_Bool_tok_struct _temp2060; _temp2060.tag= Cyc_Bool_tok;
_temp2060.f1= 1; _temp2060;}); _temp2059;}); break; case 115: _LL2058: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2062=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2062[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2063; _temp2063.tag= Cyc_TunionFieldList_tok;
_temp2063.f1=({ struct Cyc_List_List* _temp2064=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2064->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2064->tl= 0;
_temp2064;}); _temp2063;}); _temp2062;}); break; case 116: _LL2061: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2066=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2066[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2067; _temp2067.tag= Cyc_TunionFieldList_tok;
_temp2067.f1=({ struct Cyc_List_List* _temp2068=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2068->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2068->tl=
0; _temp2068;}); _temp2067;}); _temp2066;}); break; case 117: _LL2065: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2070=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2070[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2071; _temp2071.tag= Cyc_TunionFieldList_tok;
_temp2071.f1=({ struct Cyc_List_List* _temp2072=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2072->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2072->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2072;}); _temp2071;}); _temp2070;}); break; case 118:
_LL2069: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2074=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2074[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2075; _temp2075.tag=
Cyc_TunionFieldList_tok; _temp2075.f1=({ struct Cyc_List_List* _temp2076=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2076->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2076->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2076;}); _temp2075;});
_temp2074;}); break; case 119: _LL2073: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2078=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2078[ 0]=({ struct Cyc_Scope_tok_struct _temp2079; _temp2079.tag= Cyc_Scope_tok;
_temp2079.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2079;}); _temp2078;});
break; case 120: _LL2077: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2081=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2081[ 0]=({ struct Cyc_Scope_tok_struct _temp2082; _temp2082.tag= Cyc_Scope_tok;
_temp2082.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2082;}); _temp2081;});
break; case 121: _LL2080: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2084=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2084[ 0]=({ struct Cyc_Scope_tok_struct _temp2085; _temp2085.tag= Cyc_Scope_tok;
_temp2085.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2085;}); _temp2084;});
break; case 122: _LL2083: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2087=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2087[ 0]=({ struct Cyc_TunionField_tok_struct _temp2088; _temp2088.tag= Cyc_TunionField_tok;
_temp2088.f1=({ struct Cyc_Absyn_Tunionfield* _temp2089=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2089->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2089->tvs= 0;
_temp2089->typs= 0; _temp2089->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2089->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2089;}); _temp2088;}); _temp2087;}); break; case 123:
_LL2086: { struct Cyc_List_List* _temp2091=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); struct Cyc_List_List*
_temp2092=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp2093=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2093[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2094; _temp2094.tag= Cyc_TunionField_tok;
_temp2094.f1=({ struct Cyc_Absyn_Tunionfield* _temp2095=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2095->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2095->tvs=
_temp2092; _temp2095->typs= _temp2091; _temp2095->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2095->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp2095;}); _temp2094;}); _temp2093;}); break;}
case 124: _LL2090: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL2096: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2098=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2098[ 0]=({ struct Cyc_Declarator_tok_struct _temp2099; _temp2099.tag= Cyc_Declarator_tok;
_temp2099.f1=({ struct Cyc_Parse_Declarator* _temp2100=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2100->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2100->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]),( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2100;});
_temp2099;}); _temp2098;}); break; case 126: _LL2097: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2102=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2102[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2103; _temp2103.tag= Cyc_Declarator_tok;
_temp2103.f1=({ struct Cyc_Parse_Declarator* _temp2104=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2104->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2104->tms= 0;
_temp2104;}); _temp2103;}); _temp2102;}); break; case 127: _LL2101: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 128:
_LL2105: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2107=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2107[ 0]=({ struct Cyc_Declarator_tok_struct _temp2108; _temp2108.tag= Cyc_Declarator_tok;
_temp2108.f1=({ struct Cyc_Parse_Declarator* _temp2109=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2109->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->id;
_temp2109->tms=({ struct Cyc_List_List* _temp2110=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2110->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2110->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]))->tms; _temp2110;}); _temp2109;}); _temp2108;});
_temp2107;}); break; case 129: _LL2106: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2112=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2112[ 0]=({ struct Cyc_Declarator_tok_struct _temp2113; _temp2113.tag= Cyc_Declarator_tok;
_temp2113.f1=({ struct Cyc_Parse_Declarator* _temp2114=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2114->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2114->tms=({ struct Cyc_List_List* _temp2115=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2115->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2116=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2116[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2117; _temp2117.tag= Cyc_Absyn_ConstArray_mod;
_temp2117.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2117;}); _temp2116;})); _temp2115->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2115;}); _temp2114;}); _temp2113;}); _temp2112;}); break; case 130: _LL2111: {
struct _tuple17 _temp2121; struct Cyc_List_List* _temp2122; struct Cyc_Core_Opt*
_temp2124; struct Cyc_Absyn_VarargInfo* _temp2126; int _temp2128; struct Cyc_List_List*
_temp2130; struct _tuple17* _temp2119= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2121=*
_temp2119; _LL2131: _temp2130= _temp2121.f1; goto _LL2129; _LL2129: _temp2128=
_temp2121.f2; goto _LL2127; _LL2127: _temp2126= _temp2121.f3; goto _LL2125;
_LL2125: _temp2124= _temp2121.f4; goto _LL2123; _LL2123: _temp2122= _temp2121.f5;
goto _LL2120; _LL2120: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2132=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2132[ 0]=({ struct Cyc_Declarator_tok_struct _temp2133; _temp2133.tag= Cyc_Declarator_tok;
_temp2133.f1=({ struct Cyc_Parse_Declarator* _temp2134=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2134->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2134->tms=({ struct Cyc_List_List* _temp2135=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2135->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2136=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2136[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2137; _temp2137.tag= Cyc_Absyn_Function_mod;
_temp2137.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2138=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2138[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2139; _temp2139.tag= Cyc_Absyn_WithTypes;
_temp2139.f1= _temp2130; _temp2139.f2= _temp2128; _temp2139.f3= _temp2126;
_temp2139.f4= _temp2124; _temp2139.f5= _temp2122; _temp2139;}); _temp2138;}));
_temp2137;}); _temp2136;})); _temp2135->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2135;});
_temp2134;}); _temp2133;}); _temp2132;}); break;} case 131: _LL2118: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2141=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2141[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2142; _temp2142.tag= Cyc_Declarator_tok;
_temp2142.f1=({ struct Cyc_Parse_Declarator* _temp2143=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2143->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->id;
_temp2143->tms=({ struct Cyc_List_List* _temp2144=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2144->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2145=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2145[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2146; _temp2146.tag= Cyc_Absyn_Function_mod;
_temp2146.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2147=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2147[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2148; _temp2148.tag= Cyc_Absyn_WithTypes;
_temp2148.f1= 0; _temp2148.f2= 0; _temp2148.f3= 0; _temp2148.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2148.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2148;}); _temp2147;})); _temp2146;}); _temp2145;}));
_temp2144->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]))->tms; _temp2144;}); _temp2143;}); _temp2142;});
_temp2141;}); break; case 132: _LL2140: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2150=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2150[ 0]=({ struct Cyc_Declarator_tok_struct _temp2151; _temp2151.tag= Cyc_Declarator_tok;
_temp2151.f1=({ struct Cyc_Parse_Declarator* _temp2152=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2152->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2152->tms=({ struct Cyc_List_List* _temp2153=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2153->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2154=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2154[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2155; _temp2155.tag= Cyc_Absyn_Function_mod;
_temp2155.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2156=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2156[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2157; _temp2157.tag= Cyc_Absyn_NoTypes;
_temp2157.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2157.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2157;});
_temp2156;})); _temp2155;}); _temp2154;})); _temp2153->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2153;}); _temp2152;}); _temp2151;}); _temp2150;}); break; case 133: _LL2149: {
struct Cyc_List_List* _temp2159=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2160=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2160[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2161; _temp2161.tag= Cyc_Declarator_tok;
_temp2161.f1=({ struct Cyc_Parse_Declarator* _temp2162=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2162->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2162->tms=({ struct Cyc_List_List* _temp2163=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2163->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2164=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2164[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2165; _temp2165.tag= Cyc_Absyn_TypeParams_mod;
_temp2165.f1= _temp2159; _temp2165.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2165.f3=
0; _temp2165;}); _temp2164;})); _temp2163->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2163;});
_temp2162;}); _temp2161;}); _temp2160;}); break;} case 134: _LL2158:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2167=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2168=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2168[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2169; _temp2169.tag= Cyc_Declarator_tok;
_temp2169.f1=({ struct Cyc_Parse_Declarator* _temp2170=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2170->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2170->tms=({ struct Cyc_List_List* _temp2171=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2171->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2172=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2172[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2173; _temp2173.tag= Cyc_Absyn_TypeParams_mod;
_temp2173.f1= _temp2167; _temp2173.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2173.f3=
0; _temp2173;}); _temp2172;})); _temp2171->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2171;});
_temp2170;}); _temp2169;}); _temp2168;}); break;} case 135: _LL2166: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2175=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2175[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2176; _temp2176.tag= Cyc_Declarator_tok;
_temp2176.f1=({ struct Cyc_Parse_Declarator* _temp2177=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2177->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->id;
_temp2177->tms=({ struct Cyc_List_List* _temp2178=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2178->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2179=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2179[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2180; _temp2180.tag= Cyc_Absyn_Attributes_mod;
_temp2180.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2180.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2180;}); _temp2179;}));
_temp2178->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]))->tms; _temp2178;}); _temp2177;}); _temp2176;});
_temp2175;}); break; case 136: _LL2174: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2182=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2182[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2183; _temp2183.tag= Cyc_Declarator_tok;
_temp2183.f1=({ struct Cyc_Parse_Declarator* _temp2184=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2184->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2184->tms= 0;
_temp2184;}); _temp2183;}); _temp2182;}); break; case 137: _LL2181: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2186=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2186[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2187; _temp2187.tag= Cyc_Declarator_tok;
_temp2187.f1=({ struct Cyc_Parse_Declarator* _temp2188=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2188->id= Cyc_Absyn_exn_name;
_temp2188->tms= 0; _temp2188;}); _temp2187;}); _temp2186;}); break; case 138:
_LL2185: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2190=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2190[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2191; _temp2191.tag=
Cyc_TypeModifierList_tok; _temp2191.f1=({ struct Cyc_List_List* _temp2192=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2192->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2193=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2193[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2194; _temp2194.tag= Cyc_Absyn_Pointer_mod;
_temp2194.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2194.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2194.f3= Cyc_Absyn_empty_tqual(); _temp2194;}); _temp2193;}));
_temp2192->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2192;});
_temp2191;}); _temp2190;}); break; case 139: _LL2189: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2196=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2196[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2197; _temp2197.tag= Cyc_TypeModifierList_tok;
_temp2197.f1=({ struct Cyc_List_List* _temp2198=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2198->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2199=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2199[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2200; _temp2200.tag= Cyc_Absyn_Pointer_mod;
_temp2200.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2200.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2200.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2200;}); _temp2199;})); _temp2198->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0); _temp2198;});
_temp2197;}); _temp2196;}); break; case 140: _LL2195: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2202=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2202[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2203; _temp2203.tag= Cyc_TypeModifierList_tok;
_temp2203.f1=({ struct Cyc_List_List* _temp2204=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2204->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2205=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2205[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2206; _temp2206.tag= Cyc_Absyn_Pointer_mod;
_temp2206.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2206.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2206.f3= Cyc_Absyn_empty_tqual(); _temp2206;}); _temp2205;}));
_temp2204->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2204;});
_temp2203;}); _temp2202;}); break; case 141: _LL2201: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2208=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2208[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2209; _temp2209.tag= Cyc_TypeModifierList_tok;
_temp2209.f1=({ struct Cyc_List_List* _temp2210=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2210->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2211=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2211[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2212; _temp2212.tag= Cyc_Absyn_Pointer_mod;
_temp2212.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2212.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2212.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2212;}); _temp2211;})); _temp2210->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2210;});
_temp2209;}); _temp2208;}); break; case 142: _LL2207: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2214=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2214[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2215; _temp2215.tag= Cyc_Pointer_Sort_tok;
_temp2215.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2216=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2216[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2217; _temp2217.tag=
Cyc_Absyn_Nullable_ps; _temp2217.f1= Cyc_Absyn_exp_unsigned_one; _temp2217;});
_temp2216;})); _temp2215;}); _temp2214;}); break; case 143: _LL2213: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2219=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2219[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2220; _temp2220.tag= Cyc_Pointer_Sort_tok;
_temp2220.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2221=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2221[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2222; _temp2222.tag= Cyc_Absyn_NonNullable_ps; _temp2222.f1= Cyc_Absyn_exp_unsigned_one;
_temp2222;}); _temp2221;})); _temp2220;}); _temp2219;}); break; case 144:
_LL2218: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2224=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2224[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2225; _temp2225.tag=
Cyc_Pointer_Sort_tok; _temp2225.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2226=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2226[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2227; _temp2227.tag=
Cyc_Absyn_Nullable_ps; _temp2227.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2227;});
_temp2226;})); _temp2225;}); _temp2224;}); break; case 145: _LL2223: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2229=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2229[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2230; _temp2230.tag= Cyc_Pointer_Sort_tok;
_temp2230.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2231=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2231[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2232; _temp2232.tag= Cyc_Absyn_NonNullable_ps; _temp2232.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2232;});
_temp2231;})); _temp2230;}); _temp2229;}); break; case 146: _LL2228: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2234=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2234[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2235; _temp2235.tag= Cyc_Pointer_Sort_tok;
_temp2235.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2235;});
_temp2234;}); break; case 147: _LL2233: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2237=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2237[ 0]=({ struct Cyc_Rgn_tok_struct _temp2238; _temp2238.tag= Cyc_Rgn_tok;
_temp2238.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2238;}); _temp2237;});
break; case 148: _LL2236: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2239: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2241=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2241[ 0]=({ struct Cyc_Rgn_tok_struct _temp2242; _temp2242.tag= Cyc_Rgn_tok;
_temp2242.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2242;});
_temp2241;}); break; case 150: _LL2240: if( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind\n", sizeof( unsigned char), 23u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2244=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2244[ 0]=({ struct Cyc_Rgn_tok_struct _temp2245; _temp2245.tag= Cyc_Rgn_tok;
_temp2245.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2245;});
_temp2244;}); break; case 151: _LL2243: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2247=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2247[ 0]=({ struct Cyc_Rgn_tok_struct _temp2248; _temp2248.tag= Cyc_Rgn_tok;
_temp2248.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2249=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2249->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2249;}), 0); _temp2248;}); _temp2247;});
break; case 152: _LL2246: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 153: _LL2250: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2252=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2252[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2253; _temp2253.tag= Cyc_TypeQual_tok;
_temp2253.f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeQual_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2253;});
_temp2252;}); break; case 154: _LL2251: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2255=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2255[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2256; _temp2256.tag= Cyc_ParamDeclListBool_tok; _temp2256.f1=({ struct
_tuple17* _temp2257=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2257->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])); _temp2257->f2= 0; _temp2257->f3= 0; _temp2257->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2257->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2257;}); _temp2256;}); _temp2255;}); break; case 155:
_LL2254: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2259=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp2259[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp2260; _temp2260.tag=
Cyc_ParamDeclListBool_tok; _temp2260.f1=({ struct _tuple17* _temp2261=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2261->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)])); _temp2261->f2= 1;
_temp2261->f3= 0; _temp2261->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2261->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2261;});
_temp2260;}); _temp2259;}); break; case 156: _LL2258: { void* _temp2265; struct
Cyc_Absyn_Tqual _temp2267; struct Cyc_Core_Opt* _temp2269; struct _tuple2
_temp2263=* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL2270: _temp2269= _temp2263.f1; goto _LL2268; _LL2268:
_temp2267= _temp2263.f2; goto _LL2266; _LL2266: _temp2265= _temp2263.f3; goto
_LL2264; _LL2264: { struct Cyc_Absyn_VarargInfo* _temp2271=({ struct Cyc_Absyn_VarargInfo*
_temp2275=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2275->name= _temp2269; _temp2275->tq= _temp2267; _temp2275->type=( void*)
_temp2265; _temp2275->rgn=( void*) Cyc_yyget_Rgn_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2275->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2275;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2272=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2272[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2273; _temp2273.tag= Cyc_ParamDeclListBool_tok; _temp2273.f1=({ struct
_tuple17* _temp2274=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2274->f1= 0; _temp2274->f2= 0; _temp2274->f3= _temp2271; _temp2274->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2274->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2274;}); _temp2273;}); _temp2272;}); break;}} case 157:
_LL2262: { void* _temp2279; struct Cyc_Absyn_Tqual _temp2281; struct Cyc_Core_Opt*
_temp2283; struct _tuple2 _temp2277=* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL2284: _temp2283=
_temp2277.f1; goto _LL2282; _LL2282: _temp2281= _temp2277.f2; goto _LL2280;
_LL2280: _temp2279= _temp2277.f3; goto _LL2278; _LL2278: { struct Cyc_Absyn_VarargInfo*
_temp2285=({ struct Cyc_Absyn_VarargInfo* _temp2289=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2289->name= _temp2283;
_temp2289->tq= _temp2281; _temp2289->type=( void*) _temp2279; _temp2289->rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2289->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2289;}); yyval=(
void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2286=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp2286[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp2287; _temp2287.tag= Cyc_ParamDeclListBool_tok;
_temp2287.f1=({ struct _tuple17* _temp2288=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2288->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)])); _temp2288->f2= 0;
_temp2288->f3= _temp2285; _temp2288->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2288->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2288;});
_temp2287;}); _temp2286;}); break;}} case 158: _LL2276: yyval=( void*)({ struct
Cyc_TypeOpt_tok_struct* _temp2291=( struct Cyc_TypeOpt_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2291[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2292; _temp2292.tag= Cyc_TypeOpt_tok; _temp2292.f1= 0; _temp2292;});
_temp2291;}); break; case 159: _LL2290: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2294=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2294[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2295; _temp2295.tag= Cyc_TypeOpt_tok;
_temp2295.f1=({ struct Cyc_Core_Opt* _temp2296=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2296->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2297=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2297[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2298; _temp2298.tag= Cyc_Absyn_JoinEff;
_temp2298.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2298;}); _temp2297;})); _temp2296;}); _temp2295;});
_temp2294;}); break; case 160: _LL2293: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2300=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2300[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2301; _temp2301.tag= Cyc_Rgnorder_tok;
_temp2301.f1= 0; _temp2301;}); _temp2300;}); break; case 161: _LL2299: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 162:
_LL2302: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2304=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2304[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2305; _temp2305.tag= Cyc_Rgnorder_tok; _temp2305.f1=({ struct Cyc_List_List*
_temp2306=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2306->hd=( void*)({ struct _tuple19* _temp2307=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2307->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2307->f2= Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp2307;}); _temp2306->tl= 0; _temp2306;}); _temp2305;}); _temp2304;}); break;
case 163: _LL2303: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2309=(
struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2309[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2310; _temp2310.tag= Cyc_Rgnorder_tok;
_temp2310.f1=({ struct Cyc_List_List* _temp2311=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2311->hd=( void*)({ struct
_tuple19* _temp2312=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp2312->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2312->f2= Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)
Cyc_Absyn_RgnKind)); _temp2312;}); _temp2311->tl= Cyc_yyget_Rgnorder_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2311;}); _temp2310;});
_temp2309;}); break; case 164: _LL2308: yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2314=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2314[ 0]=({ struct Cyc_Bool_tok_struct _temp2315; _temp2315.tag= Cyc_Bool_tok;
_temp2315.f1= 0; _temp2315;}); _temp2314;}); break; case 165: _LL2313: { struct
_tagged_arr _temp2317= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_String_zstrcmp(
_temp2317, _tag_arr("inject", sizeof( unsigned char), 7u)) != 0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2318=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2318[ 0]=({ struct Cyc_Bool_tok_struct _temp2319; _temp2319.tag= Cyc_Bool_tok;
_temp2319.f1= 1; _temp2319;}); _temp2318;}); break;} case 166: _LL2316: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 167:
_LL2320: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2322=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2322[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2323; _temp2323.tag= Cyc_TypeList_tok; _temp2323.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2323;});
_temp2322;}); break; case 168: _LL2321: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2325=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2325[ 0]=({ struct Cyc_TypeList_tok_struct _temp2326; _temp2326.tag= Cyc_TypeList_tok;
_temp2326.f1= 0; _temp2326;}); _temp2325;}); break; case 169: _LL2324: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case
170: _LL2327: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2329=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2329[ 0]=({ struct Cyc_TypeList_tok_struct _temp2330; _temp2330.tag= Cyc_TypeList_tok;
_temp2330.f1=({ struct Cyc_List_List* _temp2331=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2331->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp2331->tl= 0; _temp2331;}); _temp2330;}); _temp2329;}); break; case 171:
_LL2328: if( Cyc_yyget_Kind_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]) !=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err( _tag_arr("expecing effect kind (E)",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2333=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2333[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2334; _temp2334.tag= Cyc_TypeList_tok; _temp2334.f1=({ struct Cyc_List_List*
_temp2335=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2335->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp2335->tl= 0;
_temp2335;}); _temp2334;}); _temp2333;}); break; case 172: _LL2332: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2337=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2337[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2338; _temp2338.tag= Cyc_TypeList_tok; _temp2338.f1=({ struct Cyc_List_List*
_temp2339=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2339->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2340=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2340[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2341; _temp2341.tag= Cyc_Absyn_AccessEff;
_temp2341.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2341;});
_temp2340;})); _temp2339->tl= 0; _temp2339;}); _temp2338;}); _temp2337;});
break; case 173: _LL2336: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2343=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2343[ 0]=({ struct Cyc_TypeList_tok_struct _temp2344; _temp2344.tag= Cyc_TypeList_tok;
_temp2344.f1=({ struct Cyc_List_List* _temp2345=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2345->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2346=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2346[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2347; _temp2347.tag= Cyc_Absyn_AccessEff;
_temp2347.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2347;});
_temp2346;})); _temp2345->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2345;}); _temp2344;});
_temp2343;}); break; case 174: _LL2342: if( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2349=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2349[ 0]=({ struct Cyc_TypeList_tok_struct _temp2350; _temp2350.tag= Cyc_TypeList_tok;
_temp2350.f1=({ struct Cyc_List_List* _temp2351=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2351->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2352=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2352[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2353; _temp2353.tag= Cyc_Absyn_AccessEff;
_temp2353.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2353;});
_temp2352;})); _temp2351->tl= 0; _temp2351;}); _temp2350;}); _temp2349;});
break; case 175: _LL2348: if( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2355=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2355[ 0]=({ struct Cyc_TypeList_tok_struct _temp2356; _temp2356.tag= Cyc_TypeList_tok;
_temp2356.f1=({ struct Cyc_List_List* _temp2357=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2357->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2358=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2358[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2359; _temp2359.tag= Cyc_Absyn_AccessEff;
_temp2359.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2359;});
_temp2358;})); _temp2357->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2357;}); _temp2356;});
_temp2355;}); break; case 176: _LL2354: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct*
_temp2361=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp2361[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp2362; _temp2362.tag=
Cyc_ParamDeclList_tok; _temp2362.f1=({ struct Cyc_List_List* _temp2363=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2363->hd=( void*)
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2363->tl= 0; _temp2363;}); _temp2362;}); _temp2361;});
break; case 177: _LL2360: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct*
_temp2365=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp2365[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp2366; _temp2366.tag=
Cyc_ParamDeclList_tok; _temp2366.f1=({ struct Cyc_List_List* _temp2367=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2367->hd=( void*)
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2367->tl= Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2367;});
_temp2366;}); _temp2365;}); break; case 178: _LL2364: { void* _temp2369= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2370=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2371=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2372=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2373= Cyc_Parse_apply_tms( _temp2371, _temp2369, _temp2370,
_temp2372); if( _temp2373.f3 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
_tuple1* _temp2374=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id;{ void* _temp2375=(*
_temp2374).f1; struct Cyc_List_List* _temp2385; struct Cyc_List_List* _temp2387;
_LL2377: if( _temp2375 ==( void*) Cyc_Absyn_Loc_n){ goto _LL2378;} else{ goto
_LL2379;} _LL2379: if(( unsigned int) _temp2375 > 1u?*(( int*) _temp2375) == Cyc_Absyn_Rel_n:
0){ _LL2386: _temp2385=(( struct Cyc_Absyn_Rel_n_struct*) _temp2375)->f1; if(
_temp2385 == 0){ goto _LL2380;} else{ goto _LL2381;}} else{ goto _LL2381;}
_LL2381: if(( unsigned int) _temp2375 > 1u?*(( int*) _temp2375) == Cyc_Absyn_Abs_n:
0){ _LL2388: _temp2387=(( struct Cyc_Absyn_Abs_n_struct*) _temp2375)->f1; if(
_temp2387 == 0){ goto _LL2382;} else{ goto _LL2383;}} else{ goto _LL2383;}
_LL2383: goto _LL2384; _LL2378: goto _LL2376; _LL2380: goto _LL2376; _LL2382:
goto _LL2376; _LL2384: Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a module name",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); goto
_LL2376; _LL2376:;}{ struct Cyc_Core_Opt* _temp2389=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp2393=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2393->v=( void*)(* _temp2374).f2; _temp2393;}); if(
_temp2373.f4 != 0){ Cyc_Parse_warn( _tag_arr("extra attributes on parameter, ignoring",
sizeof( unsigned char), 40u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2373.f3 != 0){ Cyc_Parse_warn( _tag_arr("extra type variables on parameter, ignoring",
sizeof( unsigned char), 44u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2390=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2390[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2391; _temp2391.tag= Cyc_ParamDecl_tok; _temp2391.f1=({
struct _tuple2* _temp2392=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2392->f1= _temp2389; _temp2392->f2= _temp2373.f1; _temp2392->f3= _temp2373.f2;
_temp2392;}); _temp2391;}); _temp2390;}); break;}}} case 179: _LL2368: { void*
_temp2395= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2396=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; struct Cyc_Absyn_Tqual _temp2397=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; if( _temp2396
!= 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring", sizeof(
unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2398=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2398[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2399; _temp2399.tag= Cyc_ParamDecl_tok; _temp2399.f1=({
struct _tuple2* _temp2400=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2400->f1= 0; _temp2400->f2= _temp2397; _temp2400->f3= _temp2395; _temp2400;});
_temp2399;}); _temp2398;}); break;} case 180: _LL2394: { void* _temp2402= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2403=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2404=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2405=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2406= Cyc_Parse_apply_tms( _temp2404, _temp2402, _temp2403,
_temp2405); if( _temp2406.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2406.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2407=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2407[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2408; _temp2408.tag= Cyc_ParamDecl_tok; _temp2408.f1=({
struct _tuple2* _temp2409=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2409->f1= 0; _temp2409->f2= _temp2406.f1; _temp2409->f3= _temp2406.f2;
_temp2409;}); _temp2408;}); _temp2407;}); break;} case 181: _LL2401: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2411=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2411[ 0]=({ struct Cyc_IdList_tok_struct
_temp2412; _temp2412.tag= Cyc_IdList_tok; _temp2412.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2412;});
_temp2411;}); break; case 182: _LL2410: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2414=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2414[ 0]=({ struct Cyc_IdList_tok_struct _temp2415; _temp2415.tag= Cyc_IdList_tok;
_temp2415.f1=({ struct Cyc_List_List* _temp2416=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2416->hd=( void*)({ struct
_tagged_arr* _temp2417=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2417[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2417;}); _temp2416->tl=
0; _temp2416;}); _temp2415;}); _temp2414;}); break; case 183: _LL2413: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2419=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2419[ 0]=({ struct Cyc_IdList_tok_struct
_temp2420; _temp2420.tag= Cyc_IdList_tok; _temp2420.f1=({ struct Cyc_List_List*
_temp2421=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2421->hd=( void*)({ struct _tagged_arr* _temp2422=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2422[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2422;});
_temp2421->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2421;}); _temp2420;}); _temp2419;}); break; case 184:
_LL2418: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 185: _LL2423: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 186: _LL2424: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2426=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2426[ 0]=({ struct Cyc_Exp_tok_struct _temp2427; _temp2427.tag= Cyc_Exp_tok;
_temp2427.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2428=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2428[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2429; _temp2429.tag= Cyc_Absyn_UnresolvedMem_e; _temp2429.f1= 0; _temp2429.f2=
0; _temp2429;}); _temp2428;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2427;});
_temp2426;}); break; case 187: _LL2425: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2431=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2431[ 0]=({ struct Cyc_Exp_tok_struct _temp2432; _temp2432.tag= Cyc_Exp_tok;
_temp2432.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2433=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2433[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2434; _temp2434.tag= Cyc_Absyn_UnresolvedMem_e; _temp2434.f1= 0; _temp2434.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2434;});
_temp2433;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2432;}); _temp2431;}); break; case 188:
_LL2430: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2436=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2436[ 0]=({ struct Cyc_Exp_tok_struct
_temp2437; _temp2437.tag= Cyc_Exp_tok; _temp2437.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2438=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2438[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2439; _temp2439.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2439.f1= 0; _temp2439.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp2439;});
_temp2438;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2437;}); _temp2436;}); break; case 189:
_LL2435: { struct Cyc_Absyn_Vardecl* _temp2441= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2447=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2447->f1=( void*) Cyc_Absyn_Loc_n; _temp2447->f2=({ struct _tagged_arr*
_temp2448=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp2448[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); _temp2448;}); _temp2447;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line)));
_temp2441->tq=({ struct Cyc_Absyn_Tqual _temp2442; _temp2442.q_const= 1;
_temp2442.q_volatile= 0; _temp2442.q_restrict= 1; _temp2442;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2443=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2443[ 0]=({ struct Cyc_Exp_tok_struct
_temp2444; _temp2444.tag= Cyc_Exp_tok; _temp2444.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2445=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2445[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2446; _temp2446.tag= Cyc_Absyn_Comprehension_e;
_temp2446.f1= _temp2441; _temp2446.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2446.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2446;});
_temp2445;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2444;}); _temp2443;}); break;} case
190: _LL2440: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2450=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2450[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2451; _temp2451.tag=
Cyc_InitializerList_tok; _temp2451.f1=({ struct Cyc_List_List* _temp2452=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2452->hd=(
void*)({ struct _tuple20* _temp2453=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2453->f1= 0; _temp2453->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2453;}); _temp2452->tl=
0; _temp2452;}); _temp2451;}); _temp2450;}); break; case 191: _LL2449: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2455=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2455[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2456; _temp2456.tag= Cyc_InitializerList_tok;
_temp2456.f1=({ struct Cyc_List_List* _temp2457=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2457->hd=( void*)({ struct
_tuple20* _temp2458=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2458->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2458->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2458;});
_temp2457->tl= 0; _temp2457;}); _temp2456;}); _temp2455;}); break; case 192:
_LL2454: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2460=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2460[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2461; _temp2461.tag=
Cyc_InitializerList_tok; _temp2461.f1=({ struct Cyc_List_List* _temp2462=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2462->hd=(
void*)({ struct _tuple20* _temp2463=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2463->f1= 0; _temp2463->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2463;}); _temp2462->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2462;}); _temp2461;}); _temp2460;}); break; case 193:
_LL2459: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2465=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2465[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2466; _temp2466.tag=
Cyc_InitializerList_tok; _temp2466.f1=({ struct Cyc_List_List* _temp2467=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2467->hd=(
void*)({ struct _tuple20* _temp2468=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2468->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2468->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2468;});
_temp2467->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2467;});
_temp2466;}); _temp2465;}); break; case 194: _LL2464: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2470=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2470[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2471; _temp2471.tag= Cyc_DesignatorList_tok;
_temp2471.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])); _temp2471;}); _temp2470;}); break; case 195: _LL2469:
yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp2473=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2473[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2474; _temp2474.tag= Cyc_DesignatorList_tok;
_temp2474.f1=({ struct Cyc_List_List* _temp2475=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2475->hd=( void*) Cyc_yyget_Designator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2475->tl= 0;
_temp2475;}); _temp2474;}); _temp2473;}); break; case 196: _LL2472: yyval=( void*)({
struct Cyc_DesignatorList_tok_struct* _temp2477=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2477[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2478; _temp2478.tag= Cyc_DesignatorList_tok;
_temp2478.f1=({ struct Cyc_List_List* _temp2479=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2479->hd=( void*) Cyc_yyget_Designator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2479->tl=
Cyc_yyget_DesignatorList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2479;}); _temp2478;}); _temp2477;}); break; case 197:
_LL2476: yyval=( void*)({ struct Cyc_Designator_tok_struct* _temp2481=( struct
Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp2481[ 0]=({ struct Cyc_Designator_tok_struct _temp2482; _temp2482.tag= Cyc_Designator_tok;
_temp2482.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2483=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2483[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2484; _temp2484.tag=
Cyc_Absyn_ArrayElement; _temp2484.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2484;});
_temp2483;})); _temp2482;}); _temp2481;}); break; case 198: _LL2480: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2486=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2486[ 0]=({ struct
Cyc_Designator_tok_struct _temp2487; _temp2487.tag= Cyc_Designator_tok;
_temp2487.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2488=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2488[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2489; _temp2489.tag= Cyc_Absyn_FieldName;
_temp2489.f1=({ struct _tagged_arr* _temp2490=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2490[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2490;}); _temp2489;});
_temp2488;})); _temp2487;}); _temp2486;}); break; case 199: _LL2485: { void*
_temp2492= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2493=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2493 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2494=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2495=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2495[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2496; _temp2496.tag= Cyc_ParamDecl_tok; _temp2496.f1=({
struct _tuple2* _temp2497=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp2497[ 0]=({ struct _tuple2 _temp2498; _temp2498.f1= 0; _temp2498.f2=
_temp2494; _temp2498.f3= _temp2492; _temp2498;}); _temp2497;}); _temp2496;});
_temp2495;}); break;}} case 200: _LL2491: { void* _temp2500= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2501=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2502=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2503=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2504= Cyc_Parse_apply_tms( _temp2502, _temp2500, _temp2501,
_temp2503); if( _temp2504.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2504.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring", sizeof(
unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2505=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2505[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2506; _temp2506.tag= Cyc_ParamDecl_tok; _temp2506.f1=({
struct _tuple2* _temp2507=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2507->f1= 0; _temp2507->f2= _temp2504.f1; _temp2507->f3= _temp2504.f2;
_temp2507;}); _temp2506;}); _temp2505;}); break;} case 201: _LL2499: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2509=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2509[ 0]=({ struct Cyc_Type_tok_struct
_temp2510; _temp2510.tag= Cyc_Type_tok; _temp2510.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2510;});
_temp2509;}); break; case 202: _LL2508: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2512=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2512[ 0]=({ struct Cyc_Type_tok_struct _temp2513; _temp2513.tag= Cyc_Type_tok;
_temp2513.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2514=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2514[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2515; _temp2515.tag= Cyc_Absyn_JoinEff;
_temp2515.f1= 0; _temp2515;}); _temp2514;})); _temp2513;}); _temp2512;}); break;
case 203: _LL2511: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2517=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2517[ 0]=({ struct Cyc_Type_tok_struct _temp2518; _temp2518.tag= Cyc_Type_tok;
_temp2518.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2519=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2519[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2520; _temp2520.tag= Cyc_Absyn_JoinEff;
_temp2520.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2520;}); _temp2519;})); _temp2518;});
_temp2517;}); break; case 204: _LL2516: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2522=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2522[ 0]=({ struct Cyc_Type_tok_struct _temp2523; _temp2523.tag= Cyc_Type_tok;
_temp2523.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2524=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2524[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2525; _temp2525.tag= Cyc_Absyn_JoinEff;
_temp2525.f1=({ struct Cyc_List_List* _temp2526=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2526->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2526->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2526;}); _temp2525;}); _temp2524;})); _temp2523;});
_temp2522;}); break; case 205: _LL2521: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2528=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2528[ 0]=({ struct Cyc_TypeList_tok_struct _temp2529; _temp2529.tag= Cyc_TypeList_tok;
_temp2529.f1=({ struct Cyc_List_List* _temp2530=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2530->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2530->tl= 0;
_temp2530;}); _temp2529;}); _temp2528;}); break; case 206: _LL2527: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2532=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2532[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2533; _temp2533.tag= Cyc_TypeList_tok; _temp2533.f1=({ struct Cyc_List_List*
_temp2534=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2534->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2534->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2534;});
_temp2533;}); _temp2532;}); break; case 207: _LL2531: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2536=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2536[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2537; _temp2537.tag= Cyc_AbstractDeclarator_tok;
_temp2537.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2538=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2538->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2538;});
_temp2537;}); _temp2536;}); break; case 208: _LL2535: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 209: _LL2539:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2541=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2541[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2542; _temp2542.tag=
Cyc_AbstractDeclarator_tok; _temp2542.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2543=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2543->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2543;});
_temp2542;}); _temp2541;}); break; case 210: _LL2540: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 211:
_LL2544: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2546=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2546[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2547; _temp2547.tag=
Cyc_AbstractDeclarator_tok; _temp2547.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2548=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2548->tms=({ struct Cyc_List_List* _temp2549=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2549->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2549->tl= 0; _temp2549;}); _temp2548;}); _temp2547;}); _temp2546;}); break;
case 212: _LL2545: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2551=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2551[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2552; _temp2552.tag= Cyc_AbstractDeclarator_tok; _temp2552.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2553=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2553->tms=({ struct Cyc_List_List* _temp2554=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2554->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2554->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->tms; _temp2554;});
_temp2553;}); _temp2552;}); _temp2551;}); break; case 213: _LL2550: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2556=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2556[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2557; _temp2557.tag= Cyc_AbstractDeclarator_tok;
_temp2557.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2558=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2558->tms=({
struct Cyc_List_List* _temp2559=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2559->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2560=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2560[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2561; _temp2561.tag= Cyc_Absyn_ConstArray_mod; _temp2561.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2561;});
_temp2560;})); _temp2559->tl= 0; _temp2559;}); _temp2558;}); _temp2557;});
_temp2556;}); break; case 214: _LL2555: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2563=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2563[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2564; _temp2564.tag= Cyc_AbstractDeclarator_tok; _temp2564.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2565=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2565->tms=({ struct Cyc_List_List* _temp2566=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2566->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2567=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2567[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2568; _temp2568.tag= Cyc_Absyn_ConstArray_mod;
_temp2568.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2568;}); _temp2567;})); _temp2566->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2566;}); _temp2565;}); _temp2564;}); _temp2563;}); break; case 215: _LL2562:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2570=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2570[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2571; _temp2571.tag=
Cyc_AbstractDeclarator_tok; _temp2571.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2572=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2572->tms=({ struct Cyc_List_List* _temp2573=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2573->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2574=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2574[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2575; _temp2575.tag= Cyc_Absyn_Function_mod;
_temp2575.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2576=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2576[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2577; _temp2577.tag= Cyc_Absyn_WithTypes;
_temp2577.f1= 0; _temp2577.f2= 0; _temp2577.f3= 0; _temp2577.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2577.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2577;}); _temp2576;})); _temp2575;}); _temp2574;}));
_temp2573->tl= 0; _temp2573;}); _temp2572;}); _temp2571;}); _temp2570;}); break;
case 216: _LL2569: { struct _tuple17 _temp2581; struct Cyc_List_List* _temp2582;
struct Cyc_Core_Opt* _temp2584; struct Cyc_Absyn_VarargInfo* _temp2586; int
_temp2588; struct Cyc_List_List* _temp2590; struct _tuple17* _temp2579= Cyc_yyget_ParamDeclListBool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2581=*
_temp2579; _LL2591: _temp2590= _temp2581.f1; goto _LL2589; _LL2589: _temp2588=
_temp2581.f2; goto _LL2587; _LL2587: _temp2586= _temp2581.f3; goto _LL2585;
_LL2585: _temp2584= _temp2581.f4; goto _LL2583; _LL2583: _temp2582= _temp2581.f5;
goto _LL2580; _LL2580: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2592=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2592[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2593; _temp2593.tag= Cyc_AbstractDeclarator_tok; _temp2593.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2594=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2594->tms=({ struct Cyc_List_List* _temp2595=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2595->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2596=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2596[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2597; _temp2597.tag= Cyc_Absyn_Function_mod;
_temp2597.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2598=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2598[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2599; _temp2599.tag= Cyc_Absyn_WithTypes;
_temp2599.f1= _temp2590; _temp2599.f2= _temp2588; _temp2599.f3= _temp2586;
_temp2599.f4= _temp2584; _temp2599.f5= _temp2582; _temp2599;}); _temp2598;}));
_temp2597;}); _temp2596;})); _temp2595->tl= 0; _temp2595;}); _temp2594;});
_temp2593;}); _temp2592;}); break;} case 217: _LL2578: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2601=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2601[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2602; _temp2602.tag= Cyc_AbstractDeclarator_tok;
_temp2602.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2603=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2603->tms=({
struct Cyc_List_List* _temp2604=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2604->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2605=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2605[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2606; _temp2606.tag=
Cyc_Absyn_Function_mod; _temp2606.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2607=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2607[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2608; _temp2608.tag= Cyc_Absyn_WithTypes;
_temp2608.f1= 0; _temp2608.f2= 0; _temp2608.f3= 0; _temp2608.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2608.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2608;}); _temp2607;})); _temp2606;}); _temp2605;}));
_temp2604->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->tms; _temp2604;});
_temp2603;}); _temp2602;}); _temp2601;}); break; case 218: _LL2600: { struct
_tuple17 _temp2612; struct Cyc_List_List* _temp2613; struct Cyc_Core_Opt*
_temp2615; struct Cyc_Absyn_VarargInfo* _temp2617; int _temp2619; struct Cyc_List_List*
_temp2621; struct _tuple17* _temp2610= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2612=*
_temp2610; _LL2622: _temp2621= _temp2612.f1; goto _LL2620; _LL2620: _temp2619=
_temp2612.f2; goto _LL2618; _LL2618: _temp2617= _temp2612.f3; goto _LL2616;
_LL2616: _temp2615= _temp2612.f4; goto _LL2614; _LL2614: _temp2613= _temp2612.f5;
goto _LL2611; _LL2611: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2623=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2623[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2624; _temp2624.tag= Cyc_AbstractDeclarator_tok; _temp2624.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2625=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2625->tms=({ struct Cyc_List_List* _temp2626=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2626->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2627=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2627[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2628; _temp2628.tag= Cyc_Absyn_Function_mod;
_temp2628.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2629=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2629[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2630; _temp2630.tag= Cyc_Absyn_WithTypes;
_temp2630.f1= _temp2621; _temp2630.f2= _temp2619; _temp2630.f3= _temp2617;
_temp2630.f4= _temp2615; _temp2630.f5= _temp2613; _temp2630;}); _temp2629;}));
_temp2628;}); _temp2627;})); _temp2626->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2626;}); _temp2625;}); _temp2624;}); _temp2623;}); break;} case 219:
_LL2609: { struct Cyc_List_List* _temp2632=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); yyval=(
void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2633=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2633[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2634; _temp2634.tag= Cyc_AbstractDeclarator_tok;
_temp2634.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2635=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2635->tms=({
struct Cyc_List_List* _temp2636=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2636->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2637=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2637[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2638; _temp2638.tag= Cyc_Absyn_TypeParams_mod; _temp2638.f1= _temp2632;
_temp2638.f2= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line); _temp2638.f3= 0; _temp2638;}); _temp2637;}));
_temp2636->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2636;});
_temp2635;}); _temp2634;}); _temp2633;}); break;} case 220: _LL2631:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2640=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); yyval=(
void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2641=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2641[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2642; _temp2642.tag= Cyc_AbstractDeclarator_tok;
_temp2642.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2643=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2643->tms=({
struct Cyc_List_List* _temp2644=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2644->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2645=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2645[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2646; _temp2646.tag= Cyc_Absyn_TypeParams_mod; _temp2646.f1= _temp2640;
_temp2646.f2= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line); _temp2646.f3= 0; _temp2646;}); _temp2645;}));
_temp2644->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2644;});
_temp2643;}); _temp2642;}); _temp2641;}); break;} case 221: _LL2639: yyval=(
void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2648=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2648[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2649; _temp2649.tag= Cyc_AbstractDeclarator_tok;
_temp2649.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2650=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2650->tms=({
struct Cyc_List_List* _temp2651=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2651->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp2652=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp2652[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp2653; _temp2653.tag= Cyc_Absyn_Attributes_mod; _temp2653.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2653.f2=
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2653;}); _temp2652;})); _temp2651->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->tms;
_temp2651;}); _temp2650;}); _temp2649;}); _temp2648;}); break; case 222: _LL2647:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
223: _LL2654: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 224: _LL2655: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 225: _LL2656: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 226: _LL2657:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
227: _LL2658: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 228: _LL2659: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2671=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2671->name=({ struct
_tagged_arr* _temp2672=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2672[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2672;});
_temp2671->identity= 0; _temp2671->kind=(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp2671;}); void* t=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2669=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2669[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2670; _temp2670.tag= Cyc_Absyn_VarType; _temp2670.f1=
tv; _temp2670;}); _temp2669;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2661=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2661[ 0]=({ struct Cyc_Stmt_tok_struct _temp2662; _temp2662.tag= Cyc_Stmt_tok;
_temp2662.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2663=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2663[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2664; _temp2664.tag= Cyc_Absyn_Region_s;
_temp2664.f1= tv; _temp2664.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2665=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2665->f1=(
void*) Cyc_Absyn_Loc_n; _temp2665->f2=({ struct _tagged_arr* _temp2666=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2666[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2666;});
_temp2665;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2667=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2667[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2668; _temp2668.tag=
Cyc_Absyn_RgnHandleType; _temp2668.f1=( void*) t; _temp2668;}); _temp2667;}), 0);
_temp2664.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2664;}); _temp2663;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2662;});
_temp2661;}); break;} case 229: _LL2660: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2684=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2684->name=({ struct
_tagged_arr* _temp2685=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2685[ 0]=( struct _tagged_arr)({ struct _tagged_arr
_temp2686= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); xprintf("`%.*s", _get_arr_size( _temp2686, 1u), _temp2686.curr);});
_temp2685;}); _temp2684->identity= 0; _temp2684->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp2684;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2682=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2682[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2683; _temp2683.tag= Cyc_Absyn_VarType; _temp2683.f1=
tv; _temp2683;}); _temp2682;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2674=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2674[ 0]=({ struct Cyc_Stmt_tok_struct _temp2675; _temp2675.tag= Cyc_Stmt_tok;
_temp2675.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2676=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2676[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2677; _temp2677.tag= Cyc_Absyn_Region_s;
_temp2677.f1= tv; _temp2677.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2678=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2678->f1=(
void*) Cyc_Absyn_Loc_n; _temp2678->f2=({ struct _tagged_arr* _temp2679=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2679[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2679;});
_temp2678;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2680=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2680[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2681; _temp2681.tag=
Cyc_Absyn_RgnHandleType; _temp2681.f1=( void*) t; _temp2681;}); _temp2680;}), 0);
_temp2677.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2677;}); _temp2676;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2675;});
_temp2674;}); break;} case 230: _LL2673: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2688=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2688[ 0]=({ struct Cyc_Stmt_tok_struct _temp2689; _temp2689.tag= Cyc_Stmt_tok;
_temp2689.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2690=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2690[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2691; _temp2691.tag= Cyc_Absyn_Cut_s;
_temp2691.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2691;}); _temp2690;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2689;});
_temp2688;}); break; case 231: _LL2687: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2693=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2693[ 0]=({ struct Cyc_Stmt_tok_struct _temp2694; _temp2694.tag= Cyc_Stmt_tok;
_temp2694.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2695=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2695[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2696; _temp2696.tag= Cyc_Absyn_Splice_s;
_temp2696.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2696;}); _temp2695;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2694;});
_temp2693;}); break; case 232: _LL2692: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2698=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2698[ 0]=({ struct Cyc_Stmt_tok_struct _temp2699; _temp2699.tag= Cyc_Stmt_tok;
_temp2699.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2700=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2700[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2701; _temp2701.tag= Cyc_Absyn_Label_s;
_temp2701.f1=({ struct _tagged_arr* _temp2702=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2702[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2702;});
_temp2701.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2701;}); _temp2700;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2699;});
_temp2698;}); break; case 233: _LL2697: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2704=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2704[ 0]=({ struct Cyc_Stmt_tok_struct _temp2705; _temp2705.tag= Cyc_Stmt_tok;
_temp2705.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2705;});
_temp2704;}); break; case 234: _LL2703: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2707=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2707[ 0]=({ struct Cyc_Stmt_tok_struct _temp2708; _temp2708.tag= Cyc_Stmt_tok;
_temp2708.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2708;}); _temp2707;}); break; case 235: _LL2706: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2710=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2710[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2711; _temp2711.tag= Cyc_Stmt_tok; _temp2711.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2711;}); _temp2710;}); break; case 236: _LL2709: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 237:
_LL2712:{ void* _temp2714= Cyc_yyget_BlockItem_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); struct Cyc_List_List*
_temp2722; struct Cyc_Absyn_Fndecl* _temp2724; struct Cyc_Absyn_Stmt* _temp2726;
_LL2716: if(*(( int*) _temp2714) == Cyc_Parse_TopDecls_bl){ _LL2723: _temp2722=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp2714)->f1; goto _LL2717;} else{ goto
_LL2718;} _LL2718: if(*(( int*) _temp2714) == Cyc_Parse_FnDecl_bl){ _LL2725:
_temp2724=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp2714)->f1; goto _LL2719;}
else{ goto _LL2720;} _LL2720: if(*(( int*) _temp2714) == Cyc_Parse_Stmt_bl){
_LL2727: _temp2726=(( struct Cyc_Parse_Stmt_bl_struct*) _temp2714)->f1; goto
_LL2721;} else{ goto _LL2715;} _LL2717: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2728=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2728[ 0]=({ struct Cyc_Stmt_tok_struct _temp2729; _temp2729.tag= Cyc_Stmt_tok;
_temp2729.f1= Cyc_Parse_flatten_declarations( _temp2722, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2729;}); _temp2728;}); goto _LL2715; _LL2719:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2730=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2730[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2731; _temp2731.tag= Cyc_Stmt_tok; _temp2731.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2732=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2732[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2733; _temp2733.tag= Cyc_Absyn_Fn_d;
_temp2733.f1= _temp2724; _temp2733;}); _temp2732;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2731;}); _temp2730;}); goto _LL2715; _LL2721:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2734=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2734[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2735; _temp2735.tag= Cyc_Stmt_tok; _temp2735.f1= _temp2726; _temp2735;});
_temp2734;}); goto _LL2715; _LL2715:;} break; case 238: _LL2713:{ void*
_temp2737= Cyc_yyget_BlockItem_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); struct Cyc_List_List* _temp2745; struct Cyc_Absyn_Fndecl*
_temp2747; struct Cyc_Absyn_Stmt* _temp2749; _LL2739: if(*(( int*) _temp2737) ==
Cyc_Parse_TopDecls_bl){ _LL2746: _temp2745=(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp2737)->f1; goto _LL2740;} else{ goto _LL2741;} _LL2741: if(*(( int*)
_temp2737) == Cyc_Parse_FnDecl_bl){ _LL2748: _temp2747=(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp2737)->f1; goto _LL2742;} else{ goto _LL2743;} _LL2743: if(*(( int*)
_temp2737) == Cyc_Parse_Stmt_bl){ _LL2750: _temp2749=(( struct Cyc_Parse_Stmt_bl_struct*)
_temp2737)->f1; goto _LL2744;} else{ goto _LL2738;} _LL2740: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2751=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2751[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2752; _temp2752.tag= Cyc_Stmt_tok; _temp2752.f1= Cyc_Parse_flatten_declarations(
_temp2745, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2752;}); _temp2751;}); goto _LL2738; _LL2742: yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2753=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2753[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2754; _temp2754.tag= Cyc_Stmt_tok; _temp2754.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2755=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2755[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2756; _temp2756.tag= Cyc_Absyn_Fn_d;
_temp2756.f1= _temp2747; _temp2756;}); _temp2755;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2754;}); _temp2753;}); goto _LL2738; _LL2744: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2757=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2757[ 0]=({ struct Cyc_Stmt_tok_struct _temp2758; _temp2758.tag= Cyc_Stmt_tok;
_temp2758.f1= Cyc_Absyn_seq_stmt( _temp2749, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2758;}); _temp2757;}); goto _LL2738; _LL2738:;} break; case 239: _LL2736:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2760=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp2760[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp2761; _temp2761.tag= Cyc_BlockItem_tok; _temp2761.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp2762=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp2762[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp2763; _temp2763.tag= Cyc_Parse_TopDecls_bl;
_temp2763.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2763;}); _temp2762;})); _temp2761;}); _temp2760;});
break; case 240: _LL2759: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp2765=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2765[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2766; _temp2766.tag= Cyc_BlockItem_tok;
_temp2766.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp2767=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp2767[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp2768; _temp2768.tag= Cyc_Parse_Stmt_bl;
_temp2768.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2768;}); _temp2767;})); _temp2766;}); _temp2765;}); break;
case 241: _LL2764: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2770=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2770[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2771; _temp2771.tag= Cyc_BlockItem_tok;
_temp2771.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp2772=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp2772[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp2773; _temp2773.tag= Cyc_Parse_FnDecl_bl;
_temp2773.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2773;}); _temp2772;})); _temp2771;}); _temp2770;}); break;
case 242: _LL2769: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2775=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2775[ 0]=({ struct Cyc_Stmt_tok_struct _temp2776; _temp2776.tag= Cyc_Stmt_tok;
_temp2776.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_skip_stmt(
0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2776;}); _temp2775;}); break; case 243:
_LL2774: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2778=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2778[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2779; _temp2779.tag= Cyc_Stmt_tok; _temp2779.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2779;});
_temp2778;}); break; case 244: _LL2777: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2781=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2781[ 0]=({ struct Cyc_Stmt_tok_struct _temp2782; _temp2782.tag= Cyc_Stmt_tok;
_temp2782.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2782;}); _temp2781;}); break; case 245: _LL2780: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2784=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2784[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2785; _temp2785.tag= Cyc_Stmt_tok; _temp2785.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2786=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2786[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2787; _temp2787.tag= Cyc_Absyn_SwitchC_s;
_temp2787.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2787.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2787;});
_temp2786;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2785;}); _temp2784;}); break; case 246:
_LL2783: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2789=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2789[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2790; _temp2790.tag= Cyc_Stmt_tok; _temp2790.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2790;});
_temp2789;}); break; case 247: _LL2788: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2792=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2792[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2793; _temp2793.tag= Cyc_SwitchClauseList_tok; _temp2793.f1= 0; _temp2793;});
_temp2792;}); break; case 248: _LL2791: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2795=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2795[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2796; _temp2796.tag= Cyc_SwitchClauseList_tok; _temp2796.f1=({ struct Cyc_List_List*
_temp2797=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2797->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2798=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2798->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp2798->pat_vars= 0; _temp2798->where_clause= 0; _temp2798->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2798->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2798;}); _temp2797->tl= 0; _temp2797;});
_temp2796;}); _temp2795;}); break; case 249: _LL2794: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2800=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2800[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2801; _temp2801.tag= Cyc_SwitchClauseList_tok;
_temp2801.f1=({ struct Cyc_List_List* _temp2802=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2802->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2803=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2803->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2803->pat_vars= 0; _temp2803->where_clause= 0;
_temp2803->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2803->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2803;});
_temp2802->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2802;}); _temp2801;});
_temp2800;}); break; case 250: _LL2799: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2805=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2805[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2806; _temp2806.tag= Cyc_SwitchClauseList_tok; _temp2806.f1=({ struct Cyc_List_List*
_temp2807=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2807->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2808=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2808->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2808->pat_vars=
0; _temp2808->where_clause= 0; _temp2808->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2808->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp2808;}); _temp2807->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2807;}); _temp2806;});
_temp2805;}); break; case 251: _LL2804: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2810=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2810[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2811; _temp2811.tag= Cyc_SwitchClauseList_tok; _temp2811.f1=({ struct Cyc_List_List*
_temp2812=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2812->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2813=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2813->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2813->pat_vars=
0; _temp2813->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2813->body=
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2813->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2813;});
_temp2812->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2812;}); _temp2811;});
_temp2810;}); break; case 252: _LL2809: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2815=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2815[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2816; _temp2816.tag= Cyc_SwitchClauseList_tok; _temp2816.f1=({ struct Cyc_List_List*
_temp2817=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2817->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2818=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2818->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _temp2818->pat_vars=
0; _temp2818->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2818->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2818->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2818;});
_temp2817->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2817;}); _temp2816;});
_temp2815;}); break; case 253: _LL2814: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2820=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2820[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2821; _temp2821.tag= Cyc_SwitchCClauseList_tok; _temp2821.f1= 0; _temp2821;});
_temp2820;}); break; case 254: _LL2819: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2823=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2823[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2824; _temp2824.tag= Cyc_SwitchCClauseList_tok; _temp2824.f1=({ struct Cyc_List_List*
_temp2825=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2825->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2826=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2826->cnst_exp= 0;
_temp2826->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2826->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2826;});
_temp2825->tl= 0; _temp2825;}); _temp2824;}); _temp2823;}); break; case 255:
_LL2822: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2828=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2828[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2829; _temp2829.tag=
Cyc_SwitchCClauseList_tok; _temp2829.f1=({ struct Cyc_List_List* _temp2830=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2830->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2831=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2831->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2831->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2831->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2831;});
_temp2830->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2830;}); _temp2829;});
_temp2828;}); break; case 256: _LL2827: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2833=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2833[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2834; _temp2834.tag= Cyc_SwitchCClauseList_tok; _temp2834.f1=({ struct Cyc_List_List*
_temp2835=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2835->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2836=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2836->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _temp2836->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2836->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2836;});
_temp2835->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2835;}); _temp2834;});
_temp2833;}); break; case 257: _LL2832: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2838=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2838[ 0]=({ struct Cyc_Stmt_tok_struct _temp2839; _temp2839.tag= Cyc_Stmt_tok;
_temp2839.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2839;}); _temp2838;}); break; case 258: _LL2837: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2841=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2841[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2842; _temp2842.tag= Cyc_Stmt_tok; _temp2842.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2842;}); _temp2841;}); break; case 259: _LL2840: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2844=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2844[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2845; _temp2845.tag= Cyc_Stmt_tok; _temp2845.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2845;}); _temp2844;}); break; case 260: _LL2843: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2847=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2847[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2848; _temp2848.tag= Cyc_Stmt_tok; _temp2848.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2848;}); _temp2847;}); break; case 261: _LL2846: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2850=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2850[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2851; _temp2851.tag= Cyc_Stmt_tok; _temp2851.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2851;}); _temp2850;}); break; case 262: _LL2849: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2853=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2853[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2854; _temp2854.tag= Cyc_Stmt_tok; _temp2854.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2854;}); _temp2853;}); break; case 263: _LL2852: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2856=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2856[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2857; _temp2857.tag= Cyc_Stmt_tok; _temp2857.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2857;}); _temp2856;}); break; case 264: _LL2855: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2859=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2859[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2860; _temp2860.tag= Cyc_Stmt_tok; _temp2860.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2860;});
_temp2859;}); break; case 265: _LL2858: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2862=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2862[ 0]=({ struct Cyc_Stmt_tok_struct _temp2863; _temp2863.tag= Cyc_Stmt_tok;
_temp2863.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2863;});
_temp2862;}); break; case 266: _LL2861: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2865=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2865[ 0]=({ struct Cyc_Stmt_tok_struct _temp2866; _temp2866.tag= Cyc_Stmt_tok;
_temp2866.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2866;}); _temp2865;}); break; case 267: _LL2864: { struct Cyc_List_List*
_temp2868= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); struct Cyc_Absyn_Stmt* _temp2869= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2870=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2870[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2871; _temp2871.tag= Cyc_Stmt_tok; _temp2871.f1= Cyc_Parse_flatten_declarations(
_temp2868, _temp2869); _temp2871;}); _temp2870;}); break;} case 268: _LL2867: {
struct Cyc_List_List* _temp2873= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2874= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2875=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2875[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2876; _temp2876.tag= Cyc_Stmt_tok; _temp2876.f1= Cyc_Parse_flatten_declarations(
_temp2873, _temp2874); _temp2876;}); _temp2875;}); break;} case 269: _LL2872: {
struct Cyc_List_List* _temp2878= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2879= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2880=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2880[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2881; _temp2881.tag= Cyc_Stmt_tok; _temp2881.f1= Cyc_Parse_flatten_declarations(
_temp2878, _temp2879); _temp2881;}); _temp2880;}); break;} case 270: _LL2877: {
struct Cyc_List_List* _temp2883= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); struct Cyc_Absyn_Stmt*
_temp2884= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2885=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2885[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2886; _temp2886.tag= Cyc_Stmt_tok; _temp2886.f1= Cyc_Parse_flatten_declarations(
_temp2883, _temp2884); _temp2886;}); _temp2885;}); break;} case 271: _LL2882:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2888=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2888[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2889; _temp2889.tag= Cyc_Stmt_tok; _temp2889.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2890=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2890[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2890;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2889;}); _temp2888;}); break; case 272: _LL2887: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2892=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2892[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2893; _temp2893.tag= Cyc_Stmt_tok; _temp2893.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2893;}); _temp2892;}); break; case 273:
_LL2891: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2895=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2895[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2896; _temp2896.tag= Cyc_Stmt_tok; _temp2896.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2896;}); _temp2895;}); break; case 274: _LL2894: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2898=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2898[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2899; _temp2899.tag= Cyc_Stmt_tok; _temp2899.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2899;}); _temp2898;}); break; case 275:
_LL2897: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2901=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2901[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2902; _temp2902.tag= Cyc_Stmt_tok; _temp2902.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2902;}); _temp2901;}); break; case 276: _LL2900: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2904=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2904[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2905; _temp2905.tag= Cyc_Stmt_tok; _temp2905.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2905;}); _temp2904;}); break; case 277:
_LL2903: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2907=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2907[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2908; _temp2908.tag= Cyc_Stmt_tok; _temp2908.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); _temp2908;}); _temp2907;}); break; case 278:
_LL2906: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2910=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2910[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2911; _temp2911.tag= Cyc_Stmt_tok; _temp2911.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));
_temp2911;}); _temp2910;}); break; case 279: _LL2909: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2913=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2913[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2914; _temp2914.tag= Cyc_Pattern_tok; _temp2914.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2914;});
_temp2913;}); break; case 280: _LL2912: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 281:
_LL2915: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2917=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2917[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2918; _temp2918.tag= Cyc_Pattern_tok; _temp2918.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2919=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2919[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2920; _temp2920.tag= Cyc_Absyn_Int_p; _temp2920.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2920.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2920;}); _temp2919;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2918;});
_temp2917;}); break; case 282: _LL2916: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2922=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2922[ 0]=({ struct Cyc_Pattern_tok_struct _temp2923; _temp2923.tag= Cyc_Pattern_tok;
_temp2923.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2924=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2924[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2925; _temp2925.tag= Cyc_Absyn_Int_p;
_temp2925.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2925.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2925;});
_temp2924;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2923;}); _temp2922;}); break; case 283:
_LL2921: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2927=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2927[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2928; _temp2928.tag= Cyc_Pattern_tok; _temp2928.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2929=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2929[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2930; _temp2930.tag= Cyc_Absyn_Float_p; _temp2930.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2930;}); _temp2929;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2928;});
_temp2927;}); break; case 284: _LL2926: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2932=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2932[ 0]=({ struct Cyc_Pattern_tok_struct _temp2933; _temp2933.tag= Cyc_Pattern_tok;
_temp2933.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2934=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp2934[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp2935; _temp2935.tag= Cyc_Absyn_Char_p;
_temp2935.f1= Cyc_yyget_Char_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2935;}); _temp2934;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2933;});
_temp2932;}); break; case 285: _LL2931: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2937=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2937[ 0]=({ struct Cyc_Pattern_tok_struct _temp2938; _temp2938.tag= Cyc_Pattern_tok;
_temp2938.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2938;});
_temp2937;}); break; case 286: _LL2936: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2940=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2940[ 0]=({ struct Cyc_Pattern_tok_struct _temp2941; _temp2941.tag= Cyc_Pattern_tok;
_temp2941.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2942=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2942[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2943; _temp2943.tag=
Cyc_Absyn_UnknownId_p; _temp2943.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2943;}); _temp2942;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2941;}); _temp2940;}); break; case 287:
_LL2939: { struct Cyc_List_List* _temp2945=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2946=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2946[ 0]=({ struct Cyc_Pattern_tok_struct _temp2947; _temp2947.tag= Cyc_Pattern_tok;
_temp2947.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2948=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2948[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2949; _temp2949.tag= Cyc_Absyn_UnknownCall_p; _temp2949.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2949.f2=
_temp2945; _temp2949.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2949;});
_temp2948;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2947;}); _temp2946;}); break;} case
288: _LL2944: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2951=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2951[ 0]=({ struct Cyc_Pattern_tok_struct _temp2952; _temp2952.tag= Cyc_Pattern_tok;
_temp2952.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2953=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2953[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2954; _temp2954.tag= Cyc_Absyn_Tuple_p;
_temp2954.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2954;}); _temp2953;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2952;}); _temp2951;}); break; case 289: _LL2950: { struct Cyc_List_List*
_temp2956=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp2957=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2957[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2958; _temp2958.tag= Cyc_Pattern_tok; _temp2958.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2959=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2959[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2960; _temp2960.tag= Cyc_Absyn_UnknownFields_p;
_temp2960.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp2960.f2= _temp2956; _temp2960.f3= 0; _temp2960;});
_temp2959;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2958;}); _temp2957;}); break;} case
290: _LL2955: { struct Cyc_List_List* _temp2962=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2963=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2963[ 0]=({ struct Cyc_Pattern_tok_struct _temp2964; _temp2964.tag= Cyc_Pattern_tok;
_temp2964.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2965=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2965[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2966; _temp2966.tag= Cyc_Absyn_UnknownFields_p; _temp2966.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2966.f2=
_temp2962; _temp2966.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2966;});
_temp2965;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2964;}); _temp2963;}); break;} case
291: _LL2961: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2968=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2968[ 0]=({ struct Cyc_Pattern_tok_struct _temp2969; _temp2969.tag= Cyc_Pattern_tok;
_temp2969.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2970=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2970[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2971; _temp2971.tag= Cyc_Absyn_Pointer_p;
_temp2971.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2971;}); _temp2970;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2969;});
_temp2968;}); break; case 292: _LL2967: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2973=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2973[ 0]=({ struct Cyc_Pattern_tok_struct _temp2974; _temp2974.tag= Cyc_Pattern_tok;
_temp2974.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2975=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2975[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2976; _temp2976.tag=
Cyc_Absyn_Reference_p; _temp2976.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2977=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2977->f1=(
void*) Cyc_Absyn_Loc_n; _temp2977->f2=({ struct _tagged_arr* _temp2978=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2978[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2978;});
_temp2977;}),( void*) Cyc_Absyn_VoidType, 0); _temp2976;}); _temp2975;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2974;}); _temp2973;}); break; case 293: _LL2972: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2980=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2980[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2981; _temp2981.tag= Cyc_PatternList_tok;
_temp2981.f1= 0; _temp2981;}); _temp2980;}); break; case 294: _LL2979: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2983=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2983[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2984; _temp2984.tag= Cyc_PatternList_tok;
_temp2984.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2984;}); _temp2983;}); break; case 295: _LL2982: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2986=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2986[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2987; _temp2987.tag= Cyc_PatternList_tok;
_temp2987.f1=({ struct Cyc_List_List* _temp2988=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2988->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2988->tl= 0;
_temp2988;}); _temp2987;}); _temp2986;}); break; case 296: _LL2985: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2990=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2990[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2991; _temp2991.tag= Cyc_PatternList_tok;
_temp2991.f1=({ struct Cyc_List_List* _temp2992=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2992->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2992->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2992;}); _temp2991;}); _temp2990;}); break; case 297:
_LL2989: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2994=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2994[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2995; _temp2995.tag=
Cyc_FieldPattern_tok; _temp2995.f1=({ struct _tuple14* _temp2996=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2996->f1= 0; _temp2996->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2996;}); _temp2995;}); _temp2994;}); break; case 298:
_LL2993: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2998=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2998[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2999; _temp2999.tag=
Cyc_FieldPattern_tok; _temp2999.f1=({ struct _tuple14* _temp3000=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3000->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3000->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3000;}); _temp2999;}); _temp2998;}); break; case 299:
_LL2997: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3002=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp3002[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp3003; _temp3003.tag=
Cyc_FieldPatternList_tok; _temp3003.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3003;});
_temp3002;}); break; case 300: _LL3001: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp3005=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp3005[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp3006; _temp3006.tag= Cyc_FieldPatternList_tok; _temp3006.f1=({ struct Cyc_List_List*
_temp3007=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3007->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3007->tl= 0;
_temp3007;}); _temp3006;}); _temp3005;}); break; case 301: _LL3004: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp3009=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3009[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3010; _temp3010.tag= Cyc_FieldPatternList_tok;
_temp3010.f1=({ struct Cyc_List_List* _temp3011=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3011->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3011->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp3011;}); _temp3010;}); _temp3009;}); break; case 302:
_LL3008: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 303: _LL3012: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3014=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3014[ 0]=({ struct Cyc_Exp_tok_struct _temp3015; _temp3015.tag= Cyc_Exp_tok;
_temp3015.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3015;}); _temp3014;}); break; case 304: _LL3013: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 305: _LL3016:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3018=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3018[ 0]=({ struct Cyc_Exp_tok_struct
_temp3019; _temp3019.tag= Cyc_Exp_tok; _temp3019.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3019;}); _temp3018;}); break; case 306: _LL3017: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3021=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3021[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3022; _temp3022.tag= Cyc_Primopopt_tok; _temp3022.f1=
0; _temp3022;}); _temp3021;}); break; case 307: _LL3020: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3024=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3024[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3025; _temp3025.tag= Cyc_Primopopt_tok; _temp3025.f1=({
struct Cyc_Core_Opt* _temp3026=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3026->v=( void*)(( void*) Cyc_Absyn_Times); _temp3026;});
_temp3025;}); _temp3024;}); break; case 308: _LL3023: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3028=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3028[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3029; _temp3029.tag= Cyc_Primopopt_tok; _temp3029.f1=({
struct Cyc_Core_Opt* _temp3030=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3030->v=( void*)(( void*) Cyc_Absyn_Div); _temp3030;});
_temp3029;}); _temp3028;}); break; case 309: _LL3027: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3032=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3032[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3033; _temp3033.tag= Cyc_Primopopt_tok; _temp3033.f1=({
struct Cyc_Core_Opt* _temp3034=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3034->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3034;});
_temp3033;}); _temp3032;}); break; case 310: _LL3031: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3036=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3036[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3037; _temp3037.tag= Cyc_Primopopt_tok; _temp3037.f1=({
struct Cyc_Core_Opt* _temp3038=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3038->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3038;});
_temp3037;}); _temp3036;}); break; case 311: _LL3035: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3040=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3040[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3041; _temp3041.tag= Cyc_Primopopt_tok; _temp3041.f1=({
struct Cyc_Core_Opt* _temp3042=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3042->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3042;});
_temp3041;}); _temp3040;}); break; case 312: _LL3039: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3044=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3044[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3045; _temp3045.tag= Cyc_Primopopt_tok; _temp3045.f1=({
struct Cyc_Core_Opt* _temp3046=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3046->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3046;});
_temp3045;}); _temp3044;}); break; case 313: _LL3043: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3048=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3048[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3049; _temp3049.tag= Cyc_Primopopt_tok; _temp3049.f1=({
struct Cyc_Core_Opt* _temp3050=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3050->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3050;});
_temp3049;}); _temp3048;}); break; case 314: _LL3047: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3052=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3052[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3053; _temp3053.tag= Cyc_Primopopt_tok; _temp3053.f1=({
struct Cyc_Core_Opt* _temp3054=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3054->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3054;});
_temp3053;}); _temp3052;}); break; case 315: _LL3051: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3056=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3056[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3057; _temp3057.tag= Cyc_Primopopt_tok; _temp3057.f1=({
struct Cyc_Core_Opt* _temp3058=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3058->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3058;});
_temp3057;}); _temp3056;}); break; case 316: _LL3055: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3060=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3060[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3061; _temp3061.tag= Cyc_Primopopt_tok; _temp3061.f1=({
struct Cyc_Core_Opt* _temp3062=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3062->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3062;});
_temp3061;}); _temp3060;}); break; case 317: _LL3059: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 318: _LL3063:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3065=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3065[ 0]=({ struct Cyc_Exp_tok_struct
_temp3066; _temp3066.tag= Cyc_Exp_tok; _temp3066.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3066;});
_temp3065;}); break; case 319: _LL3064: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3068=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3068[ 0]=({ struct Cyc_Exp_tok_struct _temp3069; _temp3069.tag= Cyc_Exp_tok;
_temp3069.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3069;}); _temp3068;}); break; case 320: _LL3067: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3071=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3071[ 0]=({ struct Cyc_Exp_tok_struct
_temp3072; _temp3072.tag= Cyc_Exp_tok; _temp3072.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3072;}); _temp3071;}); break; case 321: _LL3070: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3074=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3074[ 0]=({ struct Cyc_Exp_tok_struct
_temp3075; _temp3075.tag= Cyc_Exp_tok; _temp3075.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3075;}); _temp3074;}); break; case 322: _LL3073: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3077=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3077[ 0]=({ struct Cyc_Exp_tok_struct
_temp3078; _temp3078.tag= Cyc_Exp_tok; _temp3078.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3078;});
_temp3077;}); break; case 323: _LL3076: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3080=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3080[ 0]=({ struct Cyc_Exp_tok_struct _temp3081; _temp3081.tag= Cyc_Exp_tok;
_temp3081.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3081;}); _temp3080;}); break; case 324: _LL3079: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 325: _LL3082:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
326: _LL3083: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3085=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3085[ 0]=({ struct Cyc_Exp_tok_struct
_temp3086; _temp3086.tag= Cyc_Exp_tok; _temp3086.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3086;}); _temp3085;}); break; case 327: _LL3084: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 328: _LL3087:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3089=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3089[ 0]=({ struct Cyc_Exp_tok_struct
_temp3090; _temp3090.tag= Cyc_Exp_tok; _temp3090.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3090;}); _temp3089;}); break; case 329: _LL3088: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 330: _LL3091:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3093=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3093[ 0]=({ struct Cyc_Exp_tok_struct
_temp3094; _temp3094.tag= Cyc_Exp_tok; _temp3094.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3094;});
_temp3093;}); break; case 331: _LL3092: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 332: _LL3095:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3097=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3097[ 0]=({ struct Cyc_Exp_tok_struct
_temp3098; _temp3098.tag= Cyc_Exp_tok; _temp3098.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3098;});
_temp3097;}); break; case 333: _LL3096: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 334: _LL3099:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3101=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3101[ 0]=({ struct Cyc_Exp_tok_struct
_temp3102; _temp3102.tag= Cyc_Exp_tok; _temp3102.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3102;});
_temp3101;}); break; case 335: _LL3100: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 336: _LL3103:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3105=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3105[ 0]=({ struct Cyc_Exp_tok_struct
_temp3106; _temp3106.tag= Cyc_Exp_tok; _temp3106.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3106;}); _temp3105;}); break; case 337: _LL3104: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3108=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3108[ 0]=({ struct Cyc_Exp_tok_struct
_temp3109; _temp3109.tag= Cyc_Exp_tok; _temp3109.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3109;}); _temp3108;}); break; case 338: _LL3107: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 339: _LL3110:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3112=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3112[ 0]=({ struct Cyc_Exp_tok_struct
_temp3113; _temp3113.tag= Cyc_Exp_tok; _temp3113.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3113;}); _temp3112;}); break; case 340: _LL3111: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3115=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3115[ 0]=({ struct Cyc_Exp_tok_struct
_temp3116; _temp3116.tag= Cyc_Exp_tok; _temp3116.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3116;}); _temp3115;}); break; case 341: _LL3114: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3118=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3118[ 0]=({ struct Cyc_Exp_tok_struct
_temp3119; _temp3119.tag= Cyc_Exp_tok; _temp3119.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3119;}); _temp3118;}); break; case 342: _LL3117: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3121=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3121[ 0]=({ struct Cyc_Exp_tok_struct
_temp3122; _temp3122.tag= Cyc_Exp_tok; _temp3122.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3122;}); _temp3121;}); break; case 343: _LL3120: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 344: _LL3123:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3125=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3125[ 0]=({ struct Cyc_Exp_tok_struct
_temp3126; _temp3126.tag= Cyc_Exp_tok; _temp3126.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3126;}); _temp3125;}); break; case 345: _LL3124: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3128=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3128[ 0]=({ struct Cyc_Exp_tok_struct
_temp3129; _temp3129.tag= Cyc_Exp_tok; _temp3129.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3129;}); _temp3128;}); break; case 346: _LL3127: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 347: _LL3130:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3132=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3132[ 0]=({ struct Cyc_Exp_tok_struct
_temp3133; _temp3133.tag= Cyc_Exp_tok; _temp3133.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3133;});
_temp3132;}); break; case 348: _LL3131: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3135=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3135[ 0]=({ struct Cyc_Exp_tok_struct _temp3136; _temp3136.tag= Cyc_Exp_tok;
_temp3136.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3136;}); _temp3135;}); break; case 349: _LL3134: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 350: _LL3137:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3139=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3139[ 0]=({ struct Cyc_Exp_tok_struct
_temp3140; _temp3140.tag= Cyc_Exp_tok; _temp3140.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3140;});
_temp3139;}); break; case 351: _LL3138: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3142=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3142[ 0]=({ struct Cyc_Exp_tok_struct _temp3143; _temp3143.tag= Cyc_Exp_tok;
_temp3143.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3143;}); _temp3142;}); break; case 352: _LL3141: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3145=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3145[ 0]=({ struct Cyc_Exp_tok_struct
_temp3146; _temp3146.tag= Cyc_Exp_tok; _temp3146.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3146;});
_temp3145;}); break; case 353: _LL3144: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 354: _LL3147:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3149=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3149[ 0]=({ struct Cyc_Exp_tok_struct
_temp3150; _temp3150.tag= Cyc_Exp_tok; _temp3150.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3150;}); _temp3149;}); break; case 355: _LL3148: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 356: _LL3151:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3153=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3153[ 0]=({ struct Cyc_Exp_tok_struct
_temp3154; _temp3154.tag= Cyc_Exp_tok; _temp3154.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3154;}); _temp3153;}); break; case 357: _LL3152: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3156=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3156[ 0]=({ struct Cyc_Exp_tok_struct
_temp3157; _temp3157.tag= Cyc_Exp_tok; _temp3157.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3157;}); _temp3156;}); break; case 358: _LL3155: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3159=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3159[ 0]=({ struct Cyc_Exp_tok_struct
_temp3160; _temp3160.tag= Cyc_Exp_tok; _temp3160.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3160;}); _temp3159;}); break; case 359: _LL3158: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3162=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3162[ 0]=({ struct Cyc_Exp_tok_struct
_temp3163; _temp3163.tag= Cyc_Exp_tok; _temp3163.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3163;}); _temp3162;}); break; case 360: _LL3161: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 361: _LL3164:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3166=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3166[ 0]=({ struct Cyc_Exp_tok_struct
_temp3167; _temp3167.tag= Cyc_Exp_tok; _temp3167.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3167;}); _temp3166;}); break; case 362: _LL3165: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3169=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3169[ 0]=({ struct Cyc_Exp_tok_struct
_temp3170; _temp3170.tag= Cyc_Exp_tok; _temp3170.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3170;});
_temp3169;}); break; case 363: _LL3168: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3172=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3172[ 0]=({ struct Cyc_Exp_tok_struct _temp3173; _temp3173.tag= Cyc_Exp_tok;
_temp3173.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3173;}); _temp3172;}); break; case 364: _LL3171: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3175=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3175[ 0]=({ struct Cyc_Exp_tok_struct
_temp3176; _temp3176.tag= Cyc_Exp_tok; _temp3176.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])).f3,({ struct _tagged_arr* _temp3177=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3177[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3177;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3176;}); _temp3175;}); break; case 365:
_LL3174: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3179=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3179[ 0]=({ struct Cyc_Exp_tok_struct
_temp3180; _temp3180.tag= Cyc_Exp_tok; _temp3180.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3180;}); _temp3179;}); break; case 366: _LL3178: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3182=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3182[ 0]=({ struct Cyc_Exp_tok_struct
_temp3183; _temp3183.tag= Cyc_Exp_tok; _temp3183.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3184=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3184[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3185; _temp3185.tag= Cyc_Absyn_Malloc_e;
_temp3185.f1= 0; _temp3185.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3185;}); _temp3184;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3183;});
_temp3182;}); break; case 367: _LL3181: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3187=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3187[ 0]=({ struct Cyc_Exp_tok_struct _temp3188; _temp3188.tag= Cyc_Exp_tok;
_temp3188.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3189=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3189[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3190; _temp3190.tag= Cyc_Absyn_Malloc_e;
_temp3190.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]); _temp3190.f2=( void*)
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3190;}); _temp3189;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3188;});
_temp3187;}); break; case 368: _LL3186: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3192=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3192[ 0]=({ struct Cyc_Primop_tok_struct _temp3193; _temp3193.tag= Cyc_Primop_tok;
_temp3193.f1=( void*)(( void*) Cyc_Absyn_Printf); _temp3193;}); _temp3192;});
break; case 369: _LL3191: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3195=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3195[ 0]=({ struct Cyc_Primop_tok_struct _temp3196; _temp3196.tag= Cyc_Primop_tok;
_temp3196.f1=( void*)(( void*) Cyc_Absyn_Fprintf); _temp3196;}); _temp3195;});
break; case 370: _LL3194: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3198=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3198[ 0]=({ struct Cyc_Primop_tok_struct _temp3199; _temp3199.tag= Cyc_Primop_tok;
_temp3199.f1=( void*)(( void*) Cyc_Absyn_Xprintf); _temp3199;}); _temp3198;});
break; case 371: _LL3197: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3201=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3201[ 0]=({ struct Cyc_Primop_tok_struct _temp3202; _temp3202.tag= Cyc_Primop_tok;
_temp3202.f1=( void*)(( void*) Cyc_Absyn_Scanf); _temp3202;}); _temp3201;});
break; case 372: _LL3200: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3204=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3204[ 0]=({ struct Cyc_Primop_tok_struct _temp3205; _temp3205.tag= Cyc_Primop_tok;
_temp3205.f1=( void*)(( void*) Cyc_Absyn_Fscanf); _temp3205;}); _temp3204;});
break; case 373: _LL3203: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3207=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3207[ 0]=({ struct Cyc_Primop_tok_struct _temp3208; _temp3208.tag= Cyc_Primop_tok;
_temp3208.f1=( void*)(( void*) Cyc_Absyn_Sscanf); _temp3208;}); _temp3207;});
break; case 374: _LL3206: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3210=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3210[ 0]=({ struct Cyc_Primop_tok_struct _temp3211; _temp3211.tag= Cyc_Primop_tok;
_temp3211.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3211;}); _temp3210;});
break; case 375: _LL3209: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3213=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3213[ 0]=({ struct Cyc_Primop_tok_struct _temp3214; _temp3214.tag= Cyc_Primop_tok;
_temp3214.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3214;}); _temp3213;});
break; case 376: _LL3212: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3216=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3216[ 0]=({ struct Cyc_Primop_tok_struct _temp3217; _temp3217.tag= Cyc_Primop_tok;
_temp3217.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3217;}); _temp3216;});
break; case 377: _LL3215: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 378: _LL3218: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3220=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3220[ 0]=({ struct Cyc_Exp_tok_struct _temp3221; _temp3221.tag= Cyc_Exp_tok;
_temp3221.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3221;}); _temp3220;}); break; case 379: _LL3219: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3223=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3223[ 0]=({ struct Cyc_Exp_tok_struct
_temp3224; _temp3224.tag= Cyc_Exp_tok; _temp3224.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3224;}); _temp3223;}); break; case 380:
_LL3222: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3226=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3226[ 0]=({ struct Cyc_Exp_tok_struct
_temp3227; _temp3227.tag= Cyc_Exp_tok; _temp3227.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3227;});
_temp3226;}); break; case 381: _LL3225: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3229=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3229[ 0]=({ struct Cyc_Exp_tok_struct _temp3230; _temp3230.tag= Cyc_Exp_tok;
_temp3230.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3231=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3231[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3231;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3230;}); _temp3229;}); break; case 382: _LL3228: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3233=(*
q).f1; struct Cyc_List_List* _temp3243; struct Cyc_List_List* _temp3245; _LL3235:
if( _temp3233 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3236;} else{ goto _LL3237;}
_LL3237: if(( unsigned int) _temp3233 > 1u?*(( int*) _temp3233) == Cyc_Absyn_Rel_n:
0){ _LL3244: _temp3243=(( struct Cyc_Absyn_Rel_n_struct*) _temp3233)->f1; if(
_temp3243 == 0){ goto _LL3238;} else{ goto _LL3239;}} else{ goto _LL3239;}
_LL3239: if(( unsigned int) _temp3233 > 1u?*(( int*) _temp3233) == Cyc_Absyn_Abs_n:
0){ _LL3246: _temp3245=(( struct Cyc_Absyn_Abs_n_struct*) _temp3233)->f1; if(
_temp3245 == 0){ goto _LL3240;} else{ goto _LL3241;}} else{ goto _LL3241;}
_LL3241: goto _LL3242; _LL3236: goto _LL3234; _LL3238: goto _LL3234; _LL3240:
goto _LL3234; _LL3242: Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3234; _LL3234:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3247=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3247[ 0]=({ struct Cyc_Exp_tok_struct _temp3248; _temp3248.tag= Cyc_Exp_tok;
_temp3248.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3248;}); _temp3247;}); break;} case 383: _LL3232: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3250=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3250[ 0]=({ struct Cyc_Exp_tok_struct
_temp3251; _temp3251.tag= Cyc_Exp_tok; _temp3251.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),({ struct _tagged_arr* _temp3252=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) * 1); _temp3252[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3252;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3251;}); _temp3250;}); break; case 384: _LL3249: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3254=(*
q).f1; struct Cyc_List_List* _temp3264; struct Cyc_List_List* _temp3266; _LL3256:
if( _temp3254 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3257;} else{ goto _LL3258;}
_LL3258: if(( unsigned int) _temp3254 > 1u?*(( int*) _temp3254) == Cyc_Absyn_Rel_n:
0){ _LL3265: _temp3264=(( struct Cyc_Absyn_Rel_n_struct*) _temp3254)->f1; if(
_temp3264 == 0){ goto _LL3259;} else{ goto _LL3260;}} else{ goto _LL3260;}
_LL3260: if(( unsigned int) _temp3254 > 1u?*(( int*) _temp3254) == Cyc_Absyn_Abs_n:
0){ _LL3267: _temp3266=(( struct Cyc_Absyn_Abs_n_struct*) _temp3254)->f1; if(
_temp3266 == 0){ goto _LL3261;} else{ goto _LL3262;}} else{ goto _LL3262;}
_LL3262: goto _LL3263; _LL3257: goto _LL3255; _LL3259: goto _LL3255; _LL3261:
goto _LL3255; _LL3263: Cyc_Parse_err( _tag_arr("struct field is qualified with module name",
sizeof( unsigned char), 43u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3255; _LL3255:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3268=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3268[ 0]=({ struct Cyc_Exp_tok_struct _temp3269; _temp3269.tag= Cyc_Exp_tok;
_temp3269.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3269;}); _temp3268;}); break;} case 385: _LL3253: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3271=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3271[ 0]=({ struct Cyc_Exp_tok_struct
_temp3272; _temp3272.tag= Cyc_Exp_tok; _temp3272.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3272;}); _temp3271;}); break; case 386: _LL3270: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3274=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3274[ 0]=({ struct Cyc_Exp_tok_struct
_temp3275; _temp3275.tag= Cyc_Exp_tok; _temp3275.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3275;}); _temp3274;}); break; case 387: _LL3273: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3277=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3277[ 0]=({ struct Cyc_Exp_tok_struct
_temp3278; _temp3278.tag= Cyc_Exp_tok; _temp3278.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3279=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3279[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3280; _temp3280.tag= Cyc_Absyn_CompoundLit_e;
_temp3280.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp3280.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3280;});
_temp3279;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3278;}); _temp3277;}); break; case 388:
_LL3276: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3282=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3282[ 0]=({ struct Cyc_Exp_tok_struct
_temp3283; _temp3283.tag= Cyc_Exp_tok; _temp3283.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3284=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3284[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3285; _temp3285.tag= Cyc_Absyn_CompoundLit_e;
_temp3285.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp3285.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp3285;});
_temp3284;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3283;}); _temp3282;}); break; case 389:
_LL3281: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3287=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3287[ 0]=({ struct Cyc_Exp_tok_struct
_temp3288; _temp3288.tag= Cyc_Exp_tok; _temp3288.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3289=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3289[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3290; _temp3290.tag= Cyc_Absyn_Fill_e; _temp3290.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3290;});
_temp3289;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3288;}); _temp3287;}); break; case 390:
_LL3286: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3292=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3292[ 0]=({ struct Cyc_Exp_tok_struct
_temp3293; _temp3293.tag= Cyc_Exp_tok; _temp3293.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3294=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3294[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3295; _temp3295.tag= Cyc_Absyn_Codegen_e;
_temp3295.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp3295;}); _temp3294;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3293;}); _temp3292;}); break; case 391: _LL3291: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3297=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3297[ 0]=({ struct Cyc_Exp_tok_struct
_temp3298; _temp3298.tag= Cyc_Exp_tok; _temp3298.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3299=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3299[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3300; _temp3300.tag= Cyc_Absyn_UnknownId_e;
_temp3300.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3300;}); _temp3299;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3298;});
_temp3297;}); break; case 392: _LL3296: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 393: _LL3301:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3303=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3303[ 0]=({ struct Cyc_Exp_tok_struct
_temp3304; _temp3304.tag= Cyc_Exp_tok; _temp3304.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3304;});
_temp3303;}); break; case 394: _LL3302: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 395:
_LL3305: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3307=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3307[ 0]=({ struct Cyc_Exp_tok_struct
_temp3308; _temp3308.tag= Cyc_Exp_tok; _temp3308.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3309=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3309[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3310; _temp3310.tag=
Cyc_Absyn_UnknownId_e; _temp3310.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3310;});
_temp3309;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3308;});
_temp3307;}); break; case 396: _LL3306: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3312=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3312[ 0]=({ struct Cyc_Exp_tok_struct _temp3313; _temp3313.tag= Cyc_Exp_tok;
_temp3313.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3314=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3314[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3315; _temp3315.tag=
Cyc_Absyn_UnknownId_e; _temp3315.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp3315;});
_temp3314;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).last_line)),(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3313;}); _temp3312;}); break; case 397: _LL3311: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3317=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3317[ 0]=({ struct Cyc_Exp_tok_struct
_temp3318; _temp3318.tag= Cyc_Exp_tok; _temp3318.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3318;}); _temp3317;}); break; case 398: _LL3316: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3320=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3320[ 0]=({ struct Cyc_Exp_tok_struct
_temp3321; _temp3321.tag= Cyc_Exp_tok; _temp3321.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3322=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3322[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3323; _temp3323.tag= Cyc_Absyn_Struct_e;
_temp3323.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp3323.f2= 0; _temp3323.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3323.f4= 0;
_temp3323;}); _temp3322;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3321;});
_temp3320;}); break; case 399: _LL3319: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3325=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3325[ 0]=({ struct Cyc_Exp_tok_struct _temp3326; _temp3326.tag= Cyc_Exp_tok;
_temp3326.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3326;}); _temp3325;}); break; case 400: _LL3324: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3328=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3328[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3329; _temp3329.tag= Cyc_ExpList_tok; _temp3329.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3329;});
_temp3328;}); break; case 401: _LL3327: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3331=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3331[ 0]=({ struct Cyc_ExpList_tok_struct _temp3332; _temp3332.tag= Cyc_ExpList_tok;
_temp3332.f1=({ struct Cyc_List_List* _temp3333=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3333->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3333->tl= 0;
_temp3333;}); _temp3332;}); _temp3331;}); break; case 402: _LL3330: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3335=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3335[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3336; _temp3336.tag= Cyc_ExpList_tok; _temp3336.f1=({ struct Cyc_List_List*
_temp3337=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3337->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3337->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp3337;});
_temp3336;}); _temp3335;}); break; case 403: _LL3334: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3339=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3339[ 0]=({ struct Cyc_Exp_tok_struct
_temp3340; _temp3340.tag= Cyc_Exp_tok; _temp3340.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3340;});
_temp3339;}); break; case 404: _LL3338: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3342=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3342[ 0]=({ struct Cyc_Exp_tok_struct _temp3343; _temp3343.tag= Cyc_Exp_tok;
_temp3343.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3343;});
_temp3342;}); break; case 405: _LL3341: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3345=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3345[ 0]=({ struct Cyc_Exp_tok_struct _temp3346; _temp3346.tag= Cyc_Exp_tok;
_temp3346.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3346;});
_temp3345;}); break; case 406: _LL3344: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3348=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3348[ 0]=({ struct Cyc_Exp_tok_struct _temp3349; _temp3349.tag= Cyc_Exp_tok;
_temp3349.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3349;});
_temp3348;}); break; case 407: _LL3347: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3351=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3351[ 0]=({ struct Cyc_QualId_tok_struct _temp3352; _temp3352.tag= Cyc_QualId_tok;
_temp3352.f1=({ struct _tuple1* _temp3353=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3353->f1= Cyc_Absyn_rel_ns_null; _temp3353->f2=({ struct
_tagged_arr* _temp3354=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3354[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3354;}); _temp3353;});
_temp3352;}); _temp3351;}); break; case 408: _LL3350: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3355:(
int) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen; yyssp_offset -=
yylen; yylsp_offset -= yylen; yyvs[ _check_known_subscript_notnull( 10000u, ++
yyvsp_offset)]= yyval; yylsp_offset ++; if( yylen == 0){( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line= Cyc_yylloc.first_line;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_column= Cyc_yylloc.first_column;(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line;( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_column=( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_column;( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).text= _tag_arr("",
sizeof( unsigned char), 1u);} else{( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line=( yyls[ _check_known_subscript_notnull( 10000u,(
yylsp_offset + yylen) - 1)]).last_line;( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_column=( yyls[ _check_known_subscript_notnull(
10000u,( yylsp_offset + yylen) - 1)]).last_column;} yyn=( int) Cyc_yyr1[
_check_known_subscript_notnull( 409u, yyn)]; yystate= Cyc_yypgoto[
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
unsigned int _temp3357=( unsigned int)( sze + 15); unsigned char* _temp3358=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3357); struct
_tagged_arr _temp3360= _tag_arr( _temp3358, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3359= _temp3357; unsigned int i;
for( i= 0; i < _temp3359; i ++){ _temp3358[ i]='\000';}}; _temp3360;}); Cyc_String_strcpy(
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
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3370= v; struct
_tuple16* _temp3390; struct _tuple16 _temp3392; int _temp3393; void* _temp3395;
unsigned char _temp3397; short _temp3399; struct _tagged_arr _temp3401; struct
Cyc_Core_Opt* _temp3403; struct Cyc_Core_Opt* _temp3405; struct Cyc_Core_Opt
_temp3407; struct _tagged_arr* _temp3408; struct _tuple1* _temp3410; struct
_tuple1 _temp3412; struct _tagged_arr* _temp3413; void* _temp3415; _LL3372: if(
_temp3370 == Cyc_Okay_tok){ goto _LL3373;} else{ goto _LL3374;} _LL3374: if(*((
void**) _temp3370) == Cyc_Int_tok){ _LL3391: _temp3390=(( struct Cyc_Int_tok_struct*)
_temp3370)->f1; _temp3392=* _temp3390; _LL3396: _temp3395= _temp3392.f1; goto
_LL3394; _LL3394: _temp3393= _temp3392.f2; goto _LL3375;} else{ goto _LL3376;}
_LL3376: if(*(( void**) _temp3370) == Cyc_Char_tok){ _LL3398: _temp3397=((
struct Cyc_Char_tok_struct*) _temp3370)->f1; goto _LL3377;} else{ goto _LL3378;}
_LL3378: if(*(( void**) _temp3370) == Cyc_Short_tok){ _LL3400: _temp3399=((
struct Cyc_Short_tok_struct*) _temp3370)->f1; goto _LL3379;} else{ goto _LL3380;}
_LL3380: if(*(( void**) _temp3370) == Cyc_String_tok){ _LL3402: _temp3401=((
struct Cyc_String_tok_struct*) _temp3370)->f1; goto _LL3381;} else{ goto _LL3382;}
_LL3382: if(*(( void**) _temp3370) == Cyc_Stringopt_tok){ _LL3404: _temp3403=((
struct Cyc_Stringopt_tok_struct*) _temp3370)->f1; if( _temp3403 == 0){ goto
_LL3383;} else{ goto _LL3384;}} else{ goto _LL3384;} _LL3384: if(*(( void**)
_temp3370) == Cyc_Stringopt_tok){ _LL3406: _temp3405=(( struct Cyc_Stringopt_tok_struct*)
_temp3370)->f1; if( _temp3405 == 0){ goto _LL3386;} else{ _temp3407=* _temp3405;
_LL3409: _temp3408=( struct _tagged_arr*) _temp3407.v; goto _LL3385;}} else{
goto _LL3386;} _LL3386: if(*(( void**) _temp3370) == Cyc_QualId_tok){ _LL3411:
_temp3410=(( struct Cyc_QualId_tok_struct*) _temp3370)->f1; _temp3412=*
_temp3410; _LL3416: _temp3415= _temp3412.f1; goto _LL3414; _LL3414: _temp3413=
_temp3412.f2; goto _LL3387;} else{ goto _LL3388;} _LL3388: goto _LL3389; _LL3373:
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"ok"); goto _LL3371; _LL3375: fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"%d", _temp3393); goto _LL3371; _LL3377:
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%c",( int) _temp3397); goto _LL3371;
_LL3379: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%ds",( int) _temp3399);
goto _LL3371; _LL3381:({ struct _tagged_arr _temp3417= _temp3401; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _get_arr_size( _temp3417, 1u),
_temp3417.curr);}); goto _LL3371; _LL3383: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"null");
goto _LL3371; _LL3385:({ struct _tagged_arr _temp3418=* _temp3408; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _get_arr_size( _temp3418, 1u),
_temp3418.curr);}); goto _LL3371; _LL3387: { struct Cyc_List_List* _temp3419= 0;{
void* _temp3420= _temp3415; struct Cyc_List_List* _temp3428; struct Cyc_List_List*
_temp3430; _LL3422: if(( unsigned int) _temp3420 > 1u?*(( int*) _temp3420) ==
Cyc_Absyn_Rel_n: 0){ _LL3429: _temp3428=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3420)->f1; goto _LL3423;} else{ goto _LL3424;} _LL3424: if(( unsigned int)
_temp3420 > 1u?*(( int*) _temp3420) == Cyc_Absyn_Abs_n: 0){ _LL3431: _temp3430=((
struct Cyc_Absyn_Abs_n_struct*) _temp3420)->f1; goto _LL3425;} else{ goto
_LL3426;} _LL3426: if( _temp3420 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3427;}
else{ goto _LL3421;} _LL3423: _temp3419= _temp3428; goto _LL3421; _LL3425:
_temp3419= _temp3430; goto _LL3421; _LL3427: goto _LL3421; _LL3421:;} for( 0;
_temp3419 != 0; _temp3419=(( struct Cyc_List_List*) _check_null( _temp3419))->tl){({
struct _tagged_arr _temp3432=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp3419))->hd); fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s::",
_get_arr_size( _temp3432, 1u), _temp3432.curr);});}({ struct _tagged_arr
_temp3433=* _temp3413; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s::",
_get_arr_size( _temp3433, 1u), _temp3433.curr);}); goto _LL3371;} _LL3389:
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"?"); goto _LL3371; _LL3371:;} struct
Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result=
0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3434=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3434->v=( void*) Cyc_Lexing_from_file(
f); _temp3434;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}