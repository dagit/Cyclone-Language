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
goto _LL45; _LL45: { struct Cyc_Core_Opt* _temp55= 0; if( _temp53 != 0){ if( Cyc_Absyn_is_qvar_qualified((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp53))->v)){ Cyc_Parse_err(
_tag_arr("parameter cannot be qualified with a namespace", sizeof( unsigned char),
47u), loc);} _temp55=({ struct Cyc_Core_Opt* _temp56=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp56->v=( void*)(*(( struct _tuple1*)((
struct Cyc_Core_Opt*) _check_null( _temp53))->v)).f2; _temp56;});} if( _temp47
!= 0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("parameter should have no type parameters", sizeof( unsigned char), 41u),
loc);} return({ struct _tuple2* _temp57=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp57->f1= _temp55; _temp57->f2= _temp51; _temp57->f3=
_temp49; _temp57;});}} static void* Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment*
loc){ return( void*)({ struct Cyc_Parse_Type_spec_struct* _temp58=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp58[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp59; _temp59.tag= Cyc_Parse_Type_spec; _temp59.f1=(
void*) t; _temp59.f2= loc; _temp59;}); _temp58;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp60= t; struct Cyc_Absyn_Exp* _temp66; struct Cyc_Absyn_Tqual
_temp68; void* _temp70; _LL62: if(( unsigned int) _temp60 > 4u?*(( int*) _temp60)
== Cyc_Absyn_ArrayType: 0){ _LL71: _temp70=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp60)->f1; goto _LL69; _LL69: _temp68=(( struct Cyc_Absyn_ArrayType_struct*)
_temp60)->f2; goto _LL67; _LL67: _temp66=(( struct Cyc_Absyn_ArrayType_struct*)
_temp60)->f3; goto _LL63;} else{ goto _LL64;} _LL64: goto _LL65; _LL63: return
Cyc_Absyn_starb_typ( _temp70,( void*) Cyc_Absyn_HeapRgn, _temp68, _temp66 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp72=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp72[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp73; _temp73.tag= Cyc_Absyn_Upper_b;
_temp73.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp66); _temp73;}); _temp72;}));
_LL65: return t; _LL61:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple11* _temp74=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp74->f1=(* t).f2; _temp74->f2=(* t).f3;
_temp74;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_arr decl_kind;{ void* _temp75=( void*)
x->r; struct Cyc_Absyn_Vardecl* _temp101; _LL77: if(*(( int*) _temp75) == Cyc_Absyn_Var_d){
_LL102: _temp101=(( struct Cyc_Absyn_Var_d_struct*) _temp75)->f1; goto _LL78;}
else{ goto _LL79;} _LL79: if(*(( int*) _temp75) == Cyc_Absyn_Let_d){ goto _LL80;}
else{ goto _LL81;} _LL81: if(*(( int*) _temp75) == Cyc_Absyn_Letv_d){ goto _LL82;}
else{ goto _LL83;} _LL83: if(*(( int*) _temp75) == Cyc_Absyn_Fn_d){ goto _LL84;}
else{ goto _LL85;} _LL85: if(*(( int*) _temp75) == Cyc_Absyn_Struct_d){ goto
_LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp75) == Cyc_Absyn_Union_d){
goto _LL88;} else{ goto _LL89;} _LL89: if(*(( int*) _temp75) == Cyc_Absyn_Tunion_d){
goto _LL90;} else{ goto _LL91;} _LL91: if(*(( int*) _temp75) == Cyc_Absyn_Typedef_d){
goto _LL92;} else{ goto _LL93;} _LL93: if(*(( int*) _temp75) == Cyc_Absyn_Enum_d){
goto _LL94;} else{ goto _LL95;} _LL95: if(*(( int*) _temp75) == Cyc_Absyn_Namespace_d){
goto _LL96;} else{ goto _LL97;} _LL97: if(*(( int*) _temp75) == Cyc_Absyn_Using_d){
goto _LL98;} else{ goto _LL99;} _LL99: if(*(( int*) _temp75) == Cyc_Absyn_ExternC_d){
goto _LL100;} else{ goto _LL76;} _LL78: if( _temp101->initializer != 0){(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("initializers are not allowed in parameter declarations", sizeof(
unsigned char), 55u), x->loc);} if( Cyc_Absyn_is_qvar_qualified( _temp101->name)){
Cyc_Parse_err( _tag_arr("namespaces not allowed on parameter declarations",
sizeof( unsigned char), 49u), x->loc);}{ int found= 0; for( 0; params != 0;
params=(( struct Cyc_List_List*) _check_null( params))->tl){ if( Cyc_String_zstrptrcmp((*
_temp101->name).f2,( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
params))->hd) == 0){ found= 1; break;}} if( ! found){(( int(*)( struct
_tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp104; _temp104.tag= Cyc_Stdio_String_pa;
_temp104.f1=( struct _tagged_arr)*(* _temp101->name).f2;{ void* _temp103[ 1u]={&
_temp104}; Cyc_Stdio_aprintf( _tag_arr("%s is not listed as a parameter",
sizeof( unsigned char), 32u), _tag_arr( _temp103, sizeof( void*), 1u));}}), x->loc);}
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
struct Cyc_Stdio_String_pa_struct _temp106; _temp106.tag= Cyc_Stdio_String_pa;
_temp106.f1=( struct _tagged_arr) decl_kind;{ void* _temp105[ 1u]={& _temp106};
Cyc_Stdio_aprintf( _tag_arr("%s appears in parameter type", sizeof(
unsigned char), 29u), _tag_arr( _temp105, sizeof( void*), 1u));}}), x->loc);
return;} struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment*
f2; } ; static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env,
struct _tagged_arr* x){ struct _tuple12 _temp109; struct Cyc_Position_Segment*
_temp110; struct Cyc_List_List* _temp112; struct _tuple12* _temp107= env;
_temp109=* _temp107; _LL113: _temp112= _temp109.f1; goto _LL111; _LL111:
_temp110= _temp109.f2; goto _LL108; _LL108: if( _temp112 == 0){ return(( struct
_tuple2*(*)( struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp115; _temp115.tag=
Cyc_Stdio_String_pa; _temp115.f1=( struct _tagged_arr)* x;{ void* _temp114[ 1u]={&
_temp115}; Cyc_Stdio_aprintf( _tag_arr("missing type for parameter %s", sizeof(
unsigned char), 30u), _tag_arr( _temp114, sizeof( void*), 1u));}}), _temp110);}{
void* _temp116=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp112))->hd)->r; struct Cyc_Absyn_Vardecl* _temp122; _LL118: if(*((
int*) _temp116) == Cyc_Absyn_Var_d){ _LL123: _temp122=(( struct Cyc_Absyn_Var_d_struct*)
_temp116)->f1; goto _LL119;} else{ goto _LL120;} _LL120: goto _LL121; _LL119:
if( Cyc_Absyn_is_qvar_qualified( _temp122->name)){ Cyc_Parse_err( _tag_arr("namespace not allowed on parameter",
sizeof( unsigned char), 35u), _temp110);} if( Cyc_String_zstrptrcmp((* _temp122->name).f2,
x) == 0){ return({ struct _tuple2* _temp124=( struct _tuple2*) GC_malloc(
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
void* tm){ void* _temp127= tm; _LL129: if(( unsigned int) _temp127 > 1u?*(( int*)
_temp127) == Cyc_Absyn_TypeParams_mod: 0){ goto _LL130;} else{ goto _LL131;}
_LL131: goto _LL132; _LL130: return 1; _LL132: return 0; _LL128:;} static void*
Cyc_Parse_id2type( struct _tagged_arr s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp(
s, _tag_arr("`H", sizeof( unsigned char), 3u)) == 0){ return( void*) Cyc_Absyn_HeapRgn;}
else{ return( void*)({ struct Cyc_Absyn_VarType_struct* _temp133=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp133[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp134; _temp134.tag= Cyc_Absyn_VarType; _temp134.f1=({ struct Cyc_Absyn_Tvar*
_temp135=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp135->name=({ struct _tagged_arr* _temp136=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp136[ 0]= s; _temp136;}); _temp135->identity=
0; _temp135->kind= k; _temp135;}); _temp134;}); _temp133;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp137=
t; struct Cyc_Absyn_Tvar* _temp143; _LL139: if(( unsigned int) _temp137 > 4u?*((
int*) _temp137) == Cyc_Absyn_VarType: 0){ _LL144: _temp143=(( struct Cyc_Absyn_VarType_struct*)
_temp137)->f1; goto _LL140;} else{ goto _LL141;} _LL141: goto _LL142; _LL140:
return _temp143; _LL142: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_arr
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)( _tag_arr("expecting a list of type variables, not types",
sizeof( unsigned char), 46u), loc); _LL138:;} static void* Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp145=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp145[ 0]=({ struct Cyc_Absyn_VarType_struct _temp146; _temp146.tag= Cyc_Absyn_VarType;
_temp146.f1= pr; _temp146;}); _temp145;});} static int Cyc_Parse_set_vartyp_kind(
void* t, void* k){ void* _temp147= t; struct Cyc_Absyn_Tvar* _temp153; struct
Cyc_Absyn_Tvar _temp155; struct Cyc_Absyn_Conref* _temp156; _LL149: if((
unsigned int) _temp147 > 4u?*(( int*) _temp147) == Cyc_Absyn_VarType: 0){ _LL154:
_temp153=(( struct Cyc_Absyn_VarType_struct*) _temp147)->f1; _temp155=* _temp153;
_LL157: _temp156= _temp155.kind; goto _LL150;} else{ goto _LL151;} _LL151: goto
_LL152; _LL150: _temp156= Cyc_Absyn_compress_conref( _temp156);{ void* _temp158=(
void*) _temp156->v; void* _temp166; _LL160: if( _temp158 ==( void*) Cyc_Absyn_No_constr){
goto _LL161;} else{ goto _LL162;} _LL162: if(( unsigned int) _temp158 > 1u?*((
int*) _temp158) == Cyc_Absyn_Eq_constr: 0){ _LL167: _temp166=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp158)->f1; goto _LL163;} else{ goto _LL164;}
_LL164: goto _LL165; _LL161:( void*)( _temp156->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp168=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp168[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp169; _temp169.tag= Cyc_Absyn_Eq_constr;
_temp169.f1=( void*) k; _temp169;}); _temp168;}))); return 1; _LL163: return k
== _temp166; _LL165:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp170=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp170[ 0]=({ struct Cyc_Core_Impossible_struct _temp171; _temp171.tag= Cyc_Core_Impossible;
_temp171.f1= _tag_arr("forward after compress_conref", sizeof( unsigned char),
30u); _temp171;}); _temp170;})); _LL159:;} _LL152:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp172=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp172[ 0]=({ struct
Cyc_Core_Impossible_struct _temp173; _temp173.tag= Cyc_Core_Impossible; _temp173.f1=
_tag_arr("set_vartyp_kind: not a VarType", sizeof( unsigned char), 31u);
_temp173;}); _temp172;})); _LL148:;} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp174=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp180; _LL176:
if(( unsigned int) _temp174 > 1u?*(( int*) _temp174) == Cyc_Absyn_Function_mod:
0){ _LL181: _temp180=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp174)->f1;
goto _LL177;} else{ goto _LL178;} _LL178: goto _LL179; _LL177: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp182= _temp180; struct Cyc_List_List* _temp188; _LL184: if(*((
int*) _temp182) == Cyc_Absyn_WithTypes){ goto _LL185;} else{ goto _LL186;}
_LL186: if(*(( int*) _temp182) == Cyc_Absyn_NoTypes){ _LL189: _temp188=(( struct
Cyc_Absyn_NoTypes_struct*) _temp182)->f1; goto _LL187;} else{ goto _LL183;}
_LL185: Cyc_Parse_warn( _tag_arr("function declaration with both new- and old-style parameterdeclarations; ignoring old-style",
sizeof( unsigned char), 92u), loc); return tms; _LL187:(( void(*)( void(* f)(
struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl, _temp188,
tds);{ struct _tuple12* _temp190=({ struct _tuple12* _temp196=( struct _tuple12*)
GC_malloc( sizeof( struct _tuple12)); _temp196->f1= tds; _temp196->f2= loc;
_temp196;}); return({ struct Cyc_List_List* _temp191=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp191->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp192=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp192[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp193; _temp193.tag= Cyc_Absyn_Function_mod;
_temp193.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp194=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp194[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp195; _temp195.tag= Cyc_Absyn_WithTypes;
_temp195.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_arr*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp190, _temp188); _temp195.f2= 0; _temp195.f3= 0;
_temp195.f4= 0; _temp195.f5= 0; _temp195;}); _temp194;})); _temp193;}); _temp192;}));
_temp191->tl= 0; _temp191;});} _LL183:;} else{ return({ struct Cyc_List_List*
_temp197=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp197->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp197->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp197;});} _LL179: return({ struct Cyc_List_List*
_temp198=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp198->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd);
_temp198->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null(
tms))->tl, tds, loc); _temp198;}); _LL175:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp199=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp199->id= d->id; _temp199->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp199;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ switch(( enum  Cyc_Parse_Storage_class)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v){ case Cyc_Parse_Extern_sc:
_LL200: sc=( void*) Cyc_Absyn_Extern; break; case Cyc_Parse_ExternC_sc: _LL201:
sc=( void*) Cyc_Absyn_ExternC; break; case Cyc_Parse_Static_sc: _LL202: sc=(
void*) Cyc_Absyn_Static; break; default: _LL203: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); break;}}}{ struct Cyc_Core_Opt* _temp207;
void* _temp209; struct _tuple5 _temp205= Cyc_Parse_collapse_type_specifiers( tss,
loc); _LL210: _temp209= _temp205.f1; goto _LL208; _LL208: _temp207= _temp205.f2;
goto _LL206; _LL206: { struct Cyc_List_List* _temp213; struct Cyc_List_List*
_temp215; void* _temp217; struct Cyc_Absyn_Tqual _temp219; struct _tuple6
_temp211= Cyc_Parse_apply_tms( tq, _temp209, atts, d->tms); _LL220: _temp219=
_temp211.f1; goto _LL218; _LL218: _temp217= _temp211.f2; goto _LL216; _LL216:
_temp215= _temp211.f3; goto _LL214; _LL214: _temp213= _temp211.f4; goto _LL212;
_LL212: if( _temp207 != 0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp215 != 0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp221= _temp217; struct Cyc_Absyn_FnInfo _temp227; struct Cyc_List_List*
_temp229; struct Cyc_List_List* _temp231; struct Cyc_Absyn_VarargInfo* _temp233;
int _temp235; struct Cyc_List_List* _temp237; void* _temp239; struct Cyc_Core_Opt*
_temp241; struct Cyc_List_List* _temp243; _LL223: if(( unsigned int) _temp221 >
4u?*(( int*) _temp221) == Cyc_Absyn_FnType: 0){ _LL228: _temp227=(( struct Cyc_Absyn_FnType_struct*)
_temp221)->f1; _LL244: _temp243= _temp227.tvars; goto _LL242; _LL242: _temp241=
_temp227.effect; goto _LL240; _LL240: _temp239=( void*) _temp227.ret_typ; goto
_LL238; _LL238: _temp237= _temp227.args; goto _LL236; _LL236: _temp235= _temp227.c_varargs;
goto _LL234; _LL234: _temp233= _temp227.cyc_varargs; goto _LL232; _LL232:
_temp231= _temp227.rgn_po; goto _LL230; _LL230: _temp229= _temp227.attributes;
goto _LL224;} else{ goto _LL225;} _LL225: goto _LL226; _LL224: { struct Cyc_List_List*
_temp245=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp237); return({ struct Cyc_Absyn_Fndecl*
_temp246=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp246->sc=( void*) sc; _temp246->name= d->id; _temp246->tvs= _temp243;
_temp246->is_inline= is_inline; _temp246->effect= _temp241; _temp246->ret_type=(
void*) _temp239; _temp246->args= _temp245; _temp246->c_varargs= _temp235;
_temp246->cyc_varargs= _temp233; _temp246->rgn_po= _temp231; _temp246->body=
body; _temp246->cached_typ= 0; _temp246->param_vardecls= 0; _temp246->attributes=
Cyc_List_append( _temp229, _temp213); _temp246;});} _LL226: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL222:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp247=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp247->f1= _init_tag_arr(( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp247->f2=(* t).f2; _temp247->f3=(* t).f3; _temp247;});} else{ return({
struct _tuple4* _temp248=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp248->f1=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp248->f2=(* t).f2; _temp248->f3=(* t).f3; _temp248;});}} static
unsigned char _temp249[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp249, _temp249, _temp249 + 52u};
static unsigned char _temp250[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp250, _temp250, _temp250 + 63u};
static unsigned char _temp251[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp251, _temp251, _temp251 + 50u};
static unsigned char _temp252[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp252, _temp252, _temp252 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp253=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp267; void* _temp269; struct Cyc_Position_Segment*
_temp271; struct Cyc_Position_Segment* _temp273; struct Cyc_Position_Segment*
_temp275; struct Cyc_Position_Segment* _temp277; struct Cyc_Absyn_Decl* _temp279;
_LL255: if(*(( int*) _temp253) == Cyc_Parse_Type_spec){ _LL270: _temp269=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp253)->f1; goto _LL268; _LL268: _temp267=((
struct Cyc_Parse_Type_spec_struct*) _temp253)->f2; goto _LL256;} else{ goto
_LL257;} _LL257: if(*(( int*) _temp253) == Cyc_Parse_Signed_spec){ _LL272:
_temp271=(( struct Cyc_Parse_Signed_spec_struct*) _temp253)->f1; goto _LL258;}
else{ goto _LL259;} _LL259: if(*(( int*) _temp253) == Cyc_Parse_Unsigned_spec){
_LL274: _temp273=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp253)->f1; goto
_LL260;} else{ goto _LL261;} _LL261: if(*(( int*) _temp253) == Cyc_Parse_Short_spec){
_LL276: _temp275=(( struct Cyc_Parse_Short_spec_struct*) _temp253)->f1; goto
_LL262;} else{ goto _LL263;} _LL263: if(*(( int*) _temp253) == Cyc_Parse_Long_spec){
_LL278: _temp277=(( struct Cyc_Parse_Long_spec_struct*) _temp253)->f1; goto
_LL264;} else{ goto _LL265;} _LL265: if(*(( int*) _temp253) == Cyc_Parse_Decl_spec){
_LL280: _temp279=(( struct Cyc_Parse_Decl_spec_struct*) _temp253)->f1; goto
_LL266;} else{ goto _LL254;} _LL256: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp267);} last_loc= _temp267; seen_type= 1; t= _temp269; goto _LL254; _LL258:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp271);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp271);} last_loc= _temp271; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL254; _LL260: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp273);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp273);} last_loc= _temp273; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL254; _LL262: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp275);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp275);} last_loc= _temp275; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL254; _LL264: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp277);} if( seen_size){ void* _temp281= sz; _LL283: if( _temp281 ==( void*)
Cyc_Absyn_B4){ goto _LL284;} else{ goto _LL285;} _LL285: goto _LL286; _LL284: sz=(
void*) Cyc_Absyn_B8; goto _LL282; _LL286: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp277); goto _LL282; _LL282:;} last_loc=
_temp277; seen_size= 1; goto _LL254; _LL266: last_loc= _temp279->loc; if(
declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp287=( void*) _temp279->r;
struct Cyc_Absyn_Structdecl* _temp299; struct Cyc_Absyn_Tuniondecl* _temp301;
struct Cyc_Absyn_Uniondecl* _temp303; struct Cyc_Absyn_Enumdecl* _temp305;
_LL289: if(*(( int*) _temp287) == Cyc_Absyn_Struct_d){ _LL300: _temp299=((
struct Cyc_Absyn_Struct_d_struct*) _temp287)->f1; goto _LL290;} else{ goto
_LL291;} _LL291: if(*(( int*) _temp287) == Cyc_Absyn_Tunion_d){ _LL302: _temp301=((
struct Cyc_Absyn_Tunion_d_struct*) _temp287)->f1; goto _LL292;} else{ goto
_LL293;} _LL293: if(*(( int*) _temp287) == Cyc_Absyn_Union_d){ _LL304: _temp303=((
struct Cyc_Absyn_Union_d_struct*) _temp287)->f1; goto _LL294;} else{ goto _LL295;}
_LL295: if(*(( int*) _temp287) == Cyc_Absyn_Enum_d){ _LL306: _temp305=(( struct
Cyc_Absyn_Enum_d_struct*) _temp287)->f1; goto _LL296;} else{ goto _LL297;}
_LL297: goto _LL298; _LL290: { struct Cyc_List_List* _temp307=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp299->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp308=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp308[ 0]=({ struct Cyc_Absyn_StructType_struct _temp309; _temp309.tag= Cyc_Absyn_StructType;
_temp309.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp299->name))->v); _temp309.f2= _temp307; _temp309.f3= 0;
_temp309;}); _temp308;}); if( _temp299->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp310=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp310->v=( void*) _temp279; _temp310;});} goto _LL288;} _LL292: { struct Cyc_List_List*
_temp311=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp301->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp312=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp312[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp313; _temp313.tag= Cyc_Absyn_TunionType;
_temp313.f1=({ struct Cyc_Absyn_TunionInfo _temp314; _temp314.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp315=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp315[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp316; _temp316.tag= Cyc_Absyn_KnownTunion;
_temp316.f1= _temp301; _temp316;}); _temp315;})); _temp314.targs= _temp311;
_temp314.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp314;}); _temp313;});
_temp312;}); if( _temp301->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp317=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp317->v=( void*) _temp279; _temp317;});} goto _LL288;} _LL294: { struct Cyc_List_List*
_temp318=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp303->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp319=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp319[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp320; _temp320.tag= Cyc_Absyn_UnionType; _temp320.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp303->name))->v); _temp320.f2= _temp318; _temp320.f3= 0; _temp320;});
_temp319;}); if( _temp303->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp321=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp321->v=( void*) _temp279; _temp321;});} goto _LL288;} _LL296: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp322=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp322[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp323; _temp323.tag= Cyc_Absyn_EnumType; _temp323.f1=
_temp305->name; _temp323.f2= 0; _temp323;}); _temp322;}); declopt=({ struct Cyc_Core_Opt*
_temp324=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp324->v=( void*) _temp279; _temp324;}); goto _LL288; _LL298:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp279->loc); goto _LL288; _LL288:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp279->loc);} goto _LL254; _LL254:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp325=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp325[ 0]=({ struct Cyc_Absyn_IntType_struct _temp326; _temp326.tag= Cyc_Absyn_IntType;
_temp326.f1=( void*) sgn; _temp326.f2=( void*) sz; _temp326;}); _temp325;});}
else{ if( seen_sign){ void* _temp327= t; void* _temp333; _LL329: if((
unsigned int) _temp327 > 4u?*(( int*) _temp327) == Cyc_Absyn_IntType: 0){ _LL334:
_temp333=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp327)->f2; goto _LL330;}
else{ goto _LL331;} _LL331: goto _LL332; _LL330: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp335=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp335[ 0]=({ struct Cyc_Absyn_IntType_struct _temp336; _temp336.tag= Cyc_Absyn_IntType;
_temp336.f1=( void*) sgn; _temp336.f2=( void*) _temp333; _temp336;}); _temp335;});
goto _LL328; _LL332: Cyc_Parse_err( _tag_arr("sign specification on non-integral type",
sizeof( unsigned char), 40u), last_loc); goto _LL328; _LL328:;} if( seen_size){
void* _temp337= t; void* _temp343; _LL339: if(( unsigned int) _temp337 > 4u?*((
int*) _temp337) == Cyc_Absyn_IntType: 0){ _LL344: _temp343=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp337)->f1; goto _LL340;} else{ goto _LL341;} _LL341: goto _LL342; _LL340: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp345=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp345[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp346; _temp346.tag= Cyc_Absyn_IntType; _temp346.f1=( void*) _temp343;
_temp346.f2=( void*) sz; _temp346;}); _temp345;}); goto _LL338; _LL342: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL338; _LL338:;}} return({ struct _tuple5 _temp347; _temp347.f1=
t; _temp347.f2= declopt; _temp347;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp348=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp349= _temp348->id; struct Cyc_List_List* _temp352; struct
Cyc_List_List* _temp354; void* _temp356; struct Cyc_Absyn_Tqual _temp358; struct
_tuple6 _temp350= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp348->tms);
_LL359: _temp358= _temp350.f1; goto _LL357; _LL357: _temp356= _temp350.f2; goto
_LL355; _LL355: _temp354= _temp350.f3; goto _LL353; _LL353: _temp352= _temp350.f4;
goto _LL351; _LL351: return({ struct Cyc_List_List* _temp360=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp360->hd=( void*)({ struct
_tuple7* _temp361=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp361->f1= _temp349; _temp361->f2= _temp358; _temp361->f3= _temp356; _temp361->f4=
_temp354; _temp361->f5= _temp352; _temp361;}); _temp360->tl= Cyc_Parse_apply_tmss(
_temp358, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp360;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp362; _temp362.f1= tq; _temp362.f2=
t; _temp362.f3= 0; _temp362.f4= atts; _temp362;});}{ void* _temp363=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp377;
void* _temp379; struct Cyc_Position_Segment* _temp381; struct Cyc_List_List*
_temp383; struct Cyc_Absyn_Tqual _temp385; void* _temp387; void* _temp389;
struct Cyc_List_List* _temp391; struct Cyc_Position_Segment* _temp393; _LL365:
if( _temp363 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL366;} else{ goto _LL367;}
_LL367: if(( unsigned int) _temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_ConstArray_mod:
0){ _LL378: _temp377=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp363)->f1;
goto _LL368;} else{ goto _LL369;} _LL369: if(( unsigned int) _temp363 > 1u?*((
int*) _temp363) == Cyc_Absyn_Function_mod: 0){ _LL380: _temp379=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp363)->f1; goto _LL370;} else{ goto
_LL371;} _LL371: if(( unsigned int) _temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_TypeParams_mod:
0){ _LL384: _temp383=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp363)->f1;
goto _LL382; _LL382: _temp381=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp363)->f2; goto _LL372;} else{ goto _LL373;} _LL373: if(( unsigned int)
_temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Pointer_mod: 0){ _LL390: _temp389=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp363)->f1; goto _LL388;
_LL388: _temp387=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp363)->f2;
goto _LL386; _LL386: _temp385=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp363)->f3;
goto _LL374;} else{ goto _LL375;} _LL375: if(( unsigned int) _temp363 > 1u?*((
int*) _temp363) == Cyc_Absyn_Attributes_mod: 0){ _LL394: _temp393=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp363)->f1; goto _LL392; _LL392: _temp391=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp363)->f2; goto _LL376;} else{ goto _LL364;} _LL366: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp395=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp395[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp396; _temp396.tag= Cyc_Absyn_ArrayType;
_temp396.f1=( void*) t; _temp396.f2= tq; _temp396.f3= 0; _temp396;}); _temp395;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL368: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp397=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp397[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp398; _temp398.tag= Cyc_Absyn_ArrayType;
_temp398.f1=( void*) t; _temp398.f2= tq; _temp398.f3=( struct Cyc_Absyn_Exp*)
_temp377; _temp398;}); _temp397;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL370: { void* _temp399= _temp379; struct Cyc_List_List* _temp405;
struct Cyc_Core_Opt* _temp407; struct Cyc_Absyn_VarargInfo* _temp409; int
_temp411; struct Cyc_List_List* _temp413; struct Cyc_Position_Segment* _temp415;
_LL401: if(*(( int*) _temp399) == Cyc_Absyn_WithTypes){ _LL414: _temp413=((
struct Cyc_Absyn_WithTypes_struct*) _temp399)->f1; goto _LL412; _LL412: _temp411=((
struct Cyc_Absyn_WithTypes_struct*) _temp399)->f2; goto _LL410; _LL410: _temp409=((
struct Cyc_Absyn_WithTypes_struct*) _temp399)->f3; goto _LL408; _LL408: _temp407=((
struct Cyc_Absyn_WithTypes_struct*) _temp399)->f4; goto _LL406; _LL406: _temp405=((
struct Cyc_Absyn_WithTypes_struct*) _temp399)->f5; goto _LL402;} else{ goto
_LL403;} _LL403: if(*(( int*) _temp399) == Cyc_Absyn_NoTypes){ _LL416: _temp415=((
struct Cyc_Absyn_NoTypes_struct*) _temp399)->f2; goto _LL404;} else{ goto _LL400;}
_LL402: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp417=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp417->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp417->tl= fn_atts; _temp417;});} else{ new_atts=({ struct Cyc_List_List*
_temp418=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp418->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp418->tl= new_atts; _temp418;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp419=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp425; _LL421: if(( unsigned int) _temp419 > 1u?*(( int*) _temp419) == Cyc_Absyn_TypeParams_mod:
0){ _LL426: _temp425=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp419)->f1;
goto _LL422;} else{ goto _LL423;} _LL423: goto _LL424; _LL422: typvars= _temp425;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL420; _LL424: goto
_LL420; _LL420:;} if((((( ! _temp411? _temp409 == 0: 0)? _temp413 != 0: 0)?((
struct Cyc_List_List*) _check_null( _temp413))->tl == 0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp413))->hd)).f1 == 0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp413))->hd)).f3 ==( void*)
Cyc_Absyn_VoidType: 0){ _temp413= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp413); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp407, t, _temp413, _temp411, _temp409, _temp405, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL404:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp415)); _LL400:;} _LL372: if((( struct Cyc_List_List*) _check_null(
tms))->tl == 0){ return({ struct _tuple6 _temp427; _temp427.f1= tq; _temp427.f2=
t; _temp427.f3= _temp383; _temp427.f4= atts; _temp427;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp381)); _LL374: { void* _temp428= _temp389;
struct Cyc_Absyn_Exp* _temp436; struct Cyc_Absyn_Exp* _temp438; _LL430: if((
unsigned int) _temp428 > 1u?*(( int*) _temp428) == Cyc_Absyn_NonNullable_ps: 0){
_LL437: _temp436=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp428)->f1; goto
_LL431;} else{ goto _LL432;} _LL432: if(( unsigned int) _temp428 > 1u?*(( int*)
_temp428) == Cyc_Absyn_Nullable_ps: 0){ _LL439: _temp438=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp428)->f1; goto _LL433;} else{ goto _LL434;} _LL434: if( _temp428 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL435;} else{ goto _LL429;} _LL431: return Cyc_Parse_apply_tms(
_temp385, Cyc_Absyn_atb_typ( t, _temp387, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp440=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp440[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp441; _temp441.tag= Cyc_Absyn_Upper_b;
_temp441.f1= _temp436; _temp441;}); _temp440;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL433: return Cyc_Parse_apply_tms( _temp385, Cyc_Absyn_starb_typ(
t, _temp387, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp442=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp442[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp443; _temp443.tag= Cyc_Absyn_Upper_b;
_temp443.f1= _temp438; _temp443;}); _temp442;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL435: return Cyc_Parse_apply_tms( _temp385, Cyc_Absyn_tagged_typ(
t, _temp387, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL429:;}
_LL376: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp391),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL364:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp446; void* _temp448; struct _tuple5 _temp444= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL449: _temp448= _temp444.f1; goto _LL447; _LL447: _temp446=
_temp444.f2; goto _LL445; _LL445: if( _temp446 != 0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp448;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp450=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp450[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp451; _temp451.tag= Cyc_Absyn_Decl_s;
_temp451.f1= d; _temp451.f2= s; _temp451;}); _temp450;}), Cyc_Position_segment_join(
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
sizeof( unsigned char), 39u), loc); return 0;} if( ds->sc != 0){ switch(( enum 
Cyc_Parse_Storage_class)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v){ case
Cyc_Parse_Typedef_sc: _LL452: istypedef= 1; break; case Cyc_Parse_Extern_sc:
_LL453: s=( void*) Cyc_Absyn_Extern; break; case Cyc_Parse_ExternC_sc: _LL454: s=(
void*) Cyc_Absyn_ExternC; break; case Cyc_Parse_Static_sc: _LL455: s=( void*)
Cyc_Absyn_Static; break; case Cyc_Parse_Auto_sc: _LL456: s=( void*) Cyc_Absyn_Public;
break; case Cyc_Parse_Register_sc: _LL457: s=( void*) Cyc_Absyn_Public; break;
case Cyc_Parse_Abstract_sc: _LL458: s=( void*) Cyc_Absyn_Abstract; break;}}{
struct Cyc_List_List* _temp462; struct Cyc_List_List* _temp464; struct _tuple0
_temp460=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( ids);
_LL465: _temp464= _temp460.f1; goto _LL463; _LL463: _temp462= _temp460.f2; goto
_LL461; _LL461: { int exps_empty= 1;{ struct Cyc_List_List* es= _temp462; for( 0;
es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp466= Cyc_Parse_collapse_type_specifiers( tss, loc); if(
_temp464 == 0){ struct Cyc_Core_Opt* _temp469; void* _temp471; struct _tuple5
_temp467= _temp466; _LL472: _temp471= _temp467.f1; goto _LL470; _LL470: _temp469=
_temp467.f2; goto _LL468; _LL468: if( _temp469 != 0){ struct Cyc_Absyn_Decl* d=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null( _temp469))->v;{
void* _temp473=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp485; struct Cyc_Absyn_Structdecl*
_temp487; struct Cyc_Absyn_Uniondecl* _temp489; struct Cyc_Absyn_Tuniondecl*
_temp491; _LL475: if(*(( int*) _temp473) == Cyc_Absyn_Enum_d){ _LL486: _temp485=((
struct Cyc_Absyn_Enum_d_struct*) _temp473)->f1; goto _LL476;} else{ goto _LL477;}
_LL477: if(*(( int*) _temp473) == Cyc_Absyn_Struct_d){ _LL488: _temp487=((
struct Cyc_Absyn_Struct_d_struct*) _temp473)->f1; goto _LL478;} else{ goto
_LL479;} _LL479: if(*(( int*) _temp473) == Cyc_Absyn_Union_d){ _LL490: _temp489=((
struct Cyc_Absyn_Union_d_struct*) _temp473)->f1; goto _LL480;} else{ goto _LL481;}
_LL481: if(*(( int*) _temp473) == Cyc_Absyn_Tunion_d){ _LL492: _temp491=((
struct Cyc_Absyn_Tunion_d_struct*) _temp473)->f1; goto _LL482;} else{ goto
_LL483;} _LL483: goto _LL484; _LL476:( void*)( _temp485->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on enum", sizeof(
unsigned char), 23u), loc);} goto _LL474; _LL478:( void*)( _temp487->sc=( void*)
s); _temp487->attributes= atts; goto _LL474; _LL480:( void*)( _temp489->sc=(
void*) s); _temp489->attributes= atts; goto _LL474; _LL482:( void*)( _temp491->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL474; _LL484: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL474:;}
return({ struct Cyc_List_List* _temp493=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp493->hd=( void*) d; _temp493->tl= 0;
_temp493;});} else{ void* _temp494= _temp471; struct Cyc_List_List* _temp508;
struct _tuple1* _temp510; struct Cyc_Absyn_TunionInfo _temp512; void* _temp514;
struct Cyc_Absyn_Tuniondecl* _temp516; struct Cyc_Absyn_TunionInfo _temp518;
struct Cyc_List_List* _temp520; void* _temp522; struct Cyc_Absyn_UnknownTunionInfo
_temp524; int _temp526; struct _tuple1* _temp528; struct Cyc_List_List* _temp530;
struct _tuple1* _temp532; struct _tuple1* _temp534; _LL496: if(( unsigned int)
_temp494 > 4u?*(( int*) _temp494) == Cyc_Absyn_StructType: 0){ _LL511: _temp510=((
struct Cyc_Absyn_StructType_struct*) _temp494)->f1; goto _LL509; _LL509:
_temp508=(( struct Cyc_Absyn_StructType_struct*) _temp494)->f2; goto _LL497;}
else{ goto _LL498;} _LL498: if(( unsigned int) _temp494 > 4u?*(( int*) _temp494)
== Cyc_Absyn_TunionType: 0){ _LL513: _temp512=(( struct Cyc_Absyn_TunionType_struct*)
_temp494)->f1; _LL515: _temp514=( void*) _temp512.tunion_info; if(*(( int*)
_temp514) == Cyc_Absyn_KnownTunion){ _LL517: _temp516=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp514)->f1; goto _LL499;} else{ goto _LL500;}} else{ goto _LL500;} _LL500:
if(( unsigned int) _temp494 > 4u?*(( int*) _temp494) == Cyc_Absyn_TunionType: 0){
_LL519: _temp518=(( struct Cyc_Absyn_TunionType_struct*) _temp494)->f1; _LL523:
_temp522=( void*) _temp518.tunion_info; if(*(( int*) _temp522) == Cyc_Absyn_UnknownTunion){
_LL525: _temp524=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp522)->f1;
_LL529: _temp528= _temp524.name; goto _LL527; _LL527: _temp526= _temp524.is_xtunion;
goto _LL521;} else{ goto _LL502;} _LL521: _temp520= _temp518.targs; goto _LL501;}
else{ goto _LL502;} _LL502: if(( unsigned int) _temp494 > 4u?*(( int*) _temp494)
== Cyc_Absyn_UnionType: 0){ _LL533: _temp532=(( struct Cyc_Absyn_UnionType_struct*)
_temp494)->f1; goto _LL531; _LL531: _temp530=(( struct Cyc_Absyn_UnionType_struct*)
_temp494)->f2; goto _LL503;} else{ goto _LL504;} _LL504: if(( unsigned int)
_temp494 > 4u?*(( int*) _temp494) == Cyc_Absyn_EnumType: 0){ _LL535: _temp534=((
struct Cyc_Absyn_EnumType_struct*) _temp494)->f1; goto _LL505;} else{ goto
_LL506;} _LL506: goto _LL507; _LL497: { struct Cyc_List_List* _temp536=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp508); struct Cyc_Absyn_Structdecl* _temp537=({
struct Cyc_Absyn_Structdecl* _temp541=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp541->sc=( void*) s; _temp541->name=({
struct Cyc_Core_Opt* _temp542=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp542->v=( void*)(( struct _tuple1*) _check_null( _temp510));
_temp542;}); _temp541->tvs= _temp536; _temp541->fields= 0; _temp541->attributes=
0; _temp541;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp538=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp538->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp539=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp539[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp540; _temp540.tag= Cyc_Absyn_Struct_d;
_temp540.f1= _temp537; _temp540;}); _temp539;}), loc); _temp538->tl= 0; _temp538;});}
_LL499: if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp543=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp543->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp544=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp544[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp545; _temp545.tag= Cyc_Absyn_Tunion_d;
_temp545.f1= _temp516; _temp545;}); _temp544;}), loc); _temp543->tl= 0; _temp543;});
_LL501: { struct Cyc_List_List* _temp546=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp520);
struct Cyc_Absyn_Decl* _temp547= Cyc_Absyn_tunion_decl( s, _temp528, _temp546, 0,
_temp526, loc); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp548=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp548->hd=(
void*) _temp547; _temp548->tl= 0; _temp548;});} _LL503: { struct Cyc_List_List*
_temp549=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp530); struct Cyc_Absyn_Uniondecl* _temp550=({
struct Cyc_Absyn_Uniondecl* _temp555=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp555->sc=( void*) s; _temp555->name=({
struct Cyc_Core_Opt* _temp556=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp556->v=( void*)(( struct _tuple1*) _check_null( _temp532));
_temp556;}); _temp555->tvs= _temp549; _temp555->fields= 0; _temp555->attributes=
0; _temp555;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp551=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp551->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp552=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp552->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp553=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp553[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp554; _temp554.tag= Cyc_Absyn_Union_d;
_temp554.f1= _temp550; _temp554;}); _temp553;})); _temp552->loc= loc; _temp552;});
_temp551->tl= 0; _temp551;});} _LL505: { struct Cyc_Absyn_Enumdecl* _temp557=({
struct Cyc_Absyn_Enumdecl* _temp562=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp562->sc=( void*) s; _temp562->name=
_temp534; _temp562->fields= 0; _temp562;}); if( atts != 0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp558=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp558->hd=( void*)({ struct Cyc_Absyn_Decl* _temp559=(
struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp559->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp560=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp560[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp561; _temp561.tag= Cyc_Absyn_Enum_d; _temp561.f1= _temp557; _temp561;});
_temp560;})); _temp559->loc= loc; _temp559;}); _temp558->tl= 0; _temp558;});}
_LL507: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL495:;}} else{ void* t= _temp466.f1; struct Cyc_List_List*
_temp563= Cyc_Parse_apply_tmss( tq, t, _temp464, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp563); if( _temp466.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp466.f2))->v;{ void* _temp564=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp576; struct Cyc_Absyn_Tuniondecl* _temp578;
struct Cyc_Absyn_Uniondecl* _temp580; struct Cyc_Absyn_Enumdecl* _temp582;
_LL566: if(*(( int*) _temp564) == Cyc_Absyn_Struct_d){ _LL577: _temp576=((
struct Cyc_Absyn_Struct_d_struct*) _temp564)->f1; goto _LL567;} else{ goto
_LL568;} _LL568: if(*(( int*) _temp564) == Cyc_Absyn_Tunion_d){ _LL579: _temp578=((
struct Cyc_Absyn_Tunion_d_struct*) _temp564)->f1; goto _LL569;} else{ goto
_LL570;} _LL570: if(*(( int*) _temp564) == Cyc_Absyn_Union_d){ _LL581: _temp580=((
struct Cyc_Absyn_Union_d_struct*) _temp564)->f1; goto _LL571;} else{ goto _LL572;}
_LL572: if(*(( int*) _temp564) == Cyc_Absyn_Enum_d){ _LL583: _temp582=(( struct
Cyc_Absyn_Enum_d_struct*) _temp564)->f1; goto _LL573;} else{ goto _LL574;}
_LL574: goto _LL575; _LL567:( void*)( _temp576->sc=( void*) s); _temp576->attributes=
atts; atts= 0; goto _LL565; _LL569:( void*)( _temp578->sc=( void*) s); goto
_LL565; _LL571:( void*)( _temp580->sc=( void*) s); goto _LL565; _LL573:( void*)(
_temp582->sc=( void*) s); goto _LL565; _LL575: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL565; _LL565:;} decls=({ struct Cyc_List_List*
_temp584=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp584->hd=( void*) d; _temp584->tl= decls; _temp584;});} if( atts != 0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp586; _temp586.tag=
Cyc_Stdio_String_pa; _temp586.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp585[ 1u]={&
_temp586}; Cyc_Stdio_aprintf( _tag_arr("bad attribute %s in typedef", sizeof(
unsigned char), 28u), _tag_arr( _temp585, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp466.f2 != 0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp587= _temp563; for( 0; _temp587 != 0;( _temp587=(( struct
Cyc_List_List*) _check_null( _temp587))->tl, _temp462=(( struct Cyc_List_List*)
_check_null( _temp462))->tl)){ struct _tuple7 _temp590; struct Cyc_List_List*
_temp591; struct Cyc_List_List* _temp593; void* _temp595; struct Cyc_Absyn_Tqual
_temp597; struct _tuple1* _temp599; struct _tuple7* _temp588=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp587))->hd; _temp590=* _temp588; _LL600:
_temp599= _temp590.f1; goto _LL598; _LL598: _temp597= _temp590.f2; goto _LL596;
_LL596: _temp595= _temp590.f3; goto _LL594; _LL594: _temp593= _temp590.f4; goto
_LL592; _LL592: _temp591= _temp590.f5; goto _LL589; _LL589: if( _temp593 != 0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp462 == 0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected null in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp601=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp462))->hd; struct Cyc_Absyn_Vardecl*
_temp602= Cyc_Absyn_new_vardecl( _temp599, _temp595, _temp601); _temp602->tq=
_temp597;( void*)( _temp602->sc=( void*) s); _temp602->attributes= _temp591;{
struct Cyc_Absyn_Decl* _temp603=({ struct Cyc_Absyn_Decl* _temp605=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp605->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp606=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp606[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp607; _temp607.tag= Cyc_Absyn_Var_d; _temp607.f1= _temp602; _temp607;});
_temp606;})); _temp605->loc= loc; _temp605;}); decls=({ struct Cyc_List_List*
_temp604=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp604->hd=( void*) _temp603; _temp604->tl= decls; _temp604;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_String_strlen( s) != 1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp609; _temp609.tag= Cyc_Stdio_String_pa;
_temp609.f1=( struct _tagged_arr) s;{ void* _temp608[ 1u]={& _temp609}; Cyc_Stdio_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp608,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL610: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL611: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL612: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL613: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL614: return(
void*) Cyc_Absyn_EffKind; default: _LL615: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp618; _temp618.tag= Cyc_Stdio_String_pa;
_temp618.f1=( struct _tagged_arr) s;{ void* _temp617[ 1u]={& _temp618}; Cyc_Stdio_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp617,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){ return
tms;} else{ return({ struct Cyc_List_List* _temp619=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp619->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp620=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp620[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp621; _temp621.tag= Cyc_Absyn_Attributes_mod;
_temp621.f1= loc; _temp621.f2= atts; _temp621;}); _temp620;})); _temp619->tl=
tms; _temp619;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp624;
struct Cyc_List_List* _temp625; struct Cyc_List_List* _temp627; void* _temp629;
struct Cyc_Absyn_Tqual _temp631; struct _tuple1* _temp633; struct _tuple7*
_temp622= t; _temp624=* _temp622; _LL634: _temp633= _temp624.f1; goto _LL632;
_LL632: _temp631= _temp624.f2; goto _LL630; _LL630: _temp629= _temp624.f3; goto
_LL628; _LL628: _temp627= _temp624.f4; goto _LL626; _LL626: _temp625= _temp624.f5;
goto _LL623; _LL623: Cyc_Lex_register_typedef( _temp633); if( _temp625 != 0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp636;
_temp636.tag= Cyc_Stdio_String_pa; _temp636.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp625))->hd);{ void* _temp635[ 1u]={&
_temp636}; Cyc_Stdio_aprintf( _tag_arr("bad attribute %s within typedef",
sizeof( unsigned char), 32u), _tag_arr( _temp635, sizeof( void*), 1u));}}), loc);}
return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp637=(
struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp637[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp638; _temp638.tag= Cyc_Absyn_Typedef_d;
_temp638.f1=({ struct Cyc_Absyn_Typedefdecl* _temp639=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp639->name= _temp633;
_temp639->tvs= _temp627; _temp639->defn=( void*) _temp629; _temp639;}); _temp638;});
_temp637;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
unsigned char* tag; struct _tuple18* f1; } ; unsigned char Cyc_Rgnorder_tok[ 17u]="\000\000\000\000Rgnorder_tok";
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
unsigned char _temp641[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp641, _temp641, _temp641 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp642= yy1; struct _tuple16* _temp648; _LL644: if(*(( void**)
_temp642) == Cyc_Int_tok){ _LL649: _temp648=(( struct Cyc_Int_tok_struct*)
_temp642)->f1; goto _LL645;} else{ goto _LL646;} _LL646: goto _LL647; _LL645:
return _temp648; _LL647:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL643:;}
static unsigned char _temp651[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp651, _temp651, _temp651 + 11u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ void* _temp652= yy1; struct
_tagged_arr _temp658; _LL654: if(*(( void**) _temp652) == Cyc_String_tok){
_LL659: _temp658=(( struct Cyc_String_tok_struct*) _temp652)->f1; goto _LL655;}
else{ goto _LL656;} _LL656: goto _LL657; _LL655: return _temp658; _LL657:( int)
_throw(( void*)& Cyc_yyfail_String_tok); _LL653:;} static unsigned char _temp661[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp661, _temp661, _temp661 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp662= yy1; unsigned char _temp668; _LL664: if(*(( void**) _temp662) ==
Cyc_Char_tok){ _LL669: _temp668=(( struct Cyc_Char_tok_struct*) _temp662)->f1;
goto _LL665;} else{ goto _LL666;} _LL666: goto _LL667; _LL665: return _temp668;
_LL667:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL663:;} static
unsigned char _temp671[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp671, _temp671, _temp671 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp672= yy1; void*
_temp678; _LL674: if(*(( void**) _temp672) == Cyc_Pointer_Sort_tok){ _LL679:
_temp678=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp672)->f1; goto
_LL675;} else{ goto _LL676;} _LL676: goto _LL677; _LL675: return _temp678;
_LL677:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL673:;} static
unsigned char _temp681[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp681, _temp681, _temp681 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp682= yy1; struct Cyc_Absyn_Exp*
_temp688; _LL684: if(*(( void**) _temp682) == Cyc_Exp_tok){ _LL689: _temp688=((
struct Cyc_Exp_tok_struct*) _temp682)->f1; goto _LL685;} else{ goto _LL686;}
_LL686: goto _LL687; _LL685: return _temp688; _LL687:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL683:;} static unsigned char _temp691[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{ _temp691, _temp691, _temp691 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( void* yy1){ void* _temp692= yy1;
struct Cyc_List_List* _temp698; _LL694: if(*(( void**) _temp692) == Cyc_ExpList_tok){
_LL699: _temp698=(( struct Cyc_ExpList_tok_struct*) _temp692)->f1; goto _LL695;}
else{ goto _LL696;} _LL696: goto _LL697; _LL695: return _temp698; _LL697:( int)
_throw(( void*)& Cyc_yyfail_ExpList_tok); _LL693:;} static unsigned char
_temp701[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure,{ _temp701, _temp701, _temp701 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp702= yy1; struct Cyc_List_List*
_temp708; _LL704: if(*(( void**) _temp702) == Cyc_InitializerList_tok){ _LL709:
_temp708=(( struct Cyc_InitializerList_tok_struct*) _temp702)->f1; goto _LL705;}
else{ goto _LL706;} _LL706: goto _LL707; _LL705: return _temp708; _LL707:( int)
_throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL703:;} static unsigned char
_temp711[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{ _temp711, _temp711, _temp711 + 11u}}; void* Cyc_yyget_Primop_tok(
void* yy1){ void* _temp712= yy1; void* _temp718; _LL714: if(*(( void**) _temp712)
== Cyc_Primop_tok){ _LL719: _temp718=( void*)(( struct Cyc_Primop_tok_struct*)
_temp712)->f1; goto _LL715;} else{ goto _LL716;} _LL716: goto _LL717; _LL715:
return _temp718; _LL717:( int) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL713:;}
static unsigned char _temp721[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp721, _temp721, _temp721 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp722= yy1;
struct Cyc_Core_Opt* _temp728; _LL724: if(*(( void**) _temp722) == Cyc_Primopopt_tok){
_LL729: _temp728=(( struct Cyc_Primopopt_tok_struct*) _temp722)->f1; goto _LL725;}
else{ goto _LL726;} _LL726: goto _LL727; _LL725: return _temp728; _LL727:( int)
_throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL723:;} static unsigned char
_temp731[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{ _temp731, _temp731, _temp731 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok(
void* yy1){ void* _temp732= yy1; struct _tuple1* _temp738; _LL734: if(*(( void**)
_temp732) == Cyc_QualId_tok){ _LL739: _temp738=(( struct Cyc_QualId_tok_struct*)
_temp732)->f1; goto _LL735;} else{ goto _LL736;} _LL736: goto _LL737; _LL735:
return _temp738; _LL737:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL733:;}
static unsigned char _temp741[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp741, _temp741, _temp741 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp742= yy1;
struct Cyc_Absyn_Stmt* _temp748; _LL744: if(*(( void**) _temp742) == Cyc_Stmt_tok){
_LL749: _temp748=(( struct Cyc_Stmt_tok_struct*) _temp742)->f1; goto _LL745;}
else{ goto _LL746;} _LL746: goto _LL747; _LL745: return _temp748; _LL747:( int)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL743:;} static unsigned char _temp751[
21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={
Cyc_Core_Failure,{ _temp751, _temp751, _temp751 + 21u}}; struct Cyc_List_List*
Cyc_yyget_SwitchClauseList_tok( void* yy1){ void* _temp752= yy1; struct Cyc_List_List*
_temp758; _LL754: if(*(( void**) _temp752) == Cyc_SwitchClauseList_tok){ _LL759:
_temp758=(( struct Cyc_SwitchClauseList_tok_struct*) _temp752)->f1; goto _LL755;}
else{ goto _LL756;} _LL756: goto _LL757; _LL755: return _temp758; _LL757:( int)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL753:;} static
unsigned char _temp761[ 22u]="SwitchCClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{ _temp761, _temp761,
_temp761 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok( void*
yy1){ void* _temp762= yy1; struct Cyc_List_List* _temp768; _LL764: if(*(( void**)
_temp762) == Cyc_SwitchCClauseList_tok){ _LL769: _temp768=(( struct Cyc_SwitchCClauseList_tok_struct*)
_temp762)->f1; goto _LL765;} else{ goto _LL766;} _LL766: goto _LL767; _LL765:
return _temp768; _LL767:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL763:;} static unsigned char _temp771[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp771, _temp771, _temp771 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp772= yy1;
struct Cyc_Absyn_Pat* _temp778; _LL774: if(*(( void**) _temp772) == Cyc_Pattern_tok){
_LL779: _temp778=(( struct Cyc_Pattern_tok_struct*) _temp772)->f1; goto _LL775;}
else{ goto _LL776;} _LL776: goto _LL777; _LL775: return _temp778; _LL777:( int)
_throw(( void*)& Cyc_yyfail_Pattern_tok); _LL773:;} static unsigned char
_temp781[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp781, _temp781, _temp781 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp782= yy1; struct Cyc_List_List*
_temp788; _LL784: if(*(( void**) _temp782) == Cyc_PatternList_tok){ _LL789:
_temp788=(( struct Cyc_PatternList_tok_struct*) _temp782)->f1; goto _LL785;}
else{ goto _LL786;} _LL786: goto _LL787; _LL785: return _temp788; _LL787:( int)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL783:;} static unsigned char
_temp791[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp791, _temp791, _temp791 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp792= yy1; struct _tuple14* _temp798; _LL794: if(*(( void**)
_temp792) == Cyc_FieldPattern_tok){ _LL799: _temp798=(( struct Cyc_FieldPattern_tok_struct*)
_temp792)->f1; goto _LL795;} else{ goto _LL796;} _LL796: goto _LL797; _LL795:
return _temp798; _LL797:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL793:;} static unsigned char _temp801[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp801, _temp801, _temp801 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp802= yy1; struct Cyc_List_List* _temp808; _LL804: if(*((
void**) _temp802) == Cyc_FieldPatternList_tok){ _LL809: _temp808=(( struct Cyc_FieldPatternList_tok_struct*)
_temp802)->f1; goto _LL805;} else{ goto _LL806;} _LL806: goto _LL807; _LL805:
return _temp808; _LL807:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL803:;} static unsigned char _temp811[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp811, _temp811, _temp811 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp812= yy1;
struct Cyc_Absyn_Fndecl* _temp818; _LL814: if(*(( void**) _temp812) == Cyc_FnDecl_tok){
_LL819: _temp818=(( struct Cyc_FnDecl_tok_struct*) _temp812)->f1; goto _LL815;}
else{ goto _LL816;} _LL816: goto _LL817; _LL815: return _temp818; _LL817:( int)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL813:;} static unsigned char _temp821[
13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{ _temp821, _temp821, _temp821 + 13u}}; struct Cyc_List_List*
Cyc_yyget_DeclList_tok( void* yy1){ void* _temp822= yy1; struct Cyc_List_List*
_temp828; _LL824: if(*(( void**) _temp822) == Cyc_DeclList_tok){ _LL829:
_temp828=(( struct Cyc_DeclList_tok_struct*) _temp822)->f1; goto _LL825;} else{
goto _LL826;} _LL826: goto _LL827; _LL825: return _temp828; _LL827:( int) _throw((
void*)& Cyc_yyfail_DeclList_tok); _LL823:;} static unsigned char _temp831[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{
_temp831, _temp831, _temp831 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
void* yy1){ void* _temp832= yy1; struct Cyc_Parse_Declaration_spec* _temp838;
_LL834: if(*(( void**) _temp832) == Cyc_DeclSpec_tok){ _LL839: _temp838=((
struct Cyc_DeclSpec_tok_struct*) _temp832)->f1; goto _LL835;} else{ goto _LL836;}
_LL836: goto _LL837; _LL835: return _temp838; _LL837:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL833:;} static unsigned char _temp841[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp841, _temp841, _temp841 + 13u}};
struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp842= yy1; struct
_tuple15* _temp848; _LL844: if(*(( void**) _temp842) == Cyc_InitDecl_tok){
_LL849: _temp848=(( struct Cyc_InitDecl_tok_struct*) _temp842)->f1; goto _LL845;}
else{ goto _LL846;} _LL846: goto _LL847; _LL845: return _temp848; _LL847:( int)
_throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL843:;} static unsigned char
_temp851[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={
Cyc_Core_Failure,{ _temp851, _temp851, _temp851 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp852= yy1; struct Cyc_List_List*
_temp858; _LL854: if(*(( void**) _temp852) == Cyc_InitDeclList_tok){ _LL859:
_temp858=(( struct Cyc_InitDeclList_tok_struct*) _temp852)->f1; goto _LL855;}
else{ goto _LL856;} _LL856: goto _LL857; _LL855: return _temp858; _LL857:( int)
_throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL853:;} static unsigned char
_temp861[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={
Cyc_Core_Failure,{ _temp861, _temp861, _temp861 + 17u}}; enum  Cyc_Parse_Storage_class
Cyc_yyget_StorageClass_tok( void* yy1){ void* _temp862= yy1; enum  Cyc_Parse_Storage_class
_temp868; _LL864: if(*(( void**) _temp862) == Cyc_StorageClass_tok){ _LL869:
_temp868=(( struct Cyc_StorageClass_tok_struct*) _temp862)->f1; goto _LL865;}
else{ goto _LL866;} _LL866: goto _LL867; _LL865: return _temp868; _LL867:( int)
_throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL863:;} static unsigned char
_temp871[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={
Cyc_Core_Failure,{ _temp871, _temp871, _temp871 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok(
void* yy1){ void* _temp872= yy1; void* _temp878; _LL874: if(*(( void**) _temp872)
== Cyc_TypeSpecifier_tok){ _LL879: _temp878=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp872)->f1; goto _LL875;} else{ goto _LL876;} _LL876: goto _LL877; _LL875:
return _temp878; _LL877:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL873:;} static unsigned char _temp881[ 18u]="StructOrUnion_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp881, _temp881, _temp881 + 18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ void* _temp882= yy1; enum  Cyc_Parse_Struct_or_union _temp888;
_LL884: if(*(( void**) _temp882) == Cyc_StructOrUnion_tok){ _LL889: _temp888=((
struct Cyc_StructOrUnion_tok_struct*) _temp882)->f1; goto _LL885;} else{ goto
_LL886;} _LL886: goto _LL887; _LL885: return _temp888; _LL887:( int) _throw((
void*)& Cyc_yyfail_StructOrUnion_tok); _LL883:;} static unsigned char _temp891[
13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeQual_tok={
Cyc_Core_Failure,{ _temp891, _temp891, _temp891 + 13u}}; struct Cyc_Absyn_Tqual
Cyc_yyget_TypeQual_tok( void* yy1){ void* _temp892= yy1; struct Cyc_Absyn_Tqual
_temp898; _LL894: if(*(( void**) _temp892) == Cyc_TypeQual_tok){ _LL899:
_temp898=(( struct Cyc_TypeQual_tok_struct*) _temp892)->f1; goto _LL895;} else{
goto _LL896;} _LL896: goto _LL897; _LL895: return _temp898; _LL897:( int) _throw((
void*)& Cyc_yyfail_TypeQual_tok); _LL893:;} static unsigned char _temp901[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp901, _temp901, _temp901 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ void* _temp902= yy1; struct Cyc_List_List* _temp908; _LL904: if(*((
void**) _temp902) == Cyc_StructFieldDeclList_tok){ _LL909: _temp908=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp902)->f1; goto _LL905;} else{ goto
_LL906;} _LL906: goto _LL907; _LL905: return _temp908; _LL907:( int) _throw((
void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL903:;} static unsigned char
_temp911[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp911, _temp911,
_temp911 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp912= yy1; struct Cyc_List_List* _temp918; _LL914: if(*((
void**) _temp912) == Cyc_StructFieldDeclListList_tok){ _LL919: _temp918=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp912)->f1; goto _LL915;}
else{ goto _LL916;} _LL916: goto _LL917; _LL915: return _temp918; _LL917:( int)
_throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL913:;} static
unsigned char _temp921[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp921, _temp921,
_temp921 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp922= yy1; struct Cyc_List_List* _temp928; _LL924: if(*(( void**)
_temp922) == Cyc_TypeModifierList_tok){ _LL929: _temp928=(( struct Cyc_TypeModifierList_tok_struct*)
_temp922)->f1; goto _LL925;} else{ goto _LL926;} _LL926: goto _LL927; _LL925:
return _temp928; _LL927:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL923:;} static unsigned char _temp931[ 15u]="Declarator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp931,
_temp931, _temp931 + 15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ void* _temp932= yy1; struct Cyc_Parse_Declarator* _temp938; _LL934:
if(*(( void**) _temp932) == Cyc_Declarator_tok){ _LL939: _temp938=(( struct Cyc_Declarator_tok_struct*)
_temp932)->f1; goto _LL935;} else{ goto _LL936;} _LL936: goto _LL937; _LL935:
return _temp938; _LL937:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL933:;} static unsigned char _temp941[ 21u]="DeclaratorExpopt_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{
_temp941, _temp941, _temp941 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok(
void* yy1){ void* _temp942= yy1; struct _tuple13* _temp948; _LL944: if(*(( void**)
_temp942) == Cyc_DeclaratorExpopt_tok){ _LL949: _temp948=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp942)->f1; goto _LL945;} else{ goto _LL946;} _LL946: goto _LL947; _LL945:
return _temp948; _LL947:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL943:;} static unsigned char _temp951[ 25u]="DeclaratorExpoptList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp951, _temp951, _temp951 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp952= yy1; struct Cyc_List_List* _temp958; _LL954: if(*((
void**) _temp952) == Cyc_DeclaratorExpoptList_tok){ _LL959: _temp958=(( struct
Cyc_DeclaratorExpoptList_tok_struct*) _temp952)->f1; goto _LL955;} else{ goto
_LL956;} _LL956: goto _LL957; _LL955: return _temp958; _LL957:( int) _throw((
void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL953:;} static unsigned char
_temp961[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp961, _temp961,
_temp961 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp962= yy1; struct Cyc_Parse_Abstractdeclarator* _temp968;
_LL964: if(*(( void**) _temp962) == Cyc_AbstractDeclarator_tok){ _LL969:
_temp968=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp962)->f1; goto
_LL965;} else{ goto _LL966;} _LL966: goto _LL967; _LL965: return _temp968;
_LL967:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL963:;}
static unsigned char _temp971[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp971, _temp971, _temp971 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp972= yy1; int _temp978; _LL974:
if(*(( void**) _temp972) == Cyc_Bool_tok){ _LL979: _temp978=(( struct Cyc_Bool_tok_struct*)
_temp972)->f1; goto _LL975;} else{ goto _LL976;} _LL976: goto _LL977; _LL975:
return _temp978; _LL977:( int) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL973:;}
static unsigned char _temp981[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp981, _temp981, _temp981 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp982= yy1; void* _temp988;
_LL984: if(*(( void**) _temp982) == Cyc_Scope_tok){ _LL989: _temp988=( void*)((
struct Cyc_Scope_tok_struct*) _temp982)->f1; goto _LL985;} else{ goto _LL986;}
_LL986: goto _LL987; _LL985: return _temp988; _LL987:( int) _throw(( void*)& Cyc_yyfail_Scope_tok);
_LL983:;} static unsigned char _temp991[ 16u]="TunionField_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{ _temp991,
_temp991, _temp991 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp992= yy1; struct Cyc_Absyn_Tunionfield* _temp998; _LL994:
if(*(( void**) _temp992) == Cyc_TunionField_tok){ _LL999: _temp998=(( struct Cyc_TunionField_tok_struct*)
_temp992)->f1; goto _LL995;} else{ goto _LL996;} _LL996: goto _LL997; _LL995:
return _temp998; _LL997:( int) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL993:;} static unsigned char _temp1001[ 20u]="TunionFieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,{
_temp1001, _temp1001, _temp1001 + 20u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ void* _temp1002= yy1; struct Cyc_List_List* _temp1008; _LL1004: if(*((
void**) _temp1002) == Cyc_TunionFieldList_tok){ _LL1009: _temp1008=(( struct Cyc_TunionFieldList_tok_struct*)
_temp1002)->f1; goto _LL1005;} else{ goto _LL1006;} _LL1006: goto _LL1007;
_LL1005: return _temp1008; _LL1007:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1003:;} static unsigned char _temp1011[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1011, _temp1011, _temp1011 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1012= yy1; struct _tuple18* _temp1018; _LL1014: if(*((
void**) _temp1012) == Cyc_QualSpecList_tok){ _LL1019: _temp1018=(( struct Cyc_QualSpecList_tok_struct*)
_temp1012)->f1; goto _LL1015;} else{ goto _LL1016;} _LL1016: goto _LL1017;
_LL1015: return _temp1018; _LL1017:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1013:;} static unsigned char _temp1021[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1021, _temp1021, _temp1021 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1022= yy1;
struct Cyc_List_List* _temp1028; _LL1024: if(*(( void**) _temp1022) == Cyc_IdList_tok){
_LL1029: _temp1028=(( struct Cyc_IdList_tok_struct*) _temp1022)->f1; goto
_LL1025;} else{ goto _LL1026;} _LL1026: goto _LL1027; _LL1025: return _temp1028;
_LL1027:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1023:;} static
unsigned char _temp1031[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1031, _temp1031, _temp1031 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1032= yy1;
struct _tuple2* _temp1038; _LL1034: if(*(( void**) _temp1032) == Cyc_ParamDecl_tok){
_LL1039: _temp1038=(( struct Cyc_ParamDecl_tok_struct*) _temp1032)->f1; goto
_LL1035;} else{ goto _LL1036;} _LL1036: goto _LL1037; _LL1035: return _temp1038;
_LL1037:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1033:;} static
unsigned char _temp1041[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1041, _temp1041,
_temp1041 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1042= yy1; struct Cyc_List_List* _temp1048; _LL1044: if(*(( void**)
_temp1042) == Cyc_ParamDeclList_tok){ _LL1049: _temp1048=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1042)->f1; goto _LL1045;} else{ goto _LL1046;} _LL1046: goto _LL1047;
_LL1045: return _temp1048; _LL1047:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1043:;} static unsigned char _temp1051[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1051, _temp1051, _temp1051 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1052= yy1; struct _tuple17* _temp1058; _LL1054: if(*((
void**) _temp1052) == Cyc_ParamDeclListBool_tok){ _LL1059: _temp1058=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1052)->f1; goto _LL1055;} else{ goto
_LL1056;} _LL1056: goto _LL1057; _LL1055: return _temp1058; _LL1057:( int)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1053:;} static
unsigned char _temp1061[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1061, _temp1061, _temp1061 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1062=
yy1; struct Cyc_List_List* _temp1068; _LL1064: if(*(( void**) _temp1062) == Cyc_TypeList_tok){
_LL1069: _temp1068=(( struct Cyc_TypeList_tok_struct*) _temp1062)->f1; goto
_LL1065;} else{ goto _LL1066;} _LL1066: goto _LL1067; _LL1065: return _temp1068;
_LL1067:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1063:;} static
unsigned char _temp1071[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1071, _temp1071,
_temp1071 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1072= yy1; struct Cyc_List_List* _temp1078; _LL1074: if(*(( void**)
_temp1072) == Cyc_DesignatorList_tok){ _LL1079: _temp1078=(( struct Cyc_DesignatorList_tok_struct*)
_temp1072)->f1; goto _LL1075;} else{ goto _LL1076;} _LL1076: goto _LL1077;
_LL1075: return _temp1078; _LL1077:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1073:;} static unsigned char _temp1081[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1081,
_temp1081, _temp1081 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1082= yy1; void* _temp1088; _LL1084: if(*(( void**) _temp1082) == Cyc_Designator_tok){
_LL1089: _temp1088=( void*)(( struct Cyc_Designator_tok_struct*) _temp1082)->f1;
goto _LL1085;} else{ goto _LL1086;} _LL1086: goto _LL1087; _LL1085: return
_temp1088; _LL1087:( int) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1083:;}
static unsigned char _temp1091[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1091, _temp1091, _temp1091 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1092= yy1; void* _temp1098;
_LL1094: if(*(( void**) _temp1092) == Cyc_Kind_tok){ _LL1099: _temp1098=( void*)((
struct Cyc_Kind_tok_struct*) _temp1092)->f1; goto _LL1095;} else{ goto _LL1096;}
_LL1096: goto _LL1097; _LL1095: return _temp1098; _LL1097:( int) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1093:;} static unsigned char _temp1101[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1101, _temp1101, _temp1101 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1102= yy1; void* _temp1108; _LL1104: if(*(( void**) _temp1102) == Cyc_Type_tok){
_LL1109: _temp1108=( void*)(( struct Cyc_Type_tok_struct*) _temp1102)->f1; goto
_LL1105;} else{ goto _LL1106;} _LL1106: goto _LL1107; _LL1105: return _temp1108;
_LL1107:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1103:;} static
unsigned char _temp1111[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1111, _temp1111,
_temp1111 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1112= yy1; struct Cyc_List_List* _temp1118; _LL1114: if(*(( void**)
_temp1112) == Cyc_AttributeList_tok){ _LL1119: _temp1118=(( struct Cyc_AttributeList_tok_struct*)
_temp1112)->f1; goto _LL1115;} else{ goto _LL1116;} _LL1116: goto _LL1117;
_LL1115: return _temp1118; _LL1117:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1113:;} static unsigned char _temp1121[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1121,
_temp1121, _temp1121 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1122= yy1; void* _temp1128; _LL1124: if(*(( void**) _temp1122) == Cyc_Attribute_tok){
_LL1129: _temp1128=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1122)->f1;
goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127; _LL1125: return
_temp1128; _LL1127:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1123:;}
static unsigned char _temp1131[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1131, _temp1131, _temp1131 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1132= yy1; struct Cyc_Absyn_Enumfield* _temp1138; _LL1134: if(*(( void**)
_temp1132) == Cyc_Enumfield_tok){ _LL1139: _temp1138=(( struct Cyc_Enumfield_tok_struct*)
_temp1132)->f1; goto _LL1135;} else{ goto _LL1136;} _LL1136: goto _LL1137;
_LL1135: return _temp1138; _LL1137:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1133:;} static unsigned char _temp1141[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1141, _temp1141, _temp1141 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1142= yy1; struct Cyc_List_List* _temp1148; _LL1144: if(*((
void**) _temp1142) == Cyc_EnumfieldList_tok){ _LL1149: _temp1148=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1142)->f1; goto _LL1145;} else{ goto _LL1146;} _LL1146: goto _LL1147;
_LL1145: return _temp1148; _LL1147:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1143:;} static unsigned char _temp1151[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1151, _temp1151, _temp1151 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1152= yy1;
struct Cyc_Core_Opt* _temp1158; _LL1154: if(*(( void**) _temp1152) == Cyc_TypeOpt_tok){
_LL1159: _temp1158=(( struct Cyc_TypeOpt_tok_struct*) _temp1152)->f1; goto
_LL1155;} else{ goto _LL1156;} _LL1156: goto _LL1157; _LL1155: return _temp1158;
_LL1157:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1153:;} static
unsigned char _temp1161[ 13u]="Rgnorder_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{ _temp1161, _temp1161, _temp1161 +
13u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok( void* yy1){ void* _temp1162=
yy1; struct Cyc_List_List* _temp1168; _LL1164: if(*(( void**) _temp1162) == Cyc_Rgnorder_tok){
_LL1169: _temp1168=(( struct Cyc_Rgnorder_tok_struct*) _temp1162)->f1; goto
_LL1165;} else{ goto _LL1166;} _LL1166: goto _LL1167; _LL1165: return _temp1168;
_LL1167:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1163:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1170;
_temp1170.timestamp= 0; _temp1170.first_line= 0; _temp1170.first_column= 0;
_temp1170.last_line= 0; _temp1170.last_column= 0; _temp1170;});} struct Cyc_Yyltype
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
86, 87, 88, 89, 90, 91}; static unsigned char _temp1171[ 2u]="$"; static
unsigned char _temp1172[ 6u]="error"; static unsigned char _temp1173[ 12u]="$undefined.";
static unsigned char _temp1174[ 5u]="AUTO"; static unsigned char _temp1175[ 9u]="REGISTER";
static unsigned char _temp1176[ 7u]="STATIC"; static unsigned char _temp1177[ 7u]="EXTERN";
static unsigned char _temp1178[ 8u]="TYPEDEF"; static unsigned char _temp1179[ 5u]="VOID";
static unsigned char _temp1180[ 5u]="CHAR"; static unsigned char _temp1181[ 6u]="SHORT";
static unsigned char _temp1182[ 4u]="INT"; static unsigned char _temp1183[ 5u]="LONG";
static unsigned char _temp1184[ 6u]="FLOAT"; static unsigned char _temp1185[ 7u]="DOUBLE";
static unsigned char _temp1186[ 7u]="SIGNED"; static unsigned char _temp1187[ 9u]="UNSIGNED";
static unsigned char _temp1188[ 6u]="CONST"; static unsigned char _temp1189[ 9u]="VOLATILE";
static unsigned char _temp1190[ 9u]="RESTRICT"; static unsigned char _temp1191[
7u]="STRUCT"; static unsigned char _temp1192[ 6u]="UNION"; static unsigned char
_temp1193[ 5u]="CASE"; static unsigned char _temp1194[ 8u]="DEFAULT"; static
unsigned char _temp1195[ 7u]="INLINE"; static unsigned char _temp1196[ 7u]="SIZEOF";
static unsigned char _temp1197[ 9u]="OFFSETOF"; static unsigned char _temp1198[
3u]="IF"; static unsigned char _temp1199[ 5u]="ELSE"; static unsigned char
_temp1200[ 7u]="SWITCH"; static unsigned char _temp1201[ 6u]="WHILE"; static
unsigned char _temp1202[ 3u]="DO"; static unsigned char _temp1203[ 4u]="FOR";
static unsigned char _temp1204[ 5u]="GOTO"; static unsigned char _temp1205[ 9u]="CONTINUE";
static unsigned char _temp1206[ 6u]="BREAK"; static unsigned char _temp1207[ 7u]="RETURN";
static unsigned char _temp1208[ 5u]="ENUM"; static unsigned char _temp1209[ 8u]="NULL_kw";
static unsigned char _temp1210[ 4u]="LET"; static unsigned char _temp1211[ 6u]="THROW";
static unsigned char _temp1212[ 4u]="TRY"; static unsigned char _temp1213[ 6u]="CATCH";
static unsigned char _temp1214[ 4u]="NEW"; static unsigned char _temp1215[ 9u]="ABSTRACT";
static unsigned char _temp1216[ 9u]="FALLTHRU"; static unsigned char _temp1217[
6u]="USING"; static unsigned char _temp1218[ 10u]="NAMESPACE"; static
unsigned char _temp1219[ 7u]="TUNION"; static unsigned char _temp1220[ 8u]="XTUNION";
static unsigned char _temp1221[ 5u]="FILL"; static unsigned char _temp1222[ 8u]="CODEGEN";
static unsigned char _temp1223[ 4u]="CUT"; static unsigned char _temp1224[ 7u]="SPLICE";
static unsigned char _temp1225[ 7u]="MALLOC"; static unsigned char _temp1226[ 9u]="REGION_T";
static unsigned char _temp1227[ 7u]="REGION"; static unsigned char _temp1228[ 5u]="RNEW";
static unsigned char _temp1229[ 8u]="RMALLOC"; static unsigned char _temp1230[ 7u]="PTR_OP";
static unsigned char _temp1231[ 7u]="INC_OP"; static unsigned char _temp1232[ 7u]="DEC_OP";
static unsigned char _temp1233[ 8u]="LEFT_OP"; static unsigned char _temp1234[ 9u]="RIGHT_OP";
static unsigned char _temp1235[ 6u]="LE_OP"; static unsigned char _temp1236[ 6u]="GE_OP";
static unsigned char _temp1237[ 6u]="EQ_OP"; static unsigned char _temp1238[ 6u]="NE_OP";
static unsigned char _temp1239[ 7u]="AND_OP"; static unsigned char _temp1240[ 6u]="OR_OP";
static unsigned char _temp1241[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1242[ 11u]="DIV_ASSIGN"; static unsigned char _temp1243[ 11u]="MOD_ASSIGN";
static unsigned char _temp1244[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1245[ 11u]="SUB_ASSIGN"; static unsigned char _temp1246[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1247[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1248[ 11u]="AND_ASSIGN"; static unsigned char _temp1249[ 11u]="XOR_ASSIGN";
static unsigned char _temp1250[ 10u]="OR_ASSIGN"; static unsigned char _temp1251[
9u]="ELLIPSIS"; static unsigned char _temp1252[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1253[ 12u]="COLON_COLON"; static unsigned char _temp1254[ 11u]="IDENTIFIER";
static unsigned char _temp1255[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1256[ 7u]="STRING"; static unsigned char _temp1257[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1258[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1259[ 9u]="TYPE_VAR"; static unsigned char _temp1260[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1261[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1262[ 10u]="ATTRIBUTE"; static unsigned char _temp1263[ 4u]="';'"; static
unsigned char _temp1264[ 4u]="'{'"; static unsigned char _temp1265[ 4u]="'}'";
static unsigned char _temp1266[ 4u]="'='"; static unsigned char _temp1267[ 4u]="'('";
static unsigned char _temp1268[ 4u]="')'"; static unsigned char _temp1269[ 4u]="','";
static unsigned char _temp1270[ 4u]="'_'"; static unsigned char _temp1271[ 4u]="'$'";
static unsigned char _temp1272[ 4u]="'<'"; static unsigned char _temp1273[ 4u]="'>'";
static unsigned char _temp1274[ 4u]="':'"; static unsigned char _temp1275[ 4u]="'.'";
static unsigned char _temp1276[ 4u]="'['"; static unsigned char _temp1277[ 4u]="']'";
static unsigned char _temp1278[ 4u]="'*'"; static unsigned char _temp1279[ 4u]="'@'";
static unsigned char _temp1280[ 4u]="'?'"; static unsigned char _temp1281[ 4u]="'+'";
static unsigned char _temp1282[ 4u]="'-'"; static unsigned char _temp1283[ 4u]="'&'";
static unsigned char _temp1284[ 4u]="'|'"; static unsigned char _temp1285[ 4u]="'^'";
static unsigned char _temp1286[ 4u]="'/'"; static unsigned char _temp1287[ 4u]="'%'";
static unsigned char _temp1288[ 4u]="'~'"; static unsigned char _temp1289[ 4u]="'!'";
static unsigned char _temp1290[ 5u]="prog"; static unsigned char _temp1291[ 17u]="translation_unit";
static unsigned char _temp1292[ 21u]="external_declaration"; static
unsigned char _temp1293[ 20u]="function_definition"; static unsigned char
_temp1294[ 21u]="function_definition2"; static unsigned char _temp1295[ 13u]="using_action";
static unsigned char _temp1296[ 15u]="unusing_action"; static unsigned char
_temp1297[ 17u]="namespace_action"; static unsigned char _temp1298[ 19u]="unnamespace_action";
static unsigned char _temp1299[ 12u]="declaration"; static unsigned char
_temp1300[ 17u]="declaration_list"; static unsigned char _temp1301[ 23u]="declaration_specifiers";
static unsigned char _temp1302[ 24u]="storage_class_specifier"; static
unsigned char _temp1303[ 15u]="attributes_opt"; static unsigned char _temp1304[
11u]="attributes"; static unsigned char _temp1305[ 15u]="attribute_list"; static
unsigned char _temp1306[ 10u]="attribute"; static unsigned char _temp1307[ 15u]="type_specifier";
static unsigned char _temp1308[ 5u]="kind"; static unsigned char _temp1309[ 15u]="type_qualifier";
static unsigned char _temp1310[ 15u]="enum_specifier"; static unsigned char
_temp1311[ 11u]="enum_field"; static unsigned char _temp1312[ 22u]="enum_declaration_list";
static unsigned char _temp1313[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1314[ 16u]="type_params_opt"; static unsigned char _temp1315[
16u]="struct_or_union"; static unsigned char _temp1316[ 24u]="struct_declaration_list";
static unsigned char _temp1317[ 25u]="struct_declaration_list0"; static
unsigned char _temp1318[ 21u]="init_declarator_list"; static unsigned char
_temp1319[ 22u]="init_declarator_list0"; static unsigned char _temp1320[ 16u]="init_declarator";
static unsigned char _temp1321[ 19u]="struct_declaration"; static unsigned char
_temp1322[ 25u]="specifier_qualifier_list"; static unsigned char _temp1323[ 23u]="struct_declarator_list";
static unsigned char _temp1324[ 24u]="struct_declarator_list0"; static
unsigned char _temp1325[ 18u]="struct_declarator"; static unsigned char
_temp1326[ 17u]="tunion_specifier"; static unsigned char _temp1327[ 18u]="tunion_or_xtunion";
static unsigned char _temp1328[ 17u]="tunionfield_list"; static unsigned char
_temp1329[ 18u]="tunionfield_scope"; static unsigned char _temp1330[ 12u]="tunionfield";
static unsigned char _temp1331[ 11u]="declarator"; static unsigned char
_temp1332[ 18u]="direct_declarator"; static unsigned char _temp1333[ 8u]="pointer";
static unsigned char _temp1334[ 13u]="pointer_char"; static unsigned char
_temp1335[ 8u]="rgn_opt"; static unsigned char _temp1336[ 4u]="rgn"; static
unsigned char _temp1337[ 20u]="type_qualifier_list"; static unsigned char
_temp1338[ 20u]="parameter_type_list"; static unsigned char _temp1339[ 9u]="type_var";
static unsigned char _temp1340[ 16u]="optional_effect"; static unsigned char
_temp1341[ 19u]="optional_rgn_order"; static unsigned char _temp1342[ 10u]="rgn_order";
static unsigned char _temp1343[ 16u]="optional_inject"; static unsigned char
_temp1344[ 11u]="effect_set"; static unsigned char _temp1345[ 14u]="atomic_effect";
static unsigned char _temp1346[ 11u]="region_set"; static unsigned char
_temp1347[ 15u]="parameter_list"; static unsigned char _temp1348[ 22u]="parameter_declaration";
static unsigned char _temp1349[ 16u]="identifier_list"; static unsigned char
_temp1350[ 17u]="identifier_list0"; static unsigned char _temp1351[ 12u]="initializer";
static unsigned char _temp1352[ 18u]="array_initializer"; static unsigned char
_temp1353[ 17u]="initializer_list"; static unsigned char _temp1354[ 12u]="designation";
static unsigned char _temp1355[ 16u]="designator_list"; static unsigned char
_temp1356[ 11u]="designator"; static unsigned char _temp1357[ 10u]="type_name";
static unsigned char _temp1358[ 14u]="any_type_name"; static unsigned char
_temp1359[ 15u]="type_name_list"; static unsigned char _temp1360[ 20u]="abstract_declarator";
static unsigned char _temp1361[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1362[ 10u]="statement"; static unsigned char _temp1363[ 18u]="labeled_statement";
static unsigned char _temp1364[ 21u]="expression_statement"; static
unsigned char _temp1365[ 19u]="compound_statement"; static unsigned char
_temp1366[ 16u]="block_item_list"; static unsigned char _temp1367[ 20u]="selection_statement";
static unsigned char _temp1368[ 15u]="switch_clauses"; static unsigned char
_temp1369[ 16u]="switchC_clauses"; static unsigned char _temp1370[ 20u]="iteration_statement";
static unsigned char _temp1371[ 15u]="jump_statement"; static unsigned char
_temp1372[ 8u]="pattern"; static unsigned char _temp1373[ 19u]="tuple_pattern_list";
static unsigned char _temp1374[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1375[ 14u]="field_pattern"; static unsigned char _temp1376[ 19u]="field_pattern_list";
static unsigned char _temp1377[ 20u]="field_pattern_list0"; static unsigned char
_temp1378[ 11u]="expression"; static unsigned char _temp1379[ 22u]="assignment_expression";
static unsigned char _temp1380[ 20u]="assignment_operator"; static unsigned char
_temp1381[ 23u]="conditional_expression"; static unsigned char _temp1382[ 20u]="constant_expression";
static unsigned char _temp1383[ 22u]="logical_or_expression"; static
unsigned char _temp1384[ 23u]="logical_and_expression"; static unsigned char
_temp1385[ 24u]="inclusive_or_expression"; static unsigned char _temp1386[ 24u]="exclusive_or_expression";
static unsigned char _temp1387[ 15u]="and_expression"; static unsigned char
_temp1388[ 20u]="equality_expression"; static unsigned char _temp1389[ 22u]="relational_expression";
static unsigned char _temp1390[ 17u]="shift_expression"; static unsigned char
_temp1391[ 20u]="additive_expression"; static unsigned char _temp1392[ 26u]="multiplicative_expression";
static unsigned char _temp1393[ 16u]="cast_expression"; static unsigned char
_temp1394[ 17u]="unary_expression"; static unsigned char _temp1395[ 15u]="unary_operator";
static unsigned char _temp1396[ 19u]="postfix_expression"; static unsigned char
_temp1397[ 19u]="primary_expression"; static unsigned char _temp1398[ 25u]="argument_expression_list";
static unsigned char _temp1399[ 26u]="argument_expression_list0"; static
unsigned char _temp1400[ 9u]="constant"; static unsigned char _temp1401[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 231u]={{ _temp1171, _temp1171, _temp1171
+ 2u},{ _temp1172, _temp1172, _temp1172 + 6u},{ _temp1173, _temp1173, _temp1173
+ 12u},{ _temp1174, _temp1174, _temp1174 + 5u},{ _temp1175, _temp1175, _temp1175
+ 9u},{ _temp1176, _temp1176, _temp1176 + 7u},{ _temp1177, _temp1177, _temp1177
+ 7u},{ _temp1178, _temp1178, _temp1178 + 8u},{ _temp1179, _temp1179, _temp1179
+ 5u},{ _temp1180, _temp1180, _temp1180 + 5u},{ _temp1181, _temp1181, _temp1181
+ 6u},{ _temp1182, _temp1182, _temp1182 + 4u},{ _temp1183, _temp1183, _temp1183
+ 5u},{ _temp1184, _temp1184, _temp1184 + 6u},{ _temp1185, _temp1185, _temp1185
+ 7u},{ _temp1186, _temp1186, _temp1186 + 7u},{ _temp1187, _temp1187, _temp1187
+ 9u},{ _temp1188, _temp1188, _temp1188 + 6u},{ _temp1189, _temp1189, _temp1189
+ 9u},{ _temp1190, _temp1190, _temp1190 + 9u},{ _temp1191, _temp1191, _temp1191
+ 7u},{ _temp1192, _temp1192, _temp1192 + 6u},{ _temp1193, _temp1193, _temp1193
+ 5u},{ _temp1194, _temp1194, _temp1194 + 8u},{ _temp1195, _temp1195, _temp1195
+ 7u},{ _temp1196, _temp1196, _temp1196 + 7u},{ _temp1197, _temp1197, _temp1197
+ 9u},{ _temp1198, _temp1198, _temp1198 + 3u},{ _temp1199, _temp1199, _temp1199
+ 5u},{ _temp1200, _temp1200, _temp1200 + 7u},{ _temp1201, _temp1201, _temp1201
+ 6u},{ _temp1202, _temp1202, _temp1202 + 3u},{ _temp1203, _temp1203, _temp1203
+ 4u},{ _temp1204, _temp1204, _temp1204 + 5u},{ _temp1205, _temp1205, _temp1205
+ 9u},{ _temp1206, _temp1206, _temp1206 + 6u},{ _temp1207, _temp1207, _temp1207
+ 7u},{ _temp1208, _temp1208, _temp1208 + 5u},{ _temp1209, _temp1209, _temp1209
+ 8u},{ _temp1210, _temp1210, _temp1210 + 4u},{ _temp1211, _temp1211, _temp1211
+ 6u},{ _temp1212, _temp1212, _temp1212 + 4u},{ _temp1213, _temp1213, _temp1213
+ 6u},{ _temp1214, _temp1214, _temp1214 + 4u},{ _temp1215, _temp1215, _temp1215
+ 9u},{ _temp1216, _temp1216, _temp1216 + 9u},{ _temp1217, _temp1217, _temp1217
+ 6u},{ _temp1218, _temp1218, _temp1218 + 10u},{ _temp1219, _temp1219, _temp1219
+ 7u},{ _temp1220, _temp1220, _temp1220 + 8u},{ _temp1221, _temp1221, _temp1221
+ 5u},{ _temp1222, _temp1222, _temp1222 + 8u},{ _temp1223, _temp1223, _temp1223
+ 4u},{ _temp1224, _temp1224, _temp1224 + 7u},{ _temp1225, _temp1225, _temp1225
+ 7u},{ _temp1226, _temp1226, _temp1226 + 9u},{ _temp1227, _temp1227, _temp1227
+ 7u},{ _temp1228, _temp1228, _temp1228 + 5u},{ _temp1229, _temp1229, _temp1229
+ 8u},{ _temp1230, _temp1230, _temp1230 + 7u},{ _temp1231, _temp1231, _temp1231
+ 7u},{ _temp1232, _temp1232, _temp1232 + 7u},{ _temp1233, _temp1233, _temp1233
+ 8u},{ _temp1234, _temp1234, _temp1234 + 9u},{ _temp1235, _temp1235, _temp1235
+ 6u},{ _temp1236, _temp1236, _temp1236 + 6u},{ _temp1237, _temp1237, _temp1237
+ 6u},{ _temp1238, _temp1238, _temp1238 + 6u},{ _temp1239, _temp1239, _temp1239
+ 7u},{ _temp1240, _temp1240, _temp1240 + 6u},{ _temp1241, _temp1241, _temp1241
+ 11u},{ _temp1242, _temp1242, _temp1242 + 11u},{ _temp1243, _temp1243,
_temp1243 + 11u},{ _temp1244, _temp1244, _temp1244 + 11u},{ _temp1245, _temp1245,
_temp1245 + 11u},{ _temp1246, _temp1246, _temp1246 + 12u},{ _temp1247, _temp1247,
_temp1247 + 13u},{ _temp1248, _temp1248, _temp1248 + 11u},{ _temp1249, _temp1249,
_temp1249 + 11u},{ _temp1250, _temp1250, _temp1250 + 10u},{ _temp1251, _temp1251,
_temp1251 + 9u},{ _temp1252, _temp1252, _temp1252 + 11u},{ _temp1253, _temp1253,
_temp1253 + 12u},{ _temp1254, _temp1254, _temp1254 + 11u},{ _temp1255, _temp1255,
_temp1255 + 17u},{ _temp1256, _temp1256, _temp1256 + 7u},{ _temp1257, _temp1257,
_temp1257 + 19u},{ _temp1258, _temp1258, _temp1258 + 18u},{ _temp1259, _temp1259,
_temp1259 + 9u},{ _temp1260, _temp1260, _temp1260 + 16u},{ _temp1261, _temp1261,
_temp1261 + 18u},{ _temp1262, _temp1262, _temp1262 + 10u},{ _temp1263, _temp1263,
_temp1263 + 4u},{ _temp1264, _temp1264, _temp1264 + 4u},{ _temp1265, _temp1265,
_temp1265 + 4u},{ _temp1266, _temp1266, _temp1266 + 4u},{ _temp1267, _temp1267,
_temp1267 + 4u},{ _temp1268, _temp1268, _temp1268 + 4u},{ _temp1269, _temp1269,
_temp1269 + 4u},{ _temp1270, _temp1270, _temp1270 + 4u},{ _temp1271, _temp1271,
_temp1271 + 4u},{ _temp1272, _temp1272, _temp1272 + 4u},{ _temp1273, _temp1273,
_temp1273 + 4u},{ _temp1274, _temp1274, _temp1274 + 4u},{ _temp1275, _temp1275,
_temp1275 + 4u},{ _temp1276, _temp1276, _temp1276 + 4u},{ _temp1277, _temp1277,
_temp1277 + 4u},{ _temp1278, _temp1278, _temp1278 + 4u},{ _temp1279, _temp1279,
_temp1279 + 4u},{ _temp1280, _temp1280, _temp1280 + 4u},{ _temp1281, _temp1281,
_temp1281 + 4u},{ _temp1282, _temp1282, _temp1282 + 4u},{ _temp1283, _temp1283,
_temp1283 + 4u},{ _temp1284, _temp1284, _temp1284 + 4u},{ _temp1285, _temp1285,
_temp1285 + 4u},{ _temp1286, _temp1286, _temp1286 + 4u},{ _temp1287, _temp1287,
_temp1287 + 4u},{ _temp1288, _temp1288, _temp1288 + 4u},{ _temp1289, _temp1289,
_temp1289 + 4u},{ _temp1290, _temp1290, _temp1290 + 5u},{ _temp1291, _temp1291,
_temp1291 + 17u},{ _temp1292, _temp1292, _temp1292 + 21u},{ _temp1293, _temp1293,
_temp1293 + 20u},{ _temp1294, _temp1294, _temp1294 + 21u},{ _temp1295, _temp1295,
_temp1295 + 13u},{ _temp1296, _temp1296, _temp1296 + 15u},{ _temp1297, _temp1297,
_temp1297 + 17u},{ _temp1298, _temp1298, _temp1298 + 19u},{ _temp1299, _temp1299,
_temp1299 + 12u},{ _temp1300, _temp1300, _temp1300 + 17u},{ _temp1301, _temp1301,
_temp1301 + 23u},{ _temp1302, _temp1302, _temp1302 + 24u},{ _temp1303, _temp1303,
_temp1303 + 15u},{ _temp1304, _temp1304, _temp1304 + 11u},{ _temp1305, _temp1305,
_temp1305 + 15u},{ _temp1306, _temp1306, _temp1306 + 10u},{ _temp1307, _temp1307,
_temp1307 + 15u},{ _temp1308, _temp1308, _temp1308 + 5u},{ _temp1309, _temp1309,
_temp1309 + 15u},{ _temp1310, _temp1310, _temp1310 + 15u},{ _temp1311, _temp1311,
_temp1311 + 11u},{ _temp1312, _temp1312, _temp1312 + 22u},{ _temp1313, _temp1313,
_temp1313 + 26u},{ _temp1314, _temp1314, _temp1314 + 16u},{ _temp1315, _temp1315,
_temp1315 + 16u},{ _temp1316, _temp1316, _temp1316 + 24u},{ _temp1317, _temp1317,
_temp1317 + 25u},{ _temp1318, _temp1318, _temp1318 + 21u},{ _temp1319, _temp1319,
_temp1319 + 22u},{ _temp1320, _temp1320, _temp1320 + 16u},{ _temp1321, _temp1321,
_temp1321 + 19u},{ _temp1322, _temp1322, _temp1322 + 25u},{ _temp1323, _temp1323,
_temp1323 + 23u},{ _temp1324, _temp1324, _temp1324 + 24u},{ _temp1325, _temp1325,
_temp1325 + 18u},{ _temp1326, _temp1326, _temp1326 + 17u},{ _temp1327, _temp1327,
_temp1327 + 18u},{ _temp1328, _temp1328, _temp1328 + 17u},{ _temp1329, _temp1329,
_temp1329 + 18u},{ _temp1330, _temp1330, _temp1330 + 12u},{ _temp1331, _temp1331,
_temp1331 + 11u},{ _temp1332, _temp1332, _temp1332 + 18u},{ _temp1333, _temp1333,
_temp1333 + 8u},{ _temp1334, _temp1334, _temp1334 + 13u},{ _temp1335, _temp1335,
_temp1335 + 8u},{ _temp1336, _temp1336, _temp1336 + 4u},{ _temp1337, _temp1337,
_temp1337 + 20u},{ _temp1338, _temp1338, _temp1338 + 20u},{ _temp1339, _temp1339,
_temp1339 + 9u},{ _temp1340, _temp1340, _temp1340 + 16u},{ _temp1341, _temp1341,
_temp1341 + 19u},{ _temp1342, _temp1342, _temp1342 + 10u},{ _temp1343, _temp1343,
_temp1343 + 16u},{ _temp1344, _temp1344, _temp1344 + 11u},{ _temp1345, _temp1345,
_temp1345 + 14u},{ _temp1346, _temp1346, _temp1346 + 11u},{ _temp1347, _temp1347,
_temp1347 + 15u},{ _temp1348, _temp1348, _temp1348 + 22u},{ _temp1349, _temp1349,
_temp1349 + 16u},{ _temp1350, _temp1350, _temp1350 + 17u},{ _temp1351, _temp1351,
_temp1351 + 12u},{ _temp1352, _temp1352, _temp1352 + 18u},{ _temp1353, _temp1353,
_temp1353 + 17u},{ _temp1354, _temp1354, _temp1354 + 12u},{ _temp1355, _temp1355,
_temp1355 + 16u},{ _temp1356, _temp1356, _temp1356 + 11u},{ _temp1357, _temp1357,
_temp1357 + 10u},{ _temp1358, _temp1358, _temp1358 + 14u},{ _temp1359, _temp1359,
_temp1359 + 15u},{ _temp1360, _temp1360, _temp1360 + 20u},{ _temp1361, _temp1361,
_temp1361 + 27u},{ _temp1362, _temp1362, _temp1362 + 10u},{ _temp1363, _temp1363,
_temp1363 + 18u},{ _temp1364, _temp1364, _temp1364 + 21u},{ _temp1365, _temp1365,
_temp1365 + 19u},{ _temp1366, _temp1366, _temp1366 + 16u},{ _temp1367, _temp1367,
_temp1367 + 20u},{ _temp1368, _temp1368, _temp1368 + 15u},{ _temp1369, _temp1369,
_temp1369 + 16u},{ _temp1370, _temp1370, _temp1370 + 20u},{ _temp1371, _temp1371,
_temp1371 + 15u},{ _temp1372, _temp1372, _temp1372 + 8u},{ _temp1373, _temp1373,
_temp1373 + 19u},{ _temp1374, _temp1374, _temp1374 + 20u},{ _temp1375, _temp1375,
_temp1375 + 14u},{ _temp1376, _temp1376, _temp1376 + 19u},{ _temp1377, _temp1377,
_temp1377 + 20u},{ _temp1378, _temp1378, _temp1378 + 11u},{ _temp1379, _temp1379,
_temp1379 + 22u},{ _temp1380, _temp1380, _temp1380 + 20u},{ _temp1381, _temp1381,
_temp1381 + 23u},{ _temp1382, _temp1382, _temp1382 + 20u},{ _temp1383, _temp1383,
_temp1383 + 22u},{ _temp1384, _temp1384, _temp1384 + 23u},{ _temp1385, _temp1385,
_temp1385 + 24u},{ _temp1386, _temp1386, _temp1386 + 24u},{ _temp1387, _temp1387,
_temp1387 + 15u},{ _temp1388, _temp1388, _temp1388 + 20u},{ _temp1389, _temp1389,
_temp1389 + 22u},{ _temp1390, _temp1390, _temp1390 + 17u},{ _temp1391, _temp1391,
_temp1391 + 20u},{ _temp1392, _temp1392, _temp1392 + 26u},{ _temp1393, _temp1393,
_temp1393 + 16u},{ _temp1394, _temp1394, _temp1394 + 17u},{ _temp1395, _temp1395,
_temp1395 + 15u},{ _temp1396, _temp1396, _temp1396 + 19u},{ _temp1397, _temp1397,
_temp1397 + 19u},{ _temp1398, _temp1398, _temp1398 + 25u},{ _temp1399, _temp1399,
_temp1399 + 26u},{ _temp1400, _temp1400, _temp1400 + 9u},{ _temp1401, _temp1401,
_temp1401 + 20u}}; static short Cyc_yyr1[ 401u]={ 0, 119, 120, 120, 120, 120,
120, 120, 120, 121, 121, 122, 122, 122, 122, 123, 123, 124, 125, 126, 127, 128,
128, 128, 128, 129, 129, 130, 130, 130, 130, 130, 130, 130, 130, 131, 131, 131,
131, 131, 131, 131, 132, 132, 133, 134, 134, 135, 135, 135, 135, 135, 136, 136,
136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 137,
137, 138, 138, 138, 139, 139, 140, 140, 141, 141, 142, 142, 142, 142, 142, 143,
143, 143, 144, 144, 145, 146, 146, 147, 148, 148, 149, 149, 150, 151, 151, 151,
151, 152, 153, 153, 154, 154, 154, 155, 155, 155, 155, 156, 156, 157, 157, 157,
157, 158, 158, 158, 159, 159, 160, 160, 161, 161, 161, 161, 161, 161, 161, 161,
161, 161, 161, 161, 162, 162, 162, 162, 163, 163, 163, 163, 163, 164, 164, 165,
165, 166, 166, 167, 167, 167, 167, 168, 168, 169, 169, 170, 170, 171, 171, 172,
172, 173, 173, 174, 174, 174, 175, 175, 176, 176, 177, 177, 177, 178, 179, 179,
180, 180, 181, 181, 181, 181, 182, 182, 182, 182, 183, 184, 184, 185, 185, 186,
186, 187, 187, 187, 187, 188, 188, 189, 189, 189, 190, 190, 190, 190, 190, 190,
190, 190, 190, 190, 190, 190, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191,
192, 193, 193, 194, 194, 195, 195, 195, 195, 195, 195, 196, 196, 196, 196, 196,
197, 197, 197, 197, 197, 197, 198, 198, 198, 198, 199, 199, 199, 199, 199, 199,
199, 199, 199, 199, 199, 199, 199, 199, 200, 200, 200, 200, 200, 200, 200, 200,
201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 202, 202,
203, 203, 204, 204, 205, 206, 206, 207, 207, 208, 208, 209, 209, 209, 209, 209,
209, 209, 209, 209, 209, 209, 210, 210, 210, 210, 210, 210, 210, 211, 212, 212,
213, 213, 214, 214, 215, 215, 216, 216, 217, 217, 217, 218, 218, 218, 218, 218,
219, 219, 219, 220, 220, 220, 221, 221, 221, 221, 222, 222, 223, 223, 223, 223,
223, 223, 223, 223, 223, 223, 223, 223, 224, 224, 224, 225, 225, 225, 225, 225,
225, 225, 225, 225, 225, 225, 225, 225, 225, 226, 226, 226, 226, 226, 226, 226,
226, 226, 227, 228, 228, 229, 229, 229, 229, 230, 230}; static short Cyc_yyr2[
401u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4, 3, 4, 2, 1, 2, 1, 2, 3, 5,
3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1, 0, 1, 6, 1, 3, 1, 1, 4, 4,
8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 1, 1, 1, 1, 1, 5, 2, 1, 3,
1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3, 1, 3, 3, 2, 3, 2, 3, 1, 1,
3, 1, 2, 3, 6, 4, 3, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2, 6, 1, 2, 1, 3, 3, 4, 4, 5,
4, 4, 4, 2, 2, 1, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1, 1, 1, 2, 3, 5, 6, 8, 1, 3,
0, 2, 0, 2, 3, 5, 0, 1, 1, 3, 2, 3, 1, 1, 3, 1, 3, 2, 1, 2, 1, 1, 3, 1, 1, 2, 3,
4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 3, 1, 3, 1, 1, 2, 3, 2, 3, 3, 4,
4, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1, 2, 2, 3, 1, 2, 1, 2, 1,
2, 5, 7, 7, 8, 6, 0, 3, 4, 5, 6, 7, 0, 3, 4, 5, 5, 7, 6, 7, 7, 8, 7, 8, 8, 9, 6,
7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2, 0, 1,
1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 2, 2,
5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1, 3, 3,
1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 6, 7, 9, 1, 1, 1, 1, 4, 3, 4, 3, 3,
3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4, 5, 1, 1, 3, 1, 1, 1, 1, 1, 1};
static short Cyc_yydefact[ 788u]={ 0, 137, 35, 36, 37, 38, 40, 52, 54, 55, 56,
57, 58, 59, 60, 61, 71, 72, 73, 88, 89, 42, 0, 0, 41, 0, 0, 113, 114, 0, 399,
157, 400, 85, 0, 53, 0, 142, 143, 146, 1, 0, 9, 0, 0, 10, 0, 42, 42, 42, 62, 63,
0, 64, 0, 0, 124, 0, 147, 65, 126, 39, 0, 33, 43, 75, 284, 399, 280, 283, 282, 0,
278, 0, 0, 0, 0, 0, 179, 0, 285, 17, 19, 0, 0, 0, 66, 0, 0, 0, 0, 2, 0, 0, 0, 0,
21, 0, 93, 94, 96, 27, 29, 31, 85, 0, 85, 150, 0, 149, 85, 38, 0, 25, 0, 0, 11,
159, 0, 0, 135, 125, 42, 148, 136, 0, 0, 34, 0, 0, 292, 291, 281, 290, 24, 0, 0,
0, 0, 42, 42, 197, 199, 0, 69, 70, 158, 203, 0, 127, 0, 0, 174, 0, 0, 398, 0, 0,
0, 0, 0, 0, 0, 0, 0, 395, 385, 396, 397, 0, 0, 0, 0, 368, 0, 366, 367, 0, 303,
316, 324, 326, 328, 330, 332, 334, 337, 342, 345, 348, 352, 0, 354, 369, 384,
383, 0, 3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30, 32, 84, 0, 90, 91, 0, 83, 85, 0,
111, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 399, 231, 233, 0, 239, 235, 0,
237, 220, 221, 222, 0, 223, 224, 225, 0, 301, 26, 13, 96, 0, 180, 0, 0, 161, 159,
0, 0, 128, 0, 138, 0, 0, 78, 0, 76, 279, 294, 0, 293, 181, 0, 0, 292, 200, 172,
0, 99, 101, 159, 0, 205, 198, 206, 68, 0, 87, 0, 86, 0, 176, 0, 178, 67, 0, 0,
362, 0, 318, 352, 0, 319, 320, 0, 0, 0, 0, 0, 0, 355, 356, 0, 0, 0, 0, 358, 359,
357, 144, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 306, 307, 308,
309, 310, 311, 312, 313, 314, 315, 305, 0, 360, 0, 377, 378, 0, 0, 0, 387, 0, 0,
145, 18, 0, 20, 0, 95, 97, 183, 182, 14, 0, 80, 92, 0, 0, 103, 104, 106, 0, 110,
85, 119, 0, 0, 0, 0, 0, 0, 0, 271, 272, 273, 0, 0, 275, 0, 228, 229, 0, 0, 0, 0,
240, 236, 96, 238, 234, 232, 0, 165, 0, 171, 160, 167, 130, 0, 0, 0, 161, 132,
134, 133, 129, 151, 140, 139, 0, 48, 47, 0, 45, 0, 74, 0, 287, 0, 23, 288, 0, 0,
0, 0, 193, 296, 299, 0, 298, 0, 0, 201, 100, 102, 0, 161, 0, 209, 0, 207, 159, 0,
0, 219, 202, 204, 175, 0, 0, 0, 184, 188, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 386,
393, 0, 392, 325, 0, 327, 329, 331, 333, 335, 336, 340, 341, 338, 339, 343, 344,
346, 347, 349, 350, 351, 304, 375, 376, 371, 0, 373, 374, 0, 0, 0, 4, 6, 0, 323,
107, 98, 0, 0, 0, 112, 121, 120, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 270, 274, 0,
0, 0, 227, 0, 230, 0, 15, 302, 166, 0, 169, 0, 0, 0, 162, 131, 159, 153, 152,
141, 7, 0, 0, 0, 79, 77, 295, 196, 0, 297, 192, 194, 289, 0, 286, 173, 214, 0,
208, 211, 0, 161, 0, 210, 0, 361, 0, 0, 185, 0, 189, 381, 382, 0, 0, 0, 0, 0, 0,
0, 353, 389, 0, 0, 372, 370, 390, 0, 0, 82, 105, 108, 81, 109, 122, 119, 119, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 246, 276, 0, 0, 16, 159, 170, 168, 0, 165, 161, 0, 0,
0, 44, 46, 195, 300, 213, 216, 0, 218, 217, 212, 0, 0, 186, 190, 0, 0, 321, 322,
0, 391, 0, 394, 317, 388, 0, 118, 117, 241, 0, 246, 256, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 277, 0, 161, 163, 0, 154, 0, 49, 50, 215, 363, 0, 191, 0, 0, 379, 0,
0, 0, 252, 0, 0, 258, 0, 0, 0, 266, 0, 0, 0, 0, 0, 0, 0, 0, 245, 226, 155, 0,
159, 0, 0, 364, 0, 380, 0, 242, 0, 0, 0, 243, 257, 259, 260, 0, 268, 267, 0, 262,
0, 0, 0, 0, 246, 247, 164, 161, 0, 0, 0, 123, 0, 0, 244, 261, 269, 263, 264, 0,
0, 246, 248, 156, 0, 187, 365, 252, 253, 265, 246, 249, 51, 252, 254, 246, 250,
255, 251, 0, 0, 0}; static short Cyc_yydefgoto[ 112u]={ 785, 40, 41, 42, 237, 43,
365, 44, 367, 45, 204, 46, 47, 63, 64, 432, 433, 48, 146, 49, 50, 266, 267, 51,
86, 52, 210, 211, 97, 98, 99, 212, 150, 377, 378, 379, 53, 54, 529, 530, 531, 55,
56, 57, 58, 122, 108, 428, 455, 59, 456, 419, 557, 552, 415, 416, 279, 258, 152,
77, 78, 472, 370, 473, 474, 444, 445, 142, 147, 148, 457, 286, 240, 241, 242,
243, 244, 245, 686, 736, 246, 247, 270, 271, 272, 447, 448, 449, 248, 249, 352,
178, 521, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
193, 486, 487, 194, 195}; static short Cyc_yypact[ 788u]={ 1867, - -32768, -
-32768, - -32768, - -32768, - 62, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, - 31, 242, 2798, - -32768, 242, - 47, - -32768, -
-32768, 14, - -32768, - 10, - -32768, 32, 516, - -32768, 0, 55, 77, - -32768, -
-32768, 1757, - -32768, 173, 300, - -32768, 481, - 31, - 31, - 31, - -32768, -
-32768, 395, - -32768, 109, 2343, 120, 12, - 57, - -32768, 242, 125, 76, 2397, -
-32768, 135, - -32768, 184, - -32768, - -32768, - -32768, 4264, - -32768, 84,
147, 155, 4264, 156, 160, 179, 207, - -32768, - -32768, 1692, 116, 1692, -
-32768, 187, 2025, 3857, 3857, - -32768, 1757, 1976, 1757, 1976, - -32768, 203,
225, - -32768, 846, 2397, 2397, 2397, 32, 2025, 32, - -32768, 242, - -32768, 87,
245, 1184, - -32768, 2343, 481, - -32768, 2285, 1692, 2817, - -32768, 120, - 31,
- -32768, - -32768, 1976, 209, - -32768, 242, 244, 4264, - -32768, - -32768, -
-32768, - -32768, 321, 3857, 114, 254, - 31, - 31, 293, - -32768, 18, - -32768,
- -32768, - -32768, 296, 67, - -32768, 660, 297, - -32768, 4004, 320, - -32768,
3857, 3898, 325, 327, 342, 344, 349, 4059, 4059, - -32768, - -32768, - -32768, -
-32768, 1527, 368, 4100, 4100, - -32768, 4100, - -32768, - -32768, 339, - -32768,
- 29, 388, 364, 366, 374, 455, 94, 461, 418, - 16, - -32768, 606, 4100, 414, -
-32768, - -32768, 86, 396, - -32768, 398, - -32768, 401, - -32768, 516, 2882,
2343, - -32768, - -32768, - -32768, - -32768, 405, 407, 2025, - -32768, 543, 410,
32, 242, 416, - -32768, 419, 28, 430, 2448, 434, 449, 445, 457, 2947, 2448, 191,
2448, 2448, - 14, 451, - -32768, - -32768, 460, 1300, 1300, 481, 1300, - -32768,
- -32768, - -32768, 469, - -32768, - -32768, - -32768, 273, - -32768, - -32768,
- -32768, 484, - 57, - -32768, 174, 472, 473, 274, 485, 73, - -32768, 498, 126,
493, 58, 513, 522, 527, - -32768, - -32768, 537, 535, - -32768, 299, 2175, 4264,
- -32768, 539, 546, 2025, 2025, 2236, 3012, 108, - -32768, 282, - -32768, 174, -
-32768, 1692, - -32768, 2134, - -32768, 172, - -32768, - -32768, 2025, 1527, -
-32768, 2025, - -32768, - -32768, 2513, - -32768, 589, 3857, 2085, 635, 3857,
3857, 1527, - -32768, - -32768, 1300, 567, 438, 3857, - -32768, - -32768, -
-32768, - -32768, 4100, 3857, 4100, 4100, 4100, 4100, 4100, 4100, 4100, 4100,
4100, 4100, 4100, 4100, 4100, 4100, 4100, 4100, 4100, - -32768, - -32768, -
-32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, -
-32768, 3857, - -32768, 139, - -32768, - -32768, 3077, 161, 3857, - -32768, 2741,
564, - -32768, - -32768, 1757, - -32768, 1757, - -32768, - -32768, - -32768, -
-32768, - -32768, 2025, - -32768, - -32768, 3857, 576, 571, - -32768, 568, 2025,
- -32768, 32, 541, 3857, 577, 3857, 3857, 642, 1416, 583, - -32768, - -32768, -
-32768, 313, 645, - -32768, 3142, - -32768, - -32768, 2448, 600, 2448, 1638, -
-32768, - -32768, 846, - -32768, - -32768, - -32768, 3857, 607, 337, - -32768, -
-32768, 581, - -32768, 604, 596, 4225, 473, - -32768, - -32768, - -32768, -
-32768, - -32768, - -32768, 126, 1757, - -32768, 601, 599, 605, 242, - -32768,
3857, - -32768, 4264, - -32768, - -32768, 615, 3857, 4264, 142, - -32768, -
-32768, - -32768, 608, 609, 612, 617, - -32768, - -32768, - -32768, 614, 473,
619, - -32768, 611, 282, 4211, 1692, 3207, - -32768, - -32768, 296, - -32768,
622, 616, 623, - -32768, - -32768, 239, 2882, 468, 626, 516, 628, 470, 629, 631,
618, 3963, - -32768, - -32768, 632, 634, 388, 215, 364, 366, 374, 455, 94, 94,
461, 461, 461, 461, 418, 418, - 16, - 16, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, - -32768, 636, - -32768, - -32768, 56, 270, 1692, - -32768,
- -32768, 640, - -32768, - -32768, - -32768, 543, 3857, 644, - -32768, - -32768,
- -32768, 646, 242, 343, 474, 3857, 477, 487, 643, 3272, 3337, 351, - -32768, -
-32768, 648, 652, 639, - -32768, 649, - -32768, 2343, - -32768, - -32768, -
-32768, 2025, - -32768, 654, 174, 658, - -32768, - -32768, - 9, - -32768, -
-32768, - -32768, - -32768, 266, 656, 58, - -32768, - -32768, - -32768, - -32768,
655, - -32768, - -32768, - -32768, - -32768, 4233, - -32768, - -32768, - -32768,
665, - -32768, - -32768, 667, 473, 79, - -32768, 664, 653, 672, 671, - -32768,
2589, - -32768, - -32768, - -32768, 2343, 2025, 3898, 741, 653, 676, 2741, -
-32768, - -32768, 3857, 3857, - -32768, - -32768, - -32768, 2741, 328, - -32768,
- -32768, - -32768, - -32768, - -32768, 220, 13, 541, 2448, 494, 681, 2448, 3857,
3402, 360, 3467, 362, 3532, 536, - -32768, 683, 695, - -32768, 687, - -32768, -
-32768, 694, 607, 473, 686, 688, 690, - -32768, - -32768, - -32768, - -32768, -
-32768, - -32768, 708, - -32768, - -32768, - -32768, 709, 3857, - -32768, -
-32768, 2882, 711, - -32768, 589, 713, - -32768, 378, - -32768, - -32768, -
-32768, 715, - -32768, - -32768, 786, 722, 536, - -32768, 496, 2448, 499, 3597,
2448, 503, 3662, 3727, 365, 4264, 716, 727, - -32768, 2448, 473, 718, 2025, -
-32768, 739, - -32768, - -32768, - -32768, - -32768, 278, - -32768, 728, 2025, -
-32768, 2665, 2025, 2448, 580, 730, 734, - -32768, 2448, 2448, 510, - -32768,
2448, 2448, 512, 2448, 532, 3792, - 25, 1300, - -32768, - -32768, - -32768, 604,
687, 729, 3857, - -32768, 731, - -32768, 544, - -32768, 3857, 726, 736, - -32768,
- -32768, - -32768, - -32768, 2448, - -32768, - -32768, 2448, - -32768, 2448,
2448, 547, 3857, 946, - -32768, - -32768, 473, 748, 385, 737, - -32768, 281,
1300, - -32768, - -32768, - -32768, - -32768, - -32768, 2448, 305, 536, - -32768,
- -32768, 738, - -32768, - -32768, 1068, - -32768, - -32768, 946, - -32768, -
-32768, 580, - -32768, 536, - -32768, - -32768, - -32768, 833, 836, - -32768};
static short Cyc_yypgoto[ 112u]={ - -32768, 186, - -32768, 530, - -32768, -
-32768, - -32768, - -32768, - -32768, - 51, - 44, - 55, - -32768, 62, - 36, 272,
- -32768, 46, - -32768, 9, - -32768, - -32768, 408, - -32768, - 22, - -32768, -
207, - -32768, - -32768, - -32768, 637, 630, 34, - -32768, - -32768, 323, -
-32768, - -32768, - 5, - -32768, - -32768, 197, - 41, - 73, - -32768, - -32768,
- 53, - -32768, - 92, - 17, - 108, - 367, 118, 205, 290, 559, - 386, - 85, - 259,
751, - -32768, - 164, - 156, - 346, - 222, - -32768, 425, - 143, - 66, - 104, 64,
- 172, 66, - -32768, - -32768, - 48, - 216, - -32768, - 644, - 314, - -32768, -
-32768, - 21, 597, - -32768, 301, - -32768, - -32768, 193, 57, - -32768, - 144,
- 408, - 151, 550, 552, 553, 554, 555, 319, 181, 322, 317, - 98, 464, - -32768,
- -32768, - -32768, - 327, - -32768, - -32768, 10}; static short Cyc_yytable[
4377u]={ 115, 304, 79, 151, 113, 123, 305, 116, 127, 257, 60, 114, 301, 1, 260,
515, 121, 143, 527, 528, 120, 405, 406, 61, 408, 256, 315, 554, 568, 707, 511,
31, 65, 80, 571, 81, 82, 109, 467, 369, 322, 109, 107, 749, 60, 115, 206, 207,
208, 113, 129, 60, 205, 443, 560, 133, 60, 239, 137, 115, 62, 238, 106, 250, 110,
578, 251, 60, 284, 401, 124, 544, 84, 318, 319, 430, 320, 294, 750, 31, 323, 80,
209, 255, 214, 120, 80, 402, 217, 580, 107, 338, 140, 353, 140, 30, 88, 140, 482,
339, 340, 32, 60, 60, 60, 60, 768, - 116, 34, 101, 102, 103, 460, 386, 140, 83,
614, 141, 215, 141, 287, 278, 460, 777, 387, 60, 140, 140, 288, 139, 289, 139,
782, 85, 139, 60, 423, 784, 268, 213, 80, 431, 651, 16, 17, 18, 177, 196, 89,
115, 421, 139, 141, 250, 411, 468, 372, 469, 330, 331, 60, 467, 608, 139, 139,
290, 519, 360, 481, 291, 90, 290, 126, 1, 525, 424, 262, 290, 140, 361, 130, 652,
239, 239, 263, 239, 238, 238, 85, 238, 427, 216, 30, 382, 362, 332, 333, 31, 32,
144, 412, 280, 281, 141, 282, 285, 145, 275, 107, 284, 276, 62, 60, 283, 295,
139, 117, 650, 125, 294, 140, 118, 508, 60, 466, 119, 383, 91, 128, 509, 131, 87,
520, 37, 38, 39, 109, 573, 414, 132, 504, 505, 506, 100, 512, 213, 441, 442, 134,
60, 464, 513, 477, 121, 446, 30, 664, 139, 135, 239, 371, 32, 31, 238, - 205, 92,
93, 413, 292, - 205, - 205, 414, 426, 692, 136, - 205, - 180, 283, 197, 198, 199,
200, - 180, 397, 149, 80, 80, 398, 389, 140, 140, 140, 520, 635, 396, 201, 399,
400, 520, 140, - 85, 140, 60, - 85, 60, 265, 140, 140, 85, 140, 593, 264, 252,
411, 453, 454, - 85, 60, 606, 151, 140, 85, 724, 202, 141, 30, 139, 139, 139,
274, 218, 32, 141, 591, 141, 115, 139, 592, 139, 538, 459, 269, 31, 139, 139,
141, 139, 293, 277, 641, 642, 643, 115, 584, 443, 562, 113, 139, 585, 549, 371,
526, 316, 548, 609, 410, 255, 480, 610, 583, 658, 411, 420, 62, 485, 60, 411, 60,
461, 411, 520, 728, 140, 462, 773, 603, 769, 463, 658, 282, 140, 439, 94, 95,
296, 297, 278, 411, 283, 252, 37, 38, 39, 411, 273, 541, 288, 213, 776, 507, 380,
411, 611, 140, 485, 213, 300, 569, 371, 139, 395, 306, 572, 307, 464, 31, 290,
139, 657, 140, 667, 553, 726, 321, 278, 618, 407, 561, 308, 60, 309, 619, 660,
629, 268, 310, 657, 661, 80, 411, 139, 640, 678, 80, 681, 485, 324, 719, 411,
780, 411, 485, 666, 411, 317, 783, 139, 545, 550, 547, 140, 140, 702, 354, 355,
356, 703, 325, 30, 771, 326, 658, 1, 411, 32, 104, 327, 60, 105, 87, 363, 316,
364, 115, 699, 366, 141, 250, 373, 475, 634, 374, 479, 381, 316, 751, 639, 139,
139, 384, 357, 496, 497, 498, 499, 385, 489, 1, 358, 359, 587, 328, 329, 334,
335, 140, 388, 689, 336, 337, 390, 371, 391, 60, 767, 484, 411, 392, 414, 657,
617, 115, 109, 774, 1, 113, 527, 528, 205, 393, 141, 517, 514, 518, 403, 446,
404, 779, 684, 685, 781, 140, 139, 409, 30, 594, 411, 598, 411, 417, 32, 620,
411, 96, 622, 411, 418, 34, 532, 203, 534, 535, 422, 539, 623, 411, 80, 429, 37,
38, 39, 672, 411, 708, 411, 668, 710, 411, 139, 30, 714, 411, 734, 735, 425, 32,
140, 741, 411, 744, 411, 434, 34, 669, 670, 563, 435, 299, 753, 732, 302, 302,
436, 37, 38, 39, 30, 312, 313, 746, 411, 659, 32, 438, 437, 302, 302, 451, 302,
34, 452, 757, 297, 139, 765, 411, 376, 494, 495, 371, 37, 38, 39, 502, 503, 302,
500, 501, 322, 371, 478, 1, 665, 720, 483, 516, 239, 371, 522, 523, 238, 524,
536, 533, 596, 540, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 671, 542,
546, 674, 551, 555, 556, 558, 80, 239, 565, 564, 570, 238, 140, 351, 575, 566,
239, 31, 590, 576, 238, 577, 140, 579, 601, 140, 589, 371, 581, 582, 239, 588,
380, 239, 238, 595, 597, 238, 621, 599, 600, 604, 626, 628, 605, 607, 612, 730,
632, 139, 615, 624, 616, 630, 709, 30, 631, 713, 602, 139, 636, 32, 139, 633, -
177, 644, 723, 654, 292, - 177, - 177, 638, 371, 646, 648, - 177, 649, 283, 662,
37, 38, 39, 653, 733, 655, 663, 673, 687, 739, 740, 688, 255, 742, 743, 690, 745,
693, 694, 302, 695, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302, 302,
302, 302, 302, 302, 302, 696, 697, 761, 700, 701, 762, 704, 763, 764, 705, 706,
725, 675, 677, 721, 680, 722, 683, 727, 737, 729, 738, 754, 756, 759, 760, 775,
770, 786, 772, 778, 787, 476, 645, 368, 302, 375, 567, 752, 691, 637, 613, 465,
698, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 259,
574, 21, 712, 488, 450, 716, 718, 490, 647, 491, 0, 492, 0, 493, 22, 0, 23, 0, 0,
0, 0, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 302, 29, 0, 0, 0, 0, 302, 0, 0, 0, 0, 0,
748, 0, 0, 0, 0, 0, 0, 0, 0, 755, 0, 0, 0, 0, 0, 758, 0, 0, 0, 0, 0, 0, 31, 0,
33, 0, 0, 112, 0, 203, 766, 0, 0, 35, 36, 302, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 684, 685, 21, 153, 154, 219, 0, 220, 221,
222, 223, 224, 225, 226, 227, 22, 155, 23, 156, 228, 302, 157, 24, 229, 0, 0, 27,
28, 158, 159, 230, 231, 160, 29, 232, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 165, 166, 167, 168, 31, 32, 33, 0,
234, 112, 0, 0, 169, 0, 0, 35, 236, 0, 0, 0, 0, 0, 0, 171, 0, 0, 172, 173, 174,
0, 0, 0, 302, 175, 176, 0, 0, 0, 0, 0, 302, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 734, 735, 21, 153, 154, 219, 0, 220, 221, 222,
223, 224, 225, 226, 227, 22, 155, 23, 156, 228, 0, 157, 24, 229, 0, 0, 27, 28,
158, 159, 230, 231, 160, 29, 232, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 165, 166, 167, 168, 31, 32, 33, 0,
234, 112, 0, 0, 169, 0, 0, 35, 236, 0, 0, 0, 0, 0, 0, 171, 0, 0, 172, 173, 174,
0, 0, 0, 0, 175, 176, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 0, 0, 21, 153, 154, 219, 0, 220, 221, 222, 223, 224, 225, 226, 227,
22, 155, 23, 156, 228, 0, 157, 24, 229, 0, 0, 27, 28, 158, 159, 230, 231, 160,
29, 232, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 233, 165, 166, 167, 168, 31, 32, 33, 0, 234, 112, 235, 0, 169, 0,
0, 35, 236, 0, 0, 0, 0, 0, 0, 171, 0, 0, 172, 173, 174, 0, 0, 0, 0, 175, 176, 2,
3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 153,
154, 219, 0, 220, 221, 222, 223, 224, 225, 226, 227, 22, 155, 23, 156, 228, 0,
157, 24, 229, 0, 0, 27, 28, 158, 159, 230, 231, 160, 29, 232, 161, 162, 0, 163,
164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 165,
166, 167, 168, 31, 32, 33, 0, 234, 112, 0, 0, 169, 0, 0, 35, 236, 0, 0, 0, 0, 0,
0, 171, 0, 0, 172, 173, 174, 0, 0, 0, 0, 175, 176, 2, 3, 4, 111, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 153, 154, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 22, 155, 23, 156, 0, 0, 157, 24, 0, 0, 0, 27, 28, 158, 159, 0, 0, 160,
29, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 165, 166, 167, 168, 31, 32, 33, 0, 537, 0, 0, 0, 169, 0, 0, 35,
236, 0, 0, 0, 0, 0, 0, 171, 0, 0, 172, 173, 174, 0, 0, 0, 0, 175, 176, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 153, 154, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 22, 155, 0, 156, 0, 0, 157, 0, 0, 0, 0, 27, 28, 158, 159, 0, 0, 160,
29, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 165, 166, 167, 168, 31, 32, 33, 0, 0, 314, 0, 0, 169, 0, 0, 35,
236, 0, 0, 0, 0, 0, 0, 171, 0, 0, 172, 173, 174, 0, 0, 0, 0, 175, 176, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 153, 154, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 22, 155, 0, 156, 0, 0, 157, 0, 0, 0, 0, 27, 28, 158, 159, 0, 0, 160,
29, 0, 161, 162, 0, 163, 164, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 31, 32, 33, 22, 0, 0, 0, 0, 169,
0, 0, 35, 236, 0, 27, 28, 0, 0, 0, 171, 0, 29, 172, 173, 174, 0, 0, 0, 0, 175,
176, - 8, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 31, 21, 33, 0, 0, 138, 0, 0, 0, 0, 0, 35, 36, 0, 22, 0, 23, 0, 0, 0, 0,
24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 0, 0, -
8, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0, 37, 38, 39, - 8, 1, 0, 2, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0, 25, 26, 27, 28, 0, 0, 0, 0, 0,
29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 0, 0, 35, 36, 0, 0, 0, 0, 0, 0,
37, 38, 39, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 23, 0, 0, 0, 0, 24, 0,
25, 26, 27, 28, 0, 0, 0, 0, 0, 29, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0,
0, 0, - 8, 0, 34, 27, 28, 35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 1, 0, 2, 3, 4,
111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0,
31, 0, 33, 0, 0, 0, 0, 0, 0, 22, 0, 35, 36, 0, 0, 0, 24, 0, 0, 0, 27, 28, 1, 0,
0, 0, 0, 29, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 22, 0, 31, 32, 33, 0, 0, 0, 0, 0, 34, 27, 28,
35, 36, 0, 0, 0, 29, 0, 0, 37, 38, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 66, 253, 0, 0, 30, 0, 0, 0, 0, 31, 32, 33, 0, 255, 0, 0, 0, 292, -
159, 0, 35, 36, 0, 0, - 159, 0, 283, 0, 37, 38, 39, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 30, 68, 0, 69, 70, 0, 32, 0, 0, 0, 0, 440, 0, 71, 0, 22,
72, 73, 0, 0, 0, 441, 442, 0, 74, 0, 27, 28, 75, 76, 0, 0, 0, 29, 0, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 253, 0, 0, 0,
0, 0, 22, 0, 31, 0, 33, 0, 255, 0, 0, 0, 282, 27, 28, 35, 36, 0, 0, 0, 29, 283,
0, 37, 38, 39, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 253, 0, 21, 254, 0, 0, 0, 0, 31, 0, 33, 0, 255, 0, 0, 22, 0, 23, 0, 35, 36,
0, 24, 0, 0, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 2, 3, 4, 111, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 0,
33, 22, 0, 112, 0, 0, 0, 0, 24, 35, 36, 0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 153, 154, 219, 0, 220, 221, 222,
223, 224, 225, 226, 227, 31, 155, 33, 156, 228, 0, 157, 0, 229, 0, 0, 35, 36,
158, 159, 230, 231, 160, 0, 232, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 165, 166, 167, 168, 0, 32, 153, 154,
234, 112, 0, 0, 169, 470, 0, 0, 170, 0, 0, 155, 0, 156, 0, 171, 157, 0, 172, 173,
174, 0, 0, 158, 159, 175, 176, 160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 0,
0, 0, 303, 471, 0, 169, 0, 0, 0, 170, 153, 154, 0, 441, 442, 0, 171, 0, 0, 172,
173, 174, 0, 155, 0, 156, 175, 176, 157, 0, 0, 0, 0, 0, 0, 158, 159, 0, 0, 160,
0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 0, 0, 0, 303, 656, 0, 169, 0, 0, 0,
170, 153, 154, 0, 441, 442, 0, 171, 0, 0, 172, 173, 174, 0, 155, 0, 156, 175,
176, 157, 0, 0, 0, 0, 0, 0, 158, 159, 0, 0, 160, 0, 0, 161, 162, 0, 163, 164, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167,
168, 0, 32, 0, 0, 0, 303, 731, 0, 169, 0, 0, 0, 170, 153, 154, 0, 441, 442, 0,
171, 0, 0, 172, 173, 174, 0, 155, 0, 156, 175, 176, 157, 0, 0, 0, 0, 0, 0, 158,
159, 0, 0, 160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 0, 0, 0, 303, 0, 66,
169, 0, 0, 0, 170, 153, 154, 0, 441, 442, 0, 171, 0, 0, 172, 173, 174, 0, 155, 0,
156, 175, 176, 157, 0, 0, 0, 0, 0, 0, 158, 159, 0, 0, 160, 0, 0, 161, 162, 0,
163, 164, 0, 0, 67, 68, 0, 69, 70, 0, 32, 0, 0, 0, 0, 0, 0, 71, 0, 0, 72, 73, 0,
30, 165, 166, 167, 168, 74, 32, 153, 154, 75, 76, 0, 0, 169, 0, 0, 0, 170, 0, 0,
155, 0, 156, 261, 171, 157, 0, 172, 173, 174, 0, 0, 158, 159, 175, 176, 160, 0,
0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 153, 154, 0, 303, 0, 0, 169, 0, 0, 0,
170, 0, 0, 155, 0, 156, 0, 171, 157, 0, 172, 173, 174, 0, 0, 158, 159, 175, 176,
160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 153, 154, 394, 0, 0, 0, 169, 0, 0,
0, 170, 0, 0, 155, 0, 156, 0, 171, 157, 0, 172, 173, 174, 0, 0, 158, 159, 175,
176, 160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 153, 154, 0, 0, 0, 0, 169, 0,
0, 0, 170, 0, 0, 155, 0, 156, 458, 171, 157, 0, 172, 173, 174, 0, 0, 158, 159,
175, 176, 160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 153, 154, 0, 0, 0, 0,
169, 510, 0, 0, 170, 0, 0, 155, 0, 156, 0, 171, 157, 0, 172, 173, 174, 0, 0, 158,
159, 175, 176, 160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 153, 154, 0, 0, 0,
0, 169, 543, 0, 0, 170, 0, 0, 155, 0, 156, 0, 171, 157, 0, 172, 173, 174, 0, 0,
158, 159, 175, 176, 160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 153, 154, 0,
0, 0, 0, 169, 0, 0, 0, 170, 0, 0, 155, 0, 156, 586, 171, 157, 0, 172, 173, 174,
0, 0, 158, 159, 175, 176, 160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 153,
154, 625, 0, 0, 0, 169, 0, 0, 0, 170, 0, 0, 155, 0, 156, 0, 171, 157, 0, 172,
173, 174, 0, 0, 158, 159, 175, 176, 160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32,
153, 154, 627, 0, 0, 0, 169, 0, 0, 0, 170, 0, 0, 155, 0, 156, 0, 171, 157, 0,
172, 173, 174, 0, 0, 158, 159, 175, 176, 160, 0, 0, 161, 162, 0, 163, 164, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168,
0, 32, 153, 154, 0, 0, 0, 0, 169, 676, 0, 0, 170, 0, 0, 155, 0, 156, 0, 171, 157,
0, 172, 173, 174, 0, 0, 158, 159, 175, 176, 160, 0, 0, 161, 162, 0, 163, 164, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166, 167,
168, 0, 32, 153, 154, 0, 0, 0, 0, 169, 679, 0, 0, 170, 0, 0, 155, 0, 156, 0, 171,
157, 0, 172, 173, 174, 0, 0, 158, 159, 175, 176, 160, 0, 0, 161, 162, 0, 163,
164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165, 166,
167, 168, 0, 32, 153, 154, 682, 0, 0, 0, 169, 0, 0, 0, 170, 0, 0, 155, 0, 156, 0,
171, 157, 0, 172, 173, 174, 0, 0, 158, 159, 175, 176, 160, 0, 0, 161, 162, 0,
163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 165,
166, 167, 168, 0, 32, 153, 154, 0, 0, 0, 0, 169, 711, 0, 0, 170, 0, 0, 155, 0,
156, 0, 171, 157, 0, 172, 173, 174, 0, 0, 158, 159, 175, 176, 160, 0, 0, 161,
162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
30, 165, 166, 167, 168, 0, 32, 153, 154, 0, 0, 0, 0, 169, 715, 0, 0, 170, 0, 0,
155, 0, 156, 0, 171, 157, 0, 172, 173, 174, 0, 0, 158, 159, 175, 176, 160, 0, 0,
161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 30, 165, 166, 167, 168, 0, 32, 153, 154, 0, 0, 0, 0, 169, 717, 0, 0, 170,
0, 0, 155, 0, 156, 0, 171, 157, 0, 172, 173, 174, 0, 0, 158, 159, 175, 176, 160,
0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 153, 154, 0, 0, 0, 0, 169, 747, 0, 0,
170, 0, 0, 155, 0, 156, 0, 171, 157, 0, 172, 173, 174, 0, 0, 158, 159, 175, 176,
160, 0, 0, 161, 162, 0, 163, 164, 0, 0, 0, 0, 153, 154, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 155, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 0, 158, 159, 0, 0, 160,
169, 0, 0, 162, 170, 163, 164, 0, 0, 0, 0, 171, 0, 0, 172, 173, 174, 0, 0, 0, 0,
175, 176, 0, 0, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 153, 154, 0, 303, 0, 0,
169, 0, 0, 0, 170, 0, 0, 155, 0, 0, 0, 171, 0, 0, 172, 173, 174, 0, 0, 158, 159,
175, 176, 160, 0, 0, 0, 162, 0, 163, 164, 0, 0, 0, 0, 153, 154, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 155, 0, 0, 0, 30, 165, 166, 167, 168, 0, 32, 0, 158, 159, 602, 0,
160, 169, 0, 0, 162, 170, 163, 164, 0, 0, 0, 0, 171, 0, 0, 172, 173, 174, 0, 0,
0, 0, 175, 176, 0, 0, 153, 154, 0, 30, 165, 166, 167, 168, 0, 32, 0, 0, 0, 155,
0, 0, 298, 0, 0, 0, 170, 0, 0, 0, 0, 158, 159, 171, 0, 160, 172, 173, 174, 162,
0, 163, 164, 175, 176, 0, 0, 153, 154, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 155, 0,
0, 0, 30, 165, 166, 167, 168, 0, 32, 0, 158, 159, 0, 0, 160, 311, 0, 0, 162, 170,
163, 164, 0, 0, 0, 0, 171, 0, 0, 172, 173, 174, 0, 0, 0, 0, 175, 176, 0, 0, 0, 0,
0, 30, 165, 166, 167, 168, 0, 32, 0, 0, 0, 0, 0, 0, 169, 0, 0, 0, 170, 0, 0, 0,
0, 0, 0, 171, 0, 0, 172, 173, 174, 0, 0, 0, 0, 175, 176, 7, 8, 9, 10, 11, 12, 13,
14, 15, 16, 17, 18, 19, 20, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 0, 22, 0, 0, 0, 29, 0, 0, 0, 0, 66,
0, 27, 28, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 253, 0, 0, 0, 0, 0,
0, 0, 31, 0, 33, 66, 255, 0, 559, 0, 0, 0, 0, 35, 36, 0, 31, 0, 33, 30, 68, 0,
69, 70, 0, 32, 0, 35, 36, 0, 0, 0, 71, 0, 0, 72, 73, 0, 0, 0, 441, 442, 0, 74, 0,
0, 0, 75, 76, 0, 30, 68, 0, 69, 70, 0, 32, 0, 0, 0, 0, 0, 0, 71, 0, 0, 72, 73, 0,
0, 0, 0, 0, 0, 74, 0, 0, 0, 75, 76}; static short Cyc_yycheck[ 4377u]={ 55, 157,
23, 88, 55, 58, 157, 55, 63, 117, 0, 55, 156, 1, 118, 361, 57, 83, 5, 6, 56, 237,
238, 85, 240, 117, 169, 413, 436, 673, 357, 88, 22, 23, 442, 25, 83, 54, 297,
203, 69, 58, 99, 68, 34, 100, 101, 102, 103, 100, 71, 41, 100, 275, 421, 76, 46,
112, 80, 114, 91, 112, 52, 114, 54, 451, 114, 57, 141, 83, 60, 398, 82, 171, 172,
17, 174, 150, 103, 88, 109, 71, 104, 92, 106, 121, 76, 101, 110, 456, 99, 107,
83, 191, 85, 83, 96, 88, 314, 115, 116, 89, 92, 93, 94, 95, 750, 94, 96, 47, 48,
49, 284, 85, 105, 101, 524, 83, 108, 85, 102, 138, 294, 767, 96, 115, 117, 118,
110, 83, 63, 85, 776, 101, 88, 125, 63, 781, 128, 105, 130, 83, 63, 17, 18, 19,
89, 90, 93, 204, 258, 105, 118, 204, 98, 298, 204, 300, 64, 65, 150, 420, 106,
117, 118, 98, 373, 81, 311, 102, 93, 98, 96, 1, 381, 102, 119, 98, 169, 93, 96,
102, 237, 238, 122, 240, 237, 238, 101, 240, 263, 104, 83, 215, 108, 101, 102,
88, 89, 83, 253, 139, 140, 169, 96, 141, 90, 93, 99, 282, 96, 91, 202, 105, 150,
169, 96, 584, 93, 292, 211, 101, 83, 213, 290, 105, 216, 41, 93, 90, 83, 34, 376,
107, 108, 109, 253, 95, 255, 84, 338, 339, 340, 46, 83, 211, 104, 105, 92, 239,
286, 90, 307, 294, 275, 83, 602, 211, 98, 314, 203, 89, 88, 314, 92, 92, 93, 93,
96, 97, 98, 288, 263, 640, 95, 103, 92, 105, 92, 93, 94, 95, 98, 92, 97, 275,
276, 96, 222, 280, 281, 282, 436, 552, 228, 92, 230, 231, 442, 290, 93, 292, 292,
96, 294, 96, 297, 298, 101, 300, 474, 125, 115, 98, 280, 281, 96, 307, 103, 404,
311, 101, 689, 98, 290, 83, 280, 281, 282, 136, 85, 89, 298, 94, 300, 390, 290,
98, 292, 390, 283, 97, 88, 297, 298, 311, 300, 150, 94, 83, 84, 85, 407, 461,
576, 428, 407, 311, 462, 407, 303, 383, 169, 407, 94, 92, 92, 310, 98, 461, 592,
98, 98, 91, 317, 365, 98, 367, 96, 98, 524, 103, 373, 101, 103, 483, 753, 105,
610, 96, 381, 92, 92, 93, 97, 98, 413, 98, 105, 202, 107, 108, 109, 98, 83, 92,
110, 373, 103, 352, 213, 98, 516, 404, 357, 381, 96, 438, 361, 373, 227, 96, 443,
96, 460, 88, 98, 381, 592, 420, 102, 94, 691, 94, 451, 92, 239, 428, 96, 429, 96,
98, 598, 92, 434, 96, 610, 598, 438, 98, 404, 559, 92, 443, 92, 398, 68, 92, 98,
773, 98, 404, 606, 98, 96, 779, 420, 401, 411, 403, 461, 462, 94, 59, 60, 61, 98,
113, 83, 94, 114, 703, 1, 98, 89, 90, 112, 477, 93, 292, 94, 298, 94, 548, 658,
94, 462, 548, 93, 306, 548, 94, 309, 93, 311, 721, 559, 461, 462, 93, 96, 330,
331, 332, 333, 96, 323, 1, 104, 105, 463, 66, 67, 62, 63, 516, 96, 635, 110, 111,
96, 474, 83, 523, 750, 97, 98, 92, 555, 703, 530, 596, 559, 759, 1, 596, 5, 6,
596, 92, 516, 365, 359, 367, 103, 576, 96, 773, 22, 23, 776, 552, 516, 94, 83,
97, 98, 97, 98, 97, 89, 97, 98, 92, 97, 98, 103, 96, 385, 95, 387, 388, 97, 390,
97, 98, 576, 94, 107, 108, 109, 97, 98, 97, 98, 617, 97, 98, 552, 83, 97, 98, 22,
23, 106, 89, 597, 97, 98, 97, 98, 98, 96, 618, 619, 429, 94, 153, 726, 704, 156,
157, 95, 107, 108, 109, 83, 163, 164, 97, 98, 597, 89, 98, 97, 171, 172, 98, 174,
96, 94, 97, 98, 597, 97, 98, 103, 328, 329, 592, 107, 108, 109, 336, 337, 191,
334, 335, 69, 602, 25, 1, 605, 684, 97, 101, 721, 610, 92, 98, 721, 103, 30, 96,
477, 92, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 620, 42, 88, 623, 83, 110, 88,
97, 684, 750, 97, 96, 83, 750, 691, 95, 94, 98, 759, 88, 83, 98, 759, 97, 701,
97, 94, 704, 98, 658, 97, 106, 773, 97, 523, 776, 773, 97, 96, 776, 533, 98, 97,
97, 537, 538, 98, 97, 94, 701, 97, 691, 94, 96, 94, 93, 676, 83, 92, 679, 93,
701, 94, 89, 704, 102, 92, 97, 688, 83, 96, 97, 98, 101, 703, 106, 97, 103, 97,
105, 25, 107, 108, 109, 106, 705, 101, 97, 93, 92, 710, 711, 83, 92, 714, 715,
88, 717, 98, 97, 322, 97, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334,
335, 336, 337, 338, 339, 340, 97, 97, 741, 97, 96, 744, 96, 746, 747, 28, 93, 98,
624, 625, 103, 627, 94, 629, 84, 94, 97, 92, 98, 97, 103, 94, 765, 84, 0, 97, 97,
0, 307, 566, 202, 376, 211, 434, 725, 639, 555, 523, 288, 655, 3, 4, 5, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 117, 444, 24, 678, 322, 276,
681, 682, 324, 576, 325, - 1, 326, - 1, 327, 37, - 1, 39, - 1, - 1, - 1, - 1, 44,
- 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, 436, 55, - 1, - 1, - 1, - 1, 442, - 1,
- 1, - 1, - 1, - 1, 719, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 728, - 1, - 1,
- 1, - 1, - 1, 734, - 1, - 1, - 1, - 1, - 1, - 1, 88, - 1, 90, - 1, - 1, 93, - 1,
95, 749, - 1, - 1, 99, 100, 483, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34,
35, 36, 37, 38, 39, 40, 41, 524, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53,
54, 55, 56, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88,
89, 90, - 1, 92, 93, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, -
1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1, 598, 117, 118, - 1, - 1, - 1, -
1, - 1, 606, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
22, 23, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
- 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60,
61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1, 92, 93, - 1, -
1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110, 111,
112, - 1, - 1, - 1, - 1, 117, 118, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1, 29, 30, 31, 32, 33, 34,
35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1, 48, 49, 50, 51, 52, 53,
54, 55, 56, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88,
89, 90, - 1, 92, 93, 94, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, -
1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 3, 4, 5, 6, 7, 8,
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, 26, 27, - 1,
29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, - 1, 43, 44, 45, - 1, - 1,
48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
83, 84, 85, 86, 87, 88, 89, 90, - 1, 92, 93, - 1, - 1, 96, - 1, - 1, 99, 100, -
1, - 1, - 1, - 1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1, - 1,
117, 118, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, 24, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, 39,
40, - 1, - 1, 43, 44, - 1, - 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57,
58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, 88, 89, 90, - 1, 92,
- 1, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, - 1, - 1, 107, - 1, -
1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1, - 1, - 1, 48, 49, 50, 51,
- 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85,
86, 87, 88, 89, 90, - 1, - 1, 93, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1,
- 1, - 1, - 1, 107, - 1, - 1, 110, 111, 112, - 1, - 1, - 1, - 1, 117, 118, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, 25, 26, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 37, 38, - 1, 40, - 1, - 1, 43, - 1, - 1,
- 1, - 1, 48, 49, 50, 51, - 1, - 1, 54, 55, - 1, 57, 58, - 1, 60, 61, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
83, 84, 85, 86, 87, 88, 89, 90, 37, - 1, - 1, - 1, - 1, 96, - 1, - 1, 99, 100, -
1, 48, 49, - 1, - 1, - 1, 107, - 1, 55, 110, 111, 112, - 1, - 1, - 1, - 1, 117,
118, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, - 1, 88, 24, 90, - 1, - 1, 93, - 1, - 1, - 1, - 1, - 1, 99, 100, - 1, 37, -
1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, - 1, - 1, - 1, - 1,
88, 89, 90, - 1, - 1, - 1, 94, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1, - 1, -
1, - 1, 107, 108, 109, 0, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46, 47, 48, 49, - 1,
- 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83,
- 1, - 1, - 1, - 1, 88, 89, 90, - 1, - 1, - 1, - 1, - 1, 96, - 1, - 1, 99, 100,
- 1, - 1, - 1, - 1, - 1, - 1, 107, 108, 109, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 39, - 1, - 1, - 1, - 1, 44, - 1, 46,
47, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
83, - 1, - 1, 37, - 1, 88, 89, 90, - 1, - 1, - 1, 94, - 1, 96, 48, 49, 99, 100,
- 1, - 1, - 1, 55, - 1, - 1, 107, 108, 109, 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, 88, - 1, 90,
- 1, - 1, - 1, - 1, - 1, - 1, 37, - 1, 99, 100, - 1, - 1, - 1, 44, - 1, - 1, - 1,
48, 49, 1, - 1, - 1, - 1, - 1, 55, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, - 1,
- 1, 37, - 1, 88, 89, 90, - 1, - 1, - 1, - 1, - 1, 96, 48, 49, 99, 100, - 1, - 1,
- 1, 55, - 1, - 1, 107, 108, 109, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 38, 80, - 1, - 1, 83, - 1, - 1, - 1,
- 1, 88, 89, 90, - 1, 92, - 1, - 1, - 1, 96, 97, - 1, 99, 100, - 1, - 1, 103, -
1, 105, - 1, 107, 108, 109, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
83, 84, - 1, 86, 87, - 1, 89, - 1, - 1, - 1, - 1, 94, - 1, 96, - 1, 37, 99, 100,
- 1, - 1, - 1, 104, 105, - 1, 107, - 1, 48, 49, 111, 112, - 1, - 1, - 1, 55, - 1,
8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 80, - 1, - 1, - 1, - 1, - 1, 37, - 1, 88, - 1, 90, - 1, 92, -
1, - 1, - 1, 96, 48, 49, 99, 100, - 1, - 1, - 1, 55, 105, - 1, 107, 108, 109, 3,
4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 80, - 1, 24,
83, - 1, - 1, - 1, - 1, 88, - 1, 90, - 1, 92, - 1, - 1, 37, - 1, 39, - 1, 99,
100, - 1, 44, - 1, - 1, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, 3, 4, 5,
6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, - 1, 90, 37, - 1, 93, - 1, - 1, - 1, -
1, 44, 99, 100, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 25,
26, 27, - 1, 29, 30, 31, 32, 33, 34, 35, 36, 88, 38, 90, 40, 41, - 1, 43, - 1,
45, - 1, - 1, 99, 100, 50, 51, 52, 53, 54, - 1, 56, 57, 58, - 1, 60, 61, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, 93, - 1, - 1, 96, 32,
- 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1,
50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83,
84, 85, 86, 87, - 1, 89, - 1, - 1, - 1, 93, 94, - 1, 96, - 1, - 1, - 1, 100, 25,
26, - 1, 104, 105, - 1, 107, - 1, - 1, 110, 111, 112, - 1, 38, - 1, 40, 117, 118,
43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1,
60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, - 1, - 1, 93,
94, - 1, 96, - 1, - 1, - 1, 100, 25, 26, - 1, 104, 105, - 1, 107, - 1, - 1, 110,
111, 112, - 1, 38, - 1, 40, 117, 118, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51,
- 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85,
86, 87, - 1, 89, - 1, - 1, - 1, 93, 94, - 1, 96, - 1, - 1, - 1, 100, 25, 26, - 1,
104, 105, - 1, 107, - 1, - 1, 110, 111, 112, - 1, 38, - 1, 40, 117, 118, 43, - 1,
- 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1, 54, - 1, - 1, 57, 58, - 1, 60, 61, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, - 1, - 1, - 1, 93, - 1, 38,
96, - 1, - 1, - 1, 100, 25, 26, - 1, 104, 105, - 1, 107, - 1, - 1, 110, 111, 112,
- 1, 38, - 1, 40, 117, 118, 43, - 1, - 1, - 1, - 1, - 1, - 1, 50, 51, - 1, - 1,
54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, 83, 84, - 1, 86, 87, - 1, 89, - 1,
- 1, - 1, - 1, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, 83, 84, 85, 86, 87, 107, 89,
25, 26, 111, 112, - 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, 106,
107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, -
1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, 93,
- 1, - 1, 96, - 1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110,
111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, 92, - 1, - 1, - 1, 96, -
1, - 1, - 1, 100, - 1, - 1, 38, - 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1,
- 1, 50, 51, 117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 83, 84, 85, 86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, - 1, - 1, - 1,
100, - 1, - 1, 38, - 1, 40, 106, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51,
117, 118, 54, - 1, - 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85,
86, 87, - 1, 89, 25, 26, - 1, - 1, - 1, - 1, 96, 97, - 1, - 1, 100, - 1, - 1, 38,
- 1, 40, - 1, 107, 43, - 1, 110, 111, 112, - 1, - 1, 50, 51, 117, 118, 54, - 1,
- 1, 57, 58, - 1, 60, 61, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 83, 84, 85, 86, 87, - 1, 89,
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
1, - 1, 55, - 1, - 1, - 1, - 1, 38, - 1, 48, 49, - 1, - 1, - 1, - 1, - 1, 55, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 80, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, 88, - 1, 90, 38, 92, - 1, 80, - 1, - 1, - 1, - 1, 99, 100, - 1, 88, - 1, 90,
83, 84, - 1, 86, 87, - 1, 89, - 1, 99, 100, - 1, - 1, - 1, 96, - 1, - 1, 99, 100,
- 1, - 1, - 1, 104, 105, - 1, 107, - 1, - 1, - 1, 111, 112, - 1, 83, 84, - 1, 86,
87, - 1, 89, - 1, - 1, - 1, - 1, - 1, - 1, 96, - 1, - 1, 99, 100, - 1, - 1, - 1,
- 1, - 1, - 1, 107, - 1, - 1, - 1, 111, 112}; unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_arr); extern int Cyc_yylex();
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL"; static
int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static
int Cyc_yynerrs= 0; struct _tuple19{ void* f1; void* f2; } ; struct _tuple20{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int
yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; short*
yyss=( short*)({ unsigned int _temp3183= 10000u; short* _temp3184=( short*)
GC_malloc_atomic( sizeof( short) * _temp3183);{ unsigned int _temp3185=
_temp3183; unsigned int i; for( i= 0; i < _temp3185; i ++){ _temp3184[ i]=(
short) 0;}}; _temp3184;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3180= 10000u; void** _temp3181=( void**) GC_malloc( sizeof(
void*) * _temp3180);{ unsigned int _temp3182= _temp3180; unsigned int i; for( i=
0; i < _temp3182; i ++){ _temp3181[ i]= Cyc_yylval;}}; _temp3181;}); int
yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({ unsigned int
_temp3177= 10000u; struct Cyc_Yyltype* _temp3178=( struct Cyc_Yyltype*)
GC_malloc_atomic( sizeof( struct Cyc_Yyltype) * _temp3177);{ unsigned int
_temp3179= _temp3177; unsigned int i; for( i= 0; i < _temp3179; i ++){ _temp3178[
i]= Cyc_yynewloc();}}; _temp3178;}); int yystacksize= 10000; void* yyval= Cyc_yylval;
int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2;
yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate: yyss[
_check_known_subscript_notnull( 10000u, ++ yyssp_offset)]=( short) yystate; if(
yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(
_tag_arr("parser stack overflow", sizeof( unsigned char), 22u));( int) _throw((
void*) Cyc_Yystack_overflow);}} goto yybackup; yybackup: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 788u, yystate)]; if( yyn == - 32768){ goto
yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <=
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
346: 0)?( int) Cyc_yytranslate[ _check_known_subscript_notnull( 347u, Cyc_yychar)]:
231;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4376)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4377u, yyn)] != yychar1){ goto yydefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4377u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrlab;}} if( yyn == 787){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar=
- 2;} yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc; if(
yyerrstatus != 0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault:
yyn=( int) Cyc_yydefact[ _check_known_subscript_notnull( 788u, yystate)]; if(
yyn == 0){ goto yyerrlab;} yyreduce: yylen=( int) Cyc_yyr2[
_check_known_subscript_notnull( 401u, yyn)]; if( yylen > 0){ yyval= yyvs[
_check_known_subscript_notnull( 10000u,( yyvsp_offset + 1) - yylen)];} switch(
yyn){ case 1: _LL1402: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1403:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1405=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1405[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1406; _temp1406.tag= Cyc_DeclList_tok; _temp1406.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1406;});
_temp1405;}); break; case 3: _LL1404: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1408=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1408[ 0]=({ struct Cyc_DeclList_tok_struct _temp1409; _temp1409.tag= Cyc_DeclList_tok;
_temp1409.f1=({ struct Cyc_List_List* _temp1410=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1410->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1411=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1411->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1412=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1412[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1413; _temp1413.tag= Cyc_Absyn_Using_d;
_temp1413.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1413.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1413;}); _temp1412;}));
_temp1411->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1411;});
_temp1410->tl= 0; _temp1410;}); _temp1409;}); _temp1408;}); Cyc_Lex_leave_using();
break; case 4: _LL1407: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1415=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1415[ 0]=({ struct Cyc_DeclList_tok_struct _temp1416; _temp1416.tag= Cyc_DeclList_tok;
_temp1416.f1=({ struct Cyc_List_List* _temp1417=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1417->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1418=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1418->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1419=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1419[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1420; _temp1420.tag= Cyc_Absyn_Using_d;
_temp1420.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp1420.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1420;});
_temp1419;})); _temp1418->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1418;}); _temp1417->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1417;}); _temp1416;});
_temp1415;}); break; case 5: _LL1414: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1422=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1422[ 0]=({ struct Cyc_DeclList_tok_struct _temp1423; _temp1423.tag= Cyc_DeclList_tok;
_temp1423.f1=({ struct Cyc_List_List* _temp1424=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1424->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1425=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1425->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1426=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1426[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1427; _temp1427.tag=
Cyc_Absyn_Namespace_d; _temp1427.f1=({ struct _tagged_arr* _temp1428=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp1428[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1428;});
_temp1427.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1427;}); _temp1426;})); _temp1425->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1425;}); _temp1424->tl= 0; _temp1424;}); _temp1423;}); _temp1422;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1421: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1430=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1430[ 0]=({ struct Cyc_DeclList_tok_struct _temp1431; _temp1431.tag= Cyc_DeclList_tok;
_temp1431.f1=({ struct Cyc_List_List* _temp1432=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1432->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1433=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1433->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1434=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1434[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1435; _temp1435.tag=
Cyc_Absyn_Namespace_d; _temp1435.f1=({ struct _tagged_arr* _temp1436=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp1436[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1436;});
_temp1435.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1435;}); _temp1434;})); _temp1433->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1433;}); _temp1432->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1432;}); _temp1431;});
_temp1430;}); break; case 7: _LL1429: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1438=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1438[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1439; _temp1439.tag= Cyc_DeclList_tok; _temp1439.f1=({ struct Cyc_List_List*
_temp1440=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1440->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1441=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1441->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1442=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1442[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1443; _temp1443.tag= Cyc_Absyn_ExternC_d;
_temp1443.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1443;}); _temp1442;})); _temp1441->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1441;}); _temp1440->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1440;}); _temp1439;});
_temp1438;}); break; case 8: _LL1437: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1445=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1445[ 0]=({ struct Cyc_DeclList_tok_struct _temp1446; _temp1446.tag= Cyc_DeclList_tok;
_temp1446.f1= 0; _temp1446;}); _temp1445;}); break; case 9: _LL1444: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1448=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1448[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1449; _temp1449.tag= Cyc_DeclList_tok; _temp1449.f1=({ struct Cyc_List_List*
_temp1450=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1450->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1451=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1451[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1452; _temp1452.tag= Cyc_Absyn_Fn_d;
_temp1452.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1452;}); _temp1451;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1450->tl=
0; _temp1450;}); _temp1449;}); _temp1448;}); break; case 10: _LL1447: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1453: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1455=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1455[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1456; _temp1456.tag= Cyc_FnDecl_tok; _temp1456.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1456;});
_temp1455;}); break; case 12: _LL1454: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1458=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1458[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1459; _temp1459.tag= Cyc_FnDecl_tok;
_temp1459.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1460=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1460->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1460;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1459;}); _temp1458;}); break; case 13: _LL1457: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1462=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1462[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1463; _temp1463.tag= Cyc_FnDecl_tok;
_temp1463.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1463;}); _temp1462;}); break; case 14: _LL1461: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1465=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1465[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1466; _temp1466.tag= Cyc_FnDecl_tok;
_temp1466.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1467=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1467->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1467;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1466;}); _temp1465;}); break; case 15: _LL1464: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1469=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1469[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1470; _temp1470.tag= Cyc_FnDecl_tok;
_temp1470.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1471=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1471->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1471;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1470;}); _temp1469;}); break; case 16: _LL1468: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1473=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1473[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1474; _temp1474.tag= Cyc_FnDecl_tok;
_temp1474.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1475=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1475->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1475;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1474;}); _temp1473;}); break; case 17: _LL1472: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1476:
Cyc_Lex_leave_using(); break; case 19: _LL1477: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1479=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp1479[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1479;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1478: Cyc_Lex_leave_namespace(); break; case 21: _LL1480: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1482=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1482[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1483; _temp1483.tag= Cyc_DeclList_tok; _temp1483.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp1483;}); _temp1482;}); break; case 22: _LL1481: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1485=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1485[ 0]=({ struct Cyc_DeclList_tok_struct _temp1486; _temp1486.tag= Cyc_DeclList_tok;
_temp1486.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1486;}); _temp1485;}); break; case 23: _LL1484: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1488=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1488[ 0]=({ struct Cyc_DeclList_tok_struct _temp1489; _temp1489.tag= Cyc_DeclList_tok;
_temp1489.f1=({ struct Cyc_List_List* _temp1490=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1490->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1490->tl=
0; _temp1490;}); _temp1489;}); _temp1488;}); break; case 24: _LL1487: { struct
Cyc_List_List* _temp1492= 0;{ struct Cyc_List_List* _temp1493= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); for( 0;
_temp1493 != 0; _temp1493=(( struct Cyc_List_List*) _check_null( _temp1493))->tl){
struct _tagged_arr* _temp1494=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1493))->hd; struct _tuple1* _temp1495=({ struct _tuple1*
_temp1498=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1498->f1=
Cyc_Absyn_rel_ns_null; _temp1498->f2= _temp1494; _temp1498;}); struct Cyc_Absyn_Vardecl*
_temp1496= Cyc_Absyn_new_vardecl( _temp1495, Cyc_Absyn_wildtyp( 0), 0);
_temp1492=({ struct Cyc_List_List* _temp1497=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1497->hd=( void*) _temp1496; _temp1497->tl=
_temp1492; _temp1497;});}} _temp1492=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1492); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1499=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1499[ 0]=({ struct Cyc_DeclList_tok_struct _temp1500; _temp1500.tag= Cyc_DeclList_tok;
_temp1500.f1=({ struct Cyc_List_List* _temp1501=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1501->hd=( void*) Cyc_Absyn_letv_decl(
_temp1492, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1501->tl= 0; _temp1501;}); _temp1500;});
_temp1499;}); break;} case 25: _LL1491: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1502:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1504=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1504[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1505; _temp1505.tag= Cyc_DeclList_tok; _temp1505.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1505;});
_temp1504;}); break; case 27: _LL1503: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1507=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1507[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1508; _temp1508.tag= Cyc_DeclSpec_tok;
_temp1508.f1=({ struct Cyc_Parse_Declaration_spec* _temp1509=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1509->sc=({ struct
Cyc_Core_Opt* _temp1510=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp1510->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1510;});
_temp1509->tq= Cyc_Absyn_empty_tqual(); _temp1509->type_specs= 0; _temp1509->is_inline=
0; _temp1509->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1509;}); _temp1508;});
_temp1507;}); break; case 28: _LL1506: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc != 0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1512=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1512[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1513; _temp1513.tag= Cyc_DeclSpec_tok; _temp1513.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1514=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1514->sc=({ struct Cyc_Core_Opt* _temp1515=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1515->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1515;});
_temp1514->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1514->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1514->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1514->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1514;}); _temp1513;}); _temp1512;}); break; case 29: _LL1511: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1517=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1517[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1518; _temp1518.tag= Cyc_DeclSpec_tok; _temp1518.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1519=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1519->sc= 0; _temp1519->tq= Cyc_Absyn_empty_tqual(); _temp1519->type_specs=({
struct Cyc_List_List* _temp1520=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1520->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1520->tl= 0;
_temp1520;}); _temp1519->is_inline= 0; _temp1519->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1519;});
_temp1518;}); _temp1517;}); break; case 30: _LL1516: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1522=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1522[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1523; _temp1523.tag= Cyc_DeclSpec_tok;
_temp1523.f1=({ struct Cyc_Parse_Declaration_spec* _temp1524=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1524->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1524->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1524->type_specs=({ struct Cyc_List_List* _temp1525=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1525->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1525->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1525;});
_temp1524->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1524->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1524;}); _temp1523;}); _temp1522;}); break; case 31: _LL1521: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1527=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1527[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1528; _temp1528.tag= Cyc_DeclSpec_tok; _temp1528.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1529=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1529->sc= 0; _temp1529->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1529->type_specs=
0; _temp1529->is_inline= 0; _temp1529->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1529;});
_temp1528;}); _temp1527;}); break; case 32: _LL1526: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1531=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1531[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1532; _temp1532.tag= Cyc_DeclSpec_tok;
_temp1532.f1=({ struct Cyc_Parse_Declaration_spec* _temp1533=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1533->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1533->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1533->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1533->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1533->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1533;}); _temp1532;}); _temp1531;}); break; case 33: _LL1530: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1535=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1535[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1536; _temp1536.tag= Cyc_DeclSpec_tok; _temp1536.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1537=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1537->sc= 0; _temp1537->tq= Cyc_Absyn_empty_tqual(); _temp1537->type_specs=
0; _temp1537->is_inline= 1; _temp1537->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1537;});
_temp1536;}); _temp1535;}); break; case 34: _LL1534: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1539=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1539[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1540; _temp1540.tag= Cyc_DeclSpec_tok;
_temp1540.f1=({ struct Cyc_Parse_Declaration_spec* _temp1541=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1541->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1541->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1541->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1541->is_inline=
1; _temp1541->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1541;}); _temp1540;}); _temp1539;}); break; case 35: _LL1538: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1543=( struct Cyc_StorageClass_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1543[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1544; _temp1544.tag= Cyc_StorageClass_tok;
_temp1544.f1= Cyc_Parse_Auto_sc; _temp1544;}); _temp1543;}); break; case 36:
_LL1542: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1546=( struct
Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1546[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1547; _temp1547.tag=
Cyc_StorageClass_tok; _temp1547.f1= Cyc_Parse_Register_sc; _temp1547;});
_temp1546;}); break; case 37: _LL1545: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1549=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1549[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1550; _temp1550.tag= Cyc_StorageClass_tok; _temp1550.f1= Cyc_Parse_Static_sc;
_temp1550;}); _temp1549;}); break; case 38: _LL1548: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1552=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1552[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1553; _temp1553.tag= Cyc_StorageClass_tok; _temp1553.f1= Cyc_Parse_Extern_sc;
_temp1553;}); _temp1552;}); break; case 39: _LL1551: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1555=( struct Cyc_StorageClass_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1555[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1556; _temp1556.tag= Cyc_StorageClass_tok;
_temp1556.f1= Cyc_Parse_ExternC_sc; _temp1556;}); _temp1555;}); break; case 40:
_LL1554: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1558=( struct
Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1558[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1559; _temp1559.tag=
Cyc_StorageClass_tok; _temp1559.f1= Cyc_Parse_Typedef_sc; _temp1559;});
_temp1558;}); break; case 41: _LL1557: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1561=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1561[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1562; _temp1562.tag= Cyc_StorageClass_tok; _temp1562.f1= Cyc_Parse_Abstract_sc;
_temp1562;}); _temp1561;}); break; case 42: _LL1560: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1564=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1564[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1565; _temp1565.tag=
Cyc_AttributeList_tok; _temp1565.f1= 0; _temp1565;}); _temp1564;}); break; case
43: _LL1563: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1566: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1568=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1568[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1569; _temp1569.tag=
Cyc_AttributeList_tok; _temp1569.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1569;});
_temp1568;}); break; case 45: _LL1567: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1571=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1571[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1572; _temp1572.tag=
Cyc_AttributeList_tok; _temp1572.f1=({ struct Cyc_List_List* _temp1573=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1573->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1573->tl= 0; _temp1573;}); _temp1572;}); _temp1571;});
break; case 46: _LL1570: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1575=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1575[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1576; _temp1576.tag=
Cyc_AttributeList_tok; _temp1576.f1=({ struct Cyc_List_List* _temp1577=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1577->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1577->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1577;}); _temp1576;});
_temp1575;}); break; case 47: _LL1574: { struct _tagged_arr _temp1579= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1579, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1579, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1579, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1579,
sizeof( unsigned char),( int)( _get_arr_size( _temp1579, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1579,
sizeof( unsigned char),( int)( _get_arr_size( _temp1579, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1579=( struct _tagged_arr) Cyc_String_substring(
_temp1579, 2,( int)( _get_arr_size( _temp1579, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1579,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1580=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1580[ 0]=({ struct Cyc_Attribute_tok_struct _temp1581; _temp1581.tag= Cyc_Attribute_tok;
_temp1581.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1581;}); _temp1580;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1582=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1582[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1583; _temp1583.tag= Cyc_Attribute_tok; _temp1583.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1583;}); _temp1582;});} break;}} case
48: _LL1578: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1585=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1585[ 0]=({ struct Cyc_Attribute_tok_struct _temp1586; _temp1586.tag= Cyc_Attribute_tok;
_temp1586.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1586;}); _temp1585;});
break; case 49: _LL1584: { struct _tagged_arr _temp1588= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct
_tuple16 _temp1591; int _temp1592; struct _tuple16* _temp1589= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1591=*
_temp1589; _LL1593: _temp1592= _temp1591.f2; goto _LL1590; _LL1590: { void* a;
if( Cyc_String_zstrcmp( _temp1588, _tag_arr("regparm", sizeof( unsigned char), 8u))
== 0? 1: Cyc_String_zstrcmp( _temp1588, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) == 0){ if( _temp1592 <= 0? 1: _temp1592 > 3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 1 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1594=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1594[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1595; _temp1595.tag= Cyc_Absyn_Regparm_att; _temp1595.f1= _temp1592;
_temp1595;}); _temp1594;});} else{ if( Cyc_String_zstrcmp( _temp1588, _tag_arr("aligned",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1588, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) == 0){ if( _temp1592 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
unsigned int j=( unsigned int) _temp1592; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1596=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1596[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1597; _temp1597.tag= Cyc_Absyn_Aligned_att;
_temp1597.f1= _temp1592; _temp1597;}); _temp1596;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1598=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1598[ 0]=({ struct Cyc_Attribute_tok_struct _temp1599; _temp1599.tag= Cyc_Attribute_tok;
_temp1599.f1=( void*) a; _temp1599;}); _temp1598;}); break;}} case 50: _LL1587: {
struct _tagged_arr _temp1601= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct _tagged_arr
_temp1602= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); void* a; if( Cyc_String_zstrcmp( _temp1601, _tag_arr("section",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1601, _tag_arr("__section__",
sizeof( unsigned char), 12u)) == 0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1603=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1603[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1604; _temp1604.tag=
Cyc_Absyn_Section_att; _temp1604.f1= _temp1602; _temp1604;}); _temp1603;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1605=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1605[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1606; _temp1606.tag= Cyc_Attribute_tok; _temp1606.f1=(
void*) a; _temp1606;}); _temp1605;}); break;} case 51: _LL1600: { struct
_tagged_arr _temp1608= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)]); struct _tagged_arr
_temp1609= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); int _temp1612; struct _tuple16 _temp1610=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1613:
_temp1612= _temp1610.f2; goto _LL1611; _LL1611: { int _temp1616; struct _tuple16
_temp1614=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _LL1617: _temp1616= _temp1614.f2; goto _LL1615; _LL1615: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_String_zstrcmp( _temp1608,
_tag_arr("format", sizeof( unsigned char), 7u)) == 0? 1: Cyc_String_zstrcmp(
_temp1608, _tag_arr("__format__", sizeof( unsigned char), 11u)) == 0){ if( Cyc_String_zstrcmp(
_temp1609, _tag_arr("printf", sizeof( unsigned char), 7u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1618=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1618[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1619; _temp1619.tag= Cyc_Absyn_Format_att;
_temp1619.f1=( void*)(( void*) Cyc_Absyn_Printf_ft); _temp1619.f2= _temp1612;
_temp1619.f3= _temp1616; _temp1619;}); _temp1618;});} else{ if( Cyc_String_zstrcmp(
_temp1609, _tag_arr("scanf", sizeof( unsigned char), 6u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1620=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1620[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1621; _temp1621.tag= Cyc_Absyn_Format_att;
_temp1621.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft); _temp1621.f2= _temp1612;
_temp1621.f3= _temp1616; _temp1621;}); _temp1620;});} else{ Cyc_Parse_err(
_tag_arr("unrecognized format type", sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line));}}}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1622=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1622[ 0]=({ struct Cyc_Attribute_tok_struct _temp1623; _temp1623.tag= Cyc_Attribute_tok;
_temp1623.f1=( void*) a; _temp1623;}); _temp1622;}); break;}}} case 52: _LL1607:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1625=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1625[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1626; _temp1626.tag= Cyc_TypeSpecifier_tok;
_temp1626.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1626;});
_temp1625;}); break; case 53: _LL1624: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1628=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1628[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1629; _temp1629.tag=
Cyc_TypeSpecifier_tok; _temp1629.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1629;}); _temp1628;}); break; case 54: _LL1627:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1631=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1631[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1632; _temp1632.tag= Cyc_TypeSpecifier_tok;
_temp1632.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1632;});
_temp1631;}); break; case 55: _LL1630: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1634=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1634[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1635; _temp1635.tag=
Cyc_TypeSpecifier_tok; _temp1635.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1636=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1636[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1637; _temp1637.tag=
Cyc_Parse_Short_spec; _temp1637.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1637;});
_temp1636;})); _temp1635;}); _temp1634;}); break; case 56: _LL1633: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1639=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1639[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1640; _temp1640.tag= Cyc_TypeSpecifier_tok;
_temp1640.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1640;});
_temp1639;}); break; case 57: _LL1638: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1642=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1642[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1643; _temp1643.tag=
Cyc_TypeSpecifier_tok; _temp1643.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1644=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1644[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1645; _temp1645.tag= Cyc_Parse_Long_spec;
_temp1645.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1645;}); _temp1644;})); _temp1643;}); _temp1642;});
break; case 58: _LL1641: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1647=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1647[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1648; _temp1648.tag=
Cyc_TypeSpecifier_tok; _temp1648.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1648;}); _temp1647;}); break; case 59: _LL1646:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1650=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1650[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1651; _temp1651.tag= Cyc_TypeSpecifier_tok;
_temp1651.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1651;});
_temp1650;}); break; case 60: _LL1649: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1653=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1653[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1654; _temp1654.tag=
Cyc_TypeSpecifier_tok; _temp1654.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1655=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1655[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1656; _temp1656.tag=
Cyc_Parse_Signed_spec; _temp1656.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1656;});
_temp1655;})); _temp1654;}); _temp1653;}); break; case 61: _LL1652: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1658=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1658[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1659; _temp1659.tag= Cyc_TypeSpecifier_tok;
_temp1659.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1660=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1660[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1661; _temp1661.tag= Cyc_Parse_Unsigned_spec; _temp1661.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1661;});
_temp1660;})); _temp1659;}); _temp1658;}); break; case 62: _LL1657: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1662:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1663: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1664: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1666=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1666[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1667; _temp1667.tag=
Cyc_TypeSpecifier_tok; _temp1667.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1667;});
_temp1666;}); break; case 66: _LL1665: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1669=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1669[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1670; _temp1670.tag=
Cyc_TypeSpecifier_tok; _temp1670.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1671=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1671[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1672; _temp1672.tag= Cyc_Absyn_TypedefType;
_temp1672.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1672.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1672.f3= 0;
_temp1672;}); _temp1671;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1670;});
_temp1669;}); break; case 67: _LL1668: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1674=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1674[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1675; _temp1675.tag=
Cyc_TypeSpecifier_tok; _temp1675.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1676=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1676[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1677; _temp1677.tag= Cyc_Absyn_TupleType;
_temp1677.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); _temp1677;});
_temp1676;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1675;}); _temp1674;}); break; case 68:
_LL1673: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1679=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1679[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1680; _temp1680.tag=
Cyc_TypeSpecifier_tok; _temp1680.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1681=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1681[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1682; _temp1682.tag= Cyc_Absyn_RgnHandleType;
_temp1682.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1682;}); _temp1681;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1680;}); _temp1679;}); break; case 69: _LL1678: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1684=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1684[ 0]=({ struct Cyc_Kind_tok_struct _temp1685; _temp1685.tag= Cyc_Kind_tok;
_temp1685.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1685;});
_temp1684;}); break; case 70: _LL1683: { struct _tagged_arr* _temp1689; void*
_temp1691; struct _tuple1 _temp1687=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1692: _temp1691=
_temp1687.f1; goto _LL1690; _LL1690: _temp1689= _temp1687.f2; goto _LL1688;
_LL1688: if( _temp1691 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1693=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1693[ 0]=({ struct Cyc_Kind_tok_struct
_temp1694; _temp1694.tag= Cyc_Kind_tok; _temp1694.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1689, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1694;}); _temp1693;}); break;} case 71:
_LL1686: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1696=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1696[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1697; _temp1697.tag= Cyc_TypeQual_tok;
_temp1697.f1=({ struct Cyc_Absyn_Tqual _temp1698; _temp1698.q_const= 1;
_temp1698.q_volatile= 0; _temp1698.q_restrict= 0; _temp1698;}); _temp1697;});
_temp1696;}); break; case 72: _LL1695: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1700=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1700[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1701; _temp1701.tag= Cyc_TypeQual_tok; _temp1701.f1=({ struct Cyc_Absyn_Tqual
_temp1702; _temp1702.q_const= 0; _temp1702.q_volatile= 1; _temp1702.q_restrict=
0; _temp1702;}); _temp1701;}); _temp1700;}); break; case 73: _LL1699: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1704=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1704[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1705; _temp1705.tag= Cyc_TypeQual_tok;
_temp1705.f1=({ struct Cyc_Absyn_Tqual _temp1706; _temp1706.q_const= 0;
_temp1706.q_volatile= 0; _temp1706.q_restrict= 1; _temp1706;}); _temp1705;});
_temp1704;}); break; case 74: _LL1703: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1708=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1708[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1709; _temp1709.tag=
Cyc_TypeSpecifier_tok; _temp1709.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1710=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1710[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1711; _temp1711.tag= Cyc_Parse_Decl_spec;
_temp1711.f1=({ struct Cyc_Absyn_Decl* _temp1712=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1712->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1713=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1713[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1714; _temp1714.tag= Cyc_Absyn_Enum_d; _temp1714.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1715=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1715->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1715->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1715->fields=({
struct Cyc_Core_Opt* _temp1716=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1716->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1716;});
_temp1715;}); _temp1714;}); _temp1713;})); _temp1712->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1712;}); _temp1711;}); _temp1710;})); _temp1709;}); _temp1708;}); break;
case 75: _LL1707: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1718=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1718[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1719; _temp1719.tag=
Cyc_TypeSpecifier_tok; _temp1719.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1720=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1720[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1721; _temp1721.tag= Cyc_Absyn_EnumType;
_temp1721.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1721.f2= 0; _temp1721;}); _temp1720;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1719;}); _temp1718;}); break; case 76: _LL1717: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1723=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1723[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1724; _temp1724.tag= Cyc_Enumfield_tok;
_temp1724.f1=({ struct Cyc_Absyn_Enumfield* _temp1725=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1725->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1725->tag= 0;
_temp1725->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1725;});
_temp1724;}); _temp1723;}); break; case 77: _LL1722: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1727=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1727[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1728; _temp1728.tag= Cyc_Enumfield_tok;
_temp1728.f1=({ struct Cyc_Absyn_Enumfield* _temp1729=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1729->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1729->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1729->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1729;});
_temp1728;}); _temp1727;}); break; case 78: _LL1726: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1731=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1731[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1732; _temp1732.tag=
Cyc_EnumfieldList_tok; _temp1732.f1=({ struct Cyc_List_List* _temp1733=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1733->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1733->tl= 0; _temp1733;}); _temp1732;}); _temp1731;});
break; case 79: _LL1730: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1735=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1735[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1736; _temp1736.tag=
Cyc_EnumfieldList_tok; _temp1736.f1=({ struct Cyc_List_List* _temp1737=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1737->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1737->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1737;}); _temp1736;});
_temp1735;}); break; case 80: _LL1734: { void* t; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])){ case Cyc_Parse_Struct_su:
_LL1739: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp1741=( struct
Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct));
_temp1741[ 0]=({ struct Cyc_Absyn_AnonStructType_struct _temp1742; _temp1742.tag=
Cyc_Absyn_AnonStructType; _temp1742.f1= Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1742;});
_temp1741;}); break; case Cyc_Parse_Union_su: _LL1740: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1744=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1744[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1745; _temp1745.tag= Cyc_Absyn_AnonUnionType; _temp1745.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1745;});
_temp1744;}); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1746=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1746[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1747; _temp1747.tag=
Cyc_TypeSpecifier_tok; _temp1747.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1748=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1748[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1749; _temp1749.tag= Cyc_Parse_Type_spec;
_temp1749.f1=( void*) t; _temp1749.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1749;});
_temp1748;})); _temp1747;}); _temp1746;}); break;} case 81: _LL1738: { struct
Cyc_List_List* _temp1751=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)])){ case Cyc_Parse_Struct_su:
_LL1752: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1754=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1754->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1754;}), _temp1751,({ struct Cyc_Core_Opt*
_temp1755=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1755->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1755;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1753: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1757=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1757->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1757;}),
_temp1751,({ struct Cyc_Core_Opt* _temp1758=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1758->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1758;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1759=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1759[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1760; _temp1760.tag=
Cyc_TypeSpecifier_tok; _temp1760.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1761=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1761[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1762; _temp1762.tag= Cyc_Parse_Decl_spec;
_temp1762.f1= d; _temp1762;}); _temp1761;})); _temp1760;}); _temp1759;}); break;}
case 82: _LL1750: { struct Cyc_List_List* _temp1764=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d; switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)])){ case Cyc_Parse_Struct_su:
_LL1765: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp1767=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1767->v=( void*) Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp1767;}), _temp1764,({ struct Cyc_Core_Opt*
_temp1768=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1768->v=( void*) Cyc_yyget_StructFieldDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1768;}), 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break; case Cyc_Parse_Union_su: _LL1766: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1770=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1770->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1770;}),
_temp1764,({ struct Cyc_Core_Opt* _temp1771=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1771->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1771;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); break;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1772=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1772[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1773; _temp1773.tag=
Cyc_TypeSpecifier_tok; _temp1773.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1774=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1774[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1775; _temp1775.tag= Cyc_Parse_Decl_spec;
_temp1775.f1= d; _temp1775;}); _temp1774;})); _temp1773;}); _temp1772;}); break;}
case 83: _LL1763: switch( Cyc_yyget_StructOrUnion_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])){ case Cyc_Parse_Struct_su:
_LL1777: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1779=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1779[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1780; _temp1780.tag=
Cyc_TypeSpecifier_tok; _temp1780.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1781=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1781[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1782; _temp1782.tag= Cyc_Absyn_StructType;
_temp1782.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1782.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1782.f3= 0;
_temp1782;}); _temp1781;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1780;});
_temp1779;}); break; case Cyc_Parse_Union_su: _LL1778: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1784=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1784[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1785; _temp1785.tag= Cyc_TypeSpecifier_tok;
_temp1785.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1786=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1786[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1787; _temp1787.tag= Cyc_Absyn_UnionType;
_temp1787.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1787.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1787.f3= 0;
_temp1787;}); _temp1786;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1785;});
_temp1784;}); break;} break; case 84: _LL1776: switch( Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])){ case Cyc_Parse_Struct_su:
_LL1789: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1791=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1791[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1792; _temp1792.tag=
Cyc_TypeSpecifier_tok; _temp1792.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1793=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1793[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1794; _temp1794.tag= Cyc_Absyn_StructType;
_temp1794.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1794.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1794.f3= 0;
_temp1794;}); _temp1793;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1792;});
_temp1791;}); break; case Cyc_Parse_Union_su: _LL1790: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1796=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1796[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1797; _temp1797.tag= Cyc_TypeSpecifier_tok;
_temp1797.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp1798=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp1798[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp1799; _temp1799.tag= Cyc_Absyn_UnionType;
_temp1799.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1799.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1799.f3= 0;
_temp1799;}); _temp1798;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1797;});
_temp1796;}); break;} break; case 85: _LL1788: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1801=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1801[ 0]=({ struct Cyc_TypeList_tok_struct _temp1802; _temp1802.tag= Cyc_TypeList_tok;
_temp1802.f1= 0; _temp1802;}); _temp1801;}); break; case 86: _LL1800: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp1804=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1804[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1805; _temp1805.tag= Cyc_TypeList_tok; _temp1805.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1805;});
_temp1804;}); break; case 87: _LL1803:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1807=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1807[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1808; _temp1808.tag= Cyc_TypeList_tok; _temp1808.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1808;});
_temp1807;}); break; case 88: _LL1806: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1810=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1810[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1811; _temp1811.tag= Cyc_StructOrUnion_tok; _temp1811.f1= Cyc_Parse_Struct_su;
_temp1811;}); _temp1810;}); break; case 89: _LL1809: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1813=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1813[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1814; _temp1814.tag= Cyc_StructOrUnion_tok; _temp1814.f1= Cyc_Parse_Union_su;
_temp1814;}); _temp1813;}); break; case 90: _LL1812: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1816=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1816[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1817; _temp1817.tag= Cyc_StructFieldDeclList_tok; _temp1817.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1817;});
_temp1816;}); break; case 91: _LL1815: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1819=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1819[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1820; _temp1820.tag= Cyc_StructFieldDeclListList_tok; _temp1820.f1=({
struct Cyc_List_List* _temp1821=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1821->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1821->tl= 0;
_temp1821;}); _temp1820;}); _temp1819;}); break; case 92: _LL1818: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1823=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1823[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1824; _temp1824.tag= Cyc_StructFieldDeclListList_tok;
_temp1824.f1=({ struct Cyc_List_List* _temp1825=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1825->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1825->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1825;}); _temp1824;}); _temp1823;}); break;
case 93: _LL1822: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1827=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1827[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1828; _temp1828.tag=
Cyc_InitDeclList_tok; _temp1828.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1828;});
_temp1827;}); break; case 94: _LL1826: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1830=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1830[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1831; _temp1831.tag=
Cyc_InitDeclList_tok; _temp1831.f1=({ struct Cyc_List_List* _temp1832=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1832->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1832->tl= 0; _temp1832;}); _temp1831;}); _temp1830;});
break; case 95: _LL1829: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1834=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1834[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1835; _temp1835.tag=
Cyc_InitDeclList_tok; _temp1835.f1=({ struct Cyc_List_List* _temp1836=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1836->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1836->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1836;});
_temp1835;}); _temp1834;}); break; case 96: _LL1833: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1838=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1838[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1839; _temp1839.tag= Cyc_InitDecl_tok;
_temp1839.f1=({ struct _tuple15* _temp1840=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp1840->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1840->f2= 0;
_temp1840;}); _temp1839;}); _temp1838;}); break; case 97: _LL1837: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1842=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1842[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1843; _temp1843.tag= Cyc_InitDecl_tok; _temp1843.f1=({ struct _tuple15*
_temp1844=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1844->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1844->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1844;});
_temp1843;}); _temp1842;}); break; case 98: _LL1841: { struct _tuple18 _temp1848;
struct Cyc_List_List* _temp1849; struct Cyc_List_List* _temp1851; struct Cyc_Absyn_Tqual
_temp1853; struct _tuple18* _temp1846= Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1848=*
_temp1846; _LL1854: _temp1853= _temp1848.f1; goto _LL1852; _LL1852: _temp1851=
_temp1848.f2; goto _LL1850; _LL1850: _temp1849= _temp1848.f3; goto _LL1847;
_LL1847: { struct Cyc_List_List* _temp1857; struct Cyc_List_List* _temp1859;
struct _tuple0 _temp1855=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])); _LL1860: _temp1859= _temp1855.f1; goto _LL1858; _LL1858:
_temp1857= _temp1855.f2; goto _LL1856; _LL1856: { void* _temp1861= Cyc_Parse_speclist2typ(
_temp1851, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).last_line)); struct Cyc_List_List* _temp1862=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1853, _temp1861, _temp1859, _temp1849), _temp1857);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1863=( struct
Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1863[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1864; _temp1864.tag=
Cyc_StructFieldDeclList_tok; _temp1864.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), _temp1862); _temp1864;}); _temp1863;}); break;}}}
case 99: _LL1845: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1866=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1866[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1867; _temp1867.tag=
Cyc_QualSpecList_tok; _temp1867.f1=({ struct _tuple18* _temp1868=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1868->f1= Cyc_Absyn_empty_tqual();
_temp1868->f2=({ struct Cyc_List_List* _temp1869=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1869->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1869->tl=
0; _temp1869;}); _temp1868->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1868;}); _temp1867;});
_temp1866;}); break; case 100: _LL1865: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1871=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1871[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1872; _temp1872.tag=
Cyc_QualSpecList_tok; _temp1872.f1=({ struct _tuple18* _temp1873=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1873->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1873->f2=({
struct Cyc_List_List* _temp1874=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1874->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1874->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1874;}); _temp1873->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1873;});
_temp1872;}); _temp1871;}); break; case 101: _LL1870: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1876=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1876[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1877; _temp1877.tag= Cyc_QualSpecList_tok;
_temp1877.f1=({ struct _tuple18* _temp1878=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1878->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1878->f2= 0;
_temp1878->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1878;}); _temp1877;}); _temp1876;}); break; case
102: _LL1875: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1880=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1880[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1881; _temp1881.tag=
Cyc_QualSpecList_tok; _temp1881.f1=({ struct _tuple18* _temp1882=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1882->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1882->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1882->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1882;});
_temp1881;}); _temp1880;}); break; case 103: _LL1879: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp1884=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1884[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1885; _temp1885.tag= Cyc_DeclaratorExpoptList_tok;
_temp1885.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1885;}); _temp1884;}); break; case 104: _LL1883: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp1887=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1887[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1888; _temp1888.tag= Cyc_DeclaratorExpoptList_tok;
_temp1888.f1=({ struct Cyc_List_List* _temp1889=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1889->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1889->tl= 0;
_temp1889;}); _temp1888;}); _temp1887;}); break; case 105: _LL1886: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp1891=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1891[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1892; _temp1892.tag= Cyc_DeclaratorExpoptList_tok;
_temp1892.f1=({ struct Cyc_List_List* _temp1893=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1893->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1893->tl=
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1893;}); _temp1892;}); _temp1891;}); break; case 106:
_LL1890: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp1895=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp1895[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp1896; _temp1896.tag=
Cyc_DeclaratorExpopt_tok; _temp1896.f1=({ struct _tuple13* _temp1897=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1897->f1= Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1897->f2= 0;
_temp1897;}); _temp1896;}); _temp1895;}); break; case 107: _LL1894: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1899=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1899[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1900; _temp1900.tag= Cyc_DeclaratorExpopt_tok;
_temp1900.f1=({ struct _tuple13* _temp1901=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1901->f1=({ struct Cyc_Parse_Declarator*
_temp1903=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp1903->id=({ struct _tuple1* _temp1904=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp1904->f1= Cyc_Absyn_rel_ns_null; _temp1904->f2=
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1904;}); _temp1903->tms= 0; _temp1903;});
_temp1901->f2=({ struct Cyc_Core_Opt* _temp1902=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1902->v=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1902;});
_temp1901;}); _temp1900;}); _temp1899;}); break; case 108: _LL1898: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1906=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1906[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1907; _temp1907.tag= Cyc_DeclaratorExpopt_tok;
_temp1907.f1=({ struct _tuple13* _temp1908=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1908->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1908->f2=({
struct Cyc_Core_Opt* _temp1909=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1909->v=( void*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1909;}); _temp1908;});
_temp1907;}); _temp1906;}); break; case 109: _LL1905: { struct Cyc_List_List*
_temp1911=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1912=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1912[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1913; _temp1913.tag= Cyc_TypeSpecifier_tok;
_temp1913.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1914=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1914[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1915; _temp1915.tag= Cyc_Parse_Decl_spec;
_temp1915.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _temp1911,({
struct Cyc_Core_Opt* _temp1916=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1916->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1916;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1915;}); _temp1914;})); _temp1913;}); _temp1912;}); break;} case 110:
_LL1910: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1918=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1918[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1919; _temp1919.tag=
Cyc_TypeSpecifier_tok; _temp1919.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1920=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1920[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1921; _temp1921.tag= Cyc_Absyn_TunionType;
_temp1921.f1=({ struct Cyc_Absyn_TunionInfo _temp1922; _temp1922.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1923=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1923[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1924; _temp1924.tag= Cyc_Absyn_UnknownTunion;
_temp1924.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1925; _temp1925.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp1925.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1925;});
_temp1924;}); _temp1923;})); _temp1922.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1922.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp1922;}); _temp1921;}); _temp1920;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1919;}); _temp1918;}); break; case 111: _LL1917: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1927=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1927[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1928; _temp1928.tag= Cyc_TypeSpecifier_tok;
_temp1928.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1929=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1929[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1930; _temp1930.tag=
Cyc_Absyn_TunionType; _temp1930.f1=({ struct Cyc_Absyn_TunionInfo _temp1931;
_temp1931.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1932=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1932[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1933; _temp1933.tag= Cyc_Absyn_UnknownTunion; _temp1933.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1934; _temp1934.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1934.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp1934;}); _temp1933;}); _temp1932;})); _temp1931.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1931.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp1931;}); _temp1930;}); _temp1929;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1928;}); _temp1927;}); break; case 112:
_LL1926: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1936=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1936[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1937; _temp1937.tag=
Cyc_TypeSpecifier_tok; _temp1937.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1938=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1938[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1939; _temp1939.tag= Cyc_Absyn_TunionFieldType;
_temp1939.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1940; _temp1940.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp1941=( struct
Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp1941[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp1942; _temp1942.tag=
Cyc_Absyn_UnknownTunionfield; _temp1942.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp1943; _temp1943.tunion_name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1943.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp1943.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1943;});
_temp1942;}); _temp1941;})); _temp1940.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1940;}); _temp1939;});
_temp1938;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1937;}); _temp1936;}); break; case 113:
_LL1935: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1945=( struct Cyc_Bool_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1945[ 0]=({ struct
Cyc_Bool_tok_struct _temp1946; _temp1946.tag= Cyc_Bool_tok; _temp1946.f1= 0;
_temp1946;}); _temp1945;}); break; case 114: _LL1944: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1948=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1948[ 0]=({ struct Cyc_Bool_tok_struct
_temp1949; _temp1949.tag= Cyc_Bool_tok; _temp1949.f1= 1; _temp1949;}); _temp1948;});
break; case 115: _LL1947: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1951=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1951[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1952; _temp1952.tag= Cyc_TunionFieldList_tok; _temp1952.f1=({ struct Cyc_List_List*
_temp1953=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1953->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1953->tl= 0;
_temp1953;}); _temp1952;}); _temp1951;}); break; case 116: _LL1950: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1955=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1955[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1956; _temp1956.tag= Cyc_TunionFieldList_tok;
_temp1956.f1=({ struct Cyc_List_List* _temp1957=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1957->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1957->tl=
0; _temp1957;}); _temp1956;}); _temp1955;}); break; case 117: _LL1954: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1959=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1959[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1960; _temp1960.tag= Cyc_TunionFieldList_tok;
_temp1960.f1=({ struct Cyc_List_List* _temp1961=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1961->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1961->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1961;}); _temp1960;}); _temp1959;}); break; case 118:
_LL1958: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1963=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp1963[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp1964; _temp1964.tag=
Cyc_TunionFieldList_tok; _temp1964.f1=({ struct Cyc_List_List* _temp1965=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1965->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1965->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1965;}); _temp1964;});
_temp1963;}); break; case 119: _LL1962: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1967=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1967[ 0]=({ struct Cyc_Scope_tok_struct _temp1968; _temp1968.tag= Cyc_Scope_tok;
_temp1968.f1=( void*)(( void*) Cyc_Absyn_Public); _temp1968;}); _temp1967;});
break; case 120: _LL1966: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1970=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1970[ 0]=({ struct Cyc_Scope_tok_struct _temp1971; _temp1971.tag= Cyc_Scope_tok;
_temp1971.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp1971;}); _temp1970;});
break; case 121: _LL1969: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1973=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1973[ 0]=({ struct Cyc_Scope_tok_struct _temp1974; _temp1974.tag= Cyc_Scope_tok;
_temp1974.f1=( void*)(( void*) Cyc_Absyn_Static); _temp1974;}); _temp1973;});
break; case 122: _LL1972: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp1976=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp1976[ 0]=({ struct Cyc_TunionField_tok_struct _temp1977; _temp1977.tag= Cyc_TunionField_tok;
_temp1977.f1=({ struct Cyc_Absyn_Tunionfield* _temp1978=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1978->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1978->tvs= 0;
_temp1978->typs= 0; _temp1978->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1978->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1978;}); _temp1977;}); _temp1976;}); break; case 123:
_LL1975: { struct Cyc_List_List* _temp1980=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); struct Cyc_List_List*
_temp1981=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp1982=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp1982[ 0]=({ struct
Cyc_TunionField_tok_struct _temp1983; _temp1983.tag= Cyc_TunionField_tok;
_temp1983.f1=({ struct Cyc_Absyn_Tunionfield* _temp1984=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp1984->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1984->tvs=
_temp1981; _temp1984->typs= _temp1980; _temp1984->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1984->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp1984;}); _temp1983;}); _temp1982;}); break;}
case 124: _LL1979: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL1985: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp1987=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1987[ 0]=({ struct Cyc_Declarator_tok_struct _temp1988; _temp1988.tag= Cyc_Declarator_tok;
_temp1988.f1=({ struct Cyc_Parse_Declarator* _temp1989=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp1989->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp1989->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp1989;});
_temp1988;}); _temp1987;}); break; case 126: _LL1986: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp1991=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp1991[ 0]=({ struct
Cyc_Declarator_tok_struct _temp1992; _temp1992.tag= Cyc_Declarator_tok;
_temp1992.f1=({ struct Cyc_Parse_Declarator* _temp1993=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp1993->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1993->tms= 0;
_temp1993;}); _temp1992;}); _temp1991;}); break; case 127: _LL1990: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 128:
_LL1994: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp1996=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp1996[ 0]=({ struct Cyc_Declarator_tok_struct _temp1997; _temp1997.tag= Cyc_Declarator_tok;
_temp1997.f1=({ struct Cyc_Parse_Declarator* _temp1998=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp1998->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->id;
_temp1998->tms=({ struct Cyc_List_List* _temp1999=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1999->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp1999->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]))->tms; _temp1999;}); _temp1998;}); _temp1997;});
_temp1996;}); break; case 129: _LL1995: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2001=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2001[ 0]=({ struct Cyc_Declarator_tok_struct _temp2002; _temp2002.tag= Cyc_Declarator_tok;
_temp2002.f1=({ struct Cyc_Parse_Declarator* _temp2003=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2003->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2003->tms=({ struct Cyc_List_List* _temp2004=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2004->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2005=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2005[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2006; _temp2006.tag= Cyc_Absyn_ConstArray_mod;
_temp2006.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2006;}); _temp2005;})); _temp2004->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2004;}); _temp2003;}); _temp2002;}); _temp2001;}); break; case 130: _LL2000: {
struct _tuple17 _temp2010; struct Cyc_List_List* _temp2011; struct Cyc_Core_Opt*
_temp2013; struct Cyc_Absyn_VarargInfo* _temp2015; int _temp2017; struct Cyc_List_List*
_temp2019; struct _tuple17* _temp2008= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2010=*
_temp2008; _LL2020: _temp2019= _temp2010.f1; goto _LL2018; _LL2018: _temp2017=
_temp2010.f2; goto _LL2016; _LL2016: _temp2015= _temp2010.f3; goto _LL2014;
_LL2014: _temp2013= _temp2010.f4; goto _LL2012; _LL2012: _temp2011= _temp2010.f5;
goto _LL2009; _LL2009: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2021=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2021[ 0]=({ struct Cyc_Declarator_tok_struct _temp2022; _temp2022.tag= Cyc_Declarator_tok;
_temp2022.f1=({ struct Cyc_Parse_Declarator* _temp2023=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2023->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2023->tms=({ struct Cyc_List_List* _temp2024=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2024->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2025=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2025[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2026; _temp2026.tag= Cyc_Absyn_Function_mod;
_temp2026.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2027=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2027[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2028; _temp2028.tag= Cyc_Absyn_WithTypes;
_temp2028.f1= _temp2019; _temp2028.f2= _temp2017; _temp2028.f3= _temp2015;
_temp2028.f4= _temp2013; _temp2028.f5= _temp2011; _temp2028;}); _temp2027;}));
_temp2026;}); _temp2025;})); _temp2024->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2024;});
_temp2023;}); _temp2022;}); _temp2021;}); break;} case 131: _LL2007: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2030=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2030[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2031; _temp2031.tag= Cyc_Declarator_tok;
_temp2031.f1=({ struct Cyc_Parse_Declarator* _temp2032=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2032->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->id;
_temp2032->tms=({ struct Cyc_List_List* _temp2033=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2033->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2034=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2034[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2035; _temp2035.tag= Cyc_Absyn_Function_mod;
_temp2035.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2036=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2036[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2037; _temp2037.tag= Cyc_Absyn_WithTypes;
_temp2037.f1= 0; _temp2037.f2= 0; _temp2037.f3= 0; _temp2037.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2037.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2037;}); _temp2036;})); _temp2035;}); _temp2034;}));
_temp2033->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]))->tms; _temp2033;}); _temp2032;}); _temp2031;});
_temp2030;}); break; case 132: _LL2029: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2039=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2039[ 0]=({ struct Cyc_Declarator_tok_struct _temp2040; _temp2040.tag= Cyc_Declarator_tok;
_temp2040.f1=({ struct Cyc_Parse_Declarator* _temp2041=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2041->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2041->tms=({ struct Cyc_List_List* _temp2042=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2042->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2043=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2043[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2044; _temp2044.tag= Cyc_Absyn_Function_mod;
_temp2044.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2045=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2045[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2046; _temp2046.tag= Cyc_Absyn_NoTypes;
_temp2046.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2046.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2046;});
_temp2045;})); _temp2044;}); _temp2043;})); _temp2042->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2042;}); _temp2041;}); _temp2040;}); _temp2039;}); break; case 133: _LL2038: {
struct Cyc_List_List* _temp2048=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2049=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2049[ 0]=({ struct Cyc_Declarator_tok_struct _temp2050; _temp2050.tag= Cyc_Declarator_tok;
_temp2050.f1=({ struct Cyc_Parse_Declarator* _temp2051=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2051->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2051->tms=({ struct Cyc_List_List* _temp2052=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2052->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2053=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2053[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2054; _temp2054.tag= Cyc_Absyn_TypeParams_mod;
_temp2054.f1= _temp2048; _temp2054.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2054.f3=
0; _temp2054;}); _temp2053;})); _temp2052->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2052;});
_temp2051;}); _temp2050;}); _temp2049;}); break;} case 134: _LL2047:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2056=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2057=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2057[ 0]=({ struct Cyc_Declarator_tok_struct _temp2058; _temp2058.tag= Cyc_Declarator_tok;
_temp2058.f1=({ struct Cyc_Parse_Declarator* _temp2059=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2059->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2059->tms=({ struct Cyc_List_List* _temp2060=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2060->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2061=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2061[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2062; _temp2062.tag= Cyc_Absyn_TypeParams_mod;
_temp2062.f1= _temp2056; _temp2062.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2062.f3=
0; _temp2062;}); _temp2061;})); _temp2060->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2060;});
_temp2059;}); _temp2058;}); _temp2057;}); break;} case 135: _LL2055: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2064=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2064[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2065; _temp2065.tag= Cyc_Declarator_tok;
_temp2065.f1=({ struct Cyc_Parse_Declarator* _temp2066=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2066->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->id;
_temp2066->tms=({ struct Cyc_List_List* _temp2067=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2067->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2068=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2068[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2069; _temp2069.tag= Cyc_Absyn_Attributes_mod;
_temp2069.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2069.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2069;}); _temp2068;}));
_temp2067->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]))->tms; _temp2067;}); _temp2066;}); _temp2065;});
_temp2064;}); break; case 136: _LL2063: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2071=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2071[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2072; _temp2072.tag= Cyc_Declarator_tok;
_temp2072.f1=({ struct Cyc_Parse_Declarator* _temp2073=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2073->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2073->tms= 0;
_temp2073;}); _temp2072;}); _temp2071;}); break; case 137: _LL2070: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2075=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2075[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2076; _temp2076.tag= Cyc_Declarator_tok;
_temp2076.f1=({ struct Cyc_Parse_Declarator* _temp2077=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2077->id= Cyc_Absyn_exn_name;
_temp2077->tms= 0; _temp2077;}); _temp2076;}); _temp2075;}); break; case 138:
_LL2074: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2079=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2079[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2080; _temp2080.tag=
Cyc_TypeModifierList_tok; _temp2080.f1=({ struct Cyc_List_List* _temp2081=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2081->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2082=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2082[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2083; _temp2083.tag= Cyc_Absyn_Pointer_mod;
_temp2083.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2083.f2=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2083.f3= Cyc_Absyn_empty_tqual(); _temp2083;}); _temp2082;}));
_temp2081->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2081;});
_temp2080;}); _temp2079;}); break; case 139: _LL2078: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2085=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2085[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2086; _temp2086.tag= Cyc_TypeModifierList_tok;
_temp2086.f1=({ struct Cyc_List_List* _temp2087=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2087->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2088=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2088[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2089; _temp2089.tag= Cyc_Absyn_Pointer_mod;
_temp2089.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2089.f2=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2089.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2089;}); _temp2088;})); _temp2087->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0); _temp2087;});
_temp2086;}); _temp2085;}); break; case 140: _LL2084: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2091=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2091[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2092; _temp2092.tag= Cyc_TypeModifierList_tok;
_temp2092.f1=({ struct Cyc_List_List* _temp2093=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2093->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2094=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2094[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2095; _temp2095.tag= Cyc_Absyn_Pointer_mod;
_temp2095.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2095.f2=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2095.f3= Cyc_Absyn_empty_tqual(); _temp2095;}); _temp2094;}));
_temp2093->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2093;});
_temp2092;}); _temp2091;}); break; case 141: _LL2090: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2097=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2097[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2098; _temp2098.tag= Cyc_TypeModifierList_tok;
_temp2098.f1=({ struct Cyc_List_List* _temp2099=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2099->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2100=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2100[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2101; _temp2101.tag= Cyc_Absyn_Pointer_mod;
_temp2101.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2101.f2=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2101.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2101;}); _temp2100;})); _temp2099->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2099;});
_temp2098;}); _temp2097;}); break; case 142: _LL2096: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2103=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2103[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2104; _temp2104.tag= Cyc_Pointer_Sort_tok;
_temp2104.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2105=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2105[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2106; _temp2106.tag=
Cyc_Absyn_Nullable_ps; _temp2106.f1= Cyc_Absyn_exp_unsigned_one; _temp2106;});
_temp2105;})); _temp2104;}); _temp2103;}); break; case 143: _LL2102: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2108=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2108[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2109; _temp2109.tag= Cyc_Pointer_Sort_tok;
_temp2109.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2110=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2110[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2111; _temp2111.tag= Cyc_Absyn_NonNullable_ps; _temp2111.f1= Cyc_Absyn_exp_unsigned_one;
_temp2111;}); _temp2110;})); _temp2109;}); _temp2108;}); break; case 144:
_LL2107: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2113=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2113[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2114; _temp2114.tag=
Cyc_Pointer_Sort_tok; _temp2114.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2115=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2115[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2116; _temp2116.tag=
Cyc_Absyn_Nullable_ps; _temp2116.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2116;});
_temp2115;})); _temp2114;}); _temp2113;}); break; case 145: _LL2112: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2118=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2118[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2119; _temp2119.tag= Cyc_Pointer_Sort_tok;
_temp2119.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2120=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2120[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2121; _temp2121.tag= Cyc_Absyn_NonNullable_ps; _temp2121.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2121;});
_temp2120;})); _temp2119;}); _temp2118;}); break; case 146: _LL2117: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2123=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2123[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2124; _temp2124.tag= Cyc_Pointer_Sort_tok;
_temp2124.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2124;});
_temp2123;}); break; case 147: _LL2122: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2126=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2126[ 0]=({ struct Cyc_Type_tok_struct _temp2127; _temp2127.tag= Cyc_Type_tok;
_temp2127.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2127;}); _temp2126;});
break; case 148: _LL2125: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2128: if( ! Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind)){
Cyc_Parse_err( _tag_arr("expecting region kind", sizeof( unsigned char), 22u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 150: _LL2129: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2131=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2131[ 0]=({ struct Cyc_Type_tok_struct _temp2132; _temp2132.tag= Cyc_Type_tok;
_temp2132.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2133=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2133->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2133;}), 0); _temp2132;}); _temp2131;});
break; case 151: _LL2130: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 152: _LL2134: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2136=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2136[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2137; _temp2137.tag= Cyc_TypeQual_tok; _temp2137.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2137;});
_temp2136;}); break; case 153: _LL2135: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2139=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2139[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2140; _temp2140.tag= Cyc_ParamDeclListBool_tok; _temp2140.f1=({ struct
_tuple17* _temp2141=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2141->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])); _temp2141->f2= 0; _temp2141->f3= 0; _temp2141->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2141->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2141;}); _temp2140;}); _temp2139;}); break; case 154:
_LL2138: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2143=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp2143[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp2144; _temp2144.tag=
Cyc_ParamDeclListBool_tok; _temp2144.f1=({ struct _tuple17* _temp2145=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2145->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)])); _temp2145->f2= 1;
_temp2145->f3= 0; _temp2145->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2145->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2145;});
_temp2144;}); _temp2143;}); break; case 155: _LL2142: { struct _tuple2 _temp2149;
void* _temp2150; struct Cyc_Absyn_Tqual _temp2152; struct Cyc_Core_Opt*
_temp2154; struct _tuple2* _temp2147= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2149=*
_temp2147; _LL2155: _temp2154= _temp2149.f1; goto _LL2153; _LL2153: _temp2152=
_temp2149.f2; goto _LL2151; _LL2151: _temp2150= _temp2149.f3; goto _LL2148;
_LL2148: { struct Cyc_Absyn_VarargInfo* _temp2156=({ struct Cyc_Absyn_VarargInfo*
_temp2160=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2160->name= _temp2154; _temp2160->tq= _temp2152; _temp2160->type=( void*)
_temp2150; _temp2160->rgn=( void*) Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2160->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2160;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2157=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2157[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2158; _temp2158.tag= Cyc_ParamDeclListBool_tok; _temp2158.f1=({ struct
_tuple17* _temp2159=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2159->f1= 0; _temp2159->f2= 0; _temp2159->f3= _temp2156; _temp2159->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2159->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2159;}); _temp2158;}); _temp2157;}); break;}} case 156:
_LL2146: { struct _tuple2 _temp2164; void* _temp2165; struct Cyc_Absyn_Tqual
_temp2167; struct Cyc_Core_Opt* _temp2169; struct _tuple2* _temp2162= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2164=*
_temp2162; _LL2170: _temp2169= _temp2164.f1; goto _LL2168; _LL2168: _temp2167=
_temp2164.f2; goto _LL2166; _LL2166: _temp2165= _temp2164.f3; goto _LL2163;
_LL2163: { struct Cyc_Absyn_VarargInfo* _temp2171=({ struct Cyc_Absyn_VarargInfo*
_temp2175=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2175->name= _temp2169; _temp2175->tq= _temp2167; _temp2175->type=( void*)
_temp2165; _temp2175->rgn=( void*) Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2175->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2175;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2172=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2172[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2173; _temp2173.tag= Cyc_ParamDeclListBool_tok; _temp2173.f1=({ struct
_tuple17* _temp2174=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2174->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 7)])); _temp2174->f2= 0; _temp2174->f3= _temp2171; _temp2174->f4=
Cyc_yyget_TypeOpt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2174->f5= Cyc_yyget_Rgnorder_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2174;}); _temp2173;});
_temp2172;}); break;}} case 157: _LL2161: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2177=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2177[ 0]=({ struct Cyc_Type_tok_struct _temp2178; _temp2178.tag= Cyc_Type_tok;
_temp2178.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_empty_conref());
_temp2178;}); _temp2177;}); break; case 158: _LL2176: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2180=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2180[ 0]=({ struct Cyc_Type_tok_struct
_temp2181; _temp2181.tag= Cyc_Type_tok; _temp2181.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp2181;});
_temp2180;}); break; case 159: _LL2179: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2183=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2183[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2184; _temp2184.tag= Cyc_TypeOpt_tok;
_temp2184.f1= 0; _temp2184;}); _temp2183;}); break; case 160: _LL2182: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2186=( struct Cyc_TypeOpt_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2186[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2187; _temp2187.tag= Cyc_TypeOpt_tok; _temp2187.f1=({ struct Cyc_Core_Opt*
_temp2188=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2188->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2189=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2189[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2190; _temp2190.tag= Cyc_Absyn_JoinEff;
_temp2190.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2190;}); _temp2189;})); _temp2188;}); _temp2187;});
_temp2186;}); break; case 161: _LL2185: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2192=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2192[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2193; _temp2193.tag= Cyc_Rgnorder_tok;
_temp2193.f1= 0; _temp2193;}); _temp2192;}); break; case 162: _LL2191: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL2194: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2196=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2196[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2197; _temp2197.tag= Cyc_Rgnorder_tok; _temp2197.f1=({ struct Cyc_List_List*
_temp2198=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2198->hd=( void*)({ struct _tuple19* _temp2199=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2199->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2199->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2199;}); _temp2198->tl= 0; _temp2198;});
_temp2197;}); _temp2196;}); break; case 164: _LL2195: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2201=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2201[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2202; _temp2202.tag= Cyc_Rgnorder_tok; _temp2202.f1=({ struct Cyc_List_List*
_temp2203=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2203->hd=( void*)({ struct _tuple19* _temp2204=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2204->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2204->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2204;}); _temp2203->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2203;});
_temp2202;}); _temp2201;}); break; case 165: _LL2200: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2206=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2206[ 0]=({ struct Cyc_Bool_tok_struct
_temp2207; _temp2207.tag= Cyc_Bool_tok; _temp2207.f1= 0; _temp2207;}); _temp2206;});
break; case 166: _LL2205: { struct _tagged_arr _temp2209= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_String_zstrcmp(
_temp2209, _tag_arr("inject", sizeof( unsigned char), 7u)) != 0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2210=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2210[ 0]=({ struct Cyc_Bool_tok_struct _temp2211; _temp2211.tag= Cyc_Bool_tok;
_temp2211.f1= 1; _temp2211;}); _temp2210;}); break;} case 167: _LL2208: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168:
_LL2212: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2214=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2214[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2215; _temp2215.tag= Cyc_TypeList_tok; _temp2215.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2215;});
_temp2214;}); break; case 169: _LL2213: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2217=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2217[ 0]=({ struct Cyc_TypeList_tok_struct _temp2218; _temp2218.tag= Cyc_TypeList_tok;
_temp2218.f1= 0; _temp2218;}); _temp2217;}); break; case 170: _LL2216: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case
171: _LL2219: if( ! Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_EffKind)){
Cyc_Parse_err( _tag_arr("expecting effect kind", sizeof( unsigned char), 22u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2221=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2221[ 0]=({ struct Cyc_TypeList_tok_struct _temp2222; _temp2222.tag= Cyc_TypeList_tok;
_temp2222.f1=({ struct Cyc_List_List* _temp2223=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2223->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2223->tl= 0;
_temp2223;}); _temp2222;}); _temp2221;}); break; case 172: _LL2220: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2225=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2225[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2226; _temp2226.tag= Cyc_TypeList_tok; _temp2226.f1=({ struct Cyc_List_List*
_temp2227=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2227->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2228=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2228[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2229; _temp2229.tag= Cyc_Absyn_AccessEff;
_temp2229.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2229;}); _temp2228;})); _temp2227->tl= 0;
_temp2227;}); _temp2226;}); _temp2225;}); break; case 173: _LL2224: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),( void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2231=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2231[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2232; _temp2232.tag= Cyc_TypeList_tok; _temp2232.f1=({ struct Cyc_List_List*
_temp2233=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2233->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2234=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2234[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2235; _temp2235.tag= Cyc_Absyn_AccessEff;
_temp2235.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2235;}); _temp2234;})); _temp2233->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2233;});
_temp2232;}); _temp2231;}); break; case 174: _LL2230: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2237=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2237[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2238; _temp2238.tag= Cyc_ParamDeclList_tok;
_temp2238.f1=({ struct Cyc_List_List* _temp2239=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2239->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2239->tl= 0;
_temp2239;}); _temp2238;}); _temp2237;}); break; case 175: _LL2236: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2241=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2241[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2242; _temp2242.tag= Cyc_ParamDeclList_tok;
_temp2242.f1=({ struct Cyc_List_List* _temp2243=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2243->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2243->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2243;}); _temp2242;}); _temp2241;}); break; case 176:
_LL2240: { struct _tuple18 _temp2247; struct Cyc_List_List* _temp2248; struct
Cyc_List_List* _temp2250; struct Cyc_Absyn_Tqual _temp2252; struct _tuple18*
_temp2245= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2247=* _temp2245; _LL2253: _temp2252=
_temp2247.f1; goto _LL2251; _LL2251: _temp2250= _temp2247.f2; goto _LL2249;
_LL2249: _temp2248= _temp2247.f3; goto _LL2246; _LL2246: { struct Cyc_Parse_Declarator
_temp2256; struct Cyc_List_List* _temp2257; struct _tuple1* _temp2259; struct
Cyc_Parse_Declarator* _temp2254= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2256=* _temp2254;
_LL2260: _temp2259= _temp2256.id; goto _LL2258; _LL2258: _temp2257= _temp2256.tms;
goto _LL2255; _LL2255: { void* _temp2261= Cyc_Parse_speclist2typ( _temp2250, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
struct Cyc_List_List* _temp2264; struct Cyc_List_List* _temp2266; void*
_temp2268; struct Cyc_Absyn_Tqual _temp2270; struct _tuple6 _temp2262= Cyc_Parse_apply_tms(
_temp2252, _temp2261, _temp2248, _temp2257); _LL2271: _temp2270= _temp2262.f1;
goto _LL2269; _LL2269: _temp2268= _temp2262.f2; goto _LL2267; _LL2267: _temp2266=
_temp2262.f3; goto _LL2265; _LL2265: _temp2264= _temp2262.f4; goto _LL2263;
_LL2263: if( _temp2266 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2259)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
struct Cyc_Core_Opt* _temp2272=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2276=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2276->v=( void*)(* _temp2259).f2; _temp2276;}); if( _temp2264 != 0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2273=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2273[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2274; _temp2274.tag= Cyc_ParamDecl_tok;
_temp2274.f1=({ struct _tuple2* _temp2275=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp2275->f1= _temp2272; _temp2275->f2= _temp2270; _temp2275->f3=
_temp2268; _temp2275;}); _temp2274;}); _temp2273;}); break;}}}} case 177:
_LL2244: { struct _tuple18 _temp2280; struct Cyc_List_List* _temp2281; struct
Cyc_List_List* _temp2283; struct Cyc_Absyn_Tqual _temp2285; struct _tuple18*
_temp2278= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2280=* _temp2278; _LL2286: _temp2285= _temp2280.f1;
goto _LL2284; _LL2284: _temp2283= _temp2280.f2; goto _LL2282; _LL2282: _temp2281=
_temp2280.f3; goto _LL2279; _LL2279: { void* _temp2287= Cyc_Parse_speclist2typ(
_temp2283, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2281 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2288=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2288[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2289; _temp2289.tag= Cyc_ParamDecl_tok; _temp2289.f1=({
struct _tuple2* _temp2290=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2290->f1= 0; _temp2290->f2= _temp2285; _temp2290->f3= _temp2287; _temp2290;});
_temp2289;}); _temp2288;}); break;}} case 178: _LL2277: { struct _tuple18
_temp2294; struct Cyc_List_List* _temp2295; struct Cyc_List_List* _temp2297;
struct Cyc_Absyn_Tqual _temp2299; struct _tuple18* _temp2292= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2294=*
_temp2292; _LL2300: _temp2299= _temp2294.f1; goto _LL2298; _LL2298: _temp2297=
_temp2294.f2; goto _LL2296; _LL2296: _temp2295= _temp2294.f3; goto _LL2293;
_LL2293: { void* _temp2301= Cyc_Parse_speclist2typ( _temp2297, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
struct Cyc_List_List* _temp2302=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2305; struct Cyc_List_List* _temp2307; void* _temp2309; struct Cyc_Absyn_Tqual
_temp2311; struct _tuple6 _temp2303= Cyc_Parse_apply_tms( _temp2299, _temp2301,
_temp2295, _temp2302); _LL2312: _temp2311= _temp2303.f1; goto _LL2310; _LL2310:
_temp2309= _temp2303.f2; goto _LL2308; _LL2308: _temp2307= _temp2303.f3; goto
_LL2306; _LL2306: _temp2305= _temp2303.f4; goto _LL2304; _LL2304: if( _temp2307
!= 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2305 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2313=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2313[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2314; _temp2314.tag= Cyc_ParamDecl_tok; _temp2314.f1=({
struct _tuple2* _temp2315=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2315->f1= 0; _temp2315->f2= _temp2311; _temp2315->f3= _temp2309; _temp2315;});
_temp2314;}); _temp2313;}); break;}} case 179: _LL2291: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2317=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2317[ 0]=({ struct Cyc_IdList_tok_struct
_temp2318; _temp2318.tag= Cyc_IdList_tok; _temp2318.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2318;});
_temp2317;}); break; case 180: _LL2316: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2320=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2320[ 0]=({ struct Cyc_IdList_tok_struct _temp2321; _temp2321.tag= Cyc_IdList_tok;
_temp2321.f1=({ struct Cyc_List_List* _temp2322=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2322->hd=( void*)({ struct
_tagged_arr* _temp2323=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2323[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2323;}); _temp2322->tl=
0; _temp2322;}); _temp2321;}); _temp2320;}); break; case 181: _LL2319: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2325=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2325[ 0]=({ struct Cyc_IdList_tok_struct
_temp2326; _temp2326.tag= Cyc_IdList_tok; _temp2326.f1=({ struct Cyc_List_List*
_temp2327=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2327->hd=( void*)({ struct _tagged_arr* _temp2328=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp2328[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2328;});
_temp2327->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2327;}); _temp2326;}); _temp2325;}); break; case 182:
_LL2324: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 183: _LL2329: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 184: _LL2330: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2332=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2332[ 0]=({ struct Cyc_Exp_tok_struct _temp2333; _temp2333.tag= Cyc_Exp_tok;
_temp2333.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2334=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2334[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2335; _temp2335.tag= Cyc_Absyn_UnresolvedMem_e; _temp2335.f1= 0; _temp2335.f2=
0; _temp2335;}); _temp2334;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2333;});
_temp2332;}); break; case 185: _LL2331: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2337=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2337[ 0]=({ struct Cyc_Exp_tok_struct _temp2338; _temp2338.tag= Cyc_Exp_tok;
_temp2338.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2339=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2339[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2340; _temp2340.tag= Cyc_Absyn_UnresolvedMem_e; _temp2340.f1= 0; _temp2340.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2340;});
_temp2339;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2338;}); _temp2337;}); break; case 186:
_LL2336: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2342=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2342[ 0]=({ struct Cyc_Exp_tok_struct
_temp2343; _temp2343.tag= Cyc_Exp_tok; _temp2343.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2344=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2344[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2345; _temp2345.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2345.f1= 0; _temp2345.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp2345;});
_temp2344;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2343;}); _temp2342;}); break; case 187:
_LL2341: { struct Cyc_Absyn_Vardecl* _temp2347= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2353=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2353->f1=( void*) Cyc_Absyn_Loc_n; _temp2353->f2=({ struct _tagged_arr*
_temp2354=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp2354[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); _temp2354;}); _temp2353;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line)));
_temp2347->tq=({ struct Cyc_Absyn_Tqual _temp2348; _temp2348.q_const= 1;
_temp2348.q_volatile= 0; _temp2348.q_restrict= 1; _temp2348;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2349=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2349[ 0]=({ struct Cyc_Exp_tok_struct
_temp2350; _temp2350.tag= Cyc_Exp_tok; _temp2350.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2351=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2351[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2352; _temp2352.tag= Cyc_Absyn_Comprehension_e;
_temp2352.f1= _temp2347; _temp2352.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2352.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2352;});
_temp2351;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2350;}); _temp2349;}); break;} case
188: _LL2346: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2356=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2356[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2357; _temp2357.tag=
Cyc_InitializerList_tok; _temp2357.f1=({ struct Cyc_List_List* _temp2358=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2358->hd=(
void*)({ struct _tuple20* _temp2359=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2359->f1= 0; _temp2359->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2359;}); _temp2358->tl=
0; _temp2358;}); _temp2357;}); _temp2356;}); break; case 189: _LL2355: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2361=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2361[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2362; _temp2362.tag= Cyc_InitializerList_tok;
_temp2362.f1=({ struct Cyc_List_List* _temp2363=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2363->hd=( void*)({ struct
_tuple20* _temp2364=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2364->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2364->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2364;});
_temp2363->tl= 0; _temp2363;}); _temp2362;}); _temp2361;}); break; case 190:
_LL2360: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2366=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2366[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2367; _temp2367.tag=
Cyc_InitializerList_tok; _temp2367.f1=({ struct Cyc_List_List* _temp2368=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2368->hd=(
void*)({ struct _tuple20* _temp2369=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2369->f1= 0; _temp2369->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2369;}); _temp2368->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2368;}); _temp2367;}); _temp2366;}); break; case 191:
_LL2365: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2371=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2371[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2372; _temp2372.tag=
Cyc_InitializerList_tok; _temp2372.f1=({ struct Cyc_List_List* _temp2373=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2373->hd=(
void*)({ struct _tuple20* _temp2374=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2374->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2374->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2374;});
_temp2373->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2373;});
_temp2372;}); _temp2371;}); break; case 192: _LL2370: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2376=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2376[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2377; _temp2377.tag= Cyc_DesignatorList_tok;
_temp2377.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2377;});
_temp2376;}); break; case 193: _LL2375: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2379=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2379[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2380; _temp2380.tag=
Cyc_DesignatorList_tok; _temp2380.f1=({ struct Cyc_List_List* _temp2381=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2381->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2381->tl= 0; _temp2381;}); _temp2380;}); _temp2379;});
break; case 194: _LL2378: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2383=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2383[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2384; _temp2384.tag=
Cyc_DesignatorList_tok; _temp2384.f1=({ struct Cyc_List_List* _temp2385=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2385->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2385->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2385;});
_temp2384;}); _temp2383;}); break; case 195: _LL2382: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2387=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2387[ 0]=({ struct
Cyc_Designator_tok_struct _temp2388; _temp2388.tag= Cyc_Designator_tok;
_temp2388.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2389=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2389[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2390; _temp2390.tag=
Cyc_Absyn_ArrayElement; _temp2390.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2390;});
_temp2389;})); _temp2388;}); _temp2387;}); break; case 196: _LL2386: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2392=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2392[ 0]=({ struct
Cyc_Designator_tok_struct _temp2393; _temp2393.tag= Cyc_Designator_tok;
_temp2393.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2394=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2394[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2395; _temp2395.tag= Cyc_Absyn_FieldName;
_temp2395.f1=({ struct _tagged_arr* _temp2396=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp2396[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2396;}); _temp2395;});
_temp2394;})); _temp2393;}); _temp2392;}); break; case 197: _LL2391: { void*
_temp2398= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2399=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2399 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2400=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2401=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2401[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2402; _temp2402.tag= Cyc_ParamDecl_tok; _temp2402.f1=({
struct _tuple2* _temp2403=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2403->f1= 0; _temp2403->f2= _temp2400; _temp2403->f3= _temp2398; _temp2403;});
_temp2402;}); _temp2401;}); break;}} case 198: _LL2397: { void* _temp2405= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2406=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2407=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2408=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2409= Cyc_Parse_apply_tms( _temp2407, _temp2405, _temp2406,
_temp2408); if( _temp2409.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2409.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring", sizeof(
unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2410=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2410[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2411; _temp2411.tag= Cyc_ParamDecl_tok; _temp2411.f1=({
struct _tuple2* _temp2412=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2412->f1= 0; _temp2412->f2= _temp2409.f1; _temp2412->f3= _temp2409.f2;
_temp2412;}); _temp2411;}); _temp2410;}); break;} case 199: _LL2404: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2414=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2414[ 0]=({ struct Cyc_Type_tok_struct
_temp2415; _temp2415.tag= Cyc_Type_tok; _temp2415.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2415;});
_temp2414;}); break; case 200: _LL2413: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2417=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2417[ 0]=({ struct Cyc_Type_tok_struct _temp2418; _temp2418.tag= Cyc_Type_tok;
_temp2418.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2419=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2419[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2420; _temp2420.tag= Cyc_Absyn_JoinEff;
_temp2420.f1= 0; _temp2420;}); _temp2419;})); _temp2418;}); _temp2417;}); break;
case 201: _LL2416: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2422=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2422[ 0]=({ struct Cyc_Type_tok_struct _temp2423; _temp2423.tag= Cyc_Type_tok;
_temp2423.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2424=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2424[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2425; _temp2425.tag= Cyc_Absyn_JoinEff;
_temp2425.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2425;}); _temp2424;})); _temp2423;});
_temp2422;}); break; case 202: _LL2421: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2427=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2427[ 0]=({ struct Cyc_Type_tok_struct _temp2428; _temp2428.tag= Cyc_Type_tok;
_temp2428.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2429=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2429[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2430; _temp2430.tag= Cyc_Absyn_JoinEff;
_temp2430.f1=({ struct Cyc_List_List* _temp2431=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2431->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2431->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2431;}); _temp2430;}); _temp2429;})); _temp2428;});
_temp2427;}); break; case 203: _LL2426: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2433=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2433[ 0]=({ struct Cyc_TypeList_tok_struct _temp2434; _temp2434.tag= Cyc_TypeList_tok;
_temp2434.f1=({ struct Cyc_List_List* _temp2435=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2435->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2435->tl= 0;
_temp2435;}); _temp2434;}); _temp2433;}); break; case 204: _LL2432: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2437=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2437[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2438; _temp2438.tag= Cyc_TypeList_tok; _temp2438.f1=({ struct Cyc_List_List*
_temp2439=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2439->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2439->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2439;});
_temp2438;}); _temp2437;}); break; case 205: _LL2436: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2441=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2441[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2442; _temp2442.tag= Cyc_AbstractDeclarator_tok;
_temp2442.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2443=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2443->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2443;});
_temp2442;}); _temp2441;}); break; case 206: _LL2440: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 207: _LL2444:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2446=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2446[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2447; _temp2447.tag=
Cyc_AbstractDeclarator_tok; _temp2447.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2448=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2448->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2448;});
_temp2447;}); _temp2446;}); break; case 208: _LL2445: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 209:
_LL2449: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2451=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2451[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2452; _temp2452.tag=
Cyc_AbstractDeclarator_tok; _temp2452.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2453=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2453->tms=({ struct Cyc_List_List* _temp2454=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2454->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2454->tl= 0; _temp2454;}); _temp2453;}); _temp2452;}); _temp2451;}); break;
case 210: _LL2450: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2456=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2456[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2457; _temp2457.tag= Cyc_AbstractDeclarator_tok; _temp2457.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2458=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2458->tms=({ struct Cyc_List_List* _temp2459=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2459->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2459->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->tms; _temp2459;});
_temp2458;}); _temp2457;}); _temp2456;}); break; case 211: _LL2455: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2461=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2461[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2462; _temp2462.tag= Cyc_AbstractDeclarator_tok;
_temp2462.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2463=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2463->tms=({
struct Cyc_List_List* _temp2464=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2464->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2465=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2465[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2466; _temp2466.tag= Cyc_Absyn_ConstArray_mod; _temp2466.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2466;});
_temp2465;})); _temp2464->tl= 0; _temp2464;}); _temp2463;}); _temp2462;});
_temp2461;}); break; case 212: _LL2460: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2468=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2468[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2469; _temp2469.tag= Cyc_AbstractDeclarator_tok; _temp2469.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2470=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2470->tms=({ struct Cyc_List_List* _temp2471=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2471->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2472=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2472[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2473; _temp2473.tag= Cyc_Absyn_ConstArray_mod;
_temp2473.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2473;}); _temp2472;})); _temp2471->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2471;}); _temp2470;}); _temp2469;}); _temp2468;}); break; case 213: _LL2467:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2475=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2475[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2476; _temp2476.tag=
Cyc_AbstractDeclarator_tok; _temp2476.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2477=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2477->tms=({ struct Cyc_List_List* _temp2478=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2478->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2479=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2479[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2480; _temp2480.tag= Cyc_Absyn_Function_mod;
_temp2480.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2481=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2481[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2482; _temp2482.tag= Cyc_Absyn_WithTypes;
_temp2482.f1= 0; _temp2482.f2= 0; _temp2482.f3= 0; _temp2482.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2482.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2482;}); _temp2481;})); _temp2480;}); _temp2479;}));
_temp2478->tl= 0; _temp2478;}); _temp2477;}); _temp2476;}); _temp2475;}); break;
case 214: _LL2474: { struct _tuple17 _temp2486; struct Cyc_List_List* _temp2487;
struct Cyc_Core_Opt* _temp2489; struct Cyc_Absyn_VarargInfo* _temp2491; int
_temp2493; struct Cyc_List_List* _temp2495; struct _tuple17* _temp2484= Cyc_yyget_ParamDeclListBool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2486=*
_temp2484; _LL2496: _temp2495= _temp2486.f1; goto _LL2494; _LL2494: _temp2493=
_temp2486.f2; goto _LL2492; _LL2492: _temp2491= _temp2486.f3; goto _LL2490;
_LL2490: _temp2489= _temp2486.f4; goto _LL2488; _LL2488: _temp2487= _temp2486.f5;
goto _LL2485; _LL2485: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2497=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2497[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2498; _temp2498.tag= Cyc_AbstractDeclarator_tok; _temp2498.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2499=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2499->tms=({ struct Cyc_List_List* _temp2500=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2500->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2501=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2501[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2502; _temp2502.tag= Cyc_Absyn_Function_mod;
_temp2502.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2503=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2503[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2504; _temp2504.tag= Cyc_Absyn_WithTypes;
_temp2504.f1= _temp2495; _temp2504.f2= _temp2493; _temp2504.f3= _temp2491;
_temp2504.f4= _temp2489; _temp2504.f5= _temp2487; _temp2504;}); _temp2503;}));
_temp2502;}); _temp2501;})); _temp2500->tl= 0; _temp2500;}); _temp2499;});
_temp2498;}); _temp2497;}); break;} case 215: _LL2483: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2506=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2506[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2507; _temp2507.tag= Cyc_AbstractDeclarator_tok;
_temp2507.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2508=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2508->tms=({
struct Cyc_List_List* _temp2509=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2509->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2510=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2510[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2511; _temp2511.tag=
Cyc_Absyn_Function_mod; _temp2511.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2512=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2512[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2513; _temp2513.tag= Cyc_Absyn_WithTypes;
_temp2513.f1= 0; _temp2513.f2= 0; _temp2513.f3= 0; _temp2513.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2513.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2513;}); _temp2512;})); _temp2511;}); _temp2510;}));
_temp2509->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->tms; _temp2509;});
_temp2508;}); _temp2507;}); _temp2506;}); break; case 216: _LL2505: { struct
_tuple17 _temp2517; struct Cyc_List_List* _temp2518; struct Cyc_Core_Opt*
_temp2520; struct Cyc_Absyn_VarargInfo* _temp2522; int _temp2524; struct Cyc_List_List*
_temp2526; struct _tuple17* _temp2515= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2517=*
_temp2515; _LL2527: _temp2526= _temp2517.f1; goto _LL2525; _LL2525: _temp2524=
_temp2517.f2; goto _LL2523; _LL2523: _temp2522= _temp2517.f3; goto _LL2521;
_LL2521: _temp2520= _temp2517.f4; goto _LL2519; _LL2519: _temp2518= _temp2517.f5;
goto _LL2516; _LL2516: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2528=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2528[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2529; _temp2529.tag= Cyc_AbstractDeclarator_tok; _temp2529.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2530=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2530->tms=({ struct Cyc_List_List* _temp2531=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2531->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2532=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2532[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2533; _temp2533.tag= Cyc_Absyn_Function_mod;
_temp2533.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2534=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2534[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2535; _temp2535.tag= Cyc_Absyn_WithTypes;
_temp2535.f1= _temp2526; _temp2535.f2= _temp2524; _temp2535.f3= _temp2522;
_temp2535.f4= _temp2520; _temp2535.f5= _temp2518; _temp2535;}); _temp2534;}));
_temp2533;}); _temp2532;})); _temp2531->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2531;}); _temp2530;}); _temp2529;}); _temp2528;}); break;} case 217:
_LL2514: { struct Cyc_List_List* _temp2537=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2538=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2538[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2539; _temp2539.tag= Cyc_AbstractDeclarator_tok; _temp2539.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2540=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2540->tms=({ struct Cyc_List_List* _temp2541=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2541->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2542=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2542[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2543; _temp2543.tag= Cyc_Absyn_TypeParams_mod;
_temp2543.f1= _temp2537; _temp2543.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2543.f3=
0; _temp2543;}); _temp2542;})); _temp2541->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2541;}); _temp2540;}); _temp2539;}); _temp2538;}); break;} case 218:
_LL2536:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2545=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2546=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2546[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2547; _temp2547.tag= Cyc_AbstractDeclarator_tok; _temp2547.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2548=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2548->tms=({ struct Cyc_List_List* _temp2549=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2549->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2550=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2550[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2551; _temp2551.tag= Cyc_Absyn_TypeParams_mod;
_temp2551.f1= _temp2545; _temp2551.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2551.f3=
0; _temp2551;}); _temp2550;})); _temp2549->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2549;}); _temp2548;}); _temp2547;}); _temp2546;}); break;} case 219:
_LL2544: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2553=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2553[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2554; _temp2554.tag=
Cyc_AbstractDeclarator_tok; _temp2554.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2555=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2555->tms=({ struct Cyc_List_List* _temp2556=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2556->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2557=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2557[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2558; _temp2558.tag= Cyc_Absyn_Attributes_mod;
_temp2558.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2558.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2558;}); _temp2557;}));
_temp2556->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->tms; _temp2556;});
_temp2555;}); _temp2554;}); _temp2553;}); break; case 220: _LL2552: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 221: _LL2559:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
222: _LL2560: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 223: _LL2561: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 224: _LL2562: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 225: _LL2563:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
226: _LL2564: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2576=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2576->name=({ struct
_tagged_arr* _temp2577=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2577[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2577;});
_temp2576->identity= 0; _temp2576->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2576;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2574=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2574[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2575; _temp2575.tag= Cyc_Absyn_VarType;
_temp2575.f1= tv; _temp2575;}); _temp2574;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2566=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2566[ 0]=({ struct Cyc_Stmt_tok_struct _temp2567; _temp2567.tag= Cyc_Stmt_tok;
_temp2567.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2568=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2568[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2569; _temp2569.tag= Cyc_Absyn_Region_s;
_temp2569.f1= tv; _temp2569.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2570=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2570->f1=(
void*) Cyc_Absyn_Loc_n; _temp2570->f2=({ struct _tagged_arr* _temp2571=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2571[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2571;});
_temp2570;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2572=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2572[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2573; _temp2573.tag=
Cyc_Absyn_RgnHandleType; _temp2573.f1=( void*) t; _temp2573;}); _temp2572;}), 0);
_temp2569.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2569;}); _temp2568;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2567;});
_temp2566;}); break;} case 227: _LL2565: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2589=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2589->name=({ struct
_tagged_arr* _temp2590=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2590[ 0]=( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp2592; _temp2592.tag= Cyc_Stdio_String_pa; _temp2592.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]);{ void* _temp2591[ 1u]={& _temp2592}; Cyc_Stdio_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2591, sizeof( void*), 1u));}});
_temp2590;}); _temp2589->identity= 0; _temp2589->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2589;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2587=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2587[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2588; _temp2588.tag= Cyc_Absyn_VarType;
_temp2588.f1= tv; _temp2588;}); _temp2587;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2579=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2579[ 0]=({ struct Cyc_Stmt_tok_struct _temp2580; _temp2580.tag= Cyc_Stmt_tok;
_temp2580.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2581=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2581[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2582; _temp2582.tag= Cyc_Absyn_Region_s;
_temp2582.f1= tv; _temp2582.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2583=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2583->f1=(
void*) Cyc_Absyn_Loc_n; _temp2583->f2=({ struct _tagged_arr* _temp2584=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2584[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2584;});
_temp2583;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2585=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2585[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2586; _temp2586.tag=
Cyc_Absyn_RgnHandleType; _temp2586.f1=( void*) t; _temp2586;}); _temp2585;}), 0);
_temp2582.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2582;}); _temp2581;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2580;});
_temp2579;}); break;} case 228: _LL2578: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2594=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2594[ 0]=({ struct Cyc_Stmt_tok_struct _temp2595; _temp2595.tag= Cyc_Stmt_tok;
_temp2595.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2596=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2596[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2597; _temp2597.tag= Cyc_Absyn_Cut_s;
_temp2597.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2597;}); _temp2596;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2595;});
_temp2594;}); break; case 229: _LL2593: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2599=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2599[ 0]=({ struct Cyc_Stmt_tok_struct _temp2600; _temp2600.tag= Cyc_Stmt_tok;
_temp2600.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2601=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2601[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2602; _temp2602.tag= Cyc_Absyn_Splice_s;
_temp2602.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2602;}); _temp2601;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2600;});
_temp2599;}); break; case 230: _LL2598: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2604=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2604[ 0]=({ struct Cyc_Stmt_tok_struct _temp2605; _temp2605.tag= Cyc_Stmt_tok;
_temp2605.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2606=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2606[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2607; _temp2607.tag= Cyc_Absyn_Label_s;
_temp2607.f1=({ struct _tagged_arr* _temp2608=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp2608[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2608;});
_temp2607.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2607;}); _temp2606;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2605;});
_temp2604;}); break; case 231: _LL2603: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2610=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2610[ 0]=({ struct Cyc_Stmt_tok_struct _temp2611; _temp2611.tag= Cyc_Stmt_tok;
_temp2611.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2611;});
_temp2610;}); break; case 232: _LL2609: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2613=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2613[ 0]=({ struct Cyc_Stmt_tok_struct _temp2614; _temp2614.tag= Cyc_Stmt_tok;
_temp2614.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2614;}); _temp2613;}); break; case 233: _LL2612: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2616=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2616[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2617; _temp2617.tag= Cyc_Stmt_tok; _temp2617.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2617;}); _temp2616;}); break; case 234: _LL2615: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 235:
_LL2618: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2620=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2620[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2621; _temp2621.tag= Cyc_Stmt_tok; _temp2621.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2621;}); _temp2620;}); break;
case 236: _LL2619: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2623=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2623[ 0]=({ struct Cyc_Stmt_tok_struct _temp2624; _temp2624.tag= Cyc_Stmt_tok;
_temp2624.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2624;});
_temp2623;}); break; case 237: _LL2622: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 238: _LL2625:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2627=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2627[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2628; _temp2628.tag= Cyc_Stmt_tok; _temp2628.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2628;}); _temp2627;}); break; case 239: _LL2626: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2630=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2630[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2631; _temp2631.tag= Cyc_Stmt_tok; _temp2631.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2632=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2632[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2633; _temp2633.tag= Cyc_Absyn_Fn_d;
_temp2633.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2633;}); _temp2632;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2631;}); _temp2630;}); break; case 240: _LL2629: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2635=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2635[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2636; _temp2636.tag= Cyc_Stmt_tok; _temp2636.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2637=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2637[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2638; _temp2638.tag= Cyc_Absyn_Fn_d;
_temp2638.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2638;}); _temp2637;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2636;}); _temp2635;}); break; case 241: _LL2634: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2640=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2640[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2641; _temp2641.tag= Cyc_Stmt_tok; _temp2641.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2641;});
_temp2640;}); break; case 242: _LL2639: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2643=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2643[ 0]=({ struct Cyc_Stmt_tok_struct _temp2644; _temp2644.tag= Cyc_Stmt_tok;
_temp2644.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2644;}); _temp2643;}); break; case 243: _LL2642: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2646=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2646[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2647; _temp2647.tag= Cyc_Stmt_tok; _temp2647.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2647;}); _temp2646;}); break; case 244: _LL2645: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2649=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2649[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2650; _temp2650.tag= Cyc_Stmt_tok; _temp2650.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2651=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2651[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2652; _temp2652.tag= Cyc_Absyn_SwitchC_s;
_temp2652.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2652.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2652;});
_temp2651;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2650;}); _temp2649;}); break; case 245:
_LL2648: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2654=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2654[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2655; _temp2655.tag= Cyc_Stmt_tok; _temp2655.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2655;});
_temp2654;}); break; case 246: _LL2653: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2657=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2657[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2658; _temp2658.tag= Cyc_SwitchClauseList_tok; _temp2658.f1= 0; _temp2658;});
_temp2657;}); break; case 247: _LL2656: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2660=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2660[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2661; _temp2661.tag= Cyc_SwitchClauseList_tok; _temp2661.f1=({ struct Cyc_List_List*
_temp2662=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2662->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2663=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2663->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp2663->pat_vars= 0; _temp2663->where_clause= 0; _temp2663->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2663->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2663;}); _temp2662->tl= 0; _temp2662;});
_temp2661;}); _temp2660;}); break; case 248: _LL2659: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2665=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2665[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2666; _temp2666.tag= Cyc_SwitchClauseList_tok;
_temp2666.f1=({ struct Cyc_List_List* _temp2667=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2667->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2668=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2668->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2668->pat_vars= 0; _temp2668->where_clause= 0;
_temp2668->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2668->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2668;});
_temp2667->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2667;}); _temp2666;});
_temp2665;}); break; case 249: _LL2664: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2670=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2670[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2671; _temp2671.tag= Cyc_SwitchClauseList_tok; _temp2671.f1=({ struct Cyc_List_List*
_temp2672=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2672->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2673=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2673->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2673->pat_vars=
0; _temp2673->where_clause= 0; _temp2673->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2673->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp2673;}); _temp2672->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2672;}); _temp2671;});
_temp2670;}); break; case 250: _LL2669: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2675=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2675[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2676; _temp2676.tag= Cyc_SwitchClauseList_tok; _temp2676.f1=({ struct Cyc_List_List*
_temp2677=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2677->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2678=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2678->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2678->pat_vars=
0; _temp2678->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2678->body=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2678->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2678;});
_temp2677->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2677;}); _temp2676;});
_temp2675;}); break; case 251: _LL2674: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2680=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2680[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2681; _temp2681.tag= Cyc_SwitchClauseList_tok; _temp2681.f1=({ struct Cyc_List_List*
_temp2682=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2682->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2683=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2683->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _temp2683->pat_vars=
0; _temp2683->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2683->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2683->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2683;});
_temp2682->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2682;}); _temp2681;});
_temp2680;}); break; case 252: _LL2679: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2685=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2685[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2686; _temp2686.tag= Cyc_SwitchCClauseList_tok; _temp2686.f1= 0; _temp2686;});
_temp2685;}); break; case 253: _LL2684: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2688=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2688[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2689; _temp2689.tag= Cyc_SwitchCClauseList_tok; _temp2689.f1=({ struct Cyc_List_List*
_temp2690=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2690->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2691=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2691->cnst_exp= 0;
_temp2691->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2691->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2691;});
_temp2690->tl= 0; _temp2690;}); _temp2689;}); _temp2688;}); break; case 254:
_LL2687: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2693=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2693[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2694; _temp2694.tag=
Cyc_SwitchCClauseList_tok; _temp2694.f1=({ struct Cyc_List_List* _temp2695=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2695->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2696=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2696->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2696->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2696->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2696;});
_temp2695->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2695;}); _temp2694;});
_temp2693;}); break; case 255: _LL2692: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2698=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2698[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2699; _temp2699.tag= Cyc_SwitchCClauseList_tok; _temp2699.f1=({ struct Cyc_List_List*
_temp2700=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2700->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2701=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2701->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _temp2701->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2701->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2701;});
_temp2700->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2700;}); _temp2699;});
_temp2698;}); break; case 256: _LL2697: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2703=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2703[ 0]=({ struct Cyc_Stmt_tok_struct _temp2704; _temp2704.tag= Cyc_Stmt_tok;
_temp2704.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2704;}); _temp2703;}); break; case 257: _LL2702: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2706=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2706[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2707; _temp2707.tag= Cyc_Stmt_tok; _temp2707.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2707;}); _temp2706;}); break; case 258: _LL2705: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2709=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2709[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2710; _temp2710.tag= Cyc_Stmt_tok; _temp2710.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2710;}); _temp2709;}); break; case 259: _LL2708: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2712=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2712[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2713; _temp2713.tag= Cyc_Stmt_tok; _temp2713.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2713;}); _temp2712;}); break; case 260: _LL2711: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2715=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2715[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2716; _temp2716.tag= Cyc_Stmt_tok; _temp2716.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2716;}); _temp2715;}); break; case 261: _LL2714: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2718=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2718[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2719; _temp2719.tag= Cyc_Stmt_tok; _temp2719.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2719;}); _temp2718;}); break; case 262: _LL2717: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2721=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2721[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2722; _temp2722.tag= Cyc_Stmt_tok; _temp2722.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2722;}); _temp2721;}); break; case 263: _LL2720: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2724=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2724[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2725; _temp2725.tag= Cyc_Stmt_tok; _temp2725.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2725;});
_temp2724;}); break; case 264: _LL2723: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2727=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2727[ 0]=({ struct Cyc_Stmt_tok_struct _temp2728; _temp2728.tag= Cyc_Stmt_tok;
_temp2728.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2728;});
_temp2727;}); break; case 265: _LL2726: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2730=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2730[ 0]=({ struct Cyc_Stmt_tok_struct _temp2731; _temp2731.tag= Cyc_Stmt_tok;
_temp2731.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2731;}); _temp2730;}); break; case 266: _LL2729: { struct Cyc_List_List*
_temp2733= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); struct Cyc_Absyn_Stmt* _temp2734= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2735=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2735[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2736; _temp2736.tag= Cyc_Stmt_tok; _temp2736.f1= Cyc_Parse_flatten_declarations(
_temp2733, _temp2734); _temp2736;}); _temp2735;}); break;} case 267: _LL2732: {
struct Cyc_List_List* _temp2738= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2739= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2740=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2740[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2741; _temp2741.tag= Cyc_Stmt_tok; _temp2741.f1= Cyc_Parse_flatten_declarations(
_temp2738, _temp2739); _temp2741;}); _temp2740;}); break;} case 268: _LL2737: {
struct Cyc_List_List* _temp2743= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2744= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2745=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2745[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2746; _temp2746.tag= Cyc_Stmt_tok; _temp2746.f1= Cyc_Parse_flatten_declarations(
_temp2743, _temp2744); _temp2746;}); _temp2745;}); break;} case 269: _LL2742: {
struct Cyc_List_List* _temp2748= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); struct Cyc_Absyn_Stmt*
_temp2749= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2750=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2750[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2751; _temp2751.tag= Cyc_Stmt_tok; _temp2751.f1= Cyc_Parse_flatten_declarations(
_temp2748, _temp2749); _temp2751;}); _temp2750;}); break;} case 270: _LL2747:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2753=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2753[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2754; _temp2754.tag= Cyc_Stmt_tok; _temp2754.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2755=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2755[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2755;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2754;}); _temp2753;}); break; case 271: _LL2752: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2757=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2757[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2758; _temp2758.tag= Cyc_Stmt_tok; _temp2758.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2758;}); _temp2757;}); break; case 272:
_LL2756: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2760=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2760[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2761; _temp2761.tag= Cyc_Stmt_tok; _temp2761.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2761;}); _temp2760;}); break; case 273: _LL2759: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2763=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2763[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2764; _temp2764.tag= Cyc_Stmt_tok; _temp2764.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2764;}); _temp2763;}); break; case 274:
_LL2762: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2766=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2766[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2767; _temp2767.tag= Cyc_Stmt_tok; _temp2767.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2767;}); _temp2766;}); break; case 275: _LL2765: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2769=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2769[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2770; _temp2770.tag= Cyc_Stmt_tok; _temp2770.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2770;}); _temp2769;}); break; case 276:
_LL2768: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2772=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2772[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2773; _temp2773.tag= Cyc_Stmt_tok; _temp2773.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); _temp2773;}); _temp2772;}); break; case 277:
_LL2771: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2775=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2775[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2776; _temp2776.tag= Cyc_Stmt_tok; _temp2776.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));
_temp2776;}); _temp2775;}); break; case 278: _LL2774: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2778=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2778[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2779; _temp2779.tag= Cyc_Pattern_tok; _temp2779.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2779;});
_temp2778;}); break; case 279: _LL2777: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 280:
_LL2780: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2782=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2782[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2783; _temp2783.tag= Cyc_Pattern_tok; _temp2783.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2784=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2784[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2785; _temp2785.tag= Cyc_Absyn_Int_p; _temp2785.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2785.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2785;}); _temp2784;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2783;});
_temp2782;}); break; case 281: _LL2781: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2787=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2787[ 0]=({ struct Cyc_Pattern_tok_struct _temp2788; _temp2788.tag= Cyc_Pattern_tok;
_temp2788.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2789=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2789[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2790; _temp2790.tag= Cyc_Absyn_Int_p;
_temp2790.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2790.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2790;});
_temp2789;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2788;}); _temp2787;}); break; case 282:
_LL2786: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2792=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2792[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2793; _temp2793.tag= Cyc_Pattern_tok; _temp2793.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2794=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2794[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2795; _temp2795.tag= Cyc_Absyn_Float_p; _temp2795.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2795;}); _temp2794;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2793;});
_temp2792;}); break; case 283: _LL2791: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2797=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2797[ 0]=({ struct Cyc_Pattern_tok_struct _temp2798; _temp2798.tag= Cyc_Pattern_tok;
_temp2798.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2799=( struct Cyc_Absyn_Char_p_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2799[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2800; _temp2800.tag= Cyc_Absyn_Char_p; _temp2800.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2800;});
_temp2799;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2798;}); _temp2797;}); break; case 284:
_LL2796: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2802=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2802[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2803; _temp2803.tag= Cyc_Pattern_tok; _temp2803.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2803;});
_temp2802;}); break; case 285: _LL2801: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2805=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2805[ 0]=({ struct Cyc_Pattern_tok_struct _temp2806; _temp2806.tag= Cyc_Pattern_tok;
_temp2806.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2807=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2807[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2808; _temp2808.tag=
Cyc_Absyn_UnknownId_p; _temp2808.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2808;}); _temp2807;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2806;}); _temp2805;}); break; case 286:
_LL2804: { struct Cyc_List_List* _temp2810=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2811=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2811[ 0]=({ struct Cyc_Pattern_tok_struct _temp2812; _temp2812.tag= Cyc_Pattern_tok;
_temp2812.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2813=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2813[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2814; _temp2814.tag= Cyc_Absyn_UnknownCall_p; _temp2814.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2814.f2=
_temp2810; _temp2814.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2814;});
_temp2813;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2812;}); _temp2811;}); break;} case
287: _LL2809: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2816=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2816[ 0]=({ struct Cyc_Pattern_tok_struct _temp2817; _temp2817.tag= Cyc_Pattern_tok;
_temp2817.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2818=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2818[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2819; _temp2819.tag= Cyc_Absyn_Tuple_p;
_temp2819.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2819;}); _temp2818;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2817;}); _temp2816;}); break; case 288: _LL2815: { struct Cyc_List_List*
_temp2821=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp2822=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2822[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2823; _temp2823.tag= Cyc_Pattern_tok; _temp2823.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2824=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2824[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2825; _temp2825.tag= Cyc_Absyn_UnknownFields_p;
_temp2825.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp2825.f2= _temp2821; _temp2825.f3= 0; _temp2825;});
_temp2824;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2823;}); _temp2822;}); break;} case
289: _LL2820: { struct Cyc_List_List* _temp2827=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2828=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2828[ 0]=({ struct Cyc_Pattern_tok_struct _temp2829; _temp2829.tag= Cyc_Pattern_tok;
_temp2829.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2830=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2830[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2831; _temp2831.tag= Cyc_Absyn_UnknownFields_p; _temp2831.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2831.f2=
_temp2827; _temp2831.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2831;});
_temp2830;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2829;}); _temp2828;}); break;} case
290: _LL2826: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2833=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2833[ 0]=({ struct Cyc_Pattern_tok_struct _temp2834; _temp2834.tag= Cyc_Pattern_tok;
_temp2834.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2835=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2835[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2836; _temp2836.tag= Cyc_Absyn_Pointer_p;
_temp2836.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2836;}); _temp2835;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2834;});
_temp2833;}); break; case 291: _LL2832: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2838=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2838[ 0]=({ struct Cyc_Pattern_tok_struct _temp2839; _temp2839.tag= Cyc_Pattern_tok;
_temp2839.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2840=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2840[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2841; _temp2841.tag=
Cyc_Absyn_Reference_p; _temp2841.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2842=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2842->f1=(
void*) Cyc_Absyn_Loc_n; _temp2842->f2=({ struct _tagged_arr* _temp2843=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2843[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2843;});
_temp2842;}),( void*) Cyc_Absyn_VoidType, 0); _temp2841;}); _temp2840;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2839;}); _temp2838;}); break; case 292: _LL2837: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2845=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2845[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2846; _temp2846.tag= Cyc_PatternList_tok;
_temp2846.f1= 0; _temp2846;}); _temp2845;}); break; case 293: _LL2844: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2848=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2848[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2849; _temp2849.tag= Cyc_PatternList_tok;
_temp2849.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2849;}); _temp2848;}); break; case 294: _LL2847: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2851=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2851[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2852; _temp2852.tag= Cyc_PatternList_tok;
_temp2852.f1=({ struct Cyc_List_List* _temp2853=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2853->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2853->tl= 0;
_temp2853;}); _temp2852;}); _temp2851;}); break; case 295: _LL2850: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2855=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2855[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2856; _temp2856.tag= Cyc_PatternList_tok;
_temp2856.f1=({ struct Cyc_List_List* _temp2857=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2857->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2857->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2857;}); _temp2856;}); _temp2855;}); break; case 296:
_LL2854: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2859=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2859[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2860; _temp2860.tag=
Cyc_FieldPattern_tok; _temp2860.f1=({ struct _tuple14* _temp2861=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2861->f1= 0; _temp2861->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2861;}); _temp2860;}); _temp2859;}); break; case 297:
_LL2858: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2863=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2863[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2864; _temp2864.tag=
Cyc_FieldPattern_tok; _temp2864.f1=({ struct _tuple14* _temp2865=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2865->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2865->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2865;}); _temp2864;}); _temp2863;}); break; case 298:
_LL2862: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2867=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2867[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2868; _temp2868.tag=
Cyc_FieldPatternList_tok; _temp2868.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2868;});
_temp2867;}); break; case 299: _LL2866: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2870=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2870[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2871; _temp2871.tag= Cyc_FieldPatternList_tok; _temp2871.f1=({ struct Cyc_List_List*
_temp2872=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2872->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2872->tl= 0;
_temp2872;}); _temp2871;}); _temp2870;}); break; case 300: _LL2869: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2874=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2874[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2875; _temp2875.tag= Cyc_FieldPatternList_tok;
_temp2875.f1=({ struct Cyc_List_List* _temp2876=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2876->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2876->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2876;}); _temp2875;}); _temp2874;}); break; case 301:
_LL2873: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 302: _LL2877: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2879=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2879[ 0]=({ struct Cyc_Exp_tok_struct _temp2880; _temp2880.tag= Cyc_Exp_tok;
_temp2880.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2880;}); _temp2879;}); break; case 303: _LL2878: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 304: _LL2881:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2883=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2883[ 0]=({ struct Cyc_Exp_tok_struct
_temp2884; _temp2884.tag= Cyc_Exp_tok; _temp2884.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2884;}); _temp2883;}); break; case 305: _LL2882: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2886=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2886[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2887; _temp2887.tag= Cyc_Primopopt_tok; _temp2887.f1=
0; _temp2887;}); _temp2886;}); break; case 306: _LL2885: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2889=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2889[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2890; _temp2890.tag= Cyc_Primopopt_tok; _temp2890.f1=({
struct Cyc_Core_Opt* _temp2891=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2891->v=( void*)(( void*) Cyc_Absyn_Times); _temp2891;});
_temp2890;}); _temp2889;}); break; case 307: _LL2888: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2893=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2893[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2894; _temp2894.tag= Cyc_Primopopt_tok; _temp2894.f1=({
struct Cyc_Core_Opt* _temp2895=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2895->v=( void*)(( void*) Cyc_Absyn_Div); _temp2895;});
_temp2894;}); _temp2893;}); break; case 308: _LL2892: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2897=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2897[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2898; _temp2898.tag= Cyc_Primopopt_tok; _temp2898.f1=({
struct Cyc_Core_Opt* _temp2899=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2899->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2899;});
_temp2898;}); _temp2897;}); break; case 309: _LL2896: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2901=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2901[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2902; _temp2902.tag= Cyc_Primopopt_tok; _temp2902.f1=({
struct Cyc_Core_Opt* _temp2903=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2903->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2903;});
_temp2902;}); _temp2901;}); break; case 310: _LL2900: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2905=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2905[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2906; _temp2906.tag= Cyc_Primopopt_tok; _temp2906.f1=({
struct Cyc_Core_Opt* _temp2907=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2907->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2907;});
_temp2906;}); _temp2905;}); break; case 311: _LL2904: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2909=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2909[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2910; _temp2910.tag= Cyc_Primopopt_tok; _temp2910.f1=({
struct Cyc_Core_Opt* _temp2911=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2911->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2911;});
_temp2910;}); _temp2909;}); break; case 312: _LL2908: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2913=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2913[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2914; _temp2914.tag= Cyc_Primopopt_tok; _temp2914.f1=({
struct Cyc_Core_Opt* _temp2915=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2915->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2915;});
_temp2914;}); _temp2913;}); break; case 313: _LL2912: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2917=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2917[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2918; _temp2918.tag= Cyc_Primopopt_tok; _temp2918.f1=({
struct Cyc_Core_Opt* _temp2919=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2919->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2919;});
_temp2918;}); _temp2917;}); break; case 314: _LL2916: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2921=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2921[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2922; _temp2922.tag= Cyc_Primopopt_tok; _temp2922.f1=({
struct Cyc_Core_Opt* _temp2923=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2923->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2923;});
_temp2922;}); _temp2921;}); break; case 315: _LL2920: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2925=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2925[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2926; _temp2926.tag= Cyc_Primopopt_tok; _temp2926.f1=({
struct Cyc_Core_Opt* _temp2927=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2927->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2927;});
_temp2926;}); _temp2925;}); break; case 316: _LL2924: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 317: _LL2928:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2930=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2930[ 0]=({ struct Cyc_Exp_tok_struct
_temp2931; _temp2931.tag= Cyc_Exp_tok; _temp2931.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2931;});
_temp2930;}); break; case 318: _LL2929: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2933=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2933[ 0]=({ struct Cyc_Exp_tok_struct _temp2934; _temp2934.tag= Cyc_Exp_tok;
_temp2934.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2934;}); _temp2933;}); break; case 319: _LL2932: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2936=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2936[ 0]=({ struct Cyc_Exp_tok_struct
_temp2937; _temp2937.tag= Cyc_Exp_tok; _temp2937.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp2937;}); _temp2936;}); break; case 320: _LL2935: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2939=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2939[ 0]=({ struct Cyc_Exp_tok_struct
_temp2940; _temp2940.tag= Cyc_Exp_tok; _temp2940.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp2940;}); _temp2939;}); break; case 321: _LL2938: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2942=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2942[ 0]=({ struct Cyc_Exp_tok_struct
_temp2943; _temp2943.tag= Cyc_Exp_tok; _temp2943.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2943;});
_temp2942;}); break; case 322: _LL2941: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2945=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2945[ 0]=({ struct Cyc_Exp_tok_struct _temp2946; _temp2946.tag= Cyc_Exp_tok;
_temp2946.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2946;}); _temp2945;}); break; case 323: _LL2944: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 324: _LL2947:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
325: _LL2948: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2950=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2950[ 0]=({ struct Cyc_Exp_tok_struct
_temp2951; _temp2951.tag= Cyc_Exp_tok; _temp2951.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2951;}); _temp2950;}); break; case 326: _LL2949: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 327: _LL2952:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2954=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2954[ 0]=({ struct Cyc_Exp_tok_struct
_temp2955; _temp2955.tag= Cyc_Exp_tok; _temp2955.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2955;}); _temp2954;}); break; case 328: _LL2953: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL2956:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2958=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2958[ 0]=({ struct Cyc_Exp_tok_struct
_temp2959; _temp2959.tag= Cyc_Exp_tok; _temp2959.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2959;});
_temp2958;}); break; case 330: _LL2957: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL2960:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2962=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2962[ 0]=({ struct Cyc_Exp_tok_struct
_temp2963; _temp2963.tag= Cyc_Exp_tok; _temp2963.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2963;});
_temp2962;}); break; case 332: _LL2961: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL2964:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2966=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2966[ 0]=({ struct Cyc_Exp_tok_struct
_temp2967; _temp2967.tag= Cyc_Exp_tok; _temp2967.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2967;});
_temp2966;}); break; case 334: _LL2965: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL2968:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2970=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2970[ 0]=({ struct Cyc_Exp_tok_struct
_temp2971; _temp2971.tag= Cyc_Exp_tok; _temp2971.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2971;}); _temp2970;}); break; case 336: _LL2969: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2973=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2973[ 0]=({ struct Cyc_Exp_tok_struct
_temp2974; _temp2974.tag= Cyc_Exp_tok; _temp2974.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2974;}); _temp2973;}); break; case 337: _LL2972: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 338: _LL2975:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2977=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2977[ 0]=({ struct Cyc_Exp_tok_struct
_temp2978; _temp2978.tag= Cyc_Exp_tok; _temp2978.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2978;}); _temp2977;}); break; case 339: _LL2976: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2980=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2980[ 0]=({ struct Cyc_Exp_tok_struct
_temp2981; _temp2981.tag= Cyc_Exp_tok; _temp2981.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2981;}); _temp2980;}); break; case 340: _LL2979: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2983=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2983[ 0]=({ struct Cyc_Exp_tok_struct
_temp2984; _temp2984.tag= Cyc_Exp_tok; _temp2984.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2984;}); _temp2983;}); break; case 341: _LL2982: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2986=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2986[ 0]=({ struct Cyc_Exp_tok_struct
_temp2987; _temp2987.tag= Cyc_Exp_tok; _temp2987.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2987;}); _temp2986;}); break; case 342: _LL2985: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 343: _LL2988:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2990=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2990[ 0]=({ struct Cyc_Exp_tok_struct
_temp2991; _temp2991.tag= Cyc_Exp_tok; _temp2991.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2991;}); _temp2990;}); break; case 344: _LL2989: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2993=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2993[ 0]=({ struct Cyc_Exp_tok_struct
_temp2994; _temp2994.tag= Cyc_Exp_tok; _temp2994.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2994;}); _temp2993;}); break; case 345: _LL2992: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 346: _LL2995:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2997=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2997[ 0]=({ struct Cyc_Exp_tok_struct
_temp2998; _temp2998.tag= Cyc_Exp_tok; _temp2998.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2998;});
_temp2997;}); break; case 347: _LL2996: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3000=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3000[ 0]=({ struct Cyc_Exp_tok_struct _temp3001; _temp3001.tag= Cyc_Exp_tok;
_temp3001.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3001;}); _temp3000;}); break; case 348: _LL2999: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 349: _LL3002:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3004=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3004[ 0]=({ struct Cyc_Exp_tok_struct
_temp3005; _temp3005.tag= Cyc_Exp_tok; _temp3005.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3005;});
_temp3004;}); break; case 350: _LL3003: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3007=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3007[ 0]=({ struct Cyc_Exp_tok_struct _temp3008; _temp3008.tag= Cyc_Exp_tok;
_temp3008.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3008;}); _temp3007;}); break; case 351: _LL3006: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3010=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3010[ 0]=({ struct Cyc_Exp_tok_struct
_temp3011; _temp3011.tag= Cyc_Exp_tok; _temp3011.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3011;});
_temp3010;}); break; case 352: _LL3009: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 353: _LL3012:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3014=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3014[ 0]=({ struct Cyc_Exp_tok_struct
_temp3015; _temp3015.tag= Cyc_Exp_tok; _temp3015.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3015;}); _temp3014;}); break; case 354: _LL3013: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL3016:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3018=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3018[ 0]=({ struct Cyc_Exp_tok_struct
_temp3019; _temp3019.tag= Cyc_Exp_tok; _temp3019.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3019;}); _temp3018;}); break; case 356: _LL3017: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3021=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3021[ 0]=({ struct Cyc_Exp_tok_struct
_temp3022; _temp3022.tag= Cyc_Exp_tok; _temp3022.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3022;}); _temp3021;}); break; case 357: _LL3020: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3024=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3024[ 0]=({ struct Cyc_Exp_tok_struct
_temp3025; _temp3025.tag= Cyc_Exp_tok; _temp3025.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3025;}); _temp3024;}); break; case 358: _LL3023: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3027=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3027[ 0]=({ struct Cyc_Exp_tok_struct
_temp3028; _temp3028.tag= Cyc_Exp_tok; _temp3028.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3028;}); _temp3027;}); break; case 359: _LL3026: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 360: _LL3029:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3031=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3031[ 0]=({ struct Cyc_Exp_tok_struct
_temp3032; _temp3032.tag= Cyc_Exp_tok; _temp3032.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3032;}); _temp3031;}); break; case 361: _LL3030: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3034=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3034[ 0]=({ struct Cyc_Exp_tok_struct
_temp3035; _temp3035.tag= Cyc_Exp_tok; _temp3035.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3035;});
_temp3034;}); break; case 362: _LL3033: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3037=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3037[ 0]=({ struct Cyc_Exp_tok_struct _temp3038; _temp3038.tag= Cyc_Exp_tok;
_temp3038.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3038;}); _temp3037;}); break; case 363: _LL3036: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3040=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3040[ 0]=({ struct Cyc_Exp_tok_struct
_temp3041; _temp3041.tag= Cyc_Exp_tok; _temp3041.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])).f3,({ struct _tagged_arr* _temp3042=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3042[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3042;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3041;}); _temp3040;}); break; case 364:
_LL3039: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3044=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3044[ 0]=({ struct Cyc_Exp_tok_struct
_temp3045; _temp3045.tag= Cyc_Exp_tok; _temp3045.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3046=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3046[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3047; _temp3047.tag= Cyc_Absyn_Malloc_e;
_temp3047.f1= 0; _temp3047.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3047;}); _temp3046;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3045;});
_temp3044;}); break; case 365: _LL3043: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3049=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3049[ 0]=({ struct Cyc_Exp_tok_struct _temp3050; _temp3050.tag= Cyc_Exp_tok;
_temp3050.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3051=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3051[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3052; _temp3052.tag= Cyc_Absyn_Malloc_e;
_temp3052.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]); _temp3052.f2=( void*)
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3052;}); _temp3051;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3050;});
_temp3049;}); break; case 366: _LL3048: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3054=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3054[ 0]=({ struct Cyc_Primop_tok_struct _temp3055; _temp3055.tag= Cyc_Primop_tok;
_temp3055.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3055;}); _temp3054;});
break; case 367: _LL3053: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3057=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3057[ 0]=({ struct Cyc_Primop_tok_struct _temp3058; _temp3058.tag= Cyc_Primop_tok;
_temp3058.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3058;}); _temp3057;});
break; case 368: _LL3056: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3060=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3060[ 0]=({ struct Cyc_Primop_tok_struct _temp3061; _temp3061.tag= Cyc_Primop_tok;
_temp3061.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3061;}); _temp3060;});
break; case 369: _LL3059: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 370: _LL3062: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3064=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3064[ 0]=({ struct Cyc_Exp_tok_struct _temp3065; _temp3065.tag= Cyc_Exp_tok;
_temp3065.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3065;}); _temp3064;}); break; case 371: _LL3063: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3067=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3067[ 0]=({ struct Cyc_Exp_tok_struct
_temp3068; _temp3068.tag= Cyc_Exp_tok; _temp3068.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3068;}); _temp3067;}); break; case 372:
_LL3066: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3070=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3070[ 0]=({ struct Cyc_Exp_tok_struct
_temp3071; _temp3071.tag= Cyc_Exp_tok; _temp3071.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3071;});
_temp3070;}); break; case 373: _LL3069: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3073=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3073[ 0]=({ struct Cyc_Exp_tok_struct _temp3074; _temp3074.tag= Cyc_Exp_tok;
_temp3074.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3075=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3075[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3075;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3074;}); _temp3073;}); break; case 374: _LL3072: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3077=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3077[ 0]=({ struct Cyc_Exp_tok_struct
_temp3078; _temp3078.tag= Cyc_Exp_tok; _temp3078.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3078;});
_temp3077;}); break;} case 375: _LL3076: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3080=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3080[ 0]=({ struct Cyc_Exp_tok_struct _temp3081; _temp3081.tag= Cyc_Exp_tok;
_temp3081.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3082=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3082[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3082;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3081;}); _temp3080;}); break; case 376: _LL3079: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3084=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3084[ 0]=({ struct Cyc_Exp_tok_struct
_temp3085; _temp3085.tag= Cyc_Exp_tok; _temp3085.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3085;});
_temp3084;}); break;} case 377: _LL3083: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3087=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3087[ 0]=({ struct Cyc_Exp_tok_struct _temp3088; _temp3088.tag= Cyc_Exp_tok;
_temp3088.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3088;}); _temp3087;}); break; case 378: _LL3086: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3090=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3090[ 0]=({ struct Cyc_Exp_tok_struct
_temp3091; _temp3091.tag= Cyc_Exp_tok; _temp3091.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3091;}); _temp3090;}); break; case 379: _LL3089: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3093=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3093[ 0]=({ struct Cyc_Exp_tok_struct
_temp3094; _temp3094.tag= Cyc_Exp_tok; _temp3094.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3095=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3095[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3096; _temp3096.tag= Cyc_Absyn_CompoundLit_e;
_temp3096.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp3096.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3096;});
_temp3095;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3094;}); _temp3093;}); break; case 380:
_LL3092: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3098=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3098[ 0]=({ struct Cyc_Exp_tok_struct
_temp3099; _temp3099.tag= Cyc_Exp_tok; _temp3099.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3100=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3100[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3101; _temp3101.tag= Cyc_Absyn_CompoundLit_e;
_temp3101.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp3101.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp3101;});
_temp3100;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3099;}); _temp3098;}); break; case 381:
_LL3097: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3103=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3103[ 0]=({ struct Cyc_Exp_tok_struct
_temp3104; _temp3104.tag= Cyc_Exp_tok; _temp3104.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3105=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3105[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3106; _temp3106.tag= Cyc_Absyn_Fill_e; _temp3106.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3106;});
_temp3105;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3104;}); _temp3103;}); break; case 382:
_LL3102: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3108=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3108[ 0]=({ struct Cyc_Exp_tok_struct
_temp3109; _temp3109.tag= Cyc_Exp_tok; _temp3109.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3110=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3110[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3111; _temp3111.tag= Cyc_Absyn_Codegen_e;
_temp3111.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp3111;}); _temp3110;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3109;}); _temp3108;}); break; case 383: _LL3107: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3113=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3113[ 0]=({ struct Cyc_Exp_tok_struct
_temp3114; _temp3114.tag= Cyc_Exp_tok; _temp3114.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3115=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3115[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3116; _temp3116.tag= Cyc_Absyn_UnknownId_e;
_temp3116.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3116;}); _temp3115;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3114;});
_temp3113;}); break; case 384: _LL3112: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 385: _LL3117:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3119=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3119[ 0]=({ struct Cyc_Exp_tok_struct
_temp3120; _temp3120.tag= Cyc_Exp_tok; _temp3120.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3120;});
_temp3119;}); break; case 386: _LL3118: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 387:
_LL3121: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3123=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3123[ 0]=({ struct Cyc_Exp_tok_struct
_temp3124; _temp3124.tag= Cyc_Exp_tok; _temp3124.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3125=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3125[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3126; _temp3126.tag=
Cyc_Absyn_UnknownId_e; _temp3126.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3126;});
_temp3125;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3124;});
_temp3123;}); break; case 388: _LL3122: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3128=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3128[ 0]=({ struct Cyc_Exp_tok_struct _temp3129; _temp3129.tag= Cyc_Exp_tok;
_temp3129.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3130=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3130[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3131; _temp3131.tag=
Cyc_Absyn_UnknownId_e; _temp3131.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp3131;});
_temp3130;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3129;}); _temp3128;}); break; case 389: _LL3127: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3133=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3133[ 0]=({ struct Cyc_Exp_tok_struct
_temp3134; _temp3134.tag= Cyc_Exp_tok; _temp3134.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3134;}); _temp3133;}); break; case 390: _LL3132: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3136=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3136[ 0]=({ struct Cyc_Exp_tok_struct
_temp3137; _temp3137.tag= Cyc_Exp_tok; _temp3137.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3138=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3138[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3139; _temp3139.tag= Cyc_Absyn_Struct_e;
_temp3139.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp3139.f2= 0; _temp3139.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3139.f4= 0;
_temp3139;}); _temp3138;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3137;});
_temp3136;}); break; case 391: _LL3135: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3141=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3141[ 0]=({ struct Cyc_Exp_tok_struct _temp3142; _temp3142.tag= Cyc_Exp_tok;
_temp3142.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3142;}); _temp3141;}); break; case 392: _LL3140: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3144=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3144[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3145; _temp3145.tag= Cyc_ExpList_tok; _temp3145.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3145;});
_temp3144;}); break; case 393: _LL3143: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3147=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3147[ 0]=({ struct Cyc_ExpList_tok_struct _temp3148; _temp3148.tag= Cyc_ExpList_tok;
_temp3148.f1=({ struct Cyc_List_List* _temp3149=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3149->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3149->tl= 0;
_temp3149;}); _temp3148;}); _temp3147;}); break; case 394: _LL3146: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3151=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3151[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3152; _temp3152.tag= Cyc_ExpList_tok; _temp3152.f1=({ struct Cyc_List_List*
_temp3153=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3153->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3153->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp3153;});
_temp3152;}); _temp3151;}); break; case 395: _LL3150: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3155=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3155[ 0]=({ struct Cyc_Exp_tok_struct
_temp3156; _temp3156.tag= Cyc_Exp_tok; _temp3156.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3156;});
_temp3155;}); break; case 396: _LL3154: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3158=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3158[ 0]=({ struct Cyc_Exp_tok_struct _temp3159; _temp3159.tag= Cyc_Exp_tok;
_temp3159.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3159;});
_temp3158;}); break; case 397: _LL3157: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3161=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3161[ 0]=({ struct Cyc_Exp_tok_struct _temp3162; _temp3162.tag= Cyc_Exp_tok;
_temp3162.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3162;});
_temp3161;}); break; case 398: _LL3160: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3164=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3164[ 0]=({ struct Cyc_Exp_tok_struct _temp3165; _temp3165.tag= Cyc_Exp_tok;
_temp3165.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3165;});
_temp3164;}); break; case 399: _LL3163: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3167=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3167[ 0]=({ struct Cyc_QualId_tok_struct _temp3168; _temp3168.tag= Cyc_QualId_tok;
_temp3168.f1=({ struct _tuple1* _temp3169=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3169->f1= Cyc_Absyn_rel_ns_null; _temp3169->f2=({ struct
_tagged_arr* _temp3170=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3170[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3170;}); _temp3169;});
_temp3168;}); _temp3167;}); break; case 400: _LL3166: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3171:(
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
yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull( 401u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 112u, yyn - 119)] + yyss[
_check_known_subscript_notnull( 10000u, yyssp_offset)]; if(( yystate >= 0?
yystate <= 4376: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4377u, yystate)]
== yyss[ _check_known_subscript_notnull( 10000u, yyssp_offset)]: 0){ yystate=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4377u, yystate)];} else{
yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 112u, yyn - 119)];}
goto yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 788u, yystate)]; if( yyn > - 32768? yyn < 4376:
0){ int sze= 0; struct _tagged_arr msg; int x; int count; count= 0; for( x= yyn
< 0? - yyn: 0; x < 231u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4377u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 231u, x)]) + 15, count ++);}} msg=({
unsigned int _temp3173=( unsigned int)( sze + 15); unsigned char* _temp3174=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3173); struct
_tagged_arr _temp3176= _tag_arr( _temp3174, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3175= _temp3173; unsigned int i;
for( i= 0; i < _temp3175; i ++){ _temp3174[ i]='\000';}}; _temp3176;}); Cyc_String_strcpy(
msg, _tag_arr("parse error", sizeof( unsigned char), 12u)); if( count < 5){
count= 0; for( x= yyn < 0? - yyn: 0; x < 231u / sizeof( unsigned char*); x ++){
if( Cyc_yycheck[ _check_known_subscript_notnull( 4377u, x + yyn)] == x){ Cyc_String_strcat(
msg, count == 0? _tag_arr(", expecting `", sizeof( unsigned char), 14u):
_tag_arr(" or `", sizeof( unsigned char), 6u)); Cyc_String_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 231u, x)]); Cyc_String_strcat( msg, _tag_arr("'",
sizeof( unsigned char), 2u)); count ++;}}} Cyc_yyerror(( struct _tagged_arr) msg);}
else{ Cyc_yyerror( _tag_arr("parse error", sizeof( unsigned char), 12u));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus == 3){ if( Cyc_yychar == 0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset == 0){ return 1;} yyvsp_offset --; yystate=( int) yyss[
_check_known_subscript_notnull( 10000u, -- yyssp_offset)]; yylsp_offset --;
yyerrhandle: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull( 788u,
yystate)]; if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1:
yyn > 4376)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4377u, yyn)] != 1){
goto yyerrdefault;} yyn=( int) Cyc_yytable[ _check_known_subscript_notnull( 4377u,
yyn)]; if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto
yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 787){ return 0;}
yyvs[ _check_known_subscript_notnull( 10000u, ++ yyvsp_offset)]= Cyc_yylval;
yyls[ _check_known_subscript_notnull( 10000u, ++ yylsp_offset)]= Cyc_yylloc;
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3186= v; struct
_tuple16* _temp3204; struct _tuple16 _temp3206; int _temp3207; unsigned char
_temp3209; short _temp3211; struct _tagged_arr _temp3213; struct Cyc_Core_Opt*
_temp3215; struct Cyc_Core_Opt* _temp3217; struct Cyc_Core_Opt _temp3219; struct
_tagged_arr* _temp3220; struct _tuple1* _temp3222; struct _tuple1 _temp3224;
struct _tagged_arr* _temp3225; void* _temp3227; _LL3188: if(*(( void**)
_temp3186) == Cyc_Int_tok){ _LL3205: _temp3204=(( struct Cyc_Int_tok_struct*)
_temp3186)->f1; _temp3206=* _temp3204; _LL3208: _temp3207= _temp3206.f2; goto
_LL3189;} else{ goto _LL3190;} _LL3190: if(*(( void**) _temp3186) == Cyc_Char_tok){
_LL3210: _temp3209=(( struct Cyc_Char_tok_struct*) _temp3186)->f1; goto _LL3191;}
else{ goto _LL3192;} _LL3192: if(*(( void**) _temp3186) == Cyc_Short_tok){
_LL3212: _temp3211=(( struct Cyc_Short_tok_struct*) _temp3186)->f1; goto _LL3193;}
else{ goto _LL3194;} _LL3194: if(*(( void**) _temp3186) == Cyc_String_tok){
_LL3214: _temp3213=(( struct Cyc_String_tok_struct*) _temp3186)->f1; goto
_LL3195;} else{ goto _LL3196;} _LL3196: if(*(( void**) _temp3186) == Cyc_Stringopt_tok){
_LL3216: _temp3215=(( struct Cyc_Stringopt_tok_struct*) _temp3186)->f1; if(
_temp3215 == 0){ goto _LL3197;} else{ goto _LL3198;}} else{ goto _LL3198;}
_LL3198: if(*(( void**) _temp3186) == Cyc_Stringopt_tok){ _LL3218: _temp3217=((
struct Cyc_Stringopt_tok_struct*) _temp3186)->f1; if( _temp3217 == 0){ goto
_LL3200;} else{ _temp3219=* _temp3217; _LL3221: _temp3220=( struct _tagged_arr*)
_temp3219.v; goto _LL3199;}} else{ goto _LL3200;} _LL3200: if(*(( void**)
_temp3186) == Cyc_QualId_tok){ _LL3223: _temp3222=(( struct Cyc_QualId_tok_struct*)
_temp3186)->f1; _temp3224=* _temp3222; _LL3228: _temp3227= _temp3224.f1; goto
_LL3226; _LL3226: _temp3225= _temp3224.f2; goto _LL3201;} else{ goto _LL3202;}
_LL3202: goto _LL3203; _LL3189:({ struct Cyc_Stdio_Int_pa_struct _temp3230;
_temp3230.tag= Cyc_Stdio_Int_pa; _temp3230.f1=( int)(( unsigned int) _temp3207);{
void* _temp3229[ 1u]={& _temp3230}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp3229, sizeof( void*),
1u));}}); goto _LL3187; _LL3191:({ struct Cyc_Stdio_Int_pa_struct _temp3232;
_temp3232.tag= Cyc_Stdio_Int_pa; _temp3232.f1=( int)(( unsigned int)(( int)
_temp3209));{ void* _temp3231[ 1u]={& _temp3232}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%c", sizeof( unsigned char), 3u), _tag_arr( _temp3231, sizeof( void*),
1u));}}); goto _LL3187; _LL3193:({ struct Cyc_Stdio_Int_pa_struct _temp3234;
_temp3234.tag= Cyc_Stdio_Int_pa; _temp3234.f1=( int)(( unsigned int)(( int)
_temp3211));{ void* _temp3233[ 1u]={& _temp3234}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%ds", sizeof( unsigned char), 4u), _tag_arr( _temp3233, sizeof( void*),
1u));}}); goto _LL3187; _LL3195:({ struct Cyc_Stdio_String_pa_struct _temp3236;
_temp3236.tag= Cyc_Stdio_String_pa; _temp3236.f1=( struct _tagged_arr) _temp3213;{
void* _temp3235[ 1u]={& _temp3236}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("\"%s\"", sizeof( unsigned char), 5u), _tag_arr( _temp3235, sizeof(
void*), 1u));}}); goto _LL3187; _LL3197:({ void* _temp3237[ 0u]={}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("null", sizeof( unsigned char), 5u), _tag_arr(
_temp3237, sizeof( void*), 0u));}); goto _LL3187; _LL3199:({ struct Cyc_Stdio_String_pa_struct
_temp3239; _temp3239.tag= Cyc_Stdio_String_pa; _temp3239.f1=( struct _tagged_arr)*
_temp3220;{ void* _temp3238[ 1u]={& _temp3239}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("\"%s\"", sizeof( unsigned char), 5u), _tag_arr( _temp3238, sizeof(
void*), 1u));}}); goto _LL3187; _LL3201: { struct Cyc_List_List* _temp3240= 0;{
void* _temp3241= _temp3227; struct Cyc_List_List* _temp3249; struct Cyc_List_List*
_temp3251; _LL3243: if(( unsigned int) _temp3241 > 1u?*(( int*) _temp3241) ==
Cyc_Absyn_Rel_n: 0){ _LL3250: _temp3249=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3241)->f1; goto _LL3244;} else{ goto _LL3245;} _LL3245: if(( unsigned int)
_temp3241 > 1u?*(( int*) _temp3241) == Cyc_Absyn_Abs_n: 0){ _LL3252: _temp3251=((
struct Cyc_Absyn_Abs_n_struct*) _temp3241)->f1; goto _LL3246;} else{ goto
_LL3247;} _LL3247: if( _temp3241 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3248;}
else{ goto _LL3242;} _LL3244: _temp3240= _temp3249; goto _LL3242; _LL3246:
_temp3240= _temp3251; goto _LL3242; _LL3248: goto _LL3242; _LL3242:;} for( 0;
_temp3240 != 0; _temp3240=(( struct Cyc_List_List*) _check_null( _temp3240))->tl){({
struct Cyc_Stdio_String_pa_struct _temp3254; _temp3254.tag= Cyc_Stdio_String_pa;
_temp3254.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp3240))->hd);{ void* _temp3253[ 1u]={& _temp3254}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("%s::", sizeof( unsigned char), 5u), _tag_arr(
_temp3253, sizeof( void*), 1u));}});}({ struct Cyc_Stdio_String_pa_struct
_temp3256; _temp3256.tag= Cyc_Stdio_String_pa; _temp3256.f1=( struct _tagged_arr)*
_temp3225;{ void* _temp3255[ 1u]={& _temp3256}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s::", sizeof( unsigned char), 5u), _tag_arr( _temp3255, sizeof( void*),
1u));}}); goto _LL3187;} _LL3203:({ void* _temp3257[ 0u]={}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("?", sizeof( unsigned char), 2u), _tag_arr( _temp3257,
sizeof( void*), 0u));}); goto _LL3187; _LL3187:;} struct Cyc_List_List* Cyc_Parse_parse_file(
struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({
struct Cyc_Core_Opt* _temp3258=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3258->v=( void*) Cyc_Lexing_from_file( f); _temp3258;});
Cyc_yyparse(); return Cyc_Parse_parse_result;}
