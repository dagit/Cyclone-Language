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
} ; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1278;
_temp1278.timestamp= 0; _temp1278.first_line= 0; _temp1278.first_column= 0;
_temp1278.last_line= 0; _temp1278.last_column= 0; _temp1278;});} struct Cyc_Yyltype
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
_temp1279[ 2u]="$"; static unsigned char _temp1280[ 6u]="error"; static
unsigned char _temp1281[ 12u]="$undefined."; static unsigned char _temp1282[ 5u]="AUTO";
static unsigned char _temp1283[ 9u]="REGISTER"; static unsigned char _temp1284[
7u]="STATIC"; static unsigned char _temp1285[ 7u]="EXTERN"; static unsigned char
_temp1286[ 8u]="TYPEDEF"; static unsigned char _temp1287[ 5u]="VOID"; static
unsigned char _temp1288[ 5u]="CHAR"; static unsigned char _temp1289[ 6u]="SHORT";
static unsigned char _temp1290[ 4u]="INT"; static unsigned char _temp1291[ 5u]="LONG";
static unsigned char _temp1292[ 6u]="FLOAT"; static unsigned char _temp1293[ 7u]="DOUBLE";
static unsigned char _temp1294[ 7u]="SIGNED"; static unsigned char _temp1295[ 9u]="UNSIGNED";
static unsigned char _temp1296[ 6u]="CONST"; static unsigned char _temp1297[ 9u]="VOLATILE";
static unsigned char _temp1298[ 9u]="RESTRICT"; static unsigned char _temp1299[
7u]="STRUCT"; static unsigned char _temp1300[ 6u]="UNION"; static unsigned char
_temp1301[ 5u]="CASE"; static unsigned char _temp1302[ 8u]="DEFAULT"; static
unsigned char _temp1303[ 7u]="INLINE"; static unsigned char _temp1304[ 7u]="SIZEOF";
static unsigned char _temp1305[ 9u]="OFFSETOF"; static unsigned char _temp1306[
3u]="IF"; static unsigned char _temp1307[ 5u]="ELSE"; static unsigned char
_temp1308[ 7u]="SWITCH"; static unsigned char _temp1309[ 6u]="WHILE"; static
unsigned char _temp1310[ 3u]="DO"; static unsigned char _temp1311[ 4u]="FOR";
static unsigned char _temp1312[ 5u]="GOTO"; static unsigned char _temp1313[ 9u]="CONTINUE";
static unsigned char _temp1314[ 6u]="BREAK"; static unsigned char _temp1315[ 7u]="RETURN";
static unsigned char _temp1316[ 5u]="ENUM"; static unsigned char _temp1317[ 8u]="NULL_kw";
static unsigned char _temp1318[ 4u]="LET"; static unsigned char _temp1319[ 6u]="THROW";
static unsigned char _temp1320[ 4u]="TRY"; static unsigned char _temp1321[ 6u]="CATCH";
static unsigned char _temp1322[ 4u]="NEW"; static unsigned char _temp1323[ 9u]="ABSTRACT";
static unsigned char _temp1324[ 9u]="FALLTHRU"; static unsigned char _temp1325[
6u]="USING"; static unsigned char _temp1326[ 10u]="NAMESPACE"; static
unsigned char _temp1327[ 7u]="TUNION"; static unsigned char _temp1328[ 8u]="XTUNION";
static unsigned char _temp1329[ 5u]="FILL"; static unsigned char _temp1330[ 8u]="CODEGEN";
static unsigned char _temp1331[ 4u]="CUT"; static unsigned char _temp1332[ 7u]="SPLICE";
static unsigned char _temp1333[ 7u]="PRINTF"; static unsigned char _temp1334[ 8u]="FPRINTF";
static unsigned char _temp1335[ 8u]="XPRINTF"; static unsigned char _temp1336[ 6u]="SCANF";
static unsigned char _temp1337[ 7u]="FSCANF"; static unsigned char _temp1338[ 7u]="SSCANF";
static unsigned char _temp1339[ 7u]="MALLOC"; static unsigned char _temp1340[ 9u]="REGION_T";
static unsigned char _temp1341[ 7u]="REGION"; static unsigned char _temp1342[ 5u]="RNEW";
static unsigned char _temp1343[ 8u]="RMALLOC"; static unsigned char _temp1344[ 7u]="PTR_OP";
static unsigned char _temp1345[ 7u]="INC_OP"; static unsigned char _temp1346[ 7u]="DEC_OP";
static unsigned char _temp1347[ 8u]="LEFT_OP"; static unsigned char _temp1348[ 9u]="RIGHT_OP";
static unsigned char _temp1349[ 6u]="LE_OP"; static unsigned char _temp1350[ 6u]="GE_OP";
static unsigned char _temp1351[ 6u]="EQ_OP"; static unsigned char _temp1352[ 6u]="NE_OP";
static unsigned char _temp1353[ 7u]="AND_OP"; static unsigned char _temp1354[ 6u]="OR_OP";
static unsigned char _temp1355[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1356[ 11u]="DIV_ASSIGN"; static unsigned char _temp1357[ 11u]="MOD_ASSIGN";
static unsigned char _temp1358[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1359[ 11u]="SUB_ASSIGN"; static unsigned char _temp1360[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1361[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1362[ 11u]="AND_ASSIGN"; static unsigned char _temp1363[ 11u]="XOR_ASSIGN";
static unsigned char _temp1364[ 10u]="OR_ASSIGN"; static unsigned char _temp1365[
9u]="ELLIPSIS"; static unsigned char _temp1366[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1367[ 12u]="COLON_COLON"; static unsigned char _temp1368[ 11u]="IDENTIFIER";
static unsigned char _temp1369[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1370[ 7u]="STRING"; static unsigned char _temp1371[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1372[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1373[ 9u]="TYPE_VAR"; static unsigned char _temp1374[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1375[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1376[ 10u]="ATTRIBUTE"; static unsigned char _temp1377[ 4u]="';'"; static
unsigned char _temp1378[ 4u]="'{'"; static unsigned char _temp1379[ 4u]="'}'";
static unsigned char _temp1380[ 4u]="'='"; static unsigned char _temp1381[ 4u]="'('";
static unsigned char _temp1382[ 4u]="')'"; static unsigned char _temp1383[ 4u]="','";
static unsigned char _temp1384[ 4u]="'_'"; static unsigned char _temp1385[ 4u]="'$'";
static unsigned char _temp1386[ 4u]="'<'"; static unsigned char _temp1387[ 4u]="'>'";
static unsigned char _temp1388[ 4u]="':'"; static unsigned char _temp1389[ 4u]="'.'";
static unsigned char _temp1390[ 4u]="'['"; static unsigned char _temp1391[ 4u]="']'";
static unsigned char _temp1392[ 4u]="'*'"; static unsigned char _temp1393[ 4u]="'@'";
static unsigned char _temp1394[ 4u]="'?'"; static unsigned char _temp1395[ 4u]="'+'";
static unsigned char _temp1396[ 4u]="'-'"; static unsigned char _temp1397[ 4u]="'&'";
static unsigned char _temp1398[ 4u]="'|'"; static unsigned char _temp1399[ 4u]="'^'";
static unsigned char _temp1400[ 4u]="'/'"; static unsigned char _temp1401[ 4u]="'%'";
static unsigned char _temp1402[ 4u]="'~'"; static unsigned char _temp1403[ 4u]="'!'";
static unsigned char _temp1404[ 5u]="prog"; static unsigned char _temp1405[ 17u]="translation_unit";
static unsigned char _temp1406[ 21u]="external_declaration"; static
unsigned char _temp1407[ 20u]="function_definition"; static unsigned char
_temp1408[ 21u]="function_definition2"; static unsigned char _temp1409[ 13u]="using_action";
static unsigned char _temp1410[ 15u]="unusing_action"; static unsigned char
_temp1411[ 17u]="namespace_action"; static unsigned char _temp1412[ 19u]="unnamespace_action";
static unsigned char _temp1413[ 12u]="declaration"; static unsigned char
_temp1414[ 17u]="declaration_list"; static unsigned char _temp1415[ 23u]="declaration_specifiers";
static unsigned char _temp1416[ 24u]="storage_class_specifier"; static
unsigned char _temp1417[ 15u]="attributes_opt"; static unsigned char _temp1418[
11u]="attributes"; static unsigned char _temp1419[ 15u]="attribute_list"; static
unsigned char _temp1420[ 10u]="attribute"; static unsigned char _temp1421[ 15u]="type_specifier";
static unsigned char _temp1422[ 5u]="kind"; static unsigned char _temp1423[ 15u]="type_qualifier";
static unsigned char _temp1424[ 15u]="enum_specifier"; static unsigned char
_temp1425[ 11u]="enum_field"; static unsigned char _temp1426[ 22u]="enum_declaration_list";
static unsigned char _temp1427[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1428[ 16u]="type_params_opt"; static unsigned char _temp1429[
16u]="struct_or_union"; static unsigned char _temp1430[ 24u]="struct_declaration_list";
static unsigned char _temp1431[ 25u]="struct_declaration_list0"; static
unsigned char _temp1432[ 21u]="init_declarator_list"; static unsigned char
_temp1433[ 22u]="init_declarator_list0"; static unsigned char _temp1434[ 16u]="init_declarator";
static unsigned char _temp1435[ 19u]="struct_declaration"; static unsigned char
_temp1436[ 25u]="specifier_qualifier_list"; static unsigned char _temp1437[ 23u]="struct_declarator_list";
static unsigned char _temp1438[ 24u]="struct_declarator_list0"; static
unsigned char _temp1439[ 18u]="struct_declarator"; static unsigned char
_temp1440[ 17u]="tunion_specifier"; static unsigned char _temp1441[ 18u]="tunion_or_xtunion";
static unsigned char _temp1442[ 17u]="tunionfield_list"; static unsigned char
_temp1443[ 18u]="tunionfield_scope"; static unsigned char _temp1444[ 12u]="tunionfield";
static unsigned char _temp1445[ 11u]="declarator"; static unsigned char
_temp1446[ 18u]="direct_declarator"; static unsigned char _temp1447[ 8u]="pointer";
static unsigned char _temp1448[ 13u]="pointer_char"; static unsigned char
_temp1449[ 8u]="rgn_opt"; static unsigned char _temp1450[ 4u]="rgn"; static
unsigned char _temp1451[ 20u]="type_qualifier_list"; static unsigned char
_temp1452[ 20u]="parameter_type_list"; static unsigned char _temp1453[ 16u]="optional_effect";
static unsigned char _temp1454[ 19u]="optional_rgn_order"; static unsigned char
_temp1455[ 10u]="rgn_order"; static unsigned char _temp1456[ 16u]="optional_inject";
static unsigned char _temp1457[ 11u]="effect_set"; static unsigned char
_temp1458[ 14u]="atomic_effect"; static unsigned char _temp1459[ 11u]="region_set";
static unsigned char _temp1460[ 15u]="parameter_list"; static unsigned char
_temp1461[ 22u]="parameter_declaration"; static unsigned char _temp1462[ 16u]="identifier_list";
static unsigned char _temp1463[ 17u]="identifier_list0"; static unsigned char
_temp1464[ 12u]="initializer"; static unsigned char _temp1465[ 18u]="array_initializer";
static unsigned char _temp1466[ 17u]="initializer_list"; static unsigned char
_temp1467[ 12u]="designation"; static unsigned char _temp1468[ 16u]="designator_list";
static unsigned char _temp1469[ 11u]="designator"; static unsigned char
_temp1470[ 10u]="type_name"; static unsigned char _temp1471[ 14u]="any_type_name";
static unsigned char _temp1472[ 15u]="type_name_list"; static unsigned char
_temp1473[ 20u]="abstract_declarator"; static unsigned char _temp1474[ 27u]="direct_abstract_declarator";
static unsigned char _temp1475[ 10u]="statement"; static unsigned char _temp1476[
18u]="labeled_statement"; static unsigned char _temp1477[ 21u]="expression_statement";
static unsigned char _temp1478[ 19u]="compound_statement"; static unsigned char
_temp1479[ 16u]="block_item_list"; static unsigned char _temp1480[ 11u]="block_item";
static unsigned char _temp1481[ 20u]="selection_statement"; static unsigned char
_temp1482[ 15u]="switch_clauses"; static unsigned char _temp1483[ 16u]="switchC_clauses";
static unsigned char _temp1484[ 20u]="iteration_statement"; static unsigned char
_temp1485[ 15u]="jump_statement"; static unsigned char _temp1486[ 8u]="pattern";
static unsigned char _temp1487[ 19u]="tuple_pattern_list"; static unsigned char
_temp1488[ 20u]="tuple_pattern_list0"; static unsigned char _temp1489[ 14u]="field_pattern";
static unsigned char _temp1490[ 19u]="field_pattern_list"; static unsigned char
_temp1491[ 20u]="field_pattern_list0"; static unsigned char _temp1492[ 11u]="expression";
static unsigned char _temp1493[ 22u]="assignment_expression"; static
unsigned char _temp1494[ 20u]="assignment_operator"; static unsigned char
_temp1495[ 23u]="conditional_expression"; static unsigned char _temp1496[ 20u]="constant_expression";
static unsigned char _temp1497[ 22u]="logical_or_expression"; static
unsigned char _temp1498[ 23u]="logical_and_expression"; static unsigned char
_temp1499[ 24u]="inclusive_or_expression"; static unsigned char _temp1500[ 24u]="exclusive_or_expression";
static unsigned char _temp1501[ 15u]="and_expression"; static unsigned char
_temp1502[ 20u]="equality_expression"; static unsigned char _temp1503[ 22u]="relational_expression";
static unsigned char _temp1504[ 17u]="shift_expression"; static unsigned char
_temp1505[ 20u]="additive_expression"; static unsigned char _temp1506[ 26u]="multiplicative_expression";
static unsigned char _temp1507[ 16u]="cast_expression"; static unsigned char
_temp1508[ 17u]="unary_expression"; static unsigned char _temp1509[ 14u]="format_primop";
static unsigned char _temp1510[ 15u]="unary_operator"; static unsigned char
_temp1511[ 19u]="postfix_expression"; static unsigned char _temp1512[ 19u]="primary_expression";
static unsigned char _temp1513[ 25u]="argument_expression_list"; static
unsigned char _temp1514[ 26u]="argument_expression_list0"; static unsigned char
_temp1515[ 9u]="constant"; static unsigned char _temp1516[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 238u]={{ _temp1279, _temp1279, _temp1279
+ 2u},{ _temp1280, _temp1280, _temp1280 + 6u},{ _temp1281, _temp1281, _temp1281
+ 12u},{ _temp1282, _temp1282, _temp1282 + 5u},{ _temp1283, _temp1283, _temp1283
+ 9u},{ _temp1284, _temp1284, _temp1284 + 7u},{ _temp1285, _temp1285, _temp1285
+ 7u},{ _temp1286, _temp1286, _temp1286 + 8u},{ _temp1287, _temp1287, _temp1287
+ 5u},{ _temp1288, _temp1288, _temp1288 + 5u},{ _temp1289, _temp1289, _temp1289
+ 6u},{ _temp1290, _temp1290, _temp1290 + 4u},{ _temp1291, _temp1291, _temp1291
+ 5u},{ _temp1292, _temp1292, _temp1292 + 6u},{ _temp1293, _temp1293, _temp1293
+ 7u},{ _temp1294, _temp1294, _temp1294 + 7u},{ _temp1295, _temp1295, _temp1295
+ 9u},{ _temp1296, _temp1296, _temp1296 + 6u},{ _temp1297, _temp1297, _temp1297
+ 9u},{ _temp1298, _temp1298, _temp1298 + 9u},{ _temp1299, _temp1299, _temp1299
+ 7u},{ _temp1300, _temp1300, _temp1300 + 6u},{ _temp1301, _temp1301, _temp1301
+ 5u},{ _temp1302, _temp1302, _temp1302 + 8u},{ _temp1303, _temp1303, _temp1303
+ 7u},{ _temp1304, _temp1304, _temp1304 + 7u},{ _temp1305, _temp1305, _temp1305
+ 9u},{ _temp1306, _temp1306, _temp1306 + 3u},{ _temp1307, _temp1307, _temp1307
+ 5u},{ _temp1308, _temp1308, _temp1308 + 7u},{ _temp1309, _temp1309, _temp1309
+ 6u},{ _temp1310, _temp1310, _temp1310 + 3u},{ _temp1311, _temp1311, _temp1311
+ 4u},{ _temp1312, _temp1312, _temp1312 + 5u},{ _temp1313, _temp1313, _temp1313
+ 9u},{ _temp1314, _temp1314, _temp1314 + 6u},{ _temp1315, _temp1315, _temp1315
+ 7u},{ _temp1316, _temp1316, _temp1316 + 5u},{ _temp1317, _temp1317, _temp1317
+ 8u},{ _temp1318, _temp1318, _temp1318 + 4u},{ _temp1319, _temp1319, _temp1319
+ 6u},{ _temp1320, _temp1320, _temp1320 + 4u},{ _temp1321, _temp1321, _temp1321
+ 6u},{ _temp1322, _temp1322, _temp1322 + 4u},{ _temp1323, _temp1323, _temp1323
+ 9u},{ _temp1324, _temp1324, _temp1324 + 9u},{ _temp1325, _temp1325, _temp1325
+ 6u},{ _temp1326, _temp1326, _temp1326 + 10u},{ _temp1327, _temp1327, _temp1327
+ 7u},{ _temp1328, _temp1328, _temp1328 + 8u},{ _temp1329, _temp1329, _temp1329
+ 5u},{ _temp1330, _temp1330, _temp1330 + 8u},{ _temp1331, _temp1331, _temp1331
+ 4u},{ _temp1332, _temp1332, _temp1332 + 7u},{ _temp1333, _temp1333, _temp1333
+ 7u},{ _temp1334, _temp1334, _temp1334 + 8u},{ _temp1335, _temp1335, _temp1335
+ 8u},{ _temp1336, _temp1336, _temp1336 + 6u},{ _temp1337, _temp1337, _temp1337
+ 7u},{ _temp1338, _temp1338, _temp1338 + 7u},{ _temp1339, _temp1339, _temp1339
+ 7u},{ _temp1340, _temp1340, _temp1340 + 9u},{ _temp1341, _temp1341, _temp1341
+ 7u},{ _temp1342, _temp1342, _temp1342 + 5u},{ _temp1343, _temp1343, _temp1343
+ 8u},{ _temp1344, _temp1344, _temp1344 + 7u},{ _temp1345, _temp1345, _temp1345
+ 7u},{ _temp1346, _temp1346, _temp1346 + 7u},{ _temp1347, _temp1347, _temp1347
+ 8u},{ _temp1348, _temp1348, _temp1348 + 9u},{ _temp1349, _temp1349, _temp1349
+ 6u},{ _temp1350, _temp1350, _temp1350 + 6u},{ _temp1351, _temp1351, _temp1351
+ 6u},{ _temp1352, _temp1352, _temp1352 + 6u},{ _temp1353, _temp1353, _temp1353
+ 7u},{ _temp1354, _temp1354, _temp1354 + 6u},{ _temp1355, _temp1355, _temp1355
+ 11u},{ _temp1356, _temp1356, _temp1356 + 11u},{ _temp1357, _temp1357,
_temp1357 + 11u},{ _temp1358, _temp1358, _temp1358 + 11u},{ _temp1359, _temp1359,
_temp1359 + 11u},{ _temp1360, _temp1360, _temp1360 + 12u},{ _temp1361, _temp1361,
_temp1361 + 13u},{ _temp1362, _temp1362, _temp1362 + 11u},{ _temp1363, _temp1363,
_temp1363 + 11u},{ _temp1364, _temp1364, _temp1364 + 10u},{ _temp1365, _temp1365,
_temp1365 + 9u},{ _temp1366, _temp1366, _temp1366 + 11u},{ _temp1367, _temp1367,
_temp1367 + 12u},{ _temp1368, _temp1368, _temp1368 + 11u},{ _temp1369, _temp1369,
_temp1369 + 17u},{ _temp1370, _temp1370, _temp1370 + 7u},{ _temp1371, _temp1371,
_temp1371 + 19u},{ _temp1372, _temp1372, _temp1372 + 18u},{ _temp1373, _temp1373,
_temp1373 + 9u},{ _temp1374, _temp1374, _temp1374 + 16u},{ _temp1375, _temp1375,
_temp1375 + 18u},{ _temp1376, _temp1376, _temp1376 + 10u},{ _temp1377, _temp1377,
_temp1377 + 4u},{ _temp1378, _temp1378, _temp1378 + 4u},{ _temp1379, _temp1379,
_temp1379 + 4u},{ _temp1380, _temp1380, _temp1380 + 4u},{ _temp1381, _temp1381,
_temp1381 + 4u},{ _temp1382, _temp1382, _temp1382 + 4u},{ _temp1383, _temp1383,
_temp1383 + 4u},{ _temp1384, _temp1384, _temp1384 + 4u},{ _temp1385, _temp1385,
_temp1385 + 4u},{ _temp1386, _temp1386, _temp1386 + 4u},{ _temp1387, _temp1387,
_temp1387 + 4u},{ _temp1388, _temp1388, _temp1388 + 4u},{ _temp1389, _temp1389,
_temp1389 + 4u},{ _temp1390, _temp1390, _temp1390 + 4u},{ _temp1391, _temp1391,
_temp1391 + 4u},{ _temp1392, _temp1392, _temp1392 + 4u},{ _temp1393, _temp1393,
_temp1393 + 4u},{ _temp1394, _temp1394, _temp1394 + 4u},{ _temp1395, _temp1395,
_temp1395 + 4u},{ _temp1396, _temp1396, _temp1396 + 4u},{ _temp1397, _temp1397,
_temp1397 + 4u},{ _temp1398, _temp1398, _temp1398 + 4u},{ _temp1399, _temp1399,
_temp1399 + 4u},{ _temp1400, _temp1400, _temp1400 + 4u},{ _temp1401, _temp1401,
_temp1401 + 4u},{ _temp1402, _temp1402, _temp1402 + 4u},{ _temp1403, _temp1403,
_temp1403 + 4u},{ _temp1404, _temp1404, _temp1404 + 5u},{ _temp1405, _temp1405,
_temp1405 + 17u},{ _temp1406, _temp1406, _temp1406 + 21u},{ _temp1407, _temp1407,
_temp1407 + 20u},{ _temp1408, _temp1408, _temp1408 + 21u},{ _temp1409, _temp1409,
_temp1409 + 13u},{ _temp1410, _temp1410, _temp1410 + 15u},{ _temp1411, _temp1411,
_temp1411 + 17u},{ _temp1412, _temp1412, _temp1412 + 19u},{ _temp1413, _temp1413,
_temp1413 + 12u},{ _temp1414, _temp1414, _temp1414 + 17u},{ _temp1415, _temp1415,
_temp1415 + 23u},{ _temp1416, _temp1416, _temp1416 + 24u},{ _temp1417, _temp1417,
_temp1417 + 15u},{ _temp1418, _temp1418, _temp1418 + 11u},{ _temp1419, _temp1419,
_temp1419 + 15u},{ _temp1420, _temp1420, _temp1420 + 10u},{ _temp1421, _temp1421,
_temp1421 + 15u},{ _temp1422, _temp1422, _temp1422 + 5u},{ _temp1423, _temp1423,
_temp1423 + 15u},{ _temp1424, _temp1424, _temp1424 + 15u},{ _temp1425, _temp1425,
_temp1425 + 11u},{ _temp1426, _temp1426, _temp1426 + 22u},{ _temp1427, _temp1427,
_temp1427 + 26u},{ _temp1428, _temp1428, _temp1428 + 16u},{ _temp1429, _temp1429,
_temp1429 + 16u},{ _temp1430, _temp1430, _temp1430 + 24u},{ _temp1431, _temp1431,
_temp1431 + 25u},{ _temp1432, _temp1432, _temp1432 + 21u},{ _temp1433, _temp1433,
_temp1433 + 22u},{ _temp1434, _temp1434, _temp1434 + 16u},{ _temp1435, _temp1435,
_temp1435 + 19u},{ _temp1436, _temp1436, _temp1436 + 25u},{ _temp1437, _temp1437,
_temp1437 + 23u},{ _temp1438, _temp1438, _temp1438 + 24u},{ _temp1439, _temp1439,
_temp1439 + 18u},{ _temp1440, _temp1440, _temp1440 + 17u},{ _temp1441, _temp1441,
_temp1441 + 18u},{ _temp1442, _temp1442, _temp1442 + 17u},{ _temp1443, _temp1443,
_temp1443 + 18u},{ _temp1444, _temp1444, _temp1444 + 12u},{ _temp1445, _temp1445,
_temp1445 + 11u},{ _temp1446, _temp1446, _temp1446 + 18u},{ _temp1447, _temp1447,
_temp1447 + 8u},{ _temp1448, _temp1448, _temp1448 + 13u},{ _temp1449, _temp1449,
_temp1449 + 8u},{ _temp1450, _temp1450, _temp1450 + 4u},{ _temp1451, _temp1451,
_temp1451 + 20u},{ _temp1452, _temp1452, _temp1452 + 20u},{ _temp1453, _temp1453,
_temp1453 + 16u},{ _temp1454, _temp1454, _temp1454 + 19u},{ _temp1455, _temp1455,
_temp1455 + 10u},{ _temp1456, _temp1456, _temp1456 + 16u},{ _temp1457, _temp1457,
_temp1457 + 11u},{ _temp1458, _temp1458, _temp1458 + 14u},{ _temp1459, _temp1459,
_temp1459 + 11u},{ _temp1460, _temp1460, _temp1460 + 15u},{ _temp1461, _temp1461,
_temp1461 + 22u},{ _temp1462, _temp1462, _temp1462 + 16u},{ _temp1463, _temp1463,
_temp1463 + 17u},{ _temp1464, _temp1464, _temp1464 + 12u},{ _temp1465, _temp1465,
_temp1465 + 18u},{ _temp1466, _temp1466, _temp1466 + 17u},{ _temp1467, _temp1467,
_temp1467 + 12u},{ _temp1468, _temp1468, _temp1468 + 16u},{ _temp1469, _temp1469,
_temp1469 + 11u},{ _temp1470, _temp1470, _temp1470 + 10u},{ _temp1471, _temp1471,
_temp1471 + 14u},{ _temp1472, _temp1472, _temp1472 + 15u},{ _temp1473, _temp1473,
_temp1473 + 20u},{ _temp1474, _temp1474, _temp1474 + 27u},{ _temp1475, _temp1475,
_temp1475 + 10u},{ _temp1476, _temp1476, _temp1476 + 18u},{ _temp1477, _temp1477,
_temp1477 + 21u},{ _temp1478, _temp1478, _temp1478 + 19u},{ _temp1479, _temp1479,
_temp1479 + 16u},{ _temp1480, _temp1480, _temp1480 + 11u},{ _temp1481, _temp1481,
_temp1481 + 20u},{ _temp1482, _temp1482, _temp1482 + 15u},{ _temp1483, _temp1483,
_temp1483 + 16u},{ _temp1484, _temp1484, _temp1484 + 20u},{ _temp1485, _temp1485,
_temp1485 + 15u},{ _temp1486, _temp1486, _temp1486 + 8u},{ _temp1487, _temp1487,
_temp1487 + 19u},{ _temp1488, _temp1488, _temp1488 + 20u},{ _temp1489, _temp1489,
_temp1489 + 14u},{ _temp1490, _temp1490, _temp1490 + 19u},{ _temp1491, _temp1491,
_temp1491 + 20u},{ _temp1492, _temp1492, _temp1492 + 11u},{ _temp1493, _temp1493,
_temp1493 + 22u},{ _temp1494, _temp1494, _temp1494 + 20u},{ _temp1495, _temp1495,
_temp1495 + 23u},{ _temp1496, _temp1496, _temp1496 + 20u},{ _temp1497, _temp1497,
_temp1497 + 22u},{ _temp1498, _temp1498, _temp1498 + 23u},{ _temp1499, _temp1499,
_temp1499 + 24u},{ _temp1500, _temp1500, _temp1500 + 24u},{ _temp1501, _temp1501,
_temp1501 + 15u},{ _temp1502, _temp1502, _temp1502 + 20u},{ _temp1503, _temp1503,
_temp1503 + 22u},{ _temp1504, _temp1504, _temp1504 + 17u},{ _temp1505, _temp1505,
_temp1505 + 20u},{ _temp1506, _temp1506, _temp1506 + 26u},{ _temp1507, _temp1507,
_temp1507 + 16u},{ _temp1508, _temp1508, _temp1508 + 17u},{ _temp1509, _temp1509,
_temp1509 + 14u},{ _temp1510, _temp1510, _temp1510 + 15u},{ _temp1511, _temp1511,
_temp1511 + 19u},{ _temp1512, _temp1512, _temp1512 + 19u},{ _temp1513, _temp1513,
_temp1513 + 25u},{ _temp1514, _temp1514, _temp1514 + 26u},{ _temp1515, _temp1515,
_temp1515 + 9u},{ _temp1516, _temp1516, _temp1516 + 20u}}; static short Cyc_yyr1[
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
yyss=( short*)({ unsigned int _temp3366= 10000u; short* _temp3367=( short*)
GC_malloc_atomic( sizeof( short) * _temp3366);{ unsigned int _temp3368=
_temp3366; unsigned int i; for( i= 0; i < _temp3368; i ++){ _temp3367[ i]=(
short) 0;}}; _temp3367;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3363= 10000u; void** _temp3364=( void**) GC_malloc( sizeof(
void*) * _temp3363);{ unsigned int _temp3365= _temp3363; unsigned int i; for( i=
0; i < _temp3365; i ++){ _temp3364[ i]= Cyc_yylval;}}; _temp3364;}); int
yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({ unsigned int
_temp3360= 10000u; struct Cyc_Yyltype* _temp3361=( struct Cyc_Yyltype*)
GC_malloc_atomic( sizeof( struct Cyc_Yyltype) * _temp3360);{ unsigned int
_temp3362= _temp3360; unsigned int i; for( i= 0; i < _temp3362; i ++){ _temp3361[
i]= Cyc_yynewloc();}}; _temp3361;}); int yystacksize= 10000; void* yyval= Cyc_yylval;
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
yyn){ case 1: _LL1517: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1518:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1520=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1520[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1521; _temp1521.tag= Cyc_DeclList_tok; _temp1521.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1521;});
_temp1520;}); break; case 3: _LL1519: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1523=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1523[ 0]=({ struct Cyc_DeclList_tok_struct _temp1524; _temp1524.tag= Cyc_DeclList_tok;
_temp1524.f1=({ struct Cyc_List_List* _temp1525=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1525->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1526=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1526->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1527=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1527[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1528; _temp1528.tag= Cyc_Absyn_Using_d;
_temp1528.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1528.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1528;}); _temp1527;}));
_temp1526->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1526;});
_temp1525->tl= 0; _temp1525;}); _temp1524;}); _temp1523;}); Cyc_Lex_leave_using();
break; case 4: _LL1522: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1530=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1530[ 0]=({ struct Cyc_DeclList_tok_struct _temp1531; _temp1531.tag= Cyc_DeclList_tok;
_temp1531.f1=({ struct Cyc_List_List* _temp1532=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1532->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1533=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1533->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1534=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1534[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1535; _temp1535.tag= Cyc_Absyn_Using_d;
_temp1535.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp1535.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1535;});
_temp1534;})); _temp1533->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1533;}); _temp1532->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1532;}); _temp1531;});
_temp1530;}); break; case 5: _LL1529: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1537=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1537[ 0]=({ struct Cyc_DeclList_tok_struct _temp1538; _temp1538.tag= Cyc_DeclList_tok;
_temp1538.f1=({ struct Cyc_List_List* _temp1539=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1539->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1540=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1540->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1541=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1541[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1542; _temp1542.tag=
Cyc_Absyn_Namespace_d; _temp1542.f1=({ struct _tagged_arr* _temp1543=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1543[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1543;});
_temp1542.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1542;}); _temp1541;})); _temp1540->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1540;}); _temp1539->tl= 0; _temp1539;}); _temp1538;}); _temp1537;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1536: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1545=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1545[ 0]=({ struct Cyc_DeclList_tok_struct _temp1546; _temp1546.tag= Cyc_DeclList_tok;
_temp1546.f1=({ struct Cyc_List_List* _temp1547=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1547->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1548=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1548->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1549=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1549[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1550; _temp1550.tag=
Cyc_Absyn_Namespace_d; _temp1550.f1=({ struct _tagged_arr* _temp1551=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1551[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1551;});
_temp1550.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1550;}); _temp1549;})); _temp1548->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1548;}); _temp1547->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1547;}); _temp1546;});
_temp1545;}); break; case 7: _LL1544: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1553=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1553[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1554; _temp1554.tag= Cyc_DeclList_tok; _temp1554.f1=({ struct Cyc_List_List*
_temp1555=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1555->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1556=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1556->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1557=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1557[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1558; _temp1558.tag= Cyc_Absyn_ExternC_d;
_temp1558.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1558;}); _temp1557;})); _temp1556->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1556;}); _temp1555->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1555;}); _temp1554;});
_temp1553;}); break; case 8: _LL1552: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1560=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1560[ 0]=({ struct Cyc_DeclList_tok_struct _temp1561; _temp1561.tag= Cyc_DeclList_tok;
_temp1561.f1= 0; _temp1561;}); _temp1560;}); break; case 9: _LL1559: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1563=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1563[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1564; _temp1564.tag= Cyc_DeclList_tok; _temp1564.f1=({ struct Cyc_List_List*
_temp1565=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1565->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1566=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1566[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1567; _temp1567.tag= Cyc_Absyn_Fn_d;
_temp1567.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1567;}); _temp1566;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1565->tl=
0; _temp1565;}); _temp1564;}); _temp1563;}); break; case 10: _LL1562: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1568: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1570=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1570[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1571; _temp1571.tag= Cyc_FnDecl_tok; _temp1571.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1571;});
_temp1570;}); break; case 12: _LL1569: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1573=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1573[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1574; _temp1574.tag= Cyc_FnDecl_tok;
_temp1574.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1575=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1575->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1575;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1574;}); _temp1573;}); break; case 13: _LL1572: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1577=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1577[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1578; _temp1578.tag= Cyc_FnDecl_tok;
_temp1578.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1578;}); _temp1577;}); break; case 14: _LL1576: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1580=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1580[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1581; _temp1581.tag= Cyc_FnDecl_tok;
_temp1581.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1582=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1582->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1582;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1581;}); _temp1580;}); break; case 15: _LL1579: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1584=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1584[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1585; _temp1585.tag= Cyc_FnDecl_tok;
_temp1585.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1586=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1586->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1586;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1585;}); _temp1584;}); break; case 16: _LL1583: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1588=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1588[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1589; _temp1589.tag= Cyc_FnDecl_tok;
_temp1589.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1590=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1590->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1590;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1589;}); _temp1588;}); break; case 17: _LL1587: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1591:
Cyc_Lex_leave_using(); break; case 19: _LL1592: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1594=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp1594[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1594;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1593: Cyc_Lex_leave_namespace(); break; case 21: _LL1595: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1597=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1597[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1598; _temp1598.tag= Cyc_DeclList_tok; _temp1598.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp1598;}); _temp1597;}); break; case 22: _LL1596: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1600=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1600[ 0]=({ struct Cyc_DeclList_tok_struct _temp1601; _temp1601.tag= Cyc_DeclList_tok;
_temp1601.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1601;}); _temp1600;}); break; case 23: _LL1599: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1603=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1603[ 0]=({ struct Cyc_DeclList_tok_struct _temp1604; _temp1604.tag= Cyc_DeclList_tok;
_temp1604.f1=({ struct Cyc_List_List* _temp1605=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1605->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1605->tl=
0; _temp1605;}); _temp1604;}); _temp1603;}); break; case 24: _LL1602: { struct
Cyc_List_List* _temp1607= 0;{ struct Cyc_List_List* _temp1608= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); for( 0;
_temp1608 != 0; _temp1608=(( struct Cyc_List_List*) _check_null( _temp1608))->tl){
struct _tagged_arr* _temp1609=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1608))->hd; struct _tuple1* _temp1610=({ struct _tuple1*
_temp1613=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1613->f1=
Cyc_Absyn_rel_ns_null; _temp1613->f2= _temp1609; _temp1613;}); struct Cyc_Absyn_Vardecl*
_temp1611= Cyc_Absyn_new_vardecl( _temp1610, Cyc_Absyn_wildtyp( 0), 0);
_temp1607=({ struct Cyc_List_List* _temp1612=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1612->hd=( void*) _temp1611; _temp1612->tl=
_temp1607; _temp1612;});}} _temp1607=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1607); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1614=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1614[ 0]=({ struct Cyc_DeclList_tok_struct _temp1615; _temp1615.tag= Cyc_DeclList_tok;
_temp1615.f1=({ struct Cyc_List_List* _temp1616=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1616->hd=( void*) Cyc_Absyn_letv_decl(
_temp1607, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1616->tl= 0; _temp1616;}); _temp1615;});
_temp1614;}); break;} case 25: _LL1606: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1617:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1619=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1619[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1620; _temp1620.tag= Cyc_DeclList_tok; _temp1620.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1620;});
_temp1619;}); break; case 27: _LL1618: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1622=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1622[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1623; _temp1623.tag= Cyc_DeclSpec_tok;
_temp1623.f1=({ struct Cyc_Parse_Declaration_spec* _temp1624=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1624->sc=({ struct
Cyc_Core_Opt* _temp1625=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1625->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1625;});
_temp1624->tq= Cyc_Absyn_empty_tqual(); _temp1624->type_specs= 0; _temp1624->is_inline=
0; _temp1624->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1624;}); _temp1623;});
_temp1622;}); break; case 28: _LL1621: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc != 0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1627=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1627[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1628; _temp1628.tag= Cyc_DeclSpec_tok; _temp1628.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1629=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1629->sc=({ struct Cyc_Core_Opt* _temp1630=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1630->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1630;});
_temp1629->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1629->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1629->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1629->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes); _temp1629;});
_temp1628;}); _temp1627;}); break; case 29: _LL1626: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1632=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1632[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1633; _temp1633.tag= Cyc_DeclSpec_tok;
_temp1633.f1=({ struct Cyc_Parse_Declaration_spec* _temp1634=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1634->sc= 0;
_temp1634->tq= Cyc_Absyn_empty_tqual(); _temp1634->type_specs=({ struct Cyc_List_List*
_temp1635=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1635->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1635->tl= 0;
_temp1635;}); _temp1634->is_inline= 0; _temp1634->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1634;});
_temp1633;}); _temp1632;}); break; case 30: _LL1631: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1637=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1637[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1638; _temp1638.tag= Cyc_DeclSpec_tok;
_temp1638.f1=({ struct Cyc_Parse_Declaration_spec* _temp1639=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1639->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1639->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1639->type_specs=({ struct Cyc_List_List* _temp1640=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1640->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1640->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1640;});
_temp1639->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1639->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes); _temp1639;});
_temp1638;}); _temp1637;}); break; case 31: _LL1636: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1642=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1642[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1643; _temp1643.tag= Cyc_DeclSpec_tok;
_temp1643.f1=({ struct Cyc_Parse_Declaration_spec* _temp1644=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1644->sc= 0;
_temp1644->tq= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1644->type_specs= 0; _temp1644->is_inline= 0;
_temp1644->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1644;}); _temp1643;});
_temp1642;}); break; case 32: _LL1641: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1646=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1646[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1647; _temp1647.tag= Cyc_DeclSpec_tok;
_temp1647.f1=({ struct Cyc_Parse_Declaration_spec* _temp1648=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1648->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1648->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1648->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1648->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1648->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1648;}); _temp1647;}); _temp1646;}); break; case 33: _LL1645: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1650=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1650[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1651; _temp1651.tag= Cyc_DeclSpec_tok; _temp1651.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1652=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1652->sc= 0; _temp1652->tq= Cyc_Absyn_empty_tqual(); _temp1652->type_specs=
0; _temp1652->is_inline= 1; _temp1652->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1652;});
_temp1651;}); _temp1650;}); break; case 34: _LL1649: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1654=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1654[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1655; _temp1655.tag= Cyc_DeclSpec_tok;
_temp1655.f1=({ struct Cyc_Parse_Declaration_spec* _temp1656=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1656->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1656->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1656->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1656->is_inline=
1; _temp1656->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1656;}); _temp1655;}); _temp1654;}); break; case 35: _LL1653: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1658=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1658[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1659; _temp1659.tag= Cyc_StorageClass_tok;
_temp1659.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp1659;}); _temp1658;});
break; case 36: _LL1657: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1661=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1661[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1662; _temp1662.tag=
Cyc_StorageClass_tok; _temp1662.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp1662;}); _temp1661;}); break; case 37: _LL1660: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1664=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1664[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1665; _temp1665.tag=
Cyc_StorageClass_tok; _temp1665.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp1665;}); _temp1664;}); break; case 38: _LL1663: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1667=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1667[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1668; _temp1668.tag=
Cyc_StorageClass_tok; _temp1668.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp1668;}); _temp1667;}); break; case 39: _LL1666: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1670=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1670[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp1671; _temp1671.tag= Cyc_StorageClass_tok;
_temp1671.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp1671;}); _temp1670;});
break; case 40: _LL1669: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1673=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1673[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1674; _temp1674.tag=
Cyc_StorageClass_tok; _temp1674.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp1674;}); _temp1673;}); break; case 41: _LL1672: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1676=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1676[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1677; _temp1677.tag=
Cyc_StorageClass_tok; _temp1677.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp1677;}); _temp1676;}); break; case 42: _LL1675: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1679=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1679[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1680; _temp1680.tag=
Cyc_AttributeList_tok; _temp1680.f1= 0; _temp1680;}); _temp1679;}); break; case
43: _LL1678: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1681: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1683=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1683[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1684; _temp1684.tag=
Cyc_AttributeList_tok; _temp1684.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1684;});
_temp1683;}); break; case 45: _LL1682: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1686=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1686[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1687; _temp1687.tag=
Cyc_AttributeList_tok; _temp1687.f1=({ struct Cyc_List_List* _temp1688=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1688->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1688->tl= 0; _temp1688;}); _temp1687;}); _temp1686;});
break; case 46: _LL1685: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1690=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1690[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1691; _temp1691.tag=
Cyc_AttributeList_tok; _temp1691.f1=({ struct Cyc_List_List* _temp1692=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1692->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1692->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1692;}); _temp1691;});
_temp1690;}); break; case 47: _LL1689: { struct _tagged_arr _temp1694= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1694, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1694, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1694, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1694,
sizeof( unsigned char),( int)( _get_arr_size( _temp1694, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1694,
sizeof( unsigned char),( int)( _get_arr_size( _temp1694, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1694=( struct _tagged_arr) Cyc_String_substring(
_temp1694, 2,( int)( _get_arr_size( _temp1694, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1694,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1695=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1695[ 0]=({ struct Cyc_Attribute_tok_struct _temp1696; _temp1696.tag= Cyc_Attribute_tok;
_temp1696.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1696;}); _temp1695;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1697=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1697[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1698; _temp1698.tag= Cyc_Attribute_tok; _temp1698.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1698;}); _temp1697;});} break;}} case
48: _LL1693: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1700=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1700[ 0]=({ struct Cyc_Attribute_tok_struct _temp1701; _temp1701.tag= Cyc_Attribute_tok;
_temp1701.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1701;}); _temp1700;});
break; case 49: _LL1699: { struct _tagged_arr _temp1703= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct
_tuple16 _temp1706; int _temp1707; void* _temp1709; struct _tuple16* _temp1704=
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp1706=* _temp1704; _LL1710: _temp1709= _temp1706.f1; goto _LL1708;
_LL1708: _temp1707= _temp1706.f2; goto _LL1705; _LL1705: { void* a; if( Cyc_String_zstrcmp(
_temp1703, _tag_arr("regparm", sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp(
_temp1703, _tag_arr("__regparm__", sizeof( unsigned char), 12u)) == 0){ if(
_temp1707 <= 0? 1: _temp1707 > 3){ Cyc_Parse_err( _tag_arr("regparm requires value between 1 and 3",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp1711=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp1711[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp1712; _temp1712.tag= Cyc_Absyn_Regparm_att;
_temp1712.f1= _temp1707; _temp1712;}); _temp1711;});} else{ if( Cyc_String_zstrcmp(
_temp1703, _tag_arr("aligned", sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp(
_temp1703, _tag_arr("__aligned__", sizeof( unsigned char), 12u)) == 0){ if(
_temp1707 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
unsigned int j=( unsigned int) _temp1707; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1713=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1713[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1714; _temp1714.tag= Cyc_Absyn_Aligned_att;
_temp1714.f1= _temp1707; _temp1714;}); _temp1713;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1715=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1715[ 0]=({ struct Cyc_Attribute_tok_struct _temp1716; _temp1716.tag= Cyc_Attribute_tok;
_temp1716.f1=( void*) a; _temp1716;}); _temp1715;}); break;}} case 50: _LL1702: {
struct _tagged_arr _temp1718= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct _tagged_arr
_temp1719= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); void* a; if( Cyc_String_zstrcmp( _temp1718, _tag_arr("section",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1718, _tag_arr("__section__",
sizeof( unsigned char), 12u))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1720=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1720[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1721; _temp1721.tag=
Cyc_Absyn_Section_att; _temp1721.f1= _temp1719; _temp1721;}); _temp1720;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1722=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1722[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1723; _temp1723.tag= Cyc_Attribute_tok; _temp1723.f1=(
void*) a; _temp1723;}); _temp1722;}); break;} case 51: _LL1717: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1725=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1725[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1726; _temp1726.tag= Cyc_TypeSpecifier_tok;
_temp1726.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1726;});
_temp1725;}); break; case 52: _LL1724: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1728=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1728[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1729; _temp1729.tag=
Cyc_TypeSpecifier_tok; _temp1729.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1729;}); _temp1728;}); break; case 53: _LL1727:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1731=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1731[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1732; _temp1732.tag= Cyc_TypeSpecifier_tok;
_temp1732.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1732;});
_temp1731;}); break; case 54: _LL1730: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1734=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1734[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1735; _temp1735.tag=
Cyc_TypeSpecifier_tok; _temp1735.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1736=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1736[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1737; _temp1737.tag=
Cyc_Parse_Short_spec; _temp1737.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1737;});
_temp1736;})); _temp1735;}); _temp1734;}); break; case 55: _LL1733: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1739=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1739[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1740; _temp1740.tag= Cyc_TypeSpecifier_tok;
_temp1740.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1740;});
_temp1739;}); break; case 56: _LL1738: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1742=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1742[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1743; _temp1743.tag=
Cyc_TypeSpecifier_tok; _temp1743.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1744=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1744[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1745; _temp1745.tag= Cyc_Parse_Long_spec;
_temp1745.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1745;}); _temp1744;})); _temp1743;}); _temp1742;});
break; case 57: _LL1741: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1747=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1747[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1748; _temp1748.tag=
Cyc_TypeSpecifier_tok; _temp1748.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1748;}); _temp1747;}); break; case 58: _LL1746:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1750=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1750[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1751; _temp1751.tag= Cyc_TypeSpecifier_tok;
_temp1751.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1751;});
_temp1750;}); break; case 59: _LL1749: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1753=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1753[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1754; _temp1754.tag=
Cyc_TypeSpecifier_tok; _temp1754.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1755=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1755[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1756; _temp1756.tag=
Cyc_Parse_Signed_spec; _temp1756.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1756;});
_temp1755;})); _temp1754;}); _temp1753;}); break; case 60: _LL1752: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1758=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1758[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1759; _temp1759.tag= Cyc_TypeSpecifier_tok;
_temp1759.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1760=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1760[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1761; _temp1761.tag= Cyc_Parse_Unsigned_spec; _temp1761.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1761;});
_temp1760;})); _temp1759;}); _temp1758;}); break; case 61: _LL1757: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 62: _LL1762:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
63: _LL1763: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 64: _LL1764: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1766=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1766[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1767; _temp1767.tag=
Cyc_TypeSpecifier_tok; _temp1767.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1767;});
_temp1766;}); break; case 65: _LL1765: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1769=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1769[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1770; _temp1770.tag=
Cyc_TypeSpecifier_tok; _temp1770.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1770;}); _temp1769;}); break; case 66: _LL1768: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1772=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1772[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1773; _temp1773.tag=
Cyc_TypeSpecifier_tok; _temp1773.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1774=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1774[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1775; _temp1775.tag= Cyc_Absyn_TypedefType;
_temp1775.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1775.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1775.f3= 0;
_temp1775;}); _temp1774;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1773;});
_temp1772;}); break; case 67: _LL1771: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1777=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1777[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1778; _temp1778.tag=
Cyc_TypeSpecifier_tok; _temp1778.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1779=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1779[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1780; _temp1780.tag= Cyc_Absyn_TupleType;
_temp1780.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); _temp1780;});
_temp1779;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1778;}); _temp1777;}); break; case 68:
_LL1776: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1782=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1782[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1783; _temp1783.tag=
Cyc_TypeSpecifier_tok; _temp1783.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1784=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1784[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1785; _temp1785.tag= Cyc_Absyn_RgnHandleType;
_temp1785.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1785;}); _temp1784;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1783;}); _temp1782;}); break; case 69: _LL1781: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1787=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1787[ 0]=({ struct Cyc_Kind_tok_struct _temp1788; _temp1788.tag= Cyc_Kind_tok;
_temp1788.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1788;});
_temp1787;}); break; case 70: _LL1786: { struct _tagged_arr* _temp1792; void*
_temp1794; struct _tuple1 _temp1790=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1795: _temp1794=
_temp1790.f1; goto _LL1793; _LL1793: _temp1792= _temp1790.f2; goto _LL1791;
_LL1791: if( _temp1794 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1796=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1796[ 0]=({ struct Cyc_Kind_tok_struct
_temp1797; _temp1797.tag= Cyc_Kind_tok; _temp1797.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1792, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1797;}); _temp1796;}); break;} case 71:
_LL1789: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1799=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1799[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1800; _temp1800.tag= Cyc_TypeQual_tok; _temp1800.f1=({ struct Cyc_Absyn_Tqual
_temp1801; _temp1801.q_const= 1; _temp1801.q_volatile= 0; _temp1801.q_restrict=
0; _temp1801;}); _temp1800;}); _temp1799;}); break; case 72: _LL1798: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1803=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1803[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1804; _temp1804.tag= Cyc_TypeQual_tok; _temp1804.f1=({ struct Cyc_Absyn_Tqual
_temp1805; _temp1805.q_const= 0; _temp1805.q_volatile= 1; _temp1805.q_restrict=
0; _temp1805;}); _temp1804;}); _temp1803;}); break; case 73: _LL1802: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1807=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1807[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1808; _temp1808.tag= Cyc_TypeQual_tok; _temp1808.f1=({ struct Cyc_Absyn_Tqual
_temp1809; _temp1809.q_const= 0; _temp1809.q_volatile= 0; _temp1809.q_restrict=
1; _temp1809;}); _temp1808;}); _temp1807;}); break; case 74: _LL1806: yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1811=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1811[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1812; _temp1812.tag= Cyc_TypeSpecifier_tok;
_temp1812.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1813=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1813[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1814; _temp1814.tag= Cyc_Parse_Decl_spec;
_temp1814.f1=({ struct Cyc_Absyn_Decl* _temp1815=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1815->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1816=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1816[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1817; _temp1817.tag= Cyc_Absyn_Enum_d; _temp1817.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1818=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1818->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1818->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1818->fields=({
struct Cyc_Core_Opt* _temp1819=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1819->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1819;});
_temp1818;}); _temp1817;}); _temp1816;})); _temp1815->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1815;}); _temp1814;}); _temp1813;})); _temp1812;}); _temp1811;}); break;
case 75: _LL1810: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1821=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1821[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1822; _temp1822.tag=
Cyc_TypeSpecifier_tok; _temp1822.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1823=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1823[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1824; _temp1824.tag= Cyc_Absyn_EnumType;
_temp1824.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1824.f2= 0; _temp1824;}); _temp1823;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1822;}); _temp1821;}); break; case 76: _LL1820: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1826=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1826[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1827; _temp1827.tag= Cyc_Enumfield_tok;
_temp1827.f1=({ struct Cyc_Absyn_Enumfield* _temp1828=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1828->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1828->tag= 0;
_temp1828->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1828;});
_temp1827;}); _temp1826;}); break; case 77: _LL1825: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1830=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1830[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1831; _temp1831.tag= Cyc_Enumfield_tok;
_temp1831.f1=({ struct Cyc_Absyn_Enumfield* _temp1832=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1832->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1832->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1832->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1832;});
_temp1831;}); _temp1830;}); break; case 78: _LL1829: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1834=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1834[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1835; _temp1835.tag=
Cyc_EnumfieldList_tok; _temp1835.f1=({ struct Cyc_List_List* _temp1836=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1836->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1836->tl= 0; _temp1836;}); _temp1835;}); _temp1834;});
break; case 79: _LL1833: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1838=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1838[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1839; _temp1839.tag=
Cyc_EnumfieldList_tok; _temp1839.f1=({ struct Cyc_List_List* _temp1840=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1840->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1840->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1840;}); _temp1839;});
_temp1838;}); break; case 80: _LL1837: { void* t;{ void* _temp1842= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1844: if(
_temp1842 ==( void*) Cyc_Parse_Struct_su){ goto _LL1845;} else{ goto _LL1846;}
_LL1846: if( _temp1842 ==( void*) Cyc_Parse_Union_su){ goto _LL1847;} else{ goto
_LL1843;} _LL1845: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp1848=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp1848[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp1849; _temp1849.tag= Cyc_Absyn_AnonStructType; _temp1849.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1849;});
_temp1848;}); goto _LL1843; _LL1847: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1850=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1850[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1851; _temp1851.tag= Cyc_Absyn_AnonUnionType; _temp1851.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1851;});
_temp1850;}); goto _LL1843; _LL1843:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1852=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1852[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1853; _temp1853.tag=
Cyc_TypeSpecifier_tok; _temp1853.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1854=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1854[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1855; _temp1855.tag= Cyc_Parse_Type_spec;
_temp1855.f1=( void*) t; _temp1855.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1855;});
_temp1854;})); _temp1853;}); _temp1852;}); break;} case 81: _LL1841: { struct
Cyc_List_List* _temp1857=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1858= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1860: if(
_temp1858 ==( void*) Cyc_Parse_Struct_su){ goto _LL1861;} else{ goto _LL1862;}
_LL1862: if( _temp1858 ==( void*) Cyc_Parse_Union_su){ goto _LL1863;} else{ goto
_LL1859;} _LL1861: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1864=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1864->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1864;}), _temp1857,({ struct Cyc_Core_Opt*
_temp1865=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1865->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1865;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1859; _LL1863: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1866=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1866->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1866;}),
_temp1857,({ struct Cyc_Core_Opt* _temp1867=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1867->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1867;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1859; _LL1859:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1868=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1868[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1869; _temp1869.tag= Cyc_TypeSpecifier_tok;
_temp1869.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1870=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1870[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1871; _temp1871.tag= Cyc_Parse_Decl_spec;
_temp1871.f1= d; _temp1871;}); _temp1870;})); _temp1869;}); _temp1868;}); break;}
case 82: _LL1856: { struct Cyc_List_List* _temp1873=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ void* _temp1874= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _LL1876: if(
_temp1874 ==( void*) Cyc_Parse_Struct_su){ goto _LL1877;} else{ goto _LL1878;}
_LL1878: if( _temp1874 ==( void*) Cyc_Parse_Union_su){ goto _LL1879;} else{ goto
_LL1875;} _LL1877: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp1880=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1880->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1880;}), _temp1873,({ struct Cyc_Core_Opt*
_temp1881=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1881->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1881;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1875; _LL1879: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1882=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1882->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1882;}),
_temp1873,({ struct Cyc_Core_Opt* _temp1883=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1883->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1883;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1875; _LL1875:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1884=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1884[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1885; _temp1885.tag= Cyc_TypeSpecifier_tok;
_temp1885.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1886=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1886[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1887; _temp1887.tag= Cyc_Parse_Decl_spec;
_temp1887.f1= d; _temp1887;}); _temp1886;})); _temp1885;}); _temp1884;}); break;}
case 83: _LL1872:{ void* _temp1889= Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL1891: if(
_temp1889 ==( void*) Cyc_Parse_Struct_su){ goto _LL1892;} else{ goto _LL1893;}
_LL1893: if( _temp1889 ==( void*) Cyc_Parse_Union_su){ goto _LL1894;} else{ goto
_LL1890;} _LL1892: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1895=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1895[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1896; _temp1896.tag=
Cyc_TypeSpecifier_tok; _temp1896.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1897=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1897[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1898; _temp1898.tag= Cyc_Absyn_StructType;
_temp1898.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1898.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1898.f3= 0;
_temp1898;}); _temp1897;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1896;});
_temp1895;}); goto _LL1890; _LL1894: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1899=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1899[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1900; _temp1900.tag=
Cyc_TypeSpecifier_tok; _temp1900.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1901=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1901[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1902; _temp1902.tag= Cyc_Absyn_UnionType;
_temp1902.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1902.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1902.f3= 0;
_temp1902;}); _temp1901;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1900;});
_temp1899;}); goto _LL1890; _LL1890:;} break; case 84: _LL1888:{ void* _temp1904=
Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL1906: if( _temp1904 ==( void*) Cyc_Parse_Struct_su){
goto _LL1907;} else{ goto _LL1908;} _LL1908: if( _temp1904 ==( void*) Cyc_Parse_Union_su){
goto _LL1909;} else{ goto _LL1905;} _LL1907: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1910=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1910[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1911; _temp1911.tag=
Cyc_TypeSpecifier_tok; _temp1911.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1912=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1912[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1913; _temp1913.tag= Cyc_Absyn_StructType;
_temp1913.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1913.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1913.f3= 0;
_temp1913;}); _temp1912;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1911;});
_temp1910;}); goto _LL1905; _LL1909: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1914=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1914[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1915; _temp1915.tag=
Cyc_TypeSpecifier_tok; _temp1915.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1916=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1916[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1917; _temp1917.tag= Cyc_Absyn_UnionType;
_temp1917.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1917.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1917.f3= 0;
_temp1917;}); _temp1916;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1915;});
_temp1914;}); goto _LL1905; _LL1905:;} break; case 85: _LL1903: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp1919=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1919[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1920; _temp1920.tag= Cyc_TypeList_tok; _temp1920.f1= 0; _temp1920;});
_temp1919;}); break; case 86: _LL1918: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1922=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1922[ 0]=({ struct Cyc_TypeList_tok_struct _temp1923; _temp1923.tag= Cyc_TypeList_tok;
_temp1923.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])); _temp1923;}); _temp1922;}); break; case 87: _LL1921:((
struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1; yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp1925=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1925[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1926; _temp1926.tag= Cyc_TypeList_tok; _temp1926.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1926;});
_temp1925;}); break; case 88: _LL1924: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1928=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1928[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1929; _temp1929.tag=
Cyc_StructOrUnion_tok; _temp1929.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp1929;}); _temp1928;}); break; case 89: _LL1927: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1931=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp1931[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp1932; _temp1932.tag=
Cyc_StructOrUnion_tok; _temp1932.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp1932;}); _temp1931;}); break; case 90: _LL1930: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1934=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1934[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1935; _temp1935.tag= Cyc_StructFieldDeclList_tok; _temp1935.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1935;});
_temp1934;}); break; case 91: _LL1933: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1937=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1937[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1938; _temp1938.tag= Cyc_StructFieldDeclListList_tok; _temp1938.f1=({
struct Cyc_List_List* _temp1939=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1939->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1939->tl= 0;
_temp1939;}); _temp1938;}); _temp1937;}); break; case 92: _LL1936: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1941=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1941[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1942; _temp1942.tag= Cyc_StructFieldDeclListList_tok;
_temp1942.f1=({ struct Cyc_List_List* _temp1943=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1943->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1943->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1943;}); _temp1942;}); _temp1941;}); break;
case 93: _LL1940: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1945=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1945[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1946; _temp1946.tag=
Cyc_InitDeclList_tok; _temp1946.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1946;});
_temp1945;}); break; case 94: _LL1944: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1948=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1948[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1949; _temp1949.tag=
Cyc_InitDeclList_tok; _temp1949.f1=({ struct Cyc_List_List* _temp1950=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1950->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1950->tl= 0; _temp1950;}); _temp1949;}); _temp1948;});
break; case 95: _LL1947: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1952=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1952[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1953; _temp1953.tag=
Cyc_InitDeclList_tok; _temp1953.f1=({ struct Cyc_List_List* _temp1954=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1954->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1954->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1954;});
_temp1953;}); _temp1952;}); break; case 96: _LL1951: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1956=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1956[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1957; _temp1957.tag= Cyc_InitDecl_tok;
_temp1957.f1=({ struct _tuple15* _temp1958=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp1958->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1958->f2= 0;
_temp1958;}); _temp1957;}); _temp1956;}); break; case 97: _LL1955: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1960=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1960[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1961; _temp1961.tag= Cyc_InitDecl_tok; _temp1961.f1=({ struct _tuple15*
_temp1962=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1962->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1962->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1962;});
_temp1961;}); _temp1960;}); break; case 98: _LL1959: { struct Cyc_Absyn_Tqual tq=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])).f1; struct Cyc_List_List* _temp1964=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3; void*
_temp1965= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
struct Cyc_List_List* _temp1968; struct Cyc_List_List* _temp1970; struct _tuple0
_temp1966=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _LL1971:
_temp1970= _temp1966.f1; goto _LL1969; _LL1969: _temp1968= _temp1966.f2; goto
_LL1967; _LL1967: { struct Cyc_List_List* _temp1972=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, _temp1965, _temp1970, _temp1964), _temp1968); yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1973=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1973[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1974; _temp1974.tag= Cyc_StructFieldDeclList_tok; _temp1974.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),
_temp1972); _temp1974;}); _temp1973;}); break;}} case 99: _LL1963: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp1976=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1976[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1977; _temp1977.tag= Cyc_QualSpecList_tok;
_temp1977.f1=({ struct _tuple18* _temp1978=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1978->f1= Cyc_Absyn_empty_tqual(); _temp1978->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1979=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1979->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1979->tl=
0; _temp1979;}); _temp1978->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1978;}); _temp1977;});
_temp1976;}); break; case 100: _LL1975: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1981=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1981[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1982; _temp1982.tag=
Cyc_QualSpecList_tok; _temp1982.f1=({ struct _tuple18* _temp1983=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1983->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1983->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1984=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1984->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1984->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1984;}); _temp1983->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1983;});
_temp1982;}); _temp1981;}); break; case 101: _LL1980: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1986=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1986[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1987; _temp1987.tag= Cyc_QualSpecList_tok;
_temp1987.f1=({ struct _tuple18* _temp1988=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1988->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1988->f2= 0;
_temp1988->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1988;}); _temp1987;}); _temp1986;}); break; case
102: _LL1985: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1990=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1990[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1991; _temp1991.tag=
Cyc_QualSpecList_tok; _temp1991.f1=({ struct _tuple18* _temp1992=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1992->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1992->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1992->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1992;});
_temp1991;}); _temp1990;}); break; case 103: _LL1989: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp1994=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1994[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1995; _temp1995.tag= Cyc_DeclaratorExpoptList_tok;
_temp1995.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1995;}); _temp1994;}); break; case 104: _LL1993: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp1997=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1997[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1998; _temp1998.tag= Cyc_DeclaratorExpoptList_tok;
_temp1998.f1=({ struct Cyc_List_List* _temp1999=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1999->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1999->tl= 0;
_temp1999;}); _temp1998;}); _temp1997;}); break; case 105: _LL1996: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp2001=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2001[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2002; _temp2002.tag= Cyc_DeclaratorExpoptList_tok;
_temp2002.f1=({ struct Cyc_List_List* _temp2003=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2003->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2003->tl=
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2003;}); _temp2002;}); _temp2001;}); break; case 106:
_LL2000: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2005=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2005[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2006; _temp2006.tag=
Cyc_DeclaratorExpopt_tok; _temp2006.f1=({ struct _tuple13* _temp2007=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2007->f1= Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2007->f2= 0;
_temp2007;}); _temp2006;}); _temp2005;}); break; case 107: _LL2004: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp2009=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2009[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2010; _temp2010.tag= Cyc_DeclaratorExpopt_tok;
_temp2010.f1=({ struct _tuple13* _temp2011=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2011->f1=({ struct Cyc_Parse_Declarator*
_temp2013=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp2013->id=({ struct _tuple1* _temp2014=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2014->f1= Cyc_Absyn_rel_ns_null; _temp2014->f2=
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp2014;}); _temp2013->tms= 0; _temp2013;});
_temp2011->f2=({ struct Cyc_Core_Opt* _temp2012=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2012->v=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2012;});
_temp2011;}); _temp2010;}); _temp2009;}); break; case 108: _LL2008: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp2016=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2016[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2017; _temp2017.tag= Cyc_DeclaratorExpopt_tok;
_temp2017.f1=({ struct _tuple13* _temp2018=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2018->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2018->f2=({
struct Cyc_Core_Opt* _temp2019=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2019->v=( void*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2019;}); _temp2018;});
_temp2017;}); _temp2016;}); break; case 109: _LL2015: { struct Cyc_List_List*
_temp2021=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2022=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2022[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2023; _temp2023.tag= Cyc_TypeSpecifier_tok;
_temp2023.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2024=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2024[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2025; _temp2025.tag= Cyc_Parse_Decl_spec;
_temp2025.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _temp2021,({
struct Cyc_Core_Opt* _temp2026=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2026->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2026;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2025;}); _temp2024;})); _temp2023;}); _temp2022;}); break;} case 110:
_LL2020: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2028=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2028[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2029; _temp2029.tag=
Cyc_TypeSpecifier_tok; _temp2029.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2030=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2030[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2031; _temp2031.tag= Cyc_Absyn_TunionType;
_temp2031.f1=({ struct Cyc_Absyn_TunionInfo _temp2032; _temp2032.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2033=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2033[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2034; _temp2034.tag= Cyc_Absyn_UnknownTunion;
_temp2034.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2035; _temp2035.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp2035.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2035;});
_temp2034;}); _temp2033;})); _temp2032.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2032.rgn=( void*)((
void*) Cyc_Absyn_HeapRgn); _temp2032;}); _temp2031;}); _temp2030;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2029;}); _temp2028;}); break; case 111: _LL2027: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2037=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2037[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2038; _temp2038.tag= Cyc_TypeSpecifier_tok;
_temp2038.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2039=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2039[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2040; _temp2040.tag=
Cyc_Absyn_TunionType; _temp2040.f1=({ struct Cyc_Absyn_TunionInfo _temp2041;
_temp2041.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp2042=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp2042[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp2043; _temp2043.tag= Cyc_Absyn_UnknownTunion; _temp2043.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp2044; _temp2044.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2044.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2044;}); _temp2043;}); _temp2042;})); _temp2041.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2041.rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2041;}); _temp2040;}); _temp2039;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2038;}); _temp2037;}); break; case 112: _LL2036: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2046=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2046[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2047; _temp2047.tag= Cyc_TypeSpecifier_tok;
_temp2047.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2048=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2048[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2049; _temp2049.tag= Cyc_Absyn_TunionFieldType; _temp2049.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2050; _temp2050.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2051=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2051[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2052; _temp2052.tag= Cyc_Absyn_UnknownTunionfield; _temp2052.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2053; _temp2053.tunion_name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2053.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp2053.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2053;});
_temp2052;}); _temp2051;})); _temp2050.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2050;}); _temp2049;});
_temp2048;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2047;}); _temp2046;}); break; case 113:
_LL2045: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2055=( struct Cyc_Bool_tok_struct*)
GC_malloc( sizeof( struct Cyc_Bool_tok_struct)); _temp2055[ 0]=({ struct Cyc_Bool_tok_struct
_temp2056; _temp2056.tag= Cyc_Bool_tok; _temp2056.f1= 0; _temp2056;}); _temp2055;});
break; case 114: _LL2054: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2058=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2058[ 0]=({ struct Cyc_Bool_tok_struct _temp2059; _temp2059.tag= Cyc_Bool_tok;
_temp2059.f1= 1; _temp2059;}); _temp2058;}); break; case 115: _LL2057: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2061=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2061[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2062; _temp2062.tag= Cyc_TunionFieldList_tok;
_temp2062.f1=({ struct Cyc_List_List* _temp2063=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2063->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2063->tl= 0;
_temp2063;}); _temp2062;}); _temp2061;}); break; case 116: _LL2060: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2065=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2065[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2066; _temp2066.tag= Cyc_TunionFieldList_tok;
_temp2066.f1=({ struct Cyc_List_List* _temp2067=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2067->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2067->tl=
0; _temp2067;}); _temp2066;}); _temp2065;}); break; case 117: _LL2064: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2069=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2069[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2070; _temp2070.tag= Cyc_TunionFieldList_tok;
_temp2070.f1=({ struct Cyc_List_List* _temp2071=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2071->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2071->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2071;}); _temp2070;}); _temp2069;}); break; case 118:
_LL2068: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2073=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2073[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2074; _temp2074.tag=
Cyc_TunionFieldList_tok; _temp2074.f1=({ struct Cyc_List_List* _temp2075=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2075->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2075->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2075;}); _temp2074;});
_temp2073;}); break; case 119: _LL2072: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2077=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2077[ 0]=({ struct Cyc_Scope_tok_struct _temp2078; _temp2078.tag= Cyc_Scope_tok;
_temp2078.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2078;}); _temp2077;});
break; case 120: _LL2076: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2080=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2080[ 0]=({ struct Cyc_Scope_tok_struct _temp2081; _temp2081.tag= Cyc_Scope_tok;
_temp2081.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2081;}); _temp2080;});
break; case 121: _LL2079: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2083=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2083[ 0]=({ struct Cyc_Scope_tok_struct _temp2084; _temp2084.tag= Cyc_Scope_tok;
_temp2084.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2084;}); _temp2083;});
break; case 122: _LL2082: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2086=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2086[ 0]=({ struct Cyc_TunionField_tok_struct _temp2087; _temp2087.tag= Cyc_TunionField_tok;
_temp2087.f1=({ struct Cyc_Absyn_Tunionfield* _temp2088=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2088->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2088->tvs= 0;
_temp2088->typs= 0; _temp2088->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2088->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2088;}); _temp2087;}); _temp2086;}); break; case 123:
_LL2085: { struct Cyc_List_List* _temp2090=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); struct Cyc_List_List*
_temp2091=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp2092=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2092[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2093; _temp2093.tag= Cyc_TunionField_tok;
_temp2093.f1=({ struct Cyc_Absyn_Tunionfield* _temp2094=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2094->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2094->tvs=
_temp2091; _temp2094->typs= _temp2090; _temp2094->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2094->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp2094;}); _temp2093;}); _temp2092;}); break;}
case 124: _LL2089: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL2095: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2097=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2097[ 0]=({ struct Cyc_Declarator_tok_struct _temp2098; _temp2098.tag= Cyc_Declarator_tok;
_temp2098.f1=({ struct Cyc_Parse_Declarator* _temp2099=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2099->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2099->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]),( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2099;});
_temp2098;}); _temp2097;}); break; case 126: _LL2096: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2101=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2101[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2102; _temp2102.tag= Cyc_Declarator_tok;
_temp2102.f1=({ struct Cyc_Parse_Declarator* _temp2103=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2103->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2103->tms= 0;
_temp2103;}); _temp2102;}); _temp2101;}); break; case 127: _LL2100: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 128:
_LL2104: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2106=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2106[ 0]=({ struct Cyc_Declarator_tok_struct _temp2107; _temp2107.tag= Cyc_Declarator_tok;
_temp2107.f1=({ struct Cyc_Parse_Declarator* _temp2108=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2108->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->id;
_temp2108->tms=({ struct Cyc_List_List* _temp2109=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2109->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2109->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]))->tms; _temp2109;}); _temp2108;}); _temp2107;});
_temp2106;}); break; case 129: _LL2105: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2111=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2111[ 0]=({ struct Cyc_Declarator_tok_struct _temp2112; _temp2112.tag= Cyc_Declarator_tok;
_temp2112.f1=({ struct Cyc_Parse_Declarator* _temp2113=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2113->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2113->tms=({ struct Cyc_List_List* _temp2114=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2114->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2115=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2115[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2116; _temp2116.tag= Cyc_Absyn_ConstArray_mod;
_temp2116.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2116;}); _temp2115;})); _temp2114->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2114;}); _temp2113;}); _temp2112;}); _temp2111;}); break; case 130: _LL2110: {
struct _tuple17 _temp2120; struct Cyc_List_List* _temp2121; struct Cyc_Core_Opt*
_temp2123; struct Cyc_Absyn_VarargInfo* _temp2125; int _temp2127; struct Cyc_List_List*
_temp2129; struct _tuple17* _temp2118= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2120=*
_temp2118; _LL2130: _temp2129= _temp2120.f1; goto _LL2128; _LL2128: _temp2127=
_temp2120.f2; goto _LL2126; _LL2126: _temp2125= _temp2120.f3; goto _LL2124;
_LL2124: _temp2123= _temp2120.f4; goto _LL2122; _LL2122: _temp2121= _temp2120.f5;
goto _LL2119; _LL2119: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2131=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2131[ 0]=({ struct Cyc_Declarator_tok_struct _temp2132; _temp2132.tag= Cyc_Declarator_tok;
_temp2132.f1=({ struct Cyc_Parse_Declarator* _temp2133=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2133->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2133->tms=({ struct Cyc_List_List* _temp2134=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2134->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2135=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2135[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2136; _temp2136.tag= Cyc_Absyn_Function_mod;
_temp2136.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2137=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2137[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2138; _temp2138.tag= Cyc_Absyn_WithTypes;
_temp2138.f1= _temp2129; _temp2138.f2= _temp2127; _temp2138.f3= _temp2125;
_temp2138.f4= _temp2123; _temp2138.f5= _temp2121; _temp2138;}); _temp2137;}));
_temp2136;}); _temp2135;})); _temp2134->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2134;});
_temp2133;}); _temp2132;}); _temp2131;}); break;} case 131: _LL2117: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2140=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2140[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2141; _temp2141.tag= Cyc_Declarator_tok;
_temp2141.f1=({ struct Cyc_Parse_Declarator* _temp2142=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2142->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->id;
_temp2142->tms=({ struct Cyc_List_List* _temp2143=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2143->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2144=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2144[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2145; _temp2145.tag= Cyc_Absyn_Function_mod;
_temp2145.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2146=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2146[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2147; _temp2147.tag= Cyc_Absyn_WithTypes;
_temp2147.f1= 0; _temp2147.f2= 0; _temp2147.f3= 0; _temp2147.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2147.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2147;}); _temp2146;})); _temp2145;}); _temp2144;}));
_temp2143->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]))->tms; _temp2143;}); _temp2142;}); _temp2141;});
_temp2140;}); break; case 132: _LL2139: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2149=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2149[ 0]=({ struct Cyc_Declarator_tok_struct _temp2150; _temp2150.tag= Cyc_Declarator_tok;
_temp2150.f1=({ struct Cyc_Parse_Declarator* _temp2151=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2151->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2151->tms=({ struct Cyc_List_List* _temp2152=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2152->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2153=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2153[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2154; _temp2154.tag= Cyc_Absyn_Function_mod;
_temp2154.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2155=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2155[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2156; _temp2156.tag= Cyc_Absyn_NoTypes;
_temp2156.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2156.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2156;});
_temp2155;})); _temp2154;}); _temp2153;})); _temp2152->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2152;}); _temp2151;}); _temp2150;}); _temp2149;}); break; case 133: _LL2148: {
struct Cyc_List_List* _temp2158=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2159=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2159[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2160; _temp2160.tag= Cyc_Declarator_tok;
_temp2160.f1=({ struct Cyc_Parse_Declarator* _temp2161=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2161->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2161->tms=({ struct Cyc_List_List* _temp2162=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2162->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2163=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2163[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2164; _temp2164.tag= Cyc_Absyn_TypeParams_mod;
_temp2164.f1= _temp2158; _temp2164.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2164.f3=
0; _temp2164;}); _temp2163;})); _temp2162->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2162;});
_temp2161;}); _temp2160;}); _temp2159;}); break;} case 134: _LL2157:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2166=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2167=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2167[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2168; _temp2168.tag= Cyc_Declarator_tok;
_temp2168.f1=({ struct Cyc_Parse_Declarator* _temp2169=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2169->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2169->tms=({ struct Cyc_List_List* _temp2170=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2170->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2171=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2171[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2172; _temp2172.tag= Cyc_Absyn_TypeParams_mod;
_temp2172.f1= _temp2166; _temp2172.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2172.f3=
0; _temp2172;}); _temp2171;})); _temp2170->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2170;});
_temp2169;}); _temp2168;}); _temp2167;}); break;} case 135: _LL2165: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2174=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2174[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2175; _temp2175.tag= Cyc_Declarator_tok;
_temp2175.f1=({ struct Cyc_Parse_Declarator* _temp2176=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2176->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->id;
_temp2176->tms=({ struct Cyc_List_List* _temp2177=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2177->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2178=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2178[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2179; _temp2179.tag= Cyc_Absyn_Attributes_mod;
_temp2179.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2179.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2179;}); _temp2178;}));
_temp2177->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]))->tms; _temp2177;}); _temp2176;}); _temp2175;});
_temp2174;}); break; case 136: _LL2173: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2181=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2181[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2182; _temp2182.tag= Cyc_Declarator_tok;
_temp2182.f1=({ struct Cyc_Parse_Declarator* _temp2183=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2183->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2183->tms= 0;
_temp2183;}); _temp2182;}); _temp2181;}); break; case 137: _LL2180: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2185=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2185[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2186; _temp2186.tag= Cyc_Declarator_tok;
_temp2186.f1=({ struct Cyc_Parse_Declarator* _temp2187=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2187->id= Cyc_Absyn_exn_name;
_temp2187->tms= 0; _temp2187;}); _temp2186;}); _temp2185;}); break; case 138:
_LL2184: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2189=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2189[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2190; _temp2190.tag=
Cyc_TypeModifierList_tok; _temp2190.f1=({ struct Cyc_List_List* _temp2191=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2191->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2192=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2192[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2193; _temp2193.tag= Cyc_Absyn_Pointer_mod;
_temp2193.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2193.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2193.f3= Cyc_Absyn_empty_tqual(); _temp2193;}); _temp2192;}));
_temp2191->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2191;});
_temp2190;}); _temp2189;}); break; case 139: _LL2188: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2195=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2195[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2196; _temp2196.tag= Cyc_TypeModifierList_tok;
_temp2196.f1=({ struct Cyc_List_List* _temp2197=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2197->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2198=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2198[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2199; _temp2199.tag= Cyc_Absyn_Pointer_mod;
_temp2199.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2199.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2199.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2199;}); _temp2198;})); _temp2197->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0); _temp2197;});
_temp2196;}); _temp2195;}); break; case 140: _LL2194: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2201=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2201[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2202; _temp2202.tag= Cyc_TypeModifierList_tok;
_temp2202.f1=({ struct Cyc_List_List* _temp2203=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2203->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2204=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2204[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2205; _temp2205.tag= Cyc_Absyn_Pointer_mod;
_temp2205.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2205.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2205.f3= Cyc_Absyn_empty_tqual(); _temp2205;}); _temp2204;}));
_temp2203->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2203;});
_temp2202;}); _temp2201;}); break; case 141: _LL2200: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2207=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2207[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2208; _temp2208.tag= Cyc_TypeModifierList_tok;
_temp2208.f1=({ struct Cyc_List_List* _temp2209=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2209->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2210=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2210[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2211; _temp2211.tag= Cyc_Absyn_Pointer_mod;
_temp2211.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2211.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2211.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2211;}); _temp2210;})); _temp2209->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2209;});
_temp2208;}); _temp2207;}); break; case 142: _LL2206: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2213=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2213[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2214; _temp2214.tag= Cyc_Pointer_Sort_tok;
_temp2214.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2215=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2215[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2216; _temp2216.tag=
Cyc_Absyn_Nullable_ps; _temp2216.f1= Cyc_Absyn_exp_unsigned_one; _temp2216;});
_temp2215;})); _temp2214;}); _temp2213;}); break; case 143: _LL2212: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2218=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2218[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2219; _temp2219.tag= Cyc_Pointer_Sort_tok;
_temp2219.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2220=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2220[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2221; _temp2221.tag= Cyc_Absyn_NonNullable_ps; _temp2221.f1= Cyc_Absyn_exp_unsigned_one;
_temp2221;}); _temp2220;})); _temp2219;}); _temp2218;}); break; case 144:
_LL2217: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2223=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2223[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2224; _temp2224.tag=
Cyc_Pointer_Sort_tok; _temp2224.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2225=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2225[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2226; _temp2226.tag=
Cyc_Absyn_Nullable_ps; _temp2226.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2226;});
_temp2225;})); _temp2224;}); _temp2223;}); break; case 145: _LL2222: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2228=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2228[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2229; _temp2229.tag= Cyc_Pointer_Sort_tok;
_temp2229.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2230=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2230[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2231; _temp2231.tag= Cyc_Absyn_NonNullable_ps; _temp2231.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2231;});
_temp2230;})); _temp2229;}); _temp2228;}); break; case 146: _LL2227: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2233=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2233[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2234; _temp2234.tag= Cyc_Pointer_Sort_tok;
_temp2234.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2234;});
_temp2233;}); break; case 147: _LL2232: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2236=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2236[ 0]=({ struct Cyc_Rgn_tok_struct _temp2237; _temp2237.tag= Cyc_Rgn_tok;
_temp2237.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2237;}); _temp2236;});
break; case 148: _LL2235: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2238: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2240=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2240[ 0]=({ struct Cyc_Rgn_tok_struct _temp2241; _temp2241.tag= Cyc_Rgn_tok;
_temp2241.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2241;});
_temp2240;}); break; case 150: _LL2239: if( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind\n", sizeof( unsigned char), 23u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2243=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2243[ 0]=({ struct Cyc_Rgn_tok_struct _temp2244; _temp2244.tag= Cyc_Rgn_tok;
_temp2244.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2244;});
_temp2243;}); break; case 151: _LL2242: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2246=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2246[ 0]=({ struct Cyc_Rgn_tok_struct _temp2247; _temp2247.tag= Cyc_Rgn_tok;
_temp2247.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2248=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2248->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2248;}), 0); _temp2247;}); _temp2246;});
break; case 152: _LL2245: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 153: _LL2249: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2251=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2251[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2252; _temp2252.tag= Cyc_TypeQual_tok;
_temp2252.f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeQual_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2252;});
_temp2251;}); break; case 154: _LL2250: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2254=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2254[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2255; _temp2255.tag= Cyc_ParamDeclListBool_tok; _temp2255.f1=({ struct
_tuple17* _temp2256=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2256->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])); _temp2256->f2= 0; _temp2256->f3= 0; _temp2256->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2256->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2256;}); _temp2255;}); _temp2254;}); break; case 155:
_LL2253: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2258=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp2258[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp2259; _temp2259.tag=
Cyc_ParamDeclListBool_tok; _temp2259.f1=({ struct _tuple17* _temp2260=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2260->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)])); _temp2260->f2= 1;
_temp2260->f3= 0; _temp2260->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2260->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2260;});
_temp2259;}); _temp2258;}); break; case 156: _LL2257: { void* _temp2264; struct
Cyc_Absyn_Tqual _temp2266; struct Cyc_Core_Opt* _temp2268; struct _tuple2
_temp2262=* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL2269: _temp2268= _temp2262.f1; goto _LL2267; _LL2267:
_temp2266= _temp2262.f2; goto _LL2265; _LL2265: _temp2264= _temp2262.f3; goto
_LL2263; _LL2263: { struct Cyc_Absyn_VarargInfo* _temp2270=({ struct Cyc_Absyn_VarargInfo*
_temp2274=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2274->name= _temp2268; _temp2274->tq= _temp2266; _temp2274->type=( void*)
_temp2264; _temp2274->rgn=( void*) Cyc_yyget_Rgn_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2274->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2274;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2271=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2271[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2272; _temp2272.tag= Cyc_ParamDeclListBool_tok; _temp2272.f1=({ struct
_tuple17* _temp2273=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2273->f1= 0; _temp2273->f2= 0; _temp2273->f3= _temp2270; _temp2273->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2273->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2273;}); _temp2272;}); _temp2271;}); break;}} case 157:
_LL2261: { void* _temp2278; struct Cyc_Absyn_Tqual _temp2280; struct Cyc_Core_Opt*
_temp2282; struct _tuple2 _temp2276=* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL2283: _temp2282=
_temp2276.f1; goto _LL2281; _LL2281: _temp2280= _temp2276.f2; goto _LL2279;
_LL2279: _temp2278= _temp2276.f3; goto _LL2277; _LL2277: { struct Cyc_Absyn_VarargInfo*
_temp2284=({ struct Cyc_Absyn_VarargInfo* _temp2288=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2288->name= _temp2282;
_temp2288->tq= _temp2280; _temp2288->type=( void*) _temp2278; _temp2288->rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2288->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2288;}); yyval=(
void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2285=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp2285[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp2286; _temp2286.tag= Cyc_ParamDeclListBool_tok;
_temp2286.f1=({ struct _tuple17* _temp2287=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2287->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)])); _temp2287->f2= 0;
_temp2287->f3= _temp2284; _temp2287->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2287->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2287;});
_temp2286;}); _temp2285;}); break;}} case 158: _LL2275: yyval=( void*)({ struct
Cyc_TypeOpt_tok_struct* _temp2290=( struct Cyc_TypeOpt_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2290[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2291; _temp2291.tag= Cyc_TypeOpt_tok; _temp2291.f1= 0; _temp2291;});
_temp2290;}); break; case 159: _LL2289: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2293=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2293[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2294; _temp2294.tag= Cyc_TypeOpt_tok;
_temp2294.f1=({ struct Cyc_Core_Opt* _temp2295=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2295->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2296=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2296[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2297; _temp2297.tag= Cyc_Absyn_JoinEff;
_temp2297.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2297;}); _temp2296;})); _temp2295;}); _temp2294;});
_temp2293;}); break; case 160: _LL2292: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2299=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2299[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2300; _temp2300.tag= Cyc_Rgnorder_tok;
_temp2300.f1= 0; _temp2300;}); _temp2299;}); break; case 161: _LL2298: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 162:
_LL2301: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2303=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2303[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2304; _temp2304.tag= Cyc_Rgnorder_tok; _temp2304.f1=({ struct Cyc_List_List*
_temp2305=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2305->hd=( void*)({ struct _tuple19* _temp2306=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2306->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2306->f2= Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp2306;}); _temp2305->tl= 0; _temp2305;}); _temp2304;}); _temp2303;}); break;
case 163: _LL2302: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2308=(
struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2308[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2309; _temp2309.tag= Cyc_Rgnorder_tok;
_temp2309.f1=({ struct Cyc_List_List* _temp2310=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2310->hd=( void*)({ struct
_tuple19* _temp2311=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp2311->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2311->f2= Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)
Cyc_Absyn_RgnKind)); _temp2311;}); _temp2310->tl= Cyc_yyget_Rgnorder_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2310;}); _temp2309;});
_temp2308;}); break; case 164: _LL2307: yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2313=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2313[ 0]=({ struct Cyc_Bool_tok_struct _temp2314; _temp2314.tag= Cyc_Bool_tok;
_temp2314.f1= 0; _temp2314;}); _temp2313;}); break; case 165: _LL2312: { struct
_tagged_arr _temp2316= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_String_zstrcmp(
_temp2316, _tag_arr("inject", sizeof( unsigned char), 7u)) != 0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2317=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2317[ 0]=({ struct Cyc_Bool_tok_struct _temp2318; _temp2318.tag= Cyc_Bool_tok;
_temp2318.f1= 1; _temp2318;}); _temp2317;}); break;} case 166: _LL2315: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 167:
_LL2319: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2321=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2321[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2322; _temp2322.tag= Cyc_TypeList_tok; _temp2322.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2322;});
_temp2321;}); break; case 168: _LL2320: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2324=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2324[ 0]=({ struct Cyc_TypeList_tok_struct _temp2325; _temp2325.tag= Cyc_TypeList_tok;
_temp2325.f1= 0; _temp2325;}); _temp2324;}); break; case 169: _LL2323: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case
170: _LL2326: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2328=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2328[ 0]=({ struct Cyc_TypeList_tok_struct _temp2329; _temp2329.tag= Cyc_TypeList_tok;
_temp2329.f1=({ struct Cyc_List_List* _temp2330=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2330->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp2330->tl= 0; _temp2330;}); _temp2329;}); _temp2328;}); break; case 171:
_LL2327: if( Cyc_yyget_Kind_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]) !=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err( _tag_arr("expecing effect kind (E)",
sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2332=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2332[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2333; _temp2333.tag= Cyc_TypeList_tok; _temp2333.f1=({ struct Cyc_List_List*
_temp2334=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2334->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp2334->tl= 0;
_temp2334;}); _temp2333;}); _temp2332;}); break; case 172: _LL2331: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2336=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2336[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2337; _temp2337.tag= Cyc_TypeList_tok; _temp2337.f1=({ struct Cyc_List_List*
_temp2338=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2338->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2339=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2339[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2340; _temp2340.tag= Cyc_Absyn_AccessEff;
_temp2340.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2340;});
_temp2339;})); _temp2338->tl= 0; _temp2338;}); _temp2337;}); _temp2336;});
break; case 173: _LL2335: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2342=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2342[ 0]=({ struct Cyc_TypeList_tok_struct _temp2343; _temp2343.tag= Cyc_TypeList_tok;
_temp2343.f1=({ struct Cyc_List_List* _temp2344=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2344->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2345=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2345[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2346; _temp2346.tag= Cyc_Absyn_AccessEff;
_temp2346.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2346;});
_temp2345;})); _temp2344->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2344;}); _temp2343;});
_temp2342;}); break; case 174: _LL2341: if( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2348=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2348[ 0]=({ struct Cyc_TypeList_tok_struct _temp2349; _temp2349.tag= Cyc_TypeList_tok;
_temp2349.f1=({ struct Cyc_List_List* _temp2350=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2350->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2351=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2351[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2352; _temp2352.tag= Cyc_Absyn_AccessEff;
_temp2352.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2352;});
_temp2351;})); _temp2350->tl= 0; _temp2350;}); _temp2349;}); _temp2348;});
break; case 175: _LL2347: if( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2354=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2354[ 0]=({ struct Cyc_TypeList_tok_struct _temp2355; _temp2355.tag= Cyc_TypeList_tok;
_temp2355.f1=({ struct Cyc_List_List* _temp2356=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2356->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2357=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2357[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2358; _temp2358.tag= Cyc_Absyn_AccessEff;
_temp2358.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]),(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp2358;});
_temp2357;})); _temp2356->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2356;}); _temp2355;});
_temp2354;}); break; case 176: _LL2353: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct*
_temp2360=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp2360[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp2361; _temp2361.tag=
Cyc_ParamDeclList_tok; _temp2361.f1=({ struct Cyc_List_List* _temp2362=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2362->hd=( void*)
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2362->tl= 0; _temp2362;}); _temp2361;}); _temp2360;});
break; case 177: _LL2359: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct*
_temp2364=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp2364[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp2365; _temp2365.tag=
Cyc_ParamDeclList_tok; _temp2365.f1=({ struct Cyc_List_List* _temp2366=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2366->hd=( void*)
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2366->tl= Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2366;});
_temp2365;}); _temp2364;}); break; case 178: _LL2363: { void* _temp2368= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2369=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2370=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2371=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2372= Cyc_Parse_apply_tms( _temp2370, _temp2368, _temp2369,
_temp2371); if( _temp2372.f3 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
_tuple1* _temp2373=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id;{ void* _temp2374=(*
_temp2373).f1; struct Cyc_List_List* _temp2384; struct Cyc_List_List* _temp2386;
_LL2376: if( _temp2374 ==( void*) Cyc_Absyn_Loc_n){ goto _LL2377;} else{ goto
_LL2378;} _LL2378: if(( unsigned int) _temp2374 > 1u?*(( int*) _temp2374) == Cyc_Absyn_Rel_n:
0){ _LL2385: _temp2384=(( struct Cyc_Absyn_Rel_n_struct*) _temp2374)->f1; if(
_temp2384 == 0){ goto _LL2379;} else{ goto _LL2380;}} else{ goto _LL2380;}
_LL2380: if(( unsigned int) _temp2374 > 1u?*(( int*) _temp2374) == Cyc_Absyn_Abs_n:
0){ _LL2387: _temp2386=(( struct Cyc_Absyn_Abs_n_struct*) _temp2374)->f1; if(
_temp2386 == 0){ goto _LL2381;} else{ goto _LL2382;}} else{ goto _LL2382;}
_LL2382: goto _LL2383; _LL2377: goto _LL2375; _LL2379: goto _LL2375; _LL2381:
goto _LL2375; _LL2383: Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a module name",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); goto
_LL2375; _LL2375:;}{ struct Cyc_Core_Opt* _temp2388=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp2392=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2392->v=( void*)(* _temp2373).f2; _temp2392;}); if(
_temp2372.f4 != 0){ Cyc_Parse_warn( _tag_arr("extra attributes on parameter, ignoring",
sizeof( unsigned char), 40u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2372.f3 != 0){ Cyc_Parse_warn( _tag_arr("extra type variables on parameter, ignoring",
sizeof( unsigned char), 44u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2389=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2389[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2390; _temp2390.tag= Cyc_ParamDecl_tok; _temp2390.f1=({
struct _tuple2* _temp2391=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2391->f1= _temp2388; _temp2391->f2= _temp2372.f1; _temp2391->f3= _temp2372.f2;
_temp2391;}); _temp2390;}); _temp2389;}); break;}}} case 179: _LL2367: { void*
_temp2394= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2395=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; struct Cyc_Absyn_Tqual _temp2396=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; if( _temp2395
!= 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring", sizeof(
unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2397=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2397[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2398; _temp2398.tag= Cyc_ParamDecl_tok; _temp2398.f1=({
struct _tuple2* _temp2399=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2399->f1= 0; _temp2399->f2= _temp2396; _temp2399->f3= _temp2394; _temp2399;});
_temp2398;}); _temp2397;}); break;} case 180: _LL2393: { void* _temp2401= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2402=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2403=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2404=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2405= Cyc_Parse_apply_tms( _temp2403, _temp2401, _temp2402,
_temp2404); if( _temp2405.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2405.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2406=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2406[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2407; _temp2407.tag= Cyc_ParamDecl_tok; _temp2407.f1=({
struct _tuple2* _temp2408=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2408->f1= 0; _temp2408->f2= _temp2405.f1; _temp2408->f3= _temp2405.f2;
_temp2408;}); _temp2407;}); _temp2406;}); break;} case 181: _LL2400: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2410=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2410[ 0]=({ struct Cyc_IdList_tok_struct
_temp2411; _temp2411.tag= Cyc_IdList_tok; _temp2411.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2411;});
_temp2410;}); break; case 182: _LL2409: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2413=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2413[ 0]=({ struct Cyc_IdList_tok_struct _temp2414; _temp2414.tag= Cyc_IdList_tok;
_temp2414.f1=({ struct Cyc_List_List* _temp2415=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2415->hd=( void*)({ struct
_tagged_arr* _temp2416=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2416[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2416;}); _temp2415->tl=
0; _temp2415;}); _temp2414;}); _temp2413;}); break; case 183: _LL2412: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2418=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2418[ 0]=({ struct Cyc_IdList_tok_struct
_temp2419; _temp2419.tag= Cyc_IdList_tok; _temp2419.f1=({ struct Cyc_List_List*
_temp2420=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2420->hd=( void*)({ struct _tagged_arr* _temp2421=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2421[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2421;});
_temp2420->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2420;}); _temp2419;}); _temp2418;}); break; case 184:
_LL2417: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 185: _LL2422: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 186: _LL2423: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2425=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2425[ 0]=({ struct Cyc_Exp_tok_struct _temp2426; _temp2426.tag= Cyc_Exp_tok;
_temp2426.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2427=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2427[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2428; _temp2428.tag= Cyc_Absyn_UnresolvedMem_e; _temp2428.f1= 0; _temp2428.f2=
0; _temp2428;}); _temp2427;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2426;});
_temp2425;}); break; case 187: _LL2424: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2430=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2430[ 0]=({ struct Cyc_Exp_tok_struct _temp2431; _temp2431.tag= Cyc_Exp_tok;
_temp2431.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2432=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2432[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2433; _temp2433.tag= Cyc_Absyn_UnresolvedMem_e; _temp2433.f1= 0; _temp2433.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2433;});
_temp2432;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2431;}); _temp2430;}); break; case 188:
_LL2429: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2435=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2435[ 0]=({ struct Cyc_Exp_tok_struct
_temp2436; _temp2436.tag= Cyc_Exp_tok; _temp2436.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2437=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2437[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2438; _temp2438.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2438.f1= 0; _temp2438.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp2438;});
_temp2437;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2436;}); _temp2435;}); break; case 189:
_LL2434: { struct Cyc_Absyn_Vardecl* _temp2440= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2446=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2446->f1=( void*) Cyc_Absyn_Loc_n; _temp2446->f2=({ struct _tagged_arr*
_temp2447=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp2447[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); _temp2447;}); _temp2446;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line)));
_temp2440->tq=({ struct Cyc_Absyn_Tqual _temp2441; _temp2441.q_const= 1;
_temp2441.q_volatile= 0; _temp2441.q_restrict= 1; _temp2441;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2442=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2442[ 0]=({ struct Cyc_Exp_tok_struct
_temp2443; _temp2443.tag= Cyc_Exp_tok; _temp2443.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2444=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2444[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2445; _temp2445.tag= Cyc_Absyn_Comprehension_e;
_temp2445.f1= _temp2440; _temp2445.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2445.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2445;});
_temp2444;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2443;}); _temp2442;}); break;} case
190: _LL2439: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2449=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2449[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2450; _temp2450.tag=
Cyc_InitializerList_tok; _temp2450.f1=({ struct Cyc_List_List* _temp2451=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2451->hd=(
void*)({ struct _tuple20* _temp2452=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2452->f1= 0; _temp2452->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2452;}); _temp2451->tl=
0; _temp2451;}); _temp2450;}); _temp2449;}); break; case 191: _LL2448: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2454=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2454[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2455; _temp2455.tag= Cyc_InitializerList_tok;
_temp2455.f1=({ struct Cyc_List_List* _temp2456=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2456->hd=( void*)({ struct
_tuple20* _temp2457=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2457->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2457->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2457;});
_temp2456->tl= 0; _temp2456;}); _temp2455;}); _temp2454;}); break; case 192:
_LL2453: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2459=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2459[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2460; _temp2460.tag=
Cyc_InitializerList_tok; _temp2460.f1=({ struct Cyc_List_List* _temp2461=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2461->hd=(
void*)({ struct _tuple20* _temp2462=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2462->f1= 0; _temp2462->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2462;}); _temp2461->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2461;}); _temp2460;}); _temp2459;}); break; case 193:
_LL2458: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2464=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2464[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2465; _temp2465.tag=
Cyc_InitializerList_tok; _temp2465.f1=({ struct Cyc_List_List* _temp2466=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2466->hd=(
void*)({ struct _tuple20* _temp2467=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2467->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2467->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2467;});
_temp2466->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2466;});
_temp2465;}); _temp2464;}); break; case 194: _LL2463: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2469=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2469[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2470; _temp2470.tag= Cyc_DesignatorList_tok;
_temp2470.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])); _temp2470;}); _temp2469;}); break; case 195: _LL2468:
yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp2472=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2472[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2473; _temp2473.tag= Cyc_DesignatorList_tok;
_temp2473.f1=({ struct Cyc_List_List* _temp2474=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2474->hd=( void*) Cyc_yyget_Designator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2474->tl= 0;
_temp2474;}); _temp2473;}); _temp2472;}); break; case 196: _LL2471: yyval=( void*)({
struct Cyc_DesignatorList_tok_struct* _temp2476=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2476[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2477; _temp2477.tag= Cyc_DesignatorList_tok;
_temp2477.f1=({ struct Cyc_List_List* _temp2478=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2478->hd=( void*) Cyc_yyget_Designator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2478->tl=
Cyc_yyget_DesignatorList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2478;}); _temp2477;}); _temp2476;}); break; case 197:
_LL2475: yyval=( void*)({ struct Cyc_Designator_tok_struct* _temp2480=( struct
Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp2480[ 0]=({ struct Cyc_Designator_tok_struct _temp2481; _temp2481.tag= Cyc_Designator_tok;
_temp2481.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2482=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2482[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2483; _temp2483.tag=
Cyc_Absyn_ArrayElement; _temp2483.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2483;});
_temp2482;})); _temp2481;}); _temp2480;}); break; case 198: _LL2479: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2485=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2485[ 0]=({ struct
Cyc_Designator_tok_struct _temp2486; _temp2486.tag= Cyc_Designator_tok;
_temp2486.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2487=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2487[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2488; _temp2488.tag= Cyc_Absyn_FieldName;
_temp2488.f1=({ struct _tagged_arr* _temp2489=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2489[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2489;}); _temp2488;});
_temp2487;})); _temp2486;}); _temp2485;}); break; case 199: _LL2484: { void*
_temp2491= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2492=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2492 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2493=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2494=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2494[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2495; _temp2495.tag= Cyc_ParamDecl_tok; _temp2495.f1=({
struct _tuple2* _temp2496=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp2496[ 0]=({ struct _tuple2 _temp2497; _temp2497.f1= 0; _temp2497.f2=
_temp2493; _temp2497.f3= _temp2491; _temp2497;}); _temp2496;}); _temp2495;});
_temp2494;}); break;}} case 200: _LL2490: { void* _temp2499= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2500=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2501=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2502=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2503= Cyc_Parse_apply_tms( _temp2501, _temp2499, _temp2500,
_temp2502); if( _temp2503.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2503.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring", sizeof(
unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2504=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2504[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2505; _temp2505.tag= Cyc_ParamDecl_tok; _temp2505.f1=({
struct _tuple2* _temp2506=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2506->f1= 0; _temp2506->f2= _temp2503.f1; _temp2506->f3= _temp2503.f2;
_temp2506;}); _temp2505;}); _temp2504;}); break;} case 201: _LL2498: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2508=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2508[ 0]=({ struct Cyc_Type_tok_struct
_temp2509; _temp2509.tag= Cyc_Type_tok; _temp2509.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2509;});
_temp2508;}); break; case 202: _LL2507: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2511=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2511[ 0]=({ struct Cyc_Type_tok_struct _temp2512; _temp2512.tag= Cyc_Type_tok;
_temp2512.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2513=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2513[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2514; _temp2514.tag= Cyc_Absyn_JoinEff;
_temp2514.f1= 0; _temp2514;}); _temp2513;})); _temp2512;}); _temp2511;}); break;
case 203: _LL2510: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2516=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2516[ 0]=({ struct Cyc_Type_tok_struct _temp2517; _temp2517.tag= Cyc_Type_tok;
_temp2517.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2518=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2518[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2519; _temp2519.tag= Cyc_Absyn_JoinEff;
_temp2519.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2519;}); _temp2518;})); _temp2517;});
_temp2516;}); break; case 204: _LL2515: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2521=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2521[ 0]=({ struct Cyc_Type_tok_struct _temp2522; _temp2522.tag= Cyc_Type_tok;
_temp2522.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2523=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2523[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2524; _temp2524.tag= Cyc_Absyn_JoinEff;
_temp2524.f1=({ struct Cyc_List_List* _temp2525=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2525->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2525->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2525;}); _temp2524;}); _temp2523;})); _temp2522;});
_temp2521;}); break; case 205: _LL2520: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2527=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2527[ 0]=({ struct Cyc_TypeList_tok_struct _temp2528; _temp2528.tag= Cyc_TypeList_tok;
_temp2528.f1=({ struct Cyc_List_List* _temp2529=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2529->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2529->tl= 0;
_temp2529;}); _temp2528;}); _temp2527;}); break; case 206: _LL2526: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2531=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2531[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2532; _temp2532.tag= Cyc_TypeList_tok; _temp2532.f1=({ struct Cyc_List_List*
_temp2533=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2533->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2533->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2533;});
_temp2532;}); _temp2531;}); break; case 207: _LL2530: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2535=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2535[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2536; _temp2536.tag= Cyc_AbstractDeclarator_tok;
_temp2536.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2537=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2537->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2537;});
_temp2536;}); _temp2535;}); break; case 208: _LL2534: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 209: _LL2538:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2540=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2540[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2541; _temp2541.tag=
Cyc_AbstractDeclarator_tok; _temp2541.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2542=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2542->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2542;});
_temp2541;}); _temp2540;}); break; case 210: _LL2539: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 211:
_LL2543: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2545=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2545[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2546; _temp2546.tag=
Cyc_AbstractDeclarator_tok; _temp2546.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2547=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2547->tms=({ struct Cyc_List_List* _temp2548=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2548->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2548->tl= 0; _temp2548;}); _temp2547;}); _temp2546;}); _temp2545;}); break;
case 212: _LL2544: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2550=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2550[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2551; _temp2551.tag= Cyc_AbstractDeclarator_tok; _temp2551.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2552=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2552->tms=({ struct Cyc_List_List* _temp2553=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2553->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2553->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->tms; _temp2553;});
_temp2552;}); _temp2551;}); _temp2550;}); break; case 213: _LL2549: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2555=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2555[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2556; _temp2556.tag= Cyc_AbstractDeclarator_tok;
_temp2556.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2557=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2557->tms=({
struct Cyc_List_List* _temp2558=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2558->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2559=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2559[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2560; _temp2560.tag= Cyc_Absyn_ConstArray_mod; _temp2560.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2560;});
_temp2559;})); _temp2558->tl= 0; _temp2558;}); _temp2557;}); _temp2556;});
_temp2555;}); break; case 214: _LL2554: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2562=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2562[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2563; _temp2563.tag= Cyc_AbstractDeclarator_tok; _temp2563.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2564=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2564->tms=({ struct Cyc_List_List* _temp2565=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2565->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2566=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2566[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2567; _temp2567.tag= Cyc_Absyn_ConstArray_mod;
_temp2567.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2567;}); _temp2566;})); _temp2565->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2565;}); _temp2564;}); _temp2563;}); _temp2562;}); break; case 215: _LL2561:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2569=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2569[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2570; _temp2570.tag=
Cyc_AbstractDeclarator_tok; _temp2570.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2571=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2571->tms=({ struct Cyc_List_List* _temp2572=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2572->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2573=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2573[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2574; _temp2574.tag= Cyc_Absyn_Function_mod;
_temp2574.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2575=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2575[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2576; _temp2576.tag= Cyc_Absyn_WithTypes;
_temp2576.f1= 0; _temp2576.f2= 0; _temp2576.f3= 0; _temp2576.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2576.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2576;}); _temp2575;})); _temp2574;}); _temp2573;}));
_temp2572->tl= 0; _temp2572;}); _temp2571;}); _temp2570;}); _temp2569;}); break;
case 216: _LL2568: { struct _tuple17 _temp2580; struct Cyc_List_List* _temp2581;
struct Cyc_Core_Opt* _temp2583; struct Cyc_Absyn_VarargInfo* _temp2585; int
_temp2587; struct Cyc_List_List* _temp2589; struct _tuple17* _temp2578= Cyc_yyget_ParamDeclListBool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2580=*
_temp2578; _LL2590: _temp2589= _temp2580.f1; goto _LL2588; _LL2588: _temp2587=
_temp2580.f2; goto _LL2586; _LL2586: _temp2585= _temp2580.f3; goto _LL2584;
_LL2584: _temp2583= _temp2580.f4; goto _LL2582; _LL2582: _temp2581= _temp2580.f5;
goto _LL2579; _LL2579: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2591=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2591[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2592; _temp2592.tag= Cyc_AbstractDeclarator_tok; _temp2592.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2593=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2593->tms=({ struct Cyc_List_List* _temp2594=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2594->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2595=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2595[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2596; _temp2596.tag= Cyc_Absyn_Function_mod;
_temp2596.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2597=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2597[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2598; _temp2598.tag= Cyc_Absyn_WithTypes;
_temp2598.f1= _temp2589; _temp2598.f2= _temp2587; _temp2598.f3= _temp2585;
_temp2598.f4= _temp2583; _temp2598.f5= _temp2581; _temp2598;}); _temp2597;}));
_temp2596;}); _temp2595;})); _temp2594->tl= 0; _temp2594;}); _temp2593;});
_temp2592;}); _temp2591;}); break;} case 217: _LL2577: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2600=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2600[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2601; _temp2601.tag= Cyc_AbstractDeclarator_tok;
_temp2601.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2602=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2602->tms=({
struct Cyc_List_List* _temp2603=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2603->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2604=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2604[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2605; _temp2605.tag=
Cyc_Absyn_Function_mod; _temp2605.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2606=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2606[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2607; _temp2607.tag= Cyc_Absyn_WithTypes;
_temp2607.f1= 0; _temp2607.f2= 0; _temp2607.f3= 0; _temp2607.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2607.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2607;}); _temp2606;})); _temp2605;}); _temp2604;}));
_temp2603->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->tms; _temp2603;});
_temp2602;}); _temp2601;}); _temp2600;}); break; case 218: _LL2599: { struct
_tuple17 _temp2611; struct Cyc_List_List* _temp2612; struct Cyc_Core_Opt*
_temp2614; struct Cyc_Absyn_VarargInfo* _temp2616; int _temp2618; struct Cyc_List_List*
_temp2620; struct _tuple17* _temp2609= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2611=*
_temp2609; _LL2621: _temp2620= _temp2611.f1; goto _LL2619; _LL2619: _temp2618=
_temp2611.f2; goto _LL2617; _LL2617: _temp2616= _temp2611.f3; goto _LL2615;
_LL2615: _temp2614= _temp2611.f4; goto _LL2613; _LL2613: _temp2612= _temp2611.f5;
goto _LL2610; _LL2610: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2622=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2622[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2623; _temp2623.tag= Cyc_AbstractDeclarator_tok; _temp2623.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2624=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2624->tms=({ struct Cyc_List_List* _temp2625=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2625->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2626=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2626[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2627; _temp2627.tag= Cyc_Absyn_Function_mod;
_temp2627.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2628=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2628[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2629; _temp2629.tag= Cyc_Absyn_WithTypes;
_temp2629.f1= _temp2620; _temp2629.f2= _temp2618; _temp2629.f3= _temp2616;
_temp2629.f4= _temp2614; _temp2629.f5= _temp2612; _temp2629;}); _temp2628;}));
_temp2627;}); _temp2626;})); _temp2625->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2625;}); _temp2624;}); _temp2623;}); _temp2622;}); break;} case 219:
_LL2608: { struct Cyc_List_List* _temp2631=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); yyval=(
void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2632=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2632[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2633; _temp2633.tag= Cyc_AbstractDeclarator_tok;
_temp2633.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2634=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2634->tms=({
struct Cyc_List_List* _temp2635=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2635->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2636=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2636[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2637; _temp2637.tag= Cyc_Absyn_TypeParams_mod; _temp2637.f1= _temp2631;
_temp2637.f2= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line); _temp2637.f3= 0; _temp2637;}); _temp2636;}));
_temp2635->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2635;});
_temp2634;}); _temp2633;}); _temp2632;}); break;} case 220: _LL2630:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2639=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); yyval=(
void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2640=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2640[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2641; _temp2641.tag= Cyc_AbstractDeclarator_tok;
_temp2641.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2642=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2642->tms=({
struct Cyc_List_List* _temp2643=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2643->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2644=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2644[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2645; _temp2645.tag= Cyc_Absyn_TypeParams_mod; _temp2645.f1= _temp2639;
_temp2645.f2= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line); _temp2645.f3= 0; _temp2645;}); _temp2644;}));
_temp2643->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2643;});
_temp2642;}); _temp2641;}); _temp2640;}); break;} case 221: _LL2638: yyval=(
void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2647=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2647[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2648; _temp2648.tag= Cyc_AbstractDeclarator_tok;
_temp2648.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2649=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2649->tms=({
struct Cyc_List_List* _temp2650=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2650->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp2651=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp2651[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp2652; _temp2652.tag= Cyc_Absyn_Attributes_mod; _temp2652.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2652.f2=
Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2652;}); _temp2651;})); _temp2650->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->tms;
_temp2650;}); _temp2649;}); _temp2648;}); _temp2647;}); break; case 222: _LL2646:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
223: _LL2653: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 224: _LL2654: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 225: _LL2655: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 226: _LL2656:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
227: _LL2657: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 228: _LL2658: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2670=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2670->name=({ struct
_tagged_arr* _temp2671=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2671[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2671;});
_temp2670->identity= 0; _temp2670->kind=(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp2670;}); void* t=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2668=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2668[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2669; _temp2669.tag= Cyc_Absyn_VarType; _temp2669.f1=
tv; _temp2669;}); _temp2668;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2660=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2660[ 0]=({ struct Cyc_Stmt_tok_struct _temp2661; _temp2661.tag= Cyc_Stmt_tok;
_temp2661.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2662=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2662[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2663; _temp2663.tag= Cyc_Absyn_Region_s;
_temp2663.f1= tv; _temp2663.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2664=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2664->f1=(
void*) Cyc_Absyn_Loc_n; _temp2664->f2=({ struct _tagged_arr* _temp2665=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2665[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2665;});
_temp2664;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2666=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2666[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2667; _temp2667.tag=
Cyc_Absyn_RgnHandleType; _temp2667.f1=( void*) t; _temp2667;}); _temp2666;}), 0);
_temp2663.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2663;}); _temp2662;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2661;});
_temp2660;}); break;} case 229: _LL2659: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2683=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2683->name=({ struct
_tagged_arr* _temp2684=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2684[ 0]=( struct _tagged_arr)({ struct _tagged_arr
_temp2685= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); xprintf("`%.*s", _get_arr_size( _temp2685, 1u), _temp2685.curr);});
_temp2684;}); _temp2683->identity= 0; _temp2683->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp2683;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2681=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2681[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2682; _temp2682.tag= Cyc_Absyn_VarType; _temp2682.f1=
tv; _temp2682;}); _temp2681;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2673=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2673[ 0]=({ struct Cyc_Stmt_tok_struct _temp2674; _temp2674.tag= Cyc_Stmt_tok;
_temp2674.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2675=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2675[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2676; _temp2676.tag= Cyc_Absyn_Region_s;
_temp2676.f1= tv; _temp2676.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2677=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2677->f1=(
void*) Cyc_Absyn_Loc_n; _temp2677->f2=({ struct _tagged_arr* _temp2678=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2678[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2678;});
_temp2677;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2679=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2679[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2680; _temp2680.tag=
Cyc_Absyn_RgnHandleType; _temp2680.f1=( void*) t; _temp2680;}); _temp2679;}), 0);
_temp2676.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2676;}); _temp2675;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2674;});
_temp2673;}); break;} case 230: _LL2672: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2687=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2687[ 0]=({ struct Cyc_Stmt_tok_struct _temp2688; _temp2688.tag= Cyc_Stmt_tok;
_temp2688.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2689=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2689[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2690; _temp2690.tag= Cyc_Absyn_Cut_s;
_temp2690.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2690;}); _temp2689;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2688;});
_temp2687;}); break; case 231: _LL2686: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2692=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2692[ 0]=({ struct Cyc_Stmt_tok_struct _temp2693; _temp2693.tag= Cyc_Stmt_tok;
_temp2693.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2694=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2694[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2695; _temp2695.tag= Cyc_Absyn_Splice_s;
_temp2695.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2695;}); _temp2694;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2693;});
_temp2692;}); break; case 232: _LL2691: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2697=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2697[ 0]=({ struct Cyc_Stmt_tok_struct _temp2698; _temp2698.tag= Cyc_Stmt_tok;
_temp2698.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2699=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2699[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2700; _temp2700.tag= Cyc_Absyn_Label_s;
_temp2700.f1=({ struct _tagged_arr* _temp2701=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr) * 1); _temp2701[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2701;});
_temp2700.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2700;}); _temp2699;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2698;});
_temp2697;}); break; case 233: _LL2696: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2703=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2703[ 0]=({ struct Cyc_Stmt_tok_struct _temp2704; _temp2704.tag= Cyc_Stmt_tok;
_temp2704.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2704;});
_temp2703;}); break; case 234: _LL2702: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2706=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2706[ 0]=({ struct Cyc_Stmt_tok_struct _temp2707; _temp2707.tag= Cyc_Stmt_tok;
_temp2707.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2707;}); _temp2706;}); break; case 235: _LL2705: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2709=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2709[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2710; _temp2710.tag= Cyc_Stmt_tok; _temp2710.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2710;}); _temp2709;}); break; case 236: _LL2708: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 237:
_LL2711:{ void* _temp2713= Cyc_yyget_BlockItem_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); struct Cyc_List_List*
_temp2721; struct Cyc_Absyn_Fndecl* _temp2723; struct Cyc_Absyn_Stmt* _temp2725;
_LL2715: if(*(( int*) _temp2713) == Cyc_Parse_TopDecls_bl){ _LL2722: _temp2721=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp2713)->f1; goto _LL2716;} else{ goto
_LL2717;} _LL2717: if(*(( int*) _temp2713) == Cyc_Parse_FnDecl_bl){ _LL2724:
_temp2723=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp2713)->f1; goto _LL2718;}
else{ goto _LL2719;} _LL2719: if(*(( int*) _temp2713) == Cyc_Parse_Stmt_bl){
_LL2726: _temp2725=(( struct Cyc_Parse_Stmt_bl_struct*) _temp2713)->f1; goto
_LL2720;} else{ goto _LL2714;} _LL2716: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2727=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2727[ 0]=({ struct Cyc_Stmt_tok_struct _temp2728; _temp2728.tag= Cyc_Stmt_tok;
_temp2728.f1= Cyc_Parse_flatten_declarations( _temp2721, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2728;}); _temp2727;}); goto _LL2714; _LL2718:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2729=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2729[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2730; _temp2730.tag= Cyc_Stmt_tok; _temp2730.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2731=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2731[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2732; _temp2732.tag= Cyc_Absyn_Fn_d;
_temp2732.f1= _temp2723; _temp2732;}); _temp2731;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line))); _temp2730;}); _temp2729;}); goto _LL2714; _LL2720:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2733=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2733[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2734; _temp2734.tag= Cyc_Stmt_tok; _temp2734.f1= _temp2725; _temp2734;});
_temp2733;}); goto _LL2714; _LL2714:;} break; case 238: _LL2712:{ void*
_temp2736= Cyc_yyget_BlockItem_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); struct Cyc_List_List* _temp2744; struct Cyc_Absyn_Fndecl*
_temp2746; struct Cyc_Absyn_Stmt* _temp2748; _LL2738: if(*(( int*) _temp2736) ==
Cyc_Parse_TopDecls_bl){ _LL2745: _temp2744=(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp2736)->f1; goto _LL2739;} else{ goto _LL2740;} _LL2740: if(*(( int*)
_temp2736) == Cyc_Parse_FnDecl_bl){ _LL2747: _temp2746=(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp2736)->f1; goto _LL2741;} else{ goto _LL2742;} _LL2742: if(*(( int*)
_temp2736) == Cyc_Parse_Stmt_bl){ _LL2749: _temp2748=(( struct Cyc_Parse_Stmt_bl_struct*)
_temp2736)->f1; goto _LL2743;} else{ goto _LL2737;} _LL2739: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2750=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2750[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2751; _temp2751.tag= Cyc_Stmt_tok; _temp2751.f1= Cyc_Parse_flatten_declarations(
_temp2744, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2751;}); _temp2750;}); goto _LL2737; _LL2741: yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2752=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2752[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2753; _temp2753.tag= Cyc_Stmt_tok; _temp2753.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2754=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2754[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2755; _temp2755.tag= Cyc_Absyn_Fn_d;
_temp2755.f1= _temp2746; _temp2755;}); _temp2754;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2753;}); _temp2752;}); goto _LL2737; _LL2743: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2756=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2756[ 0]=({ struct Cyc_Stmt_tok_struct _temp2757; _temp2757.tag= Cyc_Stmt_tok;
_temp2757.f1= Cyc_Absyn_seq_stmt( _temp2748, Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2757;}); _temp2756;}); goto _LL2737; _LL2737:;} break; case 239: _LL2735:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2759=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp2759[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp2760; _temp2760.tag= Cyc_BlockItem_tok; _temp2760.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp2761=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp2761[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp2762; _temp2762.tag= Cyc_Parse_TopDecls_bl;
_temp2762.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2762;}); _temp2761;})); _temp2760;}); _temp2759;});
break; case 240: _LL2758: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp2764=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2764[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2765; _temp2765.tag= Cyc_BlockItem_tok;
_temp2765.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp2766=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp2766[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp2767; _temp2767.tag= Cyc_Parse_Stmt_bl;
_temp2767.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2767;}); _temp2766;})); _temp2765;}); _temp2764;}); break;
case 241: _LL2763: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp2769=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp2769[ 0]=({ struct Cyc_BlockItem_tok_struct _temp2770; _temp2770.tag= Cyc_BlockItem_tok;
_temp2770.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp2771=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp2771[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp2772; _temp2772.tag= Cyc_Parse_FnDecl_bl;
_temp2772.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2772;}); _temp2771;})); _temp2770;}); _temp2769;}); break;
case 242: _LL2768: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2774=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2774[ 0]=({ struct Cyc_Stmt_tok_struct _temp2775; _temp2775.tag= Cyc_Stmt_tok;
_temp2775.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_skip_stmt(
0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2775;}); _temp2774;}); break; case 243:
_LL2773: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2777=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2777[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2778; _temp2778.tag= Cyc_Stmt_tok; _temp2778.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2778;});
_temp2777;}); break; case 244: _LL2776: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2780=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2780[ 0]=({ struct Cyc_Stmt_tok_struct _temp2781; _temp2781.tag= Cyc_Stmt_tok;
_temp2781.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2781;}); _temp2780;}); break; case 245: _LL2779: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2783=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2783[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2784; _temp2784.tag= Cyc_Stmt_tok; _temp2784.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2785=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2785[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2786; _temp2786.tag= Cyc_Absyn_SwitchC_s;
_temp2786.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2786.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2786;});
_temp2785;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2784;}); _temp2783;}); break; case 246:
_LL2782: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2788=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2788[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2789; _temp2789.tag= Cyc_Stmt_tok; _temp2789.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2789;});
_temp2788;}); break; case 247: _LL2787: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2791=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2791[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2792; _temp2792.tag= Cyc_SwitchClauseList_tok; _temp2792.f1= 0; _temp2792;});
_temp2791;}); break; case 248: _LL2790: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2794=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2794[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2795; _temp2795.tag= Cyc_SwitchClauseList_tok; _temp2795.f1=({ struct Cyc_List_List*
_temp2796=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2796->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2797=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2797->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp2797->pat_vars= 0; _temp2797->where_clause= 0; _temp2797->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2797->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2797;}); _temp2796->tl= 0; _temp2796;});
_temp2795;}); _temp2794;}); break; case 249: _LL2793: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2799=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2799[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2800; _temp2800.tag= Cyc_SwitchClauseList_tok;
_temp2800.f1=({ struct Cyc_List_List* _temp2801=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2801->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2802=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2802->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2802->pat_vars= 0; _temp2802->where_clause= 0;
_temp2802->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2802->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2802;});
_temp2801->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2801;}); _temp2800;});
_temp2799;}); break; case 250: _LL2798: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2804=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2804[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2805; _temp2805.tag= Cyc_SwitchClauseList_tok; _temp2805.f1=({ struct Cyc_List_List*
_temp2806=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2806->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2807=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2807->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2807->pat_vars=
0; _temp2807->where_clause= 0; _temp2807->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2807->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp2807;}); _temp2806->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2806;}); _temp2805;});
_temp2804;}); break; case 251: _LL2803: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2809=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2809[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2810; _temp2810.tag= Cyc_SwitchClauseList_tok; _temp2810.f1=({ struct Cyc_List_List*
_temp2811=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2811->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2812=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2812->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2812->pat_vars=
0; _temp2812->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2812->body=
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2812->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2812;});
_temp2811->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2811;}); _temp2810;});
_temp2809;}); break; case 252: _LL2808: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2814=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2814[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2815; _temp2815.tag= Cyc_SwitchClauseList_tok; _temp2815.f1=({ struct Cyc_List_List*
_temp2816=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2816->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2817=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2817->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _temp2817->pat_vars=
0; _temp2817->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2817->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2817->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2817;});
_temp2816->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2816;}); _temp2815;});
_temp2814;}); break; case 253: _LL2813: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2819=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2819[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2820; _temp2820.tag= Cyc_SwitchCClauseList_tok; _temp2820.f1= 0; _temp2820;});
_temp2819;}); break; case 254: _LL2818: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2822=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2822[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2823; _temp2823.tag= Cyc_SwitchCClauseList_tok; _temp2823.f1=({ struct Cyc_List_List*
_temp2824=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2824->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2825=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2825->cnst_exp= 0;
_temp2825->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2825->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2825;});
_temp2824->tl= 0; _temp2824;}); _temp2823;}); _temp2822;}); break; case 255:
_LL2821: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2827=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2827[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2828; _temp2828.tag=
Cyc_SwitchCClauseList_tok; _temp2828.f1=({ struct Cyc_List_List* _temp2829=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2829->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2830=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2830->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2830->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2830->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2830;});
_temp2829->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2829;}); _temp2828;});
_temp2827;}); break; case 256: _LL2826: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2832=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2832[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2833; _temp2833.tag= Cyc_SwitchCClauseList_tok; _temp2833.f1=({ struct Cyc_List_List*
_temp2834=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2834->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2835=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2835->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _temp2835->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2835->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2835;});
_temp2834->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2834;}); _temp2833;});
_temp2832;}); break; case 257: _LL2831: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2837=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2837[ 0]=({ struct Cyc_Stmt_tok_struct _temp2838; _temp2838.tag= Cyc_Stmt_tok;
_temp2838.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2838;}); _temp2837;}); break; case 258: _LL2836: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2840=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2840[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2841; _temp2841.tag= Cyc_Stmt_tok; _temp2841.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2841;}); _temp2840;}); break; case 259: _LL2839: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2843=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2843[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2844; _temp2844.tag= Cyc_Stmt_tok; _temp2844.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2844;}); _temp2843;}); break; case 260: _LL2842: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2846=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2846[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2847; _temp2847.tag= Cyc_Stmt_tok; _temp2847.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2847;}); _temp2846;}); break; case 261: _LL2845: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2849=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2849[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2850; _temp2850.tag= Cyc_Stmt_tok; _temp2850.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2850;}); _temp2849;}); break; case 262: _LL2848: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2852=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2852[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2853; _temp2853.tag= Cyc_Stmt_tok; _temp2853.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2853;}); _temp2852;}); break; case 263: _LL2851: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2855=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2855[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2856; _temp2856.tag= Cyc_Stmt_tok; _temp2856.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2856;}); _temp2855;}); break; case 264: _LL2854: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2858=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2858[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2859; _temp2859.tag= Cyc_Stmt_tok; _temp2859.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2859;});
_temp2858;}); break; case 265: _LL2857: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2861=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2861[ 0]=({ struct Cyc_Stmt_tok_struct _temp2862; _temp2862.tag= Cyc_Stmt_tok;
_temp2862.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2862;});
_temp2861;}); break; case 266: _LL2860: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2864=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2864[ 0]=({ struct Cyc_Stmt_tok_struct _temp2865; _temp2865.tag= Cyc_Stmt_tok;
_temp2865.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2865;}); _temp2864;}); break; case 267: _LL2863: { struct Cyc_List_List*
_temp2867= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); struct Cyc_Absyn_Stmt* _temp2868= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2869=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2869[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2870; _temp2870.tag= Cyc_Stmt_tok; _temp2870.f1= Cyc_Parse_flatten_declarations(
_temp2867, _temp2868); _temp2870;}); _temp2869;}); break;} case 268: _LL2866: {
struct Cyc_List_List* _temp2872= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2873= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2874=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2874[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2875; _temp2875.tag= Cyc_Stmt_tok; _temp2875.f1= Cyc_Parse_flatten_declarations(
_temp2872, _temp2873); _temp2875;}); _temp2874;}); break;} case 269: _LL2871: {
struct Cyc_List_List* _temp2877= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2878= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2879=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2879[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2880; _temp2880.tag= Cyc_Stmt_tok; _temp2880.f1= Cyc_Parse_flatten_declarations(
_temp2877, _temp2878); _temp2880;}); _temp2879;}); break;} case 270: _LL2876: {
struct Cyc_List_List* _temp2882= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); struct Cyc_Absyn_Stmt*
_temp2883= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2884=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2884[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2885; _temp2885.tag= Cyc_Stmt_tok; _temp2885.f1= Cyc_Parse_flatten_declarations(
_temp2882, _temp2883); _temp2885;}); _temp2884;}); break;} case 271: _LL2881:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2887=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2887[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2888; _temp2888.tag= Cyc_Stmt_tok; _temp2888.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2889=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2889[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2889;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2888;}); _temp2887;}); break; case 272: _LL2886: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2891=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2891[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2892; _temp2892.tag= Cyc_Stmt_tok; _temp2892.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2892;}); _temp2891;}); break; case 273:
_LL2890: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2894=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2894[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2895; _temp2895.tag= Cyc_Stmt_tok; _temp2895.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2895;}); _temp2894;}); break; case 274: _LL2893: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2897=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2897[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2898; _temp2898.tag= Cyc_Stmt_tok; _temp2898.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2898;}); _temp2897;}); break; case 275:
_LL2896: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2900=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2900[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2901; _temp2901.tag= Cyc_Stmt_tok; _temp2901.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2901;}); _temp2900;}); break; case 276: _LL2899: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2903=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2903[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2904; _temp2904.tag= Cyc_Stmt_tok; _temp2904.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2904;}); _temp2903;}); break; case 277:
_LL2902: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2906=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2906[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2907; _temp2907.tag= Cyc_Stmt_tok; _temp2907.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); _temp2907;}); _temp2906;}); break; case 278:
_LL2905: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2909=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2909[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2910; _temp2910.tag= Cyc_Stmt_tok; _temp2910.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));
_temp2910;}); _temp2909;}); break; case 279: _LL2908: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2912=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2912[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2913; _temp2913.tag= Cyc_Pattern_tok; _temp2913.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2913;});
_temp2912;}); break; case 280: _LL2911: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 281:
_LL2914: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2916=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2916[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2917; _temp2917.tag= Cyc_Pattern_tok; _temp2917.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2918=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2918[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2919; _temp2919.tag= Cyc_Absyn_Int_p; _temp2919.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2919.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2919;}); _temp2918;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2917;});
_temp2916;}); break; case 282: _LL2915: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2921=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2921[ 0]=({ struct Cyc_Pattern_tok_struct _temp2922; _temp2922.tag= Cyc_Pattern_tok;
_temp2922.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2923=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2923[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2924; _temp2924.tag= Cyc_Absyn_Int_p;
_temp2924.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2924.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2924;});
_temp2923;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2922;}); _temp2921;}); break; case 283:
_LL2920: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2926=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2926[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2927; _temp2927.tag= Cyc_Pattern_tok; _temp2927.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2928=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2928[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2929; _temp2929.tag= Cyc_Absyn_Float_p; _temp2929.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2929;}); _temp2928;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2927;});
_temp2926;}); break; case 284: _LL2925: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2931=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2931[ 0]=({ struct Cyc_Pattern_tok_struct _temp2932; _temp2932.tag= Cyc_Pattern_tok;
_temp2932.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2933=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp2933[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp2934; _temp2934.tag= Cyc_Absyn_Char_p;
_temp2934.f1= Cyc_yyget_Char_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2934;}); _temp2933;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2932;});
_temp2931;}); break; case 285: _LL2930: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2936=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2936[ 0]=({ struct Cyc_Pattern_tok_struct _temp2937; _temp2937.tag= Cyc_Pattern_tok;
_temp2937.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2937;});
_temp2936;}); break; case 286: _LL2935: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2939=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2939[ 0]=({ struct Cyc_Pattern_tok_struct _temp2940; _temp2940.tag= Cyc_Pattern_tok;
_temp2940.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2941=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2941[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2942; _temp2942.tag=
Cyc_Absyn_UnknownId_p; _temp2942.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2942;}); _temp2941;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2940;}); _temp2939;}); break; case 287:
_LL2938: { struct Cyc_List_List* _temp2944=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2945=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2945[ 0]=({ struct Cyc_Pattern_tok_struct _temp2946; _temp2946.tag= Cyc_Pattern_tok;
_temp2946.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2947=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2947[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2948; _temp2948.tag= Cyc_Absyn_UnknownCall_p; _temp2948.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2948.f2=
_temp2944; _temp2948.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2948;});
_temp2947;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2946;}); _temp2945;}); break;} case
288: _LL2943: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2950=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2950[ 0]=({ struct Cyc_Pattern_tok_struct _temp2951; _temp2951.tag= Cyc_Pattern_tok;
_temp2951.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2952=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2952[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2953; _temp2953.tag= Cyc_Absyn_Tuple_p;
_temp2953.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2953;}); _temp2952;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2951;}); _temp2950;}); break; case 289: _LL2949: { struct Cyc_List_List*
_temp2955=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp2956=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2956[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2957; _temp2957.tag= Cyc_Pattern_tok; _temp2957.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2958=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2958[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2959; _temp2959.tag= Cyc_Absyn_UnknownFields_p;
_temp2959.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp2959.f2= _temp2955; _temp2959.f3= 0; _temp2959;});
_temp2958;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2957;}); _temp2956;}); break;} case
290: _LL2954: { struct Cyc_List_List* _temp2961=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2962=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2962[ 0]=({ struct Cyc_Pattern_tok_struct _temp2963; _temp2963.tag= Cyc_Pattern_tok;
_temp2963.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2964=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2964[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2965; _temp2965.tag= Cyc_Absyn_UnknownFields_p; _temp2965.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2965.f2=
_temp2961; _temp2965.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2965;});
_temp2964;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2963;}); _temp2962;}); break;} case
291: _LL2960: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2967=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2967[ 0]=({ struct Cyc_Pattern_tok_struct _temp2968; _temp2968.tag= Cyc_Pattern_tok;
_temp2968.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2969=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2969[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2970; _temp2970.tag= Cyc_Absyn_Pointer_p;
_temp2970.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2970;}); _temp2969;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2968;});
_temp2967;}); break; case 292: _LL2966: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2972=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2972[ 0]=({ struct Cyc_Pattern_tok_struct _temp2973; _temp2973.tag= Cyc_Pattern_tok;
_temp2973.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2974=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2974[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2975; _temp2975.tag=
Cyc_Absyn_Reference_p; _temp2975.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2976=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2976->f1=(
void*) Cyc_Absyn_Loc_n; _temp2976->f2=({ struct _tagged_arr* _temp2977=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2977[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2977;});
_temp2976;}),( void*) Cyc_Absyn_VoidType, 0); _temp2975;}); _temp2974;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2973;}); _temp2972;}); break; case 293: _LL2971: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2979=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2979[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2980; _temp2980.tag= Cyc_PatternList_tok;
_temp2980.f1= 0; _temp2980;}); _temp2979;}); break; case 294: _LL2978: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2982=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2982[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2983; _temp2983.tag= Cyc_PatternList_tok;
_temp2983.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2983;}); _temp2982;}); break; case 295: _LL2981: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2985=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2985[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2986; _temp2986.tag= Cyc_PatternList_tok;
_temp2986.f1=({ struct Cyc_List_List* _temp2987=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2987->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2987->tl= 0;
_temp2987;}); _temp2986;}); _temp2985;}); break; case 296: _LL2984: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2989=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2989[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2990; _temp2990.tag= Cyc_PatternList_tok;
_temp2990.f1=({ struct Cyc_List_List* _temp2991=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2991->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2991->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2991;}); _temp2990;}); _temp2989;}); break; case 297:
_LL2988: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2993=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2993[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2994; _temp2994.tag=
Cyc_FieldPattern_tok; _temp2994.f1=({ struct _tuple14* _temp2995=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2995->f1= 0; _temp2995->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2995;}); _temp2994;}); _temp2993;}); break; case 298:
_LL2992: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2997=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2997[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2998; _temp2998.tag=
Cyc_FieldPattern_tok; _temp2998.f1=({ struct _tuple14* _temp2999=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2999->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2999->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2999;}); _temp2998;}); _temp2997;}); break; case 299:
_LL2996: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3001=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp3001[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp3002; _temp3002.tag=
Cyc_FieldPatternList_tok; _temp3002.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3002;});
_temp3001;}); break; case 300: _LL3000: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp3004=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp3004[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp3005; _temp3005.tag= Cyc_FieldPatternList_tok; _temp3005.f1=({ struct Cyc_List_List*
_temp3006=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3006->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3006->tl= 0;
_temp3006;}); _temp3005;}); _temp3004;}); break; case 301: _LL3003: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp3008=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3008[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3009; _temp3009.tag= Cyc_FieldPatternList_tok;
_temp3009.f1=({ struct Cyc_List_List* _temp3010=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3010->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3010->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp3010;}); _temp3009;}); _temp3008;}); break; case 302:
_LL3007: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 303: _LL3011: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3013=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3013[ 0]=({ struct Cyc_Exp_tok_struct _temp3014; _temp3014.tag= Cyc_Exp_tok;
_temp3014.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3014;}); _temp3013;}); break; case 304: _LL3012: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 305: _LL3015:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3017=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3017[ 0]=({ struct Cyc_Exp_tok_struct
_temp3018; _temp3018.tag= Cyc_Exp_tok; _temp3018.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3018;}); _temp3017;}); break; case 306: _LL3016: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3020=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3020[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3021; _temp3021.tag= Cyc_Primopopt_tok; _temp3021.f1=
0; _temp3021;}); _temp3020;}); break; case 307: _LL3019: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3023=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3023[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3024; _temp3024.tag= Cyc_Primopopt_tok; _temp3024.f1=({
struct Cyc_Core_Opt* _temp3025=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3025->v=( void*)(( void*) Cyc_Absyn_Times); _temp3025;});
_temp3024;}); _temp3023;}); break; case 308: _LL3022: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3027=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3027[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3028; _temp3028.tag= Cyc_Primopopt_tok; _temp3028.f1=({
struct Cyc_Core_Opt* _temp3029=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3029->v=( void*)(( void*) Cyc_Absyn_Div); _temp3029;});
_temp3028;}); _temp3027;}); break; case 309: _LL3026: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3031=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3031[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3032; _temp3032.tag= Cyc_Primopopt_tok; _temp3032.f1=({
struct Cyc_Core_Opt* _temp3033=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3033->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3033;});
_temp3032;}); _temp3031;}); break; case 310: _LL3030: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3035=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3035[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3036; _temp3036.tag= Cyc_Primopopt_tok; _temp3036.f1=({
struct Cyc_Core_Opt* _temp3037=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3037->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3037;});
_temp3036;}); _temp3035;}); break; case 311: _LL3034: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3039=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3039[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3040; _temp3040.tag= Cyc_Primopopt_tok; _temp3040.f1=({
struct Cyc_Core_Opt* _temp3041=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3041->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3041;});
_temp3040;}); _temp3039;}); break; case 312: _LL3038: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3043=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3043[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3044; _temp3044.tag= Cyc_Primopopt_tok; _temp3044.f1=({
struct Cyc_Core_Opt* _temp3045=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3045->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3045;});
_temp3044;}); _temp3043;}); break; case 313: _LL3042: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3047=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3047[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3048; _temp3048.tag= Cyc_Primopopt_tok; _temp3048.f1=({
struct Cyc_Core_Opt* _temp3049=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3049->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3049;});
_temp3048;}); _temp3047;}); break; case 314: _LL3046: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3051=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3051[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3052; _temp3052.tag= Cyc_Primopopt_tok; _temp3052.f1=({
struct Cyc_Core_Opt* _temp3053=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3053->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3053;});
_temp3052;}); _temp3051;}); break; case 315: _LL3050: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3055=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3055[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3056; _temp3056.tag= Cyc_Primopopt_tok; _temp3056.f1=({
struct Cyc_Core_Opt* _temp3057=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3057->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3057;});
_temp3056;}); _temp3055;}); break; case 316: _LL3054: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3059=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3059[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3060; _temp3060.tag= Cyc_Primopopt_tok; _temp3060.f1=({
struct Cyc_Core_Opt* _temp3061=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3061->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3061;});
_temp3060;}); _temp3059;}); break; case 317: _LL3058: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 318: _LL3062:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3064=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3064[ 0]=({ struct Cyc_Exp_tok_struct
_temp3065; _temp3065.tag= Cyc_Exp_tok; _temp3065.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3065;});
_temp3064;}); break; case 319: _LL3063: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3067=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3067[ 0]=({ struct Cyc_Exp_tok_struct _temp3068; _temp3068.tag= Cyc_Exp_tok;
_temp3068.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3068;}); _temp3067;}); break; case 320: _LL3066: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3071;}); _temp3070;}); break; case 321: _LL3069: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3073=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3073[ 0]=({ struct Cyc_Exp_tok_struct
_temp3074; _temp3074.tag= Cyc_Exp_tok; _temp3074.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp3074;}); _temp3073;}); break; case 322: _LL3072: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3076=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3076[ 0]=({ struct Cyc_Exp_tok_struct
_temp3077; _temp3077.tag= Cyc_Exp_tok; _temp3077.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3077;});
_temp3076;}); break; case 323: _LL3075: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3079=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3079[ 0]=({ struct Cyc_Exp_tok_struct _temp3080; _temp3080.tag= Cyc_Exp_tok;
_temp3080.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3080;}); _temp3079;}); break; case 324: _LL3078: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 325: _LL3081:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
326: _LL3082: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3084=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3084[ 0]=({ struct Cyc_Exp_tok_struct
_temp3085; _temp3085.tag= Cyc_Exp_tok; _temp3085.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3085;}); _temp3084;}); break; case 327: _LL3083: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 328: _LL3086:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3088=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3088[ 0]=({ struct Cyc_Exp_tok_struct
_temp3089; _temp3089.tag= Cyc_Exp_tok; _temp3089.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3089;}); _temp3088;}); break; case 329: _LL3087: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 330: _LL3090:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3092=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3092[ 0]=({ struct Cyc_Exp_tok_struct
_temp3093; _temp3093.tag= Cyc_Exp_tok; _temp3093.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3093;});
_temp3092;}); break; case 331: _LL3091: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 332: _LL3094:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3096=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3096[ 0]=({ struct Cyc_Exp_tok_struct
_temp3097; _temp3097.tag= Cyc_Exp_tok; _temp3097.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3097;});
_temp3096;}); break; case 333: _LL3095: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 334: _LL3098:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3100=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3100[ 0]=({ struct Cyc_Exp_tok_struct
_temp3101; _temp3101.tag= Cyc_Exp_tok; _temp3101.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3101;});
_temp3100;}); break; case 335: _LL3099: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 336: _LL3102:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3104=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3104[ 0]=({ struct Cyc_Exp_tok_struct
_temp3105; _temp3105.tag= Cyc_Exp_tok; _temp3105.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3105;}); _temp3104;}); break; case 337: _LL3103: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3107=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3107[ 0]=({ struct Cyc_Exp_tok_struct
_temp3108; _temp3108.tag= Cyc_Exp_tok; _temp3108.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3108;}); _temp3107;}); break; case 338: _LL3106: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 339: _LL3109:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3111=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3111[ 0]=({ struct Cyc_Exp_tok_struct
_temp3112; _temp3112.tag= Cyc_Exp_tok; _temp3112.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3112;}); _temp3111;}); break; case 340: _LL3110: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3114=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3114[ 0]=({ struct Cyc_Exp_tok_struct
_temp3115; _temp3115.tag= Cyc_Exp_tok; _temp3115.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3115;}); _temp3114;}); break; case 341: _LL3113: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3117=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3117[ 0]=({ struct Cyc_Exp_tok_struct
_temp3118; _temp3118.tag= Cyc_Exp_tok; _temp3118.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3118;}); _temp3117;}); break; case 342: _LL3116: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3120=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3120[ 0]=({ struct Cyc_Exp_tok_struct
_temp3121; _temp3121.tag= Cyc_Exp_tok; _temp3121.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3121;}); _temp3120;}); break; case 343: _LL3119: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 344: _LL3122:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3124=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3124[ 0]=({ struct Cyc_Exp_tok_struct
_temp3125; _temp3125.tag= Cyc_Exp_tok; _temp3125.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3125;}); _temp3124;}); break; case 345: _LL3123: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3127=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3127[ 0]=({ struct Cyc_Exp_tok_struct
_temp3128; _temp3128.tag= Cyc_Exp_tok; _temp3128.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3128;}); _temp3127;}); break; case 346: _LL3126: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 347: _LL3129:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3131=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3131[ 0]=({ struct Cyc_Exp_tok_struct
_temp3132; _temp3132.tag= Cyc_Exp_tok; _temp3132.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3132;});
_temp3131;}); break; case 348: _LL3130: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3134=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3134[ 0]=({ struct Cyc_Exp_tok_struct _temp3135; _temp3135.tag= Cyc_Exp_tok;
_temp3135.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3135;}); _temp3134;}); break; case 349: _LL3133: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 350: _LL3136:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3138=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3138[ 0]=({ struct Cyc_Exp_tok_struct
_temp3139; _temp3139.tag= Cyc_Exp_tok; _temp3139.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3139;});
_temp3138;}); break; case 351: _LL3137: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3141=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3141[ 0]=({ struct Cyc_Exp_tok_struct _temp3142; _temp3142.tag= Cyc_Exp_tok;
_temp3142.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3142;}); _temp3141;}); break; case 352: _LL3140: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3144=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3144[ 0]=({ struct Cyc_Exp_tok_struct
_temp3145; _temp3145.tag= Cyc_Exp_tok; _temp3145.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3145;});
_temp3144;}); break; case 353: _LL3143: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 354: _LL3146:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3148=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3148[ 0]=({ struct Cyc_Exp_tok_struct
_temp3149; _temp3149.tag= Cyc_Exp_tok; _temp3149.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3149;}); _temp3148;}); break; case 355: _LL3147: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 356: _LL3150:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3152=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3152[ 0]=({ struct Cyc_Exp_tok_struct
_temp3153; _temp3153.tag= Cyc_Exp_tok; _temp3153.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3153;}); _temp3152;}); break; case 357: _LL3151: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3155=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3155[ 0]=({ struct Cyc_Exp_tok_struct
_temp3156; _temp3156.tag= Cyc_Exp_tok; _temp3156.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3156;}); _temp3155;}); break; case 358: _LL3154: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3158=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3158[ 0]=({ struct Cyc_Exp_tok_struct
_temp3159; _temp3159.tag= Cyc_Exp_tok; _temp3159.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3159;}); _temp3158;}); break; case 359: _LL3157: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3161=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3161[ 0]=({ struct Cyc_Exp_tok_struct
_temp3162; _temp3162.tag= Cyc_Exp_tok; _temp3162.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3162;}); _temp3161;}); break; case 360: _LL3160: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 361: _LL3163:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3165=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3165[ 0]=({ struct Cyc_Exp_tok_struct
_temp3166; _temp3166.tag= Cyc_Exp_tok; _temp3166.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3166;}); _temp3165;}); break; case 362: _LL3164: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3168=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3168[ 0]=({ struct Cyc_Exp_tok_struct
_temp3169; _temp3169.tag= Cyc_Exp_tok; _temp3169.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3169;});
_temp3168;}); break; case 363: _LL3167: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3171=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3171[ 0]=({ struct Cyc_Exp_tok_struct _temp3172; _temp3172.tag= Cyc_Exp_tok;
_temp3172.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3172;}); _temp3171;}); break; case 364: _LL3170: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3174=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3174[ 0]=({ struct Cyc_Exp_tok_struct
_temp3175; _temp3175.tag= Cyc_Exp_tok; _temp3175.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])).f3,({ struct _tagged_arr* _temp3176=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3176[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3176;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3175;}); _temp3174;}); break; case 365:
_LL3173: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3178=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3178[ 0]=({ struct Cyc_Exp_tok_struct
_temp3179; _temp3179.tag= Cyc_Exp_tok; _temp3179.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3179;}); _temp3178;}); break; case 366: _LL3177: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3181=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3181[ 0]=({ struct Cyc_Exp_tok_struct
_temp3182; _temp3182.tag= Cyc_Exp_tok; _temp3182.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3183=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3183[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3184; _temp3184.tag= Cyc_Absyn_Malloc_e;
_temp3184.f1= 0; _temp3184.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3184;}); _temp3183;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3182;});
_temp3181;}); break; case 367: _LL3180: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3186=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3186[ 0]=({ struct Cyc_Exp_tok_struct _temp3187; _temp3187.tag= Cyc_Exp_tok;
_temp3187.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3188=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3188[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3189; _temp3189.tag= Cyc_Absyn_Malloc_e;
_temp3189.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]); _temp3189.f2=( void*)
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3189;}); _temp3188;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3187;});
_temp3186;}); break; case 368: _LL3185: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3191=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3191[ 0]=({ struct Cyc_Primop_tok_struct _temp3192; _temp3192.tag= Cyc_Primop_tok;
_temp3192.f1=( void*)(( void*) Cyc_Absyn_Printf); _temp3192;}); _temp3191;});
break; case 369: _LL3190: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3194=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3194[ 0]=({ struct Cyc_Primop_tok_struct _temp3195; _temp3195.tag= Cyc_Primop_tok;
_temp3195.f1=( void*)(( void*) Cyc_Absyn_Fprintf); _temp3195;}); _temp3194;});
break; case 370: _LL3193: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3197=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3197[ 0]=({ struct Cyc_Primop_tok_struct _temp3198; _temp3198.tag= Cyc_Primop_tok;
_temp3198.f1=( void*)(( void*) Cyc_Absyn_Xprintf); _temp3198;}); _temp3197;});
break; case 371: _LL3196: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3200=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3200[ 0]=({ struct Cyc_Primop_tok_struct _temp3201; _temp3201.tag= Cyc_Primop_tok;
_temp3201.f1=( void*)(( void*) Cyc_Absyn_Scanf); _temp3201;}); _temp3200;});
break; case 372: _LL3199: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3203=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3203[ 0]=({ struct Cyc_Primop_tok_struct _temp3204; _temp3204.tag= Cyc_Primop_tok;
_temp3204.f1=( void*)(( void*) Cyc_Absyn_Fscanf); _temp3204;}); _temp3203;});
break; case 373: _LL3202: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3206=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3206[ 0]=({ struct Cyc_Primop_tok_struct _temp3207; _temp3207.tag= Cyc_Primop_tok;
_temp3207.f1=( void*)(( void*) Cyc_Absyn_Sscanf); _temp3207;}); _temp3206;});
break; case 374: _LL3205: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3209=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3209[ 0]=({ struct Cyc_Primop_tok_struct _temp3210; _temp3210.tag= Cyc_Primop_tok;
_temp3210.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3210;}); _temp3209;});
break; case 375: _LL3208: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3212=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3212[ 0]=({ struct Cyc_Primop_tok_struct _temp3213; _temp3213.tag= Cyc_Primop_tok;
_temp3213.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3213;}); _temp3212;});
break; case 376: _LL3211: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3215=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3215[ 0]=({ struct Cyc_Primop_tok_struct _temp3216; _temp3216.tag= Cyc_Primop_tok;
_temp3216.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3216;}); _temp3215;});
break; case 377: _LL3214: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 378: _LL3217: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3219=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3219[ 0]=({ struct Cyc_Exp_tok_struct _temp3220; _temp3220.tag= Cyc_Exp_tok;
_temp3220.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3220;}); _temp3219;}); break; case 379: _LL3218: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3222=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3222[ 0]=({ struct Cyc_Exp_tok_struct
_temp3223; _temp3223.tag= Cyc_Exp_tok; _temp3223.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3223;}); _temp3222;}); break; case 380:
_LL3221: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3225=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3225[ 0]=({ struct Cyc_Exp_tok_struct
_temp3226; _temp3226.tag= Cyc_Exp_tok; _temp3226.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3226;});
_temp3225;}); break; case 381: _LL3224: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3228=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3228[ 0]=({ struct Cyc_Exp_tok_struct _temp3229; _temp3229.tag= Cyc_Exp_tok;
_temp3229.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3230=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3230[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3230;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3229;}); _temp3228;}); break; case 382: _LL3227: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3232=(*
q).f1; struct Cyc_List_List* _temp3242; struct Cyc_List_List* _temp3244; _LL3234:
if( _temp3232 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3235;} else{ goto _LL3236;}
_LL3236: if(( unsigned int) _temp3232 > 1u?*(( int*) _temp3232) == Cyc_Absyn_Rel_n:
0){ _LL3243: _temp3242=(( struct Cyc_Absyn_Rel_n_struct*) _temp3232)->f1; if(
_temp3242 == 0){ goto _LL3237;} else{ goto _LL3238;}} else{ goto _LL3238;}
_LL3238: if(( unsigned int) _temp3232 > 1u?*(( int*) _temp3232) == Cyc_Absyn_Abs_n:
0){ _LL3245: _temp3244=(( struct Cyc_Absyn_Abs_n_struct*) _temp3232)->f1; if(
_temp3244 == 0){ goto _LL3239;} else{ goto _LL3240;}} else{ goto _LL3240;}
_LL3240: goto _LL3241; _LL3235: goto _LL3233; _LL3237: goto _LL3233; _LL3239:
goto _LL3233; _LL3241: Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3233; _LL3233:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3246=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3246[ 0]=({ struct Cyc_Exp_tok_struct _temp3247; _temp3247.tag= Cyc_Exp_tok;
_temp3247.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3247;}); _temp3246;}); break;} case 383: _LL3231: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3249=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3249[ 0]=({ struct Cyc_Exp_tok_struct
_temp3250; _temp3250.tag= Cyc_Exp_tok; _temp3250.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),({ struct _tagged_arr* _temp3251=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) * 1); _temp3251[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3251;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3250;}); _temp3249;}); break; case 384: _LL3248: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3253=(*
q).f1; struct Cyc_List_List* _temp3263; struct Cyc_List_List* _temp3265; _LL3255:
if( _temp3253 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3256;} else{ goto _LL3257;}
_LL3257: if(( unsigned int) _temp3253 > 1u?*(( int*) _temp3253) == Cyc_Absyn_Rel_n:
0){ _LL3264: _temp3263=(( struct Cyc_Absyn_Rel_n_struct*) _temp3253)->f1; if(
_temp3263 == 0){ goto _LL3258;} else{ goto _LL3259;}} else{ goto _LL3259;}
_LL3259: if(( unsigned int) _temp3253 > 1u?*(( int*) _temp3253) == Cyc_Absyn_Abs_n:
0){ _LL3266: _temp3265=(( struct Cyc_Absyn_Abs_n_struct*) _temp3253)->f1; if(
_temp3265 == 0){ goto _LL3260;} else{ goto _LL3261;}} else{ goto _LL3261;}
_LL3261: goto _LL3262; _LL3256: goto _LL3254; _LL3258: goto _LL3254; _LL3260:
goto _LL3254; _LL3262: Cyc_Parse_err( _tag_arr("struct field is qualified with module name",
sizeof( unsigned char), 43u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3254; _LL3254:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3267=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3267[ 0]=({ struct Cyc_Exp_tok_struct _temp3268; _temp3268.tag= Cyc_Exp_tok;
_temp3268.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3268;}); _temp3267;}); break;} case 385: _LL3252: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3270=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3270[ 0]=({ struct Cyc_Exp_tok_struct
_temp3271; _temp3271.tag= Cyc_Exp_tok; _temp3271.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3271;}); _temp3270;}); break; case 386: _LL3269: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3273=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3273[ 0]=({ struct Cyc_Exp_tok_struct
_temp3274; _temp3274.tag= Cyc_Exp_tok; _temp3274.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3274;}); _temp3273;}); break; case 387: _LL3272: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3276=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3276[ 0]=({ struct Cyc_Exp_tok_struct
_temp3277; _temp3277.tag= Cyc_Exp_tok; _temp3277.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3278=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3278[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3279; _temp3279.tag= Cyc_Absyn_CompoundLit_e;
_temp3279.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp3279.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3279;});
_temp3278;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3277;}); _temp3276;}); break; case 388:
_LL3275: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3281=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3281[ 0]=({ struct Cyc_Exp_tok_struct
_temp3282; _temp3282.tag= Cyc_Exp_tok; _temp3282.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3283=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3283[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3284; _temp3284.tag= Cyc_Absyn_CompoundLit_e;
_temp3284.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp3284.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp3284;});
_temp3283;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3282;}); _temp3281;}); break; case 389:
_LL3280: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3286=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3286[ 0]=({ struct Cyc_Exp_tok_struct
_temp3287; _temp3287.tag= Cyc_Exp_tok; _temp3287.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3288=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3288[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3289; _temp3289.tag= Cyc_Absyn_Fill_e; _temp3289.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3289;});
_temp3288;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3287;}); _temp3286;}); break; case 390:
_LL3285: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3291=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3291[ 0]=({ struct Cyc_Exp_tok_struct
_temp3292; _temp3292.tag= Cyc_Exp_tok; _temp3292.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3293=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3293[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3294; _temp3294.tag= Cyc_Absyn_Codegen_e;
_temp3294.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp3294;}); _temp3293;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3292;}); _temp3291;}); break; case 391: _LL3290: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3296=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3296[ 0]=({ struct Cyc_Exp_tok_struct
_temp3297; _temp3297.tag= Cyc_Exp_tok; _temp3297.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3298=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3298[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3299; _temp3299.tag= Cyc_Absyn_UnknownId_e;
_temp3299.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3299;}); _temp3298;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3297;});
_temp3296;}); break; case 392: _LL3295: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 393: _LL3300:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3302=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3302[ 0]=({ struct Cyc_Exp_tok_struct
_temp3303; _temp3303.tag= Cyc_Exp_tok; _temp3303.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3303;});
_temp3302;}); break; case 394: _LL3301: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 395:
_LL3304: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3306=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3306[ 0]=({ struct Cyc_Exp_tok_struct
_temp3307; _temp3307.tag= Cyc_Exp_tok; _temp3307.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3308=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3308[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3309; _temp3309.tag=
Cyc_Absyn_UnknownId_e; _temp3309.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3309;});
_temp3308;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3307;});
_temp3306;}); break; case 396: _LL3305: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3311=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3311[ 0]=({ struct Cyc_Exp_tok_struct _temp3312; _temp3312.tag= Cyc_Exp_tok;
_temp3312.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3313=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3313[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3314; _temp3314.tag=
Cyc_Absyn_UnknownId_e; _temp3314.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp3314;});
_temp3313;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).last_line)),(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3312;}); _temp3311;}); break; case 397: _LL3310: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3316=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3316[ 0]=({ struct Cyc_Exp_tok_struct
_temp3317; _temp3317.tag= Cyc_Exp_tok; _temp3317.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3317;}); _temp3316;}); break; case 398: _LL3315: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3319=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3319[ 0]=({ struct Cyc_Exp_tok_struct
_temp3320; _temp3320.tag= Cyc_Exp_tok; _temp3320.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3321=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3321[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3322; _temp3322.tag= Cyc_Absyn_Struct_e;
_temp3322.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp3322.f2= 0; _temp3322.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3322.f4= 0;
_temp3322;}); _temp3321;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3320;});
_temp3319;}); break; case 399: _LL3318: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3324=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3324[ 0]=({ struct Cyc_Exp_tok_struct _temp3325; _temp3325.tag= Cyc_Exp_tok;
_temp3325.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3325;}); _temp3324;}); break; case 400: _LL3323: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3327=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3327[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3328; _temp3328.tag= Cyc_ExpList_tok; _temp3328.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3328;});
_temp3327;}); break; case 401: _LL3326: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3330=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3330[ 0]=({ struct Cyc_ExpList_tok_struct _temp3331; _temp3331.tag= Cyc_ExpList_tok;
_temp3331.f1=({ struct Cyc_List_List* _temp3332=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3332->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3332->tl= 0;
_temp3332;}); _temp3331;}); _temp3330;}); break; case 402: _LL3329: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3334=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3334[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3335; _temp3335.tag= Cyc_ExpList_tok; _temp3335.f1=({ struct Cyc_List_List*
_temp3336=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3336->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3336->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp3336;});
_temp3335;}); _temp3334;}); break; case 403: _LL3333: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3338=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3338[ 0]=({ struct Cyc_Exp_tok_struct
_temp3339; _temp3339.tag= Cyc_Exp_tok; _temp3339.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3339;});
_temp3338;}); break; case 404: _LL3337: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3341=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3341[ 0]=({ struct Cyc_Exp_tok_struct _temp3342; _temp3342.tag= Cyc_Exp_tok;
_temp3342.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3342;});
_temp3341;}); break; case 405: _LL3340: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3344=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3344[ 0]=({ struct Cyc_Exp_tok_struct _temp3345; _temp3345.tag= Cyc_Exp_tok;
_temp3345.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3345;});
_temp3344;}); break; case 406: _LL3343: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3347=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3347[ 0]=({ struct Cyc_Exp_tok_struct _temp3348; _temp3348.tag= Cyc_Exp_tok;
_temp3348.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3348;});
_temp3347;}); break; case 407: _LL3346: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3350=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3350[ 0]=({ struct Cyc_QualId_tok_struct _temp3351; _temp3351.tag= Cyc_QualId_tok;
_temp3351.f1=({ struct _tuple1* _temp3352=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3352->f1= Cyc_Absyn_rel_ns_null; _temp3352->f2=({ struct
_tagged_arr* _temp3353=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3353[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3353;}); _temp3352;});
_temp3351;}); _temp3350;}); break; case 408: _LL3349: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3354:(
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
unsigned int _temp3356=( unsigned int)( sze + 15); unsigned char* _temp3357=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3356); struct
_tagged_arr _temp3359= _tag_arr( _temp3357, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3358= _temp3356; unsigned int i;
for( i= 0; i < _temp3358; i ++){ _temp3357[ i]='\000';}}; _temp3359;}); Cyc_String_strcpy(
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
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3369= v; struct
_tuple16* _temp3389; struct _tuple16 _temp3391; int _temp3392; void* _temp3394;
unsigned char _temp3396; short _temp3398; struct _tagged_arr _temp3400; struct
Cyc_Core_Opt* _temp3402; struct Cyc_Core_Opt* _temp3404; struct Cyc_Core_Opt
_temp3406; struct _tagged_arr* _temp3407; struct _tuple1* _temp3409; struct
_tuple1 _temp3411; struct _tagged_arr* _temp3412; void* _temp3414; _LL3371: if(
_temp3369 == Cyc_Okay_tok){ goto _LL3372;} else{ goto _LL3373;} _LL3373: if(*((
void**) _temp3369) == Cyc_Int_tok){ _LL3390: _temp3389=(( struct Cyc_Int_tok_struct*)
_temp3369)->f1; _temp3391=* _temp3389; _LL3395: _temp3394= _temp3391.f1; goto
_LL3393; _LL3393: _temp3392= _temp3391.f2; goto _LL3374;} else{ goto _LL3375;}
_LL3375: if(*(( void**) _temp3369) == Cyc_Char_tok){ _LL3397: _temp3396=((
struct Cyc_Char_tok_struct*) _temp3369)->f1; goto _LL3376;} else{ goto _LL3377;}
_LL3377: if(*(( void**) _temp3369) == Cyc_Short_tok){ _LL3399: _temp3398=((
struct Cyc_Short_tok_struct*) _temp3369)->f1; goto _LL3378;} else{ goto _LL3379;}
_LL3379: if(*(( void**) _temp3369) == Cyc_String_tok){ _LL3401: _temp3400=((
struct Cyc_String_tok_struct*) _temp3369)->f1; goto _LL3380;} else{ goto _LL3381;}
_LL3381: if(*(( void**) _temp3369) == Cyc_Stringopt_tok){ _LL3403: _temp3402=((
struct Cyc_Stringopt_tok_struct*) _temp3369)->f1; if( _temp3402 == 0){ goto
_LL3382;} else{ goto _LL3383;}} else{ goto _LL3383;} _LL3383: if(*(( void**)
_temp3369) == Cyc_Stringopt_tok){ _LL3405: _temp3404=(( struct Cyc_Stringopt_tok_struct*)
_temp3369)->f1; if( _temp3404 == 0){ goto _LL3385;} else{ _temp3406=* _temp3404;
_LL3408: _temp3407=( struct _tagged_arr*) _temp3406.v; goto _LL3384;}} else{
goto _LL3385;} _LL3385: if(*(( void**) _temp3369) == Cyc_QualId_tok){ _LL3410:
_temp3409=(( struct Cyc_QualId_tok_struct*) _temp3369)->f1; _temp3411=*
_temp3409; _LL3415: _temp3414= _temp3411.f1; goto _LL3413; _LL3413: _temp3412=
_temp3411.f2; goto _LL3386;} else{ goto _LL3387;} _LL3387: goto _LL3388; _LL3372:
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"ok"); goto _LL3370; _LL3374: fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"%d", _temp3392); goto _LL3370; _LL3376:
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%c",( int) _temp3396); goto _LL3370;
_LL3378: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%ds",( int) _temp3398);
goto _LL3370; _LL3380:({ struct _tagged_arr _temp3416= _temp3400; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _get_arr_size( _temp3416, 1u),
_temp3416.curr);}); goto _LL3370; _LL3382: fprintf( _sfile_to_file( Cyc_Stdio_stderr),"null");
goto _LL3370; _LL3384:({ struct _tagged_arr _temp3417=* _temp3407; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _get_arr_size( _temp3417, 1u),
_temp3417.curr);}); goto _LL3370; _LL3386: { struct Cyc_List_List* _temp3418= 0;{
void* _temp3419= _temp3414; struct Cyc_List_List* _temp3427; struct Cyc_List_List*
_temp3429; _LL3421: if(( unsigned int) _temp3419 > 1u?*(( int*) _temp3419) ==
Cyc_Absyn_Rel_n: 0){ _LL3428: _temp3427=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3419)->f1; goto _LL3422;} else{ goto _LL3423;} _LL3423: if(( unsigned int)
_temp3419 > 1u?*(( int*) _temp3419) == Cyc_Absyn_Abs_n: 0){ _LL3430: _temp3429=((
struct Cyc_Absyn_Abs_n_struct*) _temp3419)->f1; goto _LL3424;} else{ goto
_LL3425;} _LL3425: if( _temp3419 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3426;}
else{ goto _LL3420;} _LL3422: _temp3418= _temp3427; goto _LL3420; _LL3424:
_temp3418= _temp3429; goto _LL3420; _LL3426: goto _LL3420; _LL3420:;} for( 0;
_temp3418 != 0; _temp3418=(( struct Cyc_List_List*) _check_null( _temp3418))->tl){({
struct _tagged_arr _temp3431=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp3418))->hd); fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s::",
_get_arr_size( _temp3431, 1u), _temp3431.curr);});}({ struct _tagged_arr
_temp3432=* _temp3412; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s::",
_get_arr_size( _temp3432, 1u), _temp3432.curr);}); goto _LL3370;} _LL3388:
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"?"); goto _LL3370; _LL3370:;} struct
Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result=
0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp3433=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3433->v=( void*) Cyc_Lexing_from_file(
f); _temp3433;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}