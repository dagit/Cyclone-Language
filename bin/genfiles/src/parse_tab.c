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
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*);
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
Cyc_List_List* x); extern int Cyc_String_strlen( struct _tagged_arr s); extern
int Cyc_String_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern int
Cyc_String_zstrcmp( struct _tagged_arr, struct _tagged_arr); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strcat(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_String_strcpy(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_String_substring(
struct _tagged_arr, int ofs, int n); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; extern struct
Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int); extern struct Cyc_Position_Segment*
Cyc_Position_segment_join( struct Cyc_Position_Segment*, struct Cyc_Position_Segment*);
extern struct _tagged_arr Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse(
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
static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static const int
Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Printf_ft=
0; static const int Cyc_Absyn_Scanf_ft= 1; static const int Cyc_Absyn_Regparm_att=
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
Cyc_Absyn_Format_att= 3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1;
int f2; int f3; } ; static const int Cyc_Absyn_Carray_mod= 0; static const int
Cyc_Absyn_ConstArray_mod= 0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Pointer_mod= 1; struct
Cyc_Absyn_Pointer_mod_struct{ int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual
f3; } ; static const int Cyc_Absyn_Function_mod= 2; struct Cyc_Absyn_Function_mod_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
static const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
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
void* t); extern void Cyc_Lex_register_typedef( struct _tuple1* s); extern void
Cyc_Lex_enter_namespace( struct _tagged_arr*); extern void Cyc_Lex_leave_namespace();
extern void Cyc_Lex_enter_using( struct _tuple1*); extern void Cyc_Lex_leave_using();
enum  Cyc_Parse_Struct_or_union{ Cyc_Parse_Struct_su  =  0u, Cyc_Parse_Union_su
 =  1u}; static const int Cyc_Parse_Signed_spec= 0; struct Cyc_Parse_Signed_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Unsigned_spec=
1; struct Cyc_Parse_Unsigned_spec_struct{ int tag; struct Cyc_Position_Segment*
f1; } ; static const int Cyc_Parse_Short_spec= 2; struct Cyc_Parse_Short_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Long_spec=
3; struct Cyc_Parse_Long_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Type_spec= 4; struct Cyc_Parse_Type_spec_struct{
int tag; void* f1; struct Cyc_Position_Segment* f2; } ; static const int Cyc_Parse_Decl_spec=
5; struct Cyc_Parse_Decl_spec_struct{ int tag; struct Cyc_Absyn_Decl* f1; } ;
enum  Cyc_Parse_Storage_class{ Cyc_Parse_Typedef_sc  =  0u, Cyc_Parse_Extern_sc
 =  1u, Cyc_Parse_ExternC_sc  =  2u, Cyc_Parse_Static_sc  =  3u, Cyc_Parse_Auto_sc
 =  4u, Cyc_Parse_Register_sc  =  5u, Cyc_Parse_Abstract_sc  =  6u}; struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual tq; struct Cyc_List_List*
type_specs; int is_inline; struct Cyc_List_List* attributes; } ; struct Cyc_Parse_Declarator{
struct _tuple1* id; struct Cyc_List_List* tms; } ; struct Cyc_Parse_Abstractdeclarator{
struct Cyc_List_List* tms; } ; struct _tuple4{ struct _tagged_arr* f1; struct
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
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ struct Cyc_Stdio_String_pa_struct
_temp19; _temp19.tag= Cyc_Stdio_String_pa; _temp19.f1=( struct _tagged_arr) msg;{
struct Cyc_Stdio_String_pa_struct _temp18; _temp18.tag= Cyc_Stdio_String_pa;
_temp18.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( sg);{ void*
_temp17[ 2u]={& _temp18,& _temp19}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: Warning: %s\n", sizeof( unsigned char), 17u), _tag_arr( _temp17,
sizeof( void*), 2u));}}}); return;} static void* Cyc_Parse_unimp( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg){ return Cyc_Parse_abort((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp21; _temp21.tag=
Cyc_Stdio_String_pa; _temp21.f1=( struct _tagged_arr) msg;{ void* _temp20[ 1u]={&
_temp21}; Cyc_Stdio_aprintf( _tag_arr("%s unimplemented", sizeof( unsigned char),
17u), _tag_arr( _temp20, sizeof( void*), 1u));}}), sg);} static void Cyc_Parse_unimp2(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg){({ struct Cyc_Stdio_String_pa_struct
_temp24; _temp24.tag= Cyc_Stdio_String_pa; _temp24.f1=( struct _tagged_arr) msg;{
struct Cyc_Stdio_String_pa_struct _temp23; _temp23.tag= Cyc_Stdio_String_pa;
_temp23.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( sg);{ void*
_temp22[ 2u]={& _temp23,& _temp24}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: Warning: Cyclone does not yet support %s\n", sizeof( unsigned char),
46u), _tag_arr( _temp22, sizeof( void*), 2u));}}}); return;} struct _tuple9{
struct _tuple7* f1; struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structfield*
Cyc_Parse_make_struct_field( struct Cyc_Position_Segment* loc, struct _tuple9*
field_info){ struct _tuple9 _temp27; struct Cyc_Core_Opt* _temp28; struct
_tuple7* _temp30; struct _tuple7 _temp32; struct Cyc_List_List* _temp33; struct
Cyc_List_List* _temp35; void* _temp37; struct Cyc_Absyn_Tqual _temp39; struct
_tuple1* _temp41; struct _tuple9* _temp25= field_info; _temp27=* _temp25; _LL31:
_temp30= _temp27.f1; _temp32=* _temp30; _LL42: _temp41= _temp32.f1; goto _LL40;
_LL40: _temp39= _temp32.f2; goto _LL38; _LL38: _temp37= _temp32.f3; goto _LL36;
_LL36: _temp35= _temp32.f4; goto _LL34; _LL34: _temp33= _temp32.f5; goto _LL29;
_LL29: _temp28= _temp27.f2; goto _LL26; _LL26: if( _temp35 != 0){ Cyc_Parse_err(
_tag_arr("bad type params in struct field", sizeof( unsigned char), 32u), loc);}{
void* _temp43=(* _temp41).f1; struct Cyc_List_List* _temp53; struct Cyc_List_List*
_temp55; _LL45: if(( unsigned int) _temp43 > 1u?*(( int*) _temp43) == Cyc_Absyn_Rel_n:
0){ _LL54: _temp53=(( struct Cyc_Absyn_Rel_n_struct*) _temp43)->f1; if( _temp53
== 0){ goto _LL46;} else{ goto _LL47;}} else{ goto _LL47;} _LL47: if((
unsigned int) _temp43 > 1u?*(( int*) _temp43) == Cyc_Absyn_Abs_n: 0){ _LL56:
_temp55=(( struct Cyc_Absyn_Abs_n_struct*) _temp43)->f1; if( _temp55 == 0){ goto
_LL48;} else{ goto _LL49;}} else{ goto _LL49;} _LL49: if( _temp43 ==( void*) Cyc_Absyn_Loc_n){
goto _LL50;} else{ goto _LL51;} _LL51: goto _LL52; _LL46: goto _LL44; _LL48:
goto _LL44; _LL50: goto _LL44; _LL52: Cyc_Parse_err( _tag_arr("struct field cannot be qualified with a namespace",
sizeof( unsigned char), 50u), loc); goto _LL44; _LL44:;} return({ struct Cyc_Absyn_Structfield*
_temp57=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp57->name=(* _temp41).f2; _temp57->tq= _temp39; _temp57->type=( void*)
_temp37; _temp57->width= _temp28; _temp57->attributes= _temp33; _temp57;});}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp60; struct Cyc_List_List* _temp61; void* _temp63; struct Cyc_Absyn_Tqual
_temp65; struct Cyc_Core_Opt* _temp67; struct _tuple10* _temp58= field; _temp60=*
_temp58; _LL68: _temp67= _temp60.f1; goto _LL66; _LL66: _temp65= _temp60.f2;
goto _LL64; _LL64: _temp63= _temp60.f3; goto _LL62; _LL62: _temp61= _temp60.f4;
goto _LL59; _LL59: { struct Cyc_Core_Opt* _temp69= 0; if( _temp67 != 0){ _temp69=({
struct Cyc_Core_Opt* _temp70=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp70->v=( void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp67))->v)).f2; _temp70;});{ void* _temp71=(*(( struct _tuple1*)((
struct Cyc_Core_Opt*) _check_null( _temp67))->v)).f1; struct Cyc_List_List*
_temp81; struct Cyc_List_List* _temp83; _LL73: if(( unsigned int) _temp71 > 1u?*((
int*) _temp71) == Cyc_Absyn_Rel_n: 0){ _LL82: _temp81=(( struct Cyc_Absyn_Rel_n_struct*)
_temp71)->f1; if( _temp81 == 0){ goto _LL74;} else{ goto _LL75;}} else{ goto
_LL75;} _LL75: if(( unsigned int) _temp71 > 1u?*(( int*) _temp71) == Cyc_Absyn_Abs_n:
0){ _LL84: _temp83=(( struct Cyc_Absyn_Abs_n_struct*) _temp71)->f1; if( _temp83
== 0){ goto _LL76;} else{ goto _LL77;}} else{ goto _LL77;} _LL77: if( _temp71 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL78;} else{ goto _LL79;} _LL79: goto _LL80;
_LL74: goto _LL72; _LL76: goto _LL72; _LL78: goto _LL72; _LL80: Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a module name", sizeof(
unsigned char), 49u), loc); goto _LL72; _LL72:;}} if( _temp61 != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("parameter should have no type parameters", sizeof( unsigned char), 41u),
loc);} return({ struct _tuple2* _temp85=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp85->f1= _temp69; _temp85->f2= _temp65; _temp85->f3=
_temp63; _temp85;});}} static void* Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment*
loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct* _temp86=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp86[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp87; _temp87.tag= Cyc_Parse_Type_spec; _temp87.f1=(
void*) t; _temp87.f2= loc; _temp87;}); _temp86;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp88= t; struct Cyc_Absyn_Exp* _temp94; struct Cyc_Absyn_Tqual
_temp96; void* _temp98; _LL90: if(( unsigned int) _temp88 > 4u?*(( int*) _temp88)
== Cyc_Absyn_ArrayType: 0){ _LL99: _temp98=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp88)->f1; goto _LL97; _LL97: _temp96=(( struct Cyc_Absyn_ArrayType_struct*)
_temp88)->f2; goto _LL95; _LL95: _temp94=(( struct Cyc_Absyn_ArrayType_struct*)
_temp88)->f3; goto _LL91;} else{ goto _LL92;} _LL92: goto _LL93; _LL91: return
Cyc_Absyn_starb_typ( _temp98,( void*) Cyc_Absyn_HeapRgn, _temp96, _temp94 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp100=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp100[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp101; _temp101.tag= Cyc_Absyn_Upper_b;
_temp101.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp94); _temp101;});
_temp100;})); _LL93: return t; _LL89:;} static void Cyc_Parse_arg_array2ptr(
struct _tuple2* x){(* x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple11*
_temp102=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp102->f1=(*
t).f2; _temp102->f2=(* t).f3; _temp102;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_arr
decl_kind;{ void* _temp103=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp129;
_LL105: if(*(( int*) _temp103) == Cyc_Absyn_Var_d){ _LL130: _temp129=(( struct
Cyc_Absyn_Var_d_struct*) _temp103)->f1; goto _LL106;} else{ goto _LL107;} _LL107:
if(*(( int*) _temp103) == Cyc_Absyn_Let_d){ goto _LL108;} else{ goto _LL109;}
_LL109: if(*(( int*) _temp103) == Cyc_Absyn_Letv_d){ goto _LL110;} else{ goto
_LL111;} _LL111: if(*(( int*) _temp103) == Cyc_Absyn_Fn_d){ goto _LL112;} else{
goto _LL113;} _LL113: if(*(( int*) _temp103) == Cyc_Absyn_Struct_d){ goto _LL114;}
else{ goto _LL115;} _LL115: if(*(( int*) _temp103) == Cyc_Absyn_Union_d){ goto
_LL116;} else{ goto _LL117;} _LL117: if(*(( int*) _temp103) == Cyc_Absyn_Tunion_d){
goto _LL118;} else{ goto _LL119;} _LL119: if(*(( int*) _temp103) == Cyc_Absyn_Typedef_d){
goto _LL120;} else{ goto _LL121;} _LL121: if(*(( int*) _temp103) == Cyc_Absyn_Enum_d){
goto _LL122;} else{ goto _LL123;} _LL123: if(*(( int*) _temp103) == Cyc_Absyn_Namespace_d){
goto _LL124;} else{ goto _LL125;} _LL125: if(*(( int*) _temp103) == Cyc_Absyn_Using_d){
goto _LL126;} else{ goto _LL127;} _LL127: if(*(( int*) _temp103) == Cyc_Absyn_ExternC_d){
goto _LL128;} else{ goto _LL104;} _LL106: if( _temp129->initializer != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);}{ void* _temp131=(* _temp129->name).f1; struct
Cyc_List_List* _temp141; struct Cyc_List_List* _temp143; _LL133: if( _temp131 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL134;} else{ goto _LL135;} _LL135: if((
unsigned int) _temp131 > 1u?*(( int*) _temp131) == Cyc_Absyn_Rel_n: 0){ _LL142:
_temp141=(( struct Cyc_Absyn_Rel_n_struct*) _temp131)->f1; if( _temp141 == 0){
goto _LL136;} else{ goto _LL137;}} else{ goto _LL137;} _LL137: if(( unsigned int)
_temp131 > 1u?*(( int*) _temp131) == Cyc_Absyn_Abs_n: 0){ _LL144: _temp143=((
struct Cyc_Absyn_Abs_n_struct*) _temp131)->f1; if( _temp143 == 0){ goto _LL138;}
else{ goto _LL139;}} else{ goto _LL139;} _LL139: goto _LL140; _LL134: goto
_LL132; _LL136: goto _LL132; _LL138: goto _LL132; _LL140: Cyc_Parse_err(
_tag_arr("module names not allowed on parameter declarations", sizeof(
unsigned char), 51u), x->loc); goto _LL132; _LL132:;}{ int found= 0; for( 0;
params != 0; params=(( struct Cyc_List_List*) _check_null( params))->tl){ if(
Cyc_String_zstrptrcmp((* _temp129->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( params))->hd) == 0){ found= 1; break;}} if( ! found){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp146; _temp146.tag=
Cyc_Stdio_String_pa; _temp146.f1=( struct _tagged_arr)*(* _temp129->name).f2;{
void* _temp145[ 1u]={& _temp146}; Cyc_Stdio_aprintf( _tag_arr("%s is not listed as a parameter",
sizeof( unsigned char), 32u), _tag_arr( _temp145, sizeof( void*), 1u));}}), x->loc);}
return;} _LL108: decl_kind= _tag_arr("let declaration", sizeof( unsigned char),
16u); goto _LL104; _LL110: decl_kind= _tag_arr("let declaration", sizeof(
unsigned char), 16u); goto _LL104; _LL112: decl_kind= _tag_arr("function declaration",
sizeof( unsigned char), 21u); goto _LL104; _LL114: decl_kind= _tag_arr("struct declaration",
sizeof( unsigned char), 19u); goto _LL104; _LL116: decl_kind= _tag_arr("union declaration",
sizeof( unsigned char), 18u); goto _LL104; _LL118: decl_kind= _tag_arr("tunion declaration",
sizeof( unsigned char), 19u); goto _LL104; _LL120: decl_kind= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL104; _LL122: decl_kind= _tag_arr("enum declaration",
sizeof( unsigned char), 17u); goto _LL104; _LL124: decl_kind= _tag_arr("namespace declaration",
sizeof( unsigned char), 22u); goto _LL104; _LL126: decl_kind= _tag_arr("using declaration",
sizeof( unsigned char), 18u); goto _LL104; _LL128: decl_kind= _tag_arr("extern C declaration",
sizeof( unsigned char), 21u); goto _LL104; _LL104:;}(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp148; _temp148.tag= Cyc_Stdio_String_pa;
_temp148.f1=( struct _tagged_arr) decl_kind;{ void* _temp147[ 1u]={& _temp148};
Cyc_Stdio_aprintf( _tag_arr("%s appears in parameter type", sizeof(
unsigned char), 29u), _tag_arr( _temp147, sizeof( void*), 1u));}}), x->loc);
return;} struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment*
f2; } ; static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env,
struct _tagged_arr* x){ struct _tuple12 _temp151; struct Cyc_Position_Segment*
_temp152; struct Cyc_List_List* _temp154; struct _tuple12* _temp149= env;
_temp151=* _temp149; _LL155: _temp154= _temp151.f1; goto _LL153; _LL153:
_temp152= _temp151.f2; goto _LL150; _LL150: if( _temp154 == 0){ return(( struct
_tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp157; _temp157.tag=
Cyc_Stdio_String_pa; _temp157.f1=( struct _tagged_arr)* x;{ void* _temp156[ 1u]={&
_temp157}; Cyc_Stdio_aprintf( _tag_arr("missing type for parameter %s", sizeof(
unsigned char), 30u), _tag_arr( _temp156, sizeof( void*), 1u));}}), _temp152);}{
void* _temp158=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp154))->hd)->r; struct Cyc_Absyn_Vardecl* _temp164; _LL160: if(*((
int*) _temp158) == Cyc_Absyn_Var_d){ _LL165: _temp164=(( struct Cyc_Absyn_Var_d_struct*)
_temp158)->f1; goto _LL161;} else{ goto _LL162;} _LL162: goto _LL163; _LL161:{
void* _temp166=(* _temp164->name).f1; struct Cyc_List_List* _temp176; struct Cyc_List_List*
_temp178; _LL168: if( _temp166 ==( void*) Cyc_Absyn_Loc_n){ goto _LL169;} else{
goto _LL170;} _LL170: if(( unsigned int) _temp166 > 1u?*(( int*) _temp166) ==
Cyc_Absyn_Rel_n: 0){ _LL177: _temp176=(( struct Cyc_Absyn_Rel_n_struct*)
_temp166)->f1; if( _temp176 == 0){ goto _LL171;} else{ goto _LL172;}} else{ goto
_LL172;} _LL172: if(( unsigned int) _temp166 > 1u?*(( int*) _temp166) == Cyc_Absyn_Abs_n:
0){ _LL179: _temp178=(( struct Cyc_Absyn_Abs_n_struct*) _temp166)->f1; if(
_temp178 == 0){ goto _LL173;} else{ goto _LL174;}} else{ goto _LL174;} _LL174:
goto _LL175; _LL169: goto _LL167; _LL171: goto _LL167; _LL173: goto _LL167;
_LL175: Cyc_Parse_err( _tag_arr("module name not allowed on parameter", sizeof(
unsigned char), 37u), _temp152); goto _LL167; _LL167:;} if( Cyc_String_zstrptrcmp((*
_temp164->name).f2, x) == 0){ return({ struct _tuple2* _temp180=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp180->f1=({ struct Cyc_Core_Opt*
_temp181=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp181[ 0]=({ struct Cyc_Core_Opt _temp182; _temp182.v=( void*)(* _temp164->name).f2;
_temp182;}); _temp181;}); _temp180->f2= _temp164->tq; _temp180->f3=( void*)
_temp164->type; _temp180;});} else{ return Cyc_Parse_get_param_type(({ struct
_tuple12* _temp183=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp183->f1=(( struct Cyc_List_List*) _check_null( _temp154))->tl; _temp183->f2=
_temp152; _temp183;}), x);} _LL163: return(( struct _tuple2*(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("non-variable declaration",
sizeof( unsigned char), 25u),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp154))->hd)->loc); _LL159:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp184= tm; _LL186: if(( unsigned int) _temp184 > 1u?*(( int*)
_temp184) == Cyc_Absyn_TypeParams_mod: 0){ goto _LL187;} else{ goto _LL188;}
_LL188: goto _LL189; _LL187: return 1; _LL189: return 0; _LL185:;} static void*
Cyc_Parse_id2type( struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp(
s, _tag_arr("`H", sizeof( unsigned char), 3u)) == 0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp190=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp190[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp191; _temp191.tag= Cyc_Absyn_VarType; _temp191.f1=({ struct Cyc_Absyn_Tvar*
_temp192=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp192->name=({ struct _tagged_arr* _temp193=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp193[ 0]= s; _temp193;}); _temp192->identity=
0; _temp192->kind= k; _temp192;}); _temp191;}); _temp190;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp194=
t; struct Cyc_Absyn_Tvar* _temp200; _LL196: if(( unsigned int) _temp194 > 4u?*((
int*) _temp194) == Cyc_Absyn_VarType: 0){ _LL201: _temp200=(( struct Cyc_Absyn_VarType_struct*)
_temp194)->f1; goto _LL197;} else{ goto _LL198;} _LL198: goto _LL199; _LL197:
return _temp200; _LL199: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL195:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp202=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp202[ 0]=({ struct Cyc_Absyn_VarType_struct _temp203; _temp203.tag= Cyc_Absyn_VarType;
_temp203.f1= pr; _temp203;}); _temp202;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp204=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp210; _LL206:
if(( unsigned int) _temp204 > 1u?*(( int*) _temp204) == Cyc_Absyn_Function_mod:
0){ _LL211: _temp210=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp204)->f1;
goto _LL207;} else{ goto _LL208;} _LL208: goto _LL209; _LL207: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp212= _temp210; struct Cyc_List_List* _temp218; _LL214: if(*((
int*) _temp212) == Cyc_Absyn_WithTypes){ goto _LL215;} else{ goto _LL216;}
_LL216: if(*(( int*) _temp212) == Cyc_Absyn_NoTypes){ _LL219: _temp218=(( struct
Cyc_Absyn_NoTypes_struct*) _temp212)->f1; goto _LL217;} else{ goto _LL213;}
_LL215: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL217:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp218,
tds);{ struct _tuple12* _temp220=({ struct _tuple12* _temp226=( struct _tuple12*)
GC_malloc( sizeof( struct _tuple12)); _temp226->f1= tds; _temp226->f2= loc;
_temp226;}); return({ struct Cyc_List_List* _temp221=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp221->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp222=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp222[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp223; _temp223.tag= Cyc_Absyn_Function_mod;
_temp223.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp224=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp224[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp225; _temp225.tag= Cyc_Absyn_WithTypes;
_temp225.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp220, _temp218); _temp225.f2= 0; _temp225.f3= 0;
_temp225.f4= 0; _temp225.f5= 0; _temp225;}); _temp224;})); _temp223;}); _temp222;}));
_temp221->tl= 0; _temp221;});} _LL213:;} else{ return({ struct Cyc_List_List*
_temp227=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp227->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp227->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp227;});} _LL209: return({ struct Cyc_List_List*
_temp228=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp228->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp228->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp228;}); _LL205:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp229=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp229->id= d->id; _temp229->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp229;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ enum  Cyc_Parse_Storage_class
_temp230=( enum  Cyc_Parse_Storage_class)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v;
_LL232: if( _temp230 == Cyc_Parse_Extern_sc){ goto _LL233;} else{ goto _LL234;}
_LL234: if( _temp230 == Cyc_Parse_ExternC_sc){ goto _LL235;} else{ goto _LL236;}
_LL236: if( _temp230 == Cyc_Parse_Static_sc){ goto _LL237;} else{ goto _LL238;}
_LL238: goto _LL239; _LL233: sc=( void*) Cyc_Absyn_Extern; goto _LL231; _LL235:
sc=( void*) Cyc_Absyn_ExternC; goto _LL231; _LL237: sc=( void*) Cyc_Absyn_Static;
goto _LL231; _LL239: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL231; _LL231:;}}{ struct Cyc_Core_Opt*
_temp242; void* _temp244; struct _tuple5 _temp240= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL245: _temp244= _temp240.f1; goto _LL243; _LL243: _temp242=
_temp240.f2; goto _LL241; _LL241: { struct Cyc_List_List* _temp248; struct Cyc_List_List*
_temp250; void* _temp252; struct Cyc_Absyn_Tqual _temp254; struct _tuple6
_temp246= Cyc_Parse_apply_tms( tq, _temp244, atts, d->tms); _LL255: _temp254=
_temp246.f1; goto _LL253; _LL253: _temp252= _temp246.f2; goto _LL251; _LL251:
_temp250= _temp246.f3; goto _LL249; _LL249: _temp248= _temp246.f4; goto _LL247;
_LL247: if( _temp242 != 0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp250 != 0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp256= _temp252; struct Cyc_Absyn_FnInfo _temp262; struct Cyc_List_List*
_temp264; struct Cyc_List_List* _temp266; struct Cyc_Absyn_VarargInfo* _temp268;
int _temp270; struct Cyc_List_List* _temp272; void* _temp274; struct Cyc_Core_Opt*
_temp276; struct Cyc_List_List* _temp278; _LL258: if(( unsigned int) _temp256 >
4u?*(( int*) _temp256) == Cyc_Absyn_FnType: 0){ _LL263: _temp262=(( struct Cyc_Absyn_FnType_struct*)
_temp256)->f1; _LL279: _temp278= _temp262.tvars; goto _LL277; _LL277: _temp276=
_temp262.effect; goto _LL275; _LL275: _temp274=( void*) _temp262.ret_typ; goto
_LL273; _LL273: _temp272= _temp262.args; goto _LL271; _LL271: _temp270= _temp262.c_varargs;
goto _LL269; _LL269: _temp268= _temp262.cyc_varargs; goto _LL267; _LL267:
_temp266= _temp262.rgn_po; goto _LL265; _LL265: _temp264= _temp262.attributes;
goto _LL259;} else{ goto _LL260;} _LL260: goto _LL261; _LL259: { struct Cyc_List_List*
_temp280=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp272); return({ struct Cyc_Absyn_Fndecl*
_temp281=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp281->sc=( void*) sc; _temp281->name= d->id; _temp281->tvs= _temp278;
_temp281->is_inline= is_inline; _temp281->effect= _temp276; _temp281->ret_type=(
void*) _temp274; _temp281->args= _temp280; _temp281->c_varargs= _temp270;
_temp281->cyc_varargs= _temp268; _temp281->rgn_po= _temp266; _temp281->body=
body; _temp281->cached_typ= 0; _temp281->param_vardecls= 0; _temp281->attributes=
Cyc_List_append( _temp264, _temp248); _temp281;});} _LL261: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL257:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp282=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp282->f1=({ struct _tagged_arr*
_temp283=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp283[ 0]= _tag_arr("?", sizeof( unsigned char), 2u); _temp283;}); _temp282->f2=(*
t).f2; _temp282->f3=(* t).f3; _temp282;});} else{ return({ struct _tuple4*
_temp284=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp284->f1=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v; _temp284->f2=(*
t).f2; _temp284->f3=(* t).f3; _temp284;});}} static unsigned char _temp285[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp285, _temp285, _temp285 + 52u};
static unsigned char _temp286[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp286, _temp286, _temp286 + 63u};
static unsigned char _temp287[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp287, _temp287, _temp287 + 50u};
static unsigned char _temp288[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp288, _temp288, _temp288 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp289=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp303; void* _temp305; struct Cyc_Position_Segment*
_temp307; struct Cyc_Position_Segment* _temp309; struct Cyc_Position_Segment*
_temp311; struct Cyc_Position_Segment* _temp313; struct Cyc_Absyn_Decl* _temp315;
_LL291: if(*(( int*) _temp289) == Cyc_Parse_Type_spec){ _LL306: _temp305=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp289)->f1; goto _LL304; _LL304: _temp303=((
struct Cyc_Parse_Type_spec_struct*) _temp289)->f2; goto _LL292;} else{ goto
_LL293;} _LL293: if(*(( int*) _temp289) == Cyc_Parse_Signed_spec){ _LL308:
_temp307=(( struct Cyc_Parse_Signed_spec_struct*) _temp289)->f1; goto _LL294;}
else{ goto _LL295;} _LL295: if(*(( int*) _temp289) == Cyc_Parse_Unsigned_spec){
_LL310: _temp309=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp289)->f1; goto
_LL296;} else{ goto _LL297;} _LL297: if(*(( int*) _temp289) == Cyc_Parse_Short_spec){
_LL312: _temp311=(( struct Cyc_Parse_Short_spec_struct*) _temp289)->f1; goto
_LL298;} else{ goto _LL299;} _LL299: if(*(( int*) _temp289) == Cyc_Parse_Long_spec){
_LL314: _temp313=(( struct Cyc_Parse_Long_spec_struct*) _temp289)->f1; goto
_LL300;} else{ goto _LL301;} _LL301: if(*(( int*) _temp289) == Cyc_Parse_Decl_spec){
_LL316: _temp315=(( struct Cyc_Parse_Decl_spec_struct*) _temp289)->f1; goto
_LL302;} else{ goto _LL290;} _LL292: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp303);} last_loc= _temp303; seen_type= 1; t= _temp305; goto _LL290; _LL294:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp307);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp307);} last_loc= _temp307; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL290; _LL296: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp309);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp309);} last_loc= _temp309; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL290; _LL298: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp311);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp311);} last_loc= _temp311; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL290; _LL300: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp313);} if( seen_size){ void* _temp317= sz; _LL319: if( _temp317 ==( void*)
Cyc_Absyn_B4){ goto _LL320;} else{ goto _LL321;} _LL321: goto _LL322; _LL320: sz=(
void*) Cyc_Absyn_B8; goto _LL318; _LL322: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp313); goto _LL318; _LL318:;} last_loc=
_temp313; seen_size= 1; goto _LL290; _LL302: last_loc= _temp315->loc; if(
declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp323=( void*) _temp315->r;
struct Cyc_Absyn_Structdecl* _temp335; struct Cyc_Absyn_Tuniondecl* _temp337;
struct Cyc_Absyn_Uniondecl* _temp339; struct Cyc_Absyn_Enumdecl* _temp341;
_LL325: if(*(( int*) _temp323) == Cyc_Absyn_Struct_d){ _LL336: _temp335=((
struct Cyc_Absyn_Struct_d_struct*) _temp323)->f1; goto _LL326;} else{ goto
_LL327;} _LL327: if(*(( int*) _temp323) == Cyc_Absyn_Tunion_d){ _LL338: _temp337=((
struct Cyc_Absyn_Tunion_d_struct*) _temp323)->f1; goto _LL328;} else{ goto
_LL329;} _LL329: if(*(( int*) _temp323) == Cyc_Absyn_Union_d){ _LL340: _temp339=((
struct Cyc_Absyn_Union_d_struct*) _temp323)->f1; goto _LL330;} else{ goto _LL331;}
_LL331: if(*(( int*) _temp323) == Cyc_Absyn_Enum_d){ _LL342: _temp341=(( struct
Cyc_Absyn_Enum_d_struct*) _temp323)->f1; goto _LL332;} else{ goto _LL333;}
_LL333: goto _LL334; _LL326: { struct Cyc_List_List* _temp343=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp335->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp344=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp344[ 0]=({ struct Cyc_Absyn_StructType_struct _temp345; _temp345.tag= Cyc_Absyn_StructType;
_temp345.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp335->name))->v); _temp345.f2= _temp343; _temp345.f3= 0;
_temp345;}); _temp344;}); if( _temp335->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp346=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp346->v=( void*) _temp315; _temp346;});} goto _LL324;} _LL328: { struct Cyc_List_List*
_temp347=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp337->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp348=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp348[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp349; _temp349.tag= Cyc_Absyn_TunionType;
_temp349.f1=({ struct Cyc_Absyn_TunionInfo _temp350; _temp350.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp351=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp351[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp352; _temp352.tag= Cyc_Absyn_KnownTunion;
_temp352.f1= _temp337; _temp352;}); _temp351;})); _temp350.targs= _temp347;
_temp350.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp350;}); _temp349;});
_temp348;}); if( _temp337->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp353=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp353->v=( void*) _temp315; _temp353;});} goto _LL324;} _LL330: { struct Cyc_List_List*
_temp354=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp339->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp355=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp355[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp356; _temp356.tag= Cyc_Absyn_UnionType; _temp356.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp339->name))->v); _temp356.f2= _temp354; _temp356.f3= 0; _temp356;});
_temp355;}); if( _temp339->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp357=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp357->v=( void*) _temp315; _temp357;});} goto _LL324;} _LL332: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp358=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp358[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp359; _temp359.tag= Cyc_Absyn_EnumType; _temp359.f1=
_temp341->name; _temp359.f2= 0; _temp359;}); _temp358;}); declopt=({ struct Cyc_Core_Opt*
_temp360=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp360->v=( void*) _temp315; _temp360;}); goto _LL324; _LL334:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp315->loc); goto _LL324; _LL324:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp315->loc);} goto _LL290; _LL290:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp361=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp361[ 0]=({ struct Cyc_Absyn_IntType_struct _temp362; _temp362.tag= Cyc_Absyn_IntType;
_temp362.f1=( void*) sgn; _temp362.f2=( void*) sz; _temp362;}); _temp361;});}
else{ if( seen_sign){ void* _temp363= t; void* _temp369; _LL365: if((
unsigned int) _temp363 > 4u?*(( int*) _temp363) == Cyc_Absyn_IntType: 0){ _LL370:
_temp369=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp363)->f2; goto _LL366;}
else{ goto _LL367;} _LL367: goto _LL368; _LL366: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp371=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp371[ 0]=({ struct Cyc_Absyn_IntType_struct _temp372; _temp372.tag= Cyc_Absyn_IntType;
_temp372.f1=( void*) sgn; _temp372.f2=( void*) _temp369; _temp372;}); _temp371;});
goto _LL364; _LL368: Cyc_Parse_err( _tag_arr("sign specification on non-integral type",
sizeof( unsigned char), 40u), last_loc); goto _LL364; _LL364:;} if( seen_size){
void* _temp373= t; void* _temp379; _LL375: if(( unsigned int) _temp373 > 4u?*((
int*) _temp373) == Cyc_Absyn_IntType: 0){ _LL380: _temp379=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp373)->f1; goto _LL376;} else{ goto _LL377;} _LL377: goto _LL378; _LL376: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp381=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp381[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp382; _temp382.tag= Cyc_Absyn_IntType; _temp382.f1=( void*) _temp379;
_temp382.f2=( void*) sz; _temp382;}); _temp381;}); goto _LL374; _LL378: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL374; _LL374:;}} return({ struct _tuple5 _temp383; _temp383.f1=
t; _temp383.f2= declopt; _temp383;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp384=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp385= _temp384->id; struct Cyc_List_List* _temp388; struct
Cyc_List_List* _temp390; void* _temp392; struct Cyc_Absyn_Tqual _temp394; struct
_tuple6 _temp386= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp384->tms);
_LL395: _temp394= _temp386.f1; goto _LL393; _LL393: _temp392= _temp386.f2; goto
_LL391; _LL391: _temp390= _temp386.f3; goto _LL389; _LL389: _temp388= _temp386.f4;
goto _LL387; _LL387: return({ struct Cyc_List_List* _temp396=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp396->hd=( void*)({ struct
_tuple7* _temp397=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp397->f1= _temp385; _temp397->f2= _temp394; _temp397->f3= _temp392; _temp397->f4=
_temp390; _temp397->f5= _temp388; _temp397;}); _temp396->tl= Cyc_Parse_apply_tmss(
_temp394, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp396;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp398; _temp398.f1= tq; _temp398.f2=
t; _temp398.f3= 0; _temp398.f4= atts; _temp398;});}{ void* _temp399=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp413;
void* _temp415; struct Cyc_Position_Segment* _temp417; struct Cyc_List_List*
_temp419; struct Cyc_Absyn_Tqual _temp421; void* _temp423; void* _temp425;
struct Cyc_List_List* _temp427; struct Cyc_Position_Segment* _temp429; _LL401:
if( _temp399 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL402;} else{ goto _LL403;}
_LL403: if(( unsigned int) _temp399 > 1u?*(( int*) _temp399) == Cyc_Absyn_ConstArray_mod:
0){ _LL414: _temp413=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp399)->f1;
goto _LL404;} else{ goto _LL405;} _LL405: if(( unsigned int) _temp399 > 1u?*((
int*) _temp399) == Cyc_Absyn_Function_mod: 0){ _LL416: _temp415=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp399)->f1; goto _LL406;} else{ goto
_LL407;} _LL407: if(( unsigned int) _temp399 > 1u?*(( int*) _temp399) == Cyc_Absyn_TypeParams_mod:
0){ _LL420: _temp419=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp399)->f1;
goto _LL418; _LL418: _temp417=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp399)->f2; goto _LL408;} else{ goto _LL409;} _LL409: if(( unsigned int)
_temp399 > 1u?*(( int*) _temp399) == Cyc_Absyn_Pointer_mod: 0){ _LL426: _temp425=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp399)->f1; goto _LL424;
_LL424: _temp423=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp399)->f2;
goto _LL422; _LL422: _temp421=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp399)->f3;
goto _LL410;} else{ goto _LL411;} _LL411: if(( unsigned int) _temp399 > 1u?*((
int*) _temp399) == Cyc_Absyn_Attributes_mod: 0){ _LL430: _temp429=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp399)->f1; goto _LL428; _LL428: _temp427=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp399)->f2; goto _LL412;} else{ goto _LL400;} _LL402: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp431=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp431[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp432; _temp432.tag= Cyc_Absyn_ArrayType;
_temp432.f1=( void*) t; _temp432.f2= tq; _temp432.f3= 0; _temp432;}); _temp431;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL404: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp433=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp433[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp434; _temp434.tag= Cyc_Absyn_ArrayType;
_temp434.f1=( void*) t; _temp434.f2= tq; _temp434.f3=( struct Cyc_Absyn_Exp*)
_temp413; _temp434;}); _temp433;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL406: { void* _temp435= _temp415; struct Cyc_List_List* _temp441;
struct Cyc_Core_Opt* _temp443; struct Cyc_Absyn_VarargInfo* _temp445; int
_temp447; struct Cyc_List_List* _temp449; struct Cyc_Position_Segment* _temp451;
_LL437: if(*(( int*) _temp435) == Cyc_Absyn_WithTypes){ _LL450: _temp449=((
struct Cyc_Absyn_WithTypes_struct*) _temp435)->f1; goto _LL448; _LL448: _temp447=((
struct Cyc_Absyn_WithTypes_struct*) _temp435)->f2; goto _LL446; _LL446: _temp445=((
struct Cyc_Absyn_WithTypes_struct*) _temp435)->f3; goto _LL444; _LL444: _temp443=((
struct Cyc_Absyn_WithTypes_struct*) _temp435)->f4; goto _LL442; _LL442: _temp441=((
struct Cyc_Absyn_WithTypes_struct*) _temp435)->f5; goto _LL438;} else{ goto
_LL439;} _LL439: if(*(( int*) _temp435) == Cyc_Absyn_NoTypes){ _LL452: _temp451=((
struct Cyc_Absyn_NoTypes_struct*) _temp435)->f2; goto _LL440;} else{ goto _LL436;}
_LL438: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp453=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp453->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp453->tl= fn_atts; _temp453;});} else{ new_atts=({ struct Cyc_List_List*
_temp454=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp454->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp454->tl= new_atts; _temp454;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp455=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp461; _LL457: if(( unsigned int) _temp455 > 1u?*(( int*) _temp455) == Cyc_Absyn_TypeParams_mod:
0){ _LL462: _temp461=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp455)->f1;
goto _LL458;} else{ goto _LL459;} _LL459: goto _LL460; _LL458: typvars= _temp461;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL456; _LL460: goto
_LL456; _LL456:;} if((((( ! _temp447? _temp445 == 0: 0)? _temp449 != 0: 0)?((
struct Cyc_List_List*) _check_null( _temp449))->tl == 0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp449))->hd)).f1 == 0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp449))->hd)).f3 ==( void*)
Cyc_Absyn_VoidType: 0){ _temp449= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp449); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp443, t, _temp449, _temp447, _temp445, _temp441, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL440:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp451)); _LL436:;} _LL408: if((( struct Cyc_List_List*) _check_null(
tms))->tl == 0){ return({ struct _tuple6 _temp463; _temp463.f1= tq; _temp463.f2=
t; _temp463.f3= _temp419; _temp463.f4= atts; _temp463;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp417)); _LL410: { void* _temp464= _temp425;
struct Cyc_Absyn_Exp* _temp472; struct Cyc_Absyn_Exp* _temp474; _LL466: if((
unsigned int) _temp464 > 1u?*(( int*) _temp464) == Cyc_Absyn_NonNullable_ps: 0){
_LL473: _temp472=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp464)->f1; goto
_LL467;} else{ goto _LL468;} _LL468: if(( unsigned int) _temp464 > 1u?*(( int*)
_temp464) == Cyc_Absyn_Nullable_ps: 0){ _LL475: _temp474=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp464)->f1; goto _LL469;} else{ goto _LL470;} _LL470: if( _temp464 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL471;} else{ goto _LL465;} _LL467: return Cyc_Parse_apply_tms(
_temp421, Cyc_Absyn_atb_typ( t, _temp423, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp476=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp476[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp477; _temp477.tag= Cyc_Absyn_Upper_b;
_temp477.f1= _temp472; _temp477;}); _temp476;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL469: return Cyc_Parse_apply_tms( _temp421, Cyc_Absyn_starb_typ(
t, _temp423, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp478=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp478[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp479; _temp479.tag= Cyc_Absyn_Upper_b;
_temp479.f1= _temp474; _temp479;}); _temp478;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL471: return Cyc_Parse_apply_tms( _temp421, Cyc_Absyn_tagged_typ(
t, _temp423, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL465:;}
_LL412: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp427),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL400:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp482; void* _temp484; struct _tuple5 _temp480= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL485: _temp484= _temp480.f1; goto _LL483; _LL483: _temp482=
_temp480.f2; goto _LL481; _LL481: if( _temp482 != 0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp484;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp486=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp486[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp487; _temp487.tag= Cyc_Absyn_Decl_s;
_temp487.f1= d; _temp487.f2= s; _temp487;}); _temp486;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc != 0){ enum  Cyc_Parse_Storage_class
_temp488=( enum  Cyc_Parse_Storage_class)(( struct Cyc_Core_Opt*) _check_null(
ds->sc))->v; _LL490: if( _temp488 == Cyc_Parse_Typedef_sc){ goto _LL491;} else{
goto _LL492;} _LL492: if( _temp488 == Cyc_Parse_Extern_sc){ goto _LL493;} else{
goto _LL494;} _LL494: if( _temp488 == Cyc_Parse_ExternC_sc){ goto _LL495;} else{
goto _LL496;} _LL496: if( _temp488 == Cyc_Parse_Static_sc){ goto _LL497;} else{
goto _LL498;} _LL498: if( _temp488 == Cyc_Parse_Auto_sc){ goto _LL499;} else{
goto _LL500;} _LL500: if( _temp488 == Cyc_Parse_Register_sc){ goto _LL501;}
else{ goto _LL502;} _LL502: if( _temp488 == Cyc_Parse_Abstract_sc){ goto _LL503;}
else{ goto _LL489;} _LL491: istypedef= 1; goto _LL489; _LL493: s=( void*) Cyc_Absyn_Extern;
goto _LL489; _LL495: s=( void*) Cyc_Absyn_ExternC; goto _LL489; _LL497: s=( void*)
Cyc_Absyn_Static; goto _LL489; _LL499: s=( void*) Cyc_Absyn_Public; goto _LL489;
_LL501: s=( void*) Cyc_Absyn_Public; goto _LL489; _LL503: s=( void*) Cyc_Absyn_Abstract;
goto _LL489; _LL489:;}{ struct Cyc_List_List* _temp506; struct Cyc_List_List*
_temp508; struct _tuple0 _temp504=(( struct _tuple0(*)( struct Cyc_List_List* x))
Cyc_List_split)( ids); _LL509: _temp508= _temp504.f1; goto _LL507; _LL507:
_temp506= _temp504.f2; goto _LL505; _LL505: { int exps_empty= 1;{ struct Cyc_List_List*
es= _temp506; for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){
if(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd != 0){
exps_empty= 0; break;}}}{ struct _tuple5 _temp510= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp508 == 0){ struct Cyc_Core_Opt* _temp513; void* _temp515;
struct _tuple5 _temp511= _temp510; _LL516: _temp515= _temp511.f1; goto _LL514;
_LL514: _temp513= _temp511.f2; goto _LL512; _LL512: if( _temp513 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null(
_temp513))->v;{ void* _temp517=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp529; struct Cyc_Absyn_Structdecl* _temp531; struct Cyc_Absyn_Uniondecl*
_temp533; struct Cyc_Absyn_Tuniondecl* _temp535; _LL519: if(*(( int*) _temp517)
== Cyc_Absyn_Enum_d){ _LL530: _temp529=(( struct Cyc_Absyn_Enum_d_struct*)
_temp517)->f1; goto _LL520;} else{ goto _LL521;} _LL521: if(*(( int*) _temp517)
== Cyc_Absyn_Struct_d){ _LL532: _temp531=(( struct Cyc_Absyn_Struct_d_struct*)
_temp517)->f1; goto _LL522;} else{ goto _LL523;} _LL523: if(*(( int*) _temp517)
== Cyc_Absyn_Union_d){ _LL534: _temp533=(( struct Cyc_Absyn_Union_d_struct*)
_temp517)->f1; goto _LL524;} else{ goto _LL525;} _LL525: if(*(( int*) _temp517)
== Cyc_Absyn_Tunion_d){ _LL536: _temp535=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp517)->f1; goto _LL526;} else{ goto _LL527;} _LL527: goto _LL528; _LL520:(
void*)( _temp529->sc=( void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on enum",
sizeof( unsigned char), 23u), loc);} goto _LL518; _LL522:( void*)( _temp531->sc=(
void*) s); _temp531->attributes= atts; goto _LL518; _LL524:( void*)( _temp533->sc=(
void*) s); _temp533->attributes= atts; goto _LL518; _LL526:( void*)( _temp535->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL518; _LL528: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL518:;}
return({ struct Cyc_List_List* _temp537=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp537->hd=( void*) d; _temp537->tl= 0;
_temp537;});} else{ void* _temp538= _temp515; struct Cyc_List_List* _temp552;
struct _tuple1* _temp554; struct Cyc_Absyn_TunionInfo _temp556; void* _temp558;
struct Cyc_Absyn_Tuniondecl* _temp560; struct Cyc_Absyn_TunionInfo _temp562;
struct Cyc_List_List* _temp564; void* _temp566; struct Cyc_Absyn_UnknownTunionInfo
_temp568; int _temp570; struct _tuple1* _temp572; struct Cyc_List_List* _temp574;
struct _tuple1* _temp576; struct _tuple1* _temp578; _LL540: if(( unsigned int)
_temp538 > 4u?*(( int*) _temp538) == Cyc_Absyn_StructType: 0){ _LL555: _temp554=((
struct Cyc_Absyn_StructType_struct*) _temp538)->f1; goto _LL553; _LL553:
_temp552=(( struct Cyc_Absyn_StructType_struct*) _temp538)->f2; goto _LL541;}
else{ goto _LL542;} _LL542: if(( unsigned int) _temp538 > 4u?*(( int*) _temp538)
== Cyc_Absyn_TunionType: 0){ _LL557: _temp556=(( struct Cyc_Absyn_TunionType_struct*)
_temp538)->f1; _LL559: _temp558=( void*) _temp556.tunion_info; if(*(( int*)
_temp558) == Cyc_Absyn_KnownTunion){ _LL561: _temp560=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp558)->f1; goto _LL543;} else{ goto _LL544;}} else{ goto _LL544;} _LL544:
if(( unsigned int) _temp538 > 4u?*(( int*) _temp538) == Cyc_Absyn_TunionType: 0){
_LL563: _temp562=(( struct Cyc_Absyn_TunionType_struct*) _temp538)->f1; _LL567:
_temp566=( void*) _temp562.tunion_info; if(*(( int*) _temp566) == Cyc_Absyn_UnknownTunion){
_LL569: _temp568=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp566)->f1;
_LL573: _temp572= _temp568.name; goto _LL571; _LL571: _temp570= _temp568.is_xtunion;
goto _LL565;} else{ goto _LL546;} _LL565: _temp564= _temp562.targs; goto _LL545;}
else{ goto _LL546;} _LL546: if(( unsigned int) _temp538 > 4u?*(( int*) _temp538)
== Cyc_Absyn_UnionType: 0){ _LL577: _temp576=(( struct Cyc_Absyn_UnionType_struct*)
_temp538)->f1; goto _LL575; _LL575: _temp574=(( struct Cyc_Absyn_UnionType_struct*)
_temp538)->f2; goto _LL547;} else{ goto _LL548;} _LL548: if(( unsigned int)
_temp538 > 4u?*(( int*) _temp538) == Cyc_Absyn_EnumType: 0){ _LL579: _temp578=((
struct Cyc_Absyn_EnumType_struct*) _temp538)->f1; goto _LL549;} else{ goto
_LL550;} _LL550: goto _LL551; _LL541: { struct Cyc_List_List* _temp580=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp552); struct Cyc_Absyn_Structdecl* _temp581=({
struct Cyc_Absyn_Structdecl* _temp585=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp585->sc=( void*) s; _temp585->name=({
struct Cyc_Core_Opt* _temp586=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp586->v=( void*)(( struct _tuple1*) _check_null( _temp554));
_temp586;}); _temp585->tvs= _temp580; _temp585->fields= 0; _temp585->attributes=
0; _temp585;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp582=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp582->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp583=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp583[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp584; _temp584.tag= Cyc_Absyn_Struct_d;
_temp584.f1= _temp581; _temp584;}); _temp583;}), loc); _temp582->tl= 0; _temp582;});}
_LL543: if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp587=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp587->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp588=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp588[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp589; _temp589.tag= Cyc_Absyn_Tunion_d;
_temp589.f1= _temp560; _temp589;}); _temp588;}), loc); _temp587->tl= 0; _temp587;});
_LL545: { struct Cyc_List_List* _temp590=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp564);
struct Cyc_Absyn_Decl* _temp591= Cyc_Absyn_tunion_decl( s, _temp572, _temp590, 0,
_temp570, loc); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp592=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp592->hd=(
void*) _temp591; _temp592->tl= 0; _temp592;});} _LL547: { struct Cyc_List_List*
_temp593=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp574); struct Cyc_Absyn_Uniondecl* _temp594=({
struct Cyc_Absyn_Uniondecl* _temp599=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp599->sc=( void*) s; _temp599->name=({
struct Cyc_Core_Opt* _temp600=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp600->v=( void*)(( struct _tuple1*) _check_null( _temp576));
_temp600;}); _temp599->tvs= _temp593; _temp599->fields= 0; _temp599->attributes=
0; _temp599;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp595=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp595->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp596=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp596->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp597=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp597[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp598; _temp598.tag= Cyc_Absyn_Union_d;
_temp598.f1= _temp594; _temp598;}); _temp597;})); _temp596->loc= loc; _temp596;});
_temp595->tl= 0; _temp595;});} _LL549: { struct Cyc_Absyn_Enumdecl* _temp601=({
struct Cyc_Absyn_Enumdecl* _temp606=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp606->sc=( void*) s; _temp606->name=
_temp578; _temp606->fields= 0; _temp606;}); if( atts != 0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp602=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp602->hd=( void*)({ struct Cyc_Absyn_Decl* _temp603=(
struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp603->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp604=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp604[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp605; _temp605.tag= Cyc_Absyn_Enum_d; _temp605.f1= _temp601; _temp605;});
_temp604;})); _temp603->loc= loc; _temp603;}); _temp602->tl= 0; _temp602;});}
_LL551: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL539:;}} else{ void* t= _temp510.f1; struct Cyc_List_List*
_temp607= Cyc_Parse_apply_tmss( tq, t, _temp508, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp607); if( _temp510.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp510.f2))->v;{ void* _temp608=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp620; struct Cyc_Absyn_Tuniondecl* _temp622;
struct Cyc_Absyn_Uniondecl* _temp624; struct Cyc_Absyn_Enumdecl* _temp626;
_LL610: if(*(( int*) _temp608) == Cyc_Absyn_Struct_d){ _LL621: _temp620=((
struct Cyc_Absyn_Struct_d_struct*) _temp608)->f1; goto _LL611;} else{ goto
_LL612;} _LL612: if(*(( int*) _temp608) == Cyc_Absyn_Tunion_d){ _LL623: _temp622=((
struct Cyc_Absyn_Tunion_d_struct*) _temp608)->f1; goto _LL613;} else{ goto
_LL614;} _LL614: if(*(( int*) _temp608) == Cyc_Absyn_Union_d){ _LL625: _temp624=((
struct Cyc_Absyn_Union_d_struct*) _temp608)->f1; goto _LL615;} else{ goto _LL616;}
_LL616: if(*(( int*) _temp608) == Cyc_Absyn_Enum_d){ _LL627: _temp626=(( struct
Cyc_Absyn_Enum_d_struct*) _temp608)->f1; goto _LL617;} else{ goto _LL618;}
_LL618: goto _LL619; _LL611:( void*)( _temp620->sc=( void*) s); _temp620->attributes=
atts; atts= 0; goto _LL609; _LL613:( void*)( _temp622->sc=( void*) s); goto
_LL609; _LL615:( void*)( _temp624->sc=( void*) s); goto _LL609; _LL617:( void*)(
_temp626->sc=( void*) s); goto _LL609; _LL619: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL609; _LL609:;} decls=({ struct Cyc_List_List*
_temp628=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp628->hd=( void*) d; _temp628->tl= decls; _temp628;});} if( atts != 0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp630; _temp630.tag=
Cyc_Stdio_String_pa; _temp630.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp629[ 1u]={&
_temp630}; Cyc_Stdio_aprintf( _tag_arr("bad attribute %s in typedef", sizeof(
unsigned char), 28u), _tag_arr( _temp629, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp510.f2 != 0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp631= _temp607; for( 0; _temp631 != 0;( _temp631=(( struct
Cyc_List_List*) _check_null( _temp631))->tl, _temp506=(( struct Cyc_List_List*)
_check_null( _temp506))->tl)){ struct _tuple7 _temp634; struct Cyc_List_List*
_temp635; struct Cyc_List_List* _temp637; void* _temp639; struct Cyc_Absyn_Tqual
_temp641; struct _tuple1* _temp643; struct _tuple7* _temp632=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp631))->hd; _temp634=* _temp632; _LL644:
_temp643= _temp634.f1; goto _LL642; _LL642: _temp641= _temp634.f2; goto _LL640;
_LL640: _temp639= _temp634.f3; goto _LL638; _LL638: _temp637= _temp634.f4; goto
_LL636; _LL636: _temp635= _temp634.f5; goto _LL633; _LL633: if( _temp637 != 0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp506 == 0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected null in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp645=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp506))->hd; struct Cyc_Absyn_Vardecl*
_temp646= Cyc_Absyn_new_vardecl( _temp643, _temp639, _temp645); _temp646->tq=
_temp641;( void*)( _temp646->sc=( void*) s); _temp646->attributes= _temp635;{
struct Cyc_Absyn_Decl* _temp647=({ struct Cyc_Absyn_Decl* _temp649=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp649->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp650=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp650[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp651; _temp651.tag= Cyc_Absyn_Var_d; _temp651.f1= _temp646; _temp651;});
_temp650;})); _temp649->loc= loc; _temp649;}); decls=({ struct Cyc_List_List*
_temp648=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp648->hd=( void*) _temp647; _temp648->tl= decls; _temp648;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_String_strlen( s) != 1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp653; _temp653.tag= Cyc_Stdio_String_pa;
_temp653.f1=( struct _tagged_arr) s;{ void* _temp652[ 1u]={& _temp653}; Cyc_Stdio_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp652,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL654: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL655: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL656: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL657: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL658: return(
void*) Cyc_Absyn_EffKind; default: _LL659: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp662; _temp662.tag= Cyc_Stdio_String_pa;
_temp662.f1=( struct _tagged_arr) s;{ void* _temp661[ 1u]={& _temp662}; Cyc_Stdio_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp661,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){ return
tms;} else{ return({ struct Cyc_List_List* _temp663=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp663->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp664=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp664[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp665; _temp665.tag= Cyc_Absyn_Attributes_mod;
_temp665.f1= loc; _temp665.f2= atts; _temp665;}); _temp664;})); _temp663->tl=
tms; _temp663;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp668;
struct Cyc_List_List* _temp669; struct Cyc_List_List* _temp671; void* _temp673;
struct Cyc_Absyn_Tqual _temp675; struct _tuple1* _temp677; struct _tuple7*
_temp666= t; _temp668=* _temp666; _LL678: _temp677= _temp668.f1; goto _LL676;
_LL676: _temp675= _temp668.f2; goto _LL674; _LL674: _temp673= _temp668.f3; goto
_LL672; _LL672: _temp671= _temp668.f4; goto _LL670; _LL670: _temp669= _temp668.f5;
goto _LL667; _LL667: Cyc_Lex_register_typedef( _temp677); if( _temp669 != 0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp680;
_temp680.tag= Cyc_Stdio_String_pa; _temp680.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp669))->hd);{ void* _temp679[ 1u]={&
_temp680}; Cyc_Stdio_aprintf( _tag_arr("bad attribute %s within typedef",
sizeof( unsigned char), 32u), _tag_arr( _temp679, sizeof( void*), 1u));}}), loc);}
return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp681=(
struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp681[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp682; _temp682.tag= Cyc_Absyn_Typedef_d;
_temp682.f1=({ struct Cyc_Absyn_Typedefdecl* _temp683=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp683->name= _temp677;
_temp683->tvs= _temp671; _temp683->defn=( void*) _temp673; _temp683;}); _temp682;});
_temp681;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
f1; struct Cyc_Core_Opt* f2; } ; struct Cyc_DeclaratorExpopt_tok_struct{
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
unsigned char _temp685[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp685, _temp685, _temp685 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp686= yy1; struct _tuple16* _temp692; _LL688: if(*(( void**)
_temp686) == Cyc_Int_tok){ _LL693: _temp692=(( struct Cyc_Int_tok_struct*)
_temp686)->f1; goto _LL689;} else{ goto _LL690;} _LL690: goto _LL691; _LL689:
return _temp692; _LL691:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL687:;}
static unsigned char _temp695[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp695, _temp695, _temp695 + 11u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ void* _temp696= yy1; struct
_tagged_arr _temp702; _LL698: if(*(( void**) _temp696) == Cyc_String_tok){
_LL703: _temp702=(( struct Cyc_String_tok_struct*) _temp696)->f1; goto _LL699;}
else{ goto _LL700;} _LL700: goto _LL701; _LL699: return _temp702; _LL701:( int)
_throw(( void*)& Cyc_yyfail_String_tok); _LL697:;} static unsigned char _temp705[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp705, _temp705, _temp705 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp706= yy1; unsigned char _temp712; _LL708: if(*(( void**) _temp706) ==
Cyc_Char_tok){ _LL713: _temp712=(( struct Cyc_Char_tok_struct*) _temp706)->f1;
goto _LL709;} else{ goto _LL710;} _LL710: goto _LL711; _LL709: return _temp712;
_LL711:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL707:;} static
unsigned char _temp715[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp715, _temp715, _temp715 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp716= yy1; void*
_temp722; _LL718: if(*(( void**) _temp716) == Cyc_Pointer_Sort_tok){ _LL723:
_temp722=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp716)->f1; goto
_LL719;} else{ goto _LL720;} _LL720: goto _LL721; _LL719: return _temp722;
_LL721:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL717:;} static
unsigned char _temp725[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp725, _temp725, _temp725 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp726= yy1; struct Cyc_Absyn_Exp*
_temp732; _LL728: if(*(( void**) _temp726) == Cyc_Exp_tok){ _LL733: _temp732=((
struct Cyc_Exp_tok_struct*) _temp726)->f1; goto _LL729;} else{ goto _LL730;}
_LL730: goto _LL731; _LL729: return _temp732; _LL731:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL727:;} static unsigned char _temp735[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{ _temp735, _temp735, _temp735 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( void* yy1){ void* _temp736= yy1;
struct Cyc_List_List* _temp742; _LL738: if(*(( void**) _temp736) == Cyc_ExpList_tok){
_LL743: _temp742=(( struct Cyc_ExpList_tok_struct*) _temp736)->f1; goto _LL739;}
else{ goto _LL740;} _LL740: goto _LL741; _LL739: return _temp742; _LL741:( int)
_throw(( void*)& Cyc_yyfail_ExpList_tok); _LL737:;} static unsigned char
_temp745[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure,{ _temp745, _temp745, _temp745 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp746= yy1; struct Cyc_List_List*
_temp752; _LL748: if(*(( void**) _temp746) == Cyc_InitializerList_tok){ _LL753:
_temp752=(( struct Cyc_InitializerList_tok_struct*) _temp746)->f1; goto _LL749;}
else{ goto _LL750;} _LL750: goto _LL751; _LL749: return _temp752; _LL751:( int)
_throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL747:;} static unsigned char
_temp755[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{ _temp755, _temp755, _temp755 + 11u}}; void* Cyc_yyget_Primop_tok(
void* yy1){ void* _temp756= yy1; void* _temp762; _LL758: if(*(( void**) _temp756)
== Cyc_Primop_tok){ _LL763: _temp762=( void*)(( struct Cyc_Primop_tok_struct*)
_temp756)->f1; goto _LL759;} else{ goto _LL760;} _LL760: goto _LL761; _LL759:
return _temp762; _LL761:( int) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL757:;}
static unsigned char _temp765[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp765, _temp765, _temp765 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp766= yy1;
struct Cyc_Core_Opt* _temp772; _LL768: if(*(( void**) _temp766) == Cyc_Primopopt_tok){
_LL773: _temp772=(( struct Cyc_Primopopt_tok_struct*) _temp766)->f1; goto _LL769;}
else{ goto _LL770;} _LL770: goto _LL771; _LL769: return _temp772; _LL771:( int)
_throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL767:;} static unsigned char
_temp775[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{ _temp775, _temp775, _temp775 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok(
void* yy1){ void* _temp776= yy1; struct _tuple1* _temp782; _LL778: if(*(( void**)
_temp776) == Cyc_QualId_tok){ _LL783: _temp782=(( struct Cyc_QualId_tok_struct*)
_temp776)->f1; goto _LL779;} else{ goto _LL780;} _LL780: goto _LL781; _LL779:
return _temp782; _LL781:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL777:;}
static unsigned char _temp785[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp785, _temp785, _temp785 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp786= yy1;
struct Cyc_Absyn_Stmt* _temp792; _LL788: if(*(( void**) _temp786) == Cyc_Stmt_tok){
_LL793: _temp792=(( struct Cyc_Stmt_tok_struct*) _temp786)->f1; goto _LL789;}
else{ goto _LL790;} _LL790: goto _LL791; _LL789: return _temp792; _LL791:( int)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL787:;} static unsigned char _temp795[
21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={
Cyc_Core_Failure,{ _temp795, _temp795, _temp795 + 21u}}; struct Cyc_List_List*
Cyc_yyget_SwitchClauseList_tok( void* yy1){ void* _temp796= yy1; struct Cyc_List_List*
_temp802; _LL798: if(*(( void**) _temp796) == Cyc_SwitchClauseList_tok){ _LL803:
_temp802=(( struct Cyc_SwitchClauseList_tok_struct*) _temp796)->f1; goto _LL799;}
else{ goto _LL800;} _LL800: goto _LL801; _LL799: return _temp802; _LL801:( int)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL797:;} static
unsigned char _temp805[ 22u]="SwitchCClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{ _temp805, _temp805,
_temp805 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok( void*
yy1){ void* _temp806= yy1; struct Cyc_List_List* _temp812; _LL808: if(*(( void**)
_temp806) == Cyc_SwitchCClauseList_tok){ _LL813: _temp812=(( struct Cyc_SwitchCClauseList_tok_struct*)
_temp806)->f1; goto _LL809;} else{ goto _LL810;} _LL810: goto _LL811; _LL809:
return _temp812; _LL811:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL807:;} static unsigned char _temp815[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp815, _temp815, _temp815 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp816= yy1;
struct Cyc_Absyn_Pat* _temp822; _LL818: if(*(( void**) _temp816) == Cyc_Pattern_tok){
_LL823: _temp822=(( struct Cyc_Pattern_tok_struct*) _temp816)->f1; goto _LL819;}
else{ goto _LL820;} _LL820: goto _LL821; _LL819: return _temp822; _LL821:( int)
_throw(( void*)& Cyc_yyfail_Pattern_tok); _LL817:;} static unsigned char
_temp825[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp825, _temp825, _temp825 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp826= yy1; struct Cyc_List_List*
_temp832; _LL828: if(*(( void**) _temp826) == Cyc_PatternList_tok){ _LL833:
_temp832=(( struct Cyc_PatternList_tok_struct*) _temp826)->f1; goto _LL829;}
else{ goto _LL830;} _LL830: goto _LL831; _LL829: return _temp832; _LL831:( int)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL827:;} static unsigned char
_temp835[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp835, _temp835, _temp835 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp836= yy1; struct _tuple14* _temp842; _LL838: if(*(( void**)
_temp836) == Cyc_FieldPattern_tok){ _LL843: _temp842=(( struct Cyc_FieldPattern_tok_struct*)
_temp836)->f1; goto _LL839;} else{ goto _LL840;} _LL840: goto _LL841; _LL839:
return _temp842; _LL841:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL837:;} static unsigned char _temp845[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp845, _temp845, _temp845 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp846= yy1; struct Cyc_List_List* _temp852; _LL848: if(*((
void**) _temp846) == Cyc_FieldPatternList_tok){ _LL853: _temp852=(( struct Cyc_FieldPatternList_tok_struct*)
_temp846)->f1; goto _LL849;} else{ goto _LL850;} _LL850: goto _LL851; _LL849:
return _temp852; _LL851:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL847:;} static unsigned char _temp855[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp855, _temp855, _temp855 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp856= yy1;
struct Cyc_Absyn_Fndecl* _temp862; _LL858: if(*(( void**) _temp856) == Cyc_FnDecl_tok){
_LL863: _temp862=(( struct Cyc_FnDecl_tok_struct*) _temp856)->f1; goto _LL859;}
else{ goto _LL860;} _LL860: goto _LL861; _LL859: return _temp862; _LL861:( int)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL857:;} static unsigned char _temp865[
13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{ _temp865, _temp865, _temp865 + 13u}}; struct Cyc_List_List*
Cyc_yyget_DeclList_tok( void* yy1){ void* _temp866= yy1; struct Cyc_List_List*
_temp872; _LL868: if(*(( void**) _temp866) == Cyc_DeclList_tok){ _LL873:
_temp872=(( struct Cyc_DeclList_tok_struct*) _temp866)->f1; goto _LL869;} else{
goto _LL870;} _LL870: goto _LL871; _LL869: return _temp872; _LL871:( int) _throw((
void*)& Cyc_yyfail_DeclList_tok); _LL867:;} static unsigned char _temp875[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{
_temp875, _temp875, _temp875 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
void* yy1){ void* _temp876= yy1; struct Cyc_Parse_Declaration_spec* _temp882;
_LL878: if(*(( void**) _temp876) == Cyc_DeclSpec_tok){ _LL883: _temp882=((
struct Cyc_DeclSpec_tok_struct*) _temp876)->f1; goto _LL879;} else{ goto _LL880;}
_LL880: goto _LL881; _LL879: return _temp882; _LL881:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL877:;} static unsigned char _temp885[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp885, _temp885, _temp885 + 13u}};
struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp886= yy1; struct
_tuple15* _temp892; _LL888: if(*(( void**) _temp886) == Cyc_InitDecl_tok){
_LL893: _temp892=(( struct Cyc_InitDecl_tok_struct*) _temp886)->f1; goto _LL889;}
else{ goto _LL890;} _LL890: goto _LL891; _LL889: return _temp892; _LL891:( int)
_throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL887:;} static unsigned char
_temp895[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={
Cyc_Core_Failure,{ _temp895, _temp895, _temp895 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp896= yy1; struct Cyc_List_List*
_temp902; _LL898: if(*(( void**) _temp896) == Cyc_InitDeclList_tok){ _LL903:
_temp902=(( struct Cyc_InitDeclList_tok_struct*) _temp896)->f1; goto _LL899;}
else{ goto _LL900;} _LL900: goto _LL901; _LL899: return _temp902; _LL901:( int)
_throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL897:;} static unsigned char
_temp905[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={
Cyc_Core_Failure,{ _temp905, _temp905, _temp905 + 17u}}; enum  Cyc_Parse_Storage_class
Cyc_yyget_StorageClass_tok( void* yy1){ void* _temp906= yy1; enum  Cyc_Parse_Storage_class
_temp912; _LL908: if(*(( void**) _temp906) == Cyc_StorageClass_tok){ _LL913:
_temp912=(( struct Cyc_StorageClass_tok_struct*) _temp906)->f1; goto _LL909;}
else{ goto _LL910;} _LL910: goto _LL911; _LL909: return _temp912; _LL911:( int)
_throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL907:;} static unsigned char
_temp915[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={
Cyc_Core_Failure,{ _temp915, _temp915, _temp915 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok(
void* yy1){ void* _temp916= yy1; void* _temp922; _LL918: if(*(( void**) _temp916)
== Cyc_TypeSpecifier_tok){ _LL923: _temp922=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp916)->f1; goto _LL919;} else{ goto _LL920;} _LL920: goto _LL921; _LL919:
return _temp922; _LL921:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL917:;} static unsigned char _temp925[ 18u]="StructOrUnion_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp925, _temp925, _temp925 + 18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ void* _temp926= yy1; enum  Cyc_Parse_Struct_or_union _temp932;
_LL928: if(*(( void**) _temp926) == Cyc_StructOrUnion_tok){ _LL933: _temp932=((
struct Cyc_StructOrUnion_tok_struct*) _temp926)->f1; goto _LL929;} else{ goto
_LL930;} _LL930: goto _LL931; _LL929: return _temp932; _LL931:( int) _throw((
void*)& Cyc_yyfail_StructOrUnion_tok); _LL927:;} static unsigned char _temp935[
13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeQual_tok={
Cyc_Core_Failure,{ _temp935, _temp935, _temp935 + 13u}}; struct Cyc_Absyn_Tqual
Cyc_yyget_TypeQual_tok( void* yy1){ void* _temp936= yy1; struct Cyc_Absyn_Tqual
_temp942; _LL938: if(*(( void**) _temp936) == Cyc_TypeQual_tok){ _LL943:
_temp942=(( struct Cyc_TypeQual_tok_struct*) _temp936)->f1; goto _LL939;} else{
goto _LL940;} _LL940: goto _LL941; _LL939: return _temp942; _LL941:( int) _throw((
void*)& Cyc_yyfail_TypeQual_tok); _LL937:;} static unsigned char _temp945[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp945, _temp945, _temp945 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ void* _temp946= yy1; struct Cyc_List_List* _temp952; _LL948: if(*((
void**) _temp946) == Cyc_StructFieldDeclList_tok){ _LL953: _temp952=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp946)->f1; goto _LL949;} else{ goto
_LL950;} _LL950: goto _LL951; _LL949: return _temp952; _LL951:( int) _throw((
void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL947:;} static unsigned char
_temp955[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp955, _temp955,
_temp955 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp956= yy1; struct Cyc_List_List* _temp962; _LL958: if(*((
void**) _temp956) == Cyc_StructFieldDeclListList_tok){ _LL963: _temp962=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp956)->f1; goto _LL959;}
else{ goto _LL960;} _LL960: goto _LL961; _LL959: return _temp962; _LL961:( int)
_throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL957:;} static
unsigned char _temp965[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp965, _temp965,
_temp965 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp966= yy1; struct Cyc_List_List* _temp972; _LL968: if(*(( void**)
_temp966) == Cyc_TypeModifierList_tok){ _LL973: _temp972=(( struct Cyc_TypeModifierList_tok_struct*)
_temp966)->f1; goto _LL969;} else{ goto _LL970;} _LL970: goto _LL971; _LL969:
return _temp972; _LL971:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL967:;} static unsigned char _temp975[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure,{ _temp975, _temp975, _temp975 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp976= yy1; void* _temp982; _LL978:
if(*(( void**) _temp976) == Cyc_Rgn_tok){ _LL983: _temp982=( void*)(( struct Cyc_Rgn_tok_struct*)
_temp976)->f1; goto _LL979;} else{ goto _LL980;} _LL980: goto _LL981; _LL979:
return _temp982; _LL981:( int) _throw(( void*)& Cyc_yyfail_Rgn_tok); _LL977:;}
static unsigned char _temp985[ 15u]="Declarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp985, _temp985, _temp985 + 15u}};
struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok( void* yy1){ void*
_temp986= yy1; struct Cyc_Parse_Declarator* _temp992; _LL988: if(*(( void**)
_temp986) == Cyc_Declarator_tok){ _LL993: _temp992=(( struct Cyc_Declarator_tok_struct*)
_temp986)->f1; goto _LL989;} else{ goto _LL990;} _LL990: goto _LL991; _LL989:
return _temp992; _LL991:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL987:;} static unsigned char _temp995[ 21u]="DeclaratorExpopt_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{
_temp995, _temp995, _temp995 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok(
void* yy1){ void* _temp996= yy1; struct _tuple13* _temp1002; _LL998: if(*(( void**)
_temp996) == Cyc_DeclaratorExpopt_tok){ _LL1003: _temp1002=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp996)->f1; goto _LL999;} else{ goto _LL1000;} _LL1000: goto _LL1001; _LL999:
return _temp1002; _LL1001:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL997:;} static unsigned char _temp1005[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp1005, _temp1005, _temp1005 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1006= yy1; struct Cyc_List_List* _temp1012; _LL1008: if(*((
void**) _temp1006) == Cyc_DeclaratorExpoptList_tok){ _LL1013: _temp1012=((
struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1006)->f1; goto _LL1009;}
else{ goto _LL1010;} _LL1010: goto _LL1011; _LL1009: return _temp1012; _LL1011:(
int) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL1007:;} static
unsigned char _temp1015[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1015, _temp1015,
_temp1015 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1016= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1022;
_LL1018: if(*(( void**) _temp1016) == Cyc_AbstractDeclarator_tok){ _LL1023:
_temp1022=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1016)->f1; goto
_LL1019;} else{ goto _LL1020;} _LL1020: goto _LL1021; _LL1019: return _temp1022;
_LL1021:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1017:;}
static unsigned char _temp1025[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp1025, _temp1025, _temp1025 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1026= yy1; int _temp1032; _LL1028:
if(*(( void**) _temp1026) == Cyc_Bool_tok){ _LL1033: _temp1032=(( struct Cyc_Bool_tok_struct*)
_temp1026)->f1; goto _LL1029;} else{ goto _LL1030;} _LL1030: goto _LL1031;
_LL1029: return _temp1032; _LL1031:( int) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1027:;} static unsigned char _temp1035[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1035, _temp1035, _temp1035 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1036= yy1; void* _temp1042;
_LL1038: if(*(( void**) _temp1036) == Cyc_Scope_tok){ _LL1043: _temp1042=( void*)((
struct Cyc_Scope_tok_struct*) _temp1036)->f1; goto _LL1039;} else{ goto _LL1040;}
_LL1040: goto _LL1041; _LL1039: return _temp1042; _LL1041:( int) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1037:;} static unsigned char _temp1045[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1045, _temp1045, _temp1045 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1046= yy1; struct Cyc_Absyn_Tunionfield* _temp1052;
_LL1048: if(*(( void**) _temp1046) == Cyc_TunionField_tok){ _LL1053: _temp1052=((
struct Cyc_TunionField_tok_struct*) _temp1046)->f1; goto _LL1049;} else{ goto
_LL1050;} _LL1050: goto _LL1051; _LL1049: return _temp1052; _LL1051:( int)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1047:;} static unsigned char
_temp1055[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1055, _temp1055, _temp1055 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1056= yy1; struct Cyc_List_List*
_temp1062; _LL1058: if(*(( void**) _temp1056) == Cyc_TunionFieldList_tok){
_LL1063: _temp1062=(( struct Cyc_TunionFieldList_tok_struct*) _temp1056)->f1;
goto _LL1059;} else{ goto _LL1060;} _LL1060: goto _LL1061; _LL1059: return
_temp1062; _LL1061:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1057:;} static unsigned char _temp1065[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1065, _temp1065, _temp1065 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1066= yy1; struct _tuple18* _temp1072; _LL1068: if(*((
void**) _temp1066) == Cyc_QualSpecList_tok){ _LL1073: _temp1072=(( struct Cyc_QualSpecList_tok_struct*)
_temp1066)->f1; goto _LL1069;} else{ goto _LL1070;} _LL1070: goto _LL1071;
_LL1069: return _temp1072; _LL1071:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1067:;} static unsigned char _temp1075[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1075, _temp1075, _temp1075 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1076= yy1;
struct Cyc_List_List* _temp1082; _LL1078: if(*(( void**) _temp1076) == Cyc_IdList_tok){
_LL1083: _temp1082=(( struct Cyc_IdList_tok_struct*) _temp1076)->f1; goto
_LL1079;} else{ goto _LL1080;} _LL1080: goto _LL1081; _LL1079: return _temp1082;
_LL1081:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1077:;} static
unsigned char _temp1085[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1085, _temp1085, _temp1085 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1086= yy1;
struct _tuple2* _temp1092; _LL1088: if(*(( void**) _temp1086) == Cyc_ParamDecl_tok){
_LL1093: _temp1092=(( struct Cyc_ParamDecl_tok_struct*) _temp1086)->f1; goto
_LL1089;} else{ goto _LL1090;} _LL1090: goto _LL1091; _LL1089: return _temp1092;
_LL1091:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1087:;} static
unsigned char _temp1095[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1095, _temp1095,
_temp1095 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1096= yy1; struct Cyc_List_List* _temp1102; _LL1098: if(*(( void**)
_temp1096) == Cyc_ParamDeclList_tok){ _LL1103: _temp1102=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1096)->f1; goto _LL1099;} else{ goto _LL1100;} _LL1100: goto _LL1101;
_LL1099: return _temp1102; _LL1101:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1097:;} static unsigned char _temp1105[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1105, _temp1105, _temp1105 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1106= yy1; struct _tuple17* _temp1112; _LL1108: if(*((
void**) _temp1106) == Cyc_ParamDeclListBool_tok){ _LL1113: _temp1112=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1106)->f1; goto _LL1109;} else{ goto
_LL1110;} _LL1110: goto _LL1111; _LL1109: return _temp1112; _LL1111:( int)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1107:;} static
unsigned char _temp1115[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1115, _temp1115, _temp1115 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1116=
yy1; struct Cyc_List_List* _temp1122; _LL1118: if(*(( void**) _temp1116) == Cyc_TypeList_tok){
_LL1123: _temp1122=(( struct Cyc_TypeList_tok_struct*) _temp1116)->f1; goto
_LL1119;} else{ goto _LL1120;} _LL1120: goto _LL1121; _LL1119: return _temp1122;
_LL1121:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1117:;} static
unsigned char _temp1125[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1125, _temp1125,
_temp1125 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1126= yy1; struct Cyc_List_List* _temp1132; _LL1128: if(*(( void**)
_temp1126) == Cyc_DesignatorList_tok){ _LL1133: _temp1132=(( struct Cyc_DesignatorList_tok_struct*)
_temp1126)->f1; goto _LL1129;} else{ goto _LL1130;} _LL1130: goto _LL1131;
_LL1129: return _temp1132; _LL1131:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1127:;} static unsigned char _temp1135[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1135,
_temp1135, _temp1135 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1136= yy1; void* _temp1142; _LL1138: if(*(( void**) _temp1136) == Cyc_Designator_tok){
_LL1143: _temp1142=( void*)(( struct Cyc_Designator_tok_struct*) _temp1136)->f1;
goto _LL1139;} else{ goto _LL1140;} _LL1140: goto _LL1141; _LL1139: return
_temp1142; _LL1141:( int) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1137:;}
static unsigned char _temp1145[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1145, _temp1145, _temp1145 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1146= yy1; void* _temp1152;
_LL1148: if(*(( void**) _temp1146) == Cyc_Kind_tok){ _LL1153: _temp1152=( void*)((
struct Cyc_Kind_tok_struct*) _temp1146)->f1; goto _LL1149;} else{ goto _LL1150;}
_LL1150: goto _LL1151; _LL1149: return _temp1152; _LL1151:( int) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1147:;} static unsigned char _temp1155[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1155, _temp1155, _temp1155 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1156= yy1; void* _temp1162; _LL1158: if(*(( void**) _temp1156) == Cyc_Type_tok){
_LL1163: _temp1162=( void*)(( struct Cyc_Type_tok_struct*) _temp1156)->f1; goto
_LL1159;} else{ goto _LL1160;} _LL1160: goto _LL1161; _LL1159: return _temp1162;
_LL1161:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1157:;} static
unsigned char _temp1165[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1165, _temp1165,
_temp1165 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1166= yy1; struct Cyc_List_List* _temp1172; _LL1168: if(*(( void**)
_temp1166) == Cyc_AttributeList_tok){ _LL1173: _temp1172=(( struct Cyc_AttributeList_tok_struct*)
_temp1166)->f1; goto _LL1169;} else{ goto _LL1170;} _LL1170: goto _LL1171;
_LL1169: return _temp1172; _LL1171:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1167:;} static unsigned char _temp1175[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1175,
_temp1175, _temp1175 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1176= yy1; void* _temp1182; _LL1178: if(*(( void**) _temp1176) == Cyc_Attribute_tok){
_LL1183: _temp1182=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1176)->f1;
goto _LL1179;} else{ goto _LL1180;} _LL1180: goto _LL1181; _LL1179: return
_temp1182; _LL1181:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1177:;}
static unsigned char _temp1185[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1185, _temp1185, _temp1185 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1186= yy1; struct Cyc_Absyn_Enumfield* _temp1192; _LL1188: if(*(( void**)
_temp1186) == Cyc_Enumfield_tok){ _LL1193: _temp1192=(( struct Cyc_Enumfield_tok_struct*)
_temp1186)->f1; goto _LL1189;} else{ goto _LL1190;} _LL1190: goto _LL1191;
_LL1189: return _temp1192; _LL1191:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1187:;} static unsigned char _temp1195[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1195, _temp1195, _temp1195 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1196= yy1; struct Cyc_List_List* _temp1202; _LL1198: if(*((
void**) _temp1196) == Cyc_EnumfieldList_tok){ _LL1203: _temp1202=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1196)->f1; goto _LL1199;} else{ goto _LL1200;} _LL1200: goto _LL1201;
_LL1199: return _temp1202; _LL1201:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1197:;} static unsigned char _temp1205[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1205, _temp1205, _temp1205 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1206= yy1;
struct Cyc_Core_Opt* _temp1212; _LL1208: if(*(( void**) _temp1206) == Cyc_TypeOpt_tok){
_LL1213: _temp1212=(( struct Cyc_TypeOpt_tok_struct*) _temp1206)->f1; goto
_LL1209;} else{ goto _LL1210;} _LL1210: goto _LL1211; _LL1209: return _temp1212;
_LL1211:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1207:;} static
unsigned char _temp1215[ 13u]="Rgnorder_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{ _temp1215, _temp1215, _temp1215 +
13u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok( void* yy1){ void* _temp1216=
yy1; struct Cyc_List_List* _temp1222; _LL1218: if(*(( void**) _temp1216) == Cyc_Rgnorder_tok){
_LL1223: _temp1222=(( struct Cyc_Rgnorder_tok_struct*) _temp1216)->f1; goto
_LL1219;} else{ goto _LL1220;} _LL1220: goto _LL1221; _LL1219: return _temp1222;
_LL1221:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1217:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1224;
_temp1224.timestamp= 0; _temp1224.first_line= 0; _temp1224.first_column= 0;
_temp1224.last_line= 0; _temp1224.last_column= 0; _temp1224;});} struct Cyc_Yyltype
Cyc_yylloc={ 0, 0, 0, 0, 0}; static short Cyc_yytranslate[ 347u]={ 0, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 118, 2, 2, 100, 116, 112, 2, 96, 97, 107, 110, 98, 111, 104, 115, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 103, 92, 101, 95, 102, 109, 108, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 105, 2, 106, 114, 99, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 93, 113, 94,
117, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 3, 4,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
86, 87, 88, 89, 90, 91}; static unsigned char _temp1225[ 2u]="$"; static
unsigned char _temp1226[ 6u]="error"; static unsigned char _temp1227[ 12u]="$undefined.";
static unsigned char _temp1228[ 5u]="AUTO"; static unsigned char _temp1229[ 9u]="REGISTER";
static unsigned char _temp1230[ 7u]="STATIC"; static unsigned char _temp1231[ 7u]="EXTERN";
static unsigned char _temp1232[ 8u]="TYPEDEF"; static unsigned char _temp1233[ 5u]="VOID";
static unsigned char _temp1234[ 5u]="CHAR"; static unsigned char _temp1235[ 6u]="SHORT";
static unsigned char _temp1236[ 4u]="INT"; static unsigned char _temp1237[ 5u]="LONG";
static unsigned char _temp1238[ 6u]="FLOAT"; static unsigned char _temp1239[ 7u]="DOUBLE";
static unsigned char _temp1240[ 7u]="SIGNED"; static unsigned char _temp1241[ 9u]="UNSIGNED";
static unsigned char _temp1242[ 6u]="CONST"; static unsigned char _temp1243[ 9u]="VOLATILE";
static unsigned char _temp1244[ 9u]="RESTRICT"; static unsigned char _temp1245[
7u]="STRUCT"; static unsigned char _temp1246[ 6u]="UNION"; static unsigned char
_temp1247[ 5u]="CASE"; static unsigned char _temp1248[ 8u]="DEFAULT"; static
unsigned char _temp1249[ 7u]="INLINE"; static unsigned char _temp1250[ 7u]="SIZEOF";
static unsigned char _temp1251[ 9u]="OFFSETOF"; static unsigned char _temp1252[
3u]="IF"; static unsigned char _temp1253[ 5u]="ELSE"; static unsigned char
_temp1254[ 7u]="SWITCH"; static unsigned char _temp1255[ 6u]="WHILE"; static
unsigned char _temp1256[ 3u]="DO"; static unsigned char _temp1257[ 4u]="FOR";
static unsigned char _temp1258[ 5u]="GOTO"; static unsigned char _temp1259[ 9u]="CONTINUE";
static unsigned char _temp1260[ 6u]="BREAK"; static unsigned char _temp1261[ 7u]="RETURN";
static unsigned char _temp1262[ 5u]="ENUM"; static unsigned char _temp1263[ 8u]="NULL_kw";
static unsigned char _temp1264[ 4u]="LET"; static unsigned char _temp1265[ 6u]="THROW";
static unsigned char _temp1266[ 4u]="TRY"; static unsigned char _temp1267[ 6u]="CATCH";
static unsigned char _temp1268[ 4u]="NEW"; static unsigned char _temp1269[ 9u]="ABSTRACT";
static unsigned char _temp1270[ 9u]="FALLTHRU"; static unsigned char _temp1271[
6u]="USING"; static unsigned char _temp1272[ 10u]="NAMESPACE"; static
unsigned char _temp1273[ 7u]="TUNION"; static unsigned char _temp1274[ 8u]="XTUNION";
static unsigned char _temp1275[ 5u]="FILL"; static unsigned char _temp1276[ 8u]="CODEGEN";
static unsigned char _temp1277[ 4u]="CUT"; static unsigned char _temp1278[ 7u]="SPLICE";
static unsigned char _temp1279[ 7u]="MALLOC"; static unsigned char _temp1280[ 9u]="REGION_T";
static unsigned char _temp1281[ 7u]="REGION"; static unsigned char _temp1282[ 5u]="RNEW";
static unsigned char _temp1283[ 8u]="RMALLOC"; static unsigned char _temp1284[ 7u]="PTR_OP";
static unsigned char _temp1285[ 7u]="INC_OP"; static unsigned char _temp1286[ 7u]="DEC_OP";
static unsigned char _temp1287[ 8u]="LEFT_OP"; static unsigned char _temp1288[ 9u]="RIGHT_OP";
static unsigned char _temp1289[ 6u]="LE_OP"; static unsigned char _temp1290[ 6u]="GE_OP";
static unsigned char _temp1291[ 6u]="EQ_OP"; static unsigned char _temp1292[ 6u]="NE_OP";
static unsigned char _temp1293[ 7u]="AND_OP"; static unsigned char _temp1294[ 6u]="OR_OP";
static unsigned char _temp1295[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1296[ 11u]="DIV_ASSIGN"; static unsigned char _temp1297[ 11u]="MOD_ASSIGN";
static unsigned char _temp1298[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1299[ 11u]="SUB_ASSIGN"; static unsigned char _temp1300[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1301[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1302[ 11u]="AND_ASSIGN"; static unsigned char _temp1303[ 11u]="XOR_ASSIGN";
static unsigned char _temp1304[ 10u]="OR_ASSIGN"; static unsigned char _temp1305[
9u]="ELLIPSIS"; static unsigned char _temp1306[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1307[ 12u]="COLON_COLON"; static unsigned char _temp1308[ 11u]="IDENTIFIER";
static unsigned char _temp1309[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1310[ 7u]="STRING"; static unsigned char _temp1311[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1312[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1313[ 9u]="TYPE_VAR"; static unsigned char _temp1314[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1315[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1316[ 10u]="ATTRIBUTE"; static unsigned char _temp1317[ 4u]="';'"; static
unsigned char _temp1318[ 4u]="'{'"; static unsigned char _temp1319[ 4u]="'}'";
static unsigned char _temp1320[ 4u]="'='"; static unsigned char _temp1321[ 4u]="'('";
static unsigned char _temp1322[ 4u]="')'"; static unsigned char _temp1323[ 4u]="','";
static unsigned char _temp1324[ 4u]="'_'"; static unsigned char _temp1325[ 4u]="'$'";
static unsigned char _temp1326[ 4u]="'<'"; static unsigned char _temp1327[ 4u]="'>'";
static unsigned char _temp1328[ 4u]="':'"; static unsigned char _temp1329[ 4u]="'.'";
static unsigned char _temp1330[ 4u]="'['"; static unsigned char _temp1331[ 4u]="']'";
static unsigned char _temp1332[ 4u]="'*'"; static unsigned char _temp1333[ 4u]="'@'";
static unsigned char _temp1334[ 4u]="'?'"; static unsigned char _temp1335[ 4u]="'+'";
static unsigned char _temp1336[ 4u]="'-'"; static unsigned char _temp1337[ 4u]="'&'";
static unsigned char _temp1338[ 4u]="'|'"; static unsigned char _temp1339[ 4u]="'^'";
static unsigned char _temp1340[ 4u]="'/'"; static unsigned char _temp1341[ 4u]="'%'";
static unsigned char _temp1342[ 4u]="'~'"; static unsigned char _temp1343[ 4u]="'!'";
static unsigned char _temp1344[ 5u]="prog"; static unsigned char _temp1345[ 17u]="translation_unit";
static unsigned char _temp1346[ 21u]="external_declaration"; static
unsigned char _temp1347[ 20u]="function_definition"; static unsigned char
_temp1348[ 21u]="function_definition2"; static unsigned char _temp1349[ 13u]="using_action";
static unsigned char _temp1350[ 15u]="unusing_action"; static unsigned char
_temp1351[ 17u]="namespace_action"; static unsigned char _temp1352[ 19u]="unnamespace_action";
static unsigned char _temp1353[ 12u]="declaration"; static unsigned char
_temp1354[ 17u]="declaration_list"; static unsigned char _temp1355[ 23u]="declaration_specifiers";
static unsigned char _temp1356[ 24u]="storage_class_specifier"; static
unsigned char _temp1357[ 15u]="attributes_opt"; static unsigned char _temp1358[
11u]="attributes"; static unsigned char _temp1359[ 15u]="attribute_list"; static
unsigned char _temp1360[ 10u]="attribute"; static unsigned char _temp1361[ 15u]="type_specifier";
static unsigned char _temp1362[ 5u]="kind"; static unsigned char _temp1363[ 15u]="type_qualifier";
static unsigned char _temp1364[ 15u]="enum_specifier"; static unsigned char
_temp1365[ 11u]="enum_field"; static unsigned char _temp1366[ 22u]="enum_declaration_list";
static unsigned char _temp1367[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1368[ 16u]="type_params_opt"; static unsigned char _temp1369[
16u]="struct_or_union"; static unsigned char _temp1370[ 24u]="struct_declaration_list";
static unsigned char _temp1371[ 25u]="struct_declaration_list0"; static
unsigned char _temp1372[ 21u]="init_declarator_list"; static unsigned char
_temp1373[ 22u]="init_declarator_list0"; static unsigned char _temp1374[ 16u]="init_declarator";
static unsigned char _temp1375[ 19u]="struct_declaration"; static unsigned char
_temp1376[ 25u]="specifier_qualifier_list"; static unsigned char _temp1377[ 23u]="struct_declarator_list";
static unsigned char _temp1378[ 24u]="struct_declarator_list0"; static
unsigned char _temp1379[ 18u]="struct_declarator"; static unsigned char
_temp1380[ 17u]="tunion_specifier"; static unsigned char _temp1381[ 18u]="tunion_or_xtunion";
static unsigned char _temp1382[ 17u]="tunionfield_list"; static unsigned char
_temp1383[ 18u]="tunionfield_scope"; static unsigned char _temp1384[ 12u]="tunionfield";
static unsigned char _temp1385[ 11u]="declarator"; static unsigned char
_temp1386[ 18u]="direct_declarator"; static unsigned char _temp1387[ 8u]="pointer";
static unsigned char _temp1388[ 13u]="pointer_char"; static unsigned char
_temp1389[ 8u]="rgn_opt"; static unsigned char _temp1390[ 4u]="rgn"; static
unsigned char _temp1391[ 20u]="type_qualifier_list"; static unsigned char
_temp1392[ 20u]="parameter_type_list"; static unsigned char _temp1393[ 16u]="optional_effect";
static unsigned char _temp1394[ 19u]="optional_rgn_order"; static unsigned char
_temp1395[ 10u]="rgn_order"; static unsigned char _temp1396[ 16u]="optional_inject";
static unsigned char _temp1397[ 11u]="effect_set"; static unsigned char
_temp1398[ 14u]="atomic_effect"; static unsigned char _temp1399[ 11u]="region_set";
static unsigned char _temp1400[ 15u]="parameter_list"; static unsigned char
_temp1401[ 22u]="parameter_declaration"; static unsigned char _temp1402[ 16u]="identifier_list";
static unsigned char _temp1403[ 17u]="identifier_list0"; static unsigned char
_temp1404[ 12u]="initializer"; static unsigned char _temp1405[ 18u]="array_initializer";
static unsigned char _temp1406[ 17u]="initializer_list"; static unsigned char
_temp1407[ 12u]="designation"; static unsigned char _temp1408[ 16u]="designator_list";
static unsigned char _temp1409[ 11u]="designator"; static unsigned char
_temp1410[ 10u]="type_name"; static unsigned char _temp1411[ 14u]="any_type_name";
static unsigned char _temp1412[ 15u]="type_name_list"; static unsigned char
_temp1413[ 20u]="abstract_declarator"; static unsigned char _temp1414[ 27u]="direct_abstract_declarator";
static unsigned char _temp1415[ 10u]="statement"; static unsigned char _temp1416[
18u]="labeled_statement"; static unsigned char _temp1417[ 21u]="expression_statement";
static unsigned char _temp1418[ 19u]="compound_statement"; static unsigned char
_temp1419[ 16u]="block_item_list"; static unsigned char _temp1420[ 20u]="selection_statement";
static unsigned char _temp1421[ 15u]="switch_clauses"; static unsigned char
_temp1422[ 16u]="switchC_clauses"; static unsigned char _temp1423[ 20u]="iteration_statement";
static unsigned char _temp1424[ 15u]="jump_statement"; static unsigned char
_temp1425[ 8u]="pattern"; static unsigned char _temp1426[ 19u]="tuple_pattern_list";
static unsigned char _temp1427[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1428[ 14u]="field_pattern"; static unsigned char _temp1429[ 19u]="field_pattern_list";
static unsigned char _temp1430[ 20u]="field_pattern_list0"; static unsigned char
_temp1431[ 11u]="expression"; static unsigned char _temp1432[ 22u]="assignment_expression";
static unsigned char _temp1433[ 20u]="assignment_operator"; static unsigned char
_temp1434[ 23u]="conditional_expression"; static unsigned char _temp1435[ 20u]="constant_expression";
static unsigned char _temp1436[ 22u]="logical_or_expression"; static
unsigned char _temp1437[ 23u]="logical_and_expression"; static unsigned char
_temp1438[ 24u]="inclusive_or_expression"; static unsigned char _temp1439[ 24u]="exclusive_or_expression";
static unsigned char _temp1440[ 15u]="and_expression"; static unsigned char
_temp1441[ 20u]="equality_expression"; static unsigned char _temp1442[ 22u]="relational_expression";
static unsigned char _temp1443[ 17u]="shift_expression"; static unsigned char
_temp1444[ 20u]="additive_expression"; static unsigned char _temp1445[ 26u]="multiplicative_expression";
static unsigned char _temp1446[ 16u]="cast_expression"; static unsigned char
_temp1447[ 17u]="unary_expression"; static unsigned char _temp1448[ 15u]="unary_operator";
static unsigned char _temp1449[ 19u]="postfix_expression"; static unsigned char
_temp1450[ 19u]="primary_expression"; static unsigned char _temp1451[ 25u]="argument_expression_list";
static unsigned char _temp1452[ 26u]="argument_expression_list0"; static
unsigned char _temp1453[ 9u]="constant"; static unsigned char _temp1454[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 230u]={{ _temp1225, _temp1225, _temp1225
+ 2u},{ _temp1226, _temp1226, _temp1226 + 6u},{ _temp1227, _temp1227, _temp1227
+ 12u},{ _temp1228, _temp1228, _temp1228 + 5u},{ _temp1229, _temp1229, _temp1229
+ 9u},{ _temp1230, _temp1230, _temp1230 + 7u},{ _temp1231, _temp1231, _temp1231
+ 7u},{ _temp1232, _temp1232, _temp1232 + 8u},{ _temp1233, _temp1233, _temp1233
+ 5u},{ _temp1234, _temp1234, _temp1234 + 5u},{ _temp1235, _temp1235, _temp1235
+ 6u},{ _temp1236, _temp1236, _temp1236 + 4u},{ _temp1237, _temp1237, _temp1237
+ 5u},{ _temp1238, _temp1238, _temp1238 + 6u},{ _temp1239, _temp1239, _temp1239
+ 7u},{ _temp1240, _temp1240, _temp1240 + 7u},{ _temp1241, _temp1241, _temp1241
+ 9u},{ _temp1242, _temp1242, _temp1242 + 6u},{ _temp1243, _temp1243, _temp1243
+ 9u},{ _temp1244, _temp1244, _temp1244 + 9u},{ _temp1245, _temp1245, _temp1245
+ 7u},{ _temp1246, _temp1246, _temp1246 + 6u},{ _temp1247, _temp1247, _temp1247
+ 5u},{ _temp1248, _temp1248, _temp1248 + 8u},{ _temp1249, _temp1249, _temp1249
+ 7u},{ _temp1250, _temp1250, _temp1250 + 7u},{ _temp1251, _temp1251, _temp1251
+ 9u},{ _temp1252, _temp1252, _temp1252 + 3u},{ _temp1253, _temp1253, _temp1253
+ 5u},{ _temp1254, _temp1254, _temp1254 + 7u},{ _temp1255, _temp1255, _temp1255
+ 6u},{ _temp1256, _temp1256, _temp1256 + 3u},{ _temp1257, _temp1257, _temp1257
+ 4u},{ _temp1258, _temp1258, _temp1258 + 5u},{ _temp1259, _temp1259, _temp1259
+ 9u},{ _temp1260, _temp1260, _temp1260 + 6u},{ _temp1261, _temp1261, _temp1261
+ 7u},{ _temp1262, _temp1262, _temp1262 + 5u},{ _temp1263, _temp1263, _temp1263
+ 8u},{ _temp1264, _temp1264, _temp1264 + 4u},{ _temp1265, _temp1265, _temp1265
+ 6u},{ _temp1266, _temp1266, _temp1266 + 4u},{ _temp1267, _temp1267, _temp1267
+ 6u},{ _temp1268, _temp1268, _temp1268 + 4u},{ _temp1269, _temp1269, _temp1269
+ 9u},{ _temp1270, _temp1270, _temp1270 + 9u},{ _temp1271, _temp1271, _temp1271
+ 6u},{ _temp1272, _temp1272, _temp1272 + 10u},{ _temp1273, _temp1273, _temp1273
+ 7u},{ _temp1274, _temp1274, _temp1274 + 8u},{ _temp1275, _temp1275, _temp1275
+ 5u},{ _temp1276, _temp1276, _temp1276 + 8u},{ _temp1277, _temp1277, _temp1277
+ 4u},{ _temp1278, _temp1278, _temp1278 + 7u},{ _temp1279, _temp1279, _temp1279
+ 7u},{ _temp1280, _temp1280, _temp1280 + 9u},{ _temp1281, _temp1281, _temp1281
+ 7u},{ _temp1282, _temp1282, _temp1282 + 5u},{ _temp1283, _temp1283, _temp1283
+ 8u},{ _temp1284, _temp1284, _temp1284 + 7u},{ _temp1285, _temp1285, _temp1285
+ 7u},{ _temp1286, _temp1286, _temp1286 + 7u},{ _temp1287, _temp1287, _temp1287
+ 8u},{ _temp1288, _temp1288, _temp1288 + 9u},{ _temp1289, _temp1289, _temp1289
+ 6u},{ _temp1290, _temp1290, _temp1290 + 6u},{ _temp1291, _temp1291, _temp1291
+ 6u},{ _temp1292, _temp1292, _temp1292 + 6u},{ _temp1293, _temp1293, _temp1293
+ 7u},{ _temp1294, _temp1294, _temp1294 + 6u},{ _temp1295, _temp1295, _temp1295
+ 11u},{ _temp1296, _temp1296, _temp1296 + 11u},{ _temp1297, _temp1297,
_temp1297 + 11u},{ _temp1298, _temp1298, _temp1298 + 11u},{ _temp1299, _temp1299,
_temp1299 + 11u},{ _temp1300, _temp1300, _temp1300 + 12u},{ _temp1301, _temp1301,
_temp1301 + 13u},{ _temp1302, _temp1302, _temp1302 + 11u},{ _temp1303, _temp1303,
_temp1303 + 11u},{ _temp1304, _temp1304, _temp1304 + 10u},{ _temp1305, _temp1305,
_temp1305 + 9u},{ _temp1306, _temp1306, _temp1306 + 11u},{ _temp1307, _temp1307,
_temp1307 + 12u},{ _temp1308, _temp1308, _temp1308 + 11u},{ _temp1309, _temp1309,
_temp1309 + 17u},{ _temp1310, _temp1310, _temp1310 + 7u},{ _temp1311, _temp1311,
_temp1311 + 19u},{ _temp1312, _temp1312, _temp1312 + 18u},{ _temp1313, _temp1313,
_temp1313 + 9u},{ _temp1314, _temp1314, _temp1314 + 16u},{ _temp1315, _temp1315,
_temp1315 + 18u},{ _temp1316, _temp1316, _temp1316 + 10u},{ _temp1317, _temp1317,
_temp1317 + 4u},{ _temp1318, _temp1318, _temp1318 + 4u},{ _temp1319, _temp1319,
_temp1319 + 4u},{ _temp1320, _temp1320, _temp1320 + 4u},{ _temp1321, _temp1321,
_temp1321 + 4u},{ _temp1322, _temp1322, _temp1322 + 4u},{ _temp1323, _temp1323,
_temp1323 + 4u},{ _temp1324, _temp1324, _temp1324 + 4u},{ _temp1325, _temp1325,
_temp1325 + 4u},{ _temp1326, _temp1326, _temp1326 + 4u},{ _temp1327, _temp1327,
_temp1327 + 4u},{ _temp1328, _temp1328, _temp1328 + 4u},{ _temp1329, _temp1329,
_temp1329 + 4u},{ _temp1330, _temp1330, _temp1330 + 4u},{ _temp1331, _temp1331,
_temp1331 + 4u},{ _temp1332, _temp1332, _temp1332 + 4u},{ _temp1333, _temp1333,
_temp1333 + 4u},{ _temp1334, _temp1334, _temp1334 + 4u},{ _temp1335, _temp1335,
_temp1335 + 4u},{ _temp1336, _temp1336, _temp1336 + 4u},{ _temp1337, _temp1337,
_temp1337 + 4u},{ _temp1338, _temp1338, _temp1338 + 4u},{ _temp1339, _temp1339,
_temp1339 + 4u},{ _temp1340, _temp1340, _temp1340 + 4u},{ _temp1341, _temp1341,
_temp1341 + 4u},{ _temp1342, _temp1342, _temp1342 + 4u},{ _temp1343, _temp1343,
_temp1343 + 4u},{ _temp1344, _temp1344, _temp1344 + 5u},{ _temp1345, _temp1345,
_temp1345 + 17u},{ _temp1346, _temp1346, _temp1346 + 21u},{ _temp1347, _temp1347,
_temp1347 + 20u},{ _temp1348, _temp1348, _temp1348 + 21u},{ _temp1349, _temp1349,
_temp1349 + 13u},{ _temp1350, _temp1350, _temp1350 + 15u},{ _temp1351, _temp1351,
_temp1351 + 17u},{ _temp1352, _temp1352, _temp1352 + 19u},{ _temp1353, _temp1353,
_temp1353 + 12u},{ _temp1354, _temp1354, _temp1354 + 17u},{ _temp1355, _temp1355,
_temp1355 + 23u},{ _temp1356, _temp1356, _temp1356 + 24u},{ _temp1357, _temp1357,
_temp1357 + 15u},{ _temp1358, _temp1358, _temp1358 + 11u},{ _temp1359, _temp1359,
_temp1359 + 15u},{ _temp1360, _temp1360, _temp1360 + 10u},{ _temp1361, _temp1361,
_temp1361 + 15u},{ _temp1362, _temp1362, _temp1362 + 5u},{ _temp1363, _temp1363,
_temp1363 + 15u},{ _temp1364, _temp1364, _temp1364 + 15u},{ _temp1365, _temp1365,
_temp1365 + 11u},{ _temp1366, _temp1366, _temp1366 + 22u},{ _temp1367, _temp1367,
_temp1367 + 26u},{ _temp1368, _temp1368, _temp1368 + 16u},{ _temp1369, _temp1369,
_temp1369 + 16u},{ _temp1370, _temp1370, _temp1370 + 24u},{ _temp1371, _temp1371,
_temp1371 + 25u},{ _temp1372, _temp1372, _temp1372 + 21u},{ _temp1373, _temp1373,
_temp1373 + 22u},{ _temp1374, _temp1374, _temp1374 + 16u},{ _temp1375, _temp1375,
_temp1375 + 19u},{ _temp1376, _temp1376, _temp1376 + 25u},{ _temp1377, _temp1377,
_temp1377 + 23u},{ _temp1378, _temp1378, _temp1378 + 24u},{ _temp1379, _temp1379,
_temp1379 + 18u},{ _temp1380, _temp1380, _temp1380 + 17u},{ _temp1381, _temp1381,
_temp1381 + 18u},{ _temp1382, _temp1382, _temp1382 + 17u},{ _temp1383, _temp1383,
_temp1383 + 18u},{ _temp1384, _temp1384, _temp1384 + 12u},{ _temp1385, _temp1385,
_temp1385 + 11u},{ _temp1386, _temp1386, _temp1386 + 18u},{ _temp1387, _temp1387,
_temp1387 + 8u},{ _temp1388, _temp1388, _temp1388 + 13u},{ _temp1389, _temp1389,
_temp1389 + 8u},{ _temp1390, _temp1390, _temp1390 + 4u},{ _temp1391, _temp1391,
_temp1391 + 20u},{ _temp1392, _temp1392, _temp1392 + 20u},{ _temp1393, _temp1393,
_temp1393 + 16u},{ _temp1394, _temp1394, _temp1394 + 19u},{ _temp1395, _temp1395,
_temp1395 + 10u},{ _temp1396, _temp1396, _temp1396 + 16u},{ _temp1397, _temp1397,
_temp1397 + 11u},{ _temp1398, _temp1398, _temp1398 + 14u},{ _temp1399, _temp1399,
_temp1399 + 11u},{ _temp1400, _temp1400, _temp1400 + 15u},{ _temp1401, _temp1401,
_temp1401 + 22u},{ _temp1402, _temp1402, _temp1402 + 16u},{ _temp1403, _temp1403,
_temp1403 + 17u},{ _temp1404, _temp1404, _temp1404 + 12u},{ _temp1405, _temp1405,
_temp1405 + 18u},{ _temp1406, _temp1406, _temp1406 + 17u},{ _temp1407, _temp1407,
_temp1407 + 12u},{ _temp1408, _temp1408, _temp1408 + 16u},{ _temp1409, _temp1409,
_temp1409 + 11u},{ _temp1410, _temp1410, _temp1410 + 10u},{ _temp1411, _temp1411,
_temp1411 + 14u},{ _temp1412, _temp1412, _temp1412 + 15u},{ _temp1413, _temp1413,
_temp1413 + 20u},{ _temp1414, _temp1414, _temp1414 + 27u},{ _temp1415, _temp1415,
_temp1415 + 10u},{ _temp1416, _temp1416, _temp1416 + 18u},{ _temp1417, _temp1417,
_temp1417 + 21u},{ _temp1418, _temp1418, _temp1418 + 19u},{ _temp1419, _temp1419,
_temp1419 + 16u},{ _temp1420, _temp1420, _temp1420 + 20u},{ _temp1421, _temp1421,
_temp1421 + 15u},{ _temp1422, _temp1422, _temp1422 + 16u},{ _temp1423, _temp1423,
_temp1423 + 20u},{ _temp1424, _temp1424, _temp1424 + 15u},{ _temp1425, _temp1425,
_temp1425 + 8u},{ _temp1426, _temp1426, _temp1426 + 19u},{ _temp1427, _temp1427,
_temp1427 + 20u},{ _temp1428, _temp1428, _temp1428 + 14u},{ _temp1429, _temp1429,
_temp1429 + 19u},{ _temp1430, _temp1430, _temp1430 + 20u},{ _temp1431, _temp1431,
_temp1431 + 11u},{ _temp1432, _temp1432, _temp1432 + 22u},{ _temp1433, _temp1433,
_temp1433 + 20u},{ _temp1434, _temp1434, _temp1434 + 23u},{ _temp1435, _temp1435,
_temp1435 + 20u},{ _temp1436, _temp1436, _temp1436 + 22u},{ _temp1437, _temp1437,
_temp1437 + 23u},{ _temp1438, _temp1438, _temp1438 + 24u},{ _temp1439, _temp1439,
_temp1439 + 24u},{ _temp1440, _temp1440, _temp1440 + 15u},{ _temp1441, _temp1441,
_temp1441 + 20u},{ _temp1442, _temp1442, _temp1442 + 22u},{ _temp1443, _temp1443,
_temp1443 + 17u},{ _temp1444, _temp1444, _temp1444 + 20u},{ _temp1445, _temp1445,
_temp1445 + 26u},{ _temp1446, _temp1446, _temp1446 + 16u},{ _temp1447, _temp1447,
_temp1447 + 17u},{ _temp1448, _temp1448, _temp1448 + 15u},{ _temp1449, _temp1449,
_temp1449 + 19u},{ _temp1450, _temp1450, _temp1450 + 19u},{ _temp1451, _temp1451,
_temp1451 + 25u},{ _temp1452, _temp1452, _temp1452 + 26u},{ _temp1453, _temp1453,
_temp1453 + 9u},{ _temp1454, _temp1454, _temp1454 + 20u}}; static short Cyc_yyr1[
404u]={ 0, 119, 120, 120, 120, 120, 120, 120, 120, 121, 121, 122, 122, 122, 122,
123, 123, 124, 125, 126, 127, 128, 128, 128, 128, 129, 129, 130, 130, 130, 130,
130, 130, 130, 130, 131, 131, 131, 131, 131, 131, 131, 132, 132, 133, 134, 134,
135, 135, 135, 135, 135, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136,
136, 136, 136, 136, 136, 136, 136, 137, 137, 138, 138, 138, 139, 139, 140, 140,
141, 141, 142, 142, 142, 142, 142, 143, 143, 143, 144, 144, 145, 146, 146, 147,
148, 148, 149, 149, 150, 151, 151, 151, 151, 152, 153, 153, 154, 154, 154, 155,
155, 155, 155, 156, 156, 157, 157, 157, 157, 158, 158, 158, 159, 159, 160, 160,
161, 161, 161, 161, 161, 161, 161, 161, 161, 161, 161, 161, 162, 162, 162, 162,
163, 163, 163, 163, 163, 164, 164, 165, 165, 165, 166, 166, 167, 167, 167, 167,
168, 168, 169, 169, 170, 170, 171, 171, 172, 172, 173, 173, 173, 173, 174, 174,
174, 174, 175, 175, 176, 176, 176, 177, 178, 178, 179, 179, 180, 180, 180, 180,
181, 181, 181, 181, 182, 183, 183, 184, 184, 185, 185, 186, 186, 186, 186, 187,
187, 188, 188, 188, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189,
190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 191, 192, 192, 193, 193, 194,
194, 194, 194, 194, 194, 195, 195, 195, 195, 195, 196, 196, 196, 196, 196, 196,
197, 197, 197, 197, 198, 198, 198, 198, 198, 198, 198, 198, 198, 198, 198, 198,
198, 198, 199, 199, 199, 199, 199, 199, 199, 199, 200, 200, 200, 200, 200, 200,
200, 200, 200, 200, 200, 200, 200, 200, 201, 201, 202, 202, 203, 203, 204, 205,
205, 206, 206, 207, 207, 208, 208, 208, 208, 208, 208, 208, 208, 208, 208, 208,
209, 209, 209, 209, 209, 209, 209, 210, 211, 211, 212, 212, 213, 213, 214, 214,
215, 215, 216, 216, 216, 217, 217, 217, 217, 217, 218, 218, 218, 219, 219, 219,
220, 220, 220, 220, 221, 221, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222,
222, 222, 223, 223, 223, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224,
224, 224, 224, 225, 225, 225, 225, 225, 225, 225, 225, 225, 226, 227, 227, 228,
228, 228, 228, 229, 229}; static short Cyc_yyr2[ 404u]={ 0, 1, 2, 3, 5, 3, 5, 6,
0, 1, 1, 2, 3, 3, 4, 3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3,
1, 1, 1, 1, 2, 1, 1, 0, 1, 6, 1, 3, 1, 1, 4, 4, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 3, 2, 4, 4, 1, 1, 1, 1, 1, 5, 2, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3,
1, 1, 1, 1, 2, 1, 1, 3, 1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 4, 3, 5, 1, 1,
1, 2, 3, 3, 0, 1, 1, 2, 6, 1, 2, 1, 3, 3, 4, 4, 5, 4, 4, 4, 2, 2, 1, 3, 4, 4, 5,
1, 1, 4, 4, 1, 0, 1, 1, 3, 1, 1, 2, 3, 5, 6, 8, 0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2,
3, 1, 3, 1, 3, 3, 5, 1, 3, 2, 1, 2, 1, 1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1,
2, 3, 2, 1, 2, 1, 2, 3, 3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 1,
1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1, 2, 5, 7, 7, 8, 6, 0, 3,
4, 5, 6, 7, 0, 3, 4, 5, 5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 3, 2, 2, 2, 3,
2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1, 1, 3, 1, 2, 1, 1, 3, 1,
3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2, 5, 5, 1, 1, 3, 1, 3, 1,
3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3, 1, 3, 3, 3, 1, 4, 1, 2,
2, 2, 2, 2, 2, 4, 2, 6, 7, 9, 1, 1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4,
1, 1, 1, 3, 2, 5, 4, 4, 5, 1, 1, 3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[
795u]={ 0, 138, 35, 36, 37, 38, 40, 52, 54, 55, 56, 57, 58, 59, 60, 61, 72, 73,
74, 89, 90, 42, 0, 0, 41, 0, 0, 114, 115, 0, 402, 65, 403, 86, 0, 53, 0, 143,
144, 147, 1, 0, 9, 0, 0, 10, 0, 42, 42, 42, 62, 63, 0, 64, 0, 0, 125, 0, 148,
127, 39, 0, 33, 43, 76, 287, 402, 283, 286, 285, 0, 281, 0, 0, 0, 0, 0, 182, 0,
288, 17, 19, 0, 0, 0, 67, 0, 0, 0, 0, 2, 0, 0, 0, 0, 21, 0, 94, 95, 97, 27, 29,
31, 86, 0, 86, 150, 152, 0, 86, 38, 0, 25, 0, 0, 11, 159, 0, 0, 136, 126, 42,
149, 137, 0, 0, 34, 0, 0, 295, 294, 284, 293, 24, 0, 0, 0, 0, 42, 42, 200, 202,
0, 70, 71, 66, 206, 0, 128, 0, 0, 177, 0, 0, 401, 0, 0, 0, 0, 0, 0, 0, 0, 0, 398,
388, 399, 400, 0, 0, 0, 0, 371, 0, 369, 370, 0, 306, 319, 327, 329, 331, 333,
335, 337, 340, 345, 348, 351, 355, 0, 357, 372, 387, 386, 0, 3, 0, 5, 0, 22, 0,
0, 0, 12, 28, 30, 32, 85, 0, 91, 92, 0, 84, 0, 86, 0, 112, 39, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 402, 234, 236, 0, 242, 238, 0, 240, 223, 224, 225, 0,
226, 227, 228, 0, 304, 26, 13, 97, 0, 183, 0, 0, 161, 159, 0, 0, 129, 0, 139, 0,
0, 79, 0, 77, 282, 297, 0, 296, 184, 0, 0, 295, 173, 203, 0, 100, 102, 159, 0,
208, 201, 209, 69, 0, 88, 0, 87, 0, 179, 0, 181, 68, 0, 0, 365, 0, 321, 355, 0,
322, 323, 0, 0, 0, 0, 0, 0, 358, 359, 0, 0, 0, 0, 361, 362, 360, 145, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 309, 310, 311, 312, 313, 314, 315,
316, 317, 318, 308, 0, 363, 0, 380, 381, 0, 0, 0, 390, 0, 0, 146, 18, 0, 20, 0,
96, 98, 186, 185, 14, 0, 81, 93, 0, 0, 104, 105, 107, 0, 151, 111, 86, 120, 0, 0,
0, 0, 0, 0, 0, 274, 275, 276, 0, 0, 278, 0, 231, 232, 0, 0, 0, 0, 243, 239, 97,
241, 237, 235, 0, 165, 171, 0, 160, 167, 131, 0, 0, 0, 161, 133, 135, 134, 130,
153, 141, 140, 0, 48, 47, 0, 45, 0, 75, 0, 290, 0, 23, 291, 0, 0, 0, 0, 196, 299,
302, 0, 301, 0, 0, 0, 204, 101, 103, 0, 161, 0, 212, 0, 210, 159, 0, 0, 222, 205,
207, 178, 0, 0, 0, 187, 191, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 389, 396, 0, 395,
328, 0, 330, 332, 334, 336, 338, 339, 343, 344, 341, 342, 346, 347, 349, 350,
352, 353, 354, 307, 378, 379, 374, 0, 376, 377, 0, 0, 0, 4, 6, 0, 326, 108, 99,
0, 0, 0, 113, 122, 121, 0, 0, 116, 0, 0, 0, 0, 0, 0, 0, 0, 273, 277, 0, 0, 0,
230, 0, 233, 0, 15, 305, 166, 0, 0, 169, 0, 0, 0, 162, 132, 159, 155, 154, 142,
7, 0, 0, 0, 80, 78, 298, 199, 0, 300, 195, 197, 292, 0, 289, 175, 174, 217, 0,
211, 214, 0, 161, 0, 213, 0, 364, 0, 0, 188, 0, 192, 384, 385, 0, 0, 0, 0, 0, 0,
0, 356, 392, 0, 0, 375, 373, 393, 0, 0, 83, 106, 109, 82, 110, 123, 120, 120, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 249, 279, 0, 0, 16, 159, 172, 170, 168, 0, 165, 161,
0, 0, 0, 44, 46, 198, 303, 0, 216, 219, 0, 221, 220, 215, 0, 0, 189, 193, 0, 0,
324, 325, 0, 394, 0, 397, 320, 391, 0, 119, 118, 244, 0, 249, 259, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 280, 0, 161, 163, 0, 156, 0, 49, 50, 176, 218, 366, 0, 194,
0, 0, 382, 0, 0, 0, 255, 0, 0, 261, 0, 0, 0, 269, 0, 0, 0, 0, 0, 0, 0, 0, 248,
229, 157, 0, 159, 0, 0, 367, 0, 383, 0, 245, 0, 0, 0, 246, 260, 262, 263, 0, 271,
270, 0, 265, 0, 0, 0, 0, 249, 250, 164, 161, 0, 0, 0, 124, 0, 0, 247, 264, 272,
266, 267, 0, 0, 249, 251, 158, 0, 190, 368, 255, 256, 268, 249, 252, 51, 255,
257, 249, 253, 258, 254, 0, 0, 0}; static short Cyc_yydefgoto[ 111u]={ 792, 40,
41, 42, 237, 43, 365, 44, 367, 45, 203, 46, 47, 62, 63, 433, 434, 48, 145, 49,
50, 266, 267, 51, 85, 52, 209, 210, 96, 97, 98, 211, 149, 377, 378, 379, 53, 54,
531, 532, 533, 55, 56, 57, 58, 121, 108, 429, 457, 458, 420, 560, 554, 416, 417,
279, 258, 151, 76, 77, 474, 370, 475, 476, 445, 446, 141, 146, 147, 459, 286,
240, 241, 242, 243, 244, 245, 692, 743, 246, 247, 270, 271, 272, 448, 449, 450,
248, 249, 352, 177, 523, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
189, 190, 191, 192, 488, 489, 193, 194}; static short Cyc_yypact[ 795u]={ 1995,
- -32768, - -32768, - -32768, - -32768, - 3, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, 68, 61, 1081, - -32768, 61, - 26, - -32768,
- -32768, 13, - -32768, 93, - -32768, 89, 173, - -32768, 131, 122, 165, - -32768,
- -32768, 1885, - -32768, 152, 194, - -32768, 398, 68, 68, 68, - -32768, -
-32768, 290, - -32768, 124, 2426, 46, 24, - 18, 61, 225, 182, 2480, - -32768,
232, - -32768, 132, - -32768, - -32768, - -32768, 2248, - -32768, 200, 256, 263,
2248, 283, 265, 317, 174, - -32768, - -32768, 1820, - 14, 1820, - -32768, 330,
2153, 3864, 3864, - -32768, 1885, 2104, 1885, 2104, - -32768, 343, 332, - -32768,
2369, 2480, 2480, 2480, 89, 2153, 89, 360, - -32768, 61, 125, 372, 1312, -
-32768, 2426, 398, - -32768, 4218, 1820, 2824, - -32768, 46, 68, - -32768, -
-32768, 2104, 367, - -32768, 61, 385, 2248, - -32768, - -32768, - -32768, -
-32768, 403, 3864, 95, 159, 68, 68, 247, - -32768, 62, - -32768, - -32768, -
-32768, 386, - 11, - -32768, 482, 305, - -32768, 4011, 402, - -32768, 3864, 3905,
412, 415, 423, 432, 454, 4066, 4066, - -32768, - -32768, - -32768, - -32768,
1655, 461, 4107, 4107, - -32768, 4107, - -32768, - -32768, 465, - -32768, - 45,
504, 450, 463, 474, 354, 91, 387, 358, 19, - -32768, 608, 4107, 138, - -32768, -
-32768, 127, 490, - -32768, 498, - -32768, 502, - -32768, 173, 2889, 2426, -
-32768, - -32768, - -32768, - -32768, 508, 516, 2153, - -32768, 528, 519, - 14,
89, 61, 521, - -32768, 520, 23, 523, 2531, 525, 543, 536, 540, 2954, 2531, 203,
2531, 2531, - 50, 535, - -32768, - -32768, 547, 1428, 1428, 398, 1428, - -32768,
- -32768, - -32768, 551, - -32768, - -32768, - -32768, 185, - -32768, - -32768,
- -32768, 549, - 18, - -32768, 271, 552, 548, 199, 553, 47, - -32768, 554, 87,
560, 10, 558, 565, 568, - -32768, - -32768, 569, 567, - -32768, 235, 683, 2248,
- 56, - -32768, 573, 2153, 2153, 2262, 3019, 34, - -32768, 233, - -32768, 271, -
-32768, 1820, - -32768, 800, - -32768, 550, - -32768, - -32768, 2153, 1655, -
-32768, 2153, - -32768, - -32768, 922, - -32768, 599, 3864, 2213, 645, 3864,
3864, 1655, - -32768, - -32768, 1428, 574, 376, 3864, - -32768, - -32768, -
-32768, - -32768, 4107, 3864, 4107, 4107, 4107, 4107, 4107, 4107, 4107, 4107,
4107, 4107, 4107, 4107, 4107, 4107, 4107, 4107, 4107, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, 3864, - -32768, 119, - -32768, - -32768, 3084, 230, 3864, - -32768, 2748,
571, - -32768, - -32768, 1885, - -32768, 1885, - -32768, - -32768, - -32768, -
-32768, - -32768, 2153, - -32768, - -32768, 3864, 582, 577, - -32768, 586, 2153,
- -32768, - -32768, 89, 495, 3864, 580, 3864, 3864, 660, 1544, 600, - -32768, -
-32768, - -32768, 239, 653, - -32768, 3149, - -32768, - -32768, 2531, 609, 2531,
1766, - -32768, - -32768, 2369, - -32768, - -32768, - -32768, 3864, 613, 618,
234, - -32768, 591, - -32768, 616, 610, 4325, 548, - -32768, - -32768, - -32768,
- -32768, - -32768, - -32768, 87, 1885, - -32768, 612, 617, 615, 61, - -32768,
3864, - -32768, 2248, - -32768, - -32768, 627, 3864, 2248, - 7, - -32768, -
-32768, - -32768, 623, 620, 625, - 14, 638, - -32768, - -32768, - -32768, 630,
548, 633, - -32768, 629, 233, 4232, 1820, 3214, - -32768, - -32768, 386, -
-32768, 634, 635, 655, - -32768, - -32768, 328, 2889, 406, 639, 173, 643, 416,
647, 644, 652, 3970, - -32768, - -32768, 650, 654, 504, 264, 450, 463, 474, 354,
91, 91, 387, 387, 387, 387, 358, 358, 19, 19, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, 651, - -32768, - -32768, 133, 334, 1820, -
-32768, - -32768, 661, - -32768, - -32768, - -32768, 528, 3864, 662, - -32768, -
-32768, - -32768, 663, 61, 243, 418, 3864, 420, 424, 664, 3279, 3344, 253, -
-32768, - -32768, 669, 671, 667, - -32768, 666, - -32768, 2426, - -32768, -
-32768, - -32768, 2153, - 14, - -32768, 677, 271, 672, - -32768, - -32768, 180,
- -32768, - -32768, - -32768, - -32768, 408, 668, 10, - -32768, - -32768, -
-32768, - -32768, 670, - -32768, - -32768, - -32768, - -32768, 846, - -32768,
676, - -32768, - -32768, 678, - -32768, - -32768, 684, 548, 59, - -32768, 674,
691, 702, 685, - -32768, 2596, - -32768, - -32768, - -32768, 2426, 2153, 3905,
764, 691, 694, 2748, - -32768, - -32768, 3864, 3864, - -32768, - -32768, -
-32768, 2748, 338, - -32768, - -32768, - -32768, - -32768, - -32768, 300, 33,
495, 2531, 434, 700, 2531, 3864, 3409, 274, 3474, 292, 3539, 513, - -32768, 704,
714, - -32768, 706, - -32768, - -32768, - -32768, 711, 613, 548, 705, 703, 707,
- -32768, - -32768, - -32768, - -32768, 638, - -32768, - -32768, 709, - -32768,
- -32768, - -32768, 710, 3864, - -32768, - -32768, 2889, 725, - -32768, 599, 727,
- -32768, 361, - -32768, - -32768, - -32768, 728, - -32768, - -32768, 774, 732,
513, - -32768, 441, 2531, 446, 3604, 2531, 449, 3669, 3734, 294, 2248, 723, 733,
- -32768, 2531, 548, 730, 2153, - -32768, 745, - -32768, - -32768, - -32768, -
-32768, - -32768, 306, - -32768, 734, 2153, - -32768, 2672, 2153, 2531, 531, 736,
740, - -32768, 2531, 2531, 458, - -32768, 2531, 2531, 472, 2531, 478, 3799, 63,
1428, - -32768, - -32768, - -32768, 616, 706, 735, 3864, - -32768, 737, - -32768,
484, - -32768, 3864, 738, 741, - -32768, - -32768, - -32768, - -32768, 2531, -
-32768, - -32768, 2531, - -32768, 2531, 2531, 497, 3864, 1045, - -32768, -
-32768, 548, 752, 364, 742, - -32768, 312, 1428, - -32768, - -32768, - -32768, -
-32768, - -32768, 2531, 321, 513, - -32768, - -32768, 743, - -32768, - -32768,
1196, - -32768, - -32768, 1045, - -32768, - -32768, 531, - -32768, 513, - -32768,
- -32768, - -32768, 844, 850, - -32768}; static short Cyc_yypgoto[ 111u]={ -
-32768, 112, - -32768, 539, - -32768, - -32768, - -32768, - -32768, - -32768, -
51, - 43, - 55, - -32768, 322, - 37, 282, - -32768, 12, - 197, 8, - -32768, -
-32768, 417, - -32768, - 38, - -32768, - 140, - -32768, - -32768, - -32768, 656,
646, 97, - -32768, - -32768, 329, - -32768, - -32768, - 24, - -32768, - -32768,
- 28, - 34, - 31, - -32768, - -32768, - 53, - -32768, - 102, - 115, - 369, 121,
215, 302, 570, - 379, - 84, - 281, 746, - -32768, - 162, - 148, - 340, - 264, -
-32768, 419, - 133, - 62, - 104, 20, - 209, - 60, - -32768, - -32768, - 40, -
160, - -32768, - 642, - 392, - -32768, - -32768, - 21, 585, - -32768, 284, -
-32768, - -32768, 214, 128, - -32768, - 126, - 415, - 146, 544, 541, 542, 545,
546, 279, 134, 295, 304, 67, 588, - -32768, - -32768, - -32768, - 327, - -32768,
- -32768, 9}; static short Cyc_yytable[ 4426u]={ 114, 257, 78, 150, 112, 122, 86,
126, 304, 59, 305, 444, 113, 260, 256, 115, 469, 382, 99, 119, 142, 517, 571,
120, 322, 1, 452, 431, 574, 301, 513, 64, 79, 402, 80, 315, 557, 714, 529, 530,
369, 136, 453, 59, 114, 205, 206, 207, 112, 128, 59, 403, 289, 563, 132, 59, 239,
81, 114, 204, 238, 105, 250, 109, 323, 208, 59, 213, 123, 143, 106, 217, 546,
251, 582, 462, 144, 406, 407, 79, 409, 107, 60, 119, 79, 462, 252, 290, 576, 584,
139, 291, 139, 432, 138, 139, 138, 442, 443, 138, 59, 59, 59, 59, 16, 17, 18, 30,
387, 284, 424, 618, 139, 32, 82, 775, 138, 215, 294, 388, 34, 293, 657, 59, 139,
139, 338, - 117, 138, 138, 282, 756, 784, 59, 339, 340, 268, 61, 79, 283, 469,
789, 116, 422, 30, 290, 791, 117, 114, 425, 32, 118, 250, 90, 484, 330, 331, 290,
59, 61, 285, 658, 390, 372, 287, 470, 757, 471, 397, 295, 400, 401, 288, 252, 1,
83, 139, 383, 483, 140, 138, 140, 239, 239, 380, 239, 238, 238, 275, 238, 84,
276, 332, 333, 37, 38, 39, 354, 355, 356, 413, 212, 510, 196, 197, 198, 199, 30,
360, 511, 59, 408, 106, 32, 140, 88, 176, 195, 139, 656, 361, 59, 138, 107, -
183, 384, 84, 87, 468, 216, - 183, 412, 428, 521, 357, 362, 264, 318, 319, 612,
320, 527, 358, 359, 91, 92, 262, 277, 59, 466, 522, 284, 479, 278, 447, 581, 30,
353, 89, 239, 120, 294, 32, 238, 86, 140, 670, - 86, 106, 34, - 86, 427, 255,
639, 702, 84, 698, 411, 125, 107, 37, 38, 39, 412, 79, 79, 93, 94, 139, 139, 139,
255, 138, 138, 138, 398, 129, 421, 139, 399, 139, 59, 138, 59, 138, 139, 139,
212, 139, 138, 138, 522, 138, 514, 597, 444, 59, 522, 124, 139, 515, 150, 277,
138, 61, 127, 731, 440, 556, 463, 371, 543, 664, 412, 464, 622, 114, 412, 465,
130, 540, 623, 547, 282, 549, 633, 528, 131, 588, 274, 664, 412, 283, 114, 37,
38, 39, 112, 640, 414, 589, 587, 412, 134, 415, 550, 684, 610, 551, 100, 101,
102, 412, 30, 59, 133, 59, 455, 456, 32, 103, 139, 316, 104, 687, 138, 726, 140,
787, 139, 412, 776, 412, 138, 790, 140, - 86, 140, 565, 1, 522, 84, 296, 297,
412, 506, 507, 508, 140, 735, 412, 461, 135, 139, 615, 780, 733, 138, 572, 412,
328, 329, 595, 575, 783, 466, 596, 148, 613, 139, 201, 371, 614, 138, 663, 200,
290, 564, 482, 59, 673, 396, 214, 263, 268, 487, 664, 645, 79, 334, 335, 600,
663, 79, 666, 709, 667, 218, 778, 710, 280, 281, 412, 265, 498, 499, 500, 501,
336, 337, 212, 139, 139, 486, 412, 138, 138, 519, 212, 520, 509, 30, 269, 1, 672,
487, 273, 32, 59, 371, 95, 646, 647, 648, 34, 114, 288, 380, 300, 250, 529, 530,
706, 598, 412, 37, 38, 39, 306, 644, 638, 307, 316, 602, 412, 624, 412, 626, 412,
308, 477, 627, 412, 481, 695, 316, 139, 487, 309, 1, 138, 678, 412, 487, 59, 690,
691, 491, 715, 412, 552, 621, 566, 717, 412, 114, 721, 412, 663, 112, 310, 1,
607, 741, 742, 748, 412, 317, 447, 321, 204, 140, 139, 325, 677, 30, 138, 680,
758, 751, 412, 32, 324, 516, - 180, 753, 412, 326, 292, - 180, - 180, 764, 297,
674, 363, - 180, 327, 283, 79, 37, 38, 39, 364, 591, 772, 412, 366, 774, 675,
676, 534, 373, 536, 537, 371, 541, 781, 496, 497, 139, 374, 30, 381, 138, 385,
140, 386, 32, 760, 389, 786, 391, 716, 788, 34, 720, 392, 739, 393, 502, 503,
376, 394, 30, 730, 37, 38, 39, 404, 32, 504, 505, - 208, 405, 202, 410, 292, -
208, - 208, 418, 423, 419, 740, - 208, 430, 283, 435, 746, 747, 436, 426, 749,
750, 437, 752, 439, 438, 454, 322, 727, 480, 485, 518, 239, 524, 525, 535, 238,
341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 768, 526, 538, 769, 542, 770,
771, 544, 553, 548, 665, 79, 555, 558, 239, 351, 559, 139, 238, 561, 567, 138,
573, 239, 782, 569, 568, 238, 139, 578, 579, 139, 138, 65, 580, 138, 371, 239,
277, 583, 239, 238, 585, 592, 238, 593, 371, 586, 599, 671, 594, 601, 299, 604,
371, 302, 302, 603, 605, 608, 611, 625, 312, 313, 609, 630, 632, 616, 619, 620,
302, 302, 628, 302, 634, 635, 636, 649, 30, 67, 637, 68, 69, 641, 32, 643, 653,
654, 651, 441, 302, 70, 659, 655, 71, 72, 606, 660, 661, 442, 443, 668, 73, 669,
371, 679, 74, 75, 693, 694, 255, 696, 700, 1, 712, 699, 701, 737, 703, 704, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 707, 708, 711, 713, 728, 729, 732,
734, 744, 736, 745, 761, 763, 767, 777, 22, 371, 779, 785, 766, 681, 683, 793,
686, 478, 689, 27, 28, 794, 650, 570, 759, 617, 29, 375, 368, 467, 697, 642, 451,
259, 652, 577, 492, 490, 493, 0, 0, 0, 494, 0, 495, 0, 705, 0, 0, 0, 0, 253, 0,
0, 30, 65, 0, 0, 0, 31, 32, 33, 0, 255, 0, 0, 0, 292, - 159, 719, 35, 36, 723,
725, - 159, 0, 283, 0, 37, 38, 39, 302, 0, 302, 302, 302, 302, 302, 302, 302,
302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 30, 67, 0, 68, 69, 0, 32, 0, 0,
0, 0, 755, 0, 70, 0, 0, 71, 72, 152, 153, 762, 442, 443, 0, 73, 472, 765, 0, 74,
75, 0, 154, 0, 155, 0, 302, 156, 0, 0, 0, 0, 773, 0, 157, 158, 0, 0, 159, 0, 0,
160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 0, 0, 303, 473, 0, 168, 0, 0, 0, 169, 0,
0, 302, 442, 443, 0, 170, 0, 302, 171, 172, 173, 0, 0, 0, 0, 174, 175, 0, 0, 0,
0, 0, 0, 0, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
690, 691, 21, 152, 153, 219, 302, 220, 221, 222, 223, 224, 225, 226, 227, 22,
154, 23, 155, 228, 0, 156, 24, 229, 0, 0, 27, 28, 157, 158, 230, 231, 159, 29,
232, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 302, 0, 0, 0, 0, 65, 0, 0, 0, 0,
0, 0, 0, 0, 233, 164, 165, 166, 167, 31, 32, 33, 0, 234, 111, 0, 0, 168, 0, 0,
35, 236, 0, 0, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 66,
67, 0, 68, 69, 0, 32, 0, 0, 0, 0, 0, 0, 70, 0, 0, 71, 72, 0, 0, 0, 0, 0, 0, 73,
0, 302, 0, 74, 75, 0, 0, 0, 0, 302, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 741, 742, 21, 152, 153, 219, 0, 220, 221, 222, 223, 224,
225, 226, 227, 22, 154, 23, 155, 228, 0, 156, 24, 229, 0, 0, 27, 28, 157, 158,
230, 231, 159, 29, 232, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 164, 165, 166, 167, 31, 32, 33, 0, 234, 111,
0, 0, 168, 0, 0, 35, 236, 0, 0, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0,
174, 175, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
0, 0, 21, 152, 153, 219, 0, 220, 221, 222, 223, 224, 225, 226, 227, 22, 154, 23,
155, 228, 0, 156, 24, 229, 0, 0, 27, 28, 157, 158, 230, 231, 159, 29, 232, 160,
161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
233, 164, 165, 166, 167, 31, 32, 33, 0, 234, 111, 235, 0, 168, 0, 0, 35, 236, 0,
0, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 2, 3, 4, 110, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 152, 153, 219, 0,
220, 221, 222, 223, 224, 225, 226, 227, 22, 154, 23, 155, 228, 0, 156, 24, 229,
0, 0, 27, 28, 157, 158, 230, 231, 159, 29, 232, 160, 161, 0, 162, 163, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 164, 165, 166, 167,
31, 32, 33, 0, 234, 111, 0, 0, 168, 0, 0, 35, 236, 0, 0, 0, 0, 0, 0, 170, 0, 0,
171, 172, 173, 0, 0, 0, 0, 174, 175, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 152, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
154, 23, 155, 0, 0, 156, 24, 0, 0, 0, 27, 28, 157, 158, 0, 0, 159, 29, 0, 160,
161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 164, 165, 166, 167, 31, 32, 33, 0, 539, 0, 0, 0, 168, 0, 0, 35, 236, 0, 0, 0,
0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 152, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
154, 0, 155, 0, 0, 156, 0, 0, 0, 0, 27, 28, 157, 158, 0, 0, 159, 29, 0, 160, 161,
0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30,
164, 165, 166, 167, 31, 32, 33, 0, 0, 314, 0, 0, 168, 0, 0, 35, 236, 0, 0, 0, 0,
0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 152, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22,
154, 0, 155, 0, 0, 156, 0, 0, 0, 0, 27, 28, 157, 158, 0, 0, 159, 29, 0, 160, 161,
0, 162, 163, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0,
0, 0, 30, 164, 165, 166, 167, 31, 32, 33, 22, 0, 0, 0, 0, 168, 0, 0, 35, 236, 0,
27, 28, 0, 0, 0, 170, 0, 29, 171, 172, 173, 0, 0, 0, 0, 174, 175, - 8, 1, 0, 2,
3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 31, 21, 33,
0, 0, 137, 0, 0, 0, 0, 0, 35, 36, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27,
28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 0, 0, - 8, 0, 34, 0, 0,
35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0,
31, 32, 33, 0, 0, 0, 0, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, 1, 0,
2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0,
0, 0, 0, 0, 29, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, - 8, 0, 34, 27,
28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4, 110, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 31, 0, 33, 0, 0, 0, 0,
0, 0, 22, 0, 35, 36, 0, 0, 0, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28, 35, 36, 0, 0, 0, 29, 0,
0, 37, 38, 39, 0, 0, 0, 0, 0, 0, 0, 0, 30, 67, 0, 68, 69, 0, 32, 0, 0, 0, 0, 253,
0, 70, 0, 0, 71, 72, 0, 31, 0, 33, 0, 255, 73, 0, 0, 282, 74, 75, 35, 36, 0, 0,
0, 0, 283, 0, 37, 38, 39, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0,
0, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 2, 3, 4, 110, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 31, 0,
33, 0, 0, 111, 22, 202, 23, 0, 0, 35, 36, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29,
0, 2, 3, 4, 110, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0,
21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0, 33, 22, 0, 111, 0, 0, 0, 0, 24, 35, 36, 0,
27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 152, 153, 219, 0, 220, 221, 222, 223, 224, 225, 226, 227, 31, 154, 33, 155,
228, 0, 156, 0, 229, 0, 0, 35, 36, 157, 158, 230, 231, 159, 0, 232, 160, 161, 0,
162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233,
164, 165, 166, 167, 0, 32, 152, 153, 234, 111, 0, 0, 168, 0, 0, 0, 169, 0, 0,
154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0,
160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 0, 0, 303, 662, 0, 168, 0, 0, 0, 169,
152, 153, 0, 442, 443, 0, 170, 0, 0, 171, 172, 173, 0, 154, 0, 155, 174, 175,
156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0,
32, 0, 0, 0, 303, 738, 0, 168, 0, 0, 0, 169, 152, 153, 0, 442, 443, 0, 170, 0, 0,
171, 172, 173, 0, 154, 0, 155, 174, 175, 156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0,
159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 0, 0, 303, 0, 0, 168, 0, 0, 0,
169, 152, 153, 0, 442, 443, 0, 170, 0, 0, 171, 172, 173, 0, 154, 0, 155, 174,
175, 156, 0, 0, 0, 0, 0, 0, 157, 158, 0, 0, 159, 0, 0, 160, 161, 0, 162, 163, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166,
167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 261, 170,
156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162,
163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165,
166, 167, 0, 32, 152, 153, 0, 303, 0, 0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 0,
170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0,
162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164,
165, 166, 167, 0, 32, 152, 153, 395, 0, 0, 0, 168, 0, 0, 0, 169, 0, 0, 154, 0,
155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160,
161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 0, 0, 0, 169, 0, 0,
154, 0, 155, 460, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0,
0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 512, 0, 0,
169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175,
159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 545, 0,
0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174,
175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 0,
0, 0, 169, 0, 0, 154, 0, 155, 590, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158,
174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 629, 0, 0, 0,
168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157,
158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 631, 0, 0,
0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173, 0, 0,
157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0,
0, 0, 0, 168, 682, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171, 172, 173,
0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 152,
153, 0, 0, 0, 0, 168, 685, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0, 171,
172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167, 0, 32,
152, 153, 688, 0, 0, 0, 168, 0, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156, 0,
171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166, 167,
0, 32, 152, 153, 0, 0, 0, 0, 168, 718, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170, 156,
0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162, 163, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165, 166,
167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 722, 0, 0, 169, 0, 0, 154, 0, 155, 0, 170,
156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0, 162,
163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164, 165,
166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 724, 0, 0, 169, 0, 0, 154, 0, 155, 0,
170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160, 161, 0,
162, 163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 164,
165, 166, 167, 0, 32, 152, 153, 0, 0, 0, 0, 168, 754, 0, 0, 169, 0, 0, 154, 0,
155, 0, 170, 156, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0, 160,
161, 0, 162, 163, 0, 0, 0, 0, 152, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 0,
0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 157, 158, 0, 0, 159, 168, 0, 0, 161, 169,
162, 163, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 0, 0, 0, 0,
0, 30, 164, 165, 166, 167, 0, 32, 152, 153, 0, 303, 0, 0, 168, 0, 0, 0, 169, 0,
0, 154, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 157, 158, 174, 175, 159, 0, 0,
0, 161, 0, 162, 163, 0, 0, 0, 0, 152, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 154,
0, 0, 0, 30, 164, 165, 166, 167, 0, 32, 0, 157, 158, 606, 0, 159, 168, 0, 0, 161,
169, 162, 163, 0, 0, 0, 0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 0, 0,
152, 153, 0, 30, 164, 165, 166, 167, 0, 32, 0, 0, 0, 154, 0, 0, 298, 0, 0, 0,
169, 0, 0, 0, 0, 157, 158, 170, 0, 159, 171, 172, 173, 161, 0, 162, 163, 174,
175, 0, 0, 152, 153, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 154, 0, 0, 0, 30, 164, 165,
166, 167, 0, 32, 0, 157, 158, 0, 0, 159, 311, 0, 0, 161, 169, 162, 163, 0, 0, 0,
0, 170, 0, 0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 0, 0, 0, 0, 0, 30, 164, 165,
166, 167, 0, 32, 0, 0, 0, 0, 0, 0, 168, 0, 0, 0, 169, 0, 0, 0, 0, 0, 0, 170, 0,
0, 171, 172, 173, 0, 0, 0, 0, 174, 175, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 22, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 27, 28, 0, 22, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 27, 28, 0, 0,
0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 253, 0, 0, 254, 0, 0, 0, 0, 31, 0, 33,
0, 255, 0, 253, 0, 0, 0, 0, 35, 36, 0, 31, 0, 33, 0, 255, 0, 0, 0, 0, 0, 0, 35,
36, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 562, 0,
0, 0, 0, 0, 0, 0, 31, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 35, 36}; static short Cyc_yycheck[
4426u]={ 55, 116, 23, 87, 55, 58, 34, 62, 156, 0, 156, 275, 55, 117, 116, 55,
297, 214, 46, 56, 82, 361, 437, 57, 69, 1, 82, 17, 443, 155, 357, 22, 23, 83, 25,
168, 415, 679, 5, 6, 202, 79, 98, 34, 99, 100, 101, 102, 99, 70, 41, 101, 63,
422, 75, 46, 111, 83, 113, 99, 111, 52, 113, 54, 109, 103, 57, 105, 59, 83, 88,
109, 399, 113, 453, 284, 90, 237, 238, 70, 240, 99, 85, 120, 75, 294, 114, 98,
95, 458, 82, 102, 84, 83, 82, 87, 84, 104, 105, 87, 91, 92, 93, 94, 17, 18, 19,
83, 85, 140, 63, 526, 104, 89, 101, 757, 104, 108, 149, 96, 96, 149, 63, 114,
116, 117, 107, 94, 116, 117, 96, 68, 774, 124, 115, 116, 127, 91, 129, 105, 421,
783, 96, 258, 83, 98, 788, 101, 203, 102, 89, 105, 203, 41, 314, 64, 65, 98, 149,
91, 140, 102, 222, 203, 102, 298, 103, 300, 228, 149, 230, 231, 110, 201, 1, 82,
168, 215, 311, 82, 168, 84, 237, 238, 212, 240, 237, 238, 93, 240, 101, 96, 101,
102, 107, 108, 109, 59, 60, 61, 253, 104, 83, 91, 92, 93, 94, 83, 81, 90, 201,
239, 88, 89, 117, 93, 88, 89, 210, 588, 93, 212, 210, 99, 92, 216, 101, 96, 290,
104, 98, 98, 263, 373, 96, 108, 124, 170, 171, 106, 173, 381, 104, 105, 92, 93,
118, 88, 239, 286, 376, 282, 307, 94, 275, 452, 83, 190, 93, 314, 294, 292, 89,
314, 292, 168, 606, 93, 88, 96, 96, 263, 92, 554, 653, 101, 645, 92, 96, 99, 107,
108, 109, 98, 275, 276, 92, 93, 280, 281, 282, 92, 280, 281, 282, 92, 96, 98,
290, 96, 292, 292, 290, 294, 292, 297, 298, 210, 300, 297, 298, 437, 300, 83,
476, 579, 307, 443, 93, 311, 90, 405, 88, 311, 91, 93, 695, 92, 94, 96, 202, 92,
596, 98, 101, 92, 391, 98, 105, 83, 391, 98, 402, 96, 404, 92, 384, 84, 463, 135,
614, 98, 105, 408, 107, 108, 109, 408, 555, 88, 464, 463, 98, 98, 93, 408, 92,
103, 408, 47, 48, 49, 98, 83, 365, 92, 367, 280, 281, 89, 90, 373, 168, 93, 92,
373, 92, 290, 780, 381, 98, 760, 98, 381, 786, 298, 96, 300, 429, 1, 526, 101,
97, 98, 98, 338, 339, 340, 311, 103, 98, 283, 95, 405, 518, 103, 697, 405, 439,
98, 66, 67, 94, 444, 103, 462, 98, 97, 94, 421, 98, 303, 98, 421, 596, 92, 98,
429, 310, 430, 102, 227, 82, 121, 435, 317, 710, 562, 439, 62, 63, 479, 614, 444,
602, 94, 602, 85, 94, 98, 138, 139, 98, 96, 330, 331, 332, 333, 110, 111, 373,
463, 464, 97, 98, 463, 464, 365, 381, 367, 352, 83, 97, 1, 610, 357, 83, 89, 479,
361, 92, 83, 84, 85, 96, 550, 110, 525, 96, 550, 5, 6, 664, 97, 98, 107, 108,
109, 96, 562, 550, 96, 298, 97, 98, 97, 98, 97, 98, 96, 306, 97, 98, 309, 639,
311, 518, 399, 96, 1, 518, 97, 98, 405, 525, 22, 23, 323, 97, 98, 412, 532, 430,
97, 98, 600, 97, 98, 710, 600, 96, 1, 485, 22, 23, 97, 98, 96, 579, 94, 600, 464,
554, 113, 624, 83, 554, 627, 728, 97, 98, 89, 68, 359, 92, 97, 98, 114, 96, 97,
98, 97, 98, 621, 94, 103, 112, 105, 579, 107, 108, 109, 94, 465, 97, 98, 94, 757,
622, 623, 386, 93, 388, 389, 476, 391, 766, 328, 329, 601, 94, 83, 93, 601, 93,
518, 96, 89, 733, 96, 780, 96, 682, 783, 96, 685, 83, 711, 92, 334, 335, 103, 92,
83, 694, 107, 108, 109, 103, 89, 336, 337, 92, 96, 95, 94, 96, 97, 98, 97, 97,
103, 712, 103, 94, 105, 98, 717, 718, 94, 106, 721, 722, 95, 724, 98, 97, 94, 69,
690, 25, 97, 101, 728, 92, 98, 96, 728, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
748, 103, 30, 751, 92, 753, 754, 42, 83, 88, 601, 690, 82, 110, 757, 95, 88, 697,
757, 97, 96, 697, 83, 766, 772, 98, 97, 766, 708, 94, 98, 711, 708, 38, 97, 711,
596, 780, 88, 97, 783, 780, 97, 97, 783, 98, 606, 106, 97, 609, 83, 96, 152, 97,
614, 155, 156, 98, 94, 97, 97, 535, 162, 163, 98, 539, 540, 94, 94, 94, 170, 171,
96, 173, 93, 92, 97, 97, 83, 84, 102, 86, 87, 94, 89, 101, 98, 97, 106, 94, 190,
96, 106, 97, 99, 100, 93, 83, 101, 104, 105, 25, 107, 97, 664, 93, 111, 112, 92,
83, 92, 88, 97, 1, 28, 98, 97, 708, 97, 97, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, 97, 96, 96, 93, 103, 94, 98, 84, 94, 97, 92, 98, 97, 94, 84, 37,
710, 97, 97, 103, 628, 629, 0, 631, 307, 633, 48, 49, 0, 569, 435, 732, 525, 55,
210, 201, 288, 644, 558, 276, 116, 579, 445, 324, 322, 325, - 1, - 1, - 1, 326,
- 1, 327, - 1, 661, - 1, - 1, - 1, - 1, 80, - 1, - 1, 83, 38, - 1, - 1, - 1, 88,
89, 90, - 1, 92, - 1, - 1, - 1, 96, 97, 684, 99, 100, 687, 688, 103, - 1, 105, -
1, 107, 108, 109, 322, - 1, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333,
334, 335, 336, 337, 338, 339, 340, 83, 84, - 1, 86, 87, - 1, 89, - 1, - 1, - 1,
- 1, 726, - 1, 96, - 1, - 1, 99, 100, 25, 26, 735, 104, 105, - 1, 107, 32, 741,
- 1, 111, 112, - 1, 38, - 1, 40, - 1, 376, 43, - 1, - 1, - 1, - 1, 756, - 1, 50,
51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84,
85, 86, 87, - 1, 89, - 1, - 1, - 1, 93, 94, - 1, 96, - 1, - 1, - 1, 100, - 1, -
1, 437, 104, 105, - 1, 107, - 1, 443, 110, 111, 112, - 1, - 1, - 1, - 1, 117,
118, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 485, 29, 30, 31, 32, 33, 34,
35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53,
54, 55, 56, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 526, - 1, -
1, - 1, - 1, 38, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88,
89, 90, - 1, 92, 93, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, -
1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 83, 84, - 1, 86,
87, - 1, 89, - 1, - 1, - 1, - 1, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1,
- 1, - 1, - 1, 107, - 1, 602, - 1, 111, 112, - 1, - 1, - 1, - 1, 610, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
- 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, -
1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1, 92, 93, - 1, - 1, 96, - 1, - 1, 99,
100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1,
- 1, 117, 118, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, -
1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1, 92, 93,
94, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110,
111, 112, - 1, - 1, - 1, - 1, 117, 118, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33,
34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52,
53, 54, 55, 56, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87,
88, 89, 90, - 1, 92, 93, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, -
1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, 39, 40, - 1, - 1, 43, 44,
- 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, 61, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1, 92, - 1, - 1, - 1, 96,
- 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, -
1, - 1, - 1, - 1, 117, 118, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
- 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38,
- 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1,
57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1,
- 1, 93, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1,
- 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51,
- 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, 61, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89,
90, 37, - 1, - 1, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, 48, 49, - 1, - 1, - 1,
107, - 1, 55, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 0, 1, - 1, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, 88, 24, 90, - 1, -
1, 93, - 1, - 1, - 1, - 1, - 1, 99, 100, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1,
44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 83, - 1, - 1, - 1, - 1, 88, 89, 90, - 1, - 1, - 1,
94, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, 108, 109, 0,
1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1,
- 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39,
- 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, - 1, - 1, - 1, - 1, 88, 89,
90, - 1, - 1, - 1, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1,
107, 108, 109, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, -
1, - 1, 55, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, - 1, - 1, 37, - 1, 88, 89,
90, - 1, - 1, - 1, 94, - 1, 96, 48, 49, 99, 100, - 1, - 1, - 1, 55, - 1, - 1,
107, 108, 109, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, 88, - 1, 90, - 1, - 1, - 1, - 1, - 1, -
1, 37, - 1, 99, 100, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, -
1, - 1, 55, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1,
38, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, - 1, - 1, 37, - 1, 88, 89,
90, - 1, - 1, - 1, - 1, - 1, 96, 48, 49, 99, 100, - 1, - 1, - 1, 55, - 1, - 1,
107, 108, 109, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, - 1, 86, 87, - 1,
89, - 1, - 1, - 1, - 1, 80, - 1, 96, - 1, - 1, 99, 100, - 1, 88, - 1, 90, - 1,
92, 107, - 1, - 1, 96, 111, 112, 99, 100, - 1, - 1, - 1, - 1, 105, - 1, 107, 108,
109, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, -
1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39,
- 1, - 1, - 1, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1,
- 1, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, 88, - 1, 90, - 1, - 1, 93, 37,
95, 39, - 1, - 1, 99, 100, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1,
55, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, - 1, 90, 37, - 1,
93, - 1, - 1, - 1, - 1, 44, 99, 100, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 88, 38, 90,
40, 41, - 1, 43, - 1, 45, - 1, - 1, 99, 100, 50, 51, 52, 53, 54, - 1, 56, 57, 58,
- 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92,
93, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1,
110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, - 1, - 1, 93, 94, - 1,
96, - 1, - 1, - 1, 100, 25, 26, - 1, 104, 105, - 1, 107, - 1, - 1, 110, 111, 112,
- 1, 38, - 1, 40, 117, 118, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1,
54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87,
- 1, 89, - 1, - 1, - 1, 93, 94, - 1, 96, - 1, - 1, - 1, 100, 25, 26, - 1, 104,
105, - 1, 107, - 1, - 1, 110, 111, 112, - 1, 38, - 1, 40, 117, 118, 43, - 1, - 1,
- 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, - 1, - 1, 93, - 1, - 1, 96,
- 1, - 1, - 1, 100, 25, 26, - 1, 104, 105, - 1, 107, - 1, - 1, 110, 111, 112, -
1, 38, - 1, 40, 117, 118, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54,
- 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1,
89, 25, 26, - 1, - 1, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40,
106, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57,
58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1,
93, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1,
110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, - 1, - 1, - 1,
96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112,
- 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, - 1, - 1, -
1, 100, - 1, - 1, 38, - 1, 40, 106, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50,
51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84,
85, 86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1,
38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, -
1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89,
25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1,
107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, -
1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, - 1,
- 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, 106, 107, 43, - 1, 110,
111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, - 1, - 1, - 1, 96, -
1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1,
- 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, - 1, - 1, - 1, 96, - 1, - 1, - 1,
100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51,
117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85,
86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38,
- 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1,
- 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89,
25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1,
107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, -
1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, - 1,
- 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110,
111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96,
97, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1,
- 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1,
100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51,
117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85,
86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38,
- 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1,
- 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89,
25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1,
107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, -
1, 60, 61, - 1, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 38, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, 50, 51, - 1, -
1, 54, 96, - 1, - 1, 58, 100, 60, 61, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110,
111, 112, - 1, - 1, - 1, - 1, 117, 118, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86,
87, - 1, 89, 25, 26, - 1, 93, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, -
1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1,
- 1, - 1, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 38, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, -
1, 50, 51, 93, - 1, 54, 96, - 1, - 1, 58, 100, 60, 61, - 1, - 1, - 1, - 1, 107,
- 1, - 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, - 1, - 1, 25, 26, - 1, 83,
84, 85, 86, 87, - 1, 89, - 1, - 1, - 1, 38, - 1, - 1, 96, - 1, - 1, - 1, 100, -
1, - 1, - 1, - 1, 50, 51, 107, - 1, 54, 110, 111, 112, 58, - 1, 60, 61, 117, 118,
- 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 38, - 1,
- 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, 50, 51, - 1, - 1, 54, 96, - 1, - 1,
58, 100, 60, 61, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1,
- 1, 117, 118, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, - 1, -
1, - 1, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1,
- 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, 37, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 48, 49, - 1, 37, - 1, -
1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 80, - 1, - 1, 83, - 1, - 1, - 1,
- 1, 88, - 1, 90, - 1, 92, - 1, 80, - 1, - 1, - 1, - 1, 99, 100, - 1, 88, - 1,
90, - 1, 92, - 1, - 1, - 1, - 1, - 1, - 1, 99, 100, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 37, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 48, 49,
- 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 80, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 88, - 1, 90, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 99,
100}; unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex();
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL"; static
int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static
int Cyc_yynerrs= 0; struct _tuple19{ void* f1; void* f2; } ; struct _tuple20{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int
yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short*
yyss=( short*)({ unsigned int _temp3258= 10000u; short* _temp3259=( short*)
GC_malloc_atomic( sizeof( short) * _temp3258);{ unsigned int _temp3260=
_temp3258; unsigned int i; for( i= 0; i < _temp3260; i ++){ _temp3259[ i]=(
short) 0;}}; _temp3259;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3255= 10000u; void** _temp3256=( void**) GC_malloc( sizeof(
void*) * _temp3255);{ unsigned int _temp3257= _temp3255; unsigned int i; for( i=
0; i < _temp3257; i ++){ _temp3256[ i]= Cyc_yylval;}}; _temp3256;}); int
yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({ unsigned int
_temp3252= 10000u; struct Cyc_Yyltype* _temp3253=( struct Cyc_Yyltype*)
GC_malloc_atomic( sizeof( struct Cyc_Yyltype) * _temp3252);{ unsigned int
_temp3254= _temp3252; unsigned int i; for( i= 0; i < _temp3254; i ++){ _temp3253[
i]= Cyc_yynewloc();}}; _temp3253;}); int yystacksize= 10000; void* yyval= Cyc_yylval;
int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2;
yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate: yyss[
_check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate; if(
yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 795u, yystate)]; if( yyn == - 32768){ goto
yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <=
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
346: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 347u, Cyc_yychar)]:
230;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4425)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4426u, yyn)] != yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4426u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrlab;}} if( yyn == 794){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus != 0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 795u, yystate)]; if(
yyn == 0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 404u, yyn)]; if( yylen > 0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset + 1) - yylen)];} switch(
yyn){ case 1: _LL1455: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1456:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1458=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1458[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1459; _temp1459.tag= Cyc_DeclList_tok; _temp1459.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1459;});
_temp1458;}); break; case 3: _LL1457: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1461=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1461[ 0]=({ struct Cyc_DeclList_tok_struct _temp1462; _temp1462.tag= Cyc_DeclList_tok;
_temp1462.f1=({ struct Cyc_List_List* _temp1463=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1463->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1464=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1464->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1465=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1465[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1466; _temp1466.tag= Cyc_Absyn_Using_d;
_temp1466.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1466.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1466;}); _temp1465;}));
_temp1464->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1464;});
_temp1463->tl= 0; _temp1463;}); _temp1462;}); _temp1461;}); Cyc_Lex_leave_using();
break; case 4: _LL1460: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1468=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1468[ 0]=({ struct Cyc_DeclList_tok_struct _temp1469; _temp1469.tag= Cyc_DeclList_tok;
_temp1469.f1=({ struct Cyc_List_List* _temp1470=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1470->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1471=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1471->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1472=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1472[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1473; _temp1473.tag= Cyc_Absyn_Using_d;
_temp1473.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp1473.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1473;});
_temp1472;})); _temp1471->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1471;}); _temp1470->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1470;}); _temp1469;});
_temp1468;}); break; case 5: _LL1467: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1475=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1475[ 0]=({ struct Cyc_DeclList_tok_struct _temp1476; _temp1476.tag= Cyc_DeclList_tok;
_temp1476.f1=({ struct Cyc_List_List* _temp1477=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1477->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1478=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1478->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1479=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1479[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1480; _temp1480.tag=
Cyc_Absyn_Namespace_d; _temp1480.f1=({ struct _tagged_arr* _temp1481=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp1481[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1481;});
_temp1480.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1480;}); _temp1479;})); _temp1478->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1478;}); _temp1477->tl= 0; _temp1477;}); _temp1476;}); _temp1475;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1474: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1483=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1483[ 0]=({ struct Cyc_DeclList_tok_struct _temp1484; _temp1484.tag= Cyc_DeclList_tok;
_temp1484.f1=({ struct Cyc_List_List* _temp1485=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1485->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1486=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1486->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1487=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1487[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1488; _temp1488.tag=
Cyc_Absyn_Namespace_d; _temp1488.f1=({ struct _tagged_arr* _temp1489=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp1489[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1489;});
_temp1488.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1488;}); _temp1487;})); _temp1486->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1486;}); _temp1485->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1485;}); _temp1484;});
_temp1483;}); break; case 7: _LL1482: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1491=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1491[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1492; _temp1492.tag= Cyc_DeclList_tok; _temp1492.f1=({ struct Cyc_List_List*
_temp1493=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1493->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1494=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1494->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1495=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1495[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1496; _temp1496.tag= Cyc_Absyn_ExternC_d;
_temp1496.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1496;}); _temp1495;})); _temp1494->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1494;}); _temp1493->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1493;}); _temp1492;});
_temp1491;}); break; case 8: _LL1490: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1498=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1498[ 0]=({ struct Cyc_DeclList_tok_struct _temp1499; _temp1499.tag= Cyc_DeclList_tok;
_temp1499.f1= 0; _temp1499;}); _temp1498;}); break; case 9: _LL1497: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1501=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1501[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1502; _temp1502.tag= Cyc_DeclList_tok; _temp1502.f1=({ struct Cyc_List_List*
_temp1503=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1503->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1504=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1504[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1505; _temp1505.tag= Cyc_Absyn_Fn_d;
_temp1505.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1505;}); _temp1504;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1503->tl=
0; _temp1503;}); _temp1502;}); _temp1501;}); break; case 10: _LL1500: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1506: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1508=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1508[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1509; _temp1509.tag= Cyc_FnDecl_tok; _temp1509.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1509;});
_temp1508;}); break; case 12: _LL1507: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1511=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1511[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1512; _temp1512.tag= Cyc_FnDecl_tok;
_temp1512.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1513=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1513->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1513;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1512;}); _temp1511;}); break; case 13: _LL1510: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1515=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1515[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1516; _temp1516.tag= Cyc_FnDecl_tok;
_temp1516.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1516;}); _temp1515;}); break; case 14: _LL1514: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1518=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1518[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1519; _temp1519.tag= Cyc_FnDecl_tok;
_temp1519.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1520=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1520->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1520;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1519;}); _temp1518;}); break; case 15: _LL1517: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1522=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1522[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1523; _temp1523.tag= Cyc_FnDecl_tok;
_temp1523.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1524=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1524->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1524;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1523;}); _temp1522;}); break; case 16: _LL1521: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1526=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1526[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1527; _temp1527.tag= Cyc_FnDecl_tok;
_temp1527.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1528=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1528->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1528;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1527;}); _temp1526;}); break; case 17: _LL1525: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1529:
Cyc_Lex_leave_using(); break; case 19: _LL1530: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1532=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp1532[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1532;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1531: Cyc_Lex_leave_namespace(); break; case 21: _LL1533: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1535=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1535[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1536; _temp1536.tag= Cyc_DeclList_tok; _temp1536.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp1536;}); _temp1535;}); break; case 22: _LL1534: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1538=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1538[ 0]=({ struct Cyc_DeclList_tok_struct _temp1539; _temp1539.tag= Cyc_DeclList_tok;
_temp1539.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1539;}); _temp1538;}); break; case 23: _LL1537: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1541=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1541[ 0]=({ struct Cyc_DeclList_tok_struct _temp1542; _temp1542.tag= Cyc_DeclList_tok;
_temp1542.f1=({ struct Cyc_List_List* _temp1543=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1543->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1543->tl=
0; _temp1543;}); _temp1542;}); _temp1541;}); break; case 24: _LL1540: { struct
Cyc_List_List* _temp1545= 0;{ struct Cyc_List_List* _temp1546= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); for( 0;
_temp1546 != 0; _temp1546=(( struct Cyc_List_List*) _check_null( _temp1546))->tl){
struct _tagged_arr* _temp1547=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1546))->hd; struct _tuple1* _temp1548=({ struct _tuple1*
_temp1551=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1551->f1=
Cyc_Absyn_rel_ns_null; _temp1551->f2= _temp1547; _temp1551;}); struct Cyc_Absyn_Vardecl*
_temp1549= Cyc_Absyn_new_vardecl( _temp1548, Cyc_Absyn_wildtyp( 0), 0);
_temp1545=({ struct Cyc_List_List* _temp1550=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1550->hd=( void*) _temp1549; _temp1550->tl=
_temp1545; _temp1550;});}} _temp1545=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1545); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1552=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1552[ 0]=({ struct Cyc_DeclList_tok_struct _temp1553; _temp1553.tag= Cyc_DeclList_tok;
_temp1553.f1=({ struct Cyc_List_List* _temp1554=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1554->hd=( void*) Cyc_Absyn_letv_decl(
_temp1545, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1554->tl= 0; _temp1554;}); _temp1553;});
_temp1552;}); break;} case 25: _LL1544: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1555:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1557=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1557[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1558; _temp1558.tag= Cyc_DeclList_tok; _temp1558.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1558;});
_temp1557;}); break; case 27: _LL1556: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1560=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1560[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1561; _temp1561.tag= Cyc_DeclSpec_tok;
_temp1561.f1=({ struct Cyc_Parse_Declaration_spec* _temp1562=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1562->sc=({ struct
Cyc_Core_Opt* _temp1563=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp1563->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1563;});
_temp1562->tq= Cyc_Absyn_empty_tqual(); _temp1562->type_specs= 0; _temp1562->is_inline=
0; _temp1562->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1562;}); _temp1561;});
_temp1560;}); break; case 28: _LL1559: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc != 0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1565=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1565[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1566; _temp1566.tag= Cyc_DeclSpec_tok; _temp1566.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1567=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1567->sc=({ struct Cyc_Core_Opt* _temp1568=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1568->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1568;});
_temp1567->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1567->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1567->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1567->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1567;}); _temp1566;}); _temp1565;}); break; case 29: _LL1564: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1570=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1570[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1571; _temp1571.tag= Cyc_DeclSpec_tok; _temp1571.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1572=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1572->sc= 0; _temp1572->tq= Cyc_Absyn_empty_tqual(); _temp1572->type_specs=({
struct Cyc_List_List* _temp1573=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1573->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1573->tl= 0;
_temp1573;}); _temp1572->is_inline= 0; _temp1572->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1572;});
_temp1571;}); _temp1570;}); break; case 30: _LL1569: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1575=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1575[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1576; _temp1576.tag= Cyc_DeclSpec_tok;
_temp1576.f1=({ struct Cyc_Parse_Declaration_spec* _temp1577=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1577->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1577->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1577->type_specs=({ struct Cyc_List_List* _temp1578=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1578->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1578->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1578;});
_temp1577->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1577->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1577;}); _temp1576;}); _temp1575;}); break; case 31: _LL1574: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1580=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1580[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1581; _temp1581.tag= Cyc_DeclSpec_tok; _temp1581.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1582=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1582->sc= 0; _temp1582->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1582->type_specs=
0; _temp1582->is_inline= 0; _temp1582->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1582;});
_temp1581;}); _temp1580;}); break; case 32: _LL1579: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1584=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1584[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1585; _temp1585.tag= Cyc_DeclSpec_tok;
_temp1585.f1=({ struct Cyc_Parse_Declaration_spec* _temp1586=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1586->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1586->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1586->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1586->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1586->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1586;}); _temp1585;}); _temp1584;}); break; case 33: _LL1583: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1588=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1588[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1589; _temp1589.tag= Cyc_DeclSpec_tok; _temp1589.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1590=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1590->sc= 0; _temp1590->tq= Cyc_Absyn_empty_tqual(); _temp1590->type_specs=
0; _temp1590->is_inline= 1; _temp1590->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1590;});
_temp1589;}); _temp1588;}); break; case 34: _LL1587: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1592=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1592[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1593; _temp1593.tag= Cyc_DeclSpec_tok;
_temp1593.f1=({ struct Cyc_Parse_Declaration_spec* _temp1594=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1594->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1594->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1594->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1594->is_inline=
1; _temp1594->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1594;}); _temp1593;}); _temp1592;}); break; case 35: _LL1591: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1596=( struct Cyc_StorageClass_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1596[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1597; _temp1597.tag= Cyc_StorageClass_tok;
_temp1597.f1= Cyc_Parse_Auto_sc; _temp1597;}); _temp1596;}); break; case 36:
_LL1595: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1599=( struct
Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1599[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1600; _temp1600.tag=
Cyc_StorageClass_tok; _temp1600.f1= Cyc_Parse_Register_sc; _temp1600;});
_temp1599;}); break; case 37: _LL1598: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1602=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1602[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1603; _temp1603.tag= Cyc_StorageClass_tok; _temp1603.f1= Cyc_Parse_Static_sc;
_temp1603;}); _temp1602;}); break; case 38: _LL1601: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1605=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1605[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1606; _temp1606.tag= Cyc_StorageClass_tok; _temp1606.f1= Cyc_Parse_Extern_sc;
_temp1606;}); _temp1605;}); break; case 39: _LL1604: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1608=( struct Cyc_StorageClass_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1608[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1609; _temp1609.tag= Cyc_StorageClass_tok;
_temp1609.f1= Cyc_Parse_ExternC_sc; _temp1609;}); _temp1608;}); break; case 40:
_LL1607: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1611=( struct
Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1611[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1612; _temp1612.tag=
Cyc_StorageClass_tok; _temp1612.f1= Cyc_Parse_Typedef_sc; _temp1612;});
_temp1611;}); break; case 41: _LL1610: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1614=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1614[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1615; _temp1615.tag= Cyc_StorageClass_tok; _temp1615.f1= Cyc_Parse_Abstract_sc;
_temp1615;}); _temp1614;}); break; case 42: _LL1613: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1617=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1617[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1618; _temp1618.tag=
Cyc_AttributeList_tok; _temp1618.f1= 0; _temp1618;}); _temp1617;}); break; case
43: _LL1616: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1619: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1621=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1621[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1622; _temp1622.tag=
Cyc_AttributeList_tok; _temp1622.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1622;});
_temp1621;}); break; case 45: _LL1620: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1624=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1624[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1625; _temp1625.tag=
Cyc_AttributeList_tok; _temp1625.f1=({ struct Cyc_List_List* _temp1626=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1626->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1626->tl= 0; _temp1626;}); _temp1625;}); _temp1624;});
break; case 46: _LL1623: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1628=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1628[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1629; _temp1629.tag=
Cyc_AttributeList_tok; _temp1629.f1=({ struct Cyc_List_List* _temp1630=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1630->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1630->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1630;}); _temp1629;});
_temp1628;}); break; case 47: _LL1627: { struct _tagged_arr _temp1632= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1632, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1632, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1632, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1632,
sizeof( unsigned char),( int)( _get_arr_size( _temp1632, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1632,
sizeof( unsigned char),( int)( _get_arr_size( _temp1632, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1632=( struct _tagged_arr) Cyc_String_substring(
_temp1632, 2,( int)( _get_arr_size( _temp1632, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1632,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1633=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1633[ 0]=({ struct Cyc_Attribute_tok_struct _temp1634; _temp1634.tag= Cyc_Attribute_tok;
_temp1634.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1634;}); _temp1633;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1635=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1635[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1636; _temp1636.tag= Cyc_Attribute_tok; _temp1636.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1636;}); _temp1635;});} break;}} case
48: _LL1631: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1638=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1638[ 0]=({ struct Cyc_Attribute_tok_struct _temp1639; _temp1639.tag= Cyc_Attribute_tok;
_temp1639.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1639;}); _temp1638;});
break; case 49: _LL1637: { struct _tagged_arr _temp1641= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct
_tuple16 _temp1644; int _temp1645; struct _tuple16* _temp1642= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1644=*
_temp1642; _LL1646: _temp1645= _temp1644.f2; goto _LL1643; _LL1643: { void* a;
if( Cyc_String_zstrcmp( _temp1641, _tag_arr("regparm", sizeof( unsigned char), 8u))
== 0? 1: Cyc_String_zstrcmp( _temp1641, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) == 0){ if( _temp1645 <= 0? 1: _temp1645 > 3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 1 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1647=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1647[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1648; _temp1648.tag= Cyc_Absyn_Regparm_att; _temp1648.f1= _temp1645;
_temp1648;}); _temp1647;});} else{ if( Cyc_String_zstrcmp( _temp1641, _tag_arr("aligned",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1641, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) == 0){ if( _temp1645 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
unsigned int j=( unsigned int) _temp1645; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1649=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1649[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1650; _temp1650.tag= Cyc_Absyn_Aligned_att;
_temp1650.f1= _temp1645; _temp1650;}); _temp1649;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1651=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1651[ 0]=({ struct Cyc_Attribute_tok_struct _temp1652; _temp1652.tag= Cyc_Attribute_tok;
_temp1652.f1=( void*) a; _temp1652;}); _temp1651;}); break;}} case 50: _LL1640: {
struct _tagged_arr _temp1654= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct _tagged_arr
_temp1655= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); void* a; if( Cyc_String_zstrcmp( _temp1654, _tag_arr("section",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1654, _tag_arr("__section__",
sizeof( unsigned char), 12u)) == 0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1656=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1656[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1657; _temp1657.tag=
Cyc_Absyn_Section_att; _temp1657.f1= _temp1655; _temp1657;}); _temp1656;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1658=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1658[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1659; _temp1659.tag= Cyc_Attribute_tok; _temp1659.f1=(
void*) a; _temp1659;}); _temp1658;}); break;} case 51: _LL1653: { struct
_tagged_arr _temp1661= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)]); struct _tagged_arr
_temp1662= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); int _temp1665; struct _tuple16 _temp1663=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1666:
_temp1665= _temp1663.f2; goto _LL1664; _LL1664: { int _temp1669; struct _tuple16
_temp1667=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _LL1670: _temp1669= _temp1667.f2; goto _LL1668; _LL1668: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_String_zstrcmp( _temp1661,
_tag_arr("format", sizeof( unsigned char), 7u)) == 0? 1: Cyc_String_zstrcmp(
_temp1661, _tag_arr("__format__", sizeof( unsigned char), 11u)) == 0){ if( Cyc_String_zstrcmp(
_temp1662, _tag_arr("printf", sizeof( unsigned char), 7u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1671=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1671[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1672; _temp1672.tag= Cyc_Absyn_Format_att;
_temp1672.f1=( void*)(( void*) Cyc_Absyn_Printf_ft); _temp1672.f2= _temp1665;
_temp1672.f3= _temp1669; _temp1672;}); _temp1671;});} else{ if( Cyc_String_zstrcmp(
_temp1662, _tag_arr("scanf", sizeof( unsigned char), 6u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1673=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1673[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1674; _temp1674.tag= Cyc_Absyn_Format_att;
_temp1674.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft); _temp1674.f2= _temp1665;
_temp1674.f3= _temp1669; _temp1674;}); _temp1673;});} else{ Cyc_Parse_err(
_tag_arr("unrecognized format type", sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line));}}}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1675=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1675[ 0]=({ struct Cyc_Attribute_tok_struct _temp1676; _temp1676.tag= Cyc_Attribute_tok;
_temp1676.f1=( void*) a; _temp1676;}); _temp1675;}); break;}}} case 52: _LL1660:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1678=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1678[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1679; _temp1679.tag= Cyc_TypeSpecifier_tok;
_temp1679.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1679;});
_temp1678;}); break; case 53: _LL1677: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1681=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1681[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1682; _temp1682.tag=
Cyc_TypeSpecifier_tok; _temp1682.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1682;}); _temp1681;}); break; case 54: _LL1680:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1684=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1684[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1685; _temp1685.tag= Cyc_TypeSpecifier_tok;
_temp1685.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1685;});
_temp1684;}); break; case 55: _LL1683: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1687=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1687[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1688; _temp1688.tag=
Cyc_TypeSpecifier_tok; _temp1688.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1689=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1689[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1690; _temp1690.tag=
Cyc_Parse_Short_spec; _temp1690.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1690;});
_temp1689;})); _temp1688;}); _temp1687;}); break; case 56: _LL1686: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1692=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1692[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1693; _temp1693.tag= Cyc_TypeSpecifier_tok;
_temp1693.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1693;});
_temp1692;}); break; case 57: _LL1691: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1695=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1695[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1696; _temp1696.tag=
Cyc_TypeSpecifier_tok; _temp1696.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1697=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1697[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1698; _temp1698.tag= Cyc_Parse_Long_spec;
_temp1698.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1698;}); _temp1697;})); _temp1696;}); _temp1695;});
break; case 58: _LL1694: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1700=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1700[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1701; _temp1701.tag=
Cyc_TypeSpecifier_tok; _temp1701.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1701;}); _temp1700;}); break; case 59: _LL1699:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1703=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1703[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1704; _temp1704.tag= Cyc_TypeSpecifier_tok;
_temp1704.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1704;});
_temp1703;}); break; case 60: _LL1702: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1706=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1706[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1707; _temp1707.tag=
Cyc_TypeSpecifier_tok; _temp1707.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1708=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1708[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1709; _temp1709.tag=
Cyc_Parse_Signed_spec; _temp1709.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1709;});
_temp1708;})); _temp1707;}); _temp1706;}); break; case 61: _LL1705: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1711=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1711[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1712; _temp1712.tag= Cyc_TypeSpecifier_tok;
_temp1712.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1713=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1713[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1714; _temp1714.tag= Cyc_Parse_Unsigned_spec; _temp1714.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1714;});
_temp1713;})); _temp1712;}); _temp1711;}); break; case 62: _LL1710: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1715:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1716: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1717: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1719=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1719[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1720; _temp1720.tag=
Cyc_TypeSpecifier_tok; _temp1720.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_empty_conref()), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1720;});
_temp1719;}); break; case 66: _LL1718: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1722=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1722[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1723; _temp1723.tag=
Cyc_TypeSpecifier_tok; _temp1723.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1723;}); _temp1722;}); break; case 67: _LL1721: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1725=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1725[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1726; _temp1726.tag=
Cyc_TypeSpecifier_tok; _temp1726.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1727=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1727[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1728; _temp1728.tag= Cyc_Absyn_TypedefType;
_temp1728.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1728.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1728.f3= 0;
_temp1728;}); _temp1727;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1726;});
_temp1725;}); break; case 68: _LL1724: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1730=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1730[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1731; _temp1731.tag=
Cyc_TypeSpecifier_tok; _temp1731.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1732=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1732[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1733; _temp1733.tag= Cyc_Absyn_TupleType;
_temp1733.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); _temp1733;});
_temp1732;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1731;}); _temp1730;}); break; case 69:
_LL1729: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1735=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1735[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1736; _temp1736.tag=
Cyc_TypeSpecifier_tok; _temp1736.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1737=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1737[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1738; _temp1738.tag= Cyc_Absyn_RgnHandleType;
_temp1738.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1738;}); _temp1737;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1736;}); _temp1735;}); break; case 70: _LL1734: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1740=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1740[ 0]=({ struct Cyc_Kind_tok_struct _temp1741; _temp1741.tag= Cyc_Kind_tok;
_temp1741.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1741;});
_temp1740;}); break; case 71: _LL1739: { struct _tagged_arr* _temp1745; void*
_temp1747; struct _tuple1 _temp1743=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1748: _temp1747=
_temp1743.f1; goto _LL1746; _LL1746: _temp1745= _temp1743.f2; goto _LL1744;
_LL1744: if( _temp1747 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1749=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1749[ 0]=({ struct Cyc_Kind_tok_struct
_temp1750; _temp1750.tag= Cyc_Kind_tok; _temp1750.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1745, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1750;}); _temp1749;}); break;} case 72:
_LL1742: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1752=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1752[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1753; _temp1753.tag= Cyc_TypeQual_tok;
_temp1753.f1=({ struct Cyc_Absyn_Tqual _temp1754; _temp1754.q_const= 1;
_temp1754.q_volatile= 0; _temp1754.q_restrict= 0; _temp1754;}); _temp1753;});
_temp1752;}); break; case 73: _LL1751: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1756=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1756[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1757; _temp1757.tag= Cyc_TypeQual_tok; _temp1757.f1=({ struct Cyc_Absyn_Tqual
_temp1758; _temp1758.q_const= 0; _temp1758.q_volatile= 1; _temp1758.q_restrict=
0; _temp1758;}); _temp1757;}); _temp1756;}); break; case 74: _LL1755: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1760=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1760[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1761; _temp1761.tag= Cyc_TypeQual_tok;
_temp1761.f1=({ struct Cyc_Absyn_Tqual _temp1762; _temp1762.q_const= 0;
_temp1762.q_volatile= 0; _temp1762.q_restrict= 1; _temp1762;}); _temp1761;});
_temp1760;}); break; case 75: _LL1759: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1764=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1764[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1765; _temp1765.tag=
Cyc_TypeSpecifier_tok; _temp1765.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1766=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1766[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1767; _temp1767.tag= Cyc_Parse_Decl_spec;
_temp1767.f1=({ struct Cyc_Absyn_Decl* _temp1768=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1768->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1769=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1769[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1770; _temp1770.tag= Cyc_Absyn_Enum_d; _temp1770.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1771=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1771->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1771->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1771->fields=({
struct Cyc_Core_Opt* _temp1772=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1772->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1772;});
_temp1771;}); _temp1770;}); _temp1769;})); _temp1768->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1768;}); _temp1767;}); _temp1766;})); _temp1765;}); _temp1764;}); break;
case 76: _LL1763: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1774=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1774[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1775; _temp1775.tag=
Cyc_TypeSpecifier_tok; _temp1775.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1776=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1776[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1777; _temp1777.tag= Cyc_Absyn_EnumType;
_temp1777.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1777.f2= 0; _temp1777;}); _temp1776;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1775;}); _temp1774;}); break; case 77: _LL1773: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1779=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1779[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1780; _temp1780.tag= Cyc_Enumfield_tok;
_temp1780.f1=({ struct Cyc_Absyn_Enumfield* _temp1781=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1781->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1781->tag= 0;
_temp1781->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1781;});
_temp1780;}); _temp1779;}); break; case 78: _LL1778: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1783=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1783[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1784; _temp1784.tag= Cyc_Enumfield_tok;
_temp1784.f1=({ struct Cyc_Absyn_Enumfield* _temp1785=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1785->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1785->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1785->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1785;});
_temp1784;}); _temp1783;}); break; case 79: _LL1782: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1787=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1787[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1788; _temp1788.tag=
Cyc_EnumfieldList_tok; _temp1788.f1=({ struct Cyc_List_List* _temp1789=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1789->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1789->tl= 0; _temp1789;}); _temp1788;}); _temp1787;});
break; case 80: _LL1786: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1791=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1791[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1792; _temp1792.tag=
Cyc_EnumfieldList_tok; _temp1792.f1=({ struct Cyc_List_List* _temp1793=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1793->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1793->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1793;}); _temp1792;});
_temp1791;}); break; case 81: _LL1790: { void* t;{ enum  Cyc_Parse_Struct_or_union
_temp1795= Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _LL1797: if( _temp1795 == Cyc_Parse_Struct_su){
goto _LL1798;} else{ goto _LL1799;} _LL1799: if( _temp1795 == Cyc_Parse_Union_su){
goto _LL1800;} else{ goto _LL1796;} _LL1798: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp1801=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp1801[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp1802; _temp1802.tag= Cyc_Absyn_AnonStructType; _temp1802.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1802;});
_temp1801;}); goto _LL1796; _LL1800: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1803=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1803[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1804; _temp1804.tag= Cyc_Absyn_AnonUnionType; _temp1804.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1804;});
_temp1803;}); goto _LL1796; _LL1796:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1805=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1805[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1806; _temp1806.tag=
Cyc_TypeSpecifier_tok; _temp1806.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1807=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1807[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1808; _temp1808.tag= Cyc_Parse_Type_spec;
_temp1808.f1=( void*) t; _temp1808.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1808;});
_temp1807;})); _temp1806;}); _temp1805;}); break;} case 82: _LL1794: { struct
Cyc_List_List* _temp1810=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ enum  Cyc_Parse_Struct_or_union
_temp1811= Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _LL1813: if( _temp1811 == Cyc_Parse_Struct_su){
goto _LL1814;} else{ goto _LL1815;} _LL1815: if( _temp1811 == Cyc_Parse_Union_su){
goto _LL1816;} else{ goto _LL1812;} _LL1814: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1817=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1817->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1817;}),
_temp1810,({ struct Cyc_Core_Opt* _temp1818=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1818->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1818;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1812; _LL1816: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1819=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1819->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1819;}),
_temp1810,({ struct Cyc_Core_Opt* _temp1820=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1820->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1820;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1812; _LL1812:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1821=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1821[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1822; _temp1822.tag= Cyc_TypeSpecifier_tok;
_temp1822.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1823=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1823[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1824; _temp1824.tag= Cyc_Parse_Decl_spec;
_temp1824.f1= d; _temp1824;}); _temp1823;})); _temp1822;}); _temp1821;}); break;}
case 83: _LL1809: { struct Cyc_List_List* _temp1826=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ enum  Cyc_Parse_Struct_or_union
_temp1827= Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _LL1829: if( _temp1827 == Cyc_Parse_Struct_su){
goto _LL1830;} else{ goto _LL1831;} _LL1831: if( _temp1827 == Cyc_Parse_Union_su){
goto _LL1832;} else{ goto _LL1828;} _LL1830: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1833=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1833->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1833;}),
_temp1826,({ struct Cyc_Core_Opt* _temp1834=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1834->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1834;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1828; _LL1832: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1835=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1835->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1835;}),
_temp1826,({ struct Cyc_Core_Opt* _temp1836=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1836->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1836;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1828; _LL1828:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1837=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1837[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1838; _temp1838.tag= Cyc_TypeSpecifier_tok;
_temp1838.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1839=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1839[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1840; _temp1840.tag= Cyc_Parse_Decl_spec;
_temp1840.f1= d; _temp1840;}); _temp1839;})); _temp1838;}); _temp1837;}); break;}
case 84: _LL1825:{ enum  Cyc_Parse_Struct_or_union _temp1842= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL1844: if(
_temp1842 == Cyc_Parse_Struct_su){ goto _LL1845;} else{ goto _LL1846;} _LL1846:
if( _temp1842 == Cyc_Parse_Union_su){ goto _LL1847;} else{ goto _LL1843;}
_LL1845: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1848=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1848[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1849; _temp1849.tag=
Cyc_TypeSpecifier_tok; _temp1849.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1850=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1850[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1851; _temp1851.tag= Cyc_Absyn_StructType;
_temp1851.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1851.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1851.f3= 0;
_temp1851;}); _temp1850;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1849;});
_temp1848;}); goto _LL1843; _LL1847: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1852=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1852[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1853; _temp1853.tag=
Cyc_TypeSpecifier_tok; _temp1853.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1854=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1854[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1855; _temp1855.tag= Cyc_Absyn_UnionType;
_temp1855.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1855.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1855.f3= 0;
_temp1855;}); _temp1854;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1853;});
_temp1852;}); goto _LL1843; _LL1843:;} break; case 85: _LL1841:{ enum  Cyc_Parse_Struct_or_union
_temp1857= Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _LL1859: if( _temp1857 == Cyc_Parse_Struct_su){
goto _LL1860;} else{ goto _LL1861;} _LL1861: if( _temp1857 == Cyc_Parse_Union_su){
goto _LL1862;} else{ goto _LL1858;} _LL1860: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1863=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1863[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1864; _temp1864.tag=
Cyc_TypeSpecifier_tok; _temp1864.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1865=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1865[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1866; _temp1866.tag= Cyc_Absyn_StructType;
_temp1866.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1866.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1866.f3= 0;
_temp1866;}); _temp1865;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1864;});
_temp1863;}); goto _LL1858; _LL1862: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1867=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1867[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1868; _temp1868.tag=
Cyc_TypeSpecifier_tok; _temp1868.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1869=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1869[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1870; _temp1870.tag= Cyc_Absyn_UnionType;
_temp1870.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1870.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1870.f3= 0;
_temp1870;}); _temp1869;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1868;});
_temp1867;}); goto _LL1858; _LL1858:;} break; case 86: _LL1856: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp1872=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1872[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1873; _temp1873.tag= Cyc_TypeList_tok; _temp1873.f1= 0; _temp1873;});
_temp1872;}); break; case 87: _LL1871: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1875=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1875[ 0]=({ struct Cyc_TypeList_tok_struct _temp1876; _temp1876.tag= Cyc_TypeList_tok;
_temp1876.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1876;});
_temp1875;}); break; case 88: _LL1874:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1878=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1878[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1879; _temp1879.tag= Cyc_TypeList_tok; _temp1879.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1879;});
_temp1878;}); break; case 89: _LL1877: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1881=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1881[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1882; _temp1882.tag= Cyc_StructOrUnion_tok; _temp1882.f1= Cyc_Parse_Struct_su;
_temp1882;}); _temp1881;}); break; case 90: _LL1880: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1884=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1884[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1885; _temp1885.tag= Cyc_StructOrUnion_tok; _temp1885.f1= Cyc_Parse_Union_su;
_temp1885;}); _temp1884;}); break; case 91: _LL1883: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1887=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1887[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1888; _temp1888.tag= Cyc_StructFieldDeclList_tok; _temp1888.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1888;});
_temp1887;}); break; case 92: _LL1886: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1890=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1890[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1891; _temp1891.tag= Cyc_StructFieldDeclListList_tok; _temp1891.f1=({
struct Cyc_List_List* _temp1892=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1892->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1892->tl= 0;
_temp1892;}); _temp1891;}); _temp1890;}); break; case 93: _LL1889: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1894=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1894[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1895; _temp1895.tag= Cyc_StructFieldDeclListList_tok;
_temp1895.f1=({ struct Cyc_List_List* _temp1896=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1896->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1896->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1896;}); _temp1895;}); _temp1894;}); break;
case 94: _LL1893: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1898=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1898[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1899; _temp1899.tag=
Cyc_InitDeclList_tok; _temp1899.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1899;});
_temp1898;}); break; case 95: _LL1897: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1901=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1901[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1902; _temp1902.tag=
Cyc_InitDeclList_tok; _temp1902.f1=({ struct Cyc_List_List* _temp1903=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1903->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1903->tl= 0; _temp1903;}); _temp1902;}); _temp1901;});
break; case 96: _LL1900: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1905=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1905[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1906; _temp1906.tag=
Cyc_InitDeclList_tok; _temp1906.f1=({ struct Cyc_List_List* _temp1907=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1907->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1907->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1907;});
_temp1906;}); _temp1905;}); break; case 97: _LL1904: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1909=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1909[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1910; _temp1910.tag= Cyc_InitDecl_tok;
_temp1910.f1=({ struct _tuple15* _temp1911=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp1911->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1911->f2= 0;
_temp1911;}); _temp1910;}); _temp1909;}); break; case 98: _LL1908: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1913=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1913[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1914; _temp1914.tag= Cyc_InitDecl_tok; _temp1914.f1=({ struct _tuple15*
_temp1915=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1915->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1915->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1915;});
_temp1914;}); _temp1913;}); break; case 99: _LL1912: { struct Cyc_Absyn_Tqual tq=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])).f1; struct Cyc_List_List* _temp1917=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3; void*
_temp1918= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
struct Cyc_List_List* _temp1921; struct Cyc_List_List* _temp1923; struct _tuple0
_temp1919=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _LL1924:
_temp1923= _temp1919.f1; goto _LL1922; _LL1922: _temp1921= _temp1919.f2; goto
_LL1920; _LL1920: { struct Cyc_List_List* _temp1925=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, _temp1918, _temp1923, _temp1917), _temp1921); yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1926=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1926[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1927; _temp1927.tag= Cyc_StructFieldDeclList_tok; _temp1927.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),
_temp1925); _temp1927;}); _temp1926;}); break;}} case 100: _LL1916: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp1929=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1929[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1930; _temp1930.tag= Cyc_QualSpecList_tok;
_temp1930.f1=({ struct _tuple18* _temp1931=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1931->f1= Cyc_Absyn_empty_tqual(); _temp1931->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1932=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1932->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1932->tl=
0; _temp1932;}); _temp1931->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1931;}); _temp1930;});
_temp1929;}); break; case 101: _LL1928: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1934=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1934[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1935; _temp1935.tag=
Cyc_QualSpecList_tok; _temp1935.f1=({ struct _tuple18* _temp1936=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1936->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1936->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1937=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1937->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1937->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1937;}); _temp1936->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1936;});
_temp1935;}); _temp1934;}); break; case 102: _LL1933: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1939=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1939[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1940; _temp1940.tag= Cyc_QualSpecList_tok;
_temp1940.f1=({ struct _tuple18* _temp1941=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1941->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1941->f2= 0;
_temp1941->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1941;}); _temp1940;}); _temp1939;}); break; case
103: _LL1938: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1943=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1943[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1944; _temp1944.tag=
Cyc_QualSpecList_tok; _temp1944.f1=({ struct _tuple18* _temp1945=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1945->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1945->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1945->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1945;});
_temp1944;}); _temp1943;}); break; case 104: _LL1942: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp1947=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1947[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1948; _temp1948.tag= Cyc_DeclaratorExpoptList_tok;
_temp1948.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1948;}); _temp1947;}); break; case 105: _LL1946: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp1950=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1950[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1951; _temp1951.tag= Cyc_DeclaratorExpoptList_tok;
_temp1951.f1=({ struct Cyc_List_List* _temp1952=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1952->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1952->tl= 0;
_temp1952;}); _temp1951;}); _temp1950;}); break; case 106: _LL1949: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp1954=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1954[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1955; _temp1955.tag= Cyc_DeclaratorExpoptList_tok;
_temp1955.f1=({ struct Cyc_List_List* _temp1956=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1956->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1956->tl=
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1956;}); _temp1955;}); _temp1954;}); break; case 107:
_LL1953: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp1958=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp1958[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp1959; _temp1959.tag=
Cyc_DeclaratorExpopt_tok; _temp1959.f1=({ struct _tuple13* _temp1960=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1960->f1= Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1960->f2= 0;
_temp1960;}); _temp1959;}); _temp1958;}); break; case 108: _LL1957: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1962=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1962[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1963; _temp1963.tag= Cyc_DeclaratorExpopt_tok;
_temp1963.f1=({ struct _tuple13* _temp1964=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1964->f1=({ struct Cyc_Parse_Declarator*
_temp1966=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp1966->id=({ struct _tuple1* _temp1967=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp1967->f1= Cyc_Absyn_rel_ns_null; _temp1967->f2=
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1967;}); _temp1966->tms= 0; _temp1966;});
_temp1964->f2=({ struct Cyc_Core_Opt* _temp1965=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1965->v=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1965;});
_temp1964;}); _temp1963;}); _temp1962;}); break; case 109: _LL1961: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1969=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1969[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1970; _temp1970.tag= Cyc_DeclaratorExpopt_tok;
_temp1970.f1=({ struct _tuple13* _temp1971=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1971->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1971->f2=({
struct Cyc_Core_Opt* _temp1972=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1972->v=( void*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1972;}); _temp1971;});
_temp1970;}); _temp1969;}); break; case 110: _LL1968: { struct Cyc_List_List*
_temp1974=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1975=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1975[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1976; _temp1976.tag= Cyc_TypeSpecifier_tok;
_temp1976.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1977=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1977[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1978; _temp1978.tag= Cyc_Parse_Decl_spec;
_temp1978.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _temp1974,({
struct Cyc_Core_Opt* _temp1979=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1979->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1979;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1978;}); _temp1977;})); _temp1976;}); _temp1975;}); break;} case 111:
_LL1973: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1981=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1981[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1982; _temp1982.tag=
Cyc_TypeSpecifier_tok; _temp1982.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1983=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1983[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1984; _temp1984.tag= Cyc_Absyn_TunionType;
_temp1984.f1=({ struct Cyc_Absyn_TunionInfo _temp1985; _temp1985.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1986=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1986[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1987; _temp1987.tag= Cyc_Absyn_UnknownTunion;
_temp1987.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1988; _temp1988.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp1988.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1988;});
_temp1987;}); _temp1986;})); _temp1985.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1985.rgn=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp1985;}); _temp1984;}); _temp1983;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1982;}); _temp1981;}); break; case 112: _LL1980: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1990=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1990[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1991; _temp1991.tag= Cyc_TypeSpecifier_tok;
_temp1991.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1992=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1992[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1993; _temp1993.tag=
Cyc_Absyn_TunionType; _temp1993.f1=({ struct Cyc_Absyn_TunionInfo _temp1994;
_temp1994.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1995=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1995[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1996; _temp1996.tag= Cyc_Absyn_UnknownTunion; _temp1996.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1997; _temp1997.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1997.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp1997;}); _temp1996;}); _temp1995;})); _temp1994.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1994.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp1994;}); _temp1993;}); _temp1992;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1991;}); _temp1990;}); break; case 113:
_LL1989: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1999=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1999[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2000; _temp2000.tag=
Cyc_TypeSpecifier_tok; _temp2000.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp2001=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp2001[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp2002; _temp2002.tag= Cyc_Absyn_TunionFieldType;
_temp2002.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp2003; _temp2003.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp2004=( struct
Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp2004[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp2005; _temp2005.tag=
Cyc_Absyn_UnknownTunionfield; _temp2005.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2006; _temp2006.tunion_name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2006.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp2006.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2006;});
_temp2005;}); _temp2004;})); _temp2003.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2003;}); _temp2002;});
_temp2001;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2000;}); _temp1999;}); break; case 114:
_LL1998: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2008=( struct Cyc_Bool_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp2008[ 0]=({ struct
Cyc_Bool_tok_struct _temp2009; _temp2009.tag= Cyc_Bool_tok; _temp2009.f1= 0;
_temp2009;}); _temp2008;}); break; case 115: _LL2007: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2011=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2011[ 0]=({ struct Cyc_Bool_tok_struct
_temp2012; _temp2012.tag= Cyc_Bool_tok; _temp2012.f1= 1; _temp2012;}); _temp2011;});
break; case 116: _LL2010: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp2014=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp2014[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp2015; _temp2015.tag= Cyc_TunionFieldList_tok; _temp2015.f1=({ struct Cyc_List_List*
_temp2016=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2016->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2016->tl= 0;
_temp2016;}); _temp2015;}); _temp2014;}); break; case 117: _LL2013: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2018=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2018[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2019; _temp2019.tag= Cyc_TunionFieldList_tok;
_temp2019.f1=({ struct Cyc_List_List* _temp2020=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2020->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2020->tl=
0; _temp2020;}); _temp2019;}); _temp2018;}); break; case 118: _LL2017: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2022=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2022[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2023; _temp2023.tag= Cyc_TunionFieldList_tok;
_temp2023.f1=({ struct Cyc_List_List* _temp2024=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2024->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2024->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2024;}); _temp2023;}); _temp2022;}); break; case 119:
_LL2021: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2026=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp2026[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp2027; _temp2027.tag=
Cyc_TunionFieldList_tok; _temp2027.f1=({ struct Cyc_List_List* _temp2028=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2028->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2028->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2028;}); _temp2027;});
_temp2026;}); break; case 120: _LL2025: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2030=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2030[ 0]=({ struct Cyc_Scope_tok_struct _temp2031; _temp2031.tag= Cyc_Scope_tok;
_temp2031.f1=( void*)(( void*) Cyc_Absyn_Public); _temp2031;}); _temp2030;});
break; case 121: _LL2029: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2033=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2033[ 0]=({ struct Cyc_Scope_tok_struct _temp2034; _temp2034.tag= Cyc_Scope_tok;
_temp2034.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp2034;}); _temp2033;});
break; case 122: _LL2032: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2036=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2036[ 0]=({ struct Cyc_Scope_tok_struct _temp2037; _temp2037.tag= Cyc_Scope_tok;
_temp2037.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2037;}); _temp2036;});
break; case 123: _LL2035: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2039=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2039[ 0]=({ struct Cyc_TunionField_tok_struct _temp2040; _temp2040.tag= Cyc_TunionField_tok;
_temp2040.f1=({ struct Cyc_Absyn_Tunionfield* _temp2041=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2041->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2041->tvs= 0;
_temp2041->typs= 0; _temp2041->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2041->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2041;}); _temp2040;}); _temp2039;}); break; case 124:
_LL2038: { struct Cyc_List_List* _temp2043=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); struct Cyc_List_List*
_temp2044=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp2045=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2045[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2046; _temp2046.tag= Cyc_TunionField_tok;
_temp2046.f1=({ struct Cyc_Absyn_Tunionfield* _temp2047=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2047->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2047->tvs=
_temp2044; _temp2047->typs= _temp2043; _temp2047->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2047->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp2047;}); _temp2046;}); _temp2045;}); break;}
case 125: _LL2042: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 126: _LL2048: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2050=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2050[ 0]=({ struct Cyc_Declarator_tok_struct _temp2051; _temp2051.tag= Cyc_Declarator_tok;
_temp2051.f1=({ struct Cyc_Parse_Declarator* _temp2052=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2052->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2052->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2052;});
_temp2051;}); _temp2050;}); break; case 127: _LL2049: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2054=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2054[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2055; _temp2055.tag= Cyc_Declarator_tok;
_temp2055.f1=({ struct Cyc_Parse_Declarator* _temp2056=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2056->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2056->tms= 0;
_temp2056;}); _temp2055;}); _temp2054;}); break; case 128: _LL2053: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 129:
_LL2057: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2059=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2059[ 0]=({ struct Cyc_Declarator_tok_struct _temp2060; _temp2060.tag= Cyc_Declarator_tok;
_temp2060.f1=({ struct Cyc_Parse_Declarator* _temp2061=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2061->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->id;
_temp2061->tms=({ struct Cyc_List_List* _temp2062=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2062->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2062->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]))->tms; _temp2062;}); _temp2061;}); _temp2060;});
_temp2059;}); break; case 130: _LL2058: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2064=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2064[ 0]=({ struct Cyc_Declarator_tok_struct _temp2065; _temp2065.tag= Cyc_Declarator_tok;
_temp2065.f1=({ struct Cyc_Parse_Declarator* _temp2066=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2066->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2066->tms=({ struct Cyc_List_List* _temp2067=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2067->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2068=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2068[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2069; _temp2069.tag= Cyc_Absyn_ConstArray_mod;
_temp2069.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2069;}); _temp2068;})); _temp2067->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2067;}); _temp2066;}); _temp2065;}); _temp2064;}); break; case 131: _LL2063: {
struct _tuple17 _temp2073; struct Cyc_List_List* _temp2074; struct Cyc_Core_Opt*
_temp2076; struct Cyc_Absyn_VarargInfo* _temp2078; int _temp2080; struct Cyc_List_List*
_temp2082; struct _tuple17* _temp2071= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2073=*
_temp2071; _LL2083: _temp2082= _temp2073.f1; goto _LL2081; _LL2081: _temp2080=
_temp2073.f2; goto _LL2079; _LL2079: _temp2078= _temp2073.f3; goto _LL2077;
_LL2077: _temp2076= _temp2073.f4; goto _LL2075; _LL2075: _temp2074= _temp2073.f5;
goto _LL2072; _LL2072: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2084=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2084[ 0]=({ struct Cyc_Declarator_tok_struct _temp2085; _temp2085.tag= Cyc_Declarator_tok;
_temp2085.f1=({ struct Cyc_Parse_Declarator* _temp2086=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2086->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2086->tms=({ struct Cyc_List_List* _temp2087=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2087->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2088=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2088[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2089; _temp2089.tag= Cyc_Absyn_Function_mod;
_temp2089.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2090=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2090[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2091; _temp2091.tag= Cyc_Absyn_WithTypes;
_temp2091.f1= _temp2082; _temp2091.f2= _temp2080; _temp2091.f3= _temp2078;
_temp2091.f4= _temp2076; _temp2091.f5= _temp2074; _temp2091;}); _temp2090;}));
_temp2089;}); _temp2088;})); _temp2087->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2087;});
_temp2086;}); _temp2085;}); _temp2084;}); break;} case 132: _LL2070: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2093=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2093[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2094; _temp2094.tag= Cyc_Declarator_tok;
_temp2094.f1=({ struct Cyc_Parse_Declarator* _temp2095=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2095->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->id;
_temp2095->tms=({ struct Cyc_List_List* _temp2096=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2096->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2097=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2097[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2098; _temp2098.tag= Cyc_Absyn_Function_mod;
_temp2098.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2099=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2099[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2100; _temp2100.tag= Cyc_Absyn_WithTypes;
_temp2100.f1= 0; _temp2100.f2= 0; _temp2100.f3= 0; _temp2100.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2100.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2100;}); _temp2099;})); _temp2098;}); _temp2097;}));
_temp2096->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]))->tms; _temp2096;}); _temp2095;}); _temp2094;});
_temp2093;}); break; case 133: _LL2092: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2102=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2102[ 0]=({ struct Cyc_Declarator_tok_struct _temp2103; _temp2103.tag= Cyc_Declarator_tok;
_temp2103.f1=({ struct Cyc_Parse_Declarator* _temp2104=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2104->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2104->tms=({ struct Cyc_List_List* _temp2105=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2105->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2106=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2106[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2107; _temp2107.tag= Cyc_Absyn_Function_mod;
_temp2107.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2108=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2108[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2109; _temp2109.tag= Cyc_Absyn_NoTypes;
_temp2109.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2109.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2109;});
_temp2108;})); _temp2107;}); _temp2106;})); _temp2105->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2105;}); _temp2104;}); _temp2103;}); _temp2102;}); break; case 134: _LL2101: {
struct Cyc_List_List* _temp2111=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2112=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2112[ 0]=({ struct Cyc_Declarator_tok_struct _temp2113; _temp2113.tag= Cyc_Declarator_tok;
_temp2113.f1=({ struct Cyc_Parse_Declarator* _temp2114=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2114->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2114->tms=({ struct Cyc_List_List* _temp2115=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2115->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2116=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2116[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2117; _temp2117.tag= Cyc_Absyn_TypeParams_mod;
_temp2117.f1= _temp2111; _temp2117.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2117.f3=
0; _temp2117;}); _temp2116;})); _temp2115->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2115;});
_temp2114;}); _temp2113;}); _temp2112;}); break;} case 135: _LL2110:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2119=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2120=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2120[ 0]=({ struct Cyc_Declarator_tok_struct _temp2121; _temp2121.tag= Cyc_Declarator_tok;
_temp2121.f1=({ struct Cyc_Parse_Declarator* _temp2122=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2122->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2122->tms=({ struct Cyc_List_List* _temp2123=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2123->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2124=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2124[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2125; _temp2125.tag= Cyc_Absyn_TypeParams_mod;
_temp2125.f1= _temp2119; _temp2125.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2125.f3=
0; _temp2125;}); _temp2124;})); _temp2123->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2123;});
_temp2122;}); _temp2121;}); _temp2120;}); break;} case 136: _LL2118: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2127=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2127[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2128; _temp2128.tag= Cyc_Declarator_tok;
_temp2128.f1=({ struct Cyc_Parse_Declarator* _temp2129=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2129->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->id;
_temp2129->tms=({ struct Cyc_List_List* _temp2130=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2130->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2131=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2131[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2132; _temp2132.tag= Cyc_Absyn_Attributes_mod;
_temp2132.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2132.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2132;}); _temp2131;}));
_temp2130->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]))->tms; _temp2130;}); _temp2129;}); _temp2128;});
_temp2127;}); break; case 137: _LL2126: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2134=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2134[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2135; _temp2135.tag= Cyc_Declarator_tok;
_temp2135.f1=({ struct Cyc_Parse_Declarator* _temp2136=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2136->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2136->tms= 0;
_temp2136;}); _temp2135;}); _temp2134;}); break; case 138: _LL2133: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2138=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2138[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2139; _temp2139.tag= Cyc_Declarator_tok;
_temp2139.f1=({ struct Cyc_Parse_Declarator* _temp2140=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2140->id= Cyc_Absyn_exn_name;
_temp2140->tms= 0; _temp2140;}); _temp2139;}); _temp2138;}); break; case 139:
_LL2137: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2142=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2142[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2143; _temp2143.tag=
Cyc_TypeModifierList_tok; _temp2143.f1=({ struct Cyc_List_List* _temp2144=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2144->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2145=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2145[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2146; _temp2146.tag= Cyc_Absyn_Pointer_mod;
_temp2146.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2146.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2146.f3= Cyc_Absyn_empty_tqual(); _temp2146;}); _temp2145;}));
_temp2144->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2144;});
_temp2143;}); _temp2142;}); break; case 140: _LL2141: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2148=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2148[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2149; _temp2149.tag= Cyc_TypeModifierList_tok;
_temp2149.f1=({ struct Cyc_List_List* _temp2150=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2150->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2151=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2151[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2152; _temp2152.tag= Cyc_Absyn_Pointer_mod;
_temp2152.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2152.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2152.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2152;}); _temp2151;})); _temp2150->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0); _temp2150;});
_temp2149;}); _temp2148;}); break; case 141: _LL2147: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2154=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2154[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2155; _temp2155.tag= Cyc_TypeModifierList_tok;
_temp2155.f1=({ struct Cyc_List_List* _temp2156=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2156->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2157=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2157[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2158; _temp2158.tag= Cyc_Absyn_Pointer_mod;
_temp2158.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2158.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2158.f3= Cyc_Absyn_empty_tqual(); _temp2158;}); _temp2157;}));
_temp2156->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2156;});
_temp2155;}); _temp2154;}); break; case 142: _LL2153: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2160=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2160[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2161; _temp2161.tag= Cyc_TypeModifierList_tok;
_temp2161.f1=({ struct Cyc_List_List* _temp2162=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2162->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2163=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2163[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2164; _temp2164.tag= Cyc_Absyn_Pointer_mod;
_temp2164.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2164.f2=( void*)
Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2164.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2164;}); _temp2163;})); _temp2162->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2162;});
_temp2161;}); _temp2160;}); break; case 143: _LL2159: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2166=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2166[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2167; _temp2167.tag= Cyc_Pointer_Sort_tok;
_temp2167.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2168=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2168[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2169; _temp2169.tag=
Cyc_Absyn_Nullable_ps; _temp2169.f1= Cyc_Absyn_exp_unsigned_one; _temp2169;});
_temp2168;})); _temp2167;}); _temp2166;}); break; case 144: _LL2165: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2171=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2171[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2172; _temp2172.tag= Cyc_Pointer_Sort_tok;
_temp2172.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2173=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2173[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2174; _temp2174.tag= Cyc_Absyn_NonNullable_ps; _temp2174.f1= Cyc_Absyn_exp_unsigned_one;
_temp2174;}); _temp2173;})); _temp2172;}); _temp2171;}); break; case 145:
_LL2170: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2176=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2176[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2177; _temp2177.tag=
Cyc_Pointer_Sort_tok; _temp2177.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2178=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2178[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2179; _temp2179.tag=
Cyc_Absyn_Nullable_ps; _temp2179.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2179;});
_temp2178;})); _temp2177;}); _temp2176;}); break; case 146: _LL2175: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2181=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2181[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2182; _temp2182.tag= Cyc_Pointer_Sort_tok;
_temp2182.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2183=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2183[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2184; _temp2184.tag= Cyc_Absyn_NonNullable_ps; _temp2184.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2184;});
_temp2183;})); _temp2182;}); _temp2181;}); break; case 147: _LL2180: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2186=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2186[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2187; _temp2187.tag= Cyc_Pointer_Sort_tok;
_temp2187.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2187;});
_temp2186;}); break; case 148: _LL2185: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2189=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2189[ 0]=({ struct Cyc_Rgn_tok_struct _temp2190; _temp2190.tag= Cyc_Rgn_tok;
_temp2190.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2190;}); _temp2189;});
break; case 149: _LL2188: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 150: _LL2191: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2193=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2193[ 0]=({ struct Cyc_Rgn_tok_struct _temp2194; _temp2194.tag= Cyc_Rgn_tok;
_temp2194.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2194;}); _temp2193;}); break; case 151: _LL2192:
if( Cyc_yyget_Kind_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err( _tag_arr("expecting region kind\n",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Rgn_tok_struct* _temp2196=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp2196[ 0]=({ struct Cyc_Rgn_tok_struct
_temp2197; _temp2197.tag= Cyc_Rgn_tok; _temp2197.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind)); _temp2197;});
_temp2196;}); break; case 152: _LL2195: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp2199=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp2199[ 0]=({ struct Cyc_Rgn_tok_struct _temp2200; _temp2200.tag= Cyc_Rgn_tok;
_temp2200.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2201=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2201->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2201;}), 0); _temp2200;}); _temp2199;});
break; case 153: _LL2198: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 154: _LL2202: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2204=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2204[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2205; _temp2205.tag= Cyc_TypeQual_tok; _temp2205.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2205;});
_temp2204;}); break; case 155: _LL2203: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2207=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2207[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2208; _temp2208.tag= Cyc_ParamDeclListBool_tok; _temp2208.f1=({ struct
_tuple17* _temp2209=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2209->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])); _temp2209->f2= 0; _temp2209->f3= 0; _temp2209->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2209->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2209;}); _temp2208;}); _temp2207;}); break; case 156:
_LL2206: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2211=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp2211[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp2212; _temp2212.tag=
Cyc_ParamDeclListBool_tok; _temp2212.f1=({ struct _tuple17* _temp2213=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2213->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)])); _temp2213->f2= 1;
_temp2213->f3= 0; _temp2213->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2213->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2213;});
_temp2212;}); _temp2211;}); break; case 157: _LL2210: { void* _temp2217; struct
Cyc_Absyn_Tqual _temp2219; struct Cyc_Core_Opt* _temp2221; struct _tuple2
_temp2215=* Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _LL2222: _temp2221= _temp2215.f1; goto _LL2220; _LL2220:
_temp2219= _temp2215.f2; goto _LL2218; _LL2218: _temp2217= _temp2215.f3; goto
_LL2216; _LL2216: { struct Cyc_Absyn_VarargInfo* _temp2223=({ struct Cyc_Absyn_VarargInfo*
_temp2227=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2227->name= _temp2221; _temp2227->tq= _temp2219; _temp2227->type=( void*)
_temp2217; _temp2227->rgn=( void*) Cyc_yyget_Rgn_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2227->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2227;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2224=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2224[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2225; _temp2225.tag= Cyc_ParamDeclListBool_tok; _temp2225.f1=({ struct
_tuple17* _temp2226=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2226->f1= 0; _temp2226->f2= 0; _temp2226->f3= _temp2223; _temp2226->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2226->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2226;}); _temp2225;}); _temp2224;}); break;}} case 158:
_LL2214: { void* _temp2231; struct Cyc_Absyn_Tqual _temp2233; struct Cyc_Core_Opt*
_temp2235; struct _tuple2 _temp2229=* Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL2236: _temp2235=
_temp2229.f1; goto _LL2234; _LL2234: _temp2233= _temp2229.f2; goto _LL2232;
_LL2232: _temp2231= _temp2229.f3; goto _LL2230; _LL2230: { struct Cyc_Absyn_VarargInfo*
_temp2237=({ struct Cyc_Absyn_VarargInfo* _temp2241=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp2241->name= _temp2235;
_temp2241->tq= _temp2233; _temp2241->type=( void*) _temp2231; _temp2241->rgn=(
void*) Cyc_yyget_Rgn_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2241->inject= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2241;}); yyval=(
void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2238=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp2238[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp2239; _temp2239.tag= Cyc_ParamDeclListBool_tok;
_temp2239.f1=({ struct _tuple17* _temp2240=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2240->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)])); _temp2240->f2= 0;
_temp2240->f3= _temp2237; _temp2240->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2240->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2240;});
_temp2239;}); _temp2238;}); break;}} case 159: _LL2228: yyval=( void*)({ struct
Cyc_TypeOpt_tok_struct* _temp2243=( struct Cyc_TypeOpt_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2243[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2244; _temp2244.tag= Cyc_TypeOpt_tok; _temp2244.f1= 0; _temp2244;});
_temp2243;}); break; case 160: _LL2242: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2246=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2246[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2247; _temp2247.tag= Cyc_TypeOpt_tok;
_temp2247.f1=({ struct Cyc_Core_Opt* _temp2248=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2248->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2249=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2249[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2250; _temp2250.tag= Cyc_Absyn_JoinEff;
_temp2250.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2250;}); _temp2249;})); _temp2248;}); _temp2247;});
_temp2246;}); break; case 161: _LL2245: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2252=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2252[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2253; _temp2253.tag= Cyc_Rgnorder_tok;
_temp2253.f1= 0; _temp2253;}); _temp2252;}); break; case 162: _LL2251: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL2254: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2256=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2256[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2257; _temp2257.tag= Cyc_Rgnorder_tok; _temp2257.f1=({ struct Cyc_List_List*
_temp2258=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2258->hd=( void*)({ struct _tuple19* _temp2259=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2259->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2259->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2259;}); _temp2258->tl= 0; _temp2258;});
_temp2257;}); _temp2256;}); break; case 164: _LL2255: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2261=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2261[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2262; _temp2262.tag= Cyc_Rgnorder_tok; _temp2262.f1=({ struct Cyc_List_List*
_temp2263=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2263->hd=( void*)({ struct _tuple19* _temp2264=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2264->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2264->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2264;}); _temp2263->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2263;});
_temp2262;}); _temp2261;}); break; case 165: _LL2260: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2266=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2266[ 0]=({ struct Cyc_Bool_tok_struct
_temp2267; _temp2267.tag= Cyc_Bool_tok; _temp2267.f1= 0; _temp2267;}); _temp2266;});
break; case 166: _LL2265: { struct _tagged_arr _temp2269= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_String_zstrcmp(
_temp2269, _tag_arr("inject", sizeof( unsigned char), 7u)) != 0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2270=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2270[ 0]=({ struct Cyc_Bool_tok_struct _temp2271; _temp2271.tag= Cyc_Bool_tok;
_temp2271.f1= 1; _temp2271;}); _temp2270;}); break;} case 167: _LL2268: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168:
_LL2272: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2274=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2274[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2275; _temp2275.tag= Cyc_TypeList_tok; _temp2275.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2275;});
_temp2274;}); break; case 169: _LL2273: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2277=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2277[ 0]=({ struct Cyc_TypeList_tok_struct _temp2278; _temp2278.tag= Cyc_TypeList_tok;
_temp2278.f1= 0; _temp2278;}); _temp2277;}); break; case 170: _LL2276: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case
171: _LL2279: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2281=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2281[ 0]=({ struct Cyc_TypeList_tok_struct _temp2282; _temp2282.tag= Cyc_TypeList_tok;
_temp2282.f1=({ struct Cyc_List_List* _temp2283=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2283->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),
Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2283->tl= 0; _temp2283;});
_temp2282;}); _temp2281;}); break; case 172: _LL2280: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_EffKind){
Cyc_Parse_err( _tag_arr("expecing effect kind (E)", sizeof( unsigned char), 25u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2285=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2285[ 0]=({ struct Cyc_TypeList_tok_struct _temp2286; _temp2286.tag= Cyc_TypeList_tok;
_temp2286.f1=({ struct Cyc_List_List* _temp2287=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2287->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref(( void*) Cyc_Absyn_EffKind)); _temp2287->tl= 0;
_temp2287;}); _temp2286;}); _temp2285;}); break; case 173: _LL2284: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2289=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2289[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2290; _temp2290.tag= Cyc_TypeList_tok; _temp2290.f1=({ struct Cyc_List_List*
_temp2291=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2291->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2292=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2292[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2293; _temp2293.tag= Cyc_Absyn_AccessEff;
_temp2293.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2293;}); _temp2292;})); _temp2291->tl= 0;
_temp2291;}); _temp2290;}); _temp2289;}); break; case 174: _LL2288: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2295=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2295[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2296; _temp2296.tag= Cyc_TypeList_tok; _temp2296.f1=({ struct Cyc_List_List*
_temp2297=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2297->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2298=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2298[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2299; _temp2299.tag= Cyc_Absyn_AccessEff;
_temp2299.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2299;}); _temp2298;})); _temp2297->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2297;});
_temp2296;}); _temp2295;}); break; case 175: _LL2294: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2301=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2301[ 0]=({ struct Cyc_TypeList_tok_struct _temp2302; _temp2302.tag= Cyc_TypeList_tok;
_temp2302.f1=({ struct Cyc_List_List* _temp2303=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2303->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2304=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2304[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2305; _temp2305.tag= Cyc_Absyn_AccessEff;
_temp2305.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2305;}); _temp2304;})); _temp2303->tl= 0;
_temp2303;}); _temp2302;}); _temp2301;}); break; case 176: _LL2300: if( Cyc_yyget_Kind_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]) !=( void*) Cyc_Absyn_RgnKind){
Cyc_Parse_err( _tag_arr("expecting region kind (R)", sizeof( unsigned char), 26u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2307=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2307[ 0]=({ struct Cyc_TypeList_tok_struct _temp2308; _temp2308.tag= Cyc_TypeList_tok;
_temp2308.f1=({ struct Cyc_List_List* _temp2309=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2309->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2310=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2310[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2311; _temp2311.tag= Cyc_Absyn_AccessEff;
_temp2311.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2311;}); _temp2310;})); _temp2309->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2309;});
_temp2308;}); _temp2307;}); break; case 177: _LL2306: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2313=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2313[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2314; _temp2314.tag= Cyc_ParamDeclList_tok;
_temp2314.f1=({ struct Cyc_List_List* _temp2315=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2315->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2315->tl= 0;
_temp2315;}); _temp2314;}); _temp2313;}); break; case 178: _LL2312: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2317=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2317[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2318; _temp2318.tag= Cyc_ParamDeclList_tok;
_temp2318.f1=({ struct Cyc_List_List* _temp2319=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2319->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2319->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2319;}); _temp2318;}); _temp2317;}); break; case 179:
_LL2316: { void* _temp2321= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
struct Cyc_List_List* _temp2322=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2323=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2324=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2325= Cyc_Parse_apply_tms( _temp2323, _temp2321, _temp2322,
_temp2324); if( _temp2325.f3 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
_tuple1* _temp2326=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id;{ void* _temp2327=(*
_temp2326).f1; struct Cyc_List_List* _temp2337; struct Cyc_List_List* _temp2339;
_LL2329: if( _temp2327 ==( void*) Cyc_Absyn_Loc_n){ goto _LL2330;} else{ goto
_LL2331;} _LL2331: if(( unsigned int) _temp2327 > 1u?*(( int*) _temp2327) == Cyc_Absyn_Rel_n:
0){ _LL2338: _temp2337=(( struct Cyc_Absyn_Rel_n_struct*) _temp2327)->f1; if(
_temp2337 == 0){ goto _LL2332;} else{ goto _LL2333;}} else{ goto _LL2333;}
_LL2333: if(( unsigned int) _temp2327 > 1u?*(( int*) _temp2327) == Cyc_Absyn_Abs_n:
0){ _LL2340: _temp2339=(( struct Cyc_Absyn_Abs_n_struct*) _temp2327)->f1; if(
_temp2339 == 0){ goto _LL2334;} else{ goto _LL2335;}} else{ goto _LL2335;}
_LL2335: goto _LL2336; _LL2330: goto _LL2328; _LL2332: goto _LL2328; _LL2334:
goto _LL2328; _LL2336: Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a module name",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); goto
_LL2328; _LL2328:;}{ struct Cyc_Core_Opt* _temp2341=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp2345=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2345->v=( void*)(* _temp2326).f2; _temp2345;}); if(
_temp2325.f4 != 0){ Cyc_Parse_warn( _tag_arr("extra attributes on parameter, ignoring",
sizeof( unsigned char), 40u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2325.f3 != 0){ Cyc_Parse_warn( _tag_arr("extra type variables on parameter, ignoring",
sizeof( unsigned char), 44u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2342=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2342[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2343; _temp2343.tag= Cyc_ParamDecl_tok; _temp2343.f1=({
struct _tuple2* _temp2344=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2344->f1= _temp2341; _temp2344->f2= _temp2325.f1; _temp2344->f3= _temp2325.f2;
_temp2344;}); _temp2343;}); _temp2342;}); break;}}} case 180: _LL2320: { void*
_temp2347= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2348=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; struct Cyc_Absyn_Tqual _temp2349=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; if( _temp2348
!= 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring", sizeof(
unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2350=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2350[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2351; _temp2351.tag= Cyc_ParamDecl_tok; _temp2351.f1=({
struct _tuple2* _temp2352=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2352->f1= 0; _temp2352->f2= _temp2349; _temp2352->f3= _temp2347; _temp2352;});
_temp2351;}); _temp2350;}); break;} case 181: _LL2346: { void* _temp2354= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2355=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2356=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2357=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2358= Cyc_Parse_apply_tms( _temp2356, _temp2354, _temp2355,
_temp2357); if( _temp2358.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2358.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2359=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2359[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2360; _temp2360.tag= Cyc_ParamDecl_tok; _temp2360.f1=({
struct _tuple2* _temp2361=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2361->f1= 0; _temp2361->f2= _temp2358.f1; _temp2361->f3= _temp2358.f2;
_temp2361;}); _temp2360;}); _temp2359;}); break;} case 182: _LL2353: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2363=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2363[ 0]=({ struct Cyc_IdList_tok_struct
_temp2364; _temp2364.tag= Cyc_IdList_tok; _temp2364.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2364;});
_temp2363;}); break; case 183: _LL2362: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2366=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2366[ 0]=({ struct Cyc_IdList_tok_struct _temp2367; _temp2367.tag= Cyc_IdList_tok;
_temp2367.f1=({ struct Cyc_List_List* _temp2368=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2368->hd=( void*)({ struct
_tagged_arr* _temp2369=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2369[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2369;}); _temp2368->tl=
0; _temp2368;}); _temp2367;}); _temp2366;}); break; case 184: _LL2365: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2371=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2371[ 0]=({ struct Cyc_IdList_tok_struct
_temp2372; _temp2372.tag= Cyc_IdList_tok; _temp2372.f1=({ struct Cyc_List_List*
_temp2373=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2373->hd=( void*)({ struct _tagged_arr* _temp2374=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp2374[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2374;});
_temp2373->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2373;}); _temp2372;}); _temp2371;}); break; case 185:
_LL2370: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 186: _LL2375: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 187: _LL2376: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2378=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2378[ 0]=({ struct Cyc_Exp_tok_struct _temp2379; _temp2379.tag= Cyc_Exp_tok;
_temp2379.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2380=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2380[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2381; _temp2381.tag= Cyc_Absyn_UnresolvedMem_e; _temp2381.f1= 0; _temp2381.f2=
0; _temp2381;}); _temp2380;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2379;});
_temp2378;}); break; case 188: _LL2377: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2383=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2383[ 0]=({ struct Cyc_Exp_tok_struct _temp2384; _temp2384.tag= Cyc_Exp_tok;
_temp2384.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2385=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2385[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2386; _temp2386.tag= Cyc_Absyn_UnresolvedMem_e; _temp2386.f1= 0; _temp2386.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2386;});
_temp2385;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2384;}); _temp2383;}); break; case 189:
_LL2382: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2388=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2388[ 0]=({ struct Cyc_Exp_tok_struct
_temp2389; _temp2389.tag= Cyc_Exp_tok; _temp2389.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2390=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2390[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2391; _temp2391.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2391.f1= 0; _temp2391.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp2391;});
_temp2390;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2389;}); _temp2388;}); break; case 190:
_LL2387: { struct Cyc_Absyn_Vardecl* _temp2393= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2399=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2399->f1=( void*) Cyc_Absyn_Loc_n; _temp2399->f2=({ struct _tagged_arr*
_temp2400=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp2400[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); _temp2400;}); _temp2399;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line)));
_temp2393->tq=({ struct Cyc_Absyn_Tqual _temp2394; _temp2394.q_const= 1;
_temp2394.q_volatile= 0; _temp2394.q_restrict= 1; _temp2394;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2395=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2395[ 0]=({ struct Cyc_Exp_tok_struct
_temp2396; _temp2396.tag= Cyc_Exp_tok; _temp2396.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2397=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2397[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2398; _temp2398.tag= Cyc_Absyn_Comprehension_e;
_temp2398.f1= _temp2393; _temp2398.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2398.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2398;});
_temp2397;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2396;}); _temp2395;}); break;} case
191: _LL2392: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2402=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2402[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2403; _temp2403.tag=
Cyc_InitializerList_tok; _temp2403.f1=({ struct Cyc_List_List* _temp2404=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2404->hd=(
void*)({ struct _tuple20* _temp2405=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2405->f1= 0; _temp2405->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2405;}); _temp2404->tl=
0; _temp2404;}); _temp2403;}); _temp2402;}); break; case 192: _LL2401: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2407=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2407[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2408; _temp2408.tag= Cyc_InitializerList_tok;
_temp2408.f1=({ struct Cyc_List_List* _temp2409=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2409->hd=( void*)({ struct
_tuple20* _temp2410=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2410->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2410->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2410;});
_temp2409->tl= 0; _temp2409;}); _temp2408;}); _temp2407;}); break; case 193:
_LL2406: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2412=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2412[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2413; _temp2413.tag=
Cyc_InitializerList_tok; _temp2413.f1=({ struct Cyc_List_List* _temp2414=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2414->hd=(
void*)({ struct _tuple20* _temp2415=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2415->f1= 0; _temp2415->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2415;}); _temp2414->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2414;}); _temp2413;}); _temp2412;}); break; case 194:
_LL2411: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2417=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2417[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2418; _temp2418.tag=
Cyc_InitializerList_tok; _temp2418.f1=({ struct Cyc_List_List* _temp2419=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2419->hd=(
void*)({ struct _tuple20* _temp2420=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2420->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2420->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2420;});
_temp2419->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2419;});
_temp2418;}); _temp2417;}); break; case 195: _LL2416: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2422=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2422[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2423; _temp2423.tag= Cyc_DesignatorList_tok;
_temp2423.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2423;});
_temp2422;}); break; case 196: _LL2421: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2425=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2425[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2426; _temp2426.tag=
Cyc_DesignatorList_tok; _temp2426.f1=({ struct Cyc_List_List* _temp2427=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2427->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2427->tl= 0; _temp2427;}); _temp2426;}); _temp2425;});
break; case 197: _LL2424: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2429=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2429[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2430; _temp2430.tag=
Cyc_DesignatorList_tok; _temp2430.f1=({ struct Cyc_List_List* _temp2431=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2431->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2431->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2431;});
_temp2430;}); _temp2429;}); break; case 198: _LL2428: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2433=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2433[ 0]=({ struct
Cyc_Designator_tok_struct _temp2434; _temp2434.tag= Cyc_Designator_tok;
_temp2434.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2435=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2435[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2436; _temp2436.tag=
Cyc_Absyn_ArrayElement; _temp2436.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2436;});
_temp2435;})); _temp2434;}); _temp2433;}); break; case 199: _LL2432: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2438=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2438[ 0]=({ struct
Cyc_Designator_tok_struct _temp2439; _temp2439.tag= Cyc_Designator_tok;
_temp2439.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2440=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2440[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2441; _temp2441.tag= Cyc_Absyn_FieldName;
_temp2441.f1=({ struct _tagged_arr* _temp2442=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp2442[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2442;}); _temp2441;});
_temp2440;})); _temp2439;}); _temp2438;}); break; case 200: _LL2437: { void*
_temp2444= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2445=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2445 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2446=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2447=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2447[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2448; _temp2448.tag= Cyc_ParamDecl_tok; _temp2448.f1=({
struct _tuple2* _temp2449=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp2449[ 0]=({ struct _tuple2 _temp2450; _temp2450.f1= 0; _temp2450.f2=
_temp2446; _temp2450.f3= _temp2444; _temp2450;}); _temp2449;}); _temp2448;});
_temp2447;}); break;}} case 201: _LL2443: { void* _temp2452= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2453=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2454=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2455=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2456= Cyc_Parse_apply_tms( _temp2454, _temp2452, _temp2453,
_temp2455); if( _temp2456.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2456.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring", sizeof(
unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2457=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2457[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2458; _temp2458.tag= Cyc_ParamDecl_tok; _temp2458.f1=({
struct _tuple2* _temp2459=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2459->f1= 0; _temp2459->f2= _temp2456.f1; _temp2459->f3= _temp2456.f2;
_temp2459;}); _temp2458;}); _temp2457;}); break;} case 202: _LL2451: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2461=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2461[ 0]=({ struct Cyc_Type_tok_struct
_temp2462; _temp2462.tag= Cyc_Type_tok; _temp2462.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2462;});
_temp2461;}); break; case 203: _LL2460: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2464=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2464[ 0]=({ struct Cyc_Type_tok_struct _temp2465; _temp2465.tag= Cyc_Type_tok;
_temp2465.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2466=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2466[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2467; _temp2467.tag= Cyc_Absyn_JoinEff;
_temp2467.f1= 0; _temp2467;}); _temp2466;})); _temp2465;}); _temp2464;}); break;
case 204: _LL2463: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2469=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2469[ 0]=({ struct Cyc_Type_tok_struct _temp2470; _temp2470.tag= Cyc_Type_tok;
_temp2470.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2471=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2471[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2472; _temp2472.tag= Cyc_Absyn_JoinEff;
_temp2472.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2472;}); _temp2471;})); _temp2470;});
_temp2469;}); break; case 205: _LL2468: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2474=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2474[ 0]=({ struct Cyc_Type_tok_struct _temp2475; _temp2475.tag= Cyc_Type_tok;
_temp2475.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2476=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2476[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2477; _temp2477.tag= Cyc_Absyn_JoinEff;
_temp2477.f1=({ struct Cyc_List_List* _temp2478=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2478->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2478->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2478;}); _temp2477;}); _temp2476;})); _temp2475;});
_temp2474;}); break; case 206: _LL2473: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2480=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2480[ 0]=({ struct Cyc_TypeList_tok_struct _temp2481; _temp2481.tag= Cyc_TypeList_tok;
_temp2481.f1=({ struct Cyc_List_List* _temp2482=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2482->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2482->tl= 0;
_temp2482;}); _temp2481;}); _temp2480;}); break; case 207: _LL2479: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2484=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2484[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2485; _temp2485.tag= Cyc_TypeList_tok; _temp2485.f1=({ struct Cyc_List_List*
_temp2486=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2486->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2486->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2486;});
_temp2485;}); _temp2484;}); break; case 208: _LL2483: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2488=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2488[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2489; _temp2489.tag= Cyc_AbstractDeclarator_tok;
_temp2489.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2490=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2490->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2490;});
_temp2489;}); _temp2488;}); break; case 209: _LL2487: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 210: _LL2491:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2493=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2493[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2494; _temp2494.tag=
Cyc_AbstractDeclarator_tok; _temp2494.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2495=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2495->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2495;});
_temp2494;}); _temp2493;}); break; case 211: _LL2492: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 212:
_LL2496: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2498=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2498[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2499; _temp2499.tag=
Cyc_AbstractDeclarator_tok; _temp2499.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2500=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2500->tms=({ struct Cyc_List_List* _temp2501=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2501->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2501->tl= 0; _temp2501;}); _temp2500;}); _temp2499;}); _temp2498;}); break;
case 213: _LL2497: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2503=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2503[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2504; _temp2504.tag= Cyc_AbstractDeclarator_tok; _temp2504.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2505=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2505->tms=({ struct Cyc_List_List* _temp2506=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2506->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2506->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->tms; _temp2506;});
_temp2505;}); _temp2504;}); _temp2503;}); break; case 214: _LL2502: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2508=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2508[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2509; _temp2509.tag= Cyc_AbstractDeclarator_tok;
_temp2509.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2510=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2510->tms=({
struct Cyc_List_List* _temp2511=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2511->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2512=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2512[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2513; _temp2513.tag= Cyc_Absyn_ConstArray_mod; _temp2513.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2513;});
_temp2512;})); _temp2511->tl= 0; _temp2511;}); _temp2510;}); _temp2509;});
_temp2508;}); break; case 215: _LL2507: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2515=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2515[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2516; _temp2516.tag= Cyc_AbstractDeclarator_tok; _temp2516.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2517=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2517->tms=({ struct Cyc_List_List* _temp2518=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2518->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2519=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2519[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2520; _temp2520.tag= Cyc_Absyn_ConstArray_mod;
_temp2520.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2520;}); _temp2519;})); _temp2518->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2518;}); _temp2517;}); _temp2516;}); _temp2515;}); break; case 216: _LL2514:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2522=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2522[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2523; _temp2523.tag=
Cyc_AbstractDeclarator_tok; _temp2523.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2524=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2524->tms=({ struct Cyc_List_List* _temp2525=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2525->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2526=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2526[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2527; _temp2527.tag= Cyc_Absyn_Function_mod;
_temp2527.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2528=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2528[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2529; _temp2529.tag= Cyc_Absyn_WithTypes;
_temp2529.f1= 0; _temp2529.f2= 0; _temp2529.f3= 0; _temp2529.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2529.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2529;}); _temp2528;})); _temp2527;}); _temp2526;}));
_temp2525->tl= 0; _temp2525;}); _temp2524;}); _temp2523;}); _temp2522;}); break;
case 217: _LL2521: { struct _tuple17 _temp2533; struct Cyc_List_List* _temp2534;
struct Cyc_Core_Opt* _temp2536; struct Cyc_Absyn_VarargInfo* _temp2538; int
_temp2540; struct Cyc_List_List* _temp2542; struct _tuple17* _temp2531= Cyc_yyget_ParamDeclListBool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2533=*
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
_temp2549;}); _temp2548;})); _temp2547->tl= 0; _temp2547;}); _temp2546;});
_temp2545;}); _temp2544;}); break;} case 218: _LL2530: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2553=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2553[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2554; _temp2554.tag= Cyc_AbstractDeclarator_tok;
_temp2554.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2555=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2555->tms=({
struct Cyc_List_List* _temp2556=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2556->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2557=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2557[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2558; _temp2558.tag=
Cyc_Absyn_Function_mod; _temp2558.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2559=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2559[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2560; _temp2560.tag= Cyc_Absyn_WithTypes;
_temp2560.f1= 0; _temp2560.f2= 0; _temp2560.f3= 0; _temp2560.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2560.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2560;}); _temp2559;})); _temp2558;}); _temp2557;}));
_temp2556->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->tms; _temp2556;});
_temp2555;}); _temp2554;}); _temp2553;}); break; case 219: _LL2552: { struct
_tuple17 _temp2564; struct Cyc_List_List* _temp2565; struct Cyc_Core_Opt*
_temp2567; struct Cyc_Absyn_VarargInfo* _temp2569; int _temp2571; struct Cyc_List_List*
_temp2573; struct _tuple17* _temp2562= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2564=*
_temp2562; _LL2574: _temp2573= _temp2564.f1; goto _LL2572; _LL2572: _temp2571=
_temp2564.f2; goto _LL2570; _LL2570: _temp2569= _temp2564.f3; goto _LL2568;
_LL2568: _temp2567= _temp2564.f4; goto _LL2566; _LL2566: _temp2565= _temp2564.f5;
goto _LL2563; _LL2563: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2575=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2575[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2576; _temp2576.tag= Cyc_AbstractDeclarator_tok; _temp2576.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2577=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2577->tms=({ struct Cyc_List_List* _temp2578=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2578->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2579=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2579[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2580; _temp2580.tag= Cyc_Absyn_Function_mod;
_temp2580.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2581=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2581[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2582; _temp2582.tag= Cyc_Absyn_WithTypes;
_temp2582.f1= _temp2573; _temp2582.f2= _temp2571; _temp2582.f3= _temp2569;
_temp2582.f4= _temp2567; _temp2582.f5= _temp2565; _temp2582;}); _temp2581;}));
_temp2580;}); _temp2579;})); _temp2578->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2578;}); _temp2577;}); _temp2576;}); _temp2575;}); break;} case 220:
_LL2561: { struct Cyc_List_List* _temp2584=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2585=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2585[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2586; _temp2586.tag= Cyc_AbstractDeclarator_tok; _temp2586.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2587=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2587->tms=({ struct Cyc_List_List* _temp2588=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2588->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2589=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2589[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2590; _temp2590.tag= Cyc_Absyn_TypeParams_mod;
_temp2590.f1= _temp2584; _temp2590.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2590.f3=
0; _temp2590;}); _temp2589;})); _temp2588->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2588;}); _temp2587;}); _temp2586;}); _temp2585;}); break;} case 221:
_LL2583:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2592=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2593=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2593[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2594; _temp2594.tag= Cyc_AbstractDeclarator_tok; _temp2594.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2595=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2595->tms=({ struct Cyc_List_List* _temp2596=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2596->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2597=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2597[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2598; _temp2598.tag= Cyc_Absyn_TypeParams_mod;
_temp2598.f1= _temp2592; _temp2598.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2598.f3=
0; _temp2598;}); _temp2597;})); _temp2596->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2596;}); _temp2595;}); _temp2594;}); _temp2593;}); break;} case 222:
_LL2591: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2600=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2600[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2601; _temp2601.tag=
Cyc_AbstractDeclarator_tok; _temp2601.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2602=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2602->tms=({ struct Cyc_List_List* _temp2603=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2603->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2604=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2604[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2605; _temp2605.tag= Cyc_Absyn_Attributes_mod;
_temp2605.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2605.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2605;}); _temp2604;}));
_temp2603->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->tms; _temp2603;});
_temp2602;}); _temp2601;}); _temp2600;}); break; case 223: _LL2599: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 224: _LL2606:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
225: _LL2607: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 226: _LL2608: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 227: _LL2609: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 228: _LL2610:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
229: _LL2611: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2623=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2623->name=({ struct
_tagged_arr* _temp2624=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2624[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2624;});
_temp2623->identity= 0; _temp2623->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2623;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2621=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2621[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2622; _temp2622.tag= Cyc_Absyn_VarType;
_temp2622.f1= tv; _temp2622;}); _temp2621;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2613=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2613[ 0]=({ struct Cyc_Stmt_tok_struct _temp2614; _temp2614.tag= Cyc_Stmt_tok;
_temp2614.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2615=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2615[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2616; _temp2616.tag= Cyc_Absyn_Region_s;
_temp2616.f1= tv; _temp2616.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2617=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2617->f1=(
void*) Cyc_Absyn_Loc_n; _temp2617->f2=({ struct _tagged_arr* _temp2618=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2618[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2618;});
_temp2617;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2619=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2619[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2620; _temp2620.tag=
Cyc_Absyn_RgnHandleType; _temp2620.f1=( void*) t; _temp2620;}); _temp2619;}), 0);
_temp2616.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2616;}); _temp2615;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2614;});
_temp2613;}); break;} case 230: _LL2612: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2636=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2636->name=({ struct
_tagged_arr* _temp2637=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2637[ 0]=( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp2639; _temp2639.tag= Cyc_Stdio_String_pa; _temp2639.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]);{ void* _temp2638[ 1u]={& _temp2639}; Cyc_Stdio_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2638, sizeof( void*), 1u));}});
_temp2637;}); _temp2636->identity= 0; _temp2636->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2636;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2634=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2634[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2635; _temp2635.tag= Cyc_Absyn_VarType;
_temp2635.f1= tv; _temp2635;}); _temp2634;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2626=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2626[ 0]=({ struct Cyc_Stmt_tok_struct _temp2627; _temp2627.tag= Cyc_Stmt_tok;
_temp2627.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2628=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2628[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2629; _temp2629.tag= Cyc_Absyn_Region_s;
_temp2629.f1= tv; _temp2629.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2630=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2630->f1=(
void*) Cyc_Absyn_Loc_n; _temp2630->f2=({ struct _tagged_arr* _temp2631=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2631[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2631;});
_temp2630;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2632=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2632[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2633; _temp2633.tag=
Cyc_Absyn_RgnHandleType; _temp2633.f1=( void*) t; _temp2633;}); _temp2632;}), 0);
_temp2629.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2629;}); _temp2628;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2627;});
_temp2626;}); break;} case 231: _LL2625: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2641=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2641[ 0]=({ struct Cyc_Stmt_tok_struct _temp2642; _temp2642.tag= Cyc_Stmt_tok;
_temp2642.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2643=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2643[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2644; _temp2644.tag= Cyc_Absyn_Cut_s;
_temp2644.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2644;}); _temp2643;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2642;});
_temp2641;}); break; case 232: _LL2640: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2646=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2646[ 0]=({ struct Cyc_Stmt_tok_struct _temp2647; _temp2647.tag= Cyc_Stmt_tok;
_temp2647.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2648=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2648[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2649; _temp2649.tag= Cyc_Absyn_Splice_s;
_temp2649.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2649;}); _temp2648;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2647;});
_temp2646;}); break; case 233: _LL2645: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2651=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2651[ 0]=({ struct Cyc_Stmt_tok_struct _temp2652; _temp2652.tag= Cyc_Stmt_tok;
_temp2652.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2653=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2653[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2654; _temp2654.tag= Cyc_Absyn_Label_s;
_temp2654.f1=({ struct _tagged_arr* _temp2655=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp2655[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2655;});
_temp2654.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2654;}); _temp2653;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2652;});
_temp2651;}); break; case 234: _LL2650: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2657=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2657[ 0]=({ struct Cyc_Stmt_tok_struct _temp2658; _temp2658.tag= Cyc_Stmt_tok;
_temp2658.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2658;});
_temp2657;}); break; case 235: _LL2656: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2660=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2660[ 0]=({ struct Cyc_Stmt_tok_struct _temp2661; _temp2661.tag= Cyc_Stmt_tok;
_temp2661.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2661;}); _temp2660;}); break; case 236: _LL2659: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2663=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2663[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2664; _temp2664.tag= Cyc_Stmt_tok; _temp2664.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2664;}); _temp2663;}); break; case 237: _LL2662: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 238:
_LL2665: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2667=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2667[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2668; _temp2668.tag= Cyc_Stmt_tok; _temp2668.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2668;}); _temp2667;}); break;
case 239: _LL2666: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2670=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2670[ 0]=({ struct Cyc_Stmt_tok_struct _temp2671; _temp2671.tag= Cyc_Stmt_tok;
_temp2671.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2671;});
_temp2670;}); break; case 240: _LL2669: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 241: _LL2672:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2674=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2674[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2675; _temp2675.tag= Cyc_Stmt_tok; _temp2675.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2675;}); _temp2674;}); break; case 242: _LL2673: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2677=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2677[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2678; _temp2678.tag= Cyc_Stmt_tok; _temp2678.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2679=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2679[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2680; _temp2680.tag= Cyc_Absyn_Fn_d;
_temp2680.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2680;}); _temp2679;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2678;}); _temp2677;}); break; case 243: _LL2676: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2682=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2682[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2683; _temp2683.tag= Cyc_Stmt_tok; _temp2683.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2684=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2684[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2685; _temp2685.tag= Cyc_Absyn_Fn_d;
_temp2685.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2685;}); _temp2684;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2683;}); _temp2682;}); break; case 244: _LL2681: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2687=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2687[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2688; _temp2688.tag= Cyc_Stmt_tok; _temp2688.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2688;});
_temp2687;}); break; case 245: _LL2686: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2690=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2690[ 0]=({ struct Cyc_Stmt_tok_struct _temp2691; _temp2691.tag= Cyc_Stmt_tok;
_temp2691.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2691;}); _temp2690;}); break; case 246: _LL2689: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2693=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2693[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2694; _temp2694.tag= Cyc_Stmt_tok; _temp2694.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2694;}); _temp2693;}); break; case 247: _LL2692: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2696=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2696[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2697; _temp2697.tag= Cyc_Stmt_tok; _temp2697.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2698=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2698[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2699; _temp2699.tag= Cyc_Absyn_SwitchC_s;
_temp2699.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2699.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2699;});
_temp2698;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2697;}); _temp2696;}); break; case 248:
_LL2695: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2701=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2701[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2702; _temp2702.tag= Cyc_Stmt_tok; _temp2702.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2702;});
_temp2701;}); break; case 249: _LL2700: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2704=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2704[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2705; _temp2705.tag= Cyc_SwitchClauseList_tok; _temp2705.f1= 0; _temp2705;});
_temp2704;}); break; case 250: _LL2703: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2707=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2707[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2708; _temp2708.tag= Cyc_SwitchClauseList_tok; _temp2708.f1=({ struct Cyc_List_List*
_temp2709=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2709->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2710=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2710->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp2710->pat_vars= 0; _temp2710->where_clause= 0; _temp2710->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2710->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2710;}); _temp2709->tl= 0; _temp2709;});
_temp2708;}); _temp2707;}); break; case 251: _LL2706: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2712=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2712[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2713; _temp2713.tag= Cyc_SwitchClauseList_tok;
_temp2713.f1=({ struct Cyc_List_List* _temp2714=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2714->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2715=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2715->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2715->pat_vars= 0; _temp2715->where_clause= 0;
_temp2715->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2715->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2715;});
_temp2714->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2714;}); _temp2713;});
_temp2712;}); break; case 252: _LL2711: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2717=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2717[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2718; _temp2718.tag= Cyc_SwitchClauseList_tok; _temp2718.f1=({ struct Cyc_List_List*
_temp2719=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2719->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2720=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2720->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2720->pat_vars=
0; _temp2720->where_clause= 0; _temp2720->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2720->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp2720;}); _temp2719->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2719;}); _temp2718;});
_temp2717;}); break; case 253: _LL2716: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2722=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2722[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2723; _temp2723.tag= Cyc_SwitchClauseList_tok; _temp2723.f1=({ struct Cyc_List_List*
_temp2724=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2724->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2725=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2725->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2725->pat_vars=
0; _temp2725->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2725->body=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2725->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2725;});
_temp2724->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2724;}); _temp2723;});
_temp2722;}); break; case 254: _LL2721: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2727=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2727[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2728; _temp2728.tag= Cyc_SwitchClauseList_tok; _temp2728.f1=({ struct Cyc_List_List*
_temp2729=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2729->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2730=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2730->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _temp2730->pat_vars=
0; _temp2730->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2730->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2730->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2730;});
_temp2729->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2729;}); _temp2728;});
_temp2727;}); break; case 255: _LL2726: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2732=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2732[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2733; _temp2733.tag= Cyc_SwitchCClauseList_tok; _temp2733.f1= 0; _temp2733;});
_temp2732;}); break; case 256: _LL2731: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2735=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2735[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2736; _temp2736.tag= Cyc_SwitchCClauseList_tok; _temp2736.f1=({ struct Cyc_List_List*
_temp2737=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2737->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2738=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2738->cnst_exp= 0;
_temp2738->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2738->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2738;});
_temp2737->tl= 0; _temp2737;}); _temp2736;}); _temp2735;}); break; case 257:
_LL2734: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2740=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2740[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2741; _temp2741.tag=
Cyc_SwitchCClauseList_tok; _temp2741.f1=({ struct Cyc_List_List* _temp2742=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2742->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2743=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2743->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2743->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2743->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2743;});
_temp2742->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2742;}); _temp2741;});
_temp2740;}); break; case 258: _LL2739: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2745=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2745[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2746; _temp2746.tag= Cyc_SwitchCClauseList_tok; _temp2746.f1=({ struct Cyc_List_List*
_temp2747=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2747->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2748=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2748->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _temp2748->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2748->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2748;});
_temp2747->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2747;}); _temp2746;});
_temp2745;}); break; case 259: _LL2744: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2750=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2750[ 0]=({ struct Cyc_Stmt_tok_struct _temp2751; _temp2751.tag= Cyc_Stmt_tok;
_temp2751.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2751;}); _temp2750;}); break; case 260: _LL2749: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2753=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2753[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2754; _temp2754.tag= Cyc_Stmt_tok; _temp2754.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2754;}); _temp2753;}); break; case 261: _LL2752: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2756=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2756[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2757; _temp2757.tag= Cyc_Stmt_tok; _temp2757.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2757;}); _temp2756;}); break; case 262: _LL2755: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2759=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2759[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2760; _temp2760.tag= Cyc_Stmt_tok; _temp2760.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2760;}); _temp2759;}); break; case 263: _LL2758: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2762=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2762[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2763; _temp2763.tag= Cyc_Stmt_tok; _temp2763.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2763;}); _temp2762;}); break; case 264: _LL2761: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2765=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2765[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2766; _temp2766.tag= Cyc_Stmt_tok; _temp2766.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2766;}); _temp2765;}); break; case 265: _LL2764: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2768=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2768[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2769; _temp2769.tag= Cyc_Stmt_tok; _temp2769.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2769;}); _temp2768;}); break; case 266: _LL2767: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2771=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2771[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2772; _temp2772.tag= Cyc_Stmt_tok; _temp2772.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2772;});
_temp2771;}); break; case 267: _LL2770: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2774=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2774[ 0]=({ struct Cyc_Stmt_tok_struct _temp2775; _temp2775.tag= Cyc_Stmt_tok;
_temp2775.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2775;});
_temp2774;}); break; case 268: _LL2773: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2777=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2777[ 0]=({ struct Cyc_Stmt_tok_struct _temp2778; _temp2778.tag= Cyc_Stmt_tok;
_temp2778.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2778;}); _temp2777;}); break; case 269: _LL2776: { struct Cyc_List_List*
_temp2780= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); struct Cyc_Absyn_Stmt* _temp2781= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2782=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2782[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2783; _temp2783.tag= Cyc_Stmt_tok; _temp2783.f1= Cyc_Parse_flatten_declarations(
_temp2780, _temp2781); _temp2783;}); _temp2782;}); break;} case 270: _LL2779: {
struct Cyc_List_List* _temp2785= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2786= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2787=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2787[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2788; _temp2788.tag= Cyc_Stmt_tok; _temp2788.f1= Cyc_Parse_flatten_declarations(
_temp2785, _temp2786); _temp2788;}); _temp2787;}); break;} case 271: _LL2784: {
struct Cyc_List_List* _temp2790= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2791= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2792=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2792[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2793; _temp2793.tag= Cyc_Stmt_tok; _temp2793.f1= Cyc_Parse_flatten_declarations(
_temp2790, _temp2791); _temp2793;}); _temp2792;}); break;} case 272: _LL2789: {
struct Cyc_List_List* _temp2795= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); struct Cyc_Absyn_Stmt*
_temp2796= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2797=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2797[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2798; _temp2798.tag= Cyc_Stmt_tok; _temp2798.f1= Cyc_Parse_flatten_declarations(
_temp2795, _temp2796); _temp2798;}); _temp2797;}); break;} case 273: _LL2794:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2800=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2800[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2801; _temp2801.tag= Cyc_Stmt_tok; _temp2801.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2802=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2802[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2802;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2801;}); _temp2800;}); break; case 274: _LL2799: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2804=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2804[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2805; _temp2805.tag= Cyc_Stmt_tok; _temp2805.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2805;}); _temp2804;}); break; case 275:
_LL2803: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2807=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2807[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2808; _temp2808.tag= Cyc_Stmt_tok; _temp2808.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2808;}); _temp2807;}); break; case 276: _LL2806: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2810=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2810[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2811; _temp2811.tag= Cyc_Stmt_tok; _temp2811.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2811;}); _temp2810;}); break; case 277:
_LL2809: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2813=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2813[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2814; _temp2814.tag= Cyc_Stmt_tok; _temp2814.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2814;}); _temp2813;}); break; case 278: _LL2812: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2816=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2816[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2817; _temp2817.tag= Cyc_Stmt_tok; _temp2817.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2817;}); _temp2816;}); break; case 279:
_LL2815: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2819=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2819[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2820; _temp2820.tag= Cyc_Stmt_tok; _temp2820.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); _temp2820;}); _temp2819;}); break; case 280:
_LL2818: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2822=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2822[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2823; _temp2823.tag= Cyc_Stmt_tok; _temp2823.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));
_temp2823;}); _temp2822;}); break; case 281: _LL2821: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2825=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2825[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2826; _temp2826.tag= Cyc_Pattern_tok; _temp2826.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2826;});
_temp2825;}); break; case 282: _LL2824: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 283:
_LL2827: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2829=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2829[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2830; _temp2830.tag= Cyc_Pattern_tok; _temp2830.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2831=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2831[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2832; _temp2832.tag= Cyc_Absyn_Int_p; _temp2832.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2832.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2832;}); _temp2831;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2830;});
_temp2829;}); break; case 284: _LL2828: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2834=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2834[ 0]=({ struct Cyc_Pattern_tok_struct _temp2835; _temp2835.tag= Cyc_Pattern_tok;
_temp2835.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2836=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2836[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2837; _temp2837.tag= Cyc_Absyn_Int_p;
_temp2837.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2837.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2837;});
_temp2836;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2835;}); _temp2834;}); break; case 285:
_LL2833: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2839=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2839[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2840; _temp2840.tag= Cyc_Pattern_tok; _temp2840.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2841=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2841[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2842; _temp2842.tag= Cyc_Absyn_Float_p; _temp2842.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2842;}); _temp2841;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2840;});
_temp2839;}); break; case 286: _LL2838: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2844=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2844[ 0]=({ struct Cyc_Pattern_tok_struct _temp2845; _temp2845.tag= Cyc_Pattern_tok;
_temp2845.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2846=( struct Cyc_Absyn_Char_p_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2846[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2847; _temp2847.tag= Cyc_Absyn_Char_p; _temp2847.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2847;});
_temp2846;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2845;}); _temp2844;}); break; case 287:
_LL2843: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2849=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2849[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2850; _temp2850.tag= Cyc_Pattern_tok; _temp2850.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2850;});
_temp2849;}); break; case 288: _LL2848: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2852=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2852[ 0]=({ struct Cyc_Pattern_tok_struct _temp2853; _temp2853.tag= Cyc_Pattern_tok;
_temp2853.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2854=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2854[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2855; _temp2855.tag=
Cyc_Absyn_UnknownId_p; _temp2855.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2855;}); _temp2854;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2853;}); _temp2852;}); break; case 289:
_LL2851: { struct Cyc_List_List* _temp2857=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2858=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2858[ 0]=({ struct Cyc_Pattern_tok_struct _temp2859; _temp2859.tag= Cyc_Pattern_tok;
_temp2859.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2860=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2860[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2861; _temp2861.tag= Cyc_Absyn_UnknownCall_p; _temp2861.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2861.f2=
_temp2857; _temp2861.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2861;});
_temp2860;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2859;}); _temp2858;}); break;} case
290: _LL2856: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2863=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2863[ 0]=({ struct Cyc_Pattern_tok_struct _temp2864; _temp2864.tag= Cyc_Pattern_tok;
_temp2864.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2865=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2865[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2866; _temp2866.tag= Cyc_Absyn_Tuple_p;
_temp2866.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2866;}); _temp2865;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2864;}); _temp2863;}); break; case 291: _LL2862: { struct Cyc_List_List*
_temp2868=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp2869=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2869[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2870; _temp2870.tag= Cyc_Pattern_tok; _temp2870.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2871=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2871[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2872; _temp2872.tag= Cyc_Absyn_UnknownFields_p;
_temp2872.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp2872.f2= _temp2868; _temp2872.f3= 0; _temp2872;});
_temp2871;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2870;}); _temp2869;}); break;} case
292: _LL2867: { struct Cyc_List_List* _temp2874=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2875=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2875[ 0]=({ struct Cyc_Pattern_tok_struct _temp2876; _temp2876.tag= Cyc_Pattern_tok;
_temp2876.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2877=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2877[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2878; _temp2878.tag= Cyc_Absyn_UnknownFields_p; _temp2878.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2878.f2=
_temp2874; _temp2878.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2878;});
_temp2877;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2876;}); _temp2875;}); break;} case
293: _LL2873: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2880=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2880[ 0]=({ struct Cyc_Pattern_tok_struct _temp2881; _temp2881.tag= Cyc_Pattern_tok;
_temp2881.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2882=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2882[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2883; _temp2883.tag= Cyc_Absyn_Pointer_p;
_temp2883.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2883;}); _temp2882;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2881;});
_temp2880;}); break; case 294: _LL2879: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2885=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2885[ 0]=({ struct Cyc_Pattern_tok_struct _temp2886; _temp2886.tag= Cyc_Pattern_tok;
_temp2886.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2887=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2887[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2888; _temp2888.tag=
Cyc_Absyn_Reference_p; _temp2888.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2889=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2889->f1=(
void*) Cyc_Absyn_Loc_n; _temp2889->f2=({ struct _tagged_arr* _temp2890=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2890[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2890;});
_temp2889;}),( void*) Cyc_Absyn_VoidType, 0); _temp2888;}); _temp2887;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2886;}); _temp2885;}); break; case 295: _LL2884: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2892=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2892[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2893; _temp2893.tag= Cyc_PatternList_tok;
_temp2893.f1= 0; _temp2893;}); _temp2892;}); break; case 296: _LL2891: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2895=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2895[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2896; _temp2896.tag= Cyc_PatternList_tok;
_temp2896.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2896;}); _temp2895;}); break; case 297: _LL2894: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2898=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2898[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2899; _temp2899.tag= Cyc_PatternList_tok;
_temp2899.f1=({ struct Cyc_List_List* _temp2900=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2900->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2900->tl= 0;
_temp2900;}); _temp2899;}); _temp2898;}); break; case 298: _LL2897: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2902=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2902[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2903; _temp2903.tag= Cyc_PatternList_tok;
_temp2903.f1=({ struct Cyc_List_List* _temp2904=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2904->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2904->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2904;}); _temp2903;}); _temp2902;}); break; case 299:
_LL2901: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2906=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2906[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2907; _temp2907.tag=
Cyc_FieldPattern_tok; _temp2907.f1=({ struct _tuple14* _temp2908=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2908->f1= 0; _temp2908->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2908;}); _temp2907;}); _temp2906;}); break; case 300:
_LL2905: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2910=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2910[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2911; _temp2911.tag=
Cyc_FieldPattern_tok; _temp2911.f1=({ struct _tuple14* _temp2912=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2912->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2912->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2912;}); _temp2911;}); _temp2910;}); break; case 301:
_LL2909: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2914=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2914[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2915; _temp2915.tag=
Cyc_FieldPatternList_tok; _temp2915.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2915;});
_temp2914;}); break; case 302: _LL2913: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2917=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2917[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2918; _temp2918.tag= Cyc_FieldPatternList_tok; _temp2918.f1=({ struct Cyc_List_List*
_temp2919=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2919->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2919->tl= 0;
_temp2919;}); _temp2918;}); _temp2917;}); break; case 303: _LL2916: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2921=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2921[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2922; _temp2922.tag= Cyc_FieldPatternList_tok;
_temp2922.f1=({ struct Cyc_List_List* _temp2923=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2923->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2923->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2923;}); _temp2922;}); _temp2921;}); break; case 304:
_LL2920: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 305: _LL2924: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2926=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2926[ 0]=({ struct Cyc_Exp_tok_struct _temp2927; _temp2927.tag= Cyc_Exp_tok;
_temp2927.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2927;}); _temp2926;}); break; case 306: _LL2925: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 307: _LL2928:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2930=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2930[ 0]=({ struct Cyc_Exp_tok_struct
_temp2931; _temp2931.tag= Cyc_Exp_tok; _temp2931.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2931;}); _temp2930;}); break; case 308: _LL2929: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2933=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2933[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2934; _temp2934.tag= Cyc_Primopopt_tok; _temp2934.f1=
0; _temp2934;}); _temp2933;}); break; case 309: _LL2932: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2936=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2936[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2937; _temp2937.tag= Cyc_Primopopt_tok; _temp2937.f1=({
struct Cyc_Core_Opt* _temp2938=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2938->v=( void*)(( void*) Cyc_Absyn_Times); _temp2938;});
_temp2937;}); _temp2936;}); break; case 310: _LL2935: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2940=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2940[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2941; _temp2941.tag= Cyc_Primopopt_tok; _temp2941.f1=({
struct Cyc_Core_Opt* _temp2942=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2942->v=( void*)(( void*) Cyc_Absyn_Div); _temp2942;});
_temp2941;}); _temp2940;}); break; case 311: _LL2939: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2944=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2944[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2945; _temp2945.tag= Cyc_Primopopt_tok; _temp2945.f1=({
struct Cyc_Core_Opt* _temp2946=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2946->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2946;});
_temp2945;}); _temp2944;}); break; case 312: _LL2943: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2948=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2948[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2949; _temp2949.tag= Cyc_Primopopt_tok; _temp2949.f1=({
struct Cyc_Core_Opt* _temp2950=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2950->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2950;});
_temp2949;}); _temp2948;}); break; case 313: _LL2947: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2952=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2952[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2953; _temp2953.tag= Cyc_Primopopt_tok; _temp2953.f1=({
struct Cyc_Core_Opt* _temp2954=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2954->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2954;});
_temp2953;}); _temp2952;}); break; case 314: _LL2951: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2956=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2956[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2957; _temp2957.tag= Cyc_Primopopt_tok; _temp2957.f1=({
struct Cyc_Core_Opt* _temp2958=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2958->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2958;});
_temp2957;}); _temp2956;}); break; case 315: _LL2955: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2960=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2960[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2961; _temp2961.tag= Cyc_Primopopt_tok; _temp2961.f1=({
struct Cyc_Core_Opt* _temp2962=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2962->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2962;});
_temp2961;}); _temp2960;}); break; case 316: _LL2959: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2964=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2964[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2965; _temp2965.tag= Cyc_Primopopt_tok; _temp2965.f1=({
struct Cyc_Core_Opt* _temp2966=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2966->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2966;});
_temp2965;}); _temp2964;}); break; case 317: _LL2963: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2968=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2968[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2969; _temp2969.tag= Cyc_Primopopt_tok; _temp2969.f1=({
struct Cyc_Core_Opt* _temp2970=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2970->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2970;});
_temp2969;}); _temp2968;}); break; case 318: _LL2967: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2972=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2972[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2973; _temp2973.tag= Cyc_Primopopt_tok; _temp2973.f1=({
struct Cyc_Core_Opt* _temp2974=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2974->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2974;});
_temp2973;}); _temp2972;}); break; case 319: _LL2971: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 320: _LL2975:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2977=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2977[ 0]=({ struct Cyc_Exp_tok_struct
_temp2978; _temp2978.tag= Cyc_Exp_tok; _temp2978.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2978;});
_temp2977;}); break; case 321: _LL2976: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2980=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2980[ 0]=({ struct Cyc_Exp_tok_struct _temp2981; _temp2981.tag= Cyc_Exp_tok;
_temp2981.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2981;}); _temp2980;}); break; case 322: _LL2979: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2983=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2983[ 0]=({ struct Cyc_Exp_tok_struct
_temp2984; _temp2984.tag= Cyc_Exp_tok; _temp2984.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp2984;}); _temp2983;}); break; case 323: _LL2982: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2986=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2986[ 0]=({ struct Cyc_Exp_tok_struct
_temp2987; _temp2987.tag= Cyc_Exp_tok; _temp2987.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp2987;}); _temp2986;}); break; case 324: _LL2985: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2989=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2989[ 0]=({ struct Cyc_Exp_tok_struct
_temp2990; _temp2990.tag= Cyc_Exp_tok; _temp2990.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2990;});
_temp2989;}); break; case 325: _LL2988: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2992=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2992[ 0]=({ struct Cyc_Exp_tok_struct _temp2993; _temp2993.tag= Cyc_Exp_tok;
_temp2993.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2993;}); _temp2992;}); break; case 326: _LL2991: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 327: _LL2994:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
328: _LL2995: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2997=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2997[ 0]=({ struct Cyc_Exp_tok_struct
_temp2998; _temp2998.tag= Cyc_Exp_tok; _temp2998.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2998;}); _temp2997;}); break; case 329: _LL2996: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 330: _LL2999:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3001=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3001[ 0]=({ struct Cyc_Exp_tok_struct
_temp3002; _temp3002.tag= Cyc_Exp_tok; _temp3002.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3002;}); _temp3001;}); break; case 331: _LL3000: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 332: _LL3003:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3005=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3005[ 0]=({ struct Cyc_Exp_tok_struct
_temp3006; _temp3006.tag= Cyc_Exp_tok; _temp3006.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3006;});
_temp3005;}); break; case 333: _LL3004: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 334: _LL3007:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3009=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3009[ 0]=({ struct Cyc_Exp_tok_struct
_temp3010; _temp3010.tag= Cyc_Exp_tok; _temp3010.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3010;});
_temp3009;}); break; case 335: _LL3008: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 336: _LL3011:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3013=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3013[ 0]=({ struct Cyc_Exp_tok_struct
_temp3014; _temp3014.tag= Cyc_Exp_tok; _temp3014.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3014;});
_temp3013;}); break; case 337: _LL3012: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 338: _LL3015:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3017=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3017[ 0]=({ struct Cyc_Exp_tok_struct
_temp3018; _temp3018.tag= Cyc_Exp_tok; _temp3018.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3018;}); _temp3017;}); break; case 339: _LL3016: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3020=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3020[ 0]=({ struct Cyc_Exp_tok_struct
_temp3021; _temp3021.tag= Cyc_Exp_tok; _temp3021.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3021;}); _temp3020;}); break; case 340: _LL3019: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 341: _LL3022:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3024=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3024[ 0]=({ struct Cyc_Exp_tok_struct
_temp3025; _temp3025.tag= Cyc_Exp_tok; _temp3025.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3025;}); _temp3024;}); break; case 342: _LL3023: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3027=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3027[ 0]=({ struct Cyc_Exp_tok_struct
_temp3028; _temp3028.tag= Cyc_Exp_tok; _temp3028.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3028;}); _temp3027;}); break; case 343: _LL3026: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3030=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3030[ 0]=({ struct Cyc_Exp_tok_struct
_temp3031; _temp3031.tag= Cyc_Exp_tok; _temp3031.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3031;}); _temp3030;}); break; case 344: _LL3029: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3033=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3033[ 0]=({ struct Cyc_Exp_tok_struct
_temp3034; _temp3034.tag= Cyc_Exp_tok; _temp3034.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3034;}); _temp3033;}); break; case 345: _LL3032: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 346: _LL3035:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3037=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3037[ 0]=({ struct Cyc_Exp_tok_struct
_temp3038; _temp3038.tag= Cyc_Exp_tok; _temp3038.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3038;}); _temp3037;}); break; case 347: _LL3036: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3040=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3040[ 0]=({ struct Cyc_Exp_tok_struct
_temp3041; _temp3041.tag= Cyc_Exp_tok; _temp3041.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3041;}); _temp3040;}); break; case 348: _LL3039: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 349: _LL3042:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3044=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3044[ 0]=({ struct Cyc_Exp_tok_struct
_temp3045; _temp3045.tag= Cyc_Exp_tok; _temp3045.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3045;});
_temp3044;}); break; case 350: _LL3043: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3047=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3047[ 0]=({ struct Cyc_Exp_tok_struct _temp3048; _temp3048.tag= Cyc_Exp_tok;
_temp3048.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3048;}); _temp3047;}); break; case 351: _LL3046: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 352: _LL3049:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3051=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3051[ 0]=({ struct Cyc_Exp_tok_struct
_temp3052; _temp3052.tag= Cyc_Exp_tok; _temp3052.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3052;});
_temp3051;}); break; case 353: _LL3050: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3054=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3054[ 0]=({ struct Cyc_Exp_tok_struct _temp3055; _temp3055.tag= Cyc_Exp_tok;
_temp3055.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3055;}); _temp3054;}); break; case 354: _LL3053: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3057=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3057[ 0]=({ struct Cyc_Exp_tok_struct
_temp3058; _temp3058.tag= Cyc_Exp_tok; _temp3058.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3058;});
_temp3057;}); break; case 355: _LL3056: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 356: _LL3059:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3061=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3061[ 0]=({ struct Cyc_Exp_tok_struct
_temp3062; _temp3062.tag= Cyc_Exp_tok; _temp3062.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3062;}); _temp3061;}); break; case 357: _LL3060: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 358: _LL3063:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3065=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3065[ 0]=({ struct Cyc_Exp_tok_struct
_temp3066; _temp3066.tag= Cyc_Exp_tok; _temp3066.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3066;}); _temp3065;}); break; case 359: _LL3064: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3068=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3068[ 0]=({ struct Cyc_Exp_tok_struct
_temp3069; _temp3069.tag= Cyc_Exp_tok; _temp3069.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3069;}); _temp3068;}); break; case 360: _LL3067: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3071=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3071[ 0]=({ struct Cyc_Exp_tok_struct
_temp3072; _temp3072.tag= Cyc_Exp_tok; _temp3072.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3072;}); _temp3071;}); break; case 361: _LL3070: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3074=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3074[ 0]=({ struct Cyc_Exp_tok_struct
_temp3075; _temp3075.tag= Cyc_Exp_tok; _temp3075.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3075;}); _temp3074;}); break; case 362: _LL3073: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 363: _LL3076:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3078=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3078[ 0]=({ struct Cyc_Exp_tok_struct
_temp3079; _temp3079.tag= Cyc_Exp_tok; _temp3079.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3079;}); _temp3078;}); break; case 364: _LL3077: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3081=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3081[ 0]=({ struct Cyc_Exp_tok_struct
_temp3082; _temp3082.tag= Cyc_Exp_tok; _temp3082.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3082;});
_temp3081;}); break; case 365: _LL3080: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3084=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3084[ 0]=({ struct Cyc_Exp_tok_struct _temp3085; _temp3085.tag= Cyc_Exp_tok;
_temp3085.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3085;}); _temp3084;}); break; case 366: _LL3083: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3087=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3087[ 0]=({ struct Cyc_Exp_tok_struct
_temp3088; _temp3088.tag= Cyc_Exp_tok; _temp3088.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])).f3,({ struct _tagged_arr* _temp3089=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3089[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3089;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3088;}); _temp3087;}); break; case 367:
_LL3086: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3091=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3091[ 0]=({ struct Cyc_Exp_tok_struct
_temp3092; _temp3092.tag= Cyc_Exp_tok; _temp3092.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3093=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3093[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3094; _temp3094.tag= Cyc_Absyn_Malloc_e;
_temp3094.f1= 0; _temp3094.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3094;}); _temp3093;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3092;});
_temp3091;}); break; case 368: _LL3090: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3096=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3096[ 0]=({ struct Cyc_Exp_tok_struct _temp3097; _temp3097.tag= Cyc_Exp_tok;
_temp3097.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3098=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3098[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3099; _temp3099.tag= Cyc_Absyn_Malloc_e;
_temp3099.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]); _temp3099.f2=( void*)
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3099;}); _temp3098;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3097;});
_temp3096;}); break; case 369: _LL3095: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3101=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3101[ 0]=({ struct Cyc_Primop_tok_struct _temp3102; _temp3102.tag= Cyc_Primop_tok;
_temp3102.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3102;}); _temp3101;});
break; case 370: _LL3100: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3104=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3104[ 0]=({ struct Cyc_Primop_tok_struct _temp3105; _temp3105.tag= Cyc_Primop_tok;
_temp3105.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3105;}); _temp3104;});
break; case 371: _LL3103: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3107=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3107[ 0]=({ struct Cyc_Primop_tok_struct _temp3108; _temp3108.tag= Cyc_Primop_tok;
_temp3108.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3108;}); _temp3107;});
break; case 372: _LL3106: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 373: _LL3109: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3111=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3111[ 0]=({ struct Cyc_Exp_tok_struct _temp3112; _temp3112.tag= Cyc_Exp_tok;
_temp3112.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3112;}); _temp3111;}); break; case 374: _LL3110: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3114=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3114[ 0]=({ struct Cyc_Exp_tok_struct
_temp3115; _temp3115.tag= Cyc_Exp_tok; _temp3115.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3115;}); _temp3114;}); break; case 375:
_LL3113: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3117=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3117[ 0]=({ struct Cyc_Exp_tok_struct
_temp3118; _temp3118.tag= Cyc_Exp_tok; _temp3118.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3118;});
_temp3117;}); break; case 376: _LL3116: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3120=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3120[ 0]=({ struct Cyc_Exp_tok_struct _temp3121; _temp3121.tag= Cyc_Exp_tok;
_temp3121.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3122=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3122[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3122;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3121;}); _temp3120;}); break; case 377: _LL3119: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3124=(*
q).f1; struct Cyc_List_List* _temp3134; struct Cyc_List_List* _temp3136; _LL3126:
if( _temp3124 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3127;} else{ goto _LL3128;}
_LL3128: if(( unsigned int) _temp3124 > 1u?*(( int*) _temp3124) == Cyc_Absyn_Rel_n:
0){ _LL3135: _temp3134=(( struct Cyc_Absyn_Rel_n_struct*) _temp3124)->f1; if(
_temp3134 == 0){ goto _LL3129;} else{ goto _LL3130;}} else{ goto _LL3130;}
_LL3130: if(( unsigned int) _temp3124 > 1u?*(( int*) _temp3124) == Cyc_Absyn_Abs_n:
0){ _LL3137: _temp3136=(( struct Cyc_Absyn_Abs_n_struct*) _temp3124)->f1; if(
_temp3136 == 0){ goto _LL3131;} else{ goto _LL3132;}} else{ goto _LL3132;}
_LL3132: goto _LL3133; _LL3127: goto _LL3125; _LL3129: goto _LL3125; _LL3131:
goto _LL3125; _LL3133: Cyc_Parse_err( _tag_arr("struct field name is qualified",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3125; _LL3125:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3138=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3138[ 0]=({ struct Cyc_Exp_tok_struct _temp3139; _temp3139.tag= Cyc_Exp_tok;
_temp3139.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3139;}); _temp3138;}); break;} case 378: _LL3123: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3141=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3141[ 0]=({ struct Cyc_Exp_tok_struct
_temp3142; _temp3142.tag= Cyc_Exp_tok; _temp3142.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),({ struct _tagged_arr* _temp3143=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp3143[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3143;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3142;}); _temp3141;}); break; case 379: _LL3140: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);{ void* _temp3145=(*
q).f1; struct Cyc_List_List* _temp3155; struct Cyc_List_List* _temp3157; _LL3147:
if( _temp3145 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3148;} else{ goto _LL3149;}
_LL3149: if(( unsigned int) _temp3145 > 1u?*(( int*) _temp3145) == Cyc_Absyn_Rel_n:
0){ _LL3156: _temp3155=(( struct Cyc_Absyn_Rel_n_struct*) _temp3145)->f1; if(
_temp3155 == 0){ goto _LL3150;} else{ goto _LL3151;}} else{ goto _LL3151;}
_LL3151: if(( unsigned int) _temp3145 > 1u?*(( int*) _temp3145) == Cyc_Absyn_Abs_n:
0){ _LL3158: _temp3157=(( struct Cyc_Absyn_Abs_n_struct*) _temp3145)->f1; if(
_temp3157 == 0){ goto _LL3152;} else{ goto _LL3153;}} else{ goto _LL3153;}
_LL3153: goto _LL3154; _LL3148: goto _LL3146; _LL3150: goto _LL3146; _LL3152:
goto _LL3146; _LL3154: Cyc_Parse_err( _tag_arr("struct field is qualified with module name",
sizeof( unsigned char), 43u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); goto
_LL3146; _LL3146:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3159=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3159[ 0]=({ struct Cyc_Exp_tok_struct _temp3160; _temp3160.tag= Cyc_Exp_tok;
_temp3160.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),(* q).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3160;}); _temp3159;}); break;} case 380: _LL3144: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3162=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3162[ 0]=({ struct Cyc_Exp_tok_struct
_temp3163; _temp3163.tag= Cyc_Exp_tok; _temp3163.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3163;}); _temp3162;}); break; case 381: _LL3161: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3165=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3165[ 0]=({ struct Cyc_Exp_tok_struct
_temp3166; _temp3166.tag= Cyc_Exp_tok; _temp3166.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3166;}); _temp3165;}); break; case 382: _LL3164: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3168=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3168[ 0]=({ struct Cyc_Exp_tok_struct
_temp3169; _temp3169.tag= Cyc_Exp_tok; _temp3169.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3170=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3170[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3171; _temp3171.tag= Cyc_Absyn_CompoundLit_e;
_temp3171.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp3171.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3171;});
_temp3170;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3169;}); _temp3168;}); break; case 383:
_LL3167: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3173=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3173[ 0]=({ struct Cyc_Exp_tok_struct
_temp3174; _temp3174.tag= Cyc_Exp_tok; _temp3174.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3175=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3175[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3176; _temp3176.tag= Cyc_Absyn_CompoundLit_e;
_temp3176.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp3176.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp3176;});
_temp3175;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3174;}); _temp3173;}); break; case 384:
_LL3172: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3178=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3178[ 0]=({ struct Cyc_Exp_tok_struct
_temp3179; _temp3179.tag= Cyc_Exp_tok; _temp3179.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3180=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3180[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3181; _temp3181.tag= Cyc_Absyn_Fill_e; _temp3181.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3181;});
_temp3180;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3179;}); _temp3178;}); break; case 385:
_LL3177: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3183=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3183[ 0]=({ struct Cyc_Exp_tok_struct
_temp3184; _temp3184.tag= Cyc_Exp_tok; _temp3184.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3185=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3185[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3186; _temp3186.tag= Cyc_Absyn_Codegen_e;
_temp3186.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp3186;}); _temp3185;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3184;}); _temp3183;}); break; case 386: _LL3182: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3188=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3188[ 0]=({ struct Cyc_Exp_tok_struct
_temp3189; _temp3189.tag= Cyc_Exp_tok; _temp3189.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3190=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3190[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3191; _temp3191.tag= Cyc_Absyn_UnknownId_e;
_temp3191.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3191;}); _temp3190;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3189;});
_temp3188;}); break; case 387: _LL3187: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 388: _LL3192:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3194=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3194[ 0]=({ struct Cyc_Exp_tok_struct
_temp3195; _temp3195.tag= Cyc_Exp_tok; _temp3195.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3195;});
_temp3194;}); break; case 389: _LL3193: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 390:
_LL3196: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3198=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3198[ 0]=({ struct Cyc_Exp_tok_struct
_temp3199; _temp3199.tag= Cyc_Exp_tok; _temp3199.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3200=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3200[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3201; _temp3201.tag=
Cyc_Absyn_UnknownId_e; _temp3201.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3201;});
_temp3200;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3199;});
_temp3198;}); break; case 391: _LL3197: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3203=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3203[ 0]=({ struct Cyc_Exp_tok_struct _temp3204; _temp3204.tag= Cyc_Exp_tok;
_temp3204.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3205=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3205[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3206; _temp3206.tag=
Cyc_Absyn_UnknownId_e; _temp3206.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp3206;});
_temp3205;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3204;}); _temp3203;}); break; case 392: _LL3202: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3208=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3208[ 0]=({ struct Cyc_Exp_tok_struct
_temp3209; _temp3209.tag= Cyc_Exp_tok; _temp3209.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3209;}); _temp3208;}); break; case 393: _LL3207: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3211=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3211[ 0]=({ struct Cyc_Exp_tok_struct
_temp3212; _temp3212.tag= Cyc_Exp_tok; _temp3212.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3213=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3213[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3214; _temp3214.tag= Cyc_Absyn_Struct_e;
_temp3214.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp3214.f2= 0; _temp3214.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3214.f4= 0;
_temp3214;}); _temp3213;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3212;});
_temp3211;}); break; case 394: _LL3210: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3216=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3216[ 0]=({ struct Cyc_Exp_tok_struct _temp3217; _temp3217.tag= Cyc_Exp_tok;
_temp3217.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3217;}); _temp3216;}); break; case 395: _LL3215: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3219=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3219[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3220; _temp3220.tag= Cyc_ExpList_tok; _temp3220.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3220;});
_temp3219;}); break; case 396: _LL3218: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3222=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3222[ 0]=({ struct Cyc_ExpList_tok_struct _temp3223; _temp3223.tag= Cyc_ExpList_tok;
_temp3223.f1=({ struct Cyc_List_List* _temp3224=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3224->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3224->tl= 0;
_temp3224;}); _temp3223;}); _temp3222;}); break; case 397: _LL3221: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3226=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3226[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3227; _temp3227.tag= Cyc_ExpList_tok; _temp3227.f1=({ struct Cyc_List_List*
_temp3228=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3228->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3228->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp3228;});
_temp3227;}); _temp3226;}); break; case 398: _LL3225: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3230=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3230[ 0]=({ struct Cyc_Exp_tok_struct
_temp3231; _temp3231.tag= Cyc_Exp_tok; _temp3231.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3231;});
_temp3230;}); break; case 399: _LL3229: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3233=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3233[ 0]=({ struct Cyc_Exp_tok_struct _temp3234; _temp3234.tag= Cyc_Exp_tok;
_temp3234.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3234;});
_temp3233;}); break; case 400: _LL3232: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3236=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3236[ 0]=({ struct Cyc_Exp_tok_struct _temp3237; _temp3237.tag= Cyc_Exp_tok;
_temp3237.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3237;});
_temp3236;}); break; case 401: _LL3235: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3239=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3239[ 0]=({ struct Cyc_Exp_tok_struct _temp3240; _temp3240.tag= Cyc_Exp_tok;
_temp3240.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3240;});
_temp3239;}); break; case 402: _LL3238: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3242=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3242[ 0]=({ struct Cyc_QualId_tok_struct _temp3243; _temp3243.tag= Cyc_QualId_tok;
_temp3243.f1=({ struct _tuple1* _temp3244=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3244->f1= Cyc_Absyn_rel_ns_null; _temp3244->f2=({ struct
_tagged_arr* _temp3245=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3245[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3245;}); _temp3244;});
_temp3243;}); _temp3242;}); break; case 403: _LL3241: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3246:(
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
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 404u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 111u, yyn - 119)] + yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >= 0?
yystate <= 4425: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4426u, yystate)]
== yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){ yystate=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4426u, yystate)];} else{
yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 111u, yyn - 119)];}
goto yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 795u, yystate)]; if( yyn > - 32768? yyn < 4425:
0){ int sze= 0; struct _tagged_arr msg; int x; int count; count= 0; for( x= yyn
< 0? - yyn: 0; x < 230u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4426u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 230u, x)]) + 15, count ++);}} msg=({
unsigned int _temp3248=( unsigned int)( sze + 15); unsigned char* _temp3249=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3248); struct
_tagged_arr _temp3251= _tag_arr( _temp3249, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3250= _temp3248; unsigned int i;
for( i= 0; i < _temp3250; i ++){ _temp3249[ i]='\000';}}; _temp3251;}); Cyc_String_strcpy(
msg, _tag_arr("parse error", sizeof( unsigned char), 12u)); if( count < 5){
count= 0; for( x= yyn < 0? - yyn: 0; x < 230u / sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4426u, x + yyn)] == x){ Cyc_String_strcat(
msg, count == 0? _tag_arr(", expecting `", sizeof( unsigned char), 14u):
_tag_arr(" or `", sizeof( unsigned char), 6u)); Cyc_String_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 230u, x)]); Cyc_String_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus == 3){ if( Cyc_yychar == 0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset == 0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 795u,
yystate)]; if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1:
yyn > 4425)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4426u, yyn)] != 1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4426u,
yyn)]; if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 794){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3261= v; struct
_tuple16* _temp3279; struct _tuple16 _temp3281; int _temp3282; unsigned char
_temp3284; short _temp3286; struct _tagged_arr _temp3288; struct Cyc_Core_Opt*
_temp3290; struct Cyc_Core_Opt* _temp3292; struct Cyc_Core_Opt _temp3294; struct
_tagged_arr* _temp3295; struct _tuple1* _temp3297; struct _tuple1 _temp3299;
struct _tagged_arr* _temp3300; void* _temp3302; _LL3263: if(*(( void**)
_temp3261) == Cyc_Int_tok){ _LL3280: _temp3279=(( struct Cyc_Int_tok_struct*)
_temp3261)->f1; _temp3281=* _temp3279; _LL3283: _temp3282= _temp3281.f2; goto
_LL3264;} else{ goto _LL3265;} _LL3265: if(*(( void**) _temp3261) == Cyc_Char_tok){
_LL3285: _temp3284=(( struct Cyc_Char_tok_struct*) _temp3261)->f1; goto _LL3266;}
else{ goto _LL3267;} _LL3267: if(*(( void**) _temp3261) == Cyc_Short_tok){
_LL3287: _temp3286=(( struct Cyc_Short_tok_struct*) _temp3261)->f1; goto _LL3268;}
else{ goto _LL3269;} _LL3269: if(*(( void**) _temp3261) == Cyc_String_tok){
_LL3289: _temp3288=(( struct Cyc_String_tok_struct*) _temp3261)->f1; goto
_LL3270;} else{ goto _LL3271;} _LL3271: if(*(( void**) _temp3261) == Cyc_Stringopt_tok){
_LL3291: _temp3290=(( struct Cyc_Stringopt_tok_struct*) _temp3261)->f1; if(
_temp3290 == 0){ goto _LL3272;} else{ goto _LL3273;}} else{ goto _LL3273;}
_LL3273: if(*(( void**) _temp3261) == Cyc_Stringopt_tok){ _LL3293: _temp3292=((
struct Cyc_Stringopt_tok_struct*) _temp3261)->f1; if( _temp3292 == 0){ goto
_LL3275;} else{ _temp3294=* _temp3292; _LL3296: _temp3295=( struct _tagged_arr*)
_temp3294.v; goto _LL3274;}} else{ goto _LL3275;} _LL3275: if(*(( void**)
_temp3261) == Cyc_QualId_tok){ _LL3298: _temp3297=(( struct Cyc_QualId_tok_struct*)
_temp3261)->f1; _temp3299=* _temp3297; _LL3303: _temp3302= _temp3299.f1; goto
_LL3301; _LL3301: _temp3300= _temp3299.f2; goto _LL3276;} else{ goto _LL3277;}
_LL3277: goto _LL3278; _LL3264:({ struct Cyc_Stdio_Int_pa_struct _temp3305;
_temp3305.tag= Cyc_Stdio_Int_pa; _temp3305.f1=( int)(( unsigned int) _temp3282);{
void* _temp3304[ 1u]={& _temp3305}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp3304, sizeof( void*),
1u));}}); goto _LL3262; _LL3266:({ struct Cyc_Stdio_Int_pa_struct _temp3307;
_temp3307.tag= Cyc_Stdio_Int_pa; _temp3307.f1=( int)(( unsigned int)(( int)
_temp3284));{ void* _temp3306[ 1u]={& _temp3307}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%c", sizeof( unsigned char), 3u), _tag_arr( _temp3306, sizeof( void*),
1u));}}); goto _LL3262; _LL3268:({ struct Cyc_Stdio_Int_pa_struct _temp3309;
_temp3309.tag= Cyc_Stdio_Int_pa; _temp3309.f1=( int)(( unsigned int)(( int)
_temp3286));{ void* _temp3308[ 1u]={& _temp3309}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%ds", sizeof( unsigned char), 4u), _tag_arr( _temp3308, sizeof( void*),
1u));}}); goto _LL3262; _LL3270:({ struct Cyc_Stdio_String_pa_struct _temp3311;
_temp3311.tag= Cyc_Stdio_String_pa; _temp3311.f1=( struct _tagged_arr) _temp3288;{
void* _temp3310[ 1u]={& _temp3311}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("\"%s\"", sizeof( unsigned char), 5u), _tag_arr( _temp3310, sizeof(
void*), 1u));}}); goto _LL3262; _LL3272:({ void* _temp3312[ 0u]={}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("null", sizeof( unsigned char), 5u), _tag_arr(
_temp3312, sizeof( void*), 0u));}); goto _LL3262; _LL3274:({ struct Cyc_Stdio_String_pa_struct
_temp3314; _temp3314.tag= Cyc_Stdio_String_pa; _temp3314.f1=( struct _tagged_arr)*
_temp3295;{ void* _temp3313[ 1u]={& _temp3314}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("\"%s\"", sizeof( unsigned char), 5u), _tag_arr( _temp3313, sizeof(
void*), 1u));}}); goto _LL3262; _LL3276: { struct Cyc_List_List* _temp3315= 0;{
void* _temp3316= _temp3302; struct Cyc_List_List* _temp3324; struct Cyc_List_List*
_temp3326; _LL3318: if(( unsigned int) _temp3316 > 1u?*(( int*) _temp3316) ==
Cyc_Absyn_Rel_n: 0){ _LL3325: _temp3324=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3316)->f1; goto _LL3319;} else{ goto _LL3320;} _LL3320: if(( unsigned int)
_temp3316 > 1u?*(( int*) _temp3316) == Cyc_Absyn_Abs_n: 0){ _LL3327: _temp3326=((
struct Cyc_Absyn_Abs_n_struct*) _temp3316)->f1; goto _LL3321;} else{ goto
_LL3322;} _LL3322: if( _temp3316 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3323;}
else{ goto _LL3317;} _LL3319: _temp3315= _temp3324; goto _LL3317; _LL3321:
_temp3315= _temp3326; goto _LL3317; _LL3323: goto _LL3317; _LL3317:;} for( 0;
_temp3315 != 0; _temp3315=(( struct Cyc_List_List*) _check_null( _temp3315))->tl){({
struct Cyc_Stdio_String_pa_struct _temp3329; _temp3329.tag= Cyc_Stdio_String_pa;
_temp3329.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp3315))->hd);{ void* _temp3328[ 1u]={& _temp3329}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("%s::", sizeof( unsigned char), 5u), _tag_arr(
_temp3328, sizeof( void*), 1u));}});}({ struct Cyc_Stdio_String_pa_struct
_temp3331; _temp3331.tag= Cyc_Stdio_String_pa; _temp3331.f1=( struct _tagged_arr)*
_temp3300;{ void* _temp3330[ 1u]={& _temp3331}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s::", sizeof( unsigned char), 5u), _tag_arr( _temp3330, sizeof( void*),
1u));}}); goto _LL3262;} _LL3278:({ void* _temp3332[ 0u]={}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("?", sizeof( unsigned char), 2u), _tag_arr( _temp3332,
sizeof( void*), 0u));}); goto _LL3262; _LL3262:;} struct Cyc_List_List* Cyc_Parse_parse_file(
struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({
struct Cyc_Core_Opt* _temp3333=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3333->v=( void*) Cyc_Lexing_from_file( f); _temp3333;});
Cyc_yyparse(); return Cyc_Parse_parse_result;}
