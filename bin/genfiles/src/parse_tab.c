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
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ enum  Cyc_Parse_Storage_class
_temp200=( enum  Cyc_Parse_Storage_class)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v;
_LL202: if( _temp200 == Cyc_Parse_Extern_sc){ goto _LL203;} else{ goto _LL204;}
_LL204: if( _temp200 == Cyc_Parse_ExternC_sc){ goto _LL205;} else{ goto _LL206;}
_LL206: if( _temp200 == Cyc_Parse_Static_sc){ goto _LL207;} else{ goto _LL208;}
_LL208: goto _LL209; _LL203: sc=( void*) Cyc_Absyn_Extern; goto _LL201; _LL205:
sc=( void*) Cyc_Absyn_ExternC; goto _LL201; _LL207: sc=( void*) Cyc_Absyn_Static;
goto _LL201; _LL209: Cyc_Parse_err( _tag_arr("bad storage class on function",
sizeof( unsigned char), 30u), loc); goto _LL201; _LL201:;}}{ struct Cyc_Core_Opt*
_temp212; void* _temp214; struct _tuple5 _temp210= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL215: _temp214= _temp210.f1; goto _LL213; _LL213: _temp212=
_temp210.f2; goto _LL211; _LL211: { struct Cyc_List_List* _temp218; struct Cyc_List_List*
_temp220; void* _temp222; struct Cyc_Absyn_Tqual _temp224; struct _tuple6
_temp216= Cyc_Parse_apply_tms( tq, _temp214, atts, d->tms); _LL225: _temp224=
_temp216.f1; goto _LL223; _LL223: _temp222= _temp216.f2; goto _LL221; _LL221:
_temp220= _temp216.f3; goto _LL219; _LL219: _temp218= _temp216.f4; goto _LL217;
_LL217: if( _temp212 != 0){ Cyc_Parse_warn( _tag_arr("nested type declaration within function prototype",
sizeof( unsigned char), 50u), loc);} if( _temp220 != 0){ Cyc_Parse_warn(
_tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u), loc);}{ void*
_temp226= _temp222; struct Cyc_Absyn_FnInfo _temp232; struct Cyc_List_List*
_temp234; struct Cyc_List_List* _temp236; struct Cyc_Absyn_VarargInfo* _temp238;
int _temp240; struct Cyc_List_List* _temp242; void* _temp244; struct Cyc_Core_Opt*
_temp246; struct Cyc_List_List* _temp248; _LL228: if(( unsigned int) _temp226 >
4u?*(( int*) _temp226) == Cyc_Absyn_FnType: 0){ _LL233: _temp232=(( struct Cyc_Absyn_FnType_struct*)
_temp226)->f1; _LL249: _temp248= _temp232.tvars; goto _LL247; _LL247: _temp246=
_temp232.effect; goto _LL245; _LL245: _temp244=( void*) _temp232.ret_typ; goto
_LL243; _LL243: _temp242= _temp232.args; goto _LL241; _LL241: _temp240= _temp232.c_varargs;
goto _LL239; _LL239: _temp238= _temp232.cyc_varargs; goto _LL237; _LL237:
_temp236= _temp232.rgn_po; goto _LL235; _LL235: _temp234= _temp232.attributes;
goto _LL229;} else{ goto _LL230;} _LL230: goto _LL231; _LL229: { struct Cyc_List_List*
_temp250=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp242); return({ struct Cyc_Absyn_Fndecl*
_temp251=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp251->sc=( void*) sc; _temp251->name= d->id; _temp251->tvs= _temp248;
_temp251->is_inline= is_inline; _temp251->effect= _temp246; _temp251->ret_type=(
void*) _temp244; _temp251->args= _temp250; _temp251->c_varargs= _temp240;
_temp251->cyc_varargs= _temp238; _temp251->rgn_po= _temp236; _temp251->body=
body; _temp251->cached_typ= 0; _temp251->param_vardecls= 0; _temp251->attributes=
Cyc_List_append( _temp234, _temp218); _temp251;});} _LL231: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("declarator is not a function prototype", sizeof( unsigned char), 39u),
loc); _LL227:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct
Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(
_tag_arr("missing argument variable in function prototype", sizeof(
unsigned char), 48u), loc); return({ struct _tuple4* _temp252=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp252->f1= _init_tag_arr(( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"?", sizeof( unsigned char),
2u); _temp252->f2=(* t).f2; _temp252->f3=(* t).f3; _temp252;});} else{ return({
struct _tuple4* _temp253=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp253->f1=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp253->f2=(* t).f2; _temp253->f3=(* t).f3; _temp253;});}} static
unsigned char _temp254[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_arr Cyc_Parse_msg1={ _temp254, _temp254, _temp254 + 52u};
static unsigned char _temp255[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg2={ _temp255, _temp255, _temp255 + 63u};
static unsigned char _temp256[ 50u]="type specifier includes more than one declaration";
static struct _tagged_arr Cyc_Parse_msg3={ _temp256, _temp256, _temp256 + 50u};
static unsigned char _temp257[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_arr Cyc_Parse_msg4={ _temp257, _temp257, _temp257 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp258=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp272; void* _temp274; struct Cyc_Position_Segment*
_temp276; struct Cyc_Position_Segment* _temp278; struct Cyc_Position_Segment*
_temp280; struct Cyc_Position_Segment* _temp282; struct Cyc_Absyn_Decl* _temp284;
_LL260: if(*(( int*) _temp258) == Cyc_Parse_Type_spec){ _LL275: _temp274=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp258)->f1; goto _LL273; _LL273: _temp272=((
struct Cyc_Parse_Type_spec_struct*) _temp258)->f2; goto _LL261;} else{ goto
_LL262;} _LL262: if(*(( int*) _temp258) == Cyc_Parse_Signed_spec){ _LL277:
_temp276=(( struct Cyc_Parse_Signed_spec_struct*) _temp258)->f1; goto _LL263;}
else{ goto _LL264;} _LL264: if(*(( int*) _temp258) == Cyc_Parse_Unsigned_spec){
_LL279: _temp278=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp258)->f1; goto
_LL265;} else{ goto _LL266;} _LL266: if(*(( int*) _temp258) == Cyc_Parse_Short_spec){
_LL281: _temp280=(( struct Cyc_Parse_Short_spec_struct*) _temp258)->f1; goto
_LL267;} else{ goto _LL268;} _LL268: if(*(( int*) _temp258) == Cyc_Parse_Long_spec){
_LL283: _temp282=(( struct Cyc_Parse_Long_spec_struct*) _temp258)->f1; goto
_LL269;} else{ goto _LL270;} _LL270: if(*(( int*) _temp258) == Cyc_Parse_Decl_spec){
_LL285: _temp284=(( struct Cyc_Parse_Decl_spec_struct*) _temp258)->f1; goto
_LL271;} else{ goto _LL259;} _LL261: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp272);} last_loc= _temp272; seen_type= 1; t= _temp274; goto _LL259; _LL263:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp276);} if( seen_type){ Cyc_Parse_err(
_tag_arr("signed qualifier must come before type", sizeof( unsigned char), 39u),
_temp276);} last_loc= _temp276; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed;
goto _LL259; _LL265: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp278);}
if( seen_type){ Cyc_Parse_err( _tag_arr("signed qualifier must come before type",
sizeof( unsigned char), 39u), _temp278);} last_loc= _temp278; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Unsigned; goto _LL259; _LL267: if( seen_size){ Cyc_Parse_err(
_tag_arr("integral size may appear only once within a type specifier", sizeof(
unsigned char), 59u), _temp280);} if( seen_type){ Cyc_Parse_err( _tag_arr("short modifier must come before base type",
sizeof( unsigned char), 42u), _temp280);} last_loc= _temp280; seen_size= 1; sz=(
void*) Cyc_Absyn_B2; goto _LL259; _LL269: if( seen_type){ Cyc_Parse_err(
_tag_arr("long modifier must come before base type", sizeof( unsigned char), 41u),
_temp282);} if( seen_size){ void* _temp286= sz; _LL288: if( _temp286 ==( void*)
Cyc_Absyn_B4){ goto _LL289;} else{ goto _LL290;} _LL290: goto _LL291; _LL289: sz=(
void*) Cyc_Absyn_B8; goto _LL287; _LL291: Cyc_Parse_err( _tag_arr("extra long in type specifier",
sizeof( unsigned char), 29u), _temp282); goto _LL287; _LL287:;} last_loc=
_temp282; seen_size= 1; goto _LL259; _LL271: last_loc= _temp284->loc; if(
declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp292=( void*) _temp284->r;
struct Cyc_Absyn_Structdecl* _temp304; struct Cyc_Absyn_Tuniondecl* _temp306;
struct Cyc_Absyn_Uniondecl* _temp308; struct Cyc_Absyn_Enumdecl* _temp310;
_LL294: if(*(( int*) _temp292) == Cyc_Absyn_Struct_d){ _LL305: _temp304=((
struct Cyc_Absyn_Struct_d_struct*) _temp292)->f1; goto _LL295;} else{ goto
_LL296;} _LL296: if(*(( int*) _temp292) == Cyc_Absyn_Tunion_d){ _LL307: _temp306=((
struct Cyc_Absyn_Tunion_d_struct*) _temp292)->f1; goto _LL297;} else{ goto
_LL298;} _LL298: if(*(( int*) _temp292) == Cyc_Absyn_Union_d){ _LL309: _temp308=((
struct Cyc_Absyn_Union_d_struct*) _temp292)->f1; goto _LL299;} else{ goto _LL300;}
_LL300: if(*(( int*) _temp292) == Cyc_Absyn_Enum_d){ _LL311: _temp310=(( struct
Cyc_Absyn_Enum_d_struct*) _temp292)->f1; goto _LL301;} else{ goto _LL302;}
_LL302: goto _LL303; _LL295: { struct Cyc_List_List* _temp312=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp304->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp313=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp313[ 0]=({ struct Cyc_Absyn_StructType_struct _temp314; _temp314.tag= Cyc_Absyn_StructType;
_temp314.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp304->name))->v); _temp314.f2= _temp312; _temp314.f3= 0;
_temp314;}); _temp313;}); if( _temp304->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp315=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp315->v=( void*) _temp284; _temp315;});} goto _LL293;} _LL297: { struct Cyc_List_List*
_temp316=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp306->tvs); t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp317=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp317[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp318; _temp318.tag= Cyc_Absyn_TunionType;
_temp318.f1=({ struct Cyc_Absyn_TunionInfo _temp319; _temp319.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp320=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp320[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp321; _temp321.tag= Cyc_Absyn_KnownTunion;
_temp321.f1= _temp306; _temp321;}); _temp320;})); _temp319.targs= _temp316;
_temp319.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp319;}); _temp318;});
_temp317;}); if( _temp306->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp322=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp322->v=( void*) _temp284; _temp322;});} goto _LL293;} _LL299: { struct Cyc_List_List*
_temp323=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp308->tvs); t=(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp324=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp324[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp325; _temp325.tag= Cyc_Absyn_UnionType; _temp325.f1=(
struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp308->name))->v); _temp325.f2= _temp323; _temp325.f3= 0; _temp325;});
_temp324;}); if( _temp308->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp326=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp326->v=( void*) _temp284; _temp326;});} goto _LL293;} _LL301: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp327=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp327[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp328; _temp328.tag= Cyc_Absyn_EnumType; _temp328.f1=
_temp310->name; _temp328.f2= 0; _temp328;}); _temp327;}); declopt=({ struct Cyc_Core_Opt*
_temp329=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp329->v=( void*) _temp284; _temp329;}); goto _LL293; _LL303:(( int(*)(
struct _tagged_arr msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(
_tag_arr("bad declaration within type specifier", sizeof( unsigned char), 38u),
_temp284->loc); goto _LL293; _LL293:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3,
_temp284->loc);} goto _LL259; _LL259:;} if( ! seen_type){ if( ! seen_sign? !
seen_size: 0){ Cyc_Parse_err( _tag_arr("missing type withing specifier", sizeof(
unsigned char), 31u), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp330=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp330[ 0]=({ struct Cyc_Absyn_IntType_struct _temp331; _temp331.tag= Cyc_Absyn_IntType;
_temp331.f1=( void*) sgn; _temp331.f2=( void*) sz; _temp331;}); _temp330;});}
else{ if( seen_sign){ void* _temp332= t; void* _temp338; _LL334: if((
unsigned int) _temp332 > 4u?*(( int*) _temp332) == Cyc_Absyn_IntType: 0){ _LL339:
_temp338=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp332)->f2; goto _LL335;}
else{ goto _LL336;} _LL336: goto _LL337; _LL335: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp340=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp340[ 0]=({ struct Cyc_Absyn_IntType_struct _temp341; _temp341.tag= Cyc_Absyn_IntType;
_temp341.f1=( void*) sgn; _temp341.f2=( void*) _temp338; _temp341;}); _temp340;});
goto _LL333; _LL337: Cyc_Parse_err( _tag_arr("sign specification on non-integral type",
sizeof( unsigned char), 40u), last_loc); goto _LL333; _LL333:;} if( seen_size){
void* _temp342= t; void* _temp348; _LL344: if(( unsigned int) _temp342 > 4u?*((
int*) _temp342) == Cyc_Absyn_IntType: 0){ _LL349: _temp348=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp342)->f1; goto _LL345;} else{ goto _LL346;} _LL346: goto _LL347; _LL345: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp350=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp350[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp351; _temp351.tag= Cyc_Absyn_IntType; _temp351.f1=( void*) _temp348;
_temp351.f2=( void*) sz; _temp351;}); _temp350;}); goto _LL343; _LL347: Cyc_Parse_err(
_tag_arr("size qualifier on non-integral type", sizeof( unsigned char), 36u),
last_loc); goto _LL343; _LL343:;}} return({ struct _tuple5 _temp352; _temp352.f1=
t; _temp352.f2= declopt; _temp352;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp353=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tuple1* _temp354= _temp353->id; struct Cyc_List_List* _temp357; struct
Cyc_List_List* _temp359; void* _temp361; struct Cyc_Absyn_Tqual _temp363; struct
_tuple6 _temp355= Cyc_Parse_apply_tms( tq, t, shared_atts, _temp353->tms);
_LL364: _temp363= _temp355.f1; goto _LL362; _LL362: _temp361= _temp355.f2; goto
_LL360; _LL360: _temp359= _temp355.f3; goto _LL358; _LL358: _temp357= _temp355.f4;
goto _LL356; _LL356: return({ struct Cyc_List_List* _temp365=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp365->hd=( void*)({ struct
_tuple7* _temp366=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp366->f1= _temp354; _temp366->f2= _temp363; _temp366->f3= _temp361; _temp366->f4=
_temp359; _temp366->f5= _temp357; _temp366;}); _temp365->tl= Cyc_Parse_apply_tmss(
_temp363, Cyc_Tcutil_copy_type( t),(( struct Cyc_List_List*) _check_null( ds))->tl,
shared_atts); _temp365;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp367; _temp367.f1= tq; _temp367.f2=
t; _temp367.f3= 0; _temp367.f4= atts; _temp367;});}{ void* _temp368=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp382;
void* _temp384; struct Cyc_Position_Segment* _temp386; struct Cyc_List_List*
_temp388; struct Cyc_Absyn_Tqual _temp390; void* _temp392; void* _temp394;
struct Cyc_List_List* _temp396; struct Cyc_Position_Segment* _temp398; _LL370:
if( _temp368 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL371;} else{ goto _LL372;}
_LL372: if(( unsigned int) _temp368 > 1u?*(( int*) _temp368) == Cyc_Absyn_ConstArray_mod:
0){ _LL383: _temp382=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp368)->f1;
goto _LL373;} else{ goto _LL374;} _LL374: if(( unsigned int) _temp368 > 1u?*((
int*) _temp368) == Cyc_Absyn_Function_mod: 0){ _LL385: _temp384=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp368)->f1; goto _LL375;} else{ goto
_LL376;} _LL376: if(( unsigned int) _temp368 > 1u?*(( int*) _temp368) == Cyc_Absyn_TypeParams_mod:
0){ _LL389: _temp388=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp368)->f1;
goto _LL387; _LL387: _temp386=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp368)->f2; goto _LL377;} else{ goto _LL378;} _LL378: if(( unsigned int)
_temp368 > 1u?*(( int*) _temp368) == Cyc_Absyn_Pointer_mod: 0){ _LL395: _temp394=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp368)->f1; goto _LL393;
_LL393: _temp392=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp368)->f2;
goto _LL391; _LL391: _temp390=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp368)->f3;
goto _LL379;} else{ goto _LL380;} _LL380: if(( unsigned int) _temp368 > 1u?*((
int*) _temp368) == Cyc_Absyn_Attributes_mod: 0){ _LL399: _temp398=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp368)->f1; goto _LL397; _LL397: _temp396=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp368)->f2; goto _LL381;} else{ goto _LL369;} _LL371: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp400=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp400[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp401; _temp401.tag= Cyc_Absyn_ArrayType;
_temp401.f1=( void*) t; _temp401.f2= tq; _temp401.f3= 0; _temp401;}); _temp400;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL373: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp402=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp402[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp403; _temp403.tag= Cyc_Absyn_ArrayType;
_temp403.f1=( void*) t; _temp403.f2= tq; _temp403.f3=( struct Cyc_Absyn_Exp*)
_temp382; _temp403;}); _temp402;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL375: { void* _temp404= _temp384; struct Cyc_List_List* _temp410;
struct Cyc_Core_Opt* _temp412; struct Cyc_Absyn_VarargInfo* _temp414; int
_temp416; struct Cyc_List_List* _temp418; struct Cyc_Position_Segment* _temp420;
_LL406: if(*(( int*) _temp404) == Cyc_Absyn_WithTypes){ _LL419: _temp418=((
struct Cyc_Absyn_WithTypes_struct*) _temp404)->f1; goto _LL417; _LL417: _temp416=((
struct Cyc_Absyn_WithTypes_struct*) _temp404)->f2; goto _LL415; _LL415: _temp414=((
struct Cyc_Absyn_WithTypes_struct*) _temp404)->f3; goto _LL413; _LL413: _temp412=((
struct Cyc_Absyn_WithTypes_struct*) _temp404)->f4; goto _LL411; _LL411: _temp410=((
struct Cyc_Absyn_WithTypes_struct*) _temp404)->f5; goto _LL407;} else{ goto
_LL408;} _LL408: if(*(( int*) _temp404) == Cyc_Absyn_NoTypes){ _LL421: _temp420=((
struct Cyc_Absyn_NoTypes_struct*) _temp404)->f2; goto _LL409;} else{ goto _LL405;}
_LL407: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp422=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp422->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp422->tl= fn_atts; _temp422;});} else{ new_atts=({ struct Cyc_List_List*
_temp423=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp423->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp423->tl= new_atts; _temp423;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp424=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_List_List*
_temp430; _LL426: if(( unsigned int) _temp424 > 1u?*(( int*) _temp424) == Cyc_Absyn_TypeParams_mod:
0){ _LL431: _temp430=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp424)->f1;
goto _LL427;} else{ goto _LL428;} _LL428: goto _LL429; _LL427: typvars= _temp430;
tms=(( struct Cyc_List_List*) _check_null( tms))->tl; goto _LL425; _LL429: goto
_LL425; _LL425:;} if((((( ! _temp416? _temp414 == 0: 0)? _temp418 != 0: 0)?((
struct Cyc_List_List*) _check_null( _temp418))->tl == 0: 0)?(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp418))->hd)).f1 == 0: 0)?(*(( struct
_tuple2*)(( struct Cyc_List_List*) _check_null( _temp418))->hd)).f3 ==( void*)
Cyc_Absyn_VoidType: 0){ _temp418= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)(
void(* f)( struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp418); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp412, t, _temp418, _temp416, _temp414, _temp410, fn_atts), new_atts,((
struct Cyc_List_List*) _check_null( tms))->tl);} _LL409:( int) _throw( Cyc_Parse_abort(
_tag_arr("function declaration without parameter types", sizeof( unsigned char),
45u), _temp420)); _LL405:;} _LL377: if((( struct Cyc_List_List*) _check_null(
tms))->tl == 0){ return({ struct _tuple6 _temp432; _temp432.f1= tq; _temp432.f2=
t; _temp432.f3= _temp388; _temp432.f4= atts; _temp432;});}( int) _throw( Cyc_Parse_abort(
_tag_arr("type parameters must appear before function arguments in declarator",
sizeof( unsigned char), 68u), _temp386)); _LL379: { void* _temp433= _temp394;
struct Cyc_Absyn_Exp* _temp441; struct Cyc_Absyn_Exp* _temp443; _LL435: if((
unsigned int) _temp433 > 1u?*(( int*) _temp433) == Cyc_Absyn_NonNullable_ps: 0){
_LL442: _temp441=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp433)->f1; goto
_LL436;} else{ goto _LL437;} _LL437: if(( unsigned int) _temp433 > 1u?*(( int*)
_temp433) == Cyc_Absyn_Nullable_ps: 0){ _LL444: _temp443=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp433)->f1; goto _LL438;} else{ goto _LL439;} _LL439: if( _temp433 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL440;} else{ goto _LL434;} _LL436: return Cyc_Parse_apply_tms(
_temp390, Cyc_Absyn_atb_typ( t, _temp392, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp445=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp445[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp446; _temp446.tag= Cyc_Absyn_Upper_b;
_temp446.f1= _temp441; _temp446;}); _temp445;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL438: return Cyc_Parse_apply_tms( _temp390, Cyc_Absyn_starb_typ(
t, _temp392, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp447=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp447[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp448; _temp448.tag= Cyc_Absyn_Upper_b;
_temp448.f1= _temp443; _temp448;}); _temp447;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL440: return Cyc_Parse_apply_tms( _temp390, Cyc_Absyn_tagged_typ(
t, _temp392, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL434:;}
_LL381: return Cyc_Parse_apply_tms( tq, t, Cyc_List_append( atts, _temp396),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL369:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp451; void* _temp453; struct _tuple5 _temp449= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL454: _temp453= _temp449.f1; goto _LL452; _LL452: _temp451=
_temp449.f2; goto _LL450; _LL450: if( _temp451 != 0){ Cyc_Parse_warn( _tag_arr("ignoring nested type declaration(s) in specifier list",
sizeof( unsigned char), 54u), loc);} return _temp453;} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp455=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp455[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp456; _temp456.tag= Cyc_Absyn_Decl_s;
_temp456.f1= d; _temp456.f2= s; _temp456;}); _temp455;}), Cyc_Position_segment_join(
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
_temp457=( enum  Cyc_Parse_Storage_class)(( struct Cyc_Core_Opt*) _check_null(
ds->sc))->v; _LL459: if( _temp457 == Cyc_Parse_Typedef_sc){ goto _LL460;} else{
goto _LL461;} _LL461: if( _temp457 == Cyc_Parse_Extern_sc){ goto _LL462;} else{
goto _LL463;} _LL463: if( _temp457 == Cyc_Parse_ExternC_sc){ goto _LL464;} else{
goto _LL465;} _LL465: if( _temp457 == Cyc_Parse_Static_sc){ goto _LL466;} else{
goto _LL467;} _LL467: if( _temp457 == Cyc_Parse_Auto_sc){ goto _LL468;} else{
goto _LL469;} _LL469: if( _temp457 == Cyc_Parse_Register_sc){ goto _LL470;}
else{ goto _LL471;} _LL471: if( _temp457 == Cyc_Parse_Abstract_sc){ goto _LL472;}
else{ goto _LL458;} _LL460: istypedef= 1; goto _LL458; _LL462: s=( void*) Cyc_Absyn_Extern;
goto _LL458; _LL464: s=( void*) Cyc_Absyn_ExternC; goto _LL458; _LL466: s=( void*)
Cyc_Absyn_Static; goto _LL458; _LL468: s=( void*) Cyc_Absyn_Public; goto _LL458;
_LL470: s=( void*) Cyc_Absyn_Public; goto _LL458; _LL472: s=( void*) Cyc_Absyn_Abstract;
goto _LL458; _LL458:;}{ struct Cyc_List_List* _temp475; struct Cyc_List_List*
_temp477; struct _tuple0 _temp473=(( struct _tuple0(*)( struct Cyc_List_List* x))
Cyc_List_split)( ids); _LL478: _temp477= _temp473.f1; goto _LL476; _LL476:
_temp475= _temp473.f2; goto _LL474; _LL474: { int exps_empty= 1;{ struct Cyc_List_List*
es= _temp475; for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){
if(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd != 0){
exps_empty= 0; break;}}}{ struct _tuple5 _temp479= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp477 == 0){ struct Cyc_Core_Opt* _temp482; void* _temp484;
struct _tuple5 _temp480= _temp479; _LL485: _temp484= _temp480.f1; goto _LL483;
_LL483: _temp482= _temp480.f2; goto _LL481; _LL481: if( _temp482 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*) _check_null(
_temp482))->v;{ void* _temp486=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp498; struct Cyc_Absyn_Structdecl* _temp500; struct Cyc_Absyn_Uniondecl*
_temp502; struct Cyc_Absyn_Tuniondecl* _temp504; _LL488: if(*(( int*) _temp486)
== Cyc_Absyn_Enum_d){ _LL499: _temp498=(( struct Cyc_Absyn_Enum_d_struct*)
_temp486)->f1; goto _LL489;} else{ goto _LL490;} _LL490: if(*(( int*) _temp486)
== Cyc_Absyn_Struct_d){ _LL501: _temp500=(( struct Cyc_Absyn_Struct_d_struct*)
_temp486)->f1; goto _LL491;} else{ goto _LL492;} _LL492: if(*(( int*) _temp486)
== Cyc_Absyn_Union_d){ _LL503: _temp502=(( struct Cyc_Absyn_Union_d_struct*)
_temp486)->f1; goto _LL493;} else{ goto _LL494;} _LL494: if(*(( int*) _temp486)
== Cyc_Absyn_Tunion_d){ _LL505: _temp504=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp486)->f1; goto _LL495;} else{ goto _LL496;} _LL496: goto _LL497; _LL489:(
void*)( _temp498->sc=( void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on enum",
sizeof( unsigned char), 23u), loc);} goto _LL487; _LL491:( void*)( _temp500->sc=(
void*) s); _temp500->attributes= atts; goto _LL487; _LL493:( void*)( _temp502->sc=(
void*) s); _temp502->attributes= atts; goto _LL487; _LL495:( void*)( _temp504->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} goto _LL487; _LL497: Cyc_Parse_err(
_tag_arr("bad declaration", sizeof( unsigned char), 16u), loc); return 0; _LL487:;}
return({ struct Cyc_List_List* _temp506=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp506->hd=( void*) d; _temp506->tl= 0;
_temp506;});} else{ void* _temp507= _temp484; struct Cyc_List_List* _temp521;
struct _tuple1* _temp523; struct Cyc_Absyn_TunionInfo _temp525; void* _temp527;
struct Cyc_Absyn_Tuniondecl* _temp529; struct Cyc_Absyn_TunionInfo _temp531;
struct Cyc_List_List* _temp533; void* _temp535; struct Cyc_Absyn_UnknownTunionInfo
_temp537; int _temp539; struct _tuple1* _temp541; struct Cyc_List_List* _temp543;
struct _tuple1* _temp545; struct _tuple1* _temp547; _LL509: if(( unsigned int)
_temp507 > 4u?*(( int*) _temp507) == Cyc_Absyn_StructType: 0){ _LL524: _temp523=((
struct Cyc_Absyn_StructType_struct*) _temp507)->f1; goto _LL522; _LL522:
_temp521=(( struct Cyc_Absyn_StructType_struct*) _temp507)->f2; goto _LL510;}
else{ goto _LL511;} _LL511: if(( unsigned int) _temp507 > 4u?*(( int*) _temp507)
== Cyc_Absyn_TunionType: 0){ _LL526: _temp525=(( struct Cyc_Absyn_TunionType_struct*)
_temp507)->f1; _LL528: _temp527=( void*) _temp525.tunion_info; if(*(( int*)
_temp527) == Cyc_Absyn_KnownTunion){ _LL530: _temp529=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp527)->f1; goto _LL512;} else{ goto _LL513;}} else{ goto _LL513;} _LL513:
if(( unsigned int) _temp507 > 4u?*(( int*) _temp507) == Cyc_Absyn_TunionType: 0){
_LL532: _temp531=(( struct Cyc_Absyn_TunionType_struct*) _temp507)->f1; _LL536:
_temp535=( void*) _temp531.tunion_info; if(*(( int*) _temp535) == Cyc_Absyn_UnknownTunion){
_LL538: _temp537=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp535)->f1;
_LL542: _temp541= _temp537.name; goto _LL540; _LL540: _temp539= _temp537.is_xtunion;
goto _LL534;} else{ goto _LL515;} _LL534: _temp533= _temp531.targs; goto _LL514;}
else{ goto _LL515;} _LL515: if(( unsigned int) _temp507 > 4u?*(( int*) _temp507)
== Cyc_Absyn_UnionType: 0){ _LL546: _temp545=(( struct Cyc_Absyn_UnionType_struct*)
_temp507)->f1; goto _LL544; _LL544: _temp543=(( struct Cyc_Absyn_UnionType_struct*)
_temp507)->f2; goto _LL516;} else{ goto _LL517;} _LL517: if(( unsigned int)
_temp507 > 4u?*(( int*) _temp507) == Cyc_Absyn_EnumType: 0){ _LL548: _temp547=((
struct Cyc_Absyn_EnumType_struct*) _temp507)->f1; goto _LL518;} else{ goto
_LL519;} _LL519: goto _LL520; _LL510: { struct Cyc_List_List* _temp549=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp521); struct Cyc_Absyn_Structdecl* _temp550=({
struct Cyc_Absyn_Structdecl* _temp554=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp554->sc=( void*) s; _temp554->name=({
struct Cyc_Core_Opt* _temp555=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp555->v=( void*)(( struct _tuple1*) _check_null( _temp523));
_temp555;}); _temp554->tvs= _temp549; _temp554->fields= 0; _temp554->attributes=
0; _temp554;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on struct",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp551=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp551->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp552=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp552[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp553; _temp553.tag= Cyc_Absyn_Struct_d;
_temp553.f1= _temp550; _temp553;}); _temp552;}), loc); _temp551->tl= 0; _temp551;});}
_LL512: if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp556=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp556->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp557=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp557[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp558; _temp558.tag= Cyc_Absyn_Tunion_d;
_temp558.f1= _temp529; _temp558;}); _temp557;}), loc); _temp556->tl= 0; _temp556;});
_LL514: { struct Cyc_List_List* _temp559=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp533);
struct Cyc_Absyn_Decl* _temp560= Cyc_Absyn_tunion_decl( s, _temp541, _temp559, 0,
_temp539, loc); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on tunion",
sizeof( unsigned char), 25u), loc);} return({ struct Cyc_List_List* _temp561=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp561->hd=(
void*) _temp560; _temp561->tl= 0; _temp561;});} _LL516: { struct Cyc_List_List*
_temp562=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp543); struct Cyc_Absyn_Uniondecl* _temp563=({
struct Cyc_Absyn_Uniondecl* _temp568=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp568->sc=( void*) s; _temp568->name=({
struct Cyc_Core_Opt* _temp569=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp569->v=( void*)(( struct _tuple1*) _check_null( _temp545));
_temp569;}); _temp568->tvs= _temp562; _temp568->fields= 0; _temp568->attributes=
0; _temp568;}); if( atts != 0){ Cyc_Parse_err( _tag_arr("bad attributes on union",
sizeof( unsigned char), 24u), loc);} return({ struct Cyc_List_List* _temp564=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp564->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp565=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp565->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp566=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp566[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp567; _temp567.tag= Cyc_Absyn_Union_d;
_temp567.f1= _temp563; _temp567;}); _temp566;})); _temp565->loc= loc; _temp565;});
_temp564->tl= 0; _temp564;});} _LL518: { struct Cyc_Absyn_Enumdecl* _temp570=({
struct Cyc_Absyn_Enumdecl* _temp575=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp575->sc=( void*) s; _temp575->name=
_temp547; _temp575->fields= 0; _temp575;}); if( atts != 0){ Cyc_Parse_err(
_tag_arr("bad attributes on enum", sizeof( unsigned char), 23u), loc);} return({
struct Cyc_List_List* _temp571=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp571->hd=( void*)({ struct Cyc_Absyn_Decl* _temp572=(
struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp572->r=(
void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp573=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp573[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp574; _temp574.tag= Cyc_Absyn_Enum_d; _temp574.f1= _temp570; _temp574;});
_temp573;})); _temp572->loc= loc; _temp572;}); _temp571->tl= 0; _temp571;});}
_LL520: Cyc_Parse_err( _tag_arr("missing declarator", sizeof( unsigned char), 19u),
loc); return 0; _LL508:;}} else{ void* t= _temp479.f1; struct Cyc_List_List*
_temp576= Cyc_Parse_apply_tmss( tq, t, _temp477, atts); if( istypedef){ if( !
exps_empty){ Cyc_Parse_err( _tag_arr("initializer in typedef declaration",
sizeof( unsigned char), 35u), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, _temp576); if( _temp479.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*) _check_null( _temp479.f2))->v;{ void* _temp577=( void*) d->r;
struct Cyc_Absyn_Structdecl* _temp589; struct Cyc_Absyn_Tuniondecl* _temp591;
struct Cyc_Absyn_Uniondecl* _temp593; struct Cyc_Absyn_Enumdecl* _temp595;
_LL579: if(*(( int*) _temp577) == Cyc_Absyn_Struct_d){ _LL590: _temp589=((
struct Cyc_Absyn_Struct_d_struct*) _temp577)->f1; goto _LL580;} else{ goto
_LL581;} _LL581: if(*(( int*) _temp577) == Cyc_Absyn_Tunion_d){ _LL592: _temp591=((
struct Cyc_Absyn_Tunion_d_struct*) _temp577)->f1; goto _LL582;} else{ goto
_LL583;} _LL583: if(*(( int*) _temp577) == Cyc_Absyn_Union_d){ _LL594: _temp593=((
struct Cyc_Absyn_Union_d_struct*) _temp577)->f1; goto _LL584;} else{ goto _LL585;}
_LL585: if(*(( int*) _temp577) == Cyc_Absyn_Enum_d){ _LL596: _temp595=(( struct
Cyc_Absyn_Enum_d_struct*) _temp577)->f1; goto _LL586;} else{ goto _LL587;}
_LL587: goto _LL588; _LL580:( void*)( _temp589->sc=( void*) s); _temp589->attributes=
atts; atts= 0; goto _LL578; _LL582:( void*)( _temp591->sc=( void*) s); goto
_LL578; _LL584:( void*)( _temp593->sc=( void*) s); goto _LL578; _LL586:( void*)(
_temp595->sc=( void*) s); goto _LL578; _LL588: Cyc_Parse_err( _tag_arr("declaration within typedef is not a struct, union, tunion, or xtunion",
sizeof( unsigned char), 70u), loc); goto _LL578; _LL578:;} decls=({ struct Cyc_List_List*
_temp597=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp597->hd=( void*) d; _temp597->tl= decls; _temp597;});} if( atts != 0){ Cyc_Parse_err((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp599; _temp599.tag=
Cyc_Stdio_String_pa; _temp599.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp598[ 1u]={&
_temp599}; Cyc_Stdio_aprintf( _tag_arr("bad attribute %s in typedef", sizeof(
unsigned char), 28u), _tag_arr( _temp598, sizeof( void*), 1u));}}), loc);}
return decls;}} else{ if( _temp479.f2 != 0){ Cyc_Parse_unimp2( _tag_arr("nested type declaration within declarator",
sizeof( unsigned char), 42u), loc);}{ struct Cyc_List_List* decls= 0;{ struct
Cyc_List_List* _temp600= _temp576; for( 0; _temp600 != 0;( _temp600=(( struct
Cyc_List_List*) _check_null( _temp600))->tl, _temp475=(( struct Cyc_List_List*)
_check_null( _temp475))->tl)){ struct _tuple7 _temp603; struct Cyc_List_List*
_temp604; struct Cyc_List_List* _temp606; void* _temp608; struct Cyc_Absyn_Tqual
_temp610; struct _tuple1* _temp612; struct _tuple7* _temp601=( struct _tuple7*)((
struct Cyc_List_List*) _check_null( _temp600))->hd; _temp603=* _temp601; _LL613:
_temp612= _temp603.f1; goto _LL611; _LL611: _temp610= _temp603.f2; goto _LL609;
_LL609: _temp608= _temp603.f3; goto _LL607; _LL607: _temp606= _temp603.f4; goto
_LL605; _LL605: _temp604= _temp603.f5; goto _LL602; _LL602: if( _temp606 != 0){
Cyc_Parse_warn( _tag_arr("bad type params, ignoring", sizeof( unsigned char), 26u),
loc);} if( _temp475 == 0){(( int(*)( struct _tagged_arr msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)( _tag_arr("unexpected null in parse!", sizeof(
unsigned char), 26u), loc);}{ struct Cyc_Absyn_Exp* _temp614=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp475))->hd; struct Cyc_Absyn_Vardecl*
_temp615= Cyc_Absyn_new_vardecl( _temp612, _temp608, _temp614); _temp615->tq=
_temp610;( void*)( _temp615->sc=( void*) s); _temp615->attributes= _temp604;{
struct Cyc_Absyn_Decl* _temp616=({ struct Cyc_Absyn_Decl* _temp618=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp618->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp619=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp619[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp620; _temp620.tag= Cyc_Absyn_Var_d; _temp620.f1= _temp615; _temp620;});
_temp619;})); _temp618->loc= loc; _temp618;}); decls=({ struct Cyc_List_List*
_temp617=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp617->hd=( void*) _temp616; _temp617->tl= decls; _temp617;});}}}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ if( Cyc_String_strlen( s) != 1){ Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp622; _temp622.tag= Cyc_Stdio_String_pa;
_temp622.f1=( struct _tagged_arr) s;{ void* _temp621[ 1u]={& _temp622}; Cyc_Stdio_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp621,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0))){
case 'A': _LL623: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL624: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL625: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL626: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL627: return(
void*) Cyc_Absyn_EffKind; default: _LL628: Cyc_Parse_err(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp631; _temp631.tag= Cyc_Stdio_String_pa;
_temp631.f1=( struct _tagged_arr) s;{ void* _temp630[ 1u]={& _temp631}; Cyc_Stdio_aprintf(
_tag_arr("bad kind: %s", sizeof( unsigned char), 13u), _tag_arr( _temp630,
sizeof( void*), 1u));}}), loc); return( void*) Cyc_Absyn_BoxKind;}}} static
struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){ return
tms;} else{ return({ struct Cyc_List_List* _temp632=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp632->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp633=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp633[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp634; _temp634.tag= Cyc_Absyn_Attributes_mod;
_temp634.f1= loc; _temp634.f2= atts; _temp634;}); _temp633;})); _temp632->tl=
tms; _temp632;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple7 _temp637;
struct Cyc_List_List* _temp638; struct Cyc_List_List* _temp640; void* _temp642;
struct Cyc_Absyn_Tqual _temp644; struct _tuple1* _temp646; struct _tuple7*
_temp635= t; _temp637=* _temp635; _LL647: _temp646= _temp637.f1; goto _LL645;
_LL645: _temp644= _temp637.f2; goto _LL643; _LL643: _temp642= _temp637.f3; goto
_LL641; _LL641: _temp640= _temp637.f4; goto _LL639; _LL639: _temp638= _temp637.f5;
goto _LL636; _LL636: Cyc_Lex_register_typedef( _temp646); if( _temp638 != 0){
Cyc_Parse_err(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp649;
_temp649.tag= Cyc_Stdio_String_pa; _temp649.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp638))->hd);{ void* _temp648[ 1u]={&
_temp649}; Cyc_Stdio_aprintf( _tag_arr("bad attribute %s within typedef",
sizeof( unsigned char), 32u), _tag_arr( _temp648, sizeof( void*), 1u));}}), loc);}
return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp650=(
struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp650[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp651; _temp651.tag= Cyc_Absyn_Typedef_d;
_temp651.f1=({ struct Cyc_Absyn_Typedefdecl* _temp652=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp652->name= _temp646;
_temp652->tvs= _temp640; _temp652->defn=( void*) _temp642; _temp652;}); _temp651;});
_temp650;}), loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
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
unsigned char _temp654[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp654, _temp654, _temp654 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp655= yy1; struct _tuple16* _temp661; _LL657: if(*(( void**)
_temp655) == Cyc_Int_tok){ _LL662: _temp661=(( struct Cyc_Int_tok_struct*)
_temp655)->f1; goto _LL658;} else{ goto _LL659;} _LL659: goto _LL660; _LL658:
return _temp661; _LL660:( int) _throw(( void*)& Cyc_yyfail_Int_tok); _LL656:;}
static unsigned char _temp664[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp664, _temp664, _temp664 + 11u}};
struct _tagged_arr Cyc_yyget_String_tok( void* yy1){ void* _temp665= yy1; struct
_tagged_arr _temp671; _LL667: if(*(( void**) _temp665) == Cyc_String_tok){
_LL672: _temp671=(( struct Cyc_String_tok_struct*) _temp665)->f1; goto _LL668;}
else{ goto _LL669;} _LL669: goto _LL670; _LL668: return _temp671; _LL670:( int)
_throw(( void*)& Cyc_yyfail_String_tok); _LL666:;} static unsigned char _temp674[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp674, _temp674, _temp674 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp675= yy1; unsigned char _temp681; _LL677: if(*(( void**) _temp675) ==
Cyc_Char_tok){ _LL682: _temp681=(( struct Cyc_Char_tok_struct*) _temp675)->f1;
goto _LL678;} else{ goto _LL679;} _LL679: goto _LL680; _LL678: return _temp681;
_LL680:( int) _throw(( void*)& Cyc_yyfail_Char_tok); _LL676:;} static
unsigned char _temp684[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp684, _temp684, _temp684 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp685= yy1; void*
_temp691; _LL687: if(*(( void**) _temp685) == Cyc_Pointer_Sort_tok){ _LL692:
_temp691=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp685)->f1; goto
_LL688;} else{ goto _LL689;} _LL689: goto _LL690; _LL688: return _temp691;
_LL690:( int) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL686:;} static
unsigned char _temp694[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp694, _temp694, _temp694 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp695= yy1; struct Cyc_Absyn_Exp*
_temp701; _LL697: if(*(( void**) _temp695) == Cyc_Exp_tok){ _LL702: _temp701=((
struct Cyc_Exp_tok_struct*) _temp695)->f1; goto _LL698;} else{ goto _LL699;}
_LL699: goto _LL700; _LL698: return _temp701; _LL700:( int) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL696:;} static unsigned char _temp704[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{ _temp704, _temp704, _temp704 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( void* yy1){ void* _temp705= yy1;
struct Cyc_List_List* _temp711; _LL707: if(*(( void**) _temp705) == Cyc_ExpList_tok){
_LL712: _temp711=(( struct Cyc_ExpList_tok_struct*) _temp705)->f1; goto _LL708;}
else{ goto _LL709;} _LL709: goto _LL710; _LL708: return _temp711; _LL710:( int)
_throw(( void*)& Cyc_yyfail_ExpList_tok); _LL706:;} static unsigned char
_temp714[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure,{ _temp714, _temp714, _temp714 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp715= yy1; struct Cyc_List_List*
_temp721; _LL717: if(*(( void**) _temp715) == Cyc_InitializerList_tok){ _LL722:
_temp721=(( struct Cyc_InitializerList_tok_struct*) _temp715)->f1; goto _LL718;}
else{ goto _LL719;} _LL719: goto _LL720; _LL718: return _temp721; _LL720:( int)
_throw(( void*)& Cyc_yyfail_InitializerList_tok); _LL716:;} static unsigned char
_temp724[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primop_tok={
Cyc_Core_Failure,{ _temp724, _temp724, _temp724 + 11u}}; void* Cyc_yyget_Primop_tok(
void* yy1){ void* _temp725= yy1; void* _temp731; _LL727: if(*(( void**) _temp725)
== Cyc_Primop_tok){ _LL732: _temp731=( void*)(( struct Cyc_Primop_tok_struct*)
_temp725)->f1; goto _LL728;} else{ goto _LL729;} _LL729: goto _LL730; _LL728:
return _temp731; _LL730:( int) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL726:;}
static unsigned char _temp734[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{ _temp734, _temp734, _temp734 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp735= yy1;
struct Cyc_Core_Opt* _temp741; _LL737: if(*(( void**) _temp735) == Cyc_Primopopt_tok){
_LL742: _temp741=(( struct Cyc_Primopopt_tok_struct*) _temp735)->f1; goto _LL738;}
else{ goto _LL739;} _LL739: goto _LL740; _LL738: return _temp741; _LL740:( int)
_throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL736:;} static unsigned char
_temp744[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{ _temp744, _temp744, _temp744 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok(
void* yy1){ void* _temp745= yy1; struct _tuple1* _temp751; _LL747: if(*(( void**)
_temp745) == Cyc_QualId_tok){ _LL752: _temp751=(( struct Cyc_QualId_tok_struct*)
_temp745)->f1; goto _LL748;} else{ goto _LL749;} _LL749: goto _LL750; _LL748:
return _temp751; _LL750:( int) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL746:;}
static unsigned char _temp754[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp754, _temp754, _temp754 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp755= yy1;
struct Cyc_Absyn_Stmt* _temp761; _LL757: if(*(( void**) _temp755) == Cyc_Stmt_tok){
_LL762: _temp761=(( struct Cyc_Stmt_tok_struct*) _temp755)->f1; goto _LL758;}
else{ goto _LL759;} _LL759: goto _LL760; _LL758: return _temp761; _LL760:( int)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL756:;} static unsigned char _temp764[
21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={
Cyc_Core_Failure,{ _temp764, _temp764, _temp764 + 21u}}; struct Cyc_List_List*
Cyc_yyget_SwitchClauseList_tok( void* yy1){ void* _temp765= yy1; struct Cyc_List_List*
_temp771; _LL767: if(*(( void**) _temp765) == Cyc_SwitchClauseList_tok){ _LL772:
_temp771=(( struct Cyc_SwitchClauseList_tok_struct*) _temp765)->f1; goto _LL768;}
else{ goto _LL769;} _LL769: goto _LL770; _LL768: return _temp771; _LL770:( int)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL766:;} static
unsigned char _temp774[ 22u]="SwitchCClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchCClauseList_tok={ Cyc_Core_Failure,{ _temp774, _temp774,
_temp774 + 22u}}; struct Cyc_List_List* Cyc_yyget_SwitchCClauseList_tok( void*
yy1){ void* _temp775= yy1; struct Cyc_List_List* _temp781; _LL777: if(*(( void**)
_temp775) == Cyc_SwitchCClauseList_tok){ _LL782: _temp781=(( struct Cyc_SwitchCClauseList_tok_struct*)
_temp775)->f1; goto _LL778;} else{ goto _LL779;} _LL779: goto _LL780; _LL778:
return _temp781; _LL780:( int) _throw(( void*)& Cyc_yyfail_SwitchCClauseList_tok);
_LL776:;} static unsigned char _temp784[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp784, _temp784, _temp784 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp785= yy1;
struct Cyc_Absyn_Pat* _temp791; _LL787: if(*(( void**) _temp785) == Cyc_Pattern_tok){
_LL792: _temp791=(( struct Cyc_Pattern_tok_struct*) _temp785)->f1; goto _LL788;}
else{ goto _LL789;} _LL789: goto _LL790; _LL788: return _temp791; _LL790:( int)
_throw(( void*)& Cyc_yyfail_Pattern_tok); _LL786:;} static unsigned char
_temp794[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp794, _temp794, _temp794 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp795= yy1; struct Cyc_List_List*
_temp801; _LL797: if(*(( void**) _temp795) == Cyc_PatternList_tok){ _LL802:
_temp801=(( struct Cyc_PatternList_tok_struct*) _temp795)->f1; goto _LL798;}
else{ goto _LL799;} _LL799: goto _LL800; _LL798: return _temp801; _LL800:( int)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL796:;} static unsigned char
_temp804[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp804, _temp804, _temp804 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp805= yy1; struct _tuple14* _temp811; _LL807: if(*(( void**)
_temp805) == Cyc_FieldPattern_tok){ _LL812: _temp811=(( struct Cyc_FieldPattern_tok_struct*)
_temp805)->f1; goto _LL808;} else{ goto _LL809;} _LL809: goto _LL810; _LL808:
return _temp811; _LL810:( int) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL806:;} static unsigned char _temp814[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp814, _temp814, _temp814 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp815= yy1; struct Cyc_List_List* _temp821; _LL817: if(*((
void**) _temp815) == Cyc_FieldPatternList_tok){ _LL822: _temp821=(( struct Cyc_FieldPatternList_tok_struct*)
_temp815)->f1; goto _LL818;} else{ goto _LL819;} _LL819: goto _LL820; _LL818:
return _temp821; _LL820:( int) _throw(( void*)& Cyc_yyfail_FieldPatternList_tok);
_LL816:;} static unsigned char _temp824[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp824, _temp824, _temp824 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp825= yy1;
struct Cyc_Absyn_Fndecl* _temp831; _LL827: if(*(( void**) _temp825) == Cyc_FnDecl_tok){
_LL832: _temp831=(( struct Cyc_FnDecl_tok_struct*) _temp825)->f1; goto _LL828;}
else{ goto _LL829;} _LL829: goto _LL830; _LL828: return _temp831; _LL830:( int)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL826:;} static unsigned char _temp834[
13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={
Cyc_Core_Failure,{ _temp834, _temp834, _temp834 + 13u}}; struct Cyc_List_List*
Cyc_yyget_DeclList_tok( void* yy1){ void* _temp835= yy1; struct Cyc_List_List*
_temp841; _LL837: if(*(( void**) _temp835) == Cyc_DeclList_tok){ _LL842:
_temp841=(( struct Cyc_DeclList_tok_struct*) _temp835)->f1; goto _LL838;} else{
goto _LL839;} _LL839: goto _LL840; _LL838: return _temp841; _LL840:( int) _throw((
void*)& Cyc_yyfail_DeclList_tok); _LL836:;} static unsigned char _temp844[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{
_temp844, _temp844, _temp844 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
void* yy1){ void* _temp845= yy1; struct Cyc_Parse_Declaration_spec* _temp851;
_LL847: if(*(( void**) _temp845) == Cyc_DeclSpec_tok){ _LL852: _temp851=((
struct Cyc_DeclSpec_tok_struct*) _temp845)->f1; goto _LL848;} else{ goto _LL849;}
_LL849: goto _LL850; _LL848: return _temp851; _LL850:( int) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL846:;} static unsigned char _temp854[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp854, _temp854, _temp854 + 13u}};
struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp855= yy1; struct
_tuple15* _temp861; _LL857: if(*(( void**) _temp855) == Cyc_InitDecl_tok){
_LL862: _temp861=(( struct Cyc_InitDecl_tok_struct*) _temp855)->f1; goto _LL858;}
else{ goto _LL859;} _LL859: goto _LL860; _LL858: return _temp861; _LL860:( int)
_throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL856:;} static unsigned char
_temp864[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={
Cyc_Core_Failure,{ _temp864, _temp864, _temp864 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp865= yy1; struct Cyc_List_List*
_temp871; _LL867: if(*(( void**) _temp865) == Cyc_InitDeclList_tok){ _LL872:
_temp871=(( struct Cyc_InitDeclList_tok_struct*) _temp865)->f1; goto _LL868;}
else{ goto _LL869;} _LL869: goto _LL870; _LL868: return _temp871; _LL870:( int)
_throw(( void*)& Cyc_yyfail_InitDeclList_tok); _LL866:;} static unsigned char
_temp874[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={
Cyc_Core_Failure,{ _temp874, _temp874, _temp874 + 17u}}; enum  Cyc_Parse_Storage_class
Cyc_yyget_StorageClass_tok( void* yy1){ void* _temp875= yy1; enum  Cyc_Parse_Storage_class
_temp881; _LL877: if(*(( void**) _temp875) == Cyc_StorageClass_tok){ _LL882:
_temp881=(( struct Cyc_StorageClass_tok_struct*) _temp875)->f1; goto _LL878;}
else{ goto _LL879;} _LL879: goto _LL880; _LL878: return _temp881; _LL880:( int)
_throw(( void*)& Cyc_yyfail_StorageClass_tok); _LL876:;} static unsigned char
_temp884[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={
Cyc_Core_Failure,{ _temp884, _temp884, _temp884 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok(
void* yy1){ void* _temp885= yy1; void* _temp891; _LL887: if(*(( void**) _temp885)
== Cyc_TypeSpecifier_tok){ _LL892: _temp891=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp885)->f1; goto _LL888;} else{ goto _LL889;} _LL889: goto _LL890; _LL888:
return _temp891; _LL890:( int) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL886:;} static unsigned char _temp894[ 18u]="StructOrUnion_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp894, _temp894, _temp894 + 18u}}; enum  Cyc_Parse_Struct_or_union Cyc_yyget_StructOrUnion_tok(
void* yy1){ void* _temp895= yy1; enum  Cyc_Parse_Struct_or_union _temp901;
_LL897: if(*(( void**) _temp895) == Cyc_StructOrUnion_tok){ _LL902: _temp901=((
struct Cyc_StructOrUnion_tok_struct*) _temp895)->f1; goto _LL898;} else{ goto
_LL899;} _LL899: goto _LL900; _LL898: return _temp901; _LL900:( int) _throw((
void*)& Cyc_yyfail_StructOrUnion_tok); _LL896:;} static unsigned char _temp904[
13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeQual_tok={
Cyc_Core_Failure,{ _temp904, _temp904, _temp904 + 13u}}; struct Cyc_Absyn_Tqual
Cyc_yyget_TypeQual_tok( void* yy1){ void* _temp905= yy1; struct Cyc_Absyn_Tqual
_temp911; _LL907: if(*(( void**) _temp905) == Cyc_TypeQual_tok){ _LL912:
_temp911=(( struct Cyc_TypeQual_tok_struct*) _temp905)->f1; goto _LL908;} else{
goto _LL909;} _LL909: goto _LL910; _LL908: return _temp911; _LL910:( int) _throw((
void*)& Cyc_yyfail_TypeQual_tok); _LL906:;} static unsigned char _temp914[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp914, _temp914, _temp914 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ void* _temp915= yy1; struct Cyc_List_List* _temp921; _LL917: if(*((
void**) _temp915) == Cyc_StructFieldDeclList_tok){ _LL922: _temp921=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp915)->f1; goto _LL918;} else{ goto
_LL919;} _LL919: goto _LL920; _LL918: return _temp921; _LL920:( int) _throw((
void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL916:;} static unsigned char
_temp924[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp924, _temp924,
_temp924 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp925= yy1; struct Cyc_List_List* _temp931; _LL927: if(*((
void**) _temp925) == Cyc_StructFieldDeclListList_tok){ _LL932: _temp931=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp925)->f1; goto _LL928;}
else{ goto _LL929;} _LL929: goto _LL930; _LL928: return _temp931; _LL930:( int)
_throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL926:;} static
unsigned char _temp934[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp934, _temp934,
_temp934 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp935= yy1; struct Cyc_List_List* _temp941; _LL937: if(*(( void**)
_temp935) == Cyc_TypeModifierList_tok){ _LL942: _temp941=(( struct Cyc_TypeModifierList_tok_struct*)
_temp935)->f1; goto _LL938;} else{ goto _LL939;} _LL939: goto _LL940; _LL938:
return _temp941; _LL940:( int) _throw(( void*)& Cyc_yyfail_TypeModifierList_tok);
_LL936:;} static unsigned char _temp944[ 15u]="Declarator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp944,
_temp944, _temp944 + 15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ void* _temp945= yy1; struct Cyc_Parse_Declarator* _temp951; _LL947:
if(*(( void**) _temp945) == Cyc_Declarator_tok){ _LL952: _temp951=(( struct Cyc_Declarator_tok_struct*)
_temp945)->f1; goto _LL948;} else{ goto _LL949;} _LL949: goto _LL950; _LL948:
return _temp951; _LL950:( int) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL946:;} static unsigned char _temp954[ 21u]="DeclaratorExpopt_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{
_temp954, _temp954, _temp954 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok(
void* yy1){ void* _temp955= yy1; struct _tuple13* _temp961; _LL957: if(*(( void**)
_temp955) == Cyc_DeclaratorExpopt_tok){ _LL962: _temp961=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp955)->f1; goto _LL958;} else{ goto _LL959;} _LL959: goto _LL960; _LL958:
return _temp961; _LL960:( int) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL956:;} static unsigned char _temp964[ 25u]="DeclaratorExpoptList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp964, _temp964, _temp964 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp965= yy1; struct Cyc_List_List* _temp971; _LL967: if(*((
void**) _temp965) == Cyc_DeclaratorExpoptList_tok){ _LL972: _temp971=(( struct
Cyc_DeclaratorExpoptList_tok_struct*) _temp965)->f1; goto _LL968;} else{ goto
_LL969;} _LL969: goto _LL970; _LL968: return _temp971; _LL970:( int) _throw((
void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL966:;} static unsigned char
_temp974[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp974, _temp974,
_temp974 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp975= yy1; struct Cyc_Parse_Abstractdeclarator* _temp981;
_LL977: if(*(( void**) _temp975) == Cyc_AbstractDeclarator_tok){ _LL982:
_temp981=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp975)->f1; goto
_LL978;} else{ goto _LL979;} _LL979: goto _LL980; _LL978: return _temp981;
_LL980:( int) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL976:;}
static unsigned char _temp984[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp984, _temp984, _temp984 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp985= yy1; int _temp991; _LL987:
if(*(( void**) _temp985) == Cyc_Bool_tok){ _LL992: _temp991=(( struct Cyc_Bool_tok_struct*)
_temp985)->f1; goto _LL988;} else{ goto _LL989;} _LL989: goto _LL990; _LL988:
return _temp991; _LL990:( int) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL986:;}
static unsigned char _temp994[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp994, _temp994, _temp994 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp995= yy1; void* _temp1001;
_LL997: if(*(( void**) _temp995) == Cyc_Scope_tok){ _LL1002: _temp1001=( void*)((
struct Cyc_Scope_tok_struct*) _temp995)->f1; goto _LL998;} else{ goto _LL999;}
_LL999: goto _LL1000; _LL998: return _temp1001; _LL1000:( int) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL996:;} static unsigned char _temp1004[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1004, _temp1004, _temp1004 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1005= yy1; struct Cyc_Absyn_Tunionfield* _temp1011;
_LL1007: if(*(( void**) _temp1005) == Cyc_TunionField_tok){ _LL1012: _temp1011=((
struct Cyc_TunionField_tok_struct*) _temp1005)->f1; goto _LL1008;} else{ goto
_LL1009;} _LL1009: goto _LL1010; _LL1008: return _temp1011; _LL1010:( int)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1006:;} static unsigned char
_temp1014[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1014, _temp1014, _temp1014 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1015= yy1; struct Cyc_List_List*
_temp1021; _LL1017: if(*(( void**) _temp1015) == Cyc_TunionFieldList_tok){
_LL1022: _temp1021=(( struct Cyc_TunionFieldList_tok_struct*) _temp1015)->f1;
goto _LL1018;} else{ goto _LL1019;} _LL1019: goto _LL1020; _LL1018: return
_temp1021; _LL1020:( int) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1016:;} static unsigned char _temp1024[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1024, _temp1024, _temp1024 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1025= yy1; struct _tuple18* _temp1031; _LL1027: if(*((
void**) _temp1025) == Cyc_QualSpecList_tok){ _LL1032: _temp1031=(( struct Cyc_QualSpecList_tok_struct*)
_temp1025)->f1; goto _LL1028;} else{ goto _LL1029;} _LL1029: goto _LL1030;
_LL1028: return _temp1031; _LL1030:( int) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1026:;} static unsigned char _temp1034[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1034, _temp1034, _temp1034 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1035= yy1;
struct Cyc_List_List* _temp1041; _LL1037: if(*(( void**) _temp1035) == Cyc_IdList_tok){
_LL1042: _temp1041=(( struct Cyc_IdList_tok_struct*) _temp1035)->f1; goto
_LL1038;} else{ goto _LL1039;} _LL1039: goto _LL1040; _LL1038: return _temp1041;
_LL1040:( int) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1036:;} static
unsigned char _temp1044[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1044, _temp1044, _temp1044 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1045= yy1;
struct _tuple2* _temp1051; _LL1047: if(*(( void**) _temp1045) == Cyc_ParamDecl_tok){
_LL1052: _temp1051=(( struct Cyc_ParamDecl_tok_struct*) _temp1045)->f1; goto
_LL1048;} else{ goto _LL1049;} _LL1049: goto _LL1050; _LL1048: return _temp1051;
_LL1050:( int) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1046:;} static
unsigned char _temp1054[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1054, _temp1054,
_temp1054 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1055= yy1; struct Cyc_List_List* _temp1061; _LL1057: if(*(( void**)
_temp1055) == Cyc_ParamDeclList_tok){ _LL1062: _temp1061=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1055)->f1; goto _LL1058;} else{ goto _LL1059;} _LL1059: goto _LL1060;
_LL1058: return _temp1061; _LL1060:( int) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1056:;} static unsigned char _temp1064[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1064, _temp1064, _temp1064 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1065= yy1; struct _tuple17* _temp1071; _LL1067: if(*((
void**) _temp1065) == Cyc_ParamDeclListBool_tok){ _LL1072: _temp1071=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1065)->f1; goto _LL1068;} else{ goto
_LL1069;} _LL1069: goto _LL1070; _LL1068: return _temp1071; _LL1070:( int)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1066:;} static
unsigned char _temp1074[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1074, _temp1074, _temp1074 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1075=
yy1; struct Cyc_List_List* _temp1081; _LL1077: if(*(( void**) _temp1075) == Cyc_TypeList_tok){
_LL1082: _temp1081=(( struct Cyc_TypeList_tok_struct*) _temp1075)->f1; goto
_LL1078;} else{ goto _LL1079;} _LL1079: goto _LL1080; _LL1078: return _temp1081;
_LL1080:( int) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1076:;} static
unsigned char _temp1084[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1084, _temp1084,
_temp1084 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1085= yy1; struct Cyc_List_List* _temp1091; _LL1087: if(*(( void**)
_temp1085) == Cyc_DesignatorList_tok){ _LL1092: _temp1091=(( struct Cyc_DesignatorList_tok_struct*)
_temp1085)->f1; goto _LL1088;} else{ goto _LL1089;} _LL1089: goto _LL1090;
_LL1088: return _temp1091; _LL1090:( int) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1086:;} static unsigned char _temp1094[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1094,
_temp1094, _temp1094 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1095= yy1; void* _temp1101; _LL1097: if(*(( void**) _temp1095) == Cyc_Designator_tok){
_LL1102: _temp1101=( void*)(( struct Cyc_Designator_tok_struct*) _temp1095)->f1;
goto _LL1098;} else{ goto _LL1099;} _LL1099: goto _LL1100; _LL1098: return
_temp1101; _LL1100:( int) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1096:;}
static unsigned char _temp1104[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1104, _temp1104, _temp1104 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1105= yy1; void* _temp1111;
_LL1107: if(*(( void**) _temp1105) == Cyc_Kind_tok){ _LL1112: _temp1111=( void*)((
struct Cyc_Kind_tok_struct*) _temp1105)->f1; goto _LL1108;} else{ goto _LL1109;}
_LL1109: goto _LL1110; _LL1108: return _temp1111; _LL1110:( int) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1106:;} static unsigned char _temp1114[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1114, _temp1114, _temp1114 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1115= yy1; void* _temp1121; _LL1117: if(*(( void**) _temp1115) == Cyc_Type_tok){
_LL1122: _temp1121=( void*)(( struct Cyc_Type_tok_struct*) _temp1115)->f1; goto
_LL1118;} else{ goto _LL1119;} _LL1119: goto _LL1120; _LL1118: return _temp1121;
_LL1120:( int) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1116:;} static
unsigned char _temp1124[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1124, _temp1124,
_temp1124 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1125= yy1; struct Cyc_List_List* _temp1131; _LL1127: if(*(( void**)
_temp1125) == Cyc_AttributeList_tok){ _LL1132: _temp1131=(( struct Cyc_AttributeList_tok_struct*)
_temp1125)->f1; goto _LL1128;} else{ goto _LL1129;} _LL1129: goto _LL1130;
_LL1128: return _temp1131; _LL1130:( int) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1126:;} static unsigned char _temp1134[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1134,
_temp1134, _temp1134 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1135= yy1; void* _temp1141; _LL1137: if(*(( void**) _temp1135) == Cyc_Attribute_tok){
_LL1142: _temp1141=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1135)->f1;
goto _LL1138;} else{ goto _LL1139;} _LL1139: goto _LL1140; _LL1138: return
_temp1141; _LL1140:( int) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1136:;}
static unsigned char _temp1144[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1144, _temp1144, _temp1144 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1145= yy1; struct Cyc_Absyn_Enumfield* _temp1151; _LL1147: if(*(( void**)
_temp1145) == Cyc_Enumfield_tok){ _LL1152: _temp1151=(( struct Cyc_Enumfield_tok_struct*)
_temp1145)->f1; goto _LL1148;} else{ goto _LL1149;} _LL1149: goto _LL1150;
_LL1148: return _temp1151; _LL1150:( int) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1146:;} static unsigned char _temp1154[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1154, _temp1154, _temp1154 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1155= yy1; struct Cyc_List_List* _temp1161; _LL1157: if(*((
void**) _temp1155) == Cyc_EnumfieldList_tok){ _LL1162: _temp1161=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1155)->f1; goto _LL1158;} else{ goto _LL1159;} _LL1159: goto _LL1160;
_LL1158: return _temp1161; _LL1160:( int) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1156:;} static unsigned char _temp1164[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1164, _temp1164, _temp1164 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1165= yy1;
struct Cyc_Core_Opt* _temp1171; _LL1167: if(*(( void**) _temp1165) == Cyc_TypeOpt_tok){
_LL1172: _temp1171=(( struct Cyc_TypeOpt_tok_struct*) _temp1165)->f1; goto
_LL1168;} else{ goto _LL1169;} _LL1169: goto _LL1170; _LL1168: return _temp1171;
_LL1170:( int) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1166:;} static
unsigned char _temp1174[ 13u]="Rgnorder_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgnorder_tok={ Cyc_Core_Failure,{ _temp1174, _temp1174, _temp1174 +
13u}}; struct Cyc_List_List* Cyc_yyget_Rgnorder_tok( void* yy1){ void* _temp1175=
yy1; struct Cyc_List_List* _temp1181; _LL1177: if(*(( void**) _temp1175) == Cyc_Rgnorder_tok){
_LL1182: _temp1181=(( struct Cyc_Rgnorder_tok_struct*) _temp1175)->f1; goto
_LL1178;} else{ goto _LL1179;} _LL1179: goto _LL1180; _LL1178: return _temp1181;
_LL1180:( int) _throw(( void*)& Cyc_yyfail_Rgnorder_tok); _LL1176:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1183;
_temp1183.timestamp= 0; _temp1183.first_line= 0; _temp1183.first_column= 0;
_temp1183.last_line= 0; _temp1183.last_column= 0; _temp1183;});} struct Cyc_Yyltype
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
86, 87, 88, 89, 90, 91}; static unsigned char _temp1184[ 2u]="$"; static
unsigned char _temp1185[ 6u]="error"; static unsigned char _temp1186[ 12u]="$undefined.";
static unsigned char _temp1187[ 5u]="AUTO"; static unsigned char _temp1188[ 9u]="REGISTER";
static unsigned char _temp1189[ 7u]="STATIC"; static unsigned char _temp1190[ 7u]="EXTERN";
static unsigned char _temp1191[ 8u]="TYPEDEF"; static unsigned char _temp1192[ 5u]="VOID";
static unsigned char _temp1193[ 5u]="CHAR"; static unsigned char _temp1194[ 6u]="SHORT";
static unsigned char _temp1195[ 4u]="INT"; static unsigned char _temp1196[ 5u]="LONG";
static unsigned char _temp1197[ 6u]="FLOAT"; static unsigned char _temp1198[ 7u]="DOUBLE";
static unsigned char _temp1199[ 7u]="SIGNED"; static unsigned char _temp1200[ 9u]="UNSIGNED";
static unsigned char _temp1201[ 6u]="CONST"; static unsigned char _temp1202[ 9u]="VOLATILE";
static unsigned char _temp1203[ 9u]="RESTRICT"; static unsigned char _temp1204[
7u]="STRUCT"; static unsigned char _temp1205[ 6u]="UNION"; static unsigned char
_temp1206[ 5u]="CASE"; static unsigned char _temp1207[ 8u]="DEFAULT"; static
unsigned char _temp1208[ 7u]="INLINE"; static unsigned char _temp1209[ 7u]="SIZEOF";
static unsigned char _temp1210[ 9u]="OFFSETOF"; static unsigned char _temp1211[
3u]="IF"; static unsigned char _temp1212[ 5u]="ELSE"; static unsigned char
_temp1213[ 7u]="SWITCH"; static unsigned char _temp1214[ 6u]="WHILE"; static
unsigned char _temp1215[ 3u]="DO"; static unsigned char _temp1216[ 4u]="FOR";
static unsigned char _temp1217[ 5u]="GOTO"; static unsigned char _temp1218[ 9u]="CONTINUE";
static unsigned char _temp1219[ 6u]="BREAK"; static unsigned char _temp1220[ 7u]="RETURN";
static unsigned char _temp1221[ 5u]="ENUM"; static unsigned char _temp1222[ 8u]="NULL_kw";
static unsigned char _temp1223[ 4u]="LET"; static unsigned char _temp1224[ 6u]="THROW";
static unsigned char _temp1225[ 4u]="TRY"; static unsigned char _temp1226[ 6u]="CATCH";
static unsigned char _temp1227[ 4u]="NEW"; static unsigned char _temp1228[ 9u]="ABSTRACT";
static unsigned char _temp1229[ 9u]="FALLTHRU"; static unsigned char _temp1230[
6u]="USING"; static unsigned char _temp1231[ 10u]="NAMESPACE"; static
unsigned char _temp1232[ 7u]="TUNION"; static unsigned char _temp1233[ 8u]="XTUNION";
static unsigned char _temp1234[ 5u]="FILL"; static unsigned char _temp1235[ 8u]="CODEGEN";
static unsigned char _temp1236[ 4u]="CUT"; static unsigned char _temp1237[ 7u]="SPLICE";
static unsigned char _temp1238[ 7u]="MALLOC"; static unsigned char _temp1239[ 9u]="REGION_T";
static unsigned char _temp1240[ 7u]="REGION"; static unsigned char _temp1241[ 5u]="RNEW";
static unsigned char _temp1242[ 8u]="RMALLOC"; static unsigned char _temp1243[ 7u]="PTR_OP";
static unsigned char _temp1244[ 7u]="INC_OP"; static unsigned char _temp1245[ 7u]="DEC_OP";
static unsigned char _temp1246[ 8u]="LEFT_OP"; static unsigned char _temp1247[ 9u]="RIGHT_OP";
static unsigned char _temp1248[ 6u]="LE_OP"; static unsigned char _temp1249[ 6u]="GE_OP";
static unsigned char _temp1250[ 6u]="EQ_OP"; static unsigned char _temp1251[ 6u]="NE_OP";
static unsigned char _temp1252[ 7u]="AND_OP"; static unsigned char _temp1253[ 6u]="OR_OP";
static unsigned char _temp1254[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1255[ 11u]="DIV_ASSIGN"; static unsigned char _temp1256[ 11u]="MOD_ASSIGN";
static unsigned char _temp1257[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1258[ 11u]="SUB_ASSIGN"; static unsigned char _temp1259[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1260[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1261[ 11u]="AND_ASSIGN"; static unsigned char _temp1262[ 11u]="XOR_ASSIGN";
static unsigned char _temp1263[ 10u]="OR_ASSIGN"; static unsigned char _temp1264[
9u]="ELLIPSIS"; static unsigned char _temp1265[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1266[ 12u]="COLON_COLON"; static unsigned char _temp1267[ 11u]="IDENTIFIER";
static unsigned char _temp1268[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1269[ 7u]="STRING"; static unsigned char _temp1270[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1271[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1272[ 9u]="TYPE_VAR"; static unsigned char _temp1273[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1274[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1275[ 10u]="ATTRIBUTE"; static unsigned char _temp1276[ 4u]="';'"; static
unsigned char _temp1277[ 4u]="'{'"; static unsigned char _temp1278[ 4u]="'}'";
static unsigned char _temp1279[ 4u]="'='"; static unsigned char _temp1280[ 4u]="'('";
static unsigned char _temp1281[ 4u]="')'"; static unsigned char _temp1282[ 4u]="','";
static unsigned char _temp1283[ 4u]="'_'"; static unsigned char _temp1284[ 4u]="'$'";
static unsigned char _temp1285[ 4u]="'<'"; static unsigned char _temp1286[ 4u]="'>'";
static unsigned char _temp1287[ 4u]="':'"; static unsigned char _temp1288[ 4u]="'.'";
static unsigned char _temp1289[ 4u]="'['"; static unsigned char _temp1290[ 4u]="']'";
static unsigned char _temp1291[ 4u]="'*'"; static unsigned char _temp1292[ 4u]="'@'";
static unsigned char _temp1293[ 4u]="'?'"; static unsigned char _temp1294[ 4u]="'+'";
static unsigned char _temp1295[ 4u]="'-'"; static unsigned char _temp1296[ 4u]="'&'";
static unsigned char _temp1297[ 4u]="'|'"; static unsigned char _temp1298[ 4u]="'^'";
static unsigned char _temp1299[ 4u]="'/'"; static unsigned char _temp1300[ 4u]="'%'";
static unsigned char _temp1301[ 4u]="'~'"; static unsigned char _temp1302[ 4u]="'!'";
static unsigned char _temp1303[ 5u]="prog"; static unsigned char _temp1304[ 17u]="translation_unit";
static unsigned char _temp1305[ 21u]="external_declaration"; static
unsigned char _temp1306[ 20u]="function_definition"; static unsigned char
_temp1307[ 21u]="function_definition2"; static unsigned char _temp1308[ 13u]="using_action";
static unsigned char _temp1309[ 15u]="unusing_action"; static unsigned char
_temp1310[ 17u]="namespace_action"; static unsigned char _temp1311[ 19u]="unnamespace_action";
static unsigned char _temp1312[ 12u]="declaration"; static unsigned char
_temp1313[ 17u]="declaration_list"; static unsigned char _temp1314[ 23u]="declaration_specifiers";
static unsigned char _temp1315[ 24u]="storage_class_specifier"; static
unsigned char _temp1316[ 15u]="attributes_opt"; static unsigned char _temp1317[
11u]="attributes"; static unsigned char _temp1318[ 15u]="attribute_list"; static
unsigned char _temp1319[ 10u]="attribute"; static unsigned char _temp1320[ 15u]="type_specifier";
static unsigned char _temp1321[ 5u]="kind"; static unsigned char _temp1322[ 15u]="type_qualifier";
static unsigned char _temp1323[ 15u]="enum_specifier"; static unsigned char
_temp1324[ 11u]="enum_field"; static unsigned char _temp1325[ 22u]="enum_declaration_list";
static unsigned char _temp1326[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1327[ 16u]="type_params_opt"; static unsigned char _temp1328[
16u]="struct_or_union"; static unsigned char _temp1329[ 24u]="struct_declaration_list";
static unsigned char _temp1330[ 25u]="struct_declaration_list0"; static
unsigned char _temp1331[ 21u]="init_declarator_list"; static unsigned char
_temp1332[ 22u]="init_declarator_list0"; static unsigned char _temp1333[ 16u]="init_declarator";
static unsigned char _temp1334[ 19u]="struct_declaration"; static unsigned char
_temp1335[ 25u]="specifier_qualifier_list"; static unsigned char _temp1336[ 23u]="struct_declarator_list";
static unsigned char _temp1337[ 24u]="struct_declarator_list0"; static
unsigned char _temp1338[ 18u]="struct_declarator"; static unsigned char
_temp1339[ 17u]="tunion_specifier"; static unsigned char _temp1340[ 18u]="tunion_or_xtunion";
static unsigned char _temp1341[ 17u]="tunionfield_list"; static unsigned char
_temp1342[ 18u]="tunionfield_scope"; static unsigned char _temp1343[ 12u]="tunionfield";
static unsigned char _temp1344[ 11u]="declarator"; static unsigned char
_temp1345[ 18u]="direct_declarator"; static unsigned char _temp1346[ 8u]="pointer";
static unsigned char _temp1347[ 13u]="pointer_char"; static unsigned char
_temp1348[ 8u]="rgn_opt"; static unsigned char _temp1349[ 4u]="rgn"; static
unsigned char _temp1350[ 20u]="type_qualifier_list"; static unsigned char
_temp1351[ 20u]="parameter_type_list"; static unsigned char _temp1352[ 9u]="type_var";
static unsigned char _temp1353[ 16u]="optional_effect"; static unsigned char
_temp1354[ 19u]="optional_rgn_order"; static unsigned char _temp1355[ 10u]="rgn_order";
static unsigned char _temp1356[ 16u]="optional_inject"; static unsigned char
_temp1357[ 11u]="effect_set"; static unsigned char _temp1358[ 14u]="atomic_effect";
static unsigned char _temp1359[ 11u]="region_set"; static unsigned char
_temp1360[ 15u]="parameter_list"; static unsigned char _temp1361[ 22u]="parameter_declaration";
static unsigned char _temp1362[ 16u]="identifier_list"; static unsigned char
_temp1363[ 17u]="identifier_list0"; static unsigned char _temp1364[ 12u]="initializer";
static unsigned char _temp1365[ 18u]="array_initializer"; static unsigned char
_temp1366[ 17u]="initializer_list"; static unsigned char _temp1367[ 12u]="designation";
static unsigned char _temp1368[ 16u]="designator_list"; static unsigned char
_temp1369[ 11u]="designator"; static unsigned char _temp1370[ 10u]="type_name";
static unsigned char _temp1371[ 14u]="any_type_name"; static unsigned char
_temp1372[ 15u]="type_name_list"; static unsigned char _temp1373[ 20u]="abstract_declarator";
static unsigned char _temp1374[ 27u]="direct_abstract_declarator"; static
unsigned char _temp1375[ 10u]="statement"; static unsigned char _temp1376[ 18u]="labeled_statement";
static unsigned char _temp1377[ 21u]="expression_statement"; static
unsigned char _temp1378[ 19u]="compound_statement"; static unsigned char
_temp1379[ 16u]="block_item_list"; static unsigned char _temp1380[ 20u]="selection_statement";
static unsigned char _temp1381[ 15u]="switch_clauses"; static unsigned char
_temp1382[ 16u]="switchC_clauses"; static unsigned char _temp1383[ 20u]="iteration_statement";
static unsigned char _temp1384[ 15u]="jump_statement"; static unsigned char
_temp1385[ 8u]="pattern"; static unsigned char _temp1386[ 19u]="tuple_pattern_list";
static unsigned char _temp1387[ 20u]="tuple_pattern_list0"; static unsigned char
_temp1388[ 14u]="field_pattern"; static unsigned char _temp1389[ 19u]="field_pattern_list";
static unsigned char _temp1390[ 20u]="field_pattern_list0"; static unsigned char
_temp1391[ 11u]="expression"; static unsigned char _temp1392[ 22u]="assignment_expression";
static unsigned char _temp1393[ 20u]="assignment_operator"; static unsigned char
_temp1394[ 23u]="conditional_expression"; static unsigned char _temp1395[ 20u]="constant_expression";
static unsigned char _temp1396[ 22u]="logical_or_expression"; static
unsigned char _temp1397[ 23u]="logical_and_expression"; static unsigned char
_temp1398[ 24u]="inclusive_or_expression"; static unsigned char _temp1399[ 24u]="exclusive_or_expression";
static unsigned char _temp1400[ 15u]="and_expression"; static unsigned char
_temp1401[ 20u]="equality_expression"; static unsigned char _temp1402[ 22u]="relational_expression";
static unsigned char _temp1403[ 17u]="shift_expression"; static unsigned char
_temp1404[ 20u]="additive_expression"; static unsigned char _temp1405[ 26u]="multiplicative_expression";
static unsigned char _temp1406[ 16u]="cast_expression"; static unsigned char
_temp1407[ 17u]="unary_expression"; static unsigned char _temp1408[ 15u]="unary_operator";
static unsigned char _temp1409[ 19u]="postfix_expression"; static unsigned char
_temp1410[ 19u]="primary_expression"; static unsigned char _temp1411[ 25u]="argument_expression_list";
static unsigned char _temp1412[ 26u]="argument_expression_list0"; static
unsigned char _temp1413[ 9u]="constant"; static unsigned char _temp1414[ 20u]="qual_opt_identifier";
static struct _tagged_arr Cyc_yytname[ 231u]={{ _temp1184, _temp1184, _temp1184
+ 2u},{ _temp1185, _temp1185, _temp1185 + 6u},{ _temp1186, _temp1186, _temp1186
+ 12u},{ _temp1187, _temp1187, _temp1187 + 5u},{ _temp1188, _temp1188, _temp1188
+ 9u},{ _temp1189, _temp1189, _temp1189 + 7u},{ _temp1190, _temp1190, _temp1190
+ 7u},{ _temp1191, _temp1191, _temp1191 + 8u},{ _temp1192, _temp1192, _temp1192
+ 5u},{ _temp1193, _temp1193, _temp1193 + 5u},{ _temp1194, _temp1194, _temp1194
+ 6u},{ _temp1195, _temp1195, _temp1195 + 4u},{ _temp1196, _temp1196, _temp1196
+ 5u},{ _temp1197, _temp1197, _temp1197 + 6u},{ _temp1198, _temp1198, _temp1198
+ 7u},{ _temp1199, _temp1199, _temp1199 + 7u},{ _temp1200, _temp1200, _temp1200
+ 9u},{ _temp1201, _temp1201, _temp1201 + 6u},{ _temp1202, _temp1202, _temp1202
+ 9u},{ _temp1203, _temp1203, _temp1203 + 9u},{ _temp1204, _temp1204, _temp1204
+ 7u},{ _temp1205, _temp1205, _temp1205 + 6u},{ _temp1206, _temp1206, _temp1206
+ 5u},{ _temp1207, _temp1207, _temp1207 + 8u},{ _temp1208, _temp1208, _temp1208
+ 7u},{ _temp1209, _temp1209, _temp1209 + 7u},{ _temp1210, _temp1210, _temp1210
+ 9u},{ _temp1211, _temp1211, _temp1211 + 3u},{ _temp1212, _temp1212, _temp1212
+ 5u},{ _temp1213, _temp1213, _temp1213 + 7u},{ _temp1214, _temp1214, _temp1214
+ 6u},{ _temp1215, _temp1215, _temp1215 + 3u},{ _temp1216, _temp1216, _temp1216
+ 4u},{ _temp1217, _temp1217, _temp1217 + 5u},{ _temp1218, _temp1218, _temp1218
+ 9u},{ _temp1219, _temp1219, _temp1219 + 6u},{ _temp1220, _temp1220, _temp1220
+ 7u},{ _temp1221, _temp1221, _temp1221 + 5u},{ _temp1222, _temp1222, _temp1222
+ 8u},{ _temp1223, _temp1223, _temp1223 + 4u},{ _temp1224, _temp1224, _temp1224
+ 6u},{ _temp1225, _temp1225, _temp1225 + 4u},{ _temp1226, _temp1226, _temp1226
+ 6u},{ _temp1227, _temp1227, _temp1227 + 4u},{ _temp1228, _temp1228, _temp1228
+ 9u},{ _temp1229, _temp1229, _temp1229 + 9u},{ _temp1230, _temp1230, _temp1230
+ 6u},{ _temp1231, _temp1231, _temp1231 + 10u},{ _temp1232, _temp1232, _temp1232
+ 7u},{ _temp1233, _temp1233, _temp1233 + 8u},{ _temp1234, _temp1234, _temp1234
+ 5u},{ _temp1235, _temp1235, _temp1235 + 8u},{ _temp1236, _temp1236, _temp1236
+ 4u},{ _temp1237, _temp1237, _temp1237 + 7u},{ _temp1238, _temp1238, _temp1238
+ 7u},{ _temp1239, _temp1239, _temp1239 + 9u},{ _temp1240, _temp1240, _temp1240
+ 7u},{ _temp1241, _temp1241, _temp1241 + 5u},{ _temp1242, _temp1242, _temp1242
+ 8u},{ _temp1243, _temp1243, _temp1243 + 7u},{ _temp1244, _temp1244, _temp1244
+ 7u},{ _temp1245, _temp1245, _temp1245 + 7u},{ _temp1246, _temp1246, _temp1246
+ 8u},{ _temp1247, _temp1247, _temp1247 + 9u},{ _temp1248, _temp1248, _temp1248
+ 6u},{ _temp1249, _temp1249, _temp1249 + 6u},{ _temp1250, _temp1250, _temp1250
+ 6u},{ _temp1251, _temp1251, _temp1251 + 6u},{ _temp1252, _temp1252, _temp1252
+ 7u},{ _temp1253, _temp1253, _temp1253 + 6u},{ _temp1254, _temp1254, _temp1254
+ 11u},{ _temp1255, _temp1255, _temp1255 + 11u},{ _temp1256, _temp1256,
_temp1256 + 11u},{ _temp1257, _temp1257, _temp1257 + 11u},{ _temp1258, _temp1258,
_temp1258 + 11u},{ _temp1259, _temp1259, _temp1259 + 12u},{ _temp1260, _temp1260,
_temp1260 + 13u},{ _temp1261, _temp1261, _temp1261 + 11u},{ _temp1262, _temp1262,
_temp1262 + 11u},{ _temp1263, _temp1263, _temp1263 + 10u},{ _temp1264, _temp1264,
_temp1264 + 9u},{ _temp1265, _temp1265, _temp1265 + 11u},{ _temp1266, _temp1266,
_temp1266 + 12u},{ _temp1267, _temp1267, _temp1267 + 11u},{ _temp1268, _temp1268,
_temp1268 + 17u},{ _temp1269, _temp1269, _temp1269 + 7u},{ _temp1270, _temp1270,
_temp1270 + 19u},{ _temp1271, _temp1271, _temp1271 + 18u},{ _temp1272, _temp1272,
_temp1272 + 9u},{ _temp1273, _temp1273, _temp1273 + 16u},{ _temp1274, _temp1274,
_temp1274 + 18u},{ _temp1275, _temp1275, _temp1275 + 10u},{ _temp1276, _temp1276,
_temp1276 + 4u},{ _temp1277, _temp1277, _temp1277 + 4u},{ _temp1278, _temp1278,
_temp1278 + 4u},{ _temp1279, _temp1279, _temp1279 + 4u},{ _temp1280, _temp1280,
_temp1280 + 4u},{ _temp1281, _temp1281, _temp1281 + 4u},{ _temp1282, _temp1282,
_temp1282 + 4u},{ _temp1283, _temp1283, _temp1283 + 4u},{ _temp1284, _temp1284,
_temp1284 + 4u},{ _temp1285, _temp1285, _temp1285 + 4u},{ _temp1286, _temp1286,
_temp1286 + 4u},{ _temp1287, _temp1287, _temp1287 + 4u},{ _temp1288, _temp1288,
_temp1288 + 4u},{ _temp1289, _temp1289, _temp1289 + 4u},{ _temp1290, _temp1290,
_temp1290 + 4u},{ _temp1291, _temp1291, _temp1291 + 4u},{ _temp1292, _temp1292,
_temp1292 + 4u},{ _temp1293, _temp1293, _temp1293 + 4u},{ _temp1294, _temp1294,
_temp1294 + 4u},{ _temp1295, _temp1295, _temp1295 + 4u},{ _temp1296, _temp1296,
_temp1296 + 4u},{ _temp1297, _temp1297, _temp1297 + 4u},{ _temp1298, _temp1298,
_temp1298 + 4u},{ _temp1299, _temp1299, _temp1299 + 4u},{ _temp1300, _temp1300,
_temp1300 + 4u},{ _temp1301, _temp1301, _temp1301 + 4u},{ _temp1302, _temp1302,
_temp1302 + 4u},{ _temp1303, _temp1303, _temp1303 + 5u},{ _temp1304, _temp1304,
_temp1304 + 17u},{ _temp1305, _temp1305, _temp1305 + 21u},{ _temp1306, _temp1306,
_temp1306 + 20u},{ _temp1307, _temp1307, _temp1307 + 21u},{ _temp1308, _temp1308,
_temp1308 + 13u},{ _temp1309, _temp1309, _temp1309 + 15u},{ _temp1310, _temp1310,
_temp1310 + 17u},{ _temp1311, _temp1311, _temp1311 + 19u},{ _temp1312, _temp1312,
_temp1312 + 12u},{ _temp1313, _temp1313, _temp1313 + 17u},{ _temp1314, _temp1314,
_temp1314 + 23u},{ _temp1315, _temp1315, _temp1315 + 24u},{ _temp1316, _temp1316,
_temp1316 + 15u},{ _temp1317, _temp1317, _temp1317 + 11u},{ _temp1318, _temp1318,
_temp1318 + 15u},{ _temp1319, _temp1319, _temp1319 + 10u},{ _temp1320, _temp1320,
_temp1320 + 15u},{ _temp1321, _temp1321, _temp1321 + 5u},{ _temp1322, _temp1322,
_temp1322 + 15u},{ _temp1323, _temp1323, _temp1323 + 15u},{ _temp1324, _temp1324,
_temp1324 + 11u},{ _temp1325, _temp1325, _temp1325 + 22u},{ _temp1326, _temp1326,
_temp1326 + 26u},{ _temp1327, _temp1327, _temp1327 + 16u},{ _temp1328, _temp1328,
_temp1328 + 16u},{ _temp1329, _temp1329, _temp1329 + 24u},{ _temp1330, _temp1330,
_temp1330 + 25u},{ _temp1331, _temp1331, _temp1331 + 21u},{ _temp1332, _temp1332,
_temp1332 + 22u},{ _temp1333, _temp1333, _temp1333 + 16u},{ _temp1334, _temp1334,
_temp1334 + 19u},{ _temp1335, _temp1335, _temp1335 + 25u},{ _temp1336, _temp1336,
_temp1336 + 23u},{ _temp1337, _temp1337, _temp1337 + 24u},{ _temp1338, _temp1338,
_temp1338 + 18u},{ _temp1339, _temp1339, _temp1339 + 17u},{ _temp1340, _temp1340,
_temp1340 + 18u},{ _temp1341, _temp1341, _temp1341 + 17u},{ _temp1342, _temp1342,
_temp1342 + 18u},{ _temp1343, _temp1343, _temp1343 + 12u},{ _temp1344, _temp1344,
_temp1344 + 11u},{ _temp1345, _temp1345, _temp1345 + 18u},{ _temp1346, _temp1346,
_temp1346 + 8u},{ _temp1347, _temp1347, _temp1347 + 13u},{ _temp1348, _temp1348,
_temp1348 + 8u},{ _temp1349, _temp1349, _temp1349 + 4u},{ _temp1350, _temp1350,
_temp1350 + 20u},{ _temp1351, _temp1351, _temp1351 + 20u},{ _temp1352, _temp1352,
_temp1352 + 9u},{ _temp1353, _temp1353, _temp1353 + 16u},{ _temp1354, _temp1354,
_temp1354 + 19u},{ _temp1355, _temp1355, _temp1355 + 10u},{ _temp1356, _temp1356,
_temp1356 + 16u},{ _temp1357, _temp1357, _temp1357 + 11u},{ _temp1358, _temp1358,
_temp1358 + 14u},{ _temp1359, _temp1359, _temp1359 + 11u},{ _temp1360, _temp1360,
_temp1360 + 15u},{ _temp1361, _temp1361, _temp1361 + 22u},{ _temp1362, _temp1362,
_temp1362 + 16u},{ _temp1363, _temp1363, _temp1363 + 17u},{ _temp1364, _temp1364,
_temp1364 + 12u},{ _temp1365, _temp1365, _temp1365 + 18u},{ _temp1366, _temp1366,
_temp1366 + 17u},{ _temp1367, _temp1367, _temp1367 + 12u},{ _temp1368, _temp1368,
_temp1368 + 16u},{ _temp1369, _temp1369, _temp1369 + 11u},{ _temp1370, _temp1370,
_temp1370 + 10u},{ _temp1371, _temp1371, _temp1371 + 14u},{ _temp1372, _temp1372,
_temp1372 + 15u},{ _temp1373, _temp1373, _temp1373 + 20u},{ _temp1374, _temp1374,
_temp1374 + 27u},{ _temp1375, _temp1375, _temp1375 + 10u},{ _temp1376, _temp1376,
_temp1376 + 18u},{ _temp1377, _temp1377, _temp1377 + 21u},{ _temp1378, _temp1378,
_temp1378 + 19u},{ _temp1379, _temp1379, _temp1379 + 16u},{ _temp1380, _temp1380,
_temp1380 + 20u},{ _temp1381, _temp1381, _temp1381 + 15u},{ _temp1382, _temp1382,
_temp1382 + 16u},{ _temp1383, _temp1383, _temp1383 + 20u},{ _temp1384, _temp1384,
_temp1384 + 15u},{ _temp1385, _temp1385, _temp1385 + 8u},{ _temp1386, _temp1386,
_temp1386 + 19u},{ _temp1387, _temp1387, _temp1387 + 20u},{ _temp1388, _temp1388,
_temp1388 + 14u},{ _temp1389, _temp1389, _temp1389 + 19u},{ _temp1390, _temp1390,
_temp1390 + 20u},{ _temp1391, _temp1391, _temp1391 + 11u},{ _temp1392, _temp1392,
_temp1392 + 22u},{ _temp1393, _temp1393, _temp1393 + 20u},{ _temp1394, _temp1394,
_temp1394 + 23u},{ _temp1395, _temp1395, _temp1395 + 20u},{ _temp1396, _temp1396,
_temp1396 + 22u},{ _temp1397, _temp1397, _temp1397 + 23u},{ _temp1398, _temp1398,
_temp1398 + 24u},{ _temp1399, _temp1399, _temp1399 + 24u},{ _temp1400, _temp1400,
_temp1400 + 15u},{ _temp1401, _temp1401, _temp1401 + 20u},{ _temp1402, _temp1402,
_temp1402 + 22u},{ _temp1403, _temp1403, _temp1403 + 17u},{ _temp1404, _temp1404,
_temp1404 + 20u},{ _temp1405, _temp1405, _temp1405 + 26u},{ _temp1406, _temp1406,
_temp1406 + 16u},{ _temp1407, _temp1407, _temp1407 + 17u},{ _temp1408, _temp1408,
_temp1408 + 15u},{ _temp1409, _temp1409, _temp1409 + 19u},{ _temp1410, _temp1410,
_temp1410 + 19u},{ _temp1411, _temp1411, _temp1411 + 25u},{ _temp1412, _temp1412,
_temp1412 + 26u},{ _temp1413, _temp1413, _temp1413 + 9u},{ _temp1414, _temp1414,
_temp1414 + 20u}}; static short Cyc_yyr1[ 401u]={ 0, 119, 120, 120, 120, 120,
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
yyss=( short*)({ unsigned int _temp3211= 10000u; short* _temp3212=( short*)
GC_malloc_atomic( sizeof( short) * _temp3211);{ unsigned int _temp3213=
_temp3211; unsigned int i; for( i= 0; i < _temp3213; i ++){ _temp3212[ i]=(
short) 0;}}; _temp3212;}); int yyvsp_offset; void** yyvs=( void**)({
unsigned int _temp3208= 10000u; void** _temp3209=( void**) GC_malloc( sizeof(
void*) * _temp3208);{ unsigned int _temp3210= _temp3208; unsigned int i; for( i=
0; i < _temp3210; i ++){ _temp3209[ i]= Cyc_yylval;}}; _temp3209;}); int
yylsp_offset; struct Cyc_Yyltype* yyls=( struct Cyc_Yyltype*)({ unsigned int
_temp3205= 10000u; struct Cyc_Yyltype* _temp3206=( struct Cyc_Yyltype*)
GC_malloc_atomic( sizeof( struct Cyc_Yyltype) * _temp3205);{ unsigned int
_temp3207= _temp3205; unsigned int i; for( i= 0; i < _temp3207; i ++){ _temp3206[
i]= Cyc_yynewloc();}}; _temp3206;}); int yystacksize= 10000; void* yyval= Cyc_yylval;
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
yyn){ case 1: _LL1415: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); break; case 2: _LL1416:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1418=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1418[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1419; _temp1419.tag= Cyc_DeclList_tok; _temp1419.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1419;});
_temp1418;}); break; case 3: _LL1417: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1421=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1421[ 0]=({ struct Cyc_DeclList_tok_struct _temp1422; _temp1422.tag= Cyc_DeclList_tok;
_temp1422.f1=({ struct Cyc_List_List* _temp1423=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1423->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1424=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1424->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1425=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1425[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1426; _temp1426.tag= Cyc_Absyn_Using_d;
_temp1426.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1426.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1426;}); _temp1425;}));
_temp1424->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1424;});
_temp1423->tl= 0; _temp1423;}); _temp1422;}); _temp1421;}); Cyc_Lex_leave_using();
break; case 4: _LL1420: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1428=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1428[ 0]=({ struct Cyc_DeclList_tok_struct _temp1429; _temp1429.tag= Cyc_DeclList_tok;
_temp1429.f1=({ struct Cyc_List_List* _temp1430=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1430->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1431=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1431->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1432=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp1432[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp1433; _temp1433.tag= Cyc_Absyn_Using_d;
_temp1433.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp1433.f2= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1433;});
_temp1432;})); _temp1431->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1431;}); _temp1430->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1430;}); _temp1429;});
_temp1428;}); break; case 5: _LL1427: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1435=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1435[ 0]=({ struct Cyc_DeclList_tok_struct _temp1436; _temp1436.tag= Cyc_DeclList_tok;
_temp1436.f1=({ struct Cyc_List_List* _temp1437=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1437->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1438=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1438->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1439=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1439[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1440; _temp1440.tag=
Cyc_Absyn_Namespace_d; _temp1440.f1=({ struct _tagged_arr* _temp1441=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp1441[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1441;});
_temp1440.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1440;}); _temp1439;})); _temp1438->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1438;}); _temp1437->tl= 0; _temp1437;}); _temp1436;}); _temp1435;}); Cyc_Lex_leave_namespace();
break; case 6: _LL1434: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1443=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1443[ 0]=({ struct Cyc_DeclList_tok_struct _temp1444; _temp1444.tag= Cyc_DeclList_tok;
_temp1444.f1=({ struct Cyc_List_List* _temp1445=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1445->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1446=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1446->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1447=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1447[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1448; _temp1448.tag=
Cyc_Absyn_Namespace_d; _temp1448.f1=({ struct _tagged_arr* _temp1449=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp1449[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1449;});
_temp1448.f2= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1448;}); _temp1447;})); _temp1446->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1446;}); _temp1445->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1445;}); _temp1444;});
_temp1443;}); break; case 7: _LL1442: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));} yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1451=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1451[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1452; _temp1452.tag= Cyc_DeclList_tok; _temp1452.f1=({ struct Cyc_List_List*
_temp1453=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1453->hd=( void*)({ struct Cyc_Absyn_Decl* _temp1454=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1454->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp1455=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp1455[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp1456; _temp1456.tag= Cyc_Absyn_ExternC_d;
_temp1456.f1= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp1456;}); _temp1455;})); _temp1454->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp1454;}); _temp1453->tl= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1453;}); _temp1452;});
_temp1451;}); break; case 8: _LL1450: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1458=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1458[ 0]=({ struct Cyc_DeclList_tok_struct _temp1459; _temp1459.tag= Cyc_DeclList_tok;
_temp1459.f1= 0; _temp1459;}); _temp1458;}); break; case 9: _LL1457: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp1461=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1461[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1462; _temp1462.tag= Cyc_DeclList_tok; _temp1462.f1=({ struct Cyc_List_List*
_temp1463=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1463->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1464=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1464[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1465; _temp1465.tag= Cyc_Absyn_Fn_d;
_temp1465.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1465;}); _temp1464;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1463->tl=
0; _temp1463;}); _temp1462;}); _temp1461;}); break; case 10: _LL1460: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 11:
_LL1466: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp1468=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp1468[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp1469; _temp1469.tag= Cyc_FnDecl_tok; _temp1469.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1469;});
_temp1468;}); break; case 12: _LL1467: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1471=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1471[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1472; _temp1472.tag= Cyc_FnDecl_tok;
_temp1472.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1473=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1473->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1473;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1472;}); _temp1471;}); break; case 13: _LL1470: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1475=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1475[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1476; _temp1476.tag= Cyc_FnDecl_tok;
_temp1476.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1476;}); _temp1475;}); break; case 14: _LL1474: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1478=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1478[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1479; _temp1479.tag= Cyc_FnDecl_tok;
_temp1479.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1480=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1480->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1480;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1479;}); _temp1478;}); break; case 15: _LL1477: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1482=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1482[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1483; _temp1483.tag= Cyc_FnDecl_tok;
_temp1483.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1484=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1484->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1484;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0, Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1483;}); _temp1482;}); break; case 16: _LL1481: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp1486=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp1486[ 0]=({ struct Cyc_FnDecl_tok_struct _temp1487; _temp1487.tag= Cyc_FnDecl_tok;
_temp1487.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp1488=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1488->v=( void*)
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp1488;}), Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1487;}); _temp1486;}); break; case 17: _LL1485: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 18: _LL1489:
Cyc_Lex_leave_using(); break; case 19: _LL1490: Cyc_Lex_enter_namespace(({
struct _tagged_arr* _temp1492=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp1492[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1492;})); yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 20:
_LL1491: Cyc_Lex_leave_namespace(); break; case 21: _LL1493: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp1495=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1495[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1496; _temp1496.tag= Cyc_DeclList_tok; _temp1496.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp1496;}); _temp1495;}); break; case 22: _LL1494: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1498=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1498[ 0]=({ struct Cyc_DeclList_tok_struct _temp1499; _temp1499.tag= Cyc_DeclList_tok;
_temp1499.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_InitDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1499;}); _temp1498;}); break; case 23: _LL1497: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1501=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1501[ 0]=({ struct Cyc_DeclList_tok_struct _temp1502; _temp1502.tag= Cyc_DeclList_tok;
_temp1502.f1=({ struct Cyc_List_List* _temp1503=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1503->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), 0, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1503->tl=
0; _temp1503;}); _temp1502;}); _temp1501;}); break; case 24: _LL1500: { struct
Cyc_List_List* _temp1505= 0;{ struct Cyc_List_List* _temp1506= Cyc_yyget_IdList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); for( 0;
_temp1506 != 0; _temp1506=(( struct Cyc_List_List*) _check_null( _temp1506))->tl){
struct _tagged_arr* _temp1507=( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp1506))->hd; struct _tuple1* _temp1508=({ struct _tuple1*
_temp1511=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1511->f1=
Cyc_Absyn_rel_ns_null; _temp1511->f2= _temp1507; _temp1511;}); struct Cyc_Absyn_Vardecl*
_temp1509= Cyc_Absyn_new_vardecl( _temp1508, Cyc_Absyn_wildtyp( 0), 0);
_temp1505=({ struct Cyc_List_List* _temp1510=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1510->hd=( void*) _temp1509; _temp1510->tl=
_temp1505; _temp1510;});}} _temp1505=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1505); yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp1512=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp1512[ 0]=({ struct Cyc_DeclList_tok_struct _temp1513; _temp1513.tag= Cyc_DeclList_tok;
_temp1513.f1=({ struct Cyc_List_List* _temp1514=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1514->hd=( void*) Cyc_Absyn_letv_decl(
_temp1505, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1514->tl= 0; _temp1514;}); _temp1513;});
_temp1512;}); break;} case 25: _LL1504: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 26: _LL1515:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp1517=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp1517[ 0]=({ struct Cyc_DeclList_tok_struct
_temp1518; _temp1518.tag= Cyc_DeclList_tok; _temp1518.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_DeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1518;});
_temp1517;}); break; case 27: _LL1516: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1520=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1520[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1521; _temp1521.tag= Cyc_DeclSpec_tok;
_temp1521.f1=({ struct Cyc_Parse_Declaration_spec* _temp1522=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1522->sc=({ struct
Cyc_Core_Opt* _temp1523=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp1523->v=( void*) Cyc_yyget_StorageClass_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1523;});
_temp1522->tq= Cyc_Absyn_empty_tqual(); _temp1522->type_specs= 0; _temp1522->is_inline=
0; _temp1522->attributes= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1522;}); _temp1521;});
_temp1520;}); break; case 28: _LL1519: if(( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc != 0){ Cyc_Parse_warn(
_tag_arr("Only one storage class is allowed in a declaration", sizeof(
unsigned char), 51u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} yyval=(
void*)({ struct Cyc_DeclSpec_tok_struct* _temp1525=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1525[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1526; _temp1526.tag= Cyc_DeclSpec_tok; _temp1526.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1527=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1527->sc=({ struct Cyc_Core_Opt* _temp1528=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1528->v=( void*) Cyc_yyget_StorageClass_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1528;});
_temp1527->tq=( Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]))->tq; _temp1527->type_specs=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs;
_temp1527->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1527->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1527;}); _temp1526;}); _temp1525;}); break; case 29: _LL1524: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1530=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1530[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1531; _temp1531.tag= Cyc_DeclSpec_tok; _temp1531.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1532=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1532->sc= 0; _temp1532->tq= Cyc_Absyn_empty_tqual(); _temp1532->type_specs=({
struct Cyc_List_List* _temp1533=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1533->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1533->tl= 0;
_temp1533;}); _temp1532->is_inline= 0; _temp1532->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1532;});
_temp1531;}); _temp1530;}); break; case 30: _LL1529: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1535=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1535[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1536; _temp1536.tag= Cyc_DeclSpec_tok;
_temp1536.f1=({ struct Cyc_Parse_Declaration_spec* _temp1537=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1537->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1537->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1537->type_specs=({ struct Cyc_List_List* _temp1538=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1538->hd=(
void*) Cyc_yyget_TypeSpecifier_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1538->tl=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1538;});
_temp1537->is_inline=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline; _temp1537->attributes=
Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1537;}); _temp1536;}); _temp1535;}); break; case 31: _LL1534: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1540=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1540[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1541; _temp1541.tag= Cyc_DeclSpec_tok; _temp1541.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1542=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1542->sc= 0; _temp1542->tq= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1542->type_specs=
0; _temp1542->is_inline= 0; _temp1542->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1542;});
_temp1541;}); _temp1540;}); break; case 32: _LL1539: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1544=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1544[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1545; _temp1545.tag= Cyc_DeclSpec_tok;
_temp1545.f1=({ struct Cyc_Parse_Declaration_spec* _temp1546=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1546->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1546->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tq); _temp1546->type_specs=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->type_specs; _temp1546->is_inline=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->is_inline;
_temp1546->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1546;}); _temp1545;}); _temp1544;}); break; case 33: _LL1543: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp1548=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp1548[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp1549; _temp1549.tag= Cyc_DeclSpec_tok; _temp1549.f1=({ struct Cyc_Parse_Declaration_spec*
_temp1550=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp1550->sc= 0; _temp1550->tq= Cyc_Absyn_empty_tqual(); _temp1550->type_specs=
0; _temp1550->is_inline= 1; _temp1550->attributes= Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1550;});
_temp1549;}); _temp1548;}); break; case 34: _LL1547: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp1552=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp1552[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp1553; _temp1553.tag= Cyc_DeclSpec_tok;
_temp1553.f1=({ struct Cyc_Parse_Declaration_spec* _temp1554=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp1554->sc=( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->sc; _temp1554->tq=(
Cyc_yyget_DeclSpec_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]))->tq; _temp1554->type_specs=( Cyc_yyget_DeclSpec_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->type_specs; _temp1554->is_inline=
1; _temp1554->attributes= Cyc_List_imp_append( Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_DeclSpec_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->attributes);
_temp1554;}); _temp1553;}); _temp1552;}); break; case 35: _LL1551: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp1556=( struct Cyc_StorageClass_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1556[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1557; _temp1557.tag= Cyc_StorageClass_tok;
_temp1557.f1= Cyc_Parse_Auto_sc; _temp1557;}); _temp1556;}); break; case 36:
_LL1555: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1559=( struct
Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1559[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1560; _temp1560.tag=
Cyc_StorageClass_tok; _temp1560.f1= Cyc_Parse_Register_sc; _temp1560;});
_temp1559;}); break; case 37: _LL1558: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1562=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1562[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1563; _temp1563.tag= Cyc_StorageClass_tok; _temp1563.f1= Cyc_Parse_Static_sc;
_temp1563;}); _temp1562;}); break; case 38: _LL1561: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1565=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1565[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1566; _temp1566.tag= Cyc_StorageClass_tok; _temp1566.f1= Cyc_Parse_Extern_sc;
_temp1566;}); _temp1565;}); break; case 39: _LL1564: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only extern or extern \"C\" is allowed",
sizeof( unsigned char), 37u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_StorageClass_tok_struct* _temp1568=( struct Cyc_StorageClass_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct)); _temp1568[ 0]=({
struct Cyc_StorageClass_tok_struct _temp1569; _temp1569.tag= Cyc_StorageClass_tok;
_temp1569.f1= Cyc_Parse_ExternC_sc; _temp1569;}); _temp1568;}); break; case 40:
_LL1567: yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp1571=( struct
Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_StorageClass_tok_struct));
_temp1571[ 0]=({ struct Cyc_StorageClass_tok_struct _temp1572; _temp1572.tag=
Cyc_StorageClass_tok; _temp1572.f1= Cyc_Parse_Typedef_sc; _temp1572;});
_temp1571;}); break; case 41: _LL1570: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp1574=( struct Cyc_StorageClass_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StorageClass_tok_struct)); _temp1574[ 0]=({ struct Cyc_StorageClass_tok_struct
_temp1575; _temp1575.tag= Cyc_StorageClass_tok; _temp1575.f1= Cyc_Parse_Abstract_sc;
_temp1575;}); _temp1574;}); break; case 42: _LL1573: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1577=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1577[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1578; _temp1578.tag=
Cyc_AttributeList_tok; _temp1578.f1= 0; _temp1578;}); _temp1577;}); break; case
43: _LL1576: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 44: _LL1579: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1581=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1581[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1582; _temp1582.tag=
Cyc_AttributeList_tok; _temp1582.f1= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1582;});
_temp1581;}); break; case 45: _LL1580: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1584=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1584[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1585; _temp1585.tag=
Cyc_AttributeList_tok; _temp1585.f1=({ struct Cyc_List_List* _temp1586=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1586->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1586->tl= 0; _temp1586;}); _temp1585;}); _temp1584;});
break; case 46: _LL1583: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp1588=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp1588[ 0]=({ struct Cyc_AttributeList_tok_struct _temp1589; _temp1589.tag=
Cyc_AttributeList_tok; _temp1589.f1=({ struct Cyc_List_List* _temp1590=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1590->hd=( void*)
Cyc_yyget_Attribute_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1590->tl= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1590;}); _temp1589;});
_temp1588;}); break; case 47: _LL1587: { struct _tagged_arr _temp1592= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if((((
_get_arr_size( _temp1592, sizeof( unsigned char)) > 4?*(( const unsigned char*)
_check_unknown_subscript( _temp1592, sizeof( unsigned char), 0)) =='_': 0)?*((
const unsigned char*) _check_unknown_subscript( _temp1592, sizeof( unsigned char),
1)) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1592,
sizeof( unsigned char),( int)( _get_arr_size( _temp1592, sizeof( unsigned char))
- 2))) =='_': 0)?*(( const unsigned char*) _check_unknown_subscript( _temp1592,
sizeof( unsigned char),( int)( _get_arr_size( _temp1592, sizeof( unsigned char))
- 3))) =='_': 0){ _temp1592=( struct _tagged_arr) Cyc_String_substring(
_temp1592, 2,( int)( _get_arr_size( _temp1592, sizeof( unsigned char)) - 5));}{
int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp( _temp1592,( Cyc_Parse_att_map[
_check_known_subscript_notnull( 16u, i)]).f1) == 0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1593=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1593[ 0]=({ struct Cyc_Attribute_tok_struct _temp1594; _temp1594.tag= Cyc_Attribute_tok;
_temp1594.f1=( void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp1594;}); _temp1593;}); break;}} if( i == 16u){ Cyc_Parse_err( _tag_arr("unrecognized attribute",
sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Attribute_tok_struct* _temp1595=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1595[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1596; _temp1596.tag= Cyc_Attribute_tok; _temp1596.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp1596;}); _temp1595;});} break;}} case
48: _LL1591: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp1598=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1598[ 0]=({ struct Cyc_Attribute_tok_struct _temp1599; _temp1599.tag= Cyc_Attribute_tok;
_temp1599.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp1599;}); _temp1598;});
break; case 49: _LL1597: { struct _tagged_arr _temp1601= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct
_tuple16 _temp1604; int _temp1605; struct _tuple16* _temp1602= Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1604=*
_temp1602; _LL1606: _temp1605= _temp1604.f2; goto _LL1603; _LL1603: { void* a;
if( Cyc_String_zstrcmp( _temp1601, _tag_arr("regparm", sizeof( unsigned char), 8u))
== 0? 1: Cyc_String_zstrcmp( _temp1601, _tag_arr("__regparm__", sizeof(
unsigned char), 12u)) == 0){ if( _temp1605 <= 0? 1: _temp1605 > 3){ Cyc_Parse_err(
_tag_arr("regparm requires value between 1 and 3", sizeof( unsigned char), 39u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp1607=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp1607[ 0]=({ struct Cyc_Absyn_Regparm_att_struct
_temp1608; _temp1608.tag= Cyc_Absyn_Regparm_att; _temp1608.f1= _temp1605;
_temp1608;}); _temp1607;});} else{ if( Cyc_String_zstrcmp( _temp1601, _tag_arr("aligned",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1601, _tag_arr("__aligned__",
sizeof( unsigned char), 12u)) == 0){ if( _temp1605 < 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
unsigned int j=( unsigned int) _temp1605; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err( _tag_arr("aligned requires positive power of two",
sizeof( unsigned char), 39u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));} a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp1609=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp1609[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp1610; _temp1610.tag= Cyc_Absyn_Aligned_att;
_temp1610.f1= _temp1605; _temp1610;}); _temp1609;});}} else{ Cyc_Parse_err(
_tag_arr("unrecognized attribute", sizeof( unsigned char), 23u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line)); a=(
void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1611=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1611[ 0]=({ struct Cyc_Attribute_tok_struct _temp1612; _temp1612.tag= Cyc_Attribute_tok;
_temp1612.f1=( void*) a; _temp1612;}); _temp1611;}); break;}} case 50: _LL1600: {
struct _tagged_arr _temp1614= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); struct _tagged_arr
_temp1615= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); void* a; if( Cyc_String_zstrcmp( _temp1614, _tag_arr("section",
sizeof( unsigned char), 8u)) == 0? 1: Cyc_String_zstrcmp( _temp1614, _tag_arr("__section__",
sizeof( unsigned char), 12u)) == 0){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp1616=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp1616[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp1617; _temp1617.tag=
Cyc_Absyn_Section_att; _temp1617.f1= _temp1615; _temp1617;}); _temp1616;});}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp1618=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp1618[ 0]=({ struct
Cyc_Attribute_tok_struct _temp1619; _temp1619.tag= Cyc_Attribute_tok; _temp1619.f1=(
void*) a; _temp1619;}); _temp1618;}); break;} case 51: _LL1613: { struct
_tagged_arr _temp1621= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 7)]); struct _tagged_arr
_temp1622= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); int _temp1625; struct _tuple16 _temp1623=* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _LL1626:
_temp1625= _temp1623.f2; goto _LL1624; _LL1624: { int _temp1629; struct _tuple16
_temp1627=* Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _LL1630: _temp1629= _temp1627.f2; goto _LL1628; _LL1628: {
void* a=( void*) Cyc_Absyn_Cdecl_att; if( Cyc_String_zstrcmp( _temp1621,
_tag_arr("format", sizeof( unsigned char), 7u)) == 0? 1: Cyc_String_zstrcmp(
_temp1621, _tag_arr("__format__", sizeof( unsigned char), 11u)) == 0){ if( Cyc_String_zstrcmp(
_temp1622, _tag_arr("printf", sizeof( unsigned char), 7u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1631=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1631[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1632; _temp1632.tag= Cyc_Absyn_Format_att;
_temp1632.f1=( void*)(( void*) Cyc_Absyn_Printf_ft); _temp1632.f2= _temp1625;
_temp1632.f3= _temp1629; _temp1632;}); _temp1631;});} else{ if( Cyc_String_zstrcmp(
_temp1622, _tag_arr("scanf", sizeof( unsigned char), 6u)) == 0){ a=( void*)({
struct Cyc_Absyn_Format_att_struct* _temp1633=( struct Cyc_Absyn_Format_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Format_att_struct)); _temp1633[ 0]=({ struct
Cyc_Absyn_Format_att_struct _temp1634; _temp1634.tag= Cyc_Absyn_Format_att;
_temp1634.f1=( void*)(( void*) Cyc_Absyn_Scanf_ft); _temp1634.f2= _temp1625;
_temp1634.f3= _temp1629; _temp1634;}); _temp1633;});} else{ Cyc_Parse_err(
_tag_arr("unrecognized format type", sizeof( unsigned char), 25u), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line));}}}
else{ Cyc_Parse_err( _tag_arr("unrecognized attribute", sizeof( unsigned char),
23u), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp1635=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp1635[ 0]=({ struct Cyc_Attribute_tok_struct _temp1636; _temp1636.tag= Cyc_Attribute_tok;
_temp1636.f1=( void*) a; _temp1636;}); _temp1635;}); break;}}} case 52: _LL1620:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1638=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1638[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1639; _temp1639.tag= Cyc_TypeSpecifier_tok;
_temp1639.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1639;});
_temp1638;}); break; case 53: _LL1637: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1641=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1641[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1642; _temp1642.tag=
Cyc_TypeSpecifier_tok; _temp1642.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(
0, 0), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1642;}); _temp1641;}); break; case 54: _LL1640:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1644=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1644[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1645; _temp1645.tag= Cyc_TypeSpecifier_tok;
_temp1645.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1645;});
_temp1644;}); break; case 55: _LL1643: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1647=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1647[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1648; _temp1648.tag=
Cyc_TypeSpecifier_tok; _temp1648.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp1649=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp1649[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp1650; _temp1650.tag=
Cyc_Parse_Short_spec; _temp1650.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1650;});
_temp1649;})); _temp1648;}); _temp1647;}); break; case 56: _LL1646: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1652=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1652[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1653; _temp1653.tag= Cyc_TypeSpecifier_tok;
_temp1653.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1653;});
_temp1652;}); break; case 57: _LL1651: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1655=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1655[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1656; _temp1656.tag=
Cyc_TypeSpecifier_tok; _temp1656.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp1657=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp1657[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp1658; _temp1658.tag= Cyc_Parse_Long_spec;
_temp1658.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp1658;}); _temp1657;})); _temp1656;}); _temp1655;});
break; case 58: _LL1654: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1660=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1660[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1661; _temp1661.tag=
Cyc_TypeSpecifier_tok; _temp1661.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1661;}); _temp1660;}); break; case 59: _LL1659:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1663=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1663[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1664; _temp1664.tag= Cyc_TypeSpecifier_tok;
_temp1664.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1664;});
_temp1663;}); break; case 60: _LL1662: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1666=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1666[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1667; _temp1667.tag=
Cyc_TypeSpecifier_tok; _temp1667.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp1668=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp1668[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp1669; _temp1669.tag=
Cyc_Parse_Signed_spec; _temp1669.f1= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1669;});
_temp1668;})); _temp1667;}); _temp1666;}); break; case 61: _LL1665: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1671=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1671[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1672; _temp1672.tag= Cyc_TypeSpecifier_tok;
_temp1672.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp1673=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp1673[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp1674; _temp1674.tag= Cyc_Parse_Unsigned_spec; _temp1674.f1= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1674;});
_temp1673;})); _temp1672;}); _temp1671;}); break; case 62: _LL1670: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 63: _LL1675:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
64: _LL1676: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 65: _LL1677: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1679=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1679[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1680; _temp1680.tag=
Cyc_TypeSpecifier_tok; _temp1680.f1=( void*) Cyc_Parse_type_spec( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1680;});
_temp1679;}); break; case 66: _LL1678: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1682=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1682[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1683; _temp1683.tag=
Cyc_TypeSpecifier_tok; _temp1683.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp1684=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp1684[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp1685; _temp1685.tag= Cyc_Absyn_TypedefType;
_temp1685.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp1685.f2= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1685.f3= 0;
_temp1685;}); _temp1684;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1683;});
_temp1682;}); break; case 67: _LL1681: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1687=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1687[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1688; _temp1688.tag=
Cyc_TypeSpecifier_tok; _temp1688.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp1689=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1689[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1690; _temp1690.tag= Cyc_Absyn_TupleType;
_temp1690.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); _temp1690;});
_temp1689;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1688;}); _temp1687;}); break; case 68:
_LL1686: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1692=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1692[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1693; _temp1693.tag=
Cyc_TypeSpecifier_tok; _temp1693.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp1694=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1694[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1695; _temp1695.tag= Cyc_Absyn_RgnHandleType;
_temp1695.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1695;}); _temp1694;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1693;}); _temp1692;}); break; case 69: _LL1691: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp1697=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp1697[ 0]=({ struct Cyc_Kind_tok_struct _temp1698; _temp1698.tag= Cyc_Kind_tok;
_temp1698.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1698;});
_temp1697;}); break; case 70: _LL1696: { struct _tagged_arr* _temp1702; void*
_temp1704; struct _tuple1 _temp1700=* Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _LL1705: _temp1704=
_temp1700.f1; goto _LL1703; _LL1703: _temp1702= _temp1700.f2; goto _LL1701;
_LL1701: if( _temp1704 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err( _tag_arr("bad kind in type specifier",
sizeof( unsigned char), 27u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Kind_tok_struct* _temp1706=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp1706[ 0]=({ struct Cyc_Kind_tok_struct
_temp1707; _temp1707.tag= Cyc_Kind_tok; _temp1707.f1=( void*) Cyc_Parse_id_to_kind(*
_temp1702, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1707;}); _temp1706;}); break;} case 71:
_LL1699: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp1709=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1709[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1710; _temp1710.tag= Cyc_TypeQual_tok;
_temp1710.f1=({ struct Cyc_Absyn_Tqual _temp1711; _temp1711.q_const= 1;
_temp1711.q_volatile= 0; _temp1711.q_restrict= 0; _temp1711;}); _temp1710;});
_temp1709;}); break; case 72: _LL1708: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp1713=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp1713[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp1714; _temp1714.tag= Cyc_TypeQual_tok; _temp1714.f1=({ struct Cyc_Absyn_Tqual
_temp1715; _temp1715.q_const= 0; _temp1715.q_volatile= 1; _temp1715.q_restrict=
0; _temp1715;}); _temp1714;}); _temp1713;}); break; case 73: _LL1712: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp1717=( struct Cyc_TypeQual_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_TypeQual_tok_struct)); _temp1717[ 0]=({
struct Cyc_TypeQual_tok_struct _temp1718; _temp1718.tag= Cyc_TypeQual_tok;
_temp1718.f1=({ struct Cyc_Absyn_Tqual _temp1719; _temp1719.q_const= 0;
_temp1719.q_volatile= 0; _temp1719.q_restrict= 1; _temp1719;}); _temp1718;});
_temp1717;}); break; case 74: _LL1716: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1721=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1721[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1722; _temp1722.tag=
Cyc_TypeSpecifier_tok; _temp1722.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp1723=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1723[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1724; _temp1724.tag= Cyc_Parse_Decl_spec;
_temp1724.f1=({ struct Cyc_Absyn_Decl* _temp1725=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1725->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp1726=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp1726[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp1727; _temp1727.tag= Cyc_Absyn_Enum_d; _temp1727.f1=({ struct Cyc_Absyn_Enumdecl*
_temp1728=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp1728->sc=( void*)(( void*) Cyc_Absyn_Public); _temp1728->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1728->fields=({
struct Cyc_Core_Opt* _temp1729=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1729->v=( void*) Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1729;});
_temp1728;}); _temp1727;}); _temp1726;})); _temp1725->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp1725;}); _temp1724;}); _temp1723;})); _temp1722;}); _temp1721;}); break;
case 75: _LL1720: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1731=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1731[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1732; _temp1732.tag=
Cyc_TypeSpecifier_tok; _temp1732.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp1733=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp1733[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp1734; _temp1734.tag= Cyc_Absyn_EnumType;
_temp1734.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1734.f2= 0; _temp1734;}); _temp1733;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1732;}); _temp1731;}); break; case 76: _LL1730: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1736=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1736[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1737; _temp1737.tag= Cyc_Enumfield_tok;
_temp1737.f1=({ struct Cyc_Absyn_Enumfield* _temp1738=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1738->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1738->tag= 0;
_temp1738->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1738;});
_temp1737;}); _temp1736;}); break; case 77: _LL1735: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp1740=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp1740[ 0]=({ struct Cyc_Enumfield_tok_struct _temp1741; _temp1741.tag= Cyc_Enumfield_tok;
_temp1741.f1=({ struct Cyc_Absyn_Enumfield* _temp1742=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp1742->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1742->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1742->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1742;});
_temp1741;}); _temp1740;}); break; case 78: _LL1739: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1744=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1744[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1745; _temp1745.tag=
Cyc_EnumfieldList_tok; _temp1745.f1=({ struct Cyc_List_List* _temp1746=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1746->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1746->tl= 0; _temp1746;}); _temp1745;}); _temp1744;});
break; case 79: _LL1743: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp1748=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp1748[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp1749; _temp1749.tag=
Cyc_EnumfieldList_tok; _temp1749.f1=({ struct Cyc_List_List* _temp1750=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1750->hd=( void*)
Cyc_yyget_Enumfield_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1750->tl= Cyc_yyget_EnumfieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1750;}); _temp1749;});
_temp1748;}); break; case 80: _LL1747: { void* t;{ enum  Cyc_Parse_Struct_or_union
_temp1752= Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _LL1754: if( _temp1752 == Cyc_Parse_Struct_su){
goto _LL1755;} else{ goto _LL1756;} _LL1756: if( _temp1752 == Cyc_Parse_Union_su){
goto _LL1757;} else{ goto _LL1753;} _LL1755: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp1758=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp1758[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp1759; _temp1759.tag= Cyc_Absyn_AnonStructType; _temp1759.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1759;});
_temp1758;}); goto _LL1753; _LL1757: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp1760=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp1760[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp1761; _temp1761.tag= Cyc_Absyn_AnonUnionType; _temp1761.f1= Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1761;});
_temp1760;}); goto _LL1753; _LL1753:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1762=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1762[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1763; _temp1763.tag=
Cyc_TypeSpecifier_tok; _temp1763.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp1764=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp1764[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp1765; _temp1765.tag= Cyc_Parse_Type_spec;
_temp1765.f1=( void*) t; _temp1765.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp1765;});
_temp1764;})); _temp1763;}); _temp1762;}); break;} case 81: _LL1751: { struct
Cyc_List_List* _temp1767=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ enum  Cyc_Parse_Struct_or_union
_temp1768= Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _LL1770: if( _temp1768 == Cyc_Parse_Struct_su){
goto _LL1771;} else{ goto _LL1772;} _LL1772: if( _temp1768 == Cyc_Parse_Union_su){
goto _LL1773;} else{ goto _LL1769;} _LL1771: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1774=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1774->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1774;}),
_temp1767,({ struct Cyc_Core_Opt* _temp1775=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1775->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1775;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1769; _LL1773: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1776=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1776->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1776;}),
_temp1767,({ struct Cyc_Core_Opt* _temp1777=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1777->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1777;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1769; _LL1769:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1778=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1778[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1779; _temp1779.tag= Cyc_TypeSpecifier_tok;
_temp1779.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1780=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1780[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1781; _temp1781.tag= Cyc_Parse_Decl_spec;
_temp1781.f1= d; _temp1781;}); _temp1780;})); _temp1779;}); _temp1778;}); break;}
case 82: _LL1766: { struct Cyc_List_List* _temp1783=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); struct Cyc_Absyn_Decl* d;{ enum  Cyc_Parse_Struct_or_union
_temp1784= Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _LL1786: if( _temp1784 == Cyc_Parse_Struct_su){
goto _LL1787;} else{ goto _LL1788;} _LL1788: if( _temp1784 == Cyc_Parse_Union_su){
goto _LL1789;} else{ goto _LL1785;} _LL1787: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1790=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1790->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1790;}),
_temp1783,({ struct Cyc_Core_Opt* _temp1791=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1791->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1791;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1785; _LL1789: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp1792=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1792->v=( void*) Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1792;}),
_temp1783,({ struct Cyc_Core_Opt* _temp1793=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1793->v=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1793;}),
0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); goto _LL1785; _LL1785:;} yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1794=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1794[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1795; _temp1795.tag= Cyc_TypeSpecifier_tok;
_temp1795.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1796=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1796[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1797; _temp1797.tag= Cyc_Parse_Decl_spec;
_temp1797.f1= d; _temp1797;}); _temp1796;})); _temp1795;}); _temp1794;}); break;}
case 83: _LL1782:{ enum  Cyc_Parse_Struct_or_union _temp1799= Cyc_yyget_StructOrUnion_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _LL1801: if(
_temp1799 == Cyc_Parse_Struct_su){ goto _LL1802;} else{ goto _LL1803;} _LL1803:
if( _temp1799 == Cyc_Parse_Union_su){ goto _LL1804;} else{ goto _LL1800;}
_LL1802: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1805=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1805[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1806; _temp1806.tag=
Cyc_TypeSpecifier_tok; _temp1806.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1807=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1807[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1808; _temp1808.tag= Cyc_Absyn_StructType;
_temp1808.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1808.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1808.f3= 0;
_temp1808;}); _temp1807;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1806;});
_temp1805;}); goto _LL1800; _LL1804: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1809=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1809[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1810; _temp1810.tag=
Cyc_TypeSpecifier_tok; _temp1810.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1811=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1811[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1812; _temp1812.tag= Cyc_Absyn_UnionType;
_temp1812.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1812.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1812.f3= 0;
_temp1812;}); _temp1811;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1810;});
_temp1809;}); goto _LL1800; _LL1800:;} break; case 84: _LL1798:{ enum  Cyc_Parse_Struct_or_union
_temp1814= Cyc_yyget_StructOrUnion_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _LL1816: if( _temp1814 == Cyc_Parse_Struct_su){
goto _LL1817;} else{ goto _LL1818;} _LL1818: if( _temp1814 == Cyc_Parse_Union_su){
goto _LL1819;} else{ goto _LL1815;} _LL1817: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1820=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1820[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1821; _temp1821.tag=
Cyc_TypeSpecifier_tok; _temp1821.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp1822=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1822[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1823; _temp1823.tag= Cyc_Absyn_StructType;
_temp1823.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1823.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1823.f3= 0;
_temp1823;}); _temp1822;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1821;});
_temp1820;}); goto _LL1815; _LL1819: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp1824=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1824[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1825; _temp1825.tag=
Cyc_TypeSpecifier_tok; _temp1825.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp1826=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp1826[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp1827; _temp1827.tag= Cyc_Absyn_UnionType;
_temp1827.f1=( struct _tuple1*) Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1827.f2= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1827.f3= 0;
_temp1827;}); _temp1826;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp1825;});
_temp1824;}); goto _LL1815; _LL1815:;} break; case 85: _LL1813: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp1829=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp1829[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1830; _temp1830.tag= Cyc_TypeList_tok; _temp1830.f1= 0; _temp1830;});
_temp1829;}); break; case 86: _LL1828: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp1832=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp1832[ 0]=({ struct Cyc_TypeList_tok_struct _temp1833; _temp1833.tag= Cyc_TypeList_tok;
_temp1833.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1833;});
_temp1832;}); break; case 87: _LL1831:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1; yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp1835=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp1835[ 0]=({ struct Cyc_TypeList_tok_struct
_temp1836; _temp1836.tag= Cyc_TypeList_tok; _temp1836.f1= Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp1836;});
_temp1835;}); break; case 88: _LL1834: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1838=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1838[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1839; _temp1839.tag= Cyc_StructOrUnion_tok; _temp1839.f1= Cyc_Parse_Struct_su;
_temp1839;}); _temp1838;}); break; case 89: _LL1837: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp1841=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc_atomic( sizeof(
struct Cyc_StructOrUnion_tok_struct)); _temp1841[ 0]=({ struct Cyc_StructOrUnion_tok_struct
_temp1842; _temp1842.tag= Cyc_StructOrUnion_tok; _temp1842.f1= Cyc_Parse_Union_su;
_temp1842;}); _temp1841;}); break; case 90: _LL1840: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp1844=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp1844[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp1845; _temp1845.tag= Cyc_StructFieldDeclList_tok; _temp1845.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp1845;});
_temp1844;}); break; case 91: _LL1843: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp1847=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp1847[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp1848; _temp1848.tag= Cyc_StructFieldDeclListList_tok; _temp1848.f1=({
struct Cyc_List_List* _temp1849=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1849->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1849->tl= 0;
_temp1849;}); _temp1848;}); _temp1847;}); break; case 92: _LL1846: yyval=( void*)({
struct Cyc_StructFieldDeclListList_tok_struct* _temp1851=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp1851[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp1852; _temp1852.tag= Cyc_StructFieldDeclListList_tok;
_temp1852.f1=({ struct Cyc_List_List* _temp1853=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1853->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1853->tl=
Cyc_yyget_StructFieldDeclListList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp1853;}); _temp1852;}); _temp1851;}); break;
case 93: _LL1850: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp1855=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1855[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1856; _temp1856.tag=
Cyc_InitDeclList_tok; _temp1856.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp1856;});
_temp1855;}); break; case 94: _LL1854: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1858=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1858[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1859; _temp1859.tag=
Cyc_InitDeclList_tok; _temp1859.f1=({ struct Cyc_List_List* _temp1860=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1860->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1860->tl= 0; _temp1860;}); _temp1859;}); _temp1858;});
break; case 95: _LL1857: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp1862=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp1862[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp1863; _temp1863.tag=
Cyc_InitDeclList_tok; _temp1863.f1=({ struct Cyc_List_List* _temp1864=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1864->hd=( void*)
Cyc_yyget_InitDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1864->tl= Cyc_yyget_InitDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1864;});
_temp1863;}); _temp1862;}); break; case 96: _LL1861: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp1866=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp1866[ 0]=({ struct Cyc_InitDecl_tok_struct _temp1867; _temp1867.tag= Cyc_InitDecl_tok;
_temp1867.f1=({ struct _tuple15* _temp1868=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp1868->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1868->f2= 0;
_temp1868;}); _temp1867;}); _temp1866;}); break; case 97: _LL1865: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp1870=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp1870[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp1871; _temp1871.tag= Cyc_InitDecl_tok; _temp1871.f1=({ struct _tuple15*
_temp1872=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1872->f1=
Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1872->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1872;});
_temp1871;}); _temp1870;}); break; case 98: _LL1869: { struct _tuple18 _temp1876;
struct Cyc_List_List* _temp1877; struct Cyc_List_List* _temp1879; struct Cyc_Absyn_Tqual
_temp1881; struct _tuple18* _temp1874= Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1876=*
_temp1874; _LL1882: _temp1881= _temp1876.f1; goto _LL1880; _LL1880: _temp1879=
_temp1876.f2; goto _LL1878; _LL1878: _temp1877= _temp1876.f3; goto _LL1875;
_LL1875: { struct Cyc_List_List* _temp1885; struct Cyc_List_List* _temp1887;
struct _tuple0 _temp1883=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])); _LL1888: _temp1887= _temp1883.f1; goto _LL1886; _LL1886:
_temp1885= _temp1883.f2; goto _LL1884; _LL1884: { void* _temp1889= Cyc_Parse_speclist2typ(
_temp1879, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).last_line)); struct Cyc_List_List* _temp1890=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
Cyc_Parse_apply_tmss( _temp1881, _temp1889, _temp1887, _temp1877), _temp1885);
yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp1891=( struct
Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp1891[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct _temp1892; _temp1892.tag=
Cyc_StructFieldDeclList_tok; _temp1892.f1=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple9*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), _temp1890); _temp1892;}); _temp1891;}); break;}}}
case 99: _LL1873: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1894=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1894[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1895; _temp1895.tag=
Cyc_QualSpecList_tok; _temp1895.f1=({ struct _tuple18* _temp1896=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1896->f1= Cyc_Absyn_empty_tqual();
_temp1896->f2=({ struct Cyc_List_List* _temp1897=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1897->hd=( void*) Cyc_yyget_TypeSpecifier_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1897->tl=
0; _temp1897;}); _temp1896->f3= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1896;}); _temp1895;});
_temp1894;}); break; case 100: _LL1893: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp1899=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1899[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1900; _temp1900.tag=
Cyc_QualSpecList_tok; _temp1900.f1=({ struct _tuple18* _temp1901=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1901->f1=(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp1901->f2=({
struct Cyc_List_List* _temp1902=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1902->hd=( void*) Cyc_yyget_TypeSpecifier_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1902->tl=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1902;}); _temp1901->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1901;});
_temp1900;}); _temp1899;}); break; case 101: _LL1898: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp1904=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp1904[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp1905; _temp1905.tag= Cyc_QualSpecList_tok;
_temp1905.f1=({ struct _tuple18* _temp1906=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp1906->f1= Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1906->f2= 0;
_temp1906->f3= Cyc_yyget_AttributeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp1906;}); _temp1905;}); _temp1904;}); break; case
102: _LL1903: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp1908=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp1908[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp1909; _temp1909.tag=
Cyc_QualSpecList_tok; _temp1909.f1=({ struct _tuple18* _temp1910=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp1910->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]),(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1); _temp1910->f2=(*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])).f2; _temp1910->f3= Cyc_List_append( Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),(* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3); _temp1910;});
_temp1909;}); _temp1908;}); break; case 103: _LL1907: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp1912=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1912[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1913; _temp1913.tag= Cyc_DeclaratorExpoptList_tok;
_temp1913.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp1913;}); _temp1912;}); break; case 104: _LL1911: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp1915=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1915[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1916; _temp1916.tag= Cyc_DeclaratorExpoptList_tok;
_temp1916.f1=({ struct Cyc_List_List* _temp1917=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1917->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1917->tl= 0;
_temp1917;}); _temp1916;}); _temp1915;}); break; case 105: _LL1914: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp1919=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp1919[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp1920; _temp1920.tag= Cyc_DeclaratorExpoptList_tok;
_temp1920.f1=({ struct Cyc_List_List* _temp1921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1921->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1921->tl=
Cyc_yyget_DeclaratorExpoptList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1921;}); _temp1920;}); _temp1919;}); break; case 106:
_LL1918: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp1923=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp1923[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp1924; _temp1924.tag=
Cyc_DeclaratorExpopt_tok; _temp1924.f1=({ struct _tuple13* _temp1925=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1925->f1= Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1925->f2= 0;
_temp1925;}); _temp1924;}); _temp1923;}); break; case 107: _LL1922: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1927=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1927[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1928; _temp1928.tag= Cyc_DeclaratorExpopt_tok;
_temp1928.f1=({ struct _tuple13* _temp1929=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1929->f1=({ struct Cyc_Parse_Declarator*
_temp1931=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp1931->id=({ struct _tuple1* _temp1932=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp1932->f1= Cyc_Absyn_rel_ns_null; _temp1932->f2=
_init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"",
sizeof( unsigned char), 1u); _temp1932;}); _temp1931->tms= 0; _temp1931;});
_temp1929->f2=({ struct Cyc_Core_Opt* _temp1930=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1930->v=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1930;});
_temp1929;}); _temp1928;}); _temp1927;}); break; case 108: _LL1926: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp1934=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp1934[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp1935; _temp1935.tag= Cyc_DeclaratorExpopt_tok;
_temp1935.f1=({ struct _tuple13* _temp1936=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp1936->f1= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1936->f2=({
struct Cyc_Core_Opt* _temp1937=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1937->v=( void*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1937;}); _temp1936;});
_temp1935;}); _temp1934;}); break; case 109: _LL1933: { struct Cyc_List_List*
_temp1939=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp1940=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1940[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1941; _temp1941.tag= Cyc_TypeSpecifier_tok;
_temp1941.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp1942=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp1942[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp1943; _temp1943.tag= Cyc_Parse_Decl_spec;
_temp1943.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), _temp1939,({
struct Cyc_Core_Opt* _temp1944=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1944->v=( void*) Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1944;}), Cyc_yyget_Bool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1943;}); _temp1942;})); _temp1941;}); _temp1940;}); break;} case 110:
_LL1938: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1946=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1946[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1947; _temp1947.tag=
Cyc_TypeSpecifier_tok; _temp1947.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp1948=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1948[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1949; _temp1949.tag= Cyc_Absyn_TunionType;
_temp1949.f1=({ struct Cyc_Absyn_TunionInfo _temp1950; _temp1950.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp1951=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp1951[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp1952; _temp1952.tag= Cyc_Absyn_UnknownTunion;
_temp1952.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp1953; _temp1953.name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp1953.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1953;});
_temp1952;}); _temp1951;})); _temp1950.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1950.rgn=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp1950;}); _temp1949;}); _temp1948;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp1947;}); _temp1946;}); break; case 111: _LL1945: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp1955=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp1955[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp1956; _temp1956.tag= Cyc_TypeSpecifier_tok;
_temp1956.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1957=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1957[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1958; _temp1958.tag=
Cyc_Absyn_TunionType; _temp1958.f1=({ struct Cyc_Absyn_TunionInfo _temp1959;
_temp1959.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp1960=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp1960[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp1961; _temp1961.tag= Cyc_Absyn_UnknownTunion; _temp1961.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp1962; _temp1962.name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1962.is_xtunion=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp1962;}); _temp1961;}); _temp1960;})); _temp1959.targs= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1959.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp1959;}); _temp1958;}); _temp1957;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp1956;}); _temp1955;}); break; case 112:
_LL1954: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp1964=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp1964[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp1965; _temp1965.tag=
Cyc_TypeSpecifier_tok; _temp1965.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1966=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1966[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1967; _temp1967.tag= Cyc_Absyn_TunionFieldType;
_temp1967.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1968; _temp1968.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp1969=( struct
Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp1969[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp1970; _temp1970.tag=
Cyc_Absyn_UnknownTunionfield; _temp1970.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp1971; _temp1971.tunion_name= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp1971.field_name=
Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]); _temp1971.is_xtunion= Cyc_yyget_Bool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp1971;});
_temp1970;}); _temp1969;})); _temp1968.targs= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1968;}); _temp1967;});
_temp1966;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp1965;}); _temp1964;}); break; case 113:
_LL1963: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp1973=( struct Cyc_Bool_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct)); _temp1973[ 0]=({ struct
Cyc_Bool_tok_struct _temp1974; _temp1974.tag= Cyc_Bool_tok; _temp1974.f1= 0;
_temp1974;}); _temp1973;}); break; case 114: _LL1972: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp1976=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp1976[ 0]=({ struct Cyc_Bool_tok_struct
_temp1977; _temp1977.tag= Cyc_Bool_tok; _temp1977.f1= 1; _temp1977;}); _temp1976;});
break; case 115: _LL1975: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp1979=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp1979[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp1980; _temp1980.tag= Cyc_TunionFieldList_tok; _temp1980.f1=({ struct Cyc_List_List*
_temp1981=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1981->hd=( void*) Cyc_yyget_TunionField_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1981->tl= 0;
_temp1981;}); _temp1980;}); _temp1979;}); break; case 116: _LL1978: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp1983=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1983[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1984; _temp1984.tag= Cyc_TunionFieldList_tok;
_temp1984.f1=({ struct Cyc_List_List* _temp1985=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1985->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp1985->tl=
0; _temp1985;}); _temp1984;}); _temp1983;}); break; case 117: _LL1982: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1987=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp1987[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp1988; _temp1988.tag= Cyc_TunionFieldList_tok;
_temp1988.f1=({ struct Cyc_List_List* _temp1989=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1989->hd=( void*) Cyc_yyget_TunionField_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp1989->tl=
Cyc_yyget_TunionFieldList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp1989;}); _temp1988;}); _temp1987;}); break; case 118:
_LL1986: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp1991=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp1991[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp1992; _temp1992.tag=
Cyc_TunionFieldList_tok; _temp1992.f1=({ struct Cyc_List_List* _temp1993=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1993->hd=(
void*) Cyc_yyget_TunionField_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp1993->tl= Cyc_yyget_TunionFieldList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp1993;}); _temp1992;});
_temp1991;}); break; case 119: _LL1990: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1995=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1995[ 0]=({ struct Cyc_Scope_tok_struct _temp1996; _temp1996.tag= Cyc_Scope_tok;
_temp1996.f1=( void*)(( void*) Cyc_Absyn_Public); _temp1996;}); _temp1995;});
break; case 120: _LL1994: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp1998=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp1998[ 0]=({ struct Cyc_Scope_tok_struct _temp1999; _temp1999.tag= Cyc_Scope_tok;
_temp1999.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp1999;}); _temp1998;});
break; case 121: _LL1997: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp2001=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp2001[ 0]=({ struct Cyc_Scope_tok_struct _temp2002; _temp2002.tag= Cyc_Scope_tok;
_temp2002.f1=( void*)(( void*) Cyc_Absyn_Static); _temp2002;}); _temp2001;});
break; case 122: _LL2000: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2004=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2004[ 0]=({ struct Cyc_TunionField_tok_struct _temp2005; _temp2005.tag= Cyc_TunionField_tok;
_temp2005.f1=({ struct Cyc_Absyn_Tunionfield* _temp2006=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2006->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2006->tvs= 0;
_temp2006->typs= 0; _temp2006->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2006->sc=(
void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2006;}); _temp2005;}); _temp2004;}); break; case 123:
_LL2003: { struct Cyc_List_List* _temp2008=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))); struct Cyc_List_List*
_temp2009=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)])); yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp2010=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2010[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2011; _temp2011.tag= Cyc_TunionField_tok;
_temp2011.f1=({ struct Cyc_Absyn_Tunionfield* _temp2012=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2012->name= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2012->tvs=
_temp2009; _temp2012->typs= _temp2008; _temp2012->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line);
_temp2012->sc=( void*) Cyc_yyget_Scope_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp2012;}); _temp2011;}); _temp2010;}); break;}
case 124: _LL2007: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 125: _LL2013: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2015=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2015[ 0]=({ struct Cyc_Declarator_tok_struct _temp2016; _temp2016.tag= Cyc_Declarator_tok;
_temp2016.f1=({ struct Cyc_Parse_Declarator* _temp2017=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2017->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->id; _temp2017->tms=
Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2017;});
_temp2016;}); _temp2015;}); break; case 126: _LL2014: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2019=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2019[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2020; _temp2020.tag= Cyc_Declarator_tok;
_temp2020.f1=({ struct Cyc_Parse_Declarator* _temp2021=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2021->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2021->tms= 0;
_temp2021;}); _temp2020;}); _temp2019;}); break; case 127: _LL2018: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 128:
_LL2022: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2024=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2024[ 0]=({ struct Cyc_Declarator_tok_struct _temp2025; _temp2025.tag= Cyc_Declarator_tok;
_temp2025.f1=({ struct Cyc_Parse_Declarator* _temp2026=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2026->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->id;
_temp2026->tms=({ struct Cyc_List_List* _temp2027=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2027->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2027->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]))->tms; _temp2027;}); _temp2026;}); _temp2025;});
_temp2024;}); break; case 129: _LL2023: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2029=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2029[ 0]=({ struct Cyc_Declarator_tok_struct _temp2030; _temp2030.tag= Cyc_Declarator_tok;
_temp2030.f1=({ struct Cyc_Parse_Declarator* _temp2031=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2031->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2031->tms=({ struct Cyc_List_List* _temp2032=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2032->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2033=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2033[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2034; _temp2034.tag= Cyc_Absyn_ConstArray_mod;
_temp2034.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2034;}); _temp2033;})); _temp2032->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2032;}); _temp2031;}); _temp2030;}); _temp2029;}); break; case 130: _LL2028: {
struct _tuple17 _temp2038; struct Cyc_List_List* _temp2039; struct Cyc_Core_Opt*
_temp2041; struct Cyc_Absyn_VarargInfo* _temp2043; int _temp2045; struct Cyc_List_List*
_temp2047; struct _tuple17* _temp2036= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2038=*
_temp2036; _LL2048: _temp2047= _temp2038.f1; goto _LL2046; _LL2046: _temp2045=
_temp2038.f2; goto _LL2044; _LL2044: _temp2043= _temp2038.f3; goto _LL2042;
_LL2042: _temp2041= _temp2038.f4; goto _LL2040; _LL2040: _temp2039= _temp2038.f5;
goto _LL2037; _LL2037: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2049=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2049[ 0]=({ struct Cyc_Declarator_tok_struct _temp2050; _temp2050.tag= Cyc_Declarator_tok;
_temp2050.f1=({ struct Cyc_Parse_Declarator* _temp2051=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2051->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2051->tms=({ struct Cyc_List_List* _temp2052=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2052->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2053=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2053[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2054; _temp2054.tag= Cyc_Absyn_Function_mod;
_temp2054.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2055=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2055[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2056; _temp2056.tag= Cyc_Absyn_WithTypes;
_temp2056.f1= _temp2047; _temp2056.f2= _temp2045; _temp2056.f3= _temp2043;
_temp2056.f4= _temp2041; _temp2056.f5= _temp2039; _temp2056;}); _temp2055;}));
_temp2054;}); _temp2053;})); _temp2052->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2052;});
_temp2051;}); _temp2050;}); _temp2049;}); break;} case 131: _LL2035: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2058=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2058[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2059; _temp2059.tag= Cyc_Declarator_tok;
_temp2059.f1=({ struct Cyc_Parse_Declarator* _temp2060=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2060->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->id;
_temp2060->tms=({ struct Cyc_List_List* _temp2061=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2061->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2062=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2062[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2063; _temp2063.tag= Cyc_Absyn_Function_mod;
_temp2063.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2064=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2064[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2065; _temp2065.tag= Cyc_Absyn_WithTypes;
_temp2065.f1= 0; _temp2065.f2= 0; _temp2065.f3= 0; _temp2065.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2065.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2065;}); _temp2064;})); _temp2063;}); _temp2062;}));
_temp2061->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]))->tms; _temp2061;}); _temp2060;}); _temp2059;});
_temp2058;}); break; case 132: _LL2057: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2067=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2067[ 0]=({ struct Cyc_Declarator_tok_struct _temp2068; _temp2068.tag= Cyc_Declarator_tok;
_temp2068.f1=({ struct Cyc_Parse_Declarator* _temp2069=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2069->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2069->tms=({ struct Cyc_List_List* _temp2070=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2070->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2071=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2071[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2072; _temp2072.tag= Cyc_Absyn_Function_mod;
_temp2072.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2073=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2073[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2074; _temp2074.tag= Cyc_Absyn_NoTypes;
_temp2074.f1= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2074.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2074;});
_temp2073;})); _temp2072;}); _temp2071;})); _temp2070->tl=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2070;}); _temp2069;}); _temp2068;}); _temp2067;}); break; case 133: _LL2066: {
struct Cyc_List_List* _temp2076=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2077=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2077[ 0]=({ struct Cyc_Declarator_tok_struct _temp2078; _temp2078.tag= Cyc_Declarator_tok;
_temp2078.f1=({ struct Cyc_Parse_Declarator* _temp2079=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2079->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2079->tms=({ struct Cyc_List_List* _temp2080=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2080->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2081=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2081[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2082; _temp2082.tag= Cyc_Absyn_TypeParams_mod;
_temp2082.f1= _temp2076; _temp2082.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2082.f3=
0; _temp2082;}); _temp2081;})); _temp2080->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2080;});
_temp2079;}); _temp2078;}); _temp2077;}); break;} case 134: _LL2075:(( struct
Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2084=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2085=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2085[ 0]=({ struct Cyc_Declarator_tok_struct _temp2086; _temp2086.tag= Cyc_Declarator_tok;
_temp2086.f1=({ struct Cyc_Parse_Declarator* _temp2087=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2087->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->id;
_temp2087->tms=({ struct Cyc_List_List* _temp2088=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2088->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2089=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2089[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2090; _temp2090.tag= Cyc_Absyn_TypeParams_mod;
_temp2090.f1= _temp2084; _temp2090.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2090.f3=
0; _temp2090;}); _temp2089;})); _temp2088->tl=( Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms; _temp2088;});
_temp2087;}); _temp2086;}); _temp2085;}); break;} case 135: _LL2083: yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2092=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2092[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2093; _temp2093.tag= Cyc_Declarator_tok;
_temp2093.f1=({ struct Cyc_Parse_Declarator* _temp2094=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2094->id=( Cyc_yyget_Declarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->id;
_temp2094->tms=({ struct Cyc_List_List* _temp2095=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2095->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2096=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2096[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2097; _temp2097.tag= Cyc_Absyn_Attributes_mod;
_temp2097.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2097.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2097;}); _temp2096;}));
_temp2095->tl=( Cyc_yyget_Declarator_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]))->tms; _temp2095;}); _temp2094;}); _temp2093;});
_temp2092;}); break; case 136: _LL2091: Cyc_Parse_err( _tag_arr("identifier has not been declared as a typedef",
sizeof( unsigned char), 46u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); yyval=(
void*)({ struct Cyc_Declarator_tok_struct* _temp2099=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2099[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2100; _temp2100.tag= Cyc_Declarator_tok;
_temp2100.f1=({ struct Cyc_Parse_Declarator* _temp2101=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2101->id= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2101->tms= 0;
_temp2101;}); _temp2100;}); _temp2099;}); break; case 137: _LL2098: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2103=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2103[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2104; _temp2104.tag= Cyc_Declarator_tok;
_temp2104.f1=({ struct Cyc_Parse_Declarator* _temp2105=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2105->id= Cyc_Absyn_exn_name;
_temp2105->tms= 0; _temp2105;}); _temp2104;}); _temp2103;}); break; case 138:
_LL2102: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2107=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp2107[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp2108; _temp2108.tag=
Cyc_TypeModifierList_tok; _temp2108.f1=({ struct Cyc_List_List* _temp2109=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2109->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2110=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2110[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2111; _temp2111.tag= Cyc_Absyn_Pointer_mod;
_temp2111.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2111.f2=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2111.f3= Cyc_Absyn_empty_tqual(); _temp2111;}); _temp2110;}));
_temp2109->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), 0); _temp2109;});
_temp2108;}); _temp2107;}); break; case 139: _LL2106: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2113=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2113[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2114; _temp2114.tag= Cyc_TypeModifierList_tok;
_temp2114.f1=({ struct Cyc_List_List* _temp2115=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2115->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2116=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2116[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2117; _temp2117.tag= Cyc_Absyn_Pointer_mod;
_temp2117.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2117.f2=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2117.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2117;}); _temp2116;})); _temp2115->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), 0); _temp2115;});
_temp2114;}); _temp2113;}); break; case 140: _LL2112: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2119=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2119[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2120; _temp2120.tag= Cyc_TypeModifierList_tok;
_temp2120.f1=({ struct Cyc_List_List* _temp2121=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2121->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2122=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2122[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2123; _temp2123.tag= Cyc_Absyn_Pointer_mod;
_temp2123.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2123.f2=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]); _temp2123.f3= Cyc_Absyn_empty_tqual(); _temp2123;}); _temp2122;}));
_temp2121->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line), Cyc_yyget_AttributeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2121;});
_temp2120;}); _temp2119;}); break; case 141: _LL2118: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp2125=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2125[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2126; _temp2126.tag= Cyc_TypeModifierList_tok;
_temp2126.f1=({ struct Cyc_List_List* _temp2127=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2127->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2128=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2128[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2129; _temp2129.tag= Cyc_Absyn_Pointer_mod;
_temp2129.f1=( void*) Cyc_yyget_Pointer_Sort_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2129.f2=( void*)
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2129.f3= Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2129;}); _temp2128;})); _temp2127->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).last_line), Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2127;});
_temp2126;}); _temp2125;}); break; case 142: _LL2124: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp2131=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2131[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2132; _temp2132.tag= Cyc_Pointer_Sort_tok;
_temp2132.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2133=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2133[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2134; _temp2134.tag=
Cyc_Absyn_Nullable_ps; _temp2134.f1= Cyc_Absyn_exp_unsigned_one; _temp2134;});
_temp2133;})); _temp2132;}); _temp2131;}); break; case 143: _LL2130: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2136=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2136[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2137; _temp2137.tag= Cyc_Pointer_Sort_tok;
_temp2137.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2138=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2138[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2139; _temp2139.tag= Cyc_Absyn_NonNullable_ps; _temp2139.f1= Cyc_Absyn_exp_unsigned_one;
_temp2139;}); _temp2138;})); _temp2137;}); _temp2136;}); break; case 144:
_LL2135: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2141=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2141[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2142; _temp2142.tag=
Cyc_Pointer_Sort_tok; _temp2142.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2143=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2143[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2144; _temp2144.tag=
Cyc_Absyn_Nullable_ps; _temp2144.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2144;});
_temp2143;})); _temp2142;}); _temp2141;}); break; case 145: _LL2140: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2146=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2146[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2147; _temp2147.tag= Cyc_Pointer_Sort_tok;
_temp2147.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2148=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2148[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2149; _temp2149.tag= Cyc_Absyn_NonNullable_ps; _temp2149.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2149;});
_temp2148;})); _temp2147;}); _temp2146;}); break; case 146: _LL2145: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2151=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2151[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2152; _temp2152.tag= Cyc_Pointer_Sort_tok;
_temp2152.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp2152;});
_temp2151;}); break; case 147: _LL2150: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2154=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2154[ 0]=({ struct Cyc_Type_tok_struct _temp2155; _temp2155.tag= Cyc_Type_tok;
_temp2155.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2155;}); _temp2154;});
break; case 148: _LL2153: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 149: _LL2156: if( ! Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_RgnKind)){
Cyc_Parse_err( _tag_arr("expecting region kind", sizeof( unsigned char), 22u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 150: _LL2157: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2159=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2159[ 0]=({ struct Cyc_Type_tok_struct _temp2160; _temp2160.tag= Cyc_Type_tok;
_temp2160.f1=( void*) Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2161=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2161->v=(
void*)(( void*) Cyc_Absyn_RgnKind); _temp2161;}), 0); _temp2160;}); _temp2159;});
break; case 151: _LL2158: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 152: _LL2162: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2164=( struct Cyc_TypeQual_tok_struct*) GC_malloc_atomic( sizeof( struct
Cyc_TypeQual_tok_struct)); _temp2164[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp2165; _temp2165.tag= Cyc_TypeQual_tok; _temp2165.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_yyget_TypeQual_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2165;});
_temp2164;}); break; case 153: _LL2163: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2167=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2167[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2168; _temp2168.tag= Cyc_ParamDeclListBool_tok; _temp2168.f1=({ struct
_tuple17* _temp2169=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2169->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)])); _temp2169->f2= 0; _temp2169->f3= 0; _temp2169->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2169->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2169;}); _temp2168;}); _temp2167;}); break; case 154:
_LL2166: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp2171=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp2171[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp2172; _temp2172.tag=
Cyc_ParamDeclListBool_tok; _temp2172.f1=({ struct _tuple17* _temp2173=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2173->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)])); _temp2173->f2= 1;
_temp2173->f3= 0; _temp2173->f4= Cyc_yyget_TypeOpt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2173->f5= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2173;});
_temp2172;}); _temp2171;}); break; case 155: _LL2170: { struct _tuple2 _temp2177;
void* _temp2178; struct Cyc_Absyn_Tqual _temp2180; struct Cyc_Core_Opt*
_temp2182; struct _tuple2* _temp2175= Cyc_yyget_ParamDecl_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2177=*
_temp2175; _LL2183: _temp2182= _temp2177.f1; goto _LL2181; _LL2181: _temp2180=
_temp2177.f2; goto _LL2179; _LL2179: _temp2178= _temp2177.f3; goto _LL2176;
_LL2176: { struct Cyc_Absyn_VarargInfo* _temp2184=({ struct Cyc_Absyn_VarargInfo*
_temp2188=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2188->name= _temp2182; _temp2188->tq= _temp2180; _temp2188->type=( void*)
_temp2178; _temp2188->rgn=( void*) Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2188->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2188;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2185=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2185[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2186; _temp2186.tag= Cyc_ParamDeclListBool_tok; _temp2186.f1=({ struct
_tuple17* _temp2187=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2187->f1= 0; _temp2187->f2= 0; _temp2187->f3= _temp2184; _temp2187->f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2187->f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2187;}); _temp2186;}); _temp2185;}); break;}} case 156:
_LL2174: { struct _tuple2 _temp2192; void* _temp2193; struct Cyc_Absyn_Tqual
_temp2195; struct Cyc_Core_Opt* _temp2197; struct _tuple2* _temp2190= Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2192=*
_temp2190; _LL2198: _temp2197= _temp2192.f1; goto _LL2196; _LL2196: _temp2195=
_temp2192.f2; goto _LL2194; _LL2194: _temp2193= _temp2192.f3; goto _LL2191;
_LL2191: { struct Cyc_Absyn_VarargInfo* _temp2199=({ struct Cyc_Absyn_VarargInfo*
_temp2203=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2203->name= _temp2197; _temp2203->tq= _temp2195; _temp2203->type=( void*)
_temp2193; _temp2203->rgn=( void*) Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2203->inject=
Cyc_yyget_Bool_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]); _temp2203;}); yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp2200=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp2200[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp2201; _temp2201.tag= Cyc_ParamDeclListBool_tok; _temp2201.f1=({ struct
_tuple17* _temp2202=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp2202->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 7)])); _temp2202->f2= 0; _temp2202->f3= _temp2199; _temp2202->f4=
Cyc_yyget_TypeOpt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2202->f5= Cyc_yyget_Rgnorder_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2202;}); _temp2201;});
_temp2200;}); break;}} case 157: _LL2189: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2205=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2205[ 0]=({ struct Cyc_Type_tok_struct _temp2206; _temp2206.tag= Cyc_Type_tok;
_temp2206.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_empty_conref());
_temp2206;}); _temp2205;}); break; case 158: _LL2204: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp2208=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp2208[ 0]=({ struct Cyc_Type_tok_struct
_temp2209; _temp2209.tag= Cyc_Type_tok; _temp2209.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 2)]), Cyc_Absyn_new_conref( Cyc_yyget_Kind_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))); _temp2209;});
_temp2208;}); break; case 159: _LL2207: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp2211=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp2211[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp2212; _temp2212.tag= Cyc_TypeOpt_tok;
_temp2212.f1= 0; _temp2212;}); _temp2211;}); break; case 160: _LL2210: yyval=(
void*)({ struct Cyc_TypeOpt_tok_struct* _temp2214=( struct Cyc_TypeOpt_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp2214[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp2215; _temp2215.tag= Cyc_TypeOpt_tok; _temp2215.f1=({ struct Cyc_Core_Opt*
_temp2216=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2216->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2217=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2217[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2218; _temp2218.tag= Cyc_Absyn_JoinEff;
_temp2218.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2218;}); _temp2217;})); _temp2216;}); _temp2215;});
_temp2214;}); break; case 161: _LL2213: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct*
_temp2220=( struct Cyc_Rgnorder_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct));
_temp2220[ 0]=({ struct Cyc_Rgnorder_tok_struct _temp2221; _temp2221.tag= Cyc_Rgnorder_tok;
_temp2221.f1= 0; _temp2221;}); _temp2220;}); break; case 162: _LL2219: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 163:
_LL2222: yyval=( void*)({ struct Cyc_Rgnorder_tok_struct* _temp2224=( struct Cyc_Rgnorder_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2224[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2225; _temp2225.tag= Cyc_Rgnorder_tok; _temp2225.f1=({ struct Cyc_List_List*
_temp2226=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2226->hd=( void*)({ struct _tuple19* _temp2227=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2227->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2227->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2227;}); _temp2226->tl= 0; _temp2226;});
_temp2225;}); _temp2224;}); break; case 164: _LL2223: yyval=( void*)({ struct
Cyc_Rgnorder_tok_struct* _temp2229=( struct Cyc_Rgnorder_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgnorder_tok_struct)); _temp2229[ 0]=({ struct Cyc_Rgnorder_tok_struct
_temp2230; _temp2230.tag= Cyc_Rgnorder_tok; _temp2230.f1=({ struct Cyc_List_List*
_temp2231=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2231->hd=( void*)({ struct _tuple19* _temp2232=( struct _tuple19*)
GC_malloc( sizeof( struct _tuple19)); _temp2232->f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2232->f2= Cyc_Parse_id2type( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind)); _temp2232;}); _temp2231->tl= Cyc_yyget_Rgnorder_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2231;});
_temp2230;}); _temp2229;}); break; case 165: _LL2228: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2234=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic(
sizeof( struct Cyc_Bool_tok_struct)); _temp2234[ 0]=({ struct Cyc_Bool_tok_struct
_temp2235; _temp2235.tag= Cyc_Bool_tok; _temp2235.f1= 0; _temp2235;}); _temp2234;});
break; case 166: _LL2233: { struct _tagged_arr _temp2237= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_String_zstrcmp(
_temp2237, _tag_arr("inject", sizeof( unsigned char), 7u)) != 0){ Cyc_Parse_err(
_tag_arr("missing type in function declaration", sizeof( unsigned char), 37u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp2238=( struct Cyc_Bool_tok_struct*) GC_malloc_atomic( sizeof( struct Cyc_Bool_tok_struct));
_temp2238[ 0]=({ struct Cyc_Bool_tok_struct _temp2239; _temp2239.tag= Cyc_Bool_tok;
_temp2239.f1= 1; _temp2239;}); _temp2238;}); break;} case 167: _LL2236: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 168:
_LL2240: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2242=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2242[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2243; _temp2243.tag= Cyc_TypeList_tok; _temp2243.f1= Cyc_List_imp_append(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2243;});
_temp2242;}); break; case 169: _LL2241: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2245=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2245[ 0]=({ struct Cyc_TypeList_tok_struct _temp2246; _temp2246.tag= Cyc_TypeList_tok;
_temp2246.f1= 0; _temp2246;}); _temp2245;}); break; case 170: _LL2244: yyval=
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case
171: _LL2247: if( ! Cyc_Parse_set_vartyp_kind( Cyc_yyget_Type_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]),( void*) Cyc_Absyn_EffKind)){
Cyc_Parse_err( _tag_arr("expecting effect kind", sizeof( unsigned char), 22u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2249=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2249[ 0]=({ struct Cyc_TypeList_tok_struct _temp2250; _temp2250.tag= Cyc_TypeList_tok;
_temp2250.f1=({ struct Cyc_List_List* _temp2251=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2251->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2251->tl= 0;
_temp2251;}); _temp2250;}); _temp2249;}); break; case 172: _LL2248: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]),(
void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2253=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2253[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2254; _temp2254.tag= Cyc_TypeList_tok; _temp2254.f1=({ struct Cyc_List_List*
_temp2255=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2255->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2256=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2256[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2257; _temp2257.tag= Cyc_Absyn_AccessEff;
_temp2257.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2257;}); _temp2256;})); _temp2255->tl= 0;
_temp2255;}); _temp2254;}); _temp2253;}); break; case 173: _LL2252: if( ! Cyc_Parse_set_vartyp_kind(
Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),( void*) Cyc_Absyn_RgnKind)){ Cyc_Parse_err( _tag_arr("expecting region kind",
sizeof( unsigned char), 22u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));} yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp2259=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2259[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2260; _temp2260.tag= Cyc_TypeList_tok; _temp2260.f1=({ struct Cyc_List_List*
_temp2261=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2261->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2262=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2262[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2263; _temp2263.tag= Cyc_Absyn_AccessEff;
_temp2263.f1=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2263;}); _temp2262;})); _temp2261->tl= Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2261;});
_temp2260;}); _temp2259;}); break; case 174: _LL2258: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp2265=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2265[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2266; _temp2266.tag= Cyc_ParamDeclList_tok;
_temp2266.f1=({ struct Cyc_List_List* _temp2267=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2267->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2267->tl= 0;
_temp2267;}); _temp2266;}); _temp2265;}); break; case 175: _LL2264: yyval=( void*)({
struct Cyc_ParamDeclList_tok_struct* _temp2269=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp2269[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp2270; _temp2270.tag= Cyc_ParamDeclList_tok;
_temp2270.f1=({ struct Cyc_List_List* _temp2271=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2271->hd=( void*) Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2271->tl=
Cyc_yyget_ParamDeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2271;}); _temp2270;}); _temp2269;}); break; case 176:
_LL2268: { struct _tuple18 _temp2275; struct Cyc_List_List* _temp2276; struct
Cyc_List_List* _temp2278; struct Cyc_Absyn_Tqual _temp2280; struct _tuple18*
_temp2273= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2275=* _temp2273; _LL2281: _temp2280=
_temp2275.f1; goto _LL2279; _LL2279: _temp2278= _temp2275.f2; goto _LL2277;
_LL2277: _temp2276= _temp2275.f3; goto _LL2274; _LL2274: { struct Cyc_Parse_Declarator
_temp2284; struct Cyc_List_List* _temp2285; struct _tuple1* _temp2287; struct
Cyc_Parse_Declarator* _temp2282= Cyc_yyget_Declarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2284=* _temp2282;
_LL2288: _temp2287= _temp2284.id; goto _LL2286; _LL2286: _temp2285= _temp2284.tms;
goto _LL2283; _LL2283: { void* _temp2289= Cyc_Parse_speclist2typ( _temp2278, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
struct Cyc_List_List* _temp2292; struct Cyc_List_List* _temp2294; void*
_temp2296; struct Cyc_Absyn_Tqual _temp2298; struct _tuple6 _temp2290= Cyc_Parse_apply_tms(
_temp2280, _temp2289, _temp2276, _temp2285); _LL2299: _temp2298= _temp2290.f1;
goto _LL2297; _LL2297: _temp2296= _temp2290.f2; goto _LL2295; _LL2295: _temp2294=
_temp2290.f3; goto _LL2293; _LL2293: _temp2292= _temp2290.f4; goto _LL2291;
_LL2291: if( _temp2294 != 0){ Cyc_Parse_err( _tag_arr("parameter with bad type params",
sizeof( unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if( Cyc_Absyn_is_qvar_qualified(
_temp2287)){ Cyc_Parse_err( _tag_arr("parameter cannot be qualified with a namespace",
sizeof( unsigned char), 47u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
struct Cyc_Core_Opt* _temp2300=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp2304=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2304->v=( void*)(* _temp2287).f2; _temp2304;}); if( _temp2292 != 0){ Cyc_Parse_warn(
_tag_arr("extra attributes on parameter, ignoring", sizeof( unsigned char), 40u),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line));} yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp2301=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp2301[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp2302; _temp2302.tag= Cyc_ParamDecl_tok;
_temp2302.f1=({ struct _tuple2* _temp2303=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp2303->f1= _temp2300; _temp2303->f2= _temp2298; _temp2303->f3=
_temp2296; _temp2303;}); _temp2302;}); _temp2301;}); break;}}}} case 177:
_LL2272: { struct _tuple18 _temp2308; struct Cyc_List_List* _temp2309; struct
Cyc_List_List* _temp2311; struct Cyc_Absyn_Tqual _temp2313; struct _tuple18*
_temp2306= Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2308=* _temp2306; _LL2314: _temp2313= _temp2308.f1;
goto _LL2312; _LL2312: _temp2311= _temp2308.f2; goto _LL2310; _LL2310: _temp2309=
_temp2308.f3; goto _LL2307; _LL2307: { void* _temp2315= Cyc_Parse_speclist2typ(
_temp2311, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); if( _temp2309 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2316=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2316[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2317; _temp2317.tag= Cyc_ParamDecl_tok; _temp2317.f1=({
struct _tuple2* _temp2318=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2318->f1= 0; _temp2318->f2= _temp2313; _temp2318->f3= _temp2315; _temp2318;});
_temp2317;}); _temp2316;}); break;}} case 178: _LL2305: { struct _tuple18
_temp2322; struct Cyc_List_List* _temp2323; struct Cyc_List_List* _temp2325;
struct Cyc_Absyn_Tqual _temp2327; struct _tuple18* _temp2320= Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2322=*
_temp2320; _LL2328: _temp2327= _temp2322.f1; goto _LL2326; _LL2326: _temp2325=
_temp2322.f2; goto _LL2324; _LL2324: _temp2323= _temp2322.f3; goto _LL2321;
_LL2321: { void* _temp2329= Cyc_Parse_speclist2typ( _temp2325, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
struct Cyc_List_List* _temp2330=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct Cyc_List_List*
_temp2333; struct Cyc_List_List* _temp2335; void* _temp2337; struct Cyc_Absyn_Tqual
_temp2339; struct _tuple6 _temp2331= Cyc_Parse_apply_tms( _temp2327, _temp2329,
_temp2323, _temp2330); _LL2340: _temp2339= _temp2331.f1; goto _LL2338; _LL2338:
_temp2337= _temp2331.f2; goto _LL2336; _LL2336: _temp2335= _temp2331.f3; goto
_LL2334; _LL2334: _temp2333= _temp2331.f4; goto _LL2332; _LL2332: if( _temp2335
!= 0){ Cyc_Parse_warn( _tag_arr("bad type parameters on formal argument, ignoring",
sizeof( unsigned char), 49u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2333 != 0){ Cyc_Parse_warn( _tag_arr("bad attributes on parameter, ignoring",
sizeof( unsigned char), 38u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2341=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2341[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2342; _temp2342.tag= Cyc_ParamDecl_tok; _temp2342.f1=({
struct _tuple2* _temp2343=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2343->f1= 0; _temp2343->f2= _temp2339; _temp2343->f3= _temp2337; _temp2343;});
_temp2342;}); _temp2341;}); break;}} case 179: _LL2319: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp2345=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp2345[ 0]=({ struct Cyc_IdList_tok_struct
_temp2346; _temp2346.tag= Cyc_IdList_tok; _temp2346.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2346;});
_temp2345;}); break; case 180: _LL2344: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp2348=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp2348[ 0]=({ struct Cyc_IdList_tok_struct _temp2349; _temp2349.tag= Cyc_IdList_tok;
_temp2349.f1=({ struct Cyc_List_List* _temp2350=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2350->hd=( void*)({ struct
_tagged_arr* _temp2351=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2351[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2351;}); _temp2350->tl=
0; _temp2350;}); _temp2349;}); _temp2348;}); break; case 181: _LL2347: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp2353=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp2353[ 0]=({ struct Cyc_IdList_tok_struct
_temp2354; _temp2354.tag= Cyc_IdList_tok; _temp2354.f1=({ struct Cyc_List_List*
_temp2355=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2355->hd=( void*)({ struct _tagged_arr* _temp2356=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp2356[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2356;});
_temp2355->tl= Cyc_yyget_IdList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2355;}); _temp2354;}); _temp2353;}); break; case 182:
_LL2352: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 183: _LL2357: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 184: _LL2358: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2360=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2360[ 0]=({ struct Cyc_Exp_tok_struct _temp2361; _temp2361.tag= Cyc_Exp_tok;
_temp2361.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2362=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2362[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2363; _temp2363.tag= Cyc_Absyn_UnresolvedMem_e; _temp2363.f1= 0; _temp2363.f2=
0; _temp2363;}); _temp2362;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2361;});
_temp2360;}); break; case 185: _LL2359: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2365=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2365[ 0]=({ struct Cyc_Exp_tok_struct _temp2366; _temp2366.tag= Cyc_Exp_tok;
_temp2366.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp2367=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp2367[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp2368; _temp2368.tag= Cyc_Absyn_UnresolvedMem_e; _temp2368.f1= 0; _temp2368.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2368;});
_temp2367;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2366;}); _temp2365;}); break; case 186:
_LL2364: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2370=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2370[ 0]=({ struct Cyc_Exp_tok_struct
_temp2371; _temp2371.tag= Cyc_Exp_tok; _temp2371.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp2372=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp2372[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp2373; _temp2373.tag= Cyc_Absyn_UnresolvedMem_e;
_temp2373.f1= 0; _temp2373.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp2373;});
_temp2372;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2371;}); _temp2370;}); break; case 187:
_LL2369: { struct Cyc_Absyn_Vardecl* _temp2375= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp2381=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2381->f1=( void*) Cyc_Absyn_Loc_n; _temp2381->f2=({ struct _tagged_arr*
_temp2382=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp2382[ 0]= Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 5)]); _temp2382;}); _temp2381;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).last_line)));
_temp2375->tq=({ struct Cyc_Absyn_Tqual _temp2376; _temp2376.q_const= 1;
_temp2376.q_volatile= 0; _temp2376.q_restrict= 1; _temp2376;}); yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp2377=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp2377[ 0]=({ struct Cyc_Exp_tok_struct
_temp2378; _temp2378.tag= Cyc_Exp_tok; _temp2378.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp2379=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp2379[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp2380; _temp2380.tag= Cyc_Absyn_Comprehension_e;
_temp2380.f1= _temp2375; _temp2380.f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2380.f3= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2380;});
_temp2379;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2378;}); _temp2377;}); break;} case
188: _LL2374: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2384=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2384[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2385; _temp2385.tag=
Cyc_InitializerList_tok; _temp2385.f1=({ struct Cyc_List_List* _temp2386=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2386->hd=(
void*)({ struct _tuple20* _temp2387=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2387->f1= 0; _temp2387->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2387;}); _temp2386->tl=
0; _temp2386;}); _temp2385;}); _temp2384;}); break; case 189: _LL2383: yyval=(
void*)({ struct Cyc_InitializerList_tok_struct* _temp2389=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp2389[ 0]=({
struct Cyc_InitializerList_tok_struct _temp2390; _temp2390.tag= Cyc_InitializerList_tok;
_temp2390.f1=({ struct Cyc_List_List* _temp2391=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2391->hd=( void*)({ struct
_tuple20* _temp2392=( struct _tuple20*) GC_malloc( sizeof( struct _tuple20));
_temp2392->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2392->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2392;});
_temp2391->tl= 0; _temp2391;}); _temp2390;}); _temp2389;}); break; case 190:
_LL2388: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2394=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2394[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2395; _temp2395.tag=
Cyc_InitializerList_tok; _temp2395.f1=({ struct Cyc_List_List* _temp2396=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2396->hd=(
void*)({ struct _tuple20* _temp2397=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2397->f1= 0; _temp2397->f2= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2397;}); _temp2396->tl=
Cyc_yyget_InitializerList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2396;}); _temp2395;}); _temp2394;}); break; case 191:
_LL2393: yyval=( void*)({ struct Cyc_InitializerList_tok_struct* _temp2399=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp2399[ 0]=({ struct Cyc_InitializerList_tok_struct _temp2400; _temp2400.tag=
Cyc_InitializerList_tok; _temp2400.f1=({ struct Cyc_List_List* _temp2401=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2401->hd=(
void*)({ struct _tuple20* _temp2402=( struct _tuple20*) GC_malloc( sizeof(
struct _tuple20)); _temp2402->f1= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2402->f2= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2402;});
_temp2401->tl= Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2401;});
_temp2400;}); _temp2399;}); break; case 192: _LL2398: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp2404=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp2404[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp2405; _temp2405.tag= Cyc_DesignatorList_tok;
_temp2405.f1= Cyc_List_imp_rev( Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp2405;});
_temp2404;}); break; case 193: _LL2403: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2407=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2407[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2408; _temp2408.tag=
Cyc_DesignatorList_tok; _temp2408.f1=({ struct Cyc_List_List* _temp2409=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2409->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2409->tl= 0; _temp2409;}); _temp2408;}); _temp2407;});
break; case 194: _LL2406: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp2411=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp2411[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp2412; _temp2412.tag=
Cyc_DesignatorList_tok; _temp2412.f1=({ struct Cyc_List_List* _temp2413=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2413->hd=( void*)
Cyc_yyget_Designator_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2413->tl= Cyc_yyget_DesignatorList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2413;});
_temp2412;}); _temp2411;}); break; case 195: _LL2410: yyval=( void*)({ struct
Cyc_Designator_tok_struct* _temp2415=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2415[ 0]=({ struct
Cyc_Designator_tok_struct _temp2416; _temp2416.tag= Cyc_Designator_tok;
_temp2416.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp2417=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp2417[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp2418; _temp2418.tag=
Cyc_Absyn_ArrayElement; _temp2418.f1= Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2418;});
_temp2417;})); _temp2416;}); _temp2415;}); break; case 196: _LL2414: yyval=(
void*)({ struct Cyc_Designator_tok_struct* _temp2420=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp2420[ 0]=({ struct
Cyc_Designator_tok_struct _temp2421; _temp2421.tag= Cyc_Designator_tok;
_temp2421.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2422=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2422[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2423; _temp2423.tag= Cyc_Absyn_FieldName;
_temp2423.f1=({ struct _tagged_arr* _temp2424=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp2424[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2424;}); _temp2423;});
_temp2422;})); _temp2421;}); _temp2420;}); break; case 197: _LL2419: { void*
_temp2426= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); struct Cyc_List_List*
_temp2427=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)])).f3; if( _temp2427 != 0){ Cyc_Parse_warn( _tag_arr("ignoring attributes in type",
sizeof( unsigned char), 28u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));}{ struct
Cyc_Absyn_Tqual _temp2428=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp2429=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2429[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2430; _temp2430.tag= Cyc_ParamDecl_tok; _temp2430.f1=({
struct _tuple2* _temp2431=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2431->f1= 0; _temp2431->f2= _temp2428; _temp2431->f3= _temp2426; _temp2431;});
_temp2430;}); _temp2429;}); break;}} case 198: _LL2425: { void* _temp2433= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)); struct
Cyc_List_List* _temp2434=(* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])).f3; struct Cyc_Absyn_Tqual
_temp2435=(* Cyc_yyget_QualSpecList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)])).f1; struct Cyc_List_List* _temp2436=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms; struct
_tuple6 _temp2437= Cyc_Parse_apply_tms( _temp2435, _temp2433, _temp2434,
_temp2436); if( _temp2437.f3 != 0){ Cyc_Parse_warn( _tag_arr("bad type params, ignoring",
sizeof( unsigned char), 26u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} if(
_temp2437.f4 != 0){ Cyc_Parse_warn( _tag_arr("bad specifiers, ignoring", sizeof(
unsigned char), 25u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_ParamDecl_tok_struct* _temp2438=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp2438[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp2439; _temp2439.tag= Cyc_ParamDecl_tok; _temp2439.f1=({
struct _tuple2* _temp2440=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2440->f1= 0; _temp2440->f2= _temp2437.f1; _temp2440->f3= _temp2437.f2;
_temp2440;}); _temp2439;}); _temp2438;}); break;} case 199: _LL2432: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp2442=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp2442[ 0]=({ struct Cyc_Type_tok_struct
_temp2443; _temp2443.tag= Cyc_Type_tok; _temp2443.f1=( void*)(* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f3; _temp2443;});
_temp2442;}); break; case 200: _LL2441: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2445=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2445[ 0]=({ struct Cyc_Type_tok_struct _temp2446; _temp2446.tag= Cyc_Type_tok;
_temp2446.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2447=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2447[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2448; _temp2448.tag= Cyc_Absyn_JoinEff;
_temp2448.f1= 0; _temp2448;}); _temp2447;})); _temp2446;}); _temp2445;}); break;
case 201: _LL2444: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp2450=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2450[ 0]=({ struct Cyc_Type_tok_struct _temp2451; _temp2451.tag= Cyc_Type_tok;
_temp2451.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2452=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2452[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2453; _temp2453.tag= Cyc_Absyn_JoinEff;
_temp2453.f1= Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2453;}); _temp2452;})); _temp2451;});
_temp2450;}); break; case 202: _LL2449: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp2455=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp2455[ 0]=({ struct Cyc_Type_tok_struct _temp2456; _temp2456.tag= Cyc_Type_tok;
_temp2456.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2457=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2457[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2458; _temp2458.tag= Cyc_Absyn_JoinEff;
_temp2458.f1=({ struct Cyc_List_List* _temp2459=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2459->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2459->tl=
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2459;}); _temp2458;}); _temp2457;})); _temp2456;});
_temp2455;}); break; case 203: _LL2454: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2461=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2461[ 0]=({ struct Cyc_TypeList_tok_struct _temp2462; _temp2462.tag= Cyc_TypeList_tok;
_temp2462.f1=({ struct Cyc_List_List* _temp2463=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2463->hd=( void*) Cyc_yyget_Type_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2463->tl= 0;
_temp2463;}); _temp2462;}); _temp2461;}); break; case 204: _LL2460: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2465=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2465[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2466; _temp2466.tag= Cyc_TypeList_tok; _temp2466.f1=({ struct Cyc_List_List*
_temp2467=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2467->hd=( void*) Cyc_yyget_Type_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp2467->tl= Cyc_yyget_TypeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2467;});
_temp2466;}); _temp2465;}); break; case 205: _LL2464: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2469=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2469[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2470; _temp2470.tag= Cyc_AbstractDeclarator_tok;
_temp2470.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2471=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2471->tms= Cyc_yyget_TypeModifierList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2471;});
_temp2470;}); _temp2469;}); break; case 206: _LL2468: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 207: _LL2472:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2474=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2474[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2475; _temp2475.tag=
Cyc_AbstractDeclarator_tok; _temp2475.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2476=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2476->tms= Cyc_List_imp_append( Cyc_yyget_TypeModifierList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]),( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]))->tms); _temp2476;});
_temp2475;}); _temp2474;}); break; case 208: _LL2473: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 209:
_LL2477: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2479=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2479[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2480; _temp2480.tag=
Cyc_AbstractDeclarator_tok; _temp2480.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2481=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2481->tms=({ struct Cyc_List_List* _temp2482=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2482->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2482->tl= 0; _temp2482;}); _temp2481;}); _temp2480;}); _temp2479;}); break;
case 210: _LL2478: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2484=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2484[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2485; _temp2485.tag= Cyc_AbstractDeclarator_tok; _temp2485.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2486=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2486->tms=({ struct Cyc_List_List* _temp2487=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2487->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2487->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]))->tms; _temp2487;});
_temp2486;}); _temp2485;}); _temp2484;}); break; case 211: _LL2483: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp2489=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2489[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2490; _temp2490.tag= Cyc_AbstractDeclarator_tok;
_temp2490.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2491=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2491->tms=({
struct Cyc_List_List* _temp2492=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2492->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp2493=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp2493[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp2494; _temp2494.tag= Cyc_Absyn_ConstArray_mod; _temp2494.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2494;});
_temp2493;})); _temp2492->tl= 0; _temp2492;}); _temp2491;}); _temp2490;});
_temp2489;}); break; case 212: _LL2488: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2496=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2496[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2497; _temp2497.tag= Cyc_AbstractDeclarator_tok; _temp2497.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2498=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2498->tms=({ struct Cyc_List_List* _temp2499=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2499->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2500=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2500[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2501; _temp2501.tag= Cyc_Absyn_ConstArray_mod;
_temp2501.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2501;}); _temp2500;})); _temp2499->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2499;}); _temp2498;}); _temp2497;}); _temp2496;}); break; case 213: _LL2495:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2503=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2503[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2504; _temp2504.tag=
Cyc_AbstractDeclarator_tok; _temp2504.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2505=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2505->tms=({ struct Cyc_List_List* _temp2506=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2506->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2507=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2507[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2508; _temp2508.tag= Cyc_Absyn_Function_mod;
_temp2508.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2509=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2509[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2510; _temp2510.tag= Cyc_Absyn_WithTypes;
_temp2510.f1= 0; _temp2510.f2= 0; _temp2510.f3= 0; _temp2510.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2510.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2510;}); _temp2509;})); _temp2508;}); _temp2507;}));
_temp2506->tl= 0; _temp2506;}); _temp2505;}); _temp2504;}); _temp2503;}); break;
case 214: _LL2502: { struct _tuple17 _temp2514; struct Cyc_List_List* _temp2515;
struct Cyc_Core_Opt* _temp2517; struct Cyc_Absyn_VarargInfo* _temp2519; int
_temp2521; struct Cyc_List_List* _temp2523; struct _tuple17* _temp2512= Cyc_yyget_ParamDeclListBool_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2514=*
_temp2512; _LL2524: _temp2523= _temp2514.f1; goto _LL2522; _LL2522: _temp2521=
_temp2514.f2; goto _LL2520; _LL2520: _temp2519= _temp2514.f3; goto _LL2518;
_LL2518: _temp2517= _temp2514.f4; goto _LL2516; _LL2516: _temp2515= _temp2514.f5;
goto _LL2513; _LL2513: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2525=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2525[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2526; _temp2526.tag= Cyc_AbstractDeclarator_tok; _temp2526.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2527=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2527->tms=({ struct Cyc_List_List* _temp2528=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2528->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2529=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2529[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2530; _temp2530.tag= Cyc_Absyn_Function_mod;
_temp2530.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2531=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2531[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2532; _temp2532.tag= Cyc_Absyn_WithTypes;
_temp2532.f1= _temp2523; _temp2532.f2= _temp2521; _temp2532.f3= _temp2519;
_temp2532.f4= _temp2517; _temp2532.f5= _temp2515; _temp2532;}); _temp2531;}));
_temp2530;}); _temp2529;})); _temp2528->tl= 0; _temp2528;}); _temp2527;});
_temp2526;}); _temp2525;}); break;} case 215: _LL2511: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp2534=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp2534[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp2535; _temp2535.tag= Cyc_AbstractDeclarator_tok;
_temp2535.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp2536=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp2536->tms=({
struct Cyc_List_List* _temp2537=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2537->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2538=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2538[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2539; _temp2539.tag=
Cyc_Absyn_Function_mod; _temp2539.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2540=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2540[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2541; _temp2541.tag= Cyc_Absyn_WithTypes;
_temp2541.f1= 0; _temp2541.f2= 0; _temp2541.f3= 0; _temp2541.f4= Cyc_yyget_TypeOpt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2541.f5=
Cyc_yyget_Rgnorder_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2541;}); _temp2540;})); _temp2539;}); _temp2538;}));
_temp2537->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]))->tms; _temp2537;});
_temp2536;}); _temp2535;}); _temp2534;}); break; case 216: _LL2533: { struct
_tuple17 _temp2545; struct Cyc_List_List* _temp2546; struct Cyc_Core_Opt*
_temp2548; struct Cyc_Absyn_VarargInfo* _temp2550; int _temp2552; struct Cyc_List_List*
_temp2554; struct _tuple17* _temp2543= Cyc_yyget_ParamDeclListBool_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2545=*
_temp2543; _LL2555: _temp2554= _temp2545.f1; goto _LL2553; _LL2553: _temp2552=
_temp2545.f2; goto _LL2551; _LL2551: _temp2550= _temp2545.f3; goto _LL2549;
_LL2549: _temp2548= _temp2545.f4; goto _LL2547; _LL2547: _temp2546= _temp2545.f5;
goto _LL2544; _LL2544: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2556=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2556[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2557; _temp2557.tag= Cyc_AbstractDeclarator_tok; _temp2557.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2558=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2558->tms=({ struct Cyc_List_List* _temp2559=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2559->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2560=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2560[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2561; _temp2561.tag= Cyc_Absyn_Function_mod;
_temp2561.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2562=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2562[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2563; _temp2563.tag= Cyc_Absyn_WithTypes;
_temp2563.f1= _temp2554; _temp2563.f2= _temp2552; _temp2563.f3= _temp2550;
_temp2563.f4= _temp2548; _temp2563.f5= _temp2546; _temp2563;}); _temp2562;}));
_temp2561;}); _temp2560;})); _temp2559->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2559;}); _temp2558;}); _temp2557;}); _temp2556;}); break;} case 217:
_LL2542: { struct Cyc_List_List* _temp2565=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2566=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2566[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2567; _temp2567.tag= Cyc_AbstractDeclarator_tok; _temp2567.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2568=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2568->tms=({ struct Cyc_List_List* _temp2569=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2569->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2570=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2570[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2571; _temp2571.tag= Cyc_Absyn_TypeParams_mod;
_temp2571.f1= _temp2565; _temp2571.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2571.f3=
0; _temp2571;}); _temp2570;})); _temp2569->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2569;}); _temp2568;}); _temp2567;}); _temp2566;}); break;} case 218:
_LL2564:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2573=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line), Cyc_List_imp_rev(
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]))); yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp2574=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp2574[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp2575; _temp2575.tag= Cyc_AbstractDeclarator_tok; _temp2575.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2576=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2576->tms=({ struct Cyc_List_List* _temp2577=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2577->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2578=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2578[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2579; _temp2579.tag= Cyc_Absyn_TypeParams_mod;
_temp2579.f1= _temp2573; _temp2579.f2= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2579.f3=
0; _temp2579;}); _temp2578;})); _temp2577->tl=( Cyc_yyget_AbstractDeclarator_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]))->tms;
_temp2577;}); _temp2576;}); _temp2575;}); _temp2574;}); break;} case 219:
_LL2572: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp2581=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp2581[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp2582; _temp2582.tag=
Cyc_AbstractDeclarator_tok; _temp2582.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp2583=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp2583->tms=({ struct Cyc_List_List* _temp2584=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2584->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2585=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2585[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2586; _temp2586.tag= Cyc_Absyn_Attributes_mod;
_temp2586.f1= Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2586.f2= Cyc_yyget_AttributeList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2586;}); _temp2585;}));
_temp2584->tl=( Cyc_yyget_AbstractDeclarator_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]))->tms; _temp2584;});
_temp2583;}); _temp2582;}); _temp2581;}); break; case 220: _LL2580: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 221: _LL2587:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
222: _LL2588: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 223: _LL2589: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 224: _LL2590: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 225: _LL2591:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
226: _LL2592: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), _tag_arr("`H",
sizeof( unsigned char), 3u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2604=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2604->name=({ struct
_tagged_arr* _temp2605=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2605[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2605;});
_temp2604->identity= 0; _temp2604->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp2604;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2602=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2602[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2603; _temp2603.tag= Cyc_Absyn_VarType;
_temp2603.f1= tv; _temp2603;}); _temp2602;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2594=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2594[ 0]=({ struct Cyc_Stmt_tok_struct _temp2595; _temp2595.tag= Cyc_Stmt_tok;
_temp2595.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2596=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2596[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2597; _temp2597.tag= Cyc_Absyn_Region_s;
_temp2597.f1= tv; _temp2597.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2598=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2598->f1=(
void*) Cyc_Absyn_Loc_n; _temp2598->f2=({ struct _tagged_arr* _temp2599=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2599[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2599;});
_temp2598;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2600=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2600[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2601; _temp2601.tag=
Cyc_Absyn_RgnHandleType; _temp2601.f1=( void*) t; _temp2601;}); _temp2600;}), 0);
_temp2597.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2597;}); _temp2596;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2595;});
_temp2594;}); break;} case 227: _LL2593: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), _tag_arr("H",
sizeof( unsigned char), 2u)) == 0){ Cyc_Parse_err( _tag_arr("bad occurrence of heap region `H",
sizeof( unsigned char), 33u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp2617=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2617->name=({ struct
_tagged_arr* _temp2618=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2618[ 0]=( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp2620; _temp2620.tag= Cyc_Stdio_String_pa; _temp2620.f1=( struct _tagged_arr)
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset
- 1)]);{ void* _temp2619[ 1u]={& _temp2620}; Cyc_Stdio_aprintf( _tag_arr("`%s",
sizeof( unsigned char), 4u), _tag_arr( _temp2619, sizeof( void*), 1u));}});
_temp2618;}); _temp2617->identity= 0; _temp2617->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp2617;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2615=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2615[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2616; _temp2616.tag= Cyc_Absyn_VarType;
_temp2616.f1= tv; _temp2616;}); _temp2615;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2607=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2607[ 0]=({ struct Cyc_Stmt_tok_struct _temp2608; _temp2608.tag= Cyc_Stmt_tok;
_temp2608.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp2609=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp2609[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp2610; _temp2610.tag= Cyc_Absyn_Region_s;
_temp2610.f1= tv; _temp2610.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2611=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2611->f1=(
void*) Cyc_Absyn_Loc_n; _temp2611->f2=({ struct _tagged_arr* _temp2612=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2612[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2612;});
_temp2611;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp2613=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp2613[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp2614; _temp2614.tag=
Cyc_Absyn_RgnHandleType; _temp2614.f1=( void*) t; _temp2614;}); _temp2613;}), 0);
_temp2610.f3= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2610;}); _temp2609;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2608;});
_temp2607;}); break;} case 228: _LL2606: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2622=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2622[ 0]=({ struct Cyc_Stmt_tok_struct _temp2623; _temp2623.tag= Cyc_Stmt_tok;
_temp2623.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct*
_temp2624=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp2624[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp2625; _temp2625.tag= Cyc_Absyn_Cut_s;
_temp2625.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2625;}); _temp2624;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2623;});
_temp2622;}); break; case 229: _LL2621: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2627=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2627[ 0]=({ struct Cyc_Stmt_tok_struct _temp2628; _temp2628.tag= Cyc_Stmt_tok;
_temp2628.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp2629=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp2629[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp2630; _temp2630.tag= Cyc_Absyn_Splice_s;
_temp2630.f1= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2630;}); _temp2629;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2628;});
_temp2627;}); break; case 230: _LL2626: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2632=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2632[ 0]=({ struct Cyc_Stmt_tok_struct _temp2633; _temp2633.tag= Cyc_Stmt_tok;
_temp2633.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp2634=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp2634[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp2635; _temp2635.tag= Cyc_Absyn_Label_s;
_temp2635.f1=({ struct _tagged_arr* _temp2636=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp2636[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2636;});
_temp2635.f2= Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2635;}); _temp2634;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2633;});
_temp2632;}); break; case 231: _LL2631: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2638=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2638[ 0]=({ struct Cyc_Stmt_tok_struct _temp2639; _temp2639.tag= Cyc_Stmt_tok;
_temp2639.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2639;});
_temp2638;}); break; case 232: _LL2637: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2641=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2641[ 0]=({ struct Cyc_Stmt_tok_struct _temp2642; _temp2642.tag= Cyc_Stmt_tok;
_temp2642.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2642;}); _temp2641;}); break; case 233: _LL2640: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2644=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2644[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2645; _temp2645.tag= Cyc_Stmt_tok; _temp2645.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2645;}); _temp2644;}); break; case 234: _LL2643: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 235:
_LL2646: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2648=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2648[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2649; _temp2649.tag= Cyc_Stmt_tok; _temp2649.f1= Cyc_Parse_flatten_declarations(
Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0)); _temp2649;}); _temp2648;}); break;
case 236: _LL2647: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2651=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2651[ 0]=({ struct Cyc_Stmt_tok_struct _temp2652; _temp2652.tag= Cyc_Stmt_tok;
_temp2652.f1= Cyc_Parse_flatten_declarations( Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2652;});
_temp2651;}); break; case 237: _LL2650: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 238: _LL2653:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2655=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2655[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2656; _temp2656.tag= Cyc_Stmt_tok; _temp2656.f1= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2656;}); _temp2655;}); break; case 239: _LL2654: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2658=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2658[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2659; _temp2659.tag= Cyc_Stmt_tok; _temp2659.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2660=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2660[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2661; _temp2661.tag= Cyc_Absyn_Fn_d;
_temp2661.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2661;}); _temp2660;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)), Cyc_Absyn_skip_stmt(
0)); _temp2659;}); _temp2658;}); break; case 240: _LL2657: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp2663=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp2663[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2664; _temp2664.tag= Cyc_Stmt_tok; _temp2664.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2665=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2665[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2666; _temp2666.tag= Cyc_Absyn_Fn_d;
_temp2666.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp2666;}); _temp2665;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line)),
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]));
_temp2664;}); _temp2663;}); break; case 241: _LL2662: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2668=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2668[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2669; _temp2669.tag= Cyc_Stmt_tok; _temp2669.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2669;});
_temp2668;}); break; case 242: _LL2667: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2671=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2671[ 0]=({ struct Cyc_Stmt_tok_struct _temp2672; _temp2672.tag= Cyc_Stmt_tok;
_temp2672.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2672;}); _temp2671;}); break; case 243: _LL2670: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2674=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2674[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2675; _temp2675.tag= Cyc_Stmt_tok; _temp2675.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_SwitchClauseList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2675;}); _temp2674;}); break; case 244: _LL2673: if( Cyc_String_strcmp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), _tag_arr("C",
sizeof( unsigned char), 2u)) != 0){ Cyc_Parse_err( _tag_arr("only switch \"C\" { ... } is allowed",
sizeof( unsigned char), 35u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2677=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2677[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2678; _temp2678.tag= Cyc_Stmt_tok; _temp2678.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_SwitchC_s_struct* _temp2679=( struct Cyc_Absyn_SwitchC_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_s_struct)); _temp2679[ 0]=({ struct
Cyc_Absyn_SwitchC_s_struct _temp2680; _temp2680.tag= Cyc_Absyn_SwitchC_s;
_temp2680.f1= Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]); _temp2680.f2= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2680;});
_temp2679;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 7)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2678;}); _temp2677;}); break; case 245:
_LL2676: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2682=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2682[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2683; _temp2683.tag= Cyc_Stmt_tok; _temp2683.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_SwitchClauseList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2683;});
_temp2682;}); break; case 246: _LL2681: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2685=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2685[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2686; _temp2686.tag= Cyc_SwitchClauseList_tok; _temp2686.f1= 0; _temp2686;});
_temp2685;}); break; case 247: _LL2684: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2688=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2688[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2689; _temp2689.tag= Cyc_SwitchClauseList_tok; _temp2689.f1=({ struct Cyc_List_List*
_temp2690=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2690->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2691=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2691->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp2691->pat_vars= 0; _temp2691->where_clause= 0; _temp2691->body= Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2691->loc=
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line); _temp2691;}); _temp2690->tl= 0; _temp2690;});
_temp2689;}); _temp2688;}); break; case 248: _LL2687: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp2693=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp2693[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp2694; _temp2694.tag= Cyc_SwitchClauseList_tok;
_temp2694.f1=({ struct Cyc_List_List* _temp2695=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2695->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp2696=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2696->pattern= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2696->pat_vars= 0; _temp2696->where_clause= 0;
_temp2696->body= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2696->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2696;});
_temp2695->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2695;}); _temp2694;});
_temp2693;}); break; case 249: _LL2692: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2698=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2698[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2699; _temp2699.tag= Cyc_SwitchClauseList_tok; _temp2699.f1=({ struct Cyc_List_List*
_temp2700=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2700->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2701=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2701->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2701->pat_vars=
0; _temp2701->where_clause= 0; _temp2701->body= Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2701->loc= Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line);
_temp2701;}); _temp2700->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2700;}); _temp2699;});
_temp2698;}); break; case 250: _LL2697: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2703=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2703[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2704; _temp2704.tag= Cyc_SwitchClauseList_tok; _temp2704.f1=({ struct Cyc_List_List*
_temp2705=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2705->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2706=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2706->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2706->pat_vars=
0; _temp2706->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp2706->body=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2706->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2706;});
_temp2705->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2705;}); _temp2704;});
_temp2703;}); break; case 251: _LL2702: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp2708=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp2708[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp2709; _temp2709.tag= Cyc_SwitchClauseList_tok; _temp2709.f1=({ struct Cyc_List_List*
_temp2710=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2710->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp2711=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2711->pattern= Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); _temp2711->pat_vars=
0; _temp2711->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]); _temp2711->body=
Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
1)]); _temp2711->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2711;});
_temp2710->tl= Cyc_yyget_SwitchClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2710;}); _temp2709;});
_temp2708;}); break; case 252: _LL2707: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2713=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2713[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2714; _temp2714.tag= Cyc_SwitchCClauseList_tok; _temp2714.f1= 0; _temp2714;});
_temp2713;}); break; case 253: _LL2712: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2716=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2716[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2717; _temp2717.tag= Cyc_SwitchCClauseList_tok; _temp2717.f1=({ struct Cyc_List_List*
_temp2718=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2718->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2719=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2719->cnst_exp= 0;
_temp2719->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Absyn_break_stmt( 0),
0); _temp2719->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2719;});
_temp2718->tl= 0; _temp2718;}); _temp2717;}); _temp2716;}); break; case 254:
_LL2715: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct* _temp2721=(
struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchCClauseList_tok_struct));
_temp2721[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct _temp2722; _temp2722.tag=
Cyc_SwitchCClauseList_tok; _temp2722.f1=({ struct Cyc_List_List* _temp2723=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2723->hd=(
void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2724=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2724->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]); _temp2724->body= Cyc_Absyn_fallthru_stmt( 0, 0);
_temp2724->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2724;});
_temp2723->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2723;}); _temp2722;});
_temp2721;}); break; case 255: _LL2720: yyval=( void*)({ struct Cyc_SwitchCClauseList_tok_struct*
_temp2726=( struct Cyc_SwitchCClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchCClauseList_tok_struct)); _temp2726[ 0]=({ struct Cyc_SwitchCClauseList_tok_struct
_temp2727; _temp2727.tag= Cyc_SwitchCClauseList_tok; _temp2727.f1=({ struct Cyc_List_List*
_temp2728=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2728->hd=( void*)({ struct Cyc_Absyn_SwitchC_clause* _temp2729=( struct Cyc_Absyn_SwitchC_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_SwitchC_clause)); _temp2729->cnst_exp=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 3)]); _temp2729->body= Cyc_Absyn_seq_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Absyn_fallthru_stmt(
0, 0), 0); _temp2729->loc= Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line); _temp2729;});
_temp2728->tl= Cyc_yyget_SwitchCClauseList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2728;}); _temp2727;});
_temp2726;}); break; case 256: _LL2725: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2731=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2731[ 0]=({ struct Cyc_Stmt_tok_struct _temp2732; _temp2732.tag= Cyc_Stmt_tok;
_temp2732.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2732;}); _temp2731;}); break; case 257: _LL2730: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2734=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2734[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2735; _temp2735.tag= Cyc_Stmt_tok; _temp2735.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2735;}); _temp2734;}); break; case 258: _LL2733: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2737=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2737[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2738; _temp2738.tag= Cyc_Stmt_tok; _temp2738.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2738;}); _temp2737;}); break; case 259: _LL2736: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2740=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2740[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2741; _temp2741.tag= Cyc_Stmt_tok; _temp2741.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2741;}); _temp2740;}); break; case 260: _LL2739: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2743=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2743[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2744; _temp2744.tag= Cyc_Stmt_tok; _temp2744.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2744;}); _temp2743;}); break; case 261: _LL2742: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2746=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2746[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2747; _temp2747.tag= Cyc_Stmt_tok; _temp2747.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2747;}); _temp2746;}); break; case 262: _LL2745: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2749=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2749[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2750; _temp2750.tag= Cyc_Stmt_tok; _temp2750.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2750;}); _temp2749;}); break; case 263: _LL2748: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2752=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2752[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2753; _temp2753.tag= Cyc_Stmt_tok; _temp2753.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_Absyn_true_exp(
0), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2753;});
_temp2752;}); break; case 264: _LL2751: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2755=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2755[ 0]=({ struct Cyc_Stmt_tok_struct _temp2756; _temp2756.tag= Cyc_Stmt_tok;
_temp2756.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2756;});
_temp2755;}); break; case 265: _LL2754: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp2758=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp2758[ 0]=({ struct Cyc_Stmt_tok_struct _temp2759; _temp2759.tag= Cyc_Stmt_tok;
_temp2759.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2759;}); _temp2758;}); break; case 266: _LL2757: { struct Cyc_List_List*
_temp2761= Cyc_yyget_DeclList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); struct Cyc_Absyn_Stmt* _temp2762= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 5)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2763=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2763[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2764; _temp2764.tag= Cyc_Stmt_tok; _temp2764.f1= Cyc_Parse_flatten_declarations(
_temp2761, _temp2762); _temp2764;}); _temp2763;}); break;} case 267: _LL2760: {
struct Cyc_List_List* _temp2766= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2767= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2768=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2768[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2769; _temp2769.tag= Cyc_Stmt_tok; _temp2769.f1= Cyc_Parse_flatten_declarations(
_temp2766, _temp2767); _temp2769;}); _temp2768;}); break;} case 268: _LL2765: {
struct Cyc_List_List* _temp2771= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); struct Cyc_Absyn_Stmt*
_temp2772= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), Cyc_yyget_Stmt_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2773=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2773[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2774; _temp2774.tag= Cyc_Stmt_tok; _temp2774.f1= Cyc_Parse_flatten_declarations(
_temp2771, _temp2772); _temp2774;}); _temp2773;}); break;} case 269: _LL2770: {
struct Cyc_List_List* _temp2776= Cyc_yyget_DeclList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 5)]); struct Cyc_Absyn_Stmt*
_temp2777= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Stmt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 7)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); yyval=(
void*)({ struct Cyc_Stmt_tok_struct* _temp2778=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2778[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2779; _temp2779.tag= Cyc_Stmt_tok; _temp2779.f1= Cyc_Parse_flatten_declarations(
_temp2776, _temp2777); _temp2779;}); _temp2778;}); break;} case 270: _LL2775:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2781=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2781[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2782; _temp2782.tag= Cyc_Stmt_tok; _temp2782.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_arr* _temp2783=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2783[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2783;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2782;}); _temp2781;}); break; case 271: _LL2780: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2785=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2785[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2786; _temp2786.tag= Cyc_Stmt_tok; _temp2786.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2786;}); _temp2785;}); break; case 272:
_LL2784: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2788=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2788[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2789; _temp2789.tag= Cyc_Stmt_tok; _temp2789.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2789;}); _temp2788;}); break; case 273: _LL2787: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2791=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2791[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2792; _temp2792.tag= Cyc_Stmt_tok; _temp2792.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2792;}); _temp2791;}); break; case 274:
_LL2790: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2794=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2794[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2795; _temp2795.tag= Cyc_Stmt_tok; _temp2795.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).last_line));
_temp2795;}); _temp2794;}); break; case 275: _LL2793: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp2797=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp2797[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2798; _temp2798.tag= Cyc_Stmt_tok; _temp2798.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 1)]).last_line)); _temp2798;}); _temp2797;}); break; case 276:
_LL2796: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2800=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2800[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2801; _temp2801.tag= Cyc_Stmt_tok; _temp2801.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 3)]).last_line)); _temp2801;}); _temp2800;}); break; case 277:
_LL2799: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp2803=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp2803[ 0]=({ struct Cyc_Stmt_tok_struct
_temp2804; _temp2804.tag= Cyc_Stmt_tok; _temp2804.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).last_line));
_temp2804;}); _temp2803;}); break; case 278: _LL2802: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp2806=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp2806[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2807; _temp2807.tag= Cyc_Pattern_tok; _temp2807.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2807;});
_temp2806;}); break; case 279: _LL2805: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 280:
_LL2808: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2810=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2810[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2811; _temp2811.tag= Cyc_Pattern_tok; _temp2811.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp2812=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp2812[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp2813; _temp2813.tag= Cyc_Absyn_Int_p; _temp2813.f1=( void*)(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1; _temp2813.f2=(*
Cyc_yyget_Int_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2;
_temp2813;}); _temp2812;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2811;});
_temp2810;}); break; case 281: _LL2809: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2815=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2815[ 0]=({ struct Cyc_Pattern_tok_struct _temp2816; _temp2816.tag= Cyc_Pattern_tok;
_temp2816.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp2817=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp2817[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp2818; _temp2818.tag= Cyc_Absyn_Int_p;
_temp2818.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2818.f2= -(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2; _temp2818;});
_temp2817;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2816;}); _temp2815;}); break; case 282:
_LL2814: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2820=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2820[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2821; _temp2821.tag= Cyc_Pattern_tok; _temp2821.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp2822=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp2822[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp2823; _temp2823.tag= Cyc_Absyn_Float_p; _temp2823.f1=
Cyc_yyget_String_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]);
_temp2823;}); _temp2822;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2821;});
_temp2820;}); break; case 283: _LL2819: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2825=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2825[ 0]=({ struct Cyc_Pattern_tok_struct _temp2826; _temp2826.tag= Cyc_Pattern_tok;
_temp2826.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp2827=( struct Cyc_Absyn_Char_p_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_Char_p_struct)); _temp2827[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp2828; _temp2828.tag= Cyc_Absyn_Char_p; _temp2828.f1= Cyc_yyget_Char_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2828;});
_temp2827;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2826;}); _temp2825;}); break; case 284:
_LL2824: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2830=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2830[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2831; _temp2831.tag= Cyc_Pattern_tok; _temp2831.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2831;});
_temp2830;}); break; case 285: _LL2829: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2833=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2833[ 0]=({ struct Cyc_Pattern_tok_struct _temp2834; _temp2834.tag= Cyc_Pattern_tok;
_temp2834.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp2835=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp2835[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp2836; _temp2836.tag=
Cyc_Absyn_UnknownId_p; _temp2836.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2836;}); _temp2835;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp2834;}); _temp2833;}); break; case 286:
_LL2832: { struct Cyc_List_List* _temp2838=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2839=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2839[ 0]=({ struct Cyc_Pattern_tok_struct _temp2840; _temp2840.tag= Cyc_Pattern_tok;
_temp2840.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp2841=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp2841[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp2842; _temp2842.tag= Cyc_Absyn_UnknownCall_p; _temp2842.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2842.f2=
_temp2838; _temp2842.f3= Cyc_yyget_PatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2842;});
_temp2841;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2840;}); _temp2839;}); break;} case
287: _LL2837: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2844=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2844[ 0]=({ struct Cyc_Pattern_tok_struct _temp2845; _temp2845.tag= Cyc_Pattern_tok;
_temp2845.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp2846=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp2846[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp2847; _temp2847.tag= Cyc_Absyn_Tuple_p;
_temp2847.f1= Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 1)]); _temp2847;}); _temp2846;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2845;}); _temp2844;}); break; case 288: _LL2843: { struct Cyc_List_List*
_temp2849=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line), Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp2850=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp2850[ 0]=({ struct Cyc_Pattern_tok_struct
_temp2851; _temp2851.tag= Cyc_Pattern_tok; _temp2851.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp2852=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp2852[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp2853; _temp2853.tag= Cyc_Absyn_UnknownFields_p;
_temp2853.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp2853.f2= _temp2849; _temp2853.f3= 0; _temp2853;});
_temp2852;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2851;}); _temp2850;}); break;} case
289: _LL2848: { struct Cyc_List_List* _temp2855=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).last_line),
Cyc_yyget_TypeList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])); yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2856=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2856[ 0]=({ struct Cyc_Pattern_tok_struct _temp2857; _temp2857.tag= Cyc_Pattern_tok;
_temp2857.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp2858=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp2858[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp2859; _temp2859.tag= Cyc_Absyn_UnknownFields_p; _temp2859.f1= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp2859.f2=
_temp2855; _temp2859.f3= Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2859;});
_temp2858;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp2857;}); _temp2856;}); break;} case
290: _LL2854: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp2861=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2861[ 0]=({ struct Cyc_Pattern_tok_struct _temp2862; _temp2862.tag= Cyc_Pattern_tok;
_temp2862.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp2863=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp2863[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp2864; _temp2864.tag= Cyc_Absyn_Pointer_p;
_temp2864.f1= Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2864;}); _temp2863;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2862;});
_temp2861;}); break; case 291: _LL2860: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp2866=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp2866[ 0]=({ struct Cyc_Pattern_tok_struct _temp2867; _temp2867.tag= Cyc_Pattern_tok;
_temp2867.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp2868=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp2868[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp2869; _temp2869.tag=
Cyc_Absyn_Reference_p; _temp2869.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp2870=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2870->f1=(
void*) Cyc_Absyn_Loc_n; _temp2870->f2=({ struct _tagged_arr* _temp2871=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp2871[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2871;});
_temp2870;}),( void*) Cyc_Absyn_VoidType, 0); _temp2869;}); _temp2868;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2867;}); _temp2866;}); break; case 292: _LL2865: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp2873=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2873[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2874; _temp2874.tag= Cyc_PatternList_tok;
_temp2874.f1= 0; _temp2874;}); _temp2873;}); break; case 293: _LL2872: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2876=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2876[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2877; _temp2877.tag= Cyc_PatternList_tok;
_temp2877.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)])); _temp2877;}); _temp2876;}); break; case 294: _LL2875: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp2879=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2879[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2880; _temp2880.tag= Cyc_PatternList_tok;
_temp2880.f1=({ struct Cyc_List_List* _temp2881=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2881->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2881->tl= 0;
_temp2881;}); _temp2880;}); _temp2879;}); break; case 295: _LL2878: yyval=( void*)({
struct Cyc_PatternList_tok_struct* _temp2883=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp2883[ 0]=({ struct
Cyc_PatternList_tok_struct _temp2884; _temp2884.tag= Cyc_PatternList_tok;
_temp2884.f1=({ struct Cyc_List_List* _temp2885=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2885->hd=( void*) Cyc_yyget_Pattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2885->tl=
Cyc_yyget_PatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2885;}); _temp2884;}); _temp2883;}); break; case 296:
_LL2882: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2887=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2887[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2888; _temp2888.tag=
Cyc_FieldPattern_tok; _temp2888.f1=({ struct _tuple14* _temp2889=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2889->f1= 0; _temp2889->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2889;}); _temp2888;}); _temp2887;}); break; case 297:
_LL2886: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct* _temp2891=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp2891[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp2892; _temp2892.tag=
Cyc_FieldPattern_tok; _temp2892.f1=({ struct _tuple14* _temp2893=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2893->f1= Cyc_yyget_DesignatorList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp2893->f2=
Cyc_yyget_Pattern_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp2893;}); _temp2892;}); _temp2891;}); break; case 298:
_LL2890: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp2895=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp2895[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp2896; _temp2896.tag=
Cyc_FieldPatternList_tok; _temp2896.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp2896;});
_temp2895;}); break; case 299: _LL2894: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp2898=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp2898[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp2899; _temp2899.tag= Cyc_FieldPatternList_tok; _temp2899.f1=({ struct Cyc_List_List*
_temp2900=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2900->hd=( void*) Cyc_yyget_FieldPattern_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2900->tl= 0;
_temp2900;}); _temp2899;}); _temp2898;}); break; case 300: _LL2897: yyval=( void*)({
struct Cyc_FieldPatternList_tok_struct* _temp2902=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp2902[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp2903; _temp2903.tag= Cyc_FieldPatternList_tok;
_temp2903.f1=({ struct Cyc_List_List* _temp2904=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2904->hd=( void*) Cyc_yyget_FieldPattern_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp2904->tl=
Cyc_yyget_FieldPatternList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]); _temp2904;}); _temp2903;}); _temp2902;}); break; case 301:
_LL2901: yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)];
break; case 302: _LL2905: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2907=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2907[ 0]=({ struct Cyc_Exp_tok_struct _temp2908; _temp2908.tag= Cyc_Exp_tok;
_temp2908.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2908;}); _temp2907;}); break; case 303: _LL2906: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 304: _LL2909:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2911=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2911[ 0]=({ struct Cyc_Exp_tok_struct
_temp2912; _temp2912.tag= Cyc_Exp_tok; _temp2912.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Primopopt_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2912;}); _temp2911;}); break; case 305: _LL2910: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2914=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2914[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2915; _temp2915.tag= Cyc_Primopopt_tok; _temp2915.f1=
0; _temp2915;}); _temp2914;}); break; case 306: _LL2913: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2917=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2917[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2918; _temp2918.tag= Cyc_Primopopt_tok; _temp2918.f1=({
struct Cyc_Core_Opt* _temp2919=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2919->v=( void*)(( void*) Cyc_Absyn_Times); _temp2919;});
_temp2918;}); _temp2917;}); break; case 307: _LL2916: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2921=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2921[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2922; _temp2922.tag= Cyc_Primopopt_tok; _temp2922.f1=({
struct Cyc_Core_Opt* _temp2923=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2923->v=( void*)(( void*) Cyc_Absyn_Div); _temp2923;});
_temp2922;}); _temp2921;}); break; case 308: _LL2920: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2925=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2925[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2926; _temp2926.tag= Cyc_Primopopt_tok; _temp2926.f1=({
struct Cyc_Core_Opt* _temp2927=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2927->v=( void*)(( void*) Cyc_Absyn_Mod); _temp2927;});
_temp2926;}); _temp2925;}); break; case 309: _LL2924: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2929=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2929[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2930; _temp2930.tag= Cyc_Primopopt_tok; _temp2930.f1=({
struct Cyc_Core_Opt* _temp2931=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2931->v=( void*)(( void*) Cyc_Absyn_Plus); _temp2931;});
_temp2930;}); _temp2929;}); break; case 310: _LL2928: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2933=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2933[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2934; _temp2934.tag= Cyc_Primopopt_tok; _temp2934.f1=({
struct Cyc_Core_Opt* _temp2935=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2935->v=( void*)(( void*) Cyc_Absyn_Minus); _temp2935;});
_temp2934;}); _temp2933;}); break; case 311: _LL2932: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2937=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2937[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2938; _temp2938.tag= Cyc_Primopopt_tok; _temp2938.f1=({
struct Cyc_Core_Opt* _temp2939=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2939->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp2939;});
_temp2938;}); _temp2937;}); break; case 312: _LL2936: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2941=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2941[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2942; _temp2942.tag= Cyc_Primopopt_tok; _temp2942.f1=({
struct Cyc_Core_Opt* _temp2943=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2943->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp2943;});
_temp2942;}); _temp2941;}); break; case 313: _LL2940: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2945=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2945[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2946; _temp2946.tag= Cyc_Primopopt_tok; _temp2946.f1=({
struct Cyc_Core_Opt* _temp2947=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2947->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp2947;});
_temp2946;}); _temp2945;}); break; case 314: _LL2944: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2949=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2949[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2950; _temp2950.tag= Cyc_Primopopt_tok; _temp2950.f1=({
struct Cyc_Core_Opt* _temp2951=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2951->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp2951;});
_temp2950;}); _temp2949;}); break; case 315: _LL2948: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp2953=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp2953[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp2954; _temp2954.tag= Cyc_Primopopt_tok; _temp2954.f1=({
struct Cyc_Core_Opt* _temp2955=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2955->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp2955;});
_temp2954;}); _temp2953;}); break; case 316: _LL2952: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 317: _LL2956:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2958=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2958[ 0]=({ struct Cyc_Exp_tok_struct
_temp2959; _temp2959.tag= Cyc_Exp_tok; _temp2959.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
4)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2959;});
_temp2958;}); break; case 318: _LL2957: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2961=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2961[ 0]=({ struct Cyc_Exp_tok_struct _temp2962; _temp2962.tag= Cyc_Exp_tok;
_temp2962.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2962;}); _temp2961;}); break; case 319: _LL2960: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2964=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2964[ 0]=({ struct Cyc_Exp_tok_struct
_temp2965; _temp2965.tag= Cyc_Exp_tok; _temp2965.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp2965;}); _temp2964;}); break; case 320: _LL2963: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2967=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2967[ 0]=({ struct Cyc_Exp_tok_struct
_temp2968; _temp2968.tag= Cyc_Exp_tok; _temp2968.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset + 1)]).last_line));
_temp2968;}); _temp2967;}); break; case 321: _LL2966: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp2970=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp2970[ 0]=({ struct Cyc_Exp_tok_struct
_temp2971; _temp2971.tag= Cyc_Exp_tok; _temp2971.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2971;});
_temp2970;}); break; case 322: _LL2969: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp2973=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp2973[ 0]=({ struct Cyc_Exp_tok_struct _temp2974; _temp2974.tag= Cyc_Exp_tok;
_temp2974.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2974;}); _temp2973;}); break; case 323: _LL2972: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 324: _LL2975:
yyval= yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case
325: _LL2976: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2978=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2978[ 0]=({ struct Cyc_Exp_tok_struct
_temp2979; _temp2979.tag= Cyc_Exp_tok; _temp2979.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2979;}); _temp2978;}); break; case 326: _LL2977: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 327: _LL2980:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2982=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2982[ 0]=({ struct Cyc_Exp_tok_struct
_temp2983; _temp2983.tag= Cyc_Exp_tok; _temp2983.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2983;}); _temp2982;}); break; case 328: _LL2981: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 329: _LL2984:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2986=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2986[ 0]=({ struct Cyc_Exp_tok_struct
_temp2987; _temp2987.tag= Cyc_Exp_tok; _temp2987.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2987;});
_temp2986;}); break; case 330: _LL2985: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 331: _LL2988:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2990=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2990[ 0]=({ struct Cyc_Exp_tok_struct
_temp2991; _temp2991.tag= Cyc_Exp_tok; _temp2991.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2991;});
_temp2990;}); break; case 332: _LL2989: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 333: _LL2992:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2994=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2994[ 0]=({ struct Cyc_Exp_tok_struct
_temp2995; _temp2995.tag= Cyc_Exp_tok; _temp2995.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp2995;});
_temp2994;}); break; case 334: _LL2993: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 335: _LL2996:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp2998=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp2998[ 0]=({ struct Cyc_Exp_tok_struct
_temp2999; _temp2999.tag= Cyc_Exp_tok; _temp2999.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp2999;}); _temp2998;}); break; case 336: _LL2997: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3001=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3001[ 0]=({ struct Cyc_Exp_tok_struct
_temp3002; _temp3002.tag= Cyc_Exp_tok; _temp3002.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3002;}); _temp3001;}); break; case 337: _LL3000: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 338: _LL3003:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3005=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3005[ 0]=({ struct Cyc_Exp_tok_struct
_temp3006; _temp3006.tag= Cyc_Exp_tok; _temp3006.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3006;}); _temp3005;}); break; case 339: _LL3004: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3008=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3008[ 0]=({ struct Cyc_Exp_tok_struct
_temp3009; _temp3009.tag= Cyc_Exp_tok; _temp3009.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3009;}); _temp3008;}); break; case 340: _LL3007: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3011=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3011[ 0]=({ struct Cyc_Exp_tok_struct
_temp3012; _temp3012.tag= Cyc_Exp_tok; _temp3012.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3012;}); _temp3011;}); break; case 341: _LL3010: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3014=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3014[ 0]=({ struct Cyc_Exp_tok_struct
_temp3015; _temp3015.tag= Cyc_Exp_tok; _temp3015.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3015;}); _temp3014;}); break; case 342: _LL3013: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 343: _LL3016:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3018=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3018[ 0]=({ struct Cyc_Exp_tok_struct
_temp3019; _temp3019.tag= Cyc_Exp_tok; _temp3019.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3019;}); _temp3018;}); break; case 344: _LL3017: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3021=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3021[ 0]=({ struct Cyc_Exp_tok_struct
_temp3022; _temp3022.tag= Cyc_Exp_tok; _temp3022.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3022;}); _temp3021;}); break; case 345: _LL3020: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 346: _LL3023:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3025=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3025[ 0]=({ struct Cyc_Exp_tok_struct
_temp3026; _temp3026.tag= Cyc_Exp_tok; _temp3026.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3026;});
_temp3025;}); break; case 347: _LL3024: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3028=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3028[ 0]=({ struct Cyc_Exp_tok_struct _temp3029; _temp3029.tag= Cyc_Exp_tok;
_temp3029.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3029;}); _temp3028;}); break; case 348: _LL3027: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 349: _LL3030:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3032=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3032[ 0]=({ struct Cyc_Exp_tok_struct
_temp3033; _temp3033.tag= Cyc_Exp_tok; _temp3033.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3033;});
_temp3032;}); break; case 350: _LL3031: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3035=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3035[ 0]=({ struct Cyc_Exp_tok_struct _temp3036; _temp3036.tag= Cyc_Exp_tok;
_temp3036.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3036;}); _temp3035;}); break; case 351: _LL3034: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3038=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3038[ 0]=({ struct Cyc_Exp_tok_struct
_temp3039; _temp3039.tag= Cyc_Exp_tok; _temp3039.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 2)]), Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3039;});
_temp3038;}); break; case 352: _LL3037: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 353: _LL3040:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3042=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3042[ 0]=({ struct Cyc_Exp_tok_struct
_temp3043; _temp3043.tag= Cyc_Exp_tok; _temp3043.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f3, Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3043;}); _temp3042;}); break; case 354: _LL3041: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 355: _LL3044:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3046=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3046[ 0]=({ struct Cyc_Exp_tok_struct
_temp3047; _temp3047.tag= Cyc_Exp_tok; _temp3047.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3047;}); _temp3046;}); break; case 356: _LL3045: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3049=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3049[ 0]=({ struct Cyc_Exp_tok_struct
_temp3050; _temp3050.tag= Cyc_Exp_tok; _temp3050.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3050;}); _temp3049;}); break; case 357: _LL3048: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3052=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3052[ 0]=({ struct Cyc_Exp_tok_struct
_temp3053; _temp3053.tag= Cyc_Exp_tok; _temp3053.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3053;}); _temp3052;}); break; case 358: _LL3051: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3055=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3055[ 0]=({ struct Cyc_Exp_tok_struct
_temp3056; _temp3056.tag= Cyc_Exp_tok; _temp3056.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3056;}); _temp3055;}); break; case 359: _LL3054: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 360: _LL3057:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3059=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3059[ 0]=({ struct Cyc_Exp_tok_struct
_temp3060; _temp3060.tag= Cyc_Exp_tok; _temp3060.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3060;}); _temp3059;}); break; case 361: _LL3058: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3062=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3062[ 0]=({ struct Cyc_Exp_tok_struct
_temp3063; _temp3063.tag= Cyc_Exp_tok; _temp3063.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)])).f3, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3063;});
_temp3062;}); break; case 362: _LL3061: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3065=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3065[ 0]=({ struct Cyc_Exp_tok_struct _temp3066; _temp3066.tag= Cyc_Exp_tok;
_temp3066.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3066;}); _temp3065;}); break; case 363: _LL3064: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3068=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3068[ 0]=({ struct Cyc_Exp_tok_struct
_temp3069; _temp3069.tag= Cyc_Exp_tok; _temp3069.f1= Cyc_Absyn_offsetof_exp((*
Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)])).f3,({ struct _tagged_arr* _temp3070=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp3070[ 0]= Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3070;}),
Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull( 10000u,
yylsp_offset)]).last_line)); _temp3069;}); _temp3068;}); break; case 364:
_LL3067: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3072=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3072[ 0]=({ struct Cyc_Exp_tok_struct
_temp3073; _temp3073.tag= Cyc_Exp_tok; _temp3073.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3074=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3074[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3075; _temp3075.tag= Cyc_Absyn_Malloc_e;
_temp3075.f1= 0; _temp3075.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3075;}); _temp3074;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 6)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3073;});
_temp3072;}); break; case 365: _LL3071: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3077=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3077[ 0]=({ struct Cyc_Exp_tok_struct _temp3078; _temp3078.tag= Cyc_Exp_tok;
_temp3078.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3079=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3079[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3080; _temp3080.tag= Cyc_Absyn_Malloc_e;
_temp3080.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 6)]); _temp3080.f2=( void*)
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).last_line));
_temp3080;}); _temp3079;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 8)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3078;});
_temp3077;}); break; case 366: _LL3076: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3082=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3082[ 0]=({ struct Cyc_Primop_tok_struct _temp3083; _temp3083.tag= Cyc_Primop_tok;
_temp3083.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp3083;}); _temp3082;});
break; case 367: _LL3081: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3085=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3085[ 0]=({ struct Cyc_Primop_tok_struct _temp3086; _temp3086.tag= Cyc_Primop_tok;
_temp3086.f1=( void*)(( void*) Cyc_Absyn_Not); _temp3086;}); _temp3085;});
break; case 368: _LL3084: yyval=( void*)({ struct Cyc_Primop_tok_struct*
_temp3088=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp3088[ 0]=({ struct Cyc_Primop_tok_struct _temp3089; _temp3089.tag= Cyc_Primop_tok;
_temp3089.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp3089;}); _temp3088;});
break; case 369: _LL3087: yyval= yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]; break; case 370: _LL3090: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3092=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3092[ 0]=({ struct Cyc_Exp_tok_struct _temp3093; _temp3093.tag= Cyc_Exp_tok;
_temp3093.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 3)]), Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3093;}); _temp3092;}); break; case 371: _LL3091: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3095=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3095[ 0]=({ struct Cyc_Exp_tok_struct
_temp3096; _temp3096.tag= Cyc_Exp_tok; _temp3096.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]), 0, Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 2)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3096;}); _temp3095;}); break; case 372:
_LL3094: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3098=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3098[ 0]=({ struct Cyc_Exp_tok_struct
_temp3099; _temp3099.tag= Cyc_Exp_tok; _temp3099.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
3)]), Cyc_yyget_ExpList_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3099;});
_temp3098;}); break; case 373: _LL3097: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3101=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3101[ 0]=({ struct Cyc_Exp_tok_struct _temp3102; _temp3102.tag= Cyc_Exp_tok;
_temp3102.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3103=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3103[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3103;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3102;}); _temp3101;}); break; case 374: _LL3100: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3105=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3105[ 0]=({ struct Cyc_Exp_tok_struct
_temp3106; _temp3106.tag= Cyc_Exp_tok; _temp3106.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3106;});
_temp3105;}); break;} case 375: _LL3104: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3108=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3108[ 0]=({ struct Cyc_Exp_tok_struct _temp3109; _temp3109.tag= Cyc_Exp_tok;
_temp3109.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]),({ struct
_tagged_arr* _temp3110=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3110[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3110;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3109;}); _temp3108;}); break; case 376: _LL3107: { struct _tuple1* q= Cyc_yyget_QualId_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); if( Cyc_Absyn_is_qvar_qualified(
q)){ Cyc_Parse_err( _tag_arr("struct field name is qualified", sizeof(
unsigned char), 31u), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));} yyval=(
void*)({ struct Cyc_Exp_tok_struct* _temp3112=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3112[ 0]=({ struct Cyc_Exp_tok_struct
_temp3113; _temp3113.tag= Cyc_Exp_tok; _temp3113.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset -
2)]),(* q).f2, Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 2)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3113;});
_temp3112;}); break;} case 377: _LL3111: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3115=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3115[ 0]=({ struct Cyc_Exp_tok_struct _temp3116; _temp3116.tag= Cyc_Exp_tok;
_temp3116.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3116;}); _temp3115;}); break; case 378: _LL3114: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3118=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3118[ 0]=({ struct Cyc_Exp_tok_struct
_temp3119; _temp3119.tag= Cyc_Exp_tok; _temp3119.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3119;}); _temp3118;}); break; case 379: _LL3117: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3121=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3121[ 0]=({ struct Cyc_Exp_tok_struct
_temp3122; _temp3122.tag= Cyc_Exp_tok; _temp3122.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3123=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3123[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3124; _temp3124.tag= Cyc_Absyn_CompoundLit_e;
_temp3124.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 4)]); _temp3124.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3124;});
_temp3123;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 5)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3122;}); _temp3121;}); break; case 380:
_LL3120: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3126=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3126[ 0]=({ struct Cyc_Exp_tok_struct
_temp3127; _temp3127.tag= Cyc_Exp_tok; _temp3127.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp3128=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp3128[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp3129; _temp3129.tag= Cyc_Absyn_CompoundLit_e;
_temp3129.f1= Cyc_yyget_ParamDecl_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset - 5)]); _temp3129.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)])); _temp3129;});
_temp3128;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 6)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3127;}); _temp3126;}); break; case 381:
_LL3125: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3131=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3131[ 0]=({ struct Cyc_Exp_tok_struct
_temp3132; _temp3132.tag= Cyc_Exp_tok; _temp3132.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp3133=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp3133[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp3134; _temp3134.tag= Cyc_Absyn_Fill_e; _temp3134.f1= Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3134;});
_temp3133;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 3)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset)]).last_line)); _temp3132;}); _temp3131;}); break; case 382:
_LL3130: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3136=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3136[ 0]=({ struct Cyc_Exp_tok_struct
_temp3137; _temp3137.tag= Cyc_Exp_tok; _temp3137.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp3138=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp3138[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp3139; _temp3139.tag= Cyc_Absyn_Codegen_e;
_temp3139.f1= Cyc_yyget_FnDecl_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 1)]); _temp3139;}); _temp3138;}), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3137;}); _temp3136;}); break; case 383: _LL3135: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3141=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3141[ 0]=({ struct Cyc_Exp_tok_struct
_temp3142; _temp3142.tag= Cyc_Exp_tok; _temp3142.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp3143=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp3143[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp3144; _temp3144.tag= Cyc_Absyn_UnknownId_e;
_temp3144.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset)]); _temp3144;}); _temp3143;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3142;});
_temp3141;}); break; case 384: _LL3140: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; case 385: _LL3145:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3147=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3147[ 0]=({ struct Cyc_Exp_tok_struct
_temp3148; _temp3148.tag= Cyc_Exp_tok; _temp3148.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3148;});
_temp3147;}); break; case 386: _LL3146: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]; break; case 387:
_LL3149: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3151=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3151[ 0]=({ struct Cyc_Exp_tok_struct
_temp3152; _temp3152.tag= Cyc_Exp_tok; _temp3152.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp3153=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3153[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3154; _temp3154.tag=
Cyc_Absyn_UnknownId_e; _temp3154.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]); _temp3154;});
_temp3153;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 1)]).last_line)), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 1)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3152;});
_temp3151;}); break; case 388: _LL3150: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3156=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3156[ 0]=({ struct Cyc_Exp_tok_struct _temp3157; _temp3157.tag= Cyc_Exp_tok;
_temp3157.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp3158=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp3158[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp3159; _temp3159.tag=
Cyc_Absyn_UnknownId_e; _temp3159.f1= Cyc_yyget_QualId_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 4)]); _temp3159;});
_temp3158;}), Cyc_Position_segment_of_abs(( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).first_line,( yyls[ _check_known_subscript_notnull(
10000u, yylsp_offset - 4)]).last_line)), Cyc_List_imp_rev( Cyc_yyget_TypeList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3157;}); _temp3156;}); break; case 389: _LL3155: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3161=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3161[ 0]=({ struct Cyc_Exp_tok_struct
_temp3162; _temp3162.tag= Cyc_Exp_tok; _temp3162.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset - 1)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3162;}); _temp3161;}); break; case 390: _LL3160: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3164=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3164[ 0]=({ struct Cyc_Exp_tok_struct
_temp3165; _temp3165.tag= Cyc_Exp_tok; _temp3165.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp3166=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp3166[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp3167; _temp3167.tag= Cyc_Absyn_Struct_e;
_temp3167.f1= Cyc_yyget_QualId_tok( yyvs[ _check_known_subscript_notnull( 10000u,
yyvsp_offset - 3)]); _temp3167.f2= 0; _temp3167.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 1)])); _temp3167.f4= 0;
_temp3167;}); _temp3166;}), Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset - 3)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3165;});
_temp3164;}); break; case 391: _LL3163: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3169=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3169[ 0]=({ struct Cyc_Exp_tok_struct _temp3170; _temp3170.tag= Cyc_Exp_tok;
_temp3170.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset - 4)]).first_line,(
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line));
_temp3170;}); _temp3169;}); break; case 392: _LL3168: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp3172=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp3172[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3173; _temp3173.tag= Cyc_ExpList_tok; _temp3173.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)])); _temp3173;});
_temp3172;}); break; case 393: _LL3171: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp3175=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp3175[ 0]=({ struct Cyc_ExpList_tok_struct _temp3176; _temp3176.tag= Cyc_ExpList_tok;
_temp3176.f1=({ struct Cyc_List_List* _temp3177=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3177->hd=( void*) Cyc_yyget_Exp_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3177->tl= 0;
_temp3177;}); _temp3176;}); _temp3175;}); break; case 394: _LL3174: yyval=( void*)({
struct Cyc_ExpList_tok_struct* _temp3179=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp3179[ 0]=({ struct Cyc_ExpList_tok_struct
_temp3180; _temp3180.tag= Cyc_ExpList_tok; _temp3180.f1=({ struct Cyc_List_List*
_temp3181=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3181->hd=( void*) Cyc_yyget_Exp_tok( yyvs[ _check_known_subscript_notnull(
10000u, yyvsp_offset)]); _temp3181->tl= Cyc_yyget_ExpList_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset - 2)]); _temp3181;});
_temp3180;}); _temp3179;}); break; case 395: _LL3178: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3183=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3183[ 0]=({ struct Cyc_Exp_tok_struct
_temp3184; _temp3184.tag= Cyc_Exp_tok; _temp3184.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f1,(* Cyc_yyget_Int_tok(
yyvs[ _check_known_subscript_notnull( 10000u, yyvsp_offset)])).f2, Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3184;});
_temp3183;}); break; case 396: _LL3182: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3186=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3186[ 0]=({ struct Cyc_Exp_tok_struct _temp3187; _temp3187.tag= Cyc_Exp_tok;
_temp3187.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3187;});
_temp3186;}); break; case 397: _LL3185: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3189=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3189[ 0]=({ struct Cyc_Exp_tok_struct _temp3190; _temp3190.tag= Cyc_Exp_tok;
_temp3190.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]), Cyc_Position_segment_of_abs((
yyls[ _check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3190;});
_temp3189;}); break; case 398: _LL3188: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3192=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3192[ 0]=({ struct Cyc_Exp_tok_struct _temp3193; _temp3193.tag= Cyc_Exp_tok;
_temp3193.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).first_line,( yyls[
_check_known_subscript_notnull( 10000u, yylsp_offset)]).last_line)); _temp3193;});
_temp3192;}); break; case 399: _LL3191: yyval=( void*)({ struct Cyc_QualId_tok_struct*
_temp3195=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp3195[ 0]=({ struct Cyc_QualId_tok_struct _temp3196; _temp3196.tag= Cyc_QualId_tok;
_temp3196.f1=({ struct _tuple1* _temp3197=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3197->f1= Cyc_Absyn_rel_ns_null; _temp3197->f2=({ struct
_tagged_arr* _temp3198=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp3198[ 0]= Cyc_yyget_String_tok( yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]); _temp3198;}); _temp3197;});
_temp3196;}); _temp3195;}); break; case 400: _LL3194: yyval= yyvs[
_check_known_subscript_notnull( 10000u, yyvsp_offset)]; break; default: _LL3199:(
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
unsigned int _temp3201=( unsigned int)( sze + 15); unsigned char* _temp3202=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp3201); struct
_tagged_arr _temp3204= _tag_arr( _temp3202, sizeof( unsigned char),(
unsigned int)( sze + 15));{ unsigned int _temp3203= _temp3201; unsigned int i;
for( i= 0; i < _temp3203; i ++){ _temp3202[ i]='\000';}}; _temp3204;}); Cyc_String_strcpy(
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
goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp3214= v; struct
_tuple16* _temp3232; struct _tuple16 _temp3234; int _temp3235; unsigned char
_temp3237; short _temp3239; struct _tagged_arr _temp3241; struct Cyc_Core_Opt*
_temp3243; struct Cyc_Core_Opt* _temp3245; struct Cyc_Core_Opt _temp3247; struct
_tagged_arr* _temp3248; struct _tuple1* _temp3250; struct _tuple1 _temp3252;
struct _tagged_arr* _temp3253; void* _temp3255; _LL3216: if(*(( void**)
_temp3214) == Cyc_Int_tok){ _LL3233: _temp3232=(( struct Cyc_Int_tok_struct*)
_temp3214)->f1; _temp3234=* _temp3232; _LL3236: _temp3235= _temp3234.f2; goto
_LL3217;} else{ goto _LL3218;} _LL3218: if(*(( void**) _temp3214) == Cyc_Char_tok){
_LL3238: _temp3237=(( struct Cyc_Char_tok_struct*) _temp3214)->f1; goto _LL3219;}
else{ goto _LL3220;} _LL3220: if(*(( void**) _temp3214) == Cyc_Short_tok){
_LL3240: _temp3239=(( struct Cyc_Short_tok_struct*) _temp3214)->f1; goto _LL3221;}
else{ goto _LL3222;} _LL3222: if(*(( void**) _temp3214) == Cyc_String_tok){
_LL3242: _temp3241=(( struct Cyc_String_tok_struct*) _temp3214)->f1; goto
_LL3223;} else{ goto _LL3224;} _LL3224: if(*(( void**) _temp3214) == Cyc_Stringopt_tok){
_LL3244: _temp3243=(( struct Cyc_Stringopt_tok_struct*) _temp3214)->f1; if(
_temp3243 == 0){ goto _LL3225;} else{ goto _LL3226;}} else{ goto _LL3226;}
_LL3226: if(*(( void**) _temp3214) == Cyc_Stringopt_tok){ _LL3246: _temp3245=((
struct Cyc_Stringopt_tok_struct*) _temp3214)->f1; if( _temp3245 == 0){ goto
_LL3228;} else{ _temp3247=* _temp3245; _LL3249: _temp3248=( struct _tagged_arr*)
_temp3247.v; goto _LL3227;}} else{ goto _LL3228;} _LL3228: if(*(( void**)
_temp3214) == Cyc_QualId_tok){ _LL3251: _temp3250=(( struct Cyc_QualId_tok_struct*)
_temp3214)->f1; _temp3252=* _temp3250; _LL3256: _temp3255= _temp3252.f1; goto
_LL3254; _LL3254: _temp3253= _temp3252.f2; goto _LL3229;} else{ goto _LL3230;}
_LL3230: goto _LL3231; _LL3217:({ struct Cyc_Stdio_Int_pa_struct _temp3258;
_temp3258.tag= Cyc_Stdio_Int_pa; _temp3258.f1=( int)(( unsigned int) _temp3235);{
void* _temp3257[ 1u]={& _temp3258}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp3257, sizeof( void*),
1u));}}); goto _LL3215; _LL3219:({ struct Cyc_Stdio_Int_pa_struct _temp3260;
_temp3260.tag= Cyc_Stdio_Int_pa; _temp3260.f1=( int)(( unsigned int)(( int)
_temp3237));{ void* _temp3259[ 1u]={& _temp3260}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%c", sizeof( unsigned char), 3u), _tag_arr( _temp3259, sizeof( void*),
1u));}}); goto _LL3215; _LL3221:({ struct Cyc_Stdio_Int_pa_struct _temp3262;
_temp3262.tag= Cyc_Stdio_Int_pa; _temp3262.f1=( int)(( unsigned int)(( int)
_temp3239));{ void* _temp3261[ 1u]={& _temp3262}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%ds", sizeof( unsigned char), 4u), _tag_arr( _temp3261, sizeof( void*),
1u));}}); goto _LL3215; _LL3223:({ struct Cyc_Stdio_String_pa_struct _temp3264;
_temp3264.tag= Cyc_Stdio_String_pa; _temp3264.f1=( struct _tagged_arr) _temp3241;{
void* _temp3263[ 1u]={& _temp3264}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("\"%s\"", sizeof( unsigned char), 5u), _tag_arr( _temp3263, sizeof(
void*), 1u));}}); goto _LL3215; _LL3225:({ void* _temp3265[ 0u]={}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("null", sizeof( unsigned char), 5u), _tag_arr(
_temp3265, sizeof( void*), 0u));}); goto _LL3215; _LL3227:({ struct Cyc_Stdio_String_pa_struct
_temp3267; _temp3267.tag= Cyc_Stdio_String_pa; _temp3267.f1=( struct _tagged_arr)*
_temp3248;{ void* _temp3266[ 1u]={& _temp3267}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("\"%s\"", sizeof( unsigned char), 5u), _tag_arr( _temp3266, sizeof(
void*), 1u));}}); goto _LL3215; _LL3229: { struct Cyc_List_List* _temp3268= 0;{
void* _temp3269= _temp3255; struct Cyc_List_List* _temp3277; struct Cyc_List_List*
_temp3279; _LL3271: if(( unsigned int) _temp3269 > 1u?*(( int*) _temp3269) ==
Cyc_Absyn_Rel_n: 0){ _LL3278: _temp3277=(( struct Cyc_Absyn_Rel_n_struct*)
_temp3269)->f1; goto _LL3272;} else{ goto _LL3273;} _LL3273: if(( unsigned int)
_temp3269 > 1u?*(( int*) _temp3269) == Cyc_Absyn_Abs_n: 0){ _LL3280: _temp3279=((
struct Cyc_Absyn_Abs_n_struct*) _temp3269)->f1; goto _LL3274;} else{ goto
_LL3275;} _LL3275: if( _temp3269 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3276;}
else{ goto _LL3270;} _LL3272: _temp3268= _temp3277; goto _LL3270; _LL3274:
_temp3268= _temp3279; goto _LL3270; _LL3276: goto _LL3270; _LL3270:;} for( 0;
_temp3268 != 0; _temp3268=(( struct Cyc_List_List*) _check_null( _temp3268))->tl){({
struct Cyc_Stdio_String_pa_struct _temp3282; _temp3282.tag= Cyc_Stdio_String_pa;
_temp3282.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp3268))->hd);{ void* _temp3281[ 1u]={& _temp3282}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("%s::", sizeof( unsigned char), 5u), _tag_arr(
_temp3281, sizeof( void*), 1u));}});}({ struct Cyc_Stdio_String_pa_struct
_temp3284; _temp3284.tag= Cyc_Stdio_String_pa; _temp3284.f1=( struct _tagged_arr)*
_temp3253;{ void* _temp3283[ 1u]={& _temp3284}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s::", sizeof( unsigned char), 5u), _tag_arr( _temp3283, sizeof( void*),
1u));}}); goto _LL3215;} _LL3231:({ void* _temp3285[ 0u]={}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("?", sizeof( unsigned char), 2u), _tag_arr( _temp3285,
sizeof( void*), 0u));}); goto _LL3215; _LL3215:;} struct Cyc_List_List* Cyc_Parse_parse_file(
struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({
struct Cyc_Core_Opt* _temp3286=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3286->v=( void*) Cyc_Lexing_from_file( f); _temp3286;});
Cyc_yyparse(); return Cyc_Parse_parse_result;}
