 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _tagged_arr Cyc_Core_string_of_int( int); extern
unsigned char* string_to_Cstring( struct _tagged_arr); extern unsigned char*
underlying_Cstring( struct _tagged_arr); extern struct _tagged_arr
Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
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
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List* x);
extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); extern int Cyc_List_list_prefix( int(* cmp)( void*,
void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc*
d, int w, struct Cyc_Stdio___sFILE* f); extern struct _tagged_arr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern
struct Cyc_PP_Doc* Cyc_PP_line_doc(); extern struct Cyc_PP_Doc* Cyc_PP_text(
struct _tagged_arr s); extern struct Cyc_PP_Doc* Cyc_PP_textptr( struct
_tagged_arr* s); extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc*
d); extern struct Cyc_PP_Doc* Cyc_PP_cat( struct _tagged_arr); extern struct Cyc_PP_Doc*
Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct
Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_arr sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_seql( struct _tagged_arr sep, struct Cyc_List_List* l0);
extern struct Cyc_PP_Doc* Cyc_PP_group( struct _tagged_arr start, struct
_tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* ss); extern
struct Cyc_PP_Doc* Cyc_PP_egroup( struct _tagged_arr start, struct _tagged_arr
stop, struct _tagged_arr sep, struct Cyc_List_List* ss); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
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
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
24; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
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
_tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
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
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple0* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple0* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple0*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); struct Cyc_Buffer_t; extern int
Cyc_String_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2); extern
struct _tagged_arr Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*); struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r; extern struct
Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r; extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r; extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d);
extern struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct
_tagged_arr Cyc_Absynpp_kind2string( void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref*); extern struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_arr Cyc_Absynpp_decllist2string( struct
Cyc_List_List* tdl); extern struct _tagged_arr Cyc_Absynpp_prim2string( void* p);
extern struct _tagged_arr Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p);
extern struct _tagged_arr Cyc_Absynpp_scope2string( void* sc); extern struct
_tagged_arr Cyc_Absynpp_cyc_string; extern struct _tagged_arr* Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
_tagged_arr Cyc_Absynpp_char_escape( unsigned char); extern struct _tagged_arr
Cyc_Absynpp_string_escape( struct _tagged_arr); extern struct _tagged_arr Cyc_Absynpp_prim2str(
void* p); extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s);
struct _tuple3{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
extern struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct
_tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ;
extern struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t);
struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; extern
struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc( struct Cyc_List_List* cs); extern struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclauses2doc( struct Cyc_List_List* cs); extern struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern
struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp* e);
extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0*); extern
struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct _tuple0*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc(
void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc( int inprec, void* p,
struct Cyc_List_List* es); struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de);
extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual tq, void*
t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po); extern
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields);
static int Cyc_Absynpp_expand_typedefs; static int Cyc_Absynpp_qvar_to_Cids;
static unsigned char _temp0[ 4u]="Cyc"; struct _tagged_arr Cyc_Absynpp_cyc_string={
_temp0, _temp0, _temp0 + 4u}; struct _tagged_arr* Cyc_Absynpp_cyc_stringptr=&
Cyc_Absynpp_cyc_string; static int Cyc_Absynpp_add_cyc_prefix; static int Cyc_Absynpp_to_VC;
static int Cyc_Absynpp_decls_first; static int Cyc_Absynpp_rewrite_temp_tvars;
static int Cyc_Absynpp_print_all_tvars; static int Cyc_Absynpp_print_all_kinds;
static int Cyc_Absynpp_print_using_stmts; static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars; static int Cyc_Absynpp_use_curr_namespace;
static struct Cyc_List_List* Cyc_Absynpp_curr_namespace= 0; struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params* fs){ Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs; Cyc_Absynpp_qvar_to_Cids= fs->qvar_to_Cids; Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix; Cyc_Absynpp_to_VC= fs->to_VC; Cyc_Absynpp_decls_first= fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars= fs->rewrite_temp_tvars; Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars; Cyc_Absynpp_print_all_kinds= fs->print_all_kinds; Cyc_Absynpp_print_using_stmts=
fs->print_using_stmts; Cyc_Absynpp_print_externC_stmts= fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars= fs->print_full_evars; Cyc_Absynpp_use_curr_namespace=
fs->use_curr_namespace; Cyc_Absynpp_curr_namespace= fs->curr_namespace;} struct
Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={ 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1,
0}; struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={ 1, 0, 0, 0, 0, 1, 0, 0,
1, 1, 0, 1, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={ 1, 1, 1, 0, 1,
0, 0, 0, 0, 0, 0, 0, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={ 0,
0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}; static void Cyc_Absynpp_curr_namespace_add(
struct _tagged_arr* v){ Cyc_Absynpp_curr_namespace=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List* _temp1=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp1->hd=( void*) v; _temp1->tl= 0; _temp1;}));} static void
Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if((( struct Cyc_List_List*)
_check_null(* l))->tl == 0){* l= 0;} else{ Cyc_Absynpp_suppr_last(&(( struct Cyc_List_List*)
_check_null(* l))->tl);}} static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)(
struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_arr Cyc_Absynpp_char_escape( unsigned char c){ switch( c){ case '\a':
_LL2: return _tag_arr("\\a", sizeof( unsigned char), 3u); case '\b': _LL3:
return _tag_arr("\\b", sizeof( unsigned char), 3u); case '\f': _LL4: return
_tag_arr("\\f", sizeof( unsigned char), 3u); case '\n': _LL5: return _tag_arr("\\n",
sizeof( unsigned char), 3u); case '\r': _LL6: return _tag_arr("\\r", sizeof(
unsigned char), 3u); case '\t': _LL7: return _tag_arr("\\t", sizeof(
unsigned char), 3u); case '\v': _LL8: return _tag_arr("\\v", sizeof(
unsigned char), 3u); case '\\': _LL9: return _tag_arr("\\\\", sizeof(
unsigned char), 3u); case '"': _LL10: return _tag_arr("\"", sizeof(
unsigned char), 2u); case '\'': _LL11: return _tag_arr("\\'", sizeof(
unsigned char), 3u); default: _LL12: if( c >=' '? c <='~': 0){ struct
_tagged_arr _temp14= Cyc_Core_new_string( 1);*(( unsigned char*)
_check_unknown_subscript( _temp14, sizeof( unsigned char), 0))= c; return(
struct _tagged_arr) _temp14;} else{ struct _tagged_arr _temp15= Cyc_Core_new_string(
4); int j= 0;*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript(
_temp15, sizeof( unsigned char), j ++))=( unsigned char)('0' +(( unsigned char)
c >> 6 & 7));*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))=( unsigned char)('0' +( c >> 3 & 7));*(( unsigned char*)
_check_unknown_subscript( _temp15, sizeof( unsigned char), j ++))=(
unsigned char)('0' +( c & 7)); return( struct _tagged_arr) _temp15;}}} static
int Cyc_Absynpp_special( struct _tagged_arr s){ int sz=( int)( _get_arr_size( s,
sizeof( unsigned char)) - 1);{ int i= 0; for( 0; i < sz; i ++){ unsigned char c=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
if((( c <=' '? 1: c >='~')? 1: c =='"')? 1: c =='\\'){ return 1;}}} return 0;}
struct _tagged_arr Cyc_Absynpp_string_escape( struct _tagged_arr s){ if( ! Cyc_Absynpp_special(
s)){ return s;}{ int n=( int)( _get_arr_size( s, sizeof( unsigned char)) - 1);
if( n > 0?*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), n)) =='\000': 0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n;
i ++){ unsigned char _temp16=*(( const unsigned char*) _check_unknown_subscript(
s, sizeof( unsigned char), i)); _LL18: if( _temp16 =='\a'){ goto _LL19;} else{
goto _LL20;} _LL20: if( _temp16 =='\b'){ goto _LL21;} else{ goto _LL22;} _LL22:
if( _temp16 =='\f'){ goto _LL23;} else{ goto _LL24;} _LL24: if( _temp16 =='\n'){
goto _LL25;} else{ goto _LL26;} _LL26: if( _temp16 =='\r'){ goto _LL27;} else{
goto _LL28;} _LL28: if( _temp16 =='\t'){ goto _LL29;} else{ goto _LL30;} _LL30:
if( _temp16 =='\v'){ goto _LL31;} else{ goto _LL32;} _LL32: if( _temp16 =='\\'){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp16 =='"'){ goto _LL35;} else{
goto _LL36;} _LL36: goto _LL37; _LL19: goto _LL21; _LL21: goto _LL23; _LL23:
goto _LL25; _LL25: goto _LL27; _LL27: goto _LL29; _LL29: goto _LL31; _LL31: goto
_LL33; _LL33: goto _LL35; _LL35: len += 2; goto _LL17; _LL37: if( _temp16 >=' '?
_temp16 <='~': 0){ len ++;} else{ len += 4;} goto _LL17; _LL17:;}}{ struct
_tagged_arr t= Cyc_Core_new_string( len); int j= 0;{ int i= 0; for( 0; i <= n; i
++){ unsigned char _temp38=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); _LL40: if( _temp38 =='\a'){ goto _LL41;} else{ goto
_LL42;} _LL42: if( _temp38 =='\b'){ goto _LL43;} else{ goto _LL44;} _LL44: if(
_temp38 =='\f'){ goto _LL45;} else{ goto _LL46;} _LL46: if( _temp38 =='\n'){
goto _LL47;} else{ goto _LL48;} _LL48: if( _temp38 =='\r'){ goto _LL49;} else{
goto _LL50;} _LL50: if( _temp38 =='\t'){ goto _LL51;} else{ goto _LL52;} _LL52:
if( _temp38 =='\v'){ goto _LL53;} else{ goto _LL54;} _LL54: if( _temp38 =='\\'){
goto _LL55;} else{ goto _LL56;} _LL56: if( _temp38 =='"'){ goto _LL57;} else{
goto _LL58;} _LL58: goto _LL59; _LL41:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='a';
goto _LL39; _LL43:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='b'; goto _LL39; _LL45:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='f';
goto _LL39; _LL47:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='n'; goto _LL39; _LL49:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='r';
goto _LL39; _LL51:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='t'; goto _LL39; _LL53:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='v';
goto _LL39; _LL55:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='\\'; goto _LL39; _LL57:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='"';
goto _LL39; _LL59: if( _temp38 >=' '? _temp38 <='~': 0){*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))= _temp38;} else{*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))=(
unsigned char)('0' +( _temp38 >> 6 & 7));*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))=( unsigned char)('0'
+( _temp38 >> 3 & 7));*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))=( unsigned char)('0' +( _temp38 & 7));} goto _LL39; _LL39:;}}
return( struct _tagged_arr) t;}}}} static unsigned char _temp60[ 9u]="restrict";
static struct _tagged_arr Cyc_Absynpp_restrict_string={ _temp60, _temp60,
_temp60 + 9u}; static unsigned char _temp61[ 9u]="volatile"; static struct
_tagged_arr Cyc_Absynpp_volatile_string={ _temp61, _temp61, _temp61 + 9u};
static unsigned char _temp62[ 6u]="const"; static struct _tagged_arr Cyc_Absynpp_const_str={
_temp62, _temp62, _temp62 + 6u}; static struct _tagged_arr* Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string; static struct _tagged_arr* Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string; static struct _tagged_arr* Cyc_Absynpp_const_sp=&
Cyc_Absynpp_const_str; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual
tq){ struct Cyc_List_List* l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List*
_temp63=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp63->hd=( void*) Cyc_Absynpp_restrict_sp; _temp63->tl= l; _temp63;});} if(
tq.q_volatile){ l=({ struct Cyc_List_List* _temp64=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp64->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp64->tl= l; _temp64;});} if( tq.q_const){ l=({ struct Cyc_List_List* _temp65=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp65->hd=(
void*) Cyc_Absynpp_const_sp; _temp65->tl= l; _temp65;});} return Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char),
2u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, l));} struct _tagged_arr Cyc_Absynpp_kind2string( void* k){ void*
_temp66= k; _LL68: if( _temp66 ==( void*) Cyc_Absyn_AnyKind){ goto _LL69;} else{
goto _LL70;} _LL70: if( _temp66 ==( void*) Cyc_Absyn_MemKind){ goto _LL71;}
else{ goto _LL72;} _LL72: if( _temp66 ==( void*) Cyc_Absyn_BoxKind){ goto _LL73;}
else{ goto _LL74;} _LL74: if( _temp66 ==( void*) Cyc_Absyn_RgnKind){ goto _LL75;}
else{ goto _LL76;} _LL76: if( _temp66 ==( void*) Cyc_Absyn_EffKind){ goto _LL77;}
else{ goto _LL67;} _LL69: return _tag_arr("A", sizeof( unsigned char), 2u);
_LL71: return _tag_arr("M", sizeof( unsigned char), 2u); _LL73: return _tag_arr("B",
sizeof( unsigned char), 2u); _LL75: return _tag_arr("R", sizeof( unsigned char),
2u); _LL77: return _tag_arr("E", sizeof( unsigned char), 2u); _LL67:;} struct
_tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void* _temp78=(
void*)( Cyc_Absyn_compress_conref( c))->v; void* _temp84; _LL80: if((
unsigned int) _temp78 > 1u?*(( int*) _temp78) == Cyc_Absyn_Eq_constr: 0){ _LL85:
_temp84=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp78)->f1; goto _LL81;}
else{ goto _LL82;} _LL82: goto _LL83; _LL81: return Cyc_Absynpp_kind2string(
_temp84); _LL83: return _tag_arr("?", sizeof( unsigned char), 2u); _LL79:;}
struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string(
k));} struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){
return Cyc_PP_text( Cyc_Absynpp_ckind2string( c));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc(
struct Cyc_List_List* ts){ return Cyc_PP_egroup( _tag_arr("<", sizeof(
unsigned char), 2u), _tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));}
struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void*
_temp86=( void*)( Cyc_Absyn_compress_conref( tv->kind))->v; void* _temp94; void*
_temp96; _LL88: if(( unsigned int) _temp86 > 1u?*(( int*) _temp86) == Cyc_Absyn_Eq_constr:
0){ _LL95: _temp94=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp86)->f1;
if( _temp94 ==( void*) Cyc_Absyn_BoxKind){ goto _LL89;} else{ goto _LL90;}}
else{ goto _LL90;} _LL90: if(( unsigned int) _temp86 > 1u?*(( int*) _temp86) ==
Cyc_Absyn_Eq_constr: 0){ _LL97: _temp96=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp86)->f1; goto _LL91;} else{ goto _LL92;} _LL92: goto _LL93; _LL89: return
Cyc_PP_textptr( tv->name); _LL91: return({ struct Cyc_PP_Doc* _temp98[ 3u];
_temp98[ 2u]= Cyc_Absynpp_kind2doc( _temp96); _temp98[ 1u]= Cyc_PP_text(
_tag_arr("::", sizeof( unsigned char), 3u)); _temp98[ 0u]= Cyc_PP_textptr( tv->name);
Cyc_PP_cat( _tag_arr( _temp98, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL93:
return({ struct Cyc_PP_Doc* _temp99[ 2u]; _temp99[ 1u]= Cyc_PP_text( _tag_arr("/*::?*/",
sizeof( unsigned char), 8u)); _temp99[ 0u]= Cyc_PP_textptr( tv->name); Cyc_PP_cat(
_tag_arr( _temp99, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL87:;} struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc( struct Cyc_List_List* tvs){ return Cyc_PP_egroup(
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_arr* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_PP_egroup( _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_PP_textptr,(( struct Cyc_List_List*(*)( struct
_tagged_arr*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_get_name, tvs)));} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_noncallatt2doc(
void* att){ void* _temp100= att; _LL102: if( _temp100 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL103;} else{ goto _LL104;} _LL104: if( _temp100 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL105;} else{ goto _LL106;} _LL106: if( _temp100 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL107;} else{ goto _LL108;} _LL108: goto _LL109; _LL103: return Cyc_PP_nil_doc();
_LL105: return Cyc_PP_nil_doc(); _LL107: return Cyc_PP_nil_doc(); _LL109: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL101:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp110=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL112: if( _temp110 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp110 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp110 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL117;} else{ goto _LL118;} _LL118: goto _LL119; _LL113: return Cyc_PP_text(
_tag_arr(" _stdcall ", sizeof( unsigned char), 11u)); _LL115: return Cyc_PP_text(
_tag_arr(" _cdecl ", sizeof( unsigned char), 9u)); _LL117: return Cyc_PP_text(
_tag_arr(" _fastcall ", sizeof( unsigned char), 12u)); _LL119: goto _LL111;
_LL111:;} return Cyc_PP_nil_doc();} struct Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 != 0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){
void* _temp120=( void*)(( struct Cyc_List_List*) _check_null( atts2))->hd;
_LL122: if( _temp120 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL123;} else{ goto
_LL124;} _LL124: if( _temp120 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL125;}
else{ goto _LL126;} _LL126: if( _temp120 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL127;} else{ goto _LL128;} _LL128: goto _LL129; _LL123: goto _LL121;
_LL125: goto _LL121; _LL127: goto _LL121; _LL129: hasatt= 1; goto _LL121; _LL121:;}}
if( ! hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc* _temp130[ 3u];
_temp130[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp130[
1u]= Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Absynpp_noncallatt2doc, atts)); _temp130[ 0u]= Cyc_PP_text(
_tag_arr("__declspec(", sizeof( unsigned char), 12u)); Cyc_PP_cat( _tag_arr(
_temp130, sizeof( struct Cyc_PP_Doc*), 3u));});} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();}
if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc( atts);} return({
struct Cyc_PP_Doc* _temp131[ 2u]; _temp131[ 1u]= Cyc_PP_group( _tag_arr("((",
sizeof( unsigned char), 3u), _tag_arr("))", sizeof( unsigned char), 3u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc,
atts)); _temp131[ 0u]= Cyc_PP_text( _tag_arr(" __attribute__", sizeof(
unsigned char), 15u)); Cyc_PP_cat( _tag_arr( _temp131, sizeof( struct Cyc_PP_Doc*),
2u));});} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List* tms){ if( tms
== 0){ return 0;}{ void* _temp132=( void*)(( struct Cyc_List_List*) _check_null(
tms))->hd; _LL134: if(( unsigned int) _temp132 > 1u?*(( int*) _temp132) == Cyc_Absyn_Pointer_mod:
0){ goto _LL135;} else{ goto _LL136;} _LL136: if(( unsigned int) _temp132 > 1u?*((
int*) _temp132) == Cyc_Absyn_Attributes_mod: 0){ goto _LL137;} else{ goto _LL138;}
_LL138: goto _LL139; _LL135: return 1; _LL137: if( ! Cyc_Absynpp_to_VC){ return
0;} return Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null(
tms))->tl); _LL139: return 0; _LL133:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,(( struct Cyc_List_List*) _check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=({
struct Cyc_PP_Doc* _temp222[ 3u]; _temp222[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp222[ 1u]= rest; _temp222[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp222,
sizeof( struct Cyc_PP_Doc*), 3u));}); void* _temp140=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp154; void* _temp156; struct
Cyc_List_List* _temp158; int _temp160; struct Cyc_Position_Segment* _temp162;
struct Cyc_List_List* _temp164; struct Cyc_Absyn_Tqual _temp166; void* _temp168;
void* _temp170; _LL142: if( _temp140 ==( void*) Cyc_Absyn_Carray_mod){ goto
_LL143;} else{ goto _LL144;} _LL144: if(( unsigned int) _temp140 > 1u?*(( int*)
_temp140) == Cyc_Absyn_ConstArray_mod: 0){ _LL155: _temp154=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp140)->f1; goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int)
_temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_Function_mod: 0){ _LL157:
_temp156=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp140)->f1; goto
_LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp140 > 1u?*(( int*)
_temp140) == Cyc_Absyn_Attributes_mod: 0){ _LL159: _temp158=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp140)->f2; goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int)
_temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_TypeParams_mod: 0){ _LL165:
_temp164=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f1; goto _LL163;
_LL163: _temp162=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f2; goto
_LL161; _LL161: _temp160=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f3;
goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp140 > 1u?*((
int*) _temp140) == Cyc_Absyn_Pointer_mod: 0){ _LL171: _temp170=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp140)->f1; goto _LL169; _LL169: _temp168=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f2; goto _LL167;
_LL167: _temp166=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f3; goto
_LL153;} else{ goto _LL141;} _LL143: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*
_temp172[ 2u]; _temp172[ 1u]= Cyc_PP_text( _tag_arr("[]", sizeof( unsigned char),
3u)); _temp172[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp172, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL145: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc* _temp173[ 4u];
_temp173[ 3u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char), 2u)); _temp173[
2u]= Cyc_Absynpp_exp2doc( _temp154); _temp173[ 1u]= Cyc_PP_text( _tag_arr("[",
sizeof( unsigned char), 2u)); _temp173[ 0u]= rest; Cyc_PP_cat( _tag_arr(
_temp173, sizeof( struct Cyc_PP_Doc*), 4u));}); _LL147: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;}{ void* _temp174=
_temp156; struct Cyc_List_List* _temp180; struct Cyc_Core_Opt* _temp182; struct
Cyc_Absyn_VarargInfo* _temp184; int _temp186; struct Cyc_List_List* _temp188;
struct Cyc_Position_Segment* _temp190; struct Cyc_List_List* _temp192; _LL176:
if(*(( int*) _temp174) == Cyc_Absyn_WithTypes){ _LL189: _temp188=(( struct Cyc_Absyn_WithTypes_struct*)
_temp174)->f1; goto _LL187; _LL187: _temp186=(( struct Cyc_Absyn_WithTypes_struct*)
_temp174)->f2; goto _LL185; _LL185: _temp184=(( struct Cyc_Absyn_WithTypes_struct*)
_temp174)->f3; goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_WithTypes_struct*)
_temp174)->f4; goto _LL181; _LL181: _temp180=(( struct Cyc_Absyn_WithTypes_struct*)
_temp174)->f5; goto _LL177;} else{ goto _LL178;} _LL178: if(*(( int*) _temp174)
== Cyc_Absyn_NoTypes){ _LL193: _temp192=(( struct Cyc_Absyn_NoTypes_struct*)
_temp174)->f1; goto _LL191; _LL191: _temp190=(( struct Cyc_Absyn_NoTypes_struct*)
_temp174)->f2; goto _LL179;} else{ goto _LL175;} _LL177: return({ struct Cyc_PP_Doc*
_temp194[ 2u]; _temp194[ 1u]= Cyc_Absynpp_funargs2doc( _temp188, _temp186,
_temp184, _temp182, _temp180); _temp194[ 0u]= rest; Cyc_PP_cat( _tag_arr(
_temp194, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL179: return({ struct Cyc_PP_Doc*
_temp195[ 2u]; _temp195[ 1u]= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char),
2u), _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr, _temp192));
_temp195[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp195, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL175:;} _LL149: if( ! Cyc_Absynpp_to_VC){ if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc* _temp196[ 2u]; _temp196[ 1u]= Cyc_Absynpp_atts2doc( _temp158);
_temp196[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp196, sizeof( struct Cyc_PP_Doc*),
2u));});} else{ if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ return({ struct Cyc_PP_Doc* _temp197[ 2u]; _temp197[ 1u]=
rest; _temp197[ 0u]= Cyc_Absynpp_callconv2doc( _temp158); Cyc_PP_cat( _tag_arr(
_temp197, sizeof( struct Cyc_PP_Doc*), 2u));});} return rest;} _LL151: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} if( _temp160){
return({ struct Cyc_PP_Doc* _temp198[ 2u]; _temp198[ 1u]= Cyc_Absynpp_ktvars2doc(
_temp164); _temp198[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp198, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp199[ 2u];
_temp199[ 1u]= Cyc_Absynpp_tvars2doc( _temp164); _temp199[ 0u]= rest; Cyc_PP_cat(
_tag_arr( _temp199, sizeof( struct Cyc_PP_Doc*), 2u));});} _LL153: { struct Cyc_PP_Doc*
ptr;{ void* _temp200= _temp170; struct Cyc_Absyn_Exp* _temp208; struct Cyc_Absyn_Exp*
_temp210; _LL202: if(( unsigned int) _temp200 > 1u?*(( int*) _temp200) == Cyc_Absyn_Nullable_ps:
0){ _LL209: _temp208=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp200)->f1;
goto _LL203;} else{ goto _LL204;} _LL204: if(( unsigned int) _temp200 > 1u?*((
int*) _temp200) == Cyc_Absyn_NonNullable_ps: 0){ _LL211: _temp210=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp200)->f1; goto _LL205;} else{ goto _LL206;} _LL206: if( _temp200 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL207;} else{ goto _LL201;} _LL203: if( Cyc_Evexp_eval_const_uint_exp(
_temp208) == 1){ ptr= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc* _temp212[ 4u]; _temp212[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp212[ 2u]= Cyc_Absynpp_exp2doc(
_temp208); _temp212[ 1u]= Cyc_PP_text( _tag_arr("{", sizeof( unsigned char), 2u));
_temp212[ 0u]= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp212, sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL201; _LL205:
if( Cyc_Evexp_eval_const_uint_exp( _temp210) == 1){ ptr= Cyc_PP_text( _tag_arr("@",
sizeof( unsigned char), 2u));} else{ ptr=({ struct Cyc_PP_Doc* _temp213[ 4u];
_temp213[ 3u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp213[
2u]= Cyc_Absynpp_exp2doc( _temp210); _temp213[ 1u]= Cyc_PP_text( _tag_arr("{",
sizeof( unsigned char), 2u)); _temp213[ 0u]= Cyc_PP_text( _tag_arr("@", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp213, sizeof( struct Cyc_PP_Doc*),
4u));});} goto _LL201; _LL207: ptr= Cyc_PP_text( _tag_arr("?", sizeof(
unsigned char), 2u)); goto _LL201; _LL201:;}{ void* _temp214= Cyc_Tcutil_compress(
_temp168); _LL216: if( _temp214 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL217;}
else{ goto _LL218;} _LL218: goto _LL219; _LL217: return({ struct Cyc_PP_Doc*
_temp220[ 2u]; _temp220[ 1u]= rest; _temp220[ 0u]= ptr; Cyc_PP_cat( _tag_arr(
_temp220, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL219: return({ struct Cyc_PP_Doc*
_temp221[ 4u]; _temp221[ 3u]= rest; _temp221[ 2u]= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); _temp221[ 1u]= Cyc_Absynpp_ntyp2doc( _temp168);
_temp221[ 0u]= ptr; Cyc_PP_cat( _tag_arr( _temp221, sizeof( struct Cyc_PP_Doc*),
4u));}); _LL215:;}} _LL141:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){
void* _temp223= Cyc_Tcutil_compress( t); _LL225: if( _temp223 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL226;} else{ goto _LL227;} _LL227: goto _LL228; _LL226: return Cyc_PP_text(
_tag_arr("`H", sizeof( unsigned char), 3u)); _LL228: return Cyc_Absynpp_ntyp2doc(
t); _LL224:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp229= Cyc_Tcutil_compress(
t); void* _temp237; struct Cyc_List_List* _temp239; _LL231: if(( unsigned int)
_temp229 > 4u?*(( int*) _temp229) == Cyc_Absyn_AccessEff: 0){ _LL238: _temp237=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp229)->f1; goto _LL232;} else{
goto _LL233;} _LL233: if(( unsigned int) _temp229 > 4u?*(( int*) _temp229) ==
Cyc_Absyn_JoinEff: 0){ _LL240: _temp239=(( struct Cyc_Absyn_JoinEff_struct*)
_temp229)->f1; goto _LL234;} else{ goto _LL235;} _LL235: goto _LL236; _LL232:*
regions=({ struct Cyc_List_List* _temp241=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp241->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp237); _temp241->tl=* regions; _temp241;}); goto _LL230; _LL234: for( 0;
_temp239 != 0; _temp239=(( struct Cyc_List_List*) _check_null( _temp239))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp239))->hd);} goto _LL230; _LL236:* effects=({ struct Cyc_List_List*
_temp242=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp242->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp242->tl=* effects; _temp242;});
goto _LL230; _LL230:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("+", sizeof( unsigned char), 2u), effects);} else{ struct Cyc_PP_Doc*
_temp243= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u), regions);
return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("+", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
effects,({ struct Cyc_List_List* _temp244=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp244->hd=( void*) _temp243; _temp244->tl= 0;
_temp244;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp245= t; struct Cyc_Core_Opt* _temp291; int _temp293; struct Cyc_Core_Opt*
_temp295; struct Cyc_Core_Opt* _temp297; struct Cyc_Absyn_Tvar* _temp299; struct
Cyc_Absyn_TunionInfo _temp301; void* _temp303; struct Cyc_List_List* _temp305;
void* _temp307; struct Cyc_Absyn_TunionFieldInfo _temp309; struct Cyc_List_List*
_temp311; void* _temp313; void* _temp315; void* _temp317; struct Cyc_List_List*
_temp319; struct Cyc_List_List* _temp321; struct _tuple0* _temp323; struct Cyc_List_List*
_temp325; struct _tuple0* _temp327; struct Cyc_List_List* _temp329; struct Cyc_List_List*
_temp331; struct _tuple0* _temp333; struct Cyc_Core_Opt* _temp335; struct Cyc_List_List*
_temp337; struct _tuple0* _temp339; void* _temp341; _LL247: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_ArrayType: 0){ goto _LL248;}
else{ goto _LL249;} _LL249: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_FnType: 0){ goto _LL250;} else{ goto _LL251;} _LL251: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_PointerType: 0){
goto _LL252;} else{ goto _LL253;} _LL253: if( _temp245 ==( void*) Cyc_Absyn_VoidType){
goto _LL254;} else{ goto _LL255;} _LL255: if(( unsigned int) _temp245 > 4u?*((
int*) _temp245) == Cyc_Absyn_Evar: 0){ _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f1; goto _LL296; _LL296: _temp295=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f2; goto _LL294; _LL294: _temp293=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f3; goto _LL292; _LL292: _temp291=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f4; goto _LL256;} else{ goto _LL257;} _LL257: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_VarType: 0){ _LL300: _temp299=((
struct Cyc_Absyn_VarType_struct*) _temp245)->f1; goto _LL258;} else{ goto _LL259;}
_LL259: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TunionType:
0){ _LL302: _temp301=(( struct Cyc_Absyn_TunionType_struct*) _temp245)->f1;
_LL308: _temp307=( void*) _temp301.tunion_info; goto _LL306; _LL306: _temp305=
_temp301.targs; goto _LL304; _LL304: _temp303=( void*) _temp301.rgn; goto _LL260;}
else{ goto _LL261;} _LL261: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_TunionFieldType: 0){ _LL310: _temp309=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp245)->f1; _LL314: _temp313=( void*) _temp309.field_info; goto _LL312;
_LL312: _temp311= _temp309.targs; goto _LL262;} else{ goto _LL263;} _LL263: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_IntType: 0){ _LL318:
_temp317=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp245)->f1; goto _LL316;
_LL316: _temp315=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp245)->f2;
goto _LL264;} else{ goto _LL265;} _LL265: if( _temp245 ==( void*) Cyc_Absyn_FloatType){
goto _LL266;} else{ goto _LL267;} _LL267: if( _temp245 ==( void*) Cyc_Absyn_DoubleType){
goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int) _temp245 > 4u?*((
int*) _temp245) == Cyc_Absyn_TupleType: 0){ _LL320: _temp319=(( struct Cyc_Absyn_TupleType_struct*)
_temp245)->f1; goto _LL270;} else{ goto _LL271;} _LL271: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_StructType: 0){ _LL324: _temp323=((
struct Cyc_Absyn_StructType_struct*) _temp245)->f1; goto _LL322; _LL322:
_temp321=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f2; goto _LL272;}
else{ goto _LL273;} _LL273: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_UnionType: 0){ _LL328: _temp327=(( struct Cyc_Absyn_UnionType_struct*)
_temp245)->f1; goto _LL326; _LL326: _temp325=(( struct Cyc_Absyn_UnionType_struct*)
_temp245)->f2; goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_AnonStructType: 0){ _LL330:
_temp329=(( struct Cyc_Absyn_AnonStructType_struct*) _temp245)->f1; goto _LL276;}
else{ goto _LL277;} _LL277: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_AnonUnionType: 0){ _LL332: _temp331=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp245)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_EnumType: 0){ _LL334: _temp333=((
struct Cyc_Absyn_EnumType_struct*) _temp245)->f1; goto _LL280;} else{ goto
_LL281;} _LL281: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TypedefType:
0){ _LL340: _temp339=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f1;
goto _LL338; _LL338: _temp337=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f2;
goto _LL336; _LL336: _temp335=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f3;
goto _LL282;} else{ goto _LL283;} _LL283: if(( unsigned int) _temp245 > 4u?*((
int*) _temp245) == Cyc_Absyn_RgnHandleType: 0){ _LL342: _temp341=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp245)->f1; goto _LL284;} else{ goto
_LL285;} _LL285: if( _temp245 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL286;} else{
goto _LL287;} _LL287: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245) ==
Cyc_Absyn_AccessEff: 0){ goto _LL288;} else{ goto _LL289;} _LL289: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_JoinEff: 0){ goto
_LL290;} else{ goto _LL246;} _LL248: return Cyc_PP_text( _tag_arr("[[[array]]]",
sizeof( unsigned char), 12u)); _LL250: return Cyc_PP_nil_doc(); _LL252: return
Cyc_PP_nil_doc(); _LL254: s= Cyc_PP_text( _tag_arr("void", sizeof( unsigned char),
5u)); goto _LL246; _LL256: if( _temp295 != 0){ return Cyc_Absynpp_ntyp2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp295))->v);} else{ s=({ struct
Cyc_PP_Doc* _temp343[ 6u]; _temp343[ 5u]= _temp297 == 0? Cyc_PP_text( _tag_arr("?",
sizeof( unsigned char), 2u)): Cyc_Absynpp_kind2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp297))->v); _temp343[ 4u]= Cyc_PP_text( _tag_arr(")::", sizeof(
unsigned char), 4u)); _temp343[ 3u]=( ! Cyc_Absynpp_print_full_evars? 1:
_temp291 == 0)? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp291))->v);
_temp343[ 2u]= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp345; _temp345.tag= Cyc_Stdio_Int_pa; _temp345.f1=( int)(( unsigned int)
_temp293);{ void* _temp344[ 1u]={& _temp345}; Cyc_Stdio_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp344, sizeof( void*), 1u));}}));
_temp343[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); _temp343[
0u]= Cyc_PP_text( _tag_arr("%", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp343, sizeof( struct Cyc_PP_Doc*), 6u));});} goto _LL246; _LL258:
s= Cyc_PP_textptr( _temp299->name); if( Cyc_Absynpp_print_all_kinds){ s=({
struct Cyc_PP_Doc* _temp346[ 3u]; _temp346[ 2u]= Cyc_Absynpp_ckind2doc( _temp299->kind);
_temp346[ 1u]= Cyc_PP_text( _tag_arr("::", sizeof( unsigned char), 3u));
_temp346[ 0u]= s; Cyc_PP_cat( _tag_arr( _temp346, sizeof( struct Cyc_PP_Doc*), 3u));});}
if( Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar( _temp299): 0){ s=({
struct Cyc_PP_Doc* _temp347[ 3u]; _temp347[ 2u]= Cyc_PP_text( _tag_arr(" */",
sizeof( unsigned char), 4u)); _temp347[ 1u]= s; _temp347[ 0u]= Cyc_PP_text(
_tag_arr("_ /* ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp347,
sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL260:{ void* _temp348=
_temp307; struct Cyc_Absyn_UnknownTunionInfo _temp354; int _temp356; struct
_tuple0* _temp358; struct Cyc_Absyn_Tuniondecl* _temp360; struct Cyc_Absyn_Tuniondecl
_temp362; int _temp363; struct _tuple0* _temp365; _LL350: if(*(( int*) _temp348)
== Cyc_Absyn_UnknownTunion){ _LL355: _temp354=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp348)->f1; _LL359: _temp358= _temp354.name; goto _LL357; _LL357: _temp356=
_temp354.is_xtunion; goto _LL351;} else{ goto _LL352;} _LL352: if(*(( int*)
_temp348) == Cyc_Absyn_KnownTunion){ _LL361: _temp360=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp348)->f1; _temp362=* _temp360; _LL366: _temp365= _temp362.name; goto _LL364;
_LL364: _temp363= _temp362.is_xtunion; goto _LL353;} else{ goto _LL349;} _LL351:
_temp365= _temp358; _temp363= _temp356; goto _LL353; _LL353: { struct Cyc_PP_Doc*
_temp367= Cyc_PP_text( _temp363? _tag_arr("xtunion ", sizeof( unsigned char), 9u):
_tag_arr("tunion ", sizeof( unsigned char), 8u));{ void* _temp368= Cyc_Tcutil_compress(
_temp303); _LL370: if( _temp368 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL371;}
else{ goto _LL372;} _LL372: goto _LL373; _LL371: s=({ struct Cyc_PP_Doc*
_temp374[ 3u]; _temp374[ 2u]= Cyc_Absynpp_tps2doc( _temp305); _temp374[ 1u]= Cyc_Absynpp_qvar2doc(
_temp365); _temp374[ 0u]= _temp367; Cyc_PP_cat( _tag_arr( _temp374, sizeof(
struct Cyc_PP_Doc*), 3u));}); goto _LL369; _LL373: s=({ struct Cyc_PP_Doc*
_temp375[ 5u]; _temp375[ 4u]= Cyc_Absynpp_tps2doc( _temp305); _temp375[ 3u]= Cyc_Absynpp_qvar2doc(
_temp365); _temp375[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp375[ 1u]= Cyc_Absynpp_typ2doc( _temp303); _temp375[ 0u]= _temp367; Cyc_PP_cat(
_tag_arr( _temp375, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL369; _LL369:;}
goto _LL349;} _LL349:;} goto _LL246; _LL262:{ void* _temp376= _temp313; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp382; int _temp384; struct _tuple0*
_temp386; struct _tuple0* _temp388; struct Cyc_Absyn_Tunionfield* _temp390;
struct Cyc_Absyn_Tunionfield _temp392; struct _tuple0* _temp393; struct Cyc_Absyn_Tuniondecl*
_temp395; struct Cyc_Absyn_Tuniondecl _temp397; int _temp398; struct _tuple0*
_temp400; _LL378: if(*(( int*) _temp376) == Cyc_Absyn_UnknownTunionfield){
_LL383: _temp382=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp376)->f1;
_LL389: _temp388= _temp382.tunion_name; goto _LL387; _LL387: _temp386= _temp382.field_name;
goto _LL385; _LL385: _temp384= _temp382.is_xtunion; goto _LL379;} else{ goto
_LL380;} _LL380: if(*(( int*) _temp376) == Cyc_Absyn_KnownTunionfield){ _LL396:
_temp395=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp376)->f1; _temp397=*
_temp395; _LL401: _temp400= _temp397.name; goto _LL399; _LL399: _temp398=
_temp397.is_xtunion; goto _LL391; _LL391: _temp390=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp376)->f2; _temp392=* _temp390; _LL394: _temp393= _temp392.name; goto _LL381;}
else{ goto _LL377;} _LL379: _temp400= _temp388; _temp398= _temp384; _temp393=
_temp386; goto _LL381; _LL381: { struct Cyc_PP_Doc* _temp402= Cyc_PP_text(
_temp398? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc* _temp403[ 4u]; _temp403[ 3u]=
Cyc_Absynpp_qvar2doc( _temp393); _temp403[ 2u]= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); _temp403[ 1u]= Cyc_Absynpp_qvar2doc( _temp400);
_temp403[ 0u]= _temp402; Cyc_PP_cat( _tag_arr( _temp403, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL377;} _LL377:;} goto _LL246; _LL264: { struct _tagged_arr sns;
struct _tagged_arr ts;{ void* _temp404= _temp317; _LL406: if( _temp404 ==( void*)
Cyc_Absyn_Signed){ goto _LL407;} else{ goto _LL408;} _LL408: if( _temp404 ==(
void*) Cyc_Absyn_Unsigned){ goto _LL409;} else{ goto _LL405;} _LL407: sns=
_tag_arr("", sizeof( unsigned char), 1u); goto _LL405; _LL409: sns= _tag_arr("unsigned ",
sizeof( unsigned char), 10u); goto _LL405; _LL405:;}{ void* _temp410= _temp315;
_LL412: if( _temp410 ==( void*) Cyc_Absyn_B1){ goto _LL413;} else{ goto _LL414;}
_LL414: if( _temp410 ==( void*) Cyc_Absyn_B2){ goto _LL415;} else{ goto _LL416;}
_LL416: if( _temp410 ==( void*) Cyc_Absyn_B4){ goto _LL417;} else{ goto _LL418;}
_LL418: if( _temp410 ==( void*) Cyc_Absyn_B8){ goto _LL419;} else{ goto _LL411;}
_LL413:{ void* _temp420= _temp317; _LL422: if( _temp420 ==( void*) Cyc_Absyn_Signed){
goto _LL423;} else{ goto _LL424;} _LL424: if( _temp420 ==( void*) Cyc_Absyn_Unsigned){
goto _LL425;} else{ goto _LL421;} _LL423: sns= _tag_arr("signed ", sizeof(
unsigned char), 8u); goto _LL421; _LL425: goto _LL421; _LL421:;} ts= _tag_arr("char",
sizeof( unsigned char), 5u); goto _LL411; _LL415: ts= _tag_arr("short", sizeof(
unsigned char), 6u); goto _LL411; _LL417: ts= _tag_arr("int", sizeof(
unsigned char), 4u); goto _LL411; _LL419: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64",
sizeof( unsigned char), 8u): _tag_arr("long long", sizeof( unsigned char), 10u);
goto _LL411; _LL411:;} s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp428; _temp428.tag= Cyc_Stdio_String_pa; _temp428.f1=( struct _tagged_arr)
ts;{ struct Cyc_Stdio_String_pa_struct _temp427; _temp427.tag= Cyc_Stdio_String_pa;
_temp427.f1=( struct _tagged_arr) sns;{ void* _temp426[ 2u]={& _temp427,&
_temp428}; Cyc_Stdio_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp426, sizeof( void*), 2u));}}})); goto _LL246;} _LL266: s= Cyc_PP_text(
_tag_arr("float", sizeof( unsigned char), 6u)); goto _LL246; _LL268: s= Cyc_PP_text(
_tag_arr("double", sizeof( unsigned char), 7u)); goto _LL246; _LL270: s=({
struct Cyc_PP_Doc* _temp429[ 2u]; _temp429[ 1u]= Cyc_Absynpp_args2doc( _temp319);
_temp429[ 0u]= Cyc_PP_text( _tag_arr("$", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp429, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL272:
if( _temp323 == 0){ s=({ struct Cyc_PP_Doc* _temp430[ 2u]; _temp430[ 1u]= Cyc_Absynpp_tps2doc(
_temp321); _temp430[ 0u]= Cyc_PP_text( _tag_arr("struct ", sizeof( unsigned char),
8u)); Cyc_PP_cat( _tag_arr( _temp430, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ s=({ struct Cyc_PP_Doc* _temp431[ 3u]; _temp431[ 2u]= Cyc_Absynpp_tps2doc(
_temp321); _temp431[ 1u]= _temp323 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp323)); _temp431[ 0u]= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp431, sizeof( struct Cyc_PP_Doc*),
3u));});} goto _LL246; _LL274: if( _temp327 == 0){ s=({ struct Cyc_PP_Doc*
_temp432[ 2u]; _temp432[ 1u]= Cyc_Absynpp_tps2doc( _temp325); _temp432[ 0u]= Cyc_PP_text(
_tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp432,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp433[ 3u];
_temp433[ 2u]= Cyc_Absynpp_tps2doc( _temp325); _temp433[ 1u]= _temp327 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp327)); _temp433[ 0u]=
Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp433, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL276:
s=({ struct Cyc_PP_Doc* _temp434[ 3u]; _temp434[ 2u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp434[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp329)); _temp434[ 0u]= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp434, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL278: s=({ struct Cyc_PP_Doc* _temp435[ 3u]; _temp435[ 2u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp435[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp331)); _temp435[ 0u]= Cyc_PP_text(
_tag_arr("union {", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp435,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL280: s=({ struct Cyc_PP_Doc*
_temp436[ 2u]; _temp436[ 1u]= Cyc_Absynpp_qvar2doc( _temp333); _temp436[ 0u]=
Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp436, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL282: s=({
struct Cyc_PP_Doc* _temp437[ 2u]; _temp437[ 1u]= Cyc_Absynpp_tps2doc( _temp337);
_temp437[ 0u]= Cyc_Absynpp_qvar2doc( _temp339); Cyc_PP_cat( _tag_arr( _temp437,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL284: s=({ struct Cyc_PP_Doc*
_temp438[ 3u]; _temp438[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof( unsigned char),
2u)); _temp438[ 1u]= Cyc_Absynpp_rgn2doc( _temp341); _temp438[ 0u]= Cyc_PP_text(
_tag_arr("region_t<", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp438, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL286: s= Cyc_Absynpp_rgn2doc(
t); goto _LL246; _LL288: s= Cyc_Absynpp_eff2doc( t); goto _LL246; _LL290: s= Cyc_Absynpp_eff2doc(
t); goto _LL246; _LL246:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct
_tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc(
struct _tuple8* cmp){ struct _tuple8 _temp441; void* _temp442; void* _temp444;
struct _tuple8* _temp439= cmp; _temp441=* _temp439; _LL445: _temp444= _temp441.f1;
goto _LL443; _LL443: _temp442= _temp441.f2; goto _LL440; _LL440: return({ struct
Cyc_PP_Doc* _temp446[ 3u]; _temp446[ 2u]= Cyc_Absynpp_rgn2doc( _temp442);
_temp446[ 1u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp446[ 0u]= Cyc_Absynpp_rgn2doc( _temp444); Cyc_PP_cat( _tag_arr( _temp446,
sizeof( struct Cyc_PP_Doc*), 3u));});} struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List* po){ return Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp447=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp447->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp447;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp448=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp448=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp448,({
struct Cyc_List_List* _temp449=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp449->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp449->tl= 0; _temp449;}));} else{ if(
cyc_varargs != 0){ struct Cyc_PP_Doc* _temp450=({ struct Cyc_PP_Doc* _temp452[ 4u];
_temp452[ 3u]= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp453=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp453->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp453->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp453->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp453;})); _temp452[ 2u]=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text( _tag_arr(" inject ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp452[ 1u]= Cyc_Absynpp_typ2doc(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->rgn); _temp452[ 0u]= Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); Cyc_PP_cat( _tag_arr( _temp452, sizeof( struct Cyc_PP_Doc*),
4u));}); _temp448=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp448,({ struct Cyc_List_List* _temp451=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp451->hd=(
void*) _temp450; _temp451->tl= 0; _temp451;}));}}{ struct Cyc_PP_Doc* _temp454=
Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u), _temp448); if(
effopt != 0){ _temp454=({ struct Cyc_PP_Doc* _temp455[ 3u]; _temp455[ 2u]= Cyc_Absynpp_eff2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp455[ 1u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp455[ 0u]= _temp454; Cyc_PP_cat(
_tag_arr( _temp455, sizeof( struct Cyc_PP_Doc*), 3u));});} if( rgn_po != 0){
_temp454=({ struct Cyc_PP_Doc* _temp456[ 3u]; _temp456[ 2u]= Cyc_Absynpp_rgnpo2doc(
rgn_po); _temp456[ 1u]= Cyc_PP_text( _tag_arr(":", sizeof( unsigned char), 2u));
_temp456[ 0u]= _temp454; Cyc_PP_cat( _tag_arr( _temp456, sizeof( struct Cyc_PP_Doc*),
3u));});} return({ struct Cyc_PP_Doc* _temp457[ 3u]; _temp457[ 2u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp457[ 1u]= _temp454; _temp457[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp457, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp458=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp458->f1=({ struct Cyc_Core_Opt*
_temp459=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp459->v=( void*)(* arg).f1; _temp459;}); _temp458->f2=(* arg).f2; _temp458->f3=(*
arg).f3; _temp458;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_arr* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp460= 0; int match;{ void*
_temp461=(* q).f1; struct Cyc_List_List* _temp469; struct Cyc_List_List*
_temp471; _LL463: if( _temp461 ==( void*) Cyc_Absyn_Loc_n){ goto _LL464;} else{
goto _LL465;} _LL465: if(( unsigned int) _temp461 > 1u?*(( int*) _temp461) ==
Cyc_Absyn_Rel_n: 0){ _LL470: _temp469=(( struct Cyc_Absyn_Rel_n_struct*)
_temp461)->f1; goto _LL466;} else{ goto _LL467;} _LL467: if(( unsigned int)
_temp461 > 1u?*(( int*) _temp461) == Cyc_Absyn_Abs_n: 0){ _LL472: _temp471=((
struct Cyc_Absyn_Abs_n_struct*) _temp461)->f1; goto _LL468;} else{ goto _LL462;}
_LL464: _temp469= 0; goto _LL466; _LL466: match= 0; _temp460= _temp469; goto
_LL462; _LL468: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp471, Cyc_Absynpp_curr_namespace):
0; _temp460=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp473=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp473->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp473->tl= _temp471;
_temp473;}): _temp471; goto _LL462; _LL462:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp460,({
struct Cyc_List_List* _temp474=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp474->hd=( void*)(* q).f2; _temp474->tl= 0; _temp474;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp460,({ struct Cyc_List_List* _temp475=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp475->hd=( void*)(* q).f2; _temp475->tl= 0;
_temp475;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp476=(* v).f1; struct Cyc_List_List* _temp486; struct Cyc_List_List*
_temp488; _LL478: if( _temp476 ==( void*) Cyc_Absyn_Loc_n){ goto _LL479;} else{
goto _LL480;} _LL480: if(( unsigned int) _temp476 > 1u?*(( int*) _temp476) ==
Cyc_Absyn_Rel_n: 0){ _LL487: _temp486=(( struct Cyc_Absyn_Rel_n_struct*)
_temp476)->f1; if( _temp486 == 0){ goto _LL481;} else{ goto _LL482;}} else{ goto
_LL482;} _LL482: if(( unsigned int) _temp476 > 1u?*(( int*) _temp476) == Cyc_Absyn_Abs_n:
0){ _LL489: _temp488=(( struct Cyc_Absyn_Abs_n_struct*) _temp476)->f1; goto
_LL483;} else{ goto _LL484;} _LL484: goto _LL485; _LL479: goto _LL481; _LL481:
return Cyc_Absynpp_var2doc((* v).f2); _LL483: if((( int(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp488, Cyc_Absynpp_curr_namespace)
== 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL485;} _LL485: return({
struct Cyc_PP_Doc* _temp490[ 2u]; _temp490[ 1u]= Cyc_Absynpp_qvar2doc( v);
_temp490[ 0u]= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof(
unsigned char), 23u)); Cyc_PP_cat( _tag_arr( _temp490, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL477:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp491=(
void*) e->r; void* _temp565; struct Cyc_Absyn_Exp* _temp567; struct Cyc_Absyn_Exp*
_temp569; _LL493: if(*(( int*) _temp491) == Cyc_Absyn_Const_e){ goto _LL494;}
else{ goto _LL495;} _LL495: if(*(( int*) _temp491) == Cyc_Absyn_Var_e){ goto
_LL496;} else{ goto _LL497;} _LL497: if(*(( int*) _temp491) == Cyc_Absyn_UnknownId_e){
goto _LL498;} else{ goto _LL499;} _LL499: if(*(( int*) _temp491) == Cyc_Absyn_Primop_e){
_LL566: _temp565=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp491)->f1;
goto _LL500;} else{ goto _LL501;} _LL501: if(*(( int*) _temp491) == Cyc_Absyn_AssignOp_e){
goto _LL502;} else{ goto _LL503;} _LL503: if(*(( int*) _temp491) == Cyc_Absyn_Increment_e){
goto _LL504;} else{ goto _LL505;} _LL505: if(*(( int*) _temp491) == Cyc_Absyn_Conditional_e){
goto _LL506;} else{ goto _LL507;} _LL507: if(*(( int*) _temp491) == Cyc_Absyn_SeqExp_e){
goto _LL508;} else{ goto _LL509;} _LL509: if(*(( int*) _temp491) == Cyc_Absyn_UnknownCall_e){
goto _LL510;} else{ goto _LL511;} _LL511: if(*(( int*) _temp491) == Cyc_Absyn_FnCall_e){
goto _LL512;} else{ goto _LL513;} _LL513: if(*(( int*) _temp491) == Cyc_Absyn_Throw_e){
goto _LL514;} else{ goto _LL515;} _LL515: if(*(( int*) _temp491) == Cyc_Absyn_NoInstantiate_e){
_LL568: _temp567=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp491)->f1;
goto _LL516;} else{ goto _LL517;} _LL517: if(*(( int*) _temp491) == Cyc_Absyn_Instantiate_e){
_LL570: _temp569=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp491)->f1; goto
_LL518;} else{ goto _LL519;} _LL519: if(*(( int*) _temp491) == Cyc_Absyn_Cast_e){
goto _LL520;} else{ goto _LL521;} _LL521: if(*(( int*) _temp491) == Cyc_Absyn_New_e){
goto _LL522;} else{ goto _LL523;} _LL523: if(*(( int*) _temp491) == Cyc_Absyn_Address_e){
goto _LL524;} else{ goto _LL525;} _LL525: if(*(( int*) _temp491) == Cyc_Absyn_Sizeoftyp_e){
goto _LL526;} else{ goto _LL527;} _LL527: if(*(( int*) _temp491) == Cyc_Absyn_Sizeofexp_e){
goto _LL528;} else{ goto _LL529;} _LL529: if(*(( int*) _temp491) == Cyc_Absyn_Offsetof_e){
goto _LL530;} else{ goto _LL531;} _LL531: if(*(( int*) _temp491) == Cyc_Absyn_Deref_e){
goto _LL532;} else{ goto _LL533;} _LL533: if(*(( int*) _temp491) == Cyc_Absyn_StructMember_e){
goto _LL534;} else{ goto _LL535;} _LL535: if(*(( int*) _temp491) == Cyc_Absyn_StructArrow_e){
goto _LL536;} else{ goto _LL537;} _LL537: if(*(( int*) _temp491) == Cyc_Absyn_Subscript_e){
goto _LL538;} else{ goto _LL539;} _LL539: if(*(( int*) _temp491) == Cyc_Absyn_Tuple_e){
goto _LL540;} else{ goto _LL541;} _LL541: if(*(( int*) _temp491) == Cyc_Absyn_CompoundLit_e){
goto _LL542;} else{ goto _LL543;} _LL543: if(*(( int*) _temp491) == Cyc_Absyn_Array_e){
goto _LL544;} else{ goto _LL545;} _LL545: if(*(( int*) _temp491) == Cyc_Absyn_Comprehension_e){
goto _LL546;} else{ goto _LL547;} _LL547: if(*(( int*) _temp491) == Cyc_Absyn_Struct_e){
goto _LL548;} else{ goto _LL549;} _LL549: if(*(( int*) _temp491) == Cyc_Absyn_AnonStruct_e){
goto _LL550;} else{ goto _LL551;} _LL551: if(*(( int*) _temp491) == Cyc_Absyn_Tunion_e){
goto _LL552;} else{ goto _LL553;} _LL553: if(*(( int*) _temp491) == Cyc_Absyn_Enum_e){
goto _LL554;} else{ goto _LL555;} _LL555: if(*(( int*) _temp491) == Cyc_Absyn_Malloc_e){
goto _LL556;} else{ goto _LL557;} _LL557: if(*(( int*) _temp491) == Cyc_Absyn_UnresolvedMem_e){
goto _LL558;} else{ goto _LL559;} _LL559: if(*(( int*) _temp491) == Cyc_Absyn_StmtExp_e){
goto _LL560;} else{ goto _LL561;} _LL561: if(*(( int*) _temp491) == Cyc_Absyn_Codegen_e){
goto _LL562;} else{ goto _LL563;} _LL563: if(*(( int*) _temp491) == Cyc_Absyn_Fill_e){
goto _LL564;} else{ goto _LL492;} _LL494: return 10000; _LL496: return 10000;
_LL498: return 10000; _LL500: { void* _temp571= _temp565; _LL573: if( _temp571
==( void*) Cyc_Absyn_Plus){ goto _LL574;} else{ goto _LL575;} _LL575: if(
_temp571 ==( void*) Cyc_Absyn_Times){ goto _LL576;} else{ goto _LL577;} _LL577:
if( _temp571 ==( void*) Cyc_Absyn_Minus){ goto _LL578;} else{ goto _LL579;}
_LL579: if( _temp571 ==( void*) Cyc_Absyn_Div){ goto _LL580;} else{ goto _LL581;}
_LL581: if( _temp571 ==( void*) Cyc_Absyn_Mod){ goto _LL582;} else{ goto _LL583;}
_LL583: if( _temp571 ==( void*) Cyc_Absyn_Eq){ goto _LL584;} else{ goto _LL585;}
_LL585: if( _temp571 ==( void*) Cyc_Absyn_Neq){ goto _LL586;} else{ goto _LL587;}
_LL587: if( _temp571 ==( void*) Cyc_Absyn_Gt){ goto _LL588;} else{ goto _LL589;}
_LL589: if( _temp571 ==( void*) Cyc_Absyn_Lt){ goto _LL590;} else{ goto _LL591;}
_LL591: if( _temp571 ==( void*) Cyc_Absyn_Gte){ goto _LL592;} else{ goto _LL593;}
_LL593: if( _temp571 ==( void*) Cyc_Absyn_Lte){ goto _LL594;} else{ goto _LL595;}
_LL595: if( _temp571 ==( void*) Cyc_Absyn_Not){ goto _LL596;} else{ goto _LL597;}
_LL597: if( _temp571 ==( void*) Cyc_Absyn_Bitnot){ goto _LL598;} else{ goto
_LL599;} _LL599: if( _temp571 ==( void*) Cyc_Absyn_Bitand){ goto _LL600;} else{
goto _LL601;} _LL601: if( _temp571 ==( void*) Cyc_Absyn_Bitor){ goto _LL602;}
else{ goto _LL603;} _LL603: if( _temp571 ==( void*) Cyc_Absyn_Bitxor){ goto
_LL604;} else{ goto _LL605;} _LL605: if( _temp571 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL606;} else{ goto _LL607;} _LL607: if( _temp571 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL608;} else{ goto _LL609;} _LL609: if( _temp571 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL610;} else{ goto _LL611;} _LL611: if( _temp571 ==( void*) Cyc_Absyn_Size){
goto _LL612;} else{ goto _LL572;} _LL574: return 100; _LL576: return 110; _LL578:
return 100; _LL580: return 110; _LL582: return 110; _LL584: return 70; _LL586:
return 70; _LL588: return 80; _LL590: return 80; _LL592: return 80; _LL594:
return 80; _LL596: return 130; _LL598: return 130; _LL600: return 60; _LL602:
return 40; _LL604: return 50; _LL606: return 90; _LL608: return 80; _LL610:
return 80; _LL612: return 140; _LL572:;} _LL502: return 20; _LL504: return 130;
_LL506: return 30; _LL508: return 10; _LL510: return 140; _LL512: return 140;
_LL514: return 130; _LL516: return Cyc_Absynpp_exp_prec( _temp567); _LL518:
return Cyc_Absynpp_exp_prec( _temp569); _LL520: return 120; _LL522: return 15;
_LL524: return 130; _LL526: return 130; _LL528: return 130; _LL530: return 130;
_LL532: return 130; _LL534: return 140; _LL536: return 140; _LL538: return 140;
_LL540: return 150; _LL542: goto _LL544; _LL544: goto _LL546; _LL546: goto
_LL548; _LL548: goto _LL550; _LL550: goto _LL552; _LL552: goto _LL554; _LL554:
goto _LL556; _LL556: return 140; _LL558: return 140; _LL560: return 10000;
_LL562: return 140; _LL564: return 140; _LL492:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec(
e); struct Cyc_PP_Doc* s;{ void* _temp613=( void*) e->r; void* _temp687; struct
_tuple0* _temp689; struct _tuple0* _temp691; struct Cyc_List_List* _temp693;
void* _temp695; struct Cyc_Absyn_Exp* _temp697; struct Cyc_Core_Opt* _temp699;
struct Cyc_Absyn_Exp* _temp701; void* _temp703; struct Cyc_Absyn_Exp* _temp705;
struct Cyc_Absyn_Exp* _temp707; struct Cyc_Absyn_Exp* _temp709; struct Cyc_Absyn_Exp*
_temp711; struct Cyc_Absyn_Exp* _temp713; struct Cyc_Absyn_Exp* _temp715; struct
Cyc_List_List* _temp717; struct Cyc_Absyn_Exp* _temp719; struct Cyc_List_List*
_temp721; struct Cyc_Absyn_Exp* _temp723; struct Cyc_Absyn_Exp* _temp725; struct
Cyc_Absyn_Exp* _temp727; struct Cyc_Absyn_Exp* _temp729; struct Cyc_Absyn_Exp*
_temp731; void* _temp733; struct Cyc_Absyn_Exp* _temp735; struct Cyc_Absyn_Exp*
_temp737; struct Cyc_Absyn_Exp* _temp739; void* _temp741; struct Cyc_Absyn_Exp*
_temp743; struct _tagged_arr* _temp745; void* _temp747; struct Cyc_Absyn_Exp*
_temp749; struct _tagged_arr* _temp751; struct Cyc_Absyn_Exp* _temp753; struct
_tagged_arr* _temp755; struct Cyc_Absyn_Exp* _temp757; struct Cyc_Absyn_Exp*
_temp759; struct Cyc_Absyn_Exp* _temp761; struct Cyc_List_List* _temp763; struct
Cyc_List_List* _temp765; struct _tuple1* _temp767; struct Cyc_List_List*
_temp769; struct Cyc_Absyn_Exp* _temp771; struct Cyc_Absyn_Exp* _temp773; struct
Cyc_Absyn_Vardecl* _temp775; struct Cyc_List_List* _temp777; struct _tuple0*
_temp779; struct Cyc_List_List* _temp781; struct Cyc_Absyn_Tunionfield* _temp783;
struct Cyc_List_List* _temp785; struct _tuple0* _temp787; void* _temp789; struct
Cyc_Absyn_Exp* _temp791; struct Cyc_List_List* _temp793; struct Cyc_Core_Opt*
_temp795; struct Cyc_Absyn_Stmt* _temp797; struct Cyc_Absyn_Fndecl* _temp799;
struct Cyc_Absyn_Exp* _temp801; _LL615: if(*(( int*) _temp613) == Cyc_Absyn_Const_e){
_LL688: _temp687=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp613)->f1;
goto _LL616;} else{ goto _LL617;} _LL617: if(*(( int*) _temp613) == Cyc_Absyn_Var_e){
_LL690: _temp689=(( struct Cyc_Absyn_Var_e_struct*) _temp613)->f1; goto _LL618;}
else{ goto _LL619;} _LL619: if(*(( int*) _temp613) == Cyc_Absyn_UnknownId_e){
_LL692: _temp691=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp613)->f1; goto
_LL620;} else{ goto _LL621;} _LL621: if(*(( int*) _temp613) == Cyc_Absyn_Primop_e){
_LL696: _temp695=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp613)->f1;
goto _LL694; _LL694: _temp693=(( struct Cyc_Absyn_Primop_e_struct*) _temp613)->f2;
goto _LL622;} else{ goto _LL623;} _LL623: if(*(( int*) _temp613) == Cyc_Absyn_AssignOp_e){
_LL702: _temp701=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp613)->f1; goto
_LL700; _LL700: _temp699=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp613)->f2;
goto _LL698; _LL698: _temp697=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp613)->f3;
goto _LL624;} else{ goto _LL625;} _LL625: if(*(( int*) _temp613) == Cyc_Absyn_Increment_e){
_LL706: _temp705=(( struct Cyc_Absyn_Increment_e_struct*) _temp613)->f1; goto
_LL704; _LL704: _temp703=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp613)->f2; goto _LL626;} else{ goto _LL627;} _LL627: if(*(( int*) _temp613)
== Cyc_Absyn_Conditional_e){ _LL712: _temp711=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp613)->f1; goto _LL710; _LL710: _temp709=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp613)->f2; goto _LL708; _LL708: _temp707=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp613)->f3; goto _LL628;} else{ goto _LL629;} _LL629: if(*(( int*) _temp613)
== Cyc_Absyn_SeqExp_e){ _LL716: _temp715=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp613)->f1; goto _LL714; _LL714: _temp713=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp613)->f2; goto _LL630;} else{ goto _LL631;} _LL631: if(*(( int*) _temp613)
== Cyc_Absyn_UnknownCall_e){ _LL720: _temp719=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp613)->f1; goto _LL718; _LL718: _temp717=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp613)->f2; goto _LL632;} else{ goto _LL633;} _LL633: if(*(( int*) _temp613)
== Cyc_Absyn_FnCall_e){ _LL724: _temp723=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp613)->f1; goto _LL722; _LL722: _temp721=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp613)->f2; goto _LL634;} else{ goto _LL635;} _LL635: if(*(( int*) _temp613)
== Cyc_Absyn_Throw_e){ _LL726: _temp725=(( struct Cyc_Absyn_Throw_e_struct*)
_temp613)->f1; goto _LL636;} else{ goto _LL637;} _LL637: if(*(( int*) _temp613)
== Cyc_Absyn_NoInstantiate_e){ _LL728: _temp727=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp613)->f1; goto _LL638;} else{ goto _LL639;} _LL639: if(*(( int*) _temp613)
== Cyc_Absyn_Instantiate_e){ _LL730: _temp729=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp613)->f1; goto _LL640;} else{ goto _LL641;} _LL641: if(*(( int*) _temp613)
== Cyc_Absyn_Cast_e){ _LL734: _temp733=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp613)->f1; goto _LL732; _LL732: _temp731=(( struct Cyc_Absyn_Cast_e_struct*)
_temp613)->f2; goto _LL642;} else{ goto _LL643;} _LL643: if(*(( int*) _temp613)
== Cyc_Absyn_Address_e){ _LL736: _temp735=(( struct Cyc_Absyn_Address_e_struct*)
_temp613)->f1; goto _LL644;} else{ goto _LL645;} _LL645: if(*(( int*) _temp613)
== Cyc_Absyn_New_e){ _LL740: _temp739=(( struct Cyc_Absyn_New_e_struct*)
_temp613)->f1; goto _LL738; _LL738: _temp737=(( struct Cyc_Absyn_New_e_struct*)
_temp613)->f2; goto _LL646;} else{ goto _LL647;} _LL647: if(*(( int*) _temp613)
== Cyc_Absyn_Sizeoftyp_e){ _LL742: _temp741=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp613)->f1; goto _LL648;} else{ goto _LL649;} _LL649: if(*(( int*) _temp613)
== Cyc_Absyn_Sizeofexp_e){ _LL744: _temp743=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp613)->f1; goto _LL650;} else{ goto _LL651;} _LL651: if(*(( int*) _temp613)
== Cyc_Absyn_Offsetof_e){ _LL748: _temp747=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp613)->f1; goto _LL746; _LL746: _temp745=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp613)->f2; goto _LL652;} else{ goto _LL653;} _LL653: if(*(( int*) _temp613)
== Cyc_Absyn_Deref_e){ _LL750: _temp749=(( struct Cyc_Absyn_Deref_e_struct*)
_temp613)->f1; goto _LL654;} else{ goto _LL655;} _LL655: if(*(( int*) _temp613)
== Cyc_Absyn_StructMember_e){ _LL754: _temp753=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp613)->f1; goto _LL752; _LL752: _temp751=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp613)->f2; goto _LL656;} else{ goto _LL657;} _LL657: if(*(( int*) _temp613)
== Cyc_Absyn_StructArrow_e){ _LL758: _temp757=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp613)->f1; goto _LL756; _LL756: _temp755=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp613)->f2; goto _LL658;} else{ goto _LL659;} _LL659: if(*(( int*) _temp613)
== Cyc_Absyn_Subscript_e){ _LL762: _temp761=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp613)->f1; goto _LL760; _LL760: _temp759=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp613)->f2; goto _LL660;} else{ goto _LL661;} _LL661: if(*(( int*) _temp613)
== Cyc_Absyn_Tuple_e){ _LL764: _temp763=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp613)->f1; goto _LL662;} else{ goto _LL663;} _LL663: if(*(( int*) _temp613)
== Cyc_Absyn_CompoundLit_e){ _LL768: _temp767=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp613)->f1; goto _LL766; _LL766: _temp765=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp613)->f2; goto _LL664;} else{ goto _LL665;} _LL665: if(*(( int*) _temp613)
== Cyc_Absyn_Array_e){ _LL770: _temp769=(( struct Cyc_Absyn_Array_e_struct*)
_temp613)->f1; goto _LL666;} else{ goto _LL667;} _LL667: if(*(( int*) _temp613)
== Cyc_Absyn_Comprehension_e){ _LL776: _temp775=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp613)->f1; goto _LL774; _LL774: _temp773=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp613)->f2; goto _LL772; _LL772: _temp771=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp613)->f3; goto _LL668;} else{ goto _LL669;} _LL669: if(*(( int*) _temp613)
== Cyc_Absyn_Struct_e){ _LL780: _temp779=(( struct Cyc_Absyn_Struct_e_struct*)
_temp613)->f1; goto _LL778; _LL778: _temp777=(( struct Cyc_Absyn_Struct_e_struct*)
_temp613)->f3; goto _LL670;} else{ goto _LL671;} _LL671: if(*(( int*) _temp613)
== Cyc_Absyn_AnonStruct_e){ _LL782: _temp781=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp613)->f2; goto _LL672;} else{ goto _LL673;} _LL673: if(*(( int*) _temp613)
== Cyc_Absyn_Tunion_e){ _LL786: _temp785=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp613)->f3; goto _LL784; _LL784: _temp783=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp613)->f5; goto _LL674;} else{ goto _LL675;} _LL675: if(*(( int*) _temp613)
== Cyc_Absyn_Enum_e){ _LL788: _temp787=(( struct Cyc_Absyn_Enum_e_struct*)
_temp613)->f1; goto _LL676;} else{ goto _LL677;} _LL677: if(*(( int*) _temp613)
== Cyc_Absyn_Malloc_e){ _LL792: _temp791=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp613)->f1; goto _LL790; _LL790: _temp789=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp613)->f2; goto _LL678;} else{ goto _LL679;} _LL679: if(*(( int*) _temp613)
== Cyc_Absyn_UnresolvedMem_e){ _LL796: _temp795=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp613)->f1; goto _LL794; _LL794: _temp793=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp613)->f2; goto _LL680;} else{ goto _LL681;} _LL681: if(*(( int*) _temp613)
== Cyc_Absyn_StmtExp_e){ _LL798: _temp797=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp613)->f1; goto _LL682;} else{ goto _LL683;} _LL683: if(*(( int*) _temp613)
== Cyc_Absyn_Codegen_e){ _LL800: _temp799=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp613)->f1; goto _LL684;} else{ goto _LL685;} _LL685: if(*(( int*) _temp613)
== Cyc_Absyn_Fill_e){ _LL802: _temp801=(( struct Cyc_Absyn_Fill_e_struct*)
_temp613)->f1; goto _LL686;} else{ goto _LL614;} _LL616: s= Cyc_Absynpp_cnst2doc(
_temp687); goto _LL614; _LL618: _temp691= _temp689; goto _LL620; _LL620: s= Cyc_Absynpp_qvar2doc(
_temp691); goto _LL614; _LL622: s= Cyc_Absynpp_primapp2doc( myprec, _temp695,
_temp693); goto _LL614; _LL624: s=({ struct Cyc_PP_Doc* _temp803[ 5u]; _temp803[
4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp697); _temp803[ 3u]= Cyc_PP_text(
_tag_arr("= ", sizeof( unsigned char), 3u)); _temp803[ 2u]= _temp699 == 0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp699))->v); _temp803[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp803[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp701); Cyc_PP_cat( _tag_arr( _temp803, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL614; _LL626: { struct Cyc_PP_Doc* _temp804= Cyc_Absynpp_exp2doc_prec(
myprec, _temp705);{ void* _temp805= _temp703; _LL807: if( _temp805 ==( void*)
Cyc_Absyn_PreInc){ goto _LL808;} else{ goto _LL809;} _LL809: if( _temp805 ==(
void*) Cyc_Absyn_PreDec){ goto _LL810;} else{ goto _LL811;} _LL811: if( _temp805
==( void*) Cyc_Absyn_PostInc){ goto _LL812;} else{ goto _LL813;} _LL813: if(
_temp805 ==( void*) Cyc_Absyn_PostDec){ goto _LL814;} else{ goto _LL806;} _LL808:
s=({ struct Cyc_PP_Doc* _temp815[ 2u]; _temp815[ 1u]= _temp804; _temp815[ 0u]=
Cyc_PP_text( _tag_arr("++", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr(
_temp815, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL806; _LL810: s=({ struct
Cyc_PP_Doc* _temp816[ 2u]; _temp816[ 1u]= _temp804; _temp816[ 0u]= Cyc_PP_text(
_tag_arr("--", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr( _temp816,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL806; _LL812: s=({ struct Cyc_PP_Doc*
_temp817[ 2u]; _temp817[ 1u]= Cyc_PP_text( _tag_arr("++", sizeof( unsigned char),
3u)); _temp817[ 0u]= _temp804; Cyc_PP_cat( _tag_arr( _temp817, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL806; _LL814: s=({ struct Cyc_PP_Doc* _temp818[ 2u];
_temp818[ 1u]= Cyc_PP_text( _tag_arr("--", sizeof( unsigned char), 3u));
_temp818[ 0u]= _temp804; Cyc_PP_cat( _tag_arr( _temp818, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL806; _LL806:;} goto _LL614;} _LL628:{ struct _tuple8 _temp820=({
struct _tuple8 _temp819; _temp819.f1=( void*) _temp709->r; _temp819.f2=( void*)
_temp707->r; _temp819;}); _LL822: goto _LL823; _LL823: s=({ struct Cyc_PP_Doc*
_temp824[ 5u]; _temp824[ 4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp707);
_temp824[ 3u]= Cyc_PP_text( _tag_arr(" : ", sizeof( unsigned char), 4u));
_temp824[ 2u]= Cyc_Absynpp_exp2doc_prec( 0, _temp709); _temp824[ 1u]= Cyc_PP_text(
_tag_arr(" ? ", sizeof( unsigned char), 4u)); _temp824[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp711); Cyc_PP_cat( _tag_arr( _temp824, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL821; _LL821:;} goto _LL614; _LL630: s=({ struct Cyc_PP_Doc*
_temp825[ 5u]; _temp825[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp825[ 3u]= Cyc_Absynpp_exp2doc( _temp713); _temp825[ 2u]= Cyc_PP_text(
_tag_arr(", ", sizeof( unsigned char), 3u)); _temp825[ 1u]= Cyc_Absynpp_exp2doc(
_temp715); _temp825[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp825, sizeof( struct Cyc_PP_Doc*), 5u));}); goto
_LL614; _LL632: s=({ struct Cyc_PP_Doc* _temp826[ 4u]; _temp826[ 3u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp826[ 2u]= Cyc_Absynpp_exps2doc_prec(
20, _temp717); _temp826[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); _temp826[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp719); Cyc_PP_cat(
_tag_arr( _temp826, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL614; _LL634: s=({
struct Cyc_PP_Doc* _temp827[ 4u]; _temp827[ 3u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp827[ 2u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp721); _temp827[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
_temp827[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp723); Cyc_PP_cat( _tag_arr(
_temp827, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL614; _LL636: s=({ struct
Cyc_PP_Doc* _temp828[ 2u]; _temp828[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp725); _temp828[ 0u]= Cyc_PP_text( _tag_arr("throw ", sizeof( unsigned char),
7u)); Cyc_PP_cat( _tag_arr( _temp828, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL614; _LL638: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp727); goto _LL614;
_LL640: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp729); goto _LL614; _LL642: s=({
struct Cyc_PP_Doc* _temp829[ 4u]; _temp829[ 3u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp731); _temp829[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp829[ 1u]= Cyc_Absynpp_typ2doc( _temp733); _temp829[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp829, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL614; _LL644: s=({ struct
Cyc_PP_Doc* _temp830[ 2u]; _temp830[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp735); _temp830[ 0u]= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp830, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL614; _LL646: if( _temp739 == 0){ s=({ struct Cyc_PP_Doc* _temp831[ 2u];
_temp831[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp737); _temp831[ 0u]= Cyc_PP_text(
_tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp831,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp832[ 4u];
_temp832[ 3u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp737); _temp832[ 2u]= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); _temp832[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp739)); _temp832[ 0u]= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp832,
sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL614; _LL648: s=({ struct Cyc_PP_Doc*
_temp833[ 3u]; _temp833[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp833[ 1u]= Cyc_Absynpp_typ2doc( _temp741); _temp833[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp833,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL614; _LL650: s=({ struct Cyc_PP_Doc*
_temp834[ 3u]; _temp834[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp834[ 1u]= Cyc_Absynpp_exp2doc( _temp743); _temp834[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp834,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL614; _LL652: s=({ struct Cyc_PP_Doc*
_temp835[ 5u]; _temp835[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp835[ 3u]= Cyc_PP_textptr( _temp745); _temp835[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp835[ 1u]= Cyc_Absynpp_typ2doc(
_temp747); _temp835[ 0u]= Cyc_PP_text( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp835, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL614; _LL654: s=({ struct Cyc_PP_Doc* _temp836[ 2u]; _temp836[ 1u]=
Cyc_Absynpp_exp2doc_prec( myprec, _temp749); _temp836[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp836,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL614; _LL656: s=({ struct Cyc_PP_Doc*
_temp837[ 3u]; _temp837[ 2u]= Cyc_PP_textptr( _temp751); _temp837[ 1u]= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); _temp837[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp753); Cyc_PP_cat( _tag_arr( _temp837, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL614; _LL658: s=({ struct Cyc_PP_Doc* _temp838[ 3u]; _temp838[ 2u]=
Cyc_PP_textptr( _temp755); _temp838[ 1u]= Cyc_PP_text( _tag_arr("->", sizeof(
unsigned char), 3u)); _temp838[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp757);
Cyc_PP_cat( _tag_arr( _temp838, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL614; _LL660: s=({ struct Cyc_PP_Doc* _temp839[ 4u]; _temp839[ 3u]= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); _temp839[ 2u]= Cyc_Absynpp_exp2doc(
_temp759); _temp839[ 1u]= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u));
_temp839[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp761); Cyc_PP_cat( _tag_arr(
_temp839, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL614; _LL662: s=({ struct
Cyc_PP_Doc* _temp840[ 3u]; _temp840[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp840[ 1u]= Cyc_Absynpp_exps2doc_prec( 20, _temp763);
_temp840[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp840, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL614; _LL664: s=({
struct Cyc_PP_Doc* _temp841[ 4u]; _temp841[ 3u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp765)); _temp841[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
_temp841[ 1u]= Cyc_Absynpp_typ2doc((* _temp767).f3); _temp841[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp841,
sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL614; _LL666: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp769)); goto _LL614; _LL668: s=({ struct Cyc_PP_Doc*
_temp842[ 7u]; _temp842[ 6u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp842[ 5u]= Cyc_Absynpp_exp2doc( _temp771); _temp842[ 4u]= Cyc_PP_text(
_tag_arr(" : ", sizeof( unsigned char), 4u)); _temp842[ 3u]= Cyc_Absynpp_exp2doc(
_temp773); _temp842[ 2u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp842[ 1u]= Cyc_PP_text(*(* _temp775->name).f2); _temp842[ 0u]= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp842,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL614; _LL670: s=({ struct Cyc_PP_Doc*
_temp843[ 2u]; _temp843[ 1u]= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char),
2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp777));
_temp843[ 0u]= Cyc_Absynpp_qvar2doc( _temp779); Cyc_PP_cat( _tag_arr( _temp843,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL614; _LL672: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp781)); goto _LL614; _LL674: if( _temp785 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp783->name);} else{ s=({ struct Cyc_PP_Doc* _temp844[ 2u]; _temp844[ 1u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp785)); _temp844[ 0u]= Cyc_Absynpp_qvar2doc( _temp783->name);
Cyc_PP_cat( _tag_arr( _temp844, sizeof( struct Cyc_PP_Doc*), 2u));});} goto
_LL614; _LL676: s= Cyc_Absynpp_qvar2doc( _temp787); goto _LL614; _LL678: if(
_temp791 == 0){ s=({ struct Cyc_PP_Doc* _temp845[ 3u]; _temp845[ 2u]= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); _temp845[ 1u]= Cyc_Absynpp_typ2doc(
_temp789); _temp845[ 0u]= Cyc_PP_text( _tag_arr("malloc(sizeof(", sizeof(
unsigned char), 15u)); Cyc_PP_cat( _tag_arr( _temp845, sizeof( struct Cyc_PP_Doc*),
3u));});} else{ s=({ struct Cyc_PP_Doc* _temp846[ 6u]; _temp846[ 5u]= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); _temp846[ 4u]= Cyc_Absynpp_typ2doc(
_temp789); _temp846[ 3u]= Cyc_PP_text( _tag_arr("sizeof(", sizeof( unsigned char),
8u)); _temp846[ 2u]= Cyc_PP_text( _tag_arr(",", sizeof( unsigned char), 2u));
_temp846[ 1u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null(
_temp791)); _temp846[ 0u]= Cyc_PP_text( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp846, sizeof( struct Cyc_PP_Doc*),
6u));});} goto _LL614; _LL680: s= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp793)); goto _LL614; _LL682: s=({ struct Cyc_PP_Doc* _temp847[ 3u]; _temp847[
2u]= Cyc_PP_text( _tag_arr(" })", sizeof( unsigned char), 4u)); _temp847[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp797)); _temp847[ 0u]= Cyc_PP_text(
_tag_arr("({ ", sizeof( unsigned char), 4u)); Cyc_PP_cat( _tag_arr( _temp847,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL614; _LL684: s=({ struct Cyc_PP_Doc*
_temp848[ 3u]; _temp848[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp848[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl*
_temp849=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp849->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp850=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp850[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp851; _temp851.tag= Cyc_Absyn_Fn_d;
_temp851.f1= _temp799; _temp851;}); _temp850;})); _temp849->loc= e->loc;
_temp849;}))); _temp848[ 0u]= Cyc_PP_text( _tag_arr("codegen(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp848, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL614; _LL686: s=({ struct Cyc_PP_Doc* _temp852[ 3u]; _temp852[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp852[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_exp2doc( _temp801)); _temp852[ 0u]= Cyc_PP_text( _tag_arr("fill(",
sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp852, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL614; _LL614:;} if( inprec >= myprec){ s=({ struct Cyc_PP_Doc*
_temp853[ 3u]; _temp853[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp853[ 1u]= s; _temp853[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp853, sizeof( struct Cyc_PP_Doc*),
3u));});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){
void* _temp854= d; struct Cyc_Absyn_Exp* _temp860; struct _tagged_arr* _temp862;
_LL856: if(*(( int*) _temp854) == Cyc_Absyn_ArrayElement){ _LL861: _temp860=((
struct Cyc_Absyn_ArrayElement_struct*) _temp854)->f1; goto _LL857;} else{ goto
_LL858;} _LL858: if(*(( int*) _temp854) == Cyc_Absyn_FieldName){ _LL863:
_temp862=(( struct Cyc_Absyn_FieldName_struct*) _temp854)->f1; goto _LL859;}
else{ goto _LL855;} _LL857: return({ struct Cyc_PP_Doc* _temp864[ 3u]; _temp864[
2u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char), 2u)); _temp864[ 1u]= Cyc_Absynpp_exp2doc(
_temp860); _temp864[ 0u]= Cyc_PP_text( _tag_arr(".[", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp864, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL859:
return({ struct Cyc_PP_Doc* _temp865[ 2u]; _temp865[ 1u]= Cyc_PP_textptr(
_temp862); _temp865[ 0u]= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp865, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL855:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({ struct Cyc_PP_Doc*
_temp866[ 2u]; _temp866[ 1u]= Cyc_Absynpp_exp2doc((* de).f2); _temp866[ 0u]= Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char),
2u), _tag_arr("=", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); Cyc_PP_cat( _tag_arr( _temp866, sizeof( struct Cyc_PP_Doc*), 2u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp867= c;
unsigned char _temp885; void* _temp887; short _temp889; void* _temp891; int
_temp893; void* _temp895; int _temp897; void* _temp899; long long _temp901; void*
_temp903; struct _tagged_arr _temp905; struct _tagged_arr _temp907; _LL869: if((
unsigned int) _temp867 > 1u?*(( int*) _temp867) == Cyc_Absyn_Char_c: 0){ _LL888:
_temp887=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp867)->f1; goto _LL886;
_LL886: _temp885=(( struct Cyc_Absyn_Char_c_struct*) _temp867)->f2; goto _LL870;}
else{ goto _LL871;} _LL871: if(( unsigned int) _temp867 > 1u?*(( int*) _temp867)
== Cyc_Absyn_Short_c: 0){ _LL892: _temp891=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp867)->f1; goto _LL890; _LL890: _temp889=(( struct Cyc_Absyn_Short_c_struct*)
_temp867)->f2; goto _LL872;} else{ goto _LL873;} _LL873: if(( unsigned int)
_temp867 > 1u?*(( int*) _temp867) == Cyc_Absyn_Int_c: 0){ _LL896: _temp895=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp867)->f1; if( _temp895 ==( void*)
Cyc_Absyn_Signed){ goto _LL894;} else{ goto _LL875;} _LL894: _temp893=(( struct
Cyc_Absyn_Int_c_struct*) _temp867)->f2; goto _LL874;} else{ goto _LL875;} _LL875:
if(( unsigned int) _temp867 > 1u?*(( int*) _temp867) == Cyc_Absyn_Int_c: 0){
_LL900: _temp899=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp867)->f1; if(
_temp899 ==( void*) Cyc_Absyn_Unsigned){ goto _LL898;} else{ goto _LL877;}
_LL898: _temp897=(( struct Cyc_Absyn_Int_c_struct*) _temp867)->f2; goto _LL876;}
else{ goto _LL877;} _LL877: if(( unsigned int) _temp867 > 1u?*(( int*) _temp867)
== Cyc_Absyn_LongLong_c: 0){ _LL904: _temp903=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp867)->f1; goto _LL902; _LL902: _temp901=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp867)->f2; goto _LL878;} else{ goto _LL879;} _LL879: if(( unsigned int)
_temp867 > 1u?*(( int*) _temp867) == Cyc_Absyn_Float_c: 0){ _LL906: _temp905=((
struct Cyc_Absyn_Float_c_struct*) _temp867)->f1; goto _LL880;} else{ goto _LL881;}
_LL881: if( _temp867 ==( void*) Cyc_Absyn_Null_c){ goto _LL882;} else{ goto
_LL883;} _LL883: if(( unsigned int) _temp867 > 1u?*(( int*) _temp867) == Cyc_Absyn_String_c:
0){ _LL908: _temp907=(( struct Cyc_Absyn_String_c_struct*) _temp867)->f1; goto
_LL884;} else{ goto _LL868;} _LL870: return Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp910; _temp910.tag= Cyc_Stdio_String_pa;
_temp910.f1=( struct _tagged_arr) Cyc_Absynpp_char_escape( _temp885);{ void*
_temp909[ 1u]={& _temp910}; Cyc_Stdio_aprintf( _tag_arr("'%s'", sizeof(
unsigned char), 5u), _tag_arr( _temp909, sizeof( void*), 1u));}})); _LL872:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp912; _temp912.tag= Cyc_Stdio_Int_pa; _temp912.f1=( int)(( unsigned int)((
int) _temp889));{ void* _temp911[ 1u]={& _temp912}; Cyc_Stdio_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp911, sizeof( void*), 1u));}}));
_LL874: return Cyc_PP_text(( struct _tagged_arr) Cyc_Core_string_of_int(
_temp893)); _LL876: return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp914; _temp914.tag= Cyc_Stdio_Int_pa; _temp914.f1=( unsigned int) _temp897;{
void* _temp913[ 1u]={& _temp914}; Cyc_Stdio_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp913, sizeof( void*), 1u));}})); _LL878:
return Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char),
27u)); _LL880: return Cyc_PP_text( _temp905); _LL882: return Cyc_PP_text(
_tag_arr("null", sizeof( unsigned char), 5u)); _LL884: return({ struct Cyc_PP_Doc*
_temp915[ 3u]; _temp915[ 2u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char),
2u)); _temp915[ 1u]= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp907));
_temp915[ 0u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp915, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL868:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p ==( void*) Cyc_Absyn_Size){ if(
es == 0? 1:(( struct Cyc_List_List*) _check_null( es))->tl != 0){( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp916=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp916[ 0]=({ struct Cyc_Core_Failure_struct
_temp917; _temp917.tag= Cyc_Core_Failure; _temp917.f1=( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp919; _temp919.tag= Cyc_Stdio_String_pa;
_temp919.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp918[
1u]={& _temp919}; Cyc_Stdio_aprintf( _tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof( unsigned char), 44u), _tag_arr( _temp918, sizeof( void*), 1u));}});
_temp917;}); _temp916;}));}{ struct Cyc_PP_Doc* _temp920= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
return({ struct Cyc_PP_Doc* _temp921[ 2u]; _temp921[ 1u]= Cyc_PP_text( _tag_arr(".size",
sizeof( unsigned char), 6u)); _temp921[ 0u]= _temp920; Cyc_PP_cat( _tag_arr(
_temp921, sizeof( struct Cyc_PP_Doc*), 2u));});}} else{ struct Cyc_List_List* ds=((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp922=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp922[ 0]=({ struct Cyc_Core_Failure_struct _temp923; _temp923.tag= Cyc_Core_Failure;
_temp923.f1=( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp925;
_temp925.tag= Cyc_Stdio_String_pa; _temp925.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp924[ 1u]={& _temp925}; Cyc_Stdio_aprintf( _tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof( unsigned char), 38u), _tag_arr( _temp924, sizeof( void*), 1u));}});
_temp923;}); _temp922;}));} else{ if((( struct Cyc_List_List*) _check_null( ds))->tl
== 0){ return({ struct Cyc_PP_Doc* _temp926[ 3u]; _temp926[ 2u]=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; _temp926[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp926[ 0u]= ps; Cyc_PP_cat(
_tag_arr( _temp926, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ if((( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->tl
!= 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp927=( struct
Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp927[ 0]=({ struct Cyc_Core_Failure_struct _temp928; _temp928.tag= Cyc_Core_Failure;
_temp928.f1=( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp930;
_temp930.tag= Cyc_Stdio_String_pa; _temp930.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp929[ 1u]={& _temp930}; Cyc_Stdio_aprintf( _tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof( unsigned char), 47u), _tag_arr( _temp929, sizeof( void*), 1u));}});
_temp928;}); _temp927;}));} else{ return({ struct Cyc_PP_Doc* _temp931[ 5u];
_temp931[ 4u]=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( ds))->tl))->hd; _temp931[ 3u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp931[ 2u]= ps; _temp931[ 1u]=
Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp931[ 0u]=( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds))->hd; Cyc_PP_cat(
_tag_arr( _temp931, sizeof( struct Cyc_PP_Doc*), 5u));});}}}}} struct
_tagged_arr Cyc_Absynpp_prim2str( void* p){ void* _temp932= p; _LL934: if(
_temp932 ==( void*) Cyc_Absyn_Plus){ goto _LL935;} else{ goto _LL936;} _LL936:
if( _temp932 ==( void*) Cyc_Absyn_Times){ goto _LL937;} else{ goto _LL938;}
_LL938: if( _temp932 ==( void*) Cyc_Absyn_Minus){ goto _LL939;} else{ goto
_LL940;} _LL940: if( _temp932 ==( void*) Cyc_Absyn_Div){ goto _LL941;} else{
goto _LL942;} _LL942: if( _temp932 ==( void*) Cyc_Absyn_Mod){ goto _LL943;}
else{ goto _LL944;} _LL944: if( _temp932 ==( void*) Cyc_Absyn_Eq){ goto _LL945;}
else{ goto _LL946;} _LL946: if( _temp932 ==( void*) Cyc_Absyn_Neq){ goto _LL947;}
else{ goto _LL948;} _LL948: if( _temp932 ==( void*) Cyc_Absyn_Gt){ goto _LL949;}
else{ goto _LL950;} _LL950: if( _temp932 ==( void*) Cyc_Absyn_Lt){ goto _LL951;}
else{ goto _LL952;} _LL952: if( _temp932 ==( void*) Cyc_Absyn_Gte){ goto _LL953;}
else{ goto _LL954;} _LL954: if( _temp932 ==( void*) Cyc_Absyn_Lte){ goto _LL955;}
else{ goto _LL956;} _LL956: if( _temp932 ==( void*) Cyc_Absyn_Not){ goto _LL957;}
else{ goto _LL958;} _LL958: if( _temp932 ==( void*) Cyc_Absyn_Bitnot){ goto
_LL959;} else{ goto _LL960;} _LL960: if( _temp932 ==( void*) Cyc_Absyn_Bitand){
goto _LL961;} else{ goto _LL962;} _LL962: if( _temp932 ==( void*) Cyc_Absyn_Bitor){
goto _LL963;} else{ goto _LL964;} _LL964: if( _temp932 ==( void*) Cyc_Absyn_Bitxor){
goto _LL965;} else{ goto _LL966;} _LL966: if( _temp932 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL967;} else{ goto _LL968;} _LL968: if( _temp932 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL969;} else{ goto _LL970;} _LL970: if( _temp932 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL971;} else{ goto _LL972;} _LL972: if( _temp932 ==( void*) Cyc_Absyn_Size){
goto _LL973;} else{ goto _LL933;} _LL935: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL937: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL939: return _tag_arr("-", sizeof( unsigned char), 2u); _LL941: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL943: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL945: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL947: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL949: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL951: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL953: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL955: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL957: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL959: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL961: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL963: return _tag_arr("|", sizeof( unsigned char), 2u); _LL965: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL967: return _tag_arr("<<", sizeof(
unsigned char), 3u); _LL969: return _tag_arr(">>", sizeof( unsigned char), 3u);
_LL971: return _tag_arr(">>>", sizeof( unsigned char), 4u); _LL973: return
_tag_arr("size", sizeof( unsigned char), 5u); _LL933:;} struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc(
void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str( p));} struct _tuple9{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc(
struct _tuple9* t){ if((* t).f1 == 0){ return Cyc_Absynpp_exp2doc((* t).f2);}
else{ return({ struct Cyc_PP_Doc* _temp974[ 3u]; _temp974[ 2u]= Cyc_Absynpp_exp2doc((*
t).f2); _temp974[ 1u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp974[ 0u]= Cyc_PP_textptr(( struct _tagged_arr*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v); Cyc_PP_cat( _tag_arr( _temp974, sizeof( struct Cyc_PP_Doc*),
3u));});}} int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void*
_temp975=( void*) s->r; _LL977: if(( unsigned int) _temp975 > 1u?*(( int*)
_temp975) == Cyc_Absyn_Decl_s: 0){ goto _LL978;} else{ goto _LL979;} _LL979:
goto _LL980; _LL978: return 1; _LL980: return 0; _LL976:;} struct Cyc_PP_Doc*
Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void*
_temp981=( void*) st->r; struct Cyc_Absyn_Exp* _temp1025; struct Cyc_Absyn_Stmt*
_temp1027; struct Cyc_Absyn_Stmt* _temp1029; struct Cyc_Absyn_Exp* _temp1031;
struct Cyc_Absyn_Stmt* _temp1033; struct Cyc_Absyn_Stmt* _temp1035; struct Cyc_Absyn_Exp*
_temp1037; struct Cyc_Absyn_Stmt* _temp1039; struct _tuple2 _temp1041; struct
Cyc_Absyn_Exp* _temp1043; struct _tagged_arr* _temp1045; struct Cyc_Absyn_Stmt*
_temp1047; struct _tuple2 _temp1049; struct Cyc_Absyn_Exp* _temp1051; struct
_tuple2 _temp1053; struct Cyc_Absyn_Exp* _temp1055; struct Cyc_Absyn_Exp*
_temp1057; struct Cyc_List_List* _temp1059; struct Cyc_Absyn_Exp* _temp1061;
struct Cyc_List_List* _temp1063; struct Cyc_Absyn_Exp* _temp1065; struct Cyc_List_List*
_temp1067; struct Cyc_List_List* _temp1069; struct Cyc_Absyn_Stmt* _temp1071;
struct Cyc_Absyn_Decl* _temp1073; struct Cyc_Absyn_Stmt* _temp1075; struct Cyc_Absyn_Stmt*
_temp1077; struct Cyc_Absyn_Stmt* _temp1079; struct _tagged_arr* _temp1081;
struct _tuple2 _temp1083; struct Cyc_Absyn_Exp* _temp1085; struct Cyc_Absyn_Stmt*
_temp1087; struct Cyc_List_List* _temp1089; struct Cyc_Absyn_Stmt* _temp1091;
struct Cyc_Absyn_Stmt* _temp1093; struct Cyc_Absyn_Vardecl* _temp1095; struct
Cyc_Absyn_Tvar* _temp1097; _LL983: if( _temp981 ==( void*) Cyc_Absyn_Skip_s){
goto _LL984;} else{ goto _LL985;} _LL985: if(( unsigned int) _temp981 > 1u?*((
int*) _temp981) == Cyc_Absyn_Exp_s: 0){ _LL1026: _temp1025=(( struct Cyc_Absyn_Exp_s_struct*)
_temp981)->f1; goto _LL986;} else{ goto _LL987;} _LL987: if(( unsigned int)
_temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Seq_s: 0){ _LL1030: _temp1029=((
struct Cyc_Absyn_Seq_s_struct*) _temp981)->f1; goto _LL1028; _LL1028: _temp1027=((
struct Cyc_Absyn_Seq_s_struct*) _temp981)->f2; goto _LL988;} else{ goto _LL989;}
_LL989: if(( unsigned int) _temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Return_s:
0){ _LL1032: _temp1031=(( struct Cyc_Absyn_Return_s_struct*) _temp981)->f1; goto
_LL990;} else{ goto _LL991;} _LL991: if(( unsigned int) _temp981 > 1u?*(( int*)
_temp981) == Cyc_Absyn_IfThenElse_s: 0){ _LL1038: _temp1037=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp981)->f1; goto _LL1036; _LL1036: _temp1035=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp981)->f2; goto _LL1034; _LL1034: _temp1033=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp981)->f3; goto _LL992;} else{ goto _LL993;} _LL993: if(( unsigned int)
_temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_While_s: 0){ _LL1042: _temp1041=((
struct Cyc_Absyn_While_s_struct*) _temp981)->f1; _LL1044: _temp1043= _temp1041.f1;
goto _LL1040; _LL1040: _temp1039=(( struct Cyc_Absyn_While_s_struct*) _temp981)->f2;
goto _LL994;} else{ goto _LL995;} _LL995: if(( unsigned int) _temp981 > 1u?*((
int*) _temp981) == Cyc_Absyn_Break_s: 0){ goto _LL996;} else{ goto _LL997;}
_LL997: if(( unsigned int) _temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Continue_s:
0){ goto _LL998;} else{ goto _LL999;} _LL999: if(( unsigned int) _temp981 > 1u?*((
int*) _temp981) == Cyc_Absyn_Goto_s: 0){ _LL1046: _temp1045=(( struct Cyc_Absyn_Goto_s_struct*)
_temp981)->f1; goto _LL1000;} else{ goto _LL1001;} _LL1001: if(( unsigned int)
_temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_For_s: 0){ _LL1058: _temp1057=((
struct Cyc_Absyn_For_s_struct*) _temp981)->f1; goto _LL1054; _LL1054: _temp1053=((
struct Cyc_Absyn_For_s_struct*) _temp981)->f2; _LL1056: _temp1055= _temp1053.f1;
goto _LL1050; _LL1050: _temp1049=(( struct Cyc_Absyn_For_s_struct*) _temp981)->f3;
_LL1052: _temp1051= _temp1049.f1; goto _LL1048; _LL1048: _temp1047=(( struct Cyc_Absyn_For_s_struct*)
_temp981)->f4; goto _LL1002;} else{ goto _LL1003;} _LL1003: if(( unsigned int)
_temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Switch_s: 0){ _LL1062: _temp1061=((
struct Cyc_Absyn_Switch_s_struct*) _temp981)->f1; goto _LL1060; _LL1060:
_temp1059=(( struct Cyc_Absyn_Switch_s_struct*) _temp981)->f2; goto _LL1004;}
else{ goto _LL1005;} _LL1005: if(( unsigned int) _temp981 > 1u?*(( int*)
_temp981) == Cyc_Absyn_SwitchC_s: 0){ _LL1066: _temp1065=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp981)->f1; goto _LL1064; _LL1064: _temp1063=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp981)->f2; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(( unsigned int)
_temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Fallthru_s: 0){ _LL1068:
_temp1067=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp981)->f1; if( _temp1067
== 0){ goto _LL1008;} else{ goto _LL1009;}} else{ goto _LL1009;} _LL1009: if((
unsigned int) _temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Fallthru_s: 0){
_LL1070: _temp1069=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp981)->f1; goto
_LL1010;} else{ goto _LL1011;} _LL1011: if(( unsigned int) _temp981 > 1u?*(( int*)
_temp981) == Cyc_Absyn_Decl_s: 0){ _LL1074: _temp1073=(( struct Cyc_Absyn_Decl_s_struct*)
_temp981)->f1; goto _LL1072; _LL1072: _temp1071=(( struct Cyc_Absyn_Decl_s_struct*)
_temp981)->f2; goto _LL1012;} else{ goto _LL1013;} _LL1013: if(( unsigned int)
_temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Cut_s: 0){ _LL1076: _temp1075=((
struct Cyc_Absyn_Cut_s_struct*) _temp981)->f1; goto _LL1014;} else{ goto _LL1015;}
_LL1015: if(( unsigned int) _temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Splice_s:
0){ _LL1078: _temp1077=(( struct Cyc_Absyn_Splice_s_struct*) _temp981)->f1; goto
_LL1016;} else{ goto _LL1017;} _LL1017: if(( unsigned int) _temp981 > 1u?*(( int*)
_temp981) == Cyc_Absyn_Label_s: 0){ _LL1082: _temp1081=(( struct Cyc_Absyn_Label_s_struct*)
_temp981)->f1; goto _LL1080; _LL1080: _temp1079=(( struct Cyc_Absyn_Label_s_struct*)
_temp981)->f2; goto _LL1018;} else{ goto _LL1019;} _LL1019: if(( unsigned int)
_temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Do_s: 0){ _LL1088: _temp1087=((
struct Cyc_Absyn_Do_s_struct*) _temp981)->f1; goto _LL1084; _LL1084: _temp1083=((
struct Cyc_Absyn_Do_s_struct*) _temp981)->f2; _LL1086: _temp1085= _temp1083.f1;
goto _LL1020;} else{ goto _LL1021;} _LL1021: if(( unsigned int) _temp981 > 1u?*((
int*) _temp981) == Cyc_Absyn_TryCatch_s: 0){ _LL1092: _temp1091=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp981)->f1; goto _LL1090; _LL1090: _temp1089=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp981)->f2; goto _LL1022;} else{ goto _LL1023;} _LL1023: if(( unsigned int)
_temp981 > 1u?*(( int*) _temp981) == Cyc_Absyn_Region_s: 0){ _LL1098: _temp1097=((
struct Cyc_Absyn_Region_s_struct*) _temp981)->f1; goto _LL1096; _LL1096:
_temp1095=(( struct Cyc_Absyn_Region_s_struct*) _temp981)->f2; goto _LL1094;
_LL1094: _temp1093=(( struct Cyc_Absyn_Region_s_struct*) _temp981)->f3; goto
_LL1024;} else{ goto _LL982;} _LL984: s= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); goto _LL982; _LL986: s=({ struct Cyc_PP_Doc* _temp1099[ 2u];
_temp1099[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
_temp1099[ 0u]= Cyc_Absynpp_exp2doc( _temp1025); Cyc_PP_cat( _tag_arr( _temp1099,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL982; _LL988: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1029)){ s=({ struct Cyc_PP_Doc* _temp1100[
6u]; _temp1100[ 5u]= Cyc_Absynpp_is_declaration( _temp1027)?({ struct Cyc_PP_Doc*
_temp1101[ 4u]; _temp1101[ 3u]= Cyc_PP_line_doc(); _temp1101[ 2u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1101[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1027)); _temp1101[ 0u]= Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1101, sizeof( struct Cyc_PP_Doc*), 4u));}): Cyc_Absynpp_stmt2doc(
_temp1027); _temp1100[ 4u]= Cyc_PP_line_doc(); _temp1100[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1100[ 2u]= Cyc_PP_line_doc();
_temp1100[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1029)); _temp1100[ 0u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr(
_temp1100, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ if( Cyc_Absynpp_is_declaration(
_temp1027)){ s=({ struct Cyc_PP_Doc* _temp1102[ 6u]; _temp1102[ 5u]= Cyc_PP_line_doc();
_temp1102[ 4u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1102[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1027)); _temp1102[ 2u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); _temp1102[ 1u]= Cyc_PP_line_doc();
_temp1102[ 0u]= Cyc_Absynpp_stmt2doc( _temp1029); Cyc_PP_cat( _tag_arr(
_temp1102, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1103=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1103->hd=( void*) _temp1029;
_temp1103->tl=({ struct Cyc_List_List* _temp1104=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1104->hd=( void*) _temp1027;
_temp1104->tl= 0; _temp1104;}); _temp1103;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1105=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1105->hd=( void*) _temp1029;
_temp1105->tl=({ struct Cyc_List_List* _temp1106=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1106->hd=( void*) _temp1027;
_temp1106->tl= 0; _temp1106;}); _temp1105;}));} goto _LL982; _LL990: if(
_temp1031 == 0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc* _temp1107[ 3u]; _temp1107[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1107[ 1u]= _temp1031 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1031)); _temp1107[
0u]= Cyc_PP_text( _tag_arr("return ", sizeof( unsigned char), 8u)); Cyc_PP_cat(
_tag_arr( _temp1107, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL982; _LL992: {
int print_else;{ void* _temp1108=( void*) _temp1033->r; _LL1110: if( _temp1108
==( void*) Cyc_Absyn_Skip_s){ goto _LL1111;} else{ goto _LL1112;} _LL1112: goto
_LL1113; _LL1111: print_else= 0; goto _LL1109; _LL1113: print_else= 1; goto
_LL1109; _LL1109:;} s=({ struct Cyc_PP_Doc* _temp1114[ 7u]; _temp1114[ 6u]=
print_else?({ struct Cyc_PP_Doc* _temp1116[ 5u]; _temp1116[ 4u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1116[ 3u]= Cyc_PP_line_doc();
_temp1116[ 2u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1117[ 2u]; _temp1117[
1u]= Cyc_Absynpp_stmt2doc( _temp1033); _temp1117[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1117, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1116[ 1u]= Cyc_PP_text(
_tag_arr("else {", sizeof( unsigned char), 7u)); _temp1116[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1116, sizeof( struct Cyc_PP_Doc*), 5u));}): Cyc_PP_nil_doc();
_temp1114[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1114[ 4u]= Cyc_PP_line_doc(); _temp1114[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1115[ 2u]; _temp1115[ 1u]= Cyc_Absynpp_stmt2doc( _temp1035); _temp1115[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1115, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1114[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1114[ 1u]= Cyc_Absynpp_exp2doc( _temp1037); _temp1114[ 0u]= Cyc_PP_text(
_tag_arr("if (", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1114,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL982;} _LL994: s=({ struct Cyc_PP_Doc*
_temp1118[ 6u]; _temp1118[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1118[ 4u]= Cyc_PP_line_doc(); _temp1118[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1119[ 2u]; _temp1119[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1039); _temp1119[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1119,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1118[ 2u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1118[ 1u]= Cyc_Absynpp_exp2doc( _temp1043);
_temp1118[ 0u]= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp1118, sizeof( struct Cyc_PP_Doc*), 6u));}); goto
_LL982; _LL996: s= Cyc_PP_text( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL982; _LL998: s= Cyc_PP_text( _tag_arr("continue;", sizeof( unsigned char),
10u)); goto _LL982; _LL1000: s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp1121; _temp1121.tag= Cyc_Stdio_String_pa; _temp1121.f1=( struct _tagged_arr)*
_temp1045;{ void* _temp1120[ 1u]={& _temp1121}; Cyc_Stdio_aprintf( _tag_arr("goto %s;",
sizeof( unsigned char), 9u), _tag_arr( _temp1120, sizeof( void*), 1u));}}));
goto _LL982; _LL1002: s=({ struct Cyc_PP_Doc* _temp1122[ 10u]; _temp1122[ 9u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1122[ 8u]= Cyc_PP_line_doc();
_temp1122[ 7u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1123[ 2u]; _temp1123[
1u]= Cyc_Absynpp_stmt2doc( _temp1047); _temp1123[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1123, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1122[ 6u]= Cyc_PP_text(
_tag_arr(") {", sizeof( unsigned char), 4u)); _temp1122[ 5u]= Cyc_Absynpp_exp2doc(
_temp1051); _temp1122[ 4u]= Cyc_PP_text( _tag_arr("; ", sizeof( unsigned char),
3u)); _temp1122[ 3u]= Cyc_Absynpp_exp2doc( _temp1055); _temp1122[ 2u]= Cyc_PP_text(
_tag_arr("; ", sizeof( unsigned char), 3u)); _temp1122[ 1u]= Cyc_Absynpp_exp2doc(
_temp1057); _temp1122[ 0u]= Cyc_PP_text( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1122, sizeof( struct Cyc_PP_Doc*), 10u));});
goto _LL982; _LL1004: s=({ struct Cyc_PP_Doc* _temp1124[ 7u]; _temp1124[ 6u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1124[ 5u]= Cyc_PP_line_doc();
_temp1124[ 4u]= Cyc_Absynpp_switchclauses2doc( _temp1059); _temp1124[ 3u]= Cyc_PP_line_doc();
_temp1124[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1124[ 1u]= Cyc_Absynpp_exp2doc( _temp1061); _temp1124[ 0u]= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp1124, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL982; _LL1006: s=({
struct Cyc_PP_Doc* _temp1125[ 7u]; _temp1125[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1125[ 5u]= Cyc_PP_line_doc(); _temp1125[ 4u]=
Cyc_Absynpp_switchCclauses2doc( _temp1063); _temp1125[ 3u]= Cyc_PP_line_doc();
_temp1125[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1125[ 1u]= Cyc_Absynpp_exp2doc( _temp1065); _temp1125[ 0u]= Cyc_PP_text(
_tag_arr("switch \"C\" (", sizeof( unsigned char), 13u)); Cyc_PP_cat( _tag_arr(
_temp1125, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL982; _LL1008: s= Cyc_PP_text(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL982; _LL1010: s=({
struct Cyc_PP_Doc* _temp1126[ 3u]; _temp1126[ 2u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1126[ 1u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp1069); _temp1126[ 0u]= Cyc_PP_text( _tag_arr("fallthru(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1126, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL982; _LL1012: s=({ struct Cyc_PP_Doc* _temp1127[ 3u]; _temp1127[
2u]= Cyc_Absynpp_stmt2doc( _temp1071); _temp1127[ 1u]= Cyc_PP_line_doc();
_temp1127[ 0u]= Cyc_Absynpp_decl2doc( _temp1073); Cyc_PP_cat( _tag_arr(
_temp1127, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL982; _LL1014: s=({
struct Cyc_PP_Doc* _temp1128[ 2u]; _temp1128[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1075)); _temp1128[ 0u]= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1128, sizeof( struct Cyc_PP_Doc*), 2u));});
goto _LL982; _LL1016: s=({ struct Cyc_PP_Doc* _temp1129[ 2u]; _temp1129[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1077)); _temp1129[ 0u]= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr(
_temp1129, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL982; _LL1018: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1079): 0){ s=({ struct Cyc_PP_Doc* _temp1130[ 6u];
_temp1130[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1130[ 4u]= Cyc_PP_line_doc(); _temp1130[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1079)); _temp1130[ 2u]= Cyc_PP_line_doc(); _temp1130[ 1u]= Cyc_PP_text(
_tag_arr(": {", sizeof( unsigned char), 4u)); _temp1130[ 0u]= Cyc_PP_textptr(
_temp1081); Cyc_PP_cat( _tag_arr( _temp1130, sizeof( struct Cyc_PP_Doc*), 6u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1131[ 3u]; _temp1131[ 2u]= Cyc_Absynpp_stmt2doc(
_temp1079); _temp1131[ 1u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); _temp1131[ 0u]= Cyc_PP_textptr( _temp1081); Cyc_PP_cat( _tag_arr(
_temp1131, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL982; _LL1020: s=({
struct Cyc_PP_Doc* _temp1132[ 7u]; _temp1132[ 6u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1132[ 5u]= Cyc_Absynpp_exp2doc( _temp1085);
_temp1132[ 4u]= Cyc_PP_text( _tag_arr("} while (", sizeof( unsigned char), 10u));
_temp1132[ 3u]= Cyc_PP_line_doc(); _temp1132[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1087)); _temp1132[ 1u]= Cyc_PP_line_doc(); _temp1132[ 0u]= Cyc_PP_text(
_tag_arr("do {", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1132,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL982; _LL1022: s=({ struct Cyc_PP_Doc*
_temp1133[ 9u]; _temp1133[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1133[ 7u]= Cyc_PP_line_doc(); _temp1133[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1089)); _temp1133[ 5u]= Cyc_PP_line_doc(); _temp1133[ 4u]= Cyc_PP_text(
_tag_arr("} catch {", sizeof( unsigned char), 10u)); _temp1133[ 3u]= Cyc_PP_line_doc();
_temp1133[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1091)); _temp1133[ 1u]=
Cyc_PP_line_doc(); _temp1133[ 0u]= Cyc_PP_text( _tag_arr("try {", sizeof(
unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1133, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL982; _LL1024: s=({ struct Cyc_PP_Doc* _temp1134[ 9u]; _temp1134[
8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1134[ 7u]=
Cyc_PP_line_doc(); _temp1134[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1093)); _temp1134[ 5u]= Cyc_PP_line_doc(); _temp1134[ 4u]= Cyc_PP_text(
_tag_arr("{", sizeof( unsigned char), 2u)); _temp1134[ 3u]= Cyc_Absynpp_qvar2doc(
_temp1095->name); _temp1134[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); _temp1134[ 1u]= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1097)); _temp1134[ 0u]= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp1134, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL982; _LL982:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1135=( void*) p->r;
int _temp1167; void* _temp1169; unsigned char _temp1171; struct _tagged_arr
_temp1173; struct Cyc_Absyn_Vardecl* _temp1175; struct Cyc_List_List* _temp1177;
struct Cyc_Absyn_Pat* _temp1179; struct Cyc_Absyn_Vardecl* _temp1181; struct
_tuple0* _temp1183; struct Cyc_List_List* _temp1185; struct Cyc_List_List*
_temp1187; struct _tuple0* _temp1189; struct Cyc_List_List* _temp1191; struct
Cyc_List_List* _temp1193; struct _tuple0* _temp1195; struct Cyc_List_List*
_temp1197; struct Cyc_List_List* _temp1199; struct Cyc_Absyn_Structdecl*
_temp1201; struct Cyc_Absyn_Enumfield* _temp1203; struct Cyc_List_List*
_temp1205; struct Cyc_List_List* _temp1207; struct Cyc_Absyn_Tunionfield*
_temp1209; _LL1137: if( _temp1135 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1138;}
else{ goto _LL1139;} _LL1139: if( _temp1135 ==( void*) Cyc_Absyn_Null_p){ goto
_LL1140;} else{ goto _LL1141;} _LL1141: if(( unsigned int) _temp1135 > 2u?*((
int*) _temp1135) == Cyc_Absyn_Int_p: 0){ _LL1170: _temp1169=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1135)->f1; goto _LL1168; _LL1168: _temp1167=((
struct Cyc_Absyn_Int_p_struct*) _temp1135)->f2; goto _LL1142;} else{ goto
_LL1143;} _LL1143: if(( unsigned int) _temp1135 > 2u?*(( int*) _temp1135) == Cyc_Absyn_Char_p:
0){ _LL1172: _temp1171=(( struct Cyc_Absyn_Char_p_struct*) _temp1135)->f1; goto
_LL1144;} else{ goto _LL1145;} _LL1145: if(( unsigned int) _temp1135 > 2u?*((
int*) _temp1135) == Cyc_Absyn_Float_p: 0){ _LL1174: _temp1173=(( struct Cyc_Absyn_Float_p_struct*)
_temp1135)->f1; goto _LL1146;} else{ goto _LL1147;} _LL1147: if(( unsigned int)
_temp1135 > 2u?*(( int*) _temp1135) == Cyc_Absyn_Var_p: 0){ _LL1176: _temp1175=((
struct Cyc_Absyn_Var_p_struct*) _temp1135)->f1; goto _LL1148;} else{ goto
_LL1149;} _LL1149: if(( unsigned int) _temp1135 > 2u?*(( int*) _temp1135) == Cyc_Absyn_Tuple_p:
0){ _LL1178: _temp1177=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1135)->f1; goto
_LL1150;} else{ goto _LL1151;} _LL1151: if(( unsigned int) _temp1135 > 2u?*((
int*) _temp1135) == Cyc_Absyn_Pointer_p: 0){ _LL1180: _temp1179=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1135)->f1; goto _LL1152;} else{ goto _LL1153;} _LL1153: if(( unsigned int)
_temp1135 > 2u?*(( int*) _temp1135) == Cyc_Absyn_Reference_p: 0){ _LL1182:
_temp1181=(( struct Cyc_Absyn_Reference_p_struct*) _temp1135)->f1; goto _LL1154;}
else{ goto _LL1155;} _LL1155: if(( unsigned int) _temp1135 > 2u?*(( int*)
_temp1135) == Cyc_Absyn_UnknownId_p: 0){ _LL1184: _temp1183=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1135)->f1; goto _LL1156;} else{ goto _LL1157;} _LL1157: if(( unsigned int)
_temp1135 > 2u?*(( int*) _temp1135) == Cyc_Absyn_UnknownCall_p: 0){ _LL1190:
_temp1189=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1135)->f1; goto
_LL1188; _LL1188: _temp1187=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1135)->f2;
goto _LL1186; _LL1186: _temp1185=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1135)->f3; goto _LL1158;} else{ goto _LL1159;} _LL1159: if(( unsigned int)
_temp1135 > 2u?*(( int*) _temp1135) == Cyc_Absyn_UnknownFields_p: 0){ _LL1196:
_temp1195=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1135)->f1; goto
_LL1194; _LL1194: _temp1193=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1135)->f2; goto _LL1192; _LL1192: _temp1191=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1135)->f3; goto _LL1160;} else{ goto _LL1161;} _LL1161: if(( unsigned int)
_temp1135 > 2u?*(( int*) _temp1135) == Cyc_Absyn_Struct_p: 0){ _LL1202:
_temp1201=(( struct Cyc_Absyn_Struct_p_struct*) _temp1135)->f1; goto _LL1200;
_LL1200: _temp1199=(( struct Cyc_Absyn_Struct_p_struct*) _temp1135)->f3; goto
_LL1198; _LL1198: _temp1197=(( struct Cyc_Absyn_Struct_p_struct*) _temp1135)->f4;
goto _LL1162;} else{ goto _LL1163;} _LL1163: if(( unsigned int) _temp1135 > 2u?*((
int*) _temp1135) == Cyc_Absyn_Enum_p: 0){ _LL1204: _temp1203=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1135)->f2; goto _LL1164;} else{ goto _LL1165;} _LL1165: if(( unsigned int)
_temp1135 > 2u?*(( int*) _temp1135) == Cyc_Absyn_Tunion_p: 0){ _LL1210:
_temp1209=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1135)->f2; goto _LL1208;
_LL1208: _temp1207=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1135)->f3; goto
_LL1206; _LL1206: _temp1205=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1135)->f4;
goto _LL1166;} else{ goto _LL1136;} _LL1138: s= Cyc_PP_text( _tag_arr("_",
sizeof( unsigned char), 2u)); goto _LL1136; _LL1140: s= Cyc_PP_text( _tag_arr("null",
sizeof( unsigned char), 5u)); goto _LL1136; _LL1142: if( _temp1169 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp1212; _temp1212.tag= Cyc_Stdio_Int_pa; _temp1212.f1=( int)(( unsigned int)
_temp1167);{ void* _temp1211[ 1u]={& _temp1212}; Cyc_Stdio_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp1211, sizeof( void*), 1u));}}));}
else{ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp1214; _temp1214.tag= Cyc_Stdio_Int_pa; _temp1214.f1=( unsigned int)
_temp1167;{ void* _temp1213[ 1u]={& _temp1214}; Cyc_Stdio_aprintf( _tag_arr("%u",
sizeof( unsigned char), 3u), _tag_arr( _temp1213, sizeof( void*), 1u));}}));}
goto _LL1136; _LL1144: s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp1216; _temp1216.tag= Cyc_Stdio_String_pa; _temp1216.f1=( struct _tagged_arr)
Cyc_Absynpp_char_escape( _temp1171);{ void* _temp1215[ 1u]={& _temp1216}; Cyc_Stdio_aprintf(
_tag_arr("'%s'", sizeof( unsigned char), 5u), _tag_arr( _temp1215, sizeof( void*),
1u));}})); goto _LL1136; _LL1146: s= Cyc_PP_text( _temp1173); goto _LL1136;
_LL1148: s= Cyc_Absynpp_qvar2doc( _temp1175->name); goto _LL1136; _LL1150: s=({
struct Cyc_PP_Doc* _temp1217[ 3u]; _temp1217[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp1217[ 1u]=(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc, _tag_arr(",", sizeof( unsigned char), 2u),
_temp1177); _temp1217[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1217, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1136; _LL1152: s=({ struct Cyc_PP_Doc* _temp1218[ 2u]; _temp1218[ 1u]=
Cyc_Absynpp_pat2doc( _temp1179); _temp1218[ 0u]= Cyc_PP_text( _tag_arr("&",
sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1218, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL1136; _LL1154: s=({ struct Cyc_PP_Doc* _temp1219[
2u]; _temp1219[ 1u]= Cyc_Absynpp_qvar2doc( _temp1181->name); _temp1219[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1219,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1136; _LL1156: s= Cyc_Absynpp_qvar2doc(
_temp1183); goto _LL1136; _LL1158: s=({ struct Cyc_PP_Doc* _temp1220[ 3u];
_temp1220[ 2u]= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1185));
_temp1220[ 1u]= Cyc_Absynpp_tvars2doc( _temp1187); _temp1220[ 0u]= Cyc_Absynpp_qvar2doc(
_temp1189); Cyc_PP_cat( _tag_arr( _temp1220, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1136; _LL1160: s=({ struct Cyc_PP_Doc* _temp1221[ 3u]; _temp1221[ 2u]=
Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1191)); _temp1221[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1193); _temp1221[ 0u]= Cyc_Absynpp_qvar2doc( _temp1195); Cyc_PP_cat(
_tag_arr( _temp1221, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1136; _LL1162:
s=({ struct Cyc_PP_Doc* _temp1222[ 3u]; _temp1222[ 2u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1197)); _temp1222[ 1u]= Cyc_Absynpp_tvars2doc( _temp1199); _temp1222[ 0u]=
_temp1201->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1201->name))->v); Cyc_PP_cat( _tag_arr(
_temp1222, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1136; _LL1164: s= Cyc_Absynpp_qvar2doc(
_temp1203->name); goto _LL1136; _LL1166: if( _temp1205 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1209->name);} else{ s=({ struct Cyc_PP_Doc* _temp1223[ 3u]; _temp1223[ 2u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1205)); _temp1223[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1207); _temp1223[ 0u]= Cyc_Absynpp_qvar2doc( _temp1209->name); Cyc_PP_cat(
_tag_arr( _temp1223, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1136;
_LL1136:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc* _temp1224[ 2u]; _temp1224[ 1u]= Cyc_Absynpp_pat2doc((*
dp).f2); _temp1224[ 0u]= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)); Cyc_PP_cat( _tag_arr(
_temp1224, sizeof( struct Cyc_PP_Doc*), 2u));});} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
==( void*) Cyc_Absyn_Wild_p: 0){ return({ struct Cyc_PP_Doc* _temp1225[ 2u];
_temp1225[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1226[ 2u]; _temp1226[
1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1226[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1226, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1225[ 0u]= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1225, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ if( c->where_clause == 0){
return({ struct Cyc_PP_Doc* _temp1227[ 4u]; _temp1227[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1228[ 2u]; _temp1228[ 1u]= Cyc_Absynpp_stmt2doc( c->body);
_temp1228[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1228, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1227[ 2u]= Cyc_PP_text( _tag_arr(": ",
sizeof( unsigned char), 3u)); _temp1227[ 1u]= Cyc_Absynpp_pat2doc( c->pattern);
_temp1227[ 0u]= Cyc_PP_text( _tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp1227, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ return({
struct Cyc_PP_Doc* _temp1229[ 6u]; _temp1229[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1230[ 2u]; _temp1230[ 1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1230[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1230, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1229[ 4u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
_temp1229[ 3u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( c->where_clause));
_temp1229[ 2u]= Cyc_PP_text( _tag_arr(" && ", sizeof( unsigned char), 5u));
_temp1229[ 1u]= Cyc_Absynpp_pat2doc( c->pattern); _temp1229[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1229,
sizeof( struct Cyc_PP_Doc*), 6u));});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1233;
struct Cyc_Absyn_Stmt* _temp1234; struct Cyc_Absyn_Exp* _temp1236; struct Cyc_Absyn_SwitchC_clause*
_temp1231= c; _temp1233=* _temp1231; _LL1237: _temp1236= _temp1233.cnst_exp;
goto _LL1235; _LL1235: _temp1234= _temp1233.body; goto _LL1232; _LL1232: if(
_temp1236 == 0){ return({ struct Cyc_PP_Doc* _temp1238[ 2u]; _temp1238[ 1u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1239[ 2u]; _temp1239[ 1u]= Cyc_Absynpp_stmt2doc( c->body);
_temp1239[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1239, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1238[ 0u]= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1238, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp1240[ 4u];
_temp1240[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1241[ 2u]; _temp1241[
1u]= Cyc_Absynpp_stmt2doc( _temp1234); _temp1241[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1241, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1240[ 2u]= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); _temp1240[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1236)); _temp1240[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1240,
sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc* _temp1242[ 3u]; _temp1242[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->tag)); _temp1242[ 1u]= Cyc_PP_text(
_tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1242[ 0u]= Cyc_Absynpp_qvar2doc(
f->name); Cyc_PP_cat( _tag_arr( _temp1242, sizeof( struct Cyc_PP_Doc*), 3u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,
_tag_arr(",", sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1243=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1269; struct Cyc_Absyn_Structdecl* _temp1271;
struct Cyc_Absyn_Uniondecl* _temp1273; struct Cyc_Absyn_Vardecl* _temp1275;
struct Cyc_Absyn_Vardecl _temp1277; struct Cyc_List_List* _temp1278; struct Cyc_Absyn_Exp*
_temp1280; void* _temp1282; struct Cyc_Absyn_Tqual _temp1284; struct _tuple0*
_temp1286; void* _temp1288; struct Cyc_Absyn_Tuniondecl* _temp1290; struct Cyc_Absyn_Tuniondecl
_temp1292; int _temp1293; struct Cyc_Core_Opt* _temp1295; struct Cyc_List_List*
_temp1297; struct _tuple0* _temp1299; void* _temp1301; struct Cyc_Absyn_Exp*
_temp1303; struct Cyc_Absyn_Pat* _temp1305; struct Cyc_List_List* _temp1307;
struct Cyc_Absyn_Enumdecl* _temp1309; struct Cyc_Absyn_Enumdecl _temp1311;
struct Cyc_Core_Opt* _temp1312; struct _tuple0* _temp1314; void* _temp1316;
struct Cyc_Absyn_Typedefdecl* _temp1318; struct Cyc_List_List* _temp1320; struct
_tagged_arr* _temp1322; struct Cyc_List_List* _temp1324; struct _tuple0*
_temp1326; struct Cyc_List_List* _temp1328; _LL1245: if(*(( int*) _temp1243) ==
Cyc_Absyn_Fn_d){ _LL1270: _temp1269=(( struct Cyc_Absyn_Fn_d_struct*) _temp1243)->f1;
goto _LL1246;} else{ goto _LL1247;} _LL1247: if(*(( int*) _temp1243) == Cyc_Absyn_Struct_d){
_LL1272: _temp1271=(( struct Cyc_Absyn_Struct_d_struct*) _temp1243)->f1; goto
_LL1248;} else{ goto _LL1249;} _LL1249: if(*(( int*) _temp1243) == Cyc_Absyn_Union_d){
_LL1274: _temp1273=(( struct Cyc_Absyn_Union_d_struct*) _temp1243)->f1; goto
_LL1250;} else{ goto _LL1251;} _LL1251: if(*(( int*) _temp1243) == Cyc_Absyn_Var_d){
_LL1276: _temp1275=(( struct Cyc_Absyn_Var_d_struct*) _temp1243)->f1; _temp1277=*
_temp1275; _LL1289: _temp1288=( void*) _temp1277.sc; goto _LL1287; _LL1287:
_temp1286= _temp1277.name; goto _LL1285; _LL1285: _temp1284= _temp1277.tq; goto
_LL1283; _LL1283: _temp1282=( void*) _temp1277.type; goto _LL1281; _LL1281:
_temp1280= _temp1277.initializer; goto _LL1279; _LL1279: _temp1278= _temp1277.attributes;
goto _LL1252;} else{ goto _LL1253;} _LL1253: if(*(( int*) _temp1243) == Cyc_Absyn_Tunion_d){
_LL1291: _temp1290=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1243)->f1;
_temp1292=* _temp1290; _LL1302: _temp1301=( void*) _temp1292.sc; goto _LL1300;
_LL1300: _temp1299= _temp1292.name; goto _LL1298; _LL1298: _temp1297= _temp1292.tvs;
goto _LL1296; _LL1296: _temp1295= _temp1292.fields; goto _LL1294; _LL1294:
_temp1293= _temp1292.is_xtunion; goto _LL1254;} else{ goto _LL1255;} _LL1255:
if(*(( int*) _temp1243) == Cyc_Absyn_Let_d){ _LL1306: _temp1305=(( struct Cyc_Absyn_Let_d_struct*)
_temp1243)->f1; goto _LL1304; _LL1304: _temp1303=(( struct Cyc_Absyn_Let_d_struct*)
_temp1243)->f4; goto _LL1256;} else{ goto _LL1257;} _LL1257: if(*(( int*)
_temp1243) == Cyc_Absyn_Letv_d){ _LL1308: _temp1307=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1243)->f1; goto _LL1258;} else{ goto _LL1259;} _LL1259: if(*(( int*)
_temp1243) == Cyc_Absyn_Enum_d){ _LL1310: _temp1309=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1243)->f1; _temp1311=* _temp1309; _LL1317: _temp1316=( void*) _temp1311.sc;
goto _LL1315; _LL1315: _temp1314= _temp1311.name; goto _LL1313; _LL1313:
_temp1312= _temp1311.fields; goto _LL1260;} else{ goto _LL1261;} _LL1261: if(*((
int*) _temp1243) == Cyc_Absyn_Typedef_d){ _LL1319: _temp1318=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1243)->f1; goto _LL1262;} else{ goto _LL1263;} _LL1263: if(*(( int*)
_temp1243) == Cyc_Absyn_Namespace_d){ _LL1323: _temp1322=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1243)->f1; goto _LL1321; _LL1321: _temp1320=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1243)->f2; goto _LL1264;} else{ goto _LL1265;} _LL1265: if(*(( int*)
_temp1243) == Cyc_Absyn_Using_d){ _LL1327: _temp1326=(( struct Cyc_Absyn_Using_d_struct*)
_temp1243)->f1; goto _LL1325; _LL1325: _temp1324=(( struct Cyc_Absyn_Using_d_struct*)
_temp1243)->f2; goto _LL1266;} else{ goto _LL1267;} _LL1267: if(*(( int*)
_temp1243) == Cyc_Absyn_ExternC_d){ _LL1329: _temp1328=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1243)->f1; goto _LL1268;} else{ goto _LL1244;} _LL1246: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp1337=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1337[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1338; _temp1338.tag= Cyc_Absyn_FnType; _temp1338.f1=({ struct Cyc_Absyn_FnInfo
_temp1339; _temp1339.tvars= _temp1269->tvs; _temp1339.effect= _temp1269->effect;
_temp1339.ret_typ=( void*)(( void*) _temp1269->ret_type); _temp1339.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1269->args); _temp1339.c_varargs=
_temp1269->c_varargs; _temp1339.cyc_varargs= _temp1269->cyc_varargs; _temp1339.rgn_po=
_temp1269->rgn_po; _temp1339.attributes= 0; _temp1339;}); _temp1338;});
_temp1337;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1269->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1269->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1269->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1269->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1269->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1334; _temp1334.q_const= 0; _temp1334.q_volatile= 0; _temp1334.q_restrict=
0; _temp1334;}), t,({ struct Cyc_Core_Opt* _temp1335=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1335->v=( void*)({ struct Cyc_PP_Doc*
_temp1336[ 2u]; _temp1336[ 1u]= namedoc; _temp1336[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1336, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1335;})); struct
Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc* _temp1332[ 4u]; _temp1332[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1332[ 2u]= Cyc_PP_line_doc();
_temp1332[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1333[ 2u]; _temp1333[
1u]= Cyc_Absynpp_stmt2doc( _temp1269->body); _temp1333[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1333, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1332[ 0u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp1332, sizeof( struct Cyc_PP_Doc*), 4u));}); s=({ struct Cyc_PP_Doc*
_temp1330[ 4u]; _temp1330[ 3u]= bodydoc; _temp1330[ 2u]= tqtddoc; _temp1330[ 1u]=
scopedoc; _temp1330[ 0u]= inlinedoc; Cyc_PP_cat( _tag_arr( _temp1330, sizeof(
struct Cyc_PP_Doc*), 4u));}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*
_temp1331[ 2u]; _temp1331[ 1u]= s; _temp1331[ 0u]= attsdoc; Cyc_PP_cat( _tag_arr(
_temp1331, sizeof( struct Cyc_PP_Doc*), 2u));});} goto _LL1244;}} _LL1248: if(
_temp1271->fields == 0){ s=({ struct Cyc_PP_Doc* _temp1340[ 5u]; _temp1340[ 4u]=
Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp1340[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1271->tvs); _temp1340[ 2u]= _temp1271->name == 0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1271->name))->v); _temp1340[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1340[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1271->sc); Cyc_PP_cat( _tag_arr( _temp1340, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ s=({ struct Cyc_PP_Doc* _temp1341[ 10u]; _temp1341[ 9u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1341[ 8u]= Cyc_Absynpp_atts2doc(
_temp1271->attributes); _temp1341[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); _temp1341[ 6u]= Cyc_PP_line_doc(); _temp1341[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1342[ 2u]; _temp1342[ 1u]= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1271->fields))->v);
_temp1342[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1342, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1341[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1341[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1271->tvs);
_temp1341[ 2u]= _temp1271->name == 0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1271->name))->v); _temp1341[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1341[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1271->sc); Cyc_PP_cat( _tag_arr( _temp1341, sizeof( struct Cyc_PP_Doc*),
10u));});} goto _LL1244; _LL1250: if( _temp1273->fields == 0){ s=({ struct Cyc_PP_Doc*
_temp1343[ 5u]; _temp1343[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1343[ 3u]= Cyc_Absynpp_tvars2doc( _temp1273->tvs); _temp1343[ 2u]=
_temp1273->name == 0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1273->name))->v); _temp1343[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1343[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1273->sc);
Cyc_PP_cat( _tag_arr( _temp1343, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({
struct Cyc_PP_Doc* _temp1344[ 10u]; _temp1344[ 9u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1344[ 8u]= Cyc_Absynpp_atts2doc( _temp1273->attributes);
_temp1344[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1344[ 6u]= Cyc_PP_line_doc(); _temp1344[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1345[ 2u]; _temp1345[ 1u]= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1273->fields))->v); _temp1345[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1345, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1344[ 4u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1344[ 3u]= Cyc_Absynpp_tvars2doc( _temp1273->tvs); _temp1344[ 2u]=
_temp1273->name == 0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1273->name))->v); _temp1344[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1344[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1273->sc);
Cyc_PP_cat( _tag_arr( _temp1344, sizeof( struct Cyc_PP_Doc*), 10u));});} goto
_LL1244; _LL1252: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp1286); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1278); struct
Cyc_PP_Doc* beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;}
else{ void* _temp1346= Cyc_Tcutil_compress( _temp1282); struct Cyc_Absyn_FnInfo
_temp1352; struct Cyc_List_List* _temp1354; _LL1348: if(( unsigned int)
_temp1346 > 4u?*(( int*) _temp1346) == Cyc_Absyn_FnType: 0){ _LL1353: _temp1352=((
struct Cyc_Absyn_FnType_struct*) _temp1346)->f1; _LL1355: _temp1354= _temp1352.attributes;
goto _LL1349;} else{ goto _LL1350;} _LL1350: goto _LL1351; _LL1349:
beforenamedoc= Cyc_Absynpp_callconv2doc( _temp1354); goto _LL1347; _LL1351:
beforenamedoc= Cyc_PP_nil_doc(); goto _LL1347; _LL1347:;} s=({ struct Cyc_PP_Doc*
_temp1356[ 5u]; _temp1356[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1356[ 3u]= _temp1280 == 0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)):({ struct Cyc_PP_Doc* _temp1359[ 2u]; _temp1359[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1280)); _temp1359[ 0u]= Cyc_PP_text(
_tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_PP_cat( _tag_arr( _temp1359,
sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1356[ 2u]= Cyc_Absynpp_tqtd2doc(
_temp1284, _temp1282,({ struct Cyc_Core_Opt* _temp1357=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1357->v=( void*)({ struct Cyc_PP_Doc*
_temp1358[ 2u]; _temp1358[ 1u]= sn; _temp1358[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1358, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1357;}));
_temp1356[ 1u]= Cyc_Absynpp_scope2doc( _temp1288); _temp1356[ 0u]= Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc(); Cyc_PP_cat( _tag_arr( _temp1356, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL1244;} _LL1254: if( _temp1295 == 0){ s=({ struct Cyc_PP_Doc*
_temp1360[ 5u]; _temp1360[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1360[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1297); _temp1360[ 2u]=
_temp1293? Cyc_Absynpp_qvar2doc( _temp1299): Cyc_Absynpp_typedef_name2doc(
_temp1299); _temp1360[ 1u]= _temp1293? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1360[ 0u]= Cyc_Absynpp_scope2doc( _temp1301); Cyc_PP_cat( _tag_arr(
_temp1360, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1361[ 8u]; _temp1361[ 7u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1361[ 6u]= Cyc_PP_line_doc(); _temp1361[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1362[ 2u]; _temp1362[ 1u]= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1295))->v);
_temp1362[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1362, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1361[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1361[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1297);
_temp1361[ 2u]= _temp1293? Cyc_Absynpp_qvar2doc( _temp1299): Cyc_Absynpp_typedef_name2doc(
_temp1299); _temp1361[ 1u]= _temp1293? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1361[ 0u]= Cyc_Absynpp_scope2doc( _temp1301); Cyc_PP_cat( _tag_arr(
_temp1361, sizeof( struct Cyc_PP_Doc*), 8u));});} goto _LL1244; _LL1256: s=({
struct Cyc_PP_Doc* _temp1363[ 5u]; _temp1363[ 4u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1363[ 3u]= Cyc_Absynpp_exp2doc( _temp1303);
_temp1363[ 2u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp1363[ 1u]= Cyc_Absynpp_pat2doc( _temp1305); _temp1363[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1363,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL1244; _LL1258: s=({ struct Cyc_PP_Doc*
_temp1364[ 3u]; _temp1364[ 2u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1364[ 1u]= Cyc_Absynpp_ids2doc( _temp1307); _temp1364[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1364,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1244; _LL1260: if( _temp1312 == 0){
s=({ struct Cyc_PP_Doc* _temp1365[ 4u]; _temp1365[ 3u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1365[ 2u]= Cyc_Absynpp_typedef_name2doc(
_temp1314); _temp1365[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1365[ 0u]= Cyc_Absynpp_scope2doc( _temp1316); Cyc_PP_cat( _tag_arr(
_temp1365, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1366[ 7u]; _temp1366[ 6u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1366[ 5u]= Cyc_PP_line_doc(); _temp1366[ 4u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1367[ 2u]; _temp1367[ 1u]= Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1312))->v);
_temp1367[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1367, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1366[ 3u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1366[ 2u]= Cyc_Absynpp_qvar2doc( _temp1314);
_temp1366[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u));
_temp1366[ 0u]= Cyc_Absynpp_scope2doc( _temp1316); Cyc_PP_cat( _tag_arr(
_temp1366, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1244; _LL1262: s=({
struct Cyc_PP_Doc* _temp1368[ 3u]; _temp1368[ 2u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1368[ 1u]= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1369; _temp1369.q_const= 0; _temp1369.q_volatile= 0; _temp1369.q_restrict=
0; _temp1369;}),( void*) _temp1318->defn,({ struct Cyc_Core_Opt* _temp1370=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1370->v=(
void*)({ struct Cyc_PP_Doc* _temp1371[ 2u]; _temp1371[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1318->tvs); _temp1371[ 0u]= Cyc_Absynpp_typedef_name2doc( _temp1318->name);
Cyc_PP_cat( _tag_arr( _temp1371, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1370;}));
_temp1368[ 0u]= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
Cyc_PP_cat( _tag_arr( _temp1368, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL1244; _LL1264: if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add(
_temp1322);} s=({ struct Cyc_PP_Doc* _temp1372[ 7u]; _temp1372[ 6u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1372[ 5u]= Cyc_PP_line_doc();
_temp1372[ 4u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1320); _temp1372[ 3u]= Cyc_PP_line_doc();
_temp1372[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1372[ 1u]= Cyc_PP_textptr( _temp1322); _temp1372[ 0u]= Cyc_PP_text(
_tag_arr("namespace ", sizeof( unsigned char), 11u)); Cyc_PP_cat( _tag_arr(
_temp1372, sizeof( struct Cyc_PP_Doc*), 7u));}); if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();} goto _LL1244; _LL1266: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc* _temp1373[ 7u]; _temp1373[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1373[ 5u]= Cyc_PP_line_doc(); _temp1373[ 4u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1324); _temp1373[ 3u]= Cyc_PP_line_doc();
_temp1373[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1373[ 1u]= Cyc_Absynpp_qvar2doc( _temp1326); _temp1373[ 0u]= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp1373,
sizeof( struct Cyc_PP_Doc*), 7u));});} else{ s=({ struct Cyc_PP_Doc* _temp1374[
7u]; _temp1374[ 6u]= Cyc_PP_text( _tag_arr("/* } */", sizeof( unsigned char), 8u));
_temp1374[ 5u]= Cyc_PP_line_doc(); _temp1374[ 4u]=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1324); _temp1374[ 3u]= Cyc_PP_line_doc();
_temp1374[ 2u]= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u));
_temp1374[ 1u]= Cyc_Absynpp_qvar2doc( _temp1326); _temp1374[ 0u]= Cyc_PP_text(
_tag_arr("/* using ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1374, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1244; _LL1268: if( Cyc_Absynpp_print_externC_stmts){
s=({ struct Cyc_PP_Doc* _temp1375[ 5u]; _temp1375[ 4u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1375[ 3u]= Cyc_PP_line_doc(); _temp1375[ 2u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1328); _temp1375[ 1u]= Cyc_PP_line_doc();
_temp1375[ 0u]= Cyc_PP_text( _tag_arr("extern \"C\" {", sizeof( unsigned char),
13u)); Cyc_PP_cat( _tag_arr( _temp1375, sizeof( struct Cyc_PP_Doc*), 5u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1376[ 5u]; _temp1376[ 4u]= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); _temp1376[ 3u]= Cyc_PP_line_doc();
_temp1376[ 2u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1328); _temp1376[ 1u]= Cyc_PP_line_doc();
_temp1376[ 0u]= Cyc_PP_text( _tag_arr("/* extern \"C\" { */", sizeof(
unsigned char), 19u)); Cyc_PP_cat( _tag_arr( _temp1376, sizeof( struct Cyc_PP_Doc*),
5u));});} goto _LL1244; _LL1244:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp1377= sc; _LL1379: if( _temp1377 ==( void*) Cyc_Absyn_Static){
goto _LL1380;} else{ goto _LL1381;} _LL1381: if( _temp1377 ==( void*) Cyc_Absyn_Public){
goto _LL1382;} else{ goto _LL1383;} _LL1383: if( _temp1377 ==( void*) Cyc_Absyn_Extern){
goto _LL1384;} else{ goto _LL1385;} _LL1385: if( _temp1377 ==( void*) Cyc_Absyn_ExternC){
goto _LL1386;} else{ goto _LL1387;} _LL1387: if( _temp1377 ==( void*) Cyc_Absyn_Abstract){
goto _LL1388;} else{ goto _LL1378;} _LL1380: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL1382: return Cyc_PP_nil_doc(); _LL1384: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL1386: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL1388:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL1378:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp1389= t; struct
Cyc_Absyn_Tvar* _temp1397; struct Cyc_List_List* _temp1399; _LL1391: if((
unsigned int) _temp1389 > 4u?*(( int*) _temp1389) == Cyc_Absyn_VarType: 0){
_LL1398: _temp1397=(( struct Cyc_Absyn_VarType_struct*) _temp1389)->f1; goto
_LL1392;} else{ goto _LL1393;} _LL1393: if(( unsigned int) _temp1389 > 4u?*((
int*) _temp1389) == Cyc_Absyn_JoinEff: 0){ _LL1400: _temp1399=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1389)->f1; goto _LL1394;} else{ goto _LL1395;} _LL1395: goto _LL1396;
_LL1392: return Cyc_Tcutil_is_temp_tvar( _temp1397); _LL1394: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp1399); _LL1396: return 0; _LL1390:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp1401= t; struct Cyc_Absyn_Exp* _temp1415; struct Cyc_Absyn_Tqual _temp1417;
void* _temp1419; struct Cyc_Absyn_PtrInfo _temp1421; struct Cyc_Absyn_Conref*
_temp1423; struct Cyc_Absyn_Tqual _temp1425; struct Cyc_Absyn_Conref* _temp1427;
void* _temp1429; void* _temp1431; struct Cyc_Absyn_FnInfo _temp1433; struct Cyc_List_List*
_temp1435; struct Cyc_List_List* _temp1437; struct Cyc_Absyn_VarargInfo*
_temp1439; int _temp1441; struct Cyc_List_List* _temp1443; void* _temp1445;
struct Cyc_Core_Opt* _temp1447; struct Cyc_List_List* _temp1449; int _temp1451;
struct Cyc_Core_Opt* _temp1453; struct Cyc_Core_Opt* _temp1455; struct Cyc_Core_Opt*
_temp1457; struct Cyc_List_List* _temp1459; struct _tuple0* _temp1461; _LL1403:
if(( unsigned int) _temp1401 > 4u?*(( int*) _temp1401) == Cyc_Absyn_ArrayType: 0){
_LL1420: _temp1419=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1401)->f1;
goto _LL1418; _LL1418: _temp1417=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1401)->f2; goto _LL1416; _LL1416: _temp1415=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1401)->f3; goto _LL1404;} else{ goto _LL1405;} _LL1405: if(( unsigned int)
_temp1401 > 4u?*(( int*) _temp1401) == Cyc_Absyn_PointerType: 0){ _LL1422:
_temp1421=(( struct Cyc_Absyn_PointerType_struct*) _temp1401)->f1; _LL1432:
_temp1431=( void*) _temp1421.elt_typ; goto _LL1430; _LL1430: _temp1429=( void*)
_temp1421.rgn_typ; goto _LL1428; _LL1428: _temp1427= _temp1421.nullable; goto
_LL1426; _LL1426: _temp1425= _temp1421.tq; goto _LL1424; _LL1424: _temp1423=
_temp1421.bounds; goto _LL1406;} else{ goto _LL1407;} _LL1407: if(( unsigned int)
_temp1401 > 4u?*(( int*) _temp1401) == Cyc_Absyn_FnType: 0){ _LL1434: _temp1433=((
struct Cyc_Absyn_FnType_struct*) _temp1401)->f1; _LL1450: _temp1449= _temp1433.tvars;
goto _LL1448; _LL1448: _temp1447= _temp1433.effect; goto _LL1446; _LL1446:
_temp1445=( void*) _temp1433.ret_typ; goto _LL1444; _LL1444: _temp1443=
_temp1433.args; goto _LL1442; _LL1442: _temp1441= _temp1433.c_varargs; goto
_LL1440; _LL1440: _temp1439= _temp1433.cyc_varargs; goto _LL1438; _LL1438:
_temp1437= _temp1433.rgn_po; goto _LL1436; _LL1436: _temp1435= _temp1433.attributes;
goto _LL1408;} else{ goto _LL1409;} _LL1409: if(( unsigned int) _temp1401 > 4u?*((
int*) _temp1401) == Cyc_Absyn_Evar: 0){ _LL1456: _temp1455=(( struct Cyc_Absyn_Evar_struct*)
_temp1401)->f1; goto _LL1454; _LL1454: _temp1453=(( struct Cyc_Absyn_Evar_struct*)
_temp1401)->f2; goto _LL1452; _LL1452: _temp1451=(( struct Cyc_Absyn_Evar_struct*)
_temp1401)->f3; goto _LL1410;} else{ goto _LL1411;} _LL1411: if(( unsigned int)
_temp1401 > 4u?*(( int*) _temp1401) == Cyc_Absyn_TypedefType: 0){ _LL1462:
_temp1461=(( struct Cyc_Absyn_TypedefType_struct*) _temp1401)->f1; goto _LL1460;
_LL1460: _temp1459=(( struct Cyc_Absyn_TypedefType_struct*) _temp1401)->f2; goto
_LL1458; _LL1458: _temp1457=(( struct Cyc_Absyn_TypedefType_struct*) _temp1401)->f3;
goto _LL1412;} else{ goto _LL1413;} _LL1413: goto _LL1414; _LL1404: { struct Cyc_List_List*
_temp1465; void* _temp1467; struct Cyc_Absyn_Tqual _temp1469; struct _tuple4
_temp1463= Cyc_Absynpp_to_tms( _temp1417, _temp1419); _LL1470: _temp1469=
_temp1463.f1; goto _LL1468; _LL1468: _temp1467= _temp1463.f2; goto _LL1466;
_LL1466: _temp1465= _temp1463.f3; goto _LL1464; _LL1464: { void* tm; if(
_temp1415 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp1471=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1471[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1472; _temp1472.tag= Cyc_Absyn_ConstArray_mod;
_temp1472.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1415); _temp1472;});
_temp1471;});} return({ struct _tuple4 _temp1473; _temp1473.f1= _temp1469;
_temp1473.f2= _temp1467; _temp1473.f3=({ struct Cyc_List_List* _temp1474=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1474->hd=(
void*) tm; _temp1474->tl= _temp1465; _temp1474;}); _temp1473;});}} _LL1406: {
struct Cyc_List_List* _temp1477; void* _temp1479; struct Cyc_Absyn_Tqual
_temp1481; struct _tuple4 _temp1475= Cyc_Absynpp_to_tms( _temp1425, _temp1431);
_LL1482: _temp1481= _temp1475.f1; goto _LL1480; _LL1480: _temp1479= _temp1475.f2;
goto _LL1478; _LL1478: _temp1477= _temp1475.f3; goto _LL1476; _LL1476: { void*
ps;{ struct _tuple8 _temp1484=({ struct _tuple8 _temp1483; _temp1483.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1427))->v; _temp1483.f2=( void*)( Cyc_Absyn_compress_conref( _temp1423))->v;
_temp1483;}); void* _temp1494; void* _temp1496; struct Cyc_Absyn_Exp* _temp1498;
void* _temp1500; int _temp1502; void* _temp1504; void* _temp1506; struct Cyc_Absyn_Exp*
_temp1508; void* _temp1510; int _temp1512; void* _temp1514; void* _temp1516;
_LL1486: _LL1501: _temp1500= _temp1484.f1; if(( unsigned int) _temp1500 > 1u?*((
int*) _temp1500) == Cyc_Absyn_Eq_constr: 0){ _LL1503: _temp1502=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp1500)->f1; if( _temp1502 == 1){ goto _LL1495;}
else{ goto _LL1488;}} else{ goto _LL1488;} _LL1495: _temp1494= _temp1484.f2; if((
unsigned int) _temp1494 > 1u?*(( int*) _temp1494) == Cyc_Absyn_Eq_constr: 0){
_LL1497: _temp1496=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1494)->f1;
if(( unsigned int) _temp1496 > 1u?*(( int*) _temp1496) == Cyc_Absyn_Upper_b: 0){
_LL1499: _temp1498=(( struct Cyc_Absyn_Upper_b_struct*) _temp1496)->f1; goto
_LL1487;} else{ goto _LL1488;}} else{ goto _LL1488;} _LL1488: _LL1511: _temp1510=
_temp1484.f1; if(( unsigned int) _temp1510 > 1u?*(( int*) _temp1510) == Cyc_Absyn_Eq_constr:
0){ _LL1513: _temp1512=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1510)->f1;
if( _temp1512 == 0){ goto _LL1505;} else{ goto _LL1490;}} else{ goto _LL1490;}
_LL1505: _temp1504= _temp1484.f2; if(( unsigned int) _temp1504 > 1u?*(( int*)
_temp1504) == Cyc_Absyn_Eq_constr: 0){ _LL1507: _temp1506=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1504)->f1; if(( unsigned int) _temp1506 > 1u?*(( int*) _temp1506) == Cyc_Absyn_Upper_b:
0){ _LL1509: _temp1508=(( struct Cyc_Absyn_Upper_b_struct*) _temp1506)->f1; goto
_LL1489;} else{ goto _LL1490;}} else{ goto _LL1490;} _LL1490: _LL1515: _temp1514=
_temp1484.f2; if(( unsigned int) _temp1514 > 1u?*(( int*) _temp1514) == Cyc_Absyn_Eq_constr:
0){ _LL1517: _temp1516=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1514)->f1;
if( _temp1516 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1491;} else{ goto _LL1492;}}
else{ goto _LL1492;} _LL1492: goto _LL1493; _LL1487: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp1518=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp1518[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp1519; _temp1519.tag=
Cyc_Absyn_Nullable_ps; _temp1519.f1= _temp1498; _temp1519;}); _temp1518;}); goto
_LL1485; _LL1489: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp1520=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp1520[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp1521; _temp1521.tag= Cyc_Absyn_NonNullable_ps; _temp1521.f1= _temp1508;
_temp1521;}); _temp1520;}); goto _LL1485; _LL1491: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL1485; _LL1493: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL1485;
_LL1485:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp1524=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp1524[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp1525; _temp1525.tag=
Cyc_Absyn_Pointer_mod; _temp1525.f1=( void*) ps; _temp1525.f2=( void*) _temp1429;
_temp1525.f3= tq; _temp1525;}); _temp1524;}); return({ struct _tuple4 _temp1522;
_temp1522.f1= _temp1481; _temp1522.f2= _temp1479; _temp1522.f3=({ struct Cyc_List_List*
_temp1523=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1523->hd=( void*) tm; _temp1523->tl= _temp1477; _temp1523;}); _temp1522;});}}}
_LL1408: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp1447 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1447))->v)){ _temp1447= 0;
_temp1449= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_rewrite_temp_tvar,
_temp1449);}}{ struct Cyc_List_List* _temp1528; void* _temp1530; struct Cyc_Absyn_Tqual
_temp1532; struct _tuple4 _temp1526= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp1445); _LL1533: _temp1532= _temp1526.f1; goto _LL1531; _LL1531: _temp1530=
_temp1526.f2; goto _LL1529; _LL1529: _temp1528= _temp1526.f3; goto _LL1527;
_LL1527: { struct Cyc_List_List* tms= _temp1528; if( _temp1435 != 0? ! Cyc_Absynpp_to_VC:
0){ tms=({ struct Cyc_List_List* _temp1534=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1534->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp1535=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp1535[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp1536; _temp1536.tag= Cyc_Absyn_Attributes_mod; _temp1536.f1= 0; _temp1536.f2=
_temp1435; _temp1536;}); _temp1535;})); _temp1534->tl= tms; _temp1534;});} tms=({
struct Cyc_List_List* _temp1537=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1537->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp1538=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp1538[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp1539; _temp1539.tag=
Cyc_Absyn_Function_mod; _temp1539.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp1540=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp1540[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp1541; _temp1541.tag= Cyc_Absyn_WithTypes;
_temp1541.f1= _temp1443; _temp1541.f2= _temp1441; _temp1541.f3= _temp1439;
_temp1541.f4= _temp1447; _temp1541.f5= _temp1437; _temp1541;}); _temp1540;}));
_temp1539;}); _temp1538;})); _temp1537->tl= tms; _temp1537;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp1435 != 0; _temp1435=(( struct Cyc_List_List*) _check_null(
_temp1435))->tl){ void* _temp1542=( void*)(( struct Cyc_List_List*) _check_null(
_temp1435))->hd; _LL1544: if( _temp1542 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1545;} else{ goto _LL1546;} _LL1546: if( _temp1542 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL1547;} else{ goto _LL1548;} _LL1548: if( _temp1542 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL1549;} else{ goto _LL1550;} _LL1550: goto _LL1551; _LL1545: goto _LL1547;
_LL1547: goto _LL1549; _LL1549: tms=({ struct Cyc_List_List* _temp1552=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1552->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1553=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1553[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1554; _temp1554.tag= Cyc_Absyn_Attributes_mod;
_temp1554.f1= 0; _temp1554.f2=({ struct Cyc_List_List* _temp1555=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1555->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp1435))->hd); _temp1555->tl= 0;
_temp1555;}); _temp1554;}); _temp1553;})); _temp1552->tl= tms; _temp1552;});
goto AfterAtts; _LL1551: goto _LL1543; _LL1543:;}} AfterAtts: if( _temp1449 != 0){
tms=({ struct Cyc_List_List* _temp1556=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1556->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp1557=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp1557[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp1558; _temp1558.tag= Cyc_Absyn_TypeParams_mod; _temp1558.f1= _temp1449;
_temp1558.f2= 0; _temp1558.f3= 1; _temp1558;}); _temp1557;})); _temp1556->tl=
tms; _temp1556;});} return({ struct _tuple4 _temp1559; _temp1559.f1= _temp1532;
_temp1559.f2= _temp1530; _temp1559.f3= tms; _temp1559;});}} _LL1410: if(
_temp1453 == 0){ return({ struct _tuple4 _temp1560; _temp1560.f1= tq; _temp1560.f2=
t; _temp1560.f3= 0; _temp1560;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1453))->v);} _LL1412: if( _temp1457 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp1561; _temp1561.f1=
tq; _temp1561.f2= t; _temp1561.f3= 0; _temp1561;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1457))->v);} _LL1414:
return({ struct _tuple4 _temp1562; _temp1562.f1= tq; _temp1562.f2= t; _temp1562.f3=
0; _temp1562;}); _LL1402:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp1565; void*
_temp1567; struct Cyc_Absyn_Tqual _temp1569; struct _tuple4 _temp1563= Cyc_Absynpp_to_tms(
tq, t); _LL1570: _temp1569= _temp1563.f1; goto _LL1568; _LL1568: _temp1567=
_temp1563.f2; goto _LL1566; _LL1566: _temp1565= _temp1563.f3; goto _LL1564;
_LL1564: _temp1565= Cyc_List_imp_rev( _temp1565); if( _temp1565 == 0? dopt == 0:
0){ return({ struct Cyc_PP_Doc* _temp1571[ 2u]; _temp1571[ 1u]= Cyc_Absynpp_ntyp2doc(
_temp1567); _temp1571[ 0u]= Cyc_Absynpp_tqual2doc( _temp1569); Cyc_PP_cat(
_tag_arr( _temp1571, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ return({
struct Cyc_PP_Doc* _temp1572[ 4u]; _temp1572[ 3u]= Cyc_Absynpp_dtms2doc( dopt ==
0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null(
dopt))->v, _temp1565); _temp1572[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof(
unsigned char), 2u)); _temp1572[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1567);
_temp1572[ 0u]= Cyc_Absynpp_tqual2doc( _temp1569); Cyc_PP_cat( _tag_arr(
_temp1572, sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return({ struct Cyc_PP_Doc*
_temp1573[ 5u]; _temp1573[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1573[ 3u]= Cyc_Absynpp_atts2doc( f->attributes); _temp1573[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( f->width))->v);
_temp1573[ 1u]= Cyc_PP_text( _tag_arr(":", sizeof( unsigned char), 2u));
_temp1573[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt*
_temp1574=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1574->v=( void*) Cyc_PP_textptr( f->name); _temp1574;})); Cyc_PP_cat(
_tag_arr( _temp1573, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ return({
struct Cyc_PP_Doc* _temp1575[ 3u]; _temp1575[ 2u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1575[ 1u]= Cyc_Absynpp_atts2doc( f->attributes);
_temp1575[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt*
_temp1576=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1576->v=( void*) Cyc_PP_textptr( f->name); _temp1576;})); Cyc_PP_cat(
_tag_arr( _temp1575, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc, _tag_arr("",
sizeof( unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc* _temp1577[ 4u];
_temp1577[ 3u]= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs);
_temp1577[ 2u]= f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs);
_temp1577[ 1u]= Cyc_Absynpp_typedef_name2doc( f->name); _temp1577[ 0u]= Cyc_Absynpp_scope2doc((
void*) f->sc); Cyc_PP_cat( _tag_arr( _temp1577, sizeof( struct Cyc_PP_Doc*), 4u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Tunionfield*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=((
struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd), 72, f);({
void* _temp1578[ 0u]={}; Cyc_Stdio_fprintf( f, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1578, sizeof( void*), 0u));});}} struct
_tagged_arr Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc(
Cyc_PP_seql( _tag_arr("", sizeof( unsigned char), 1u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_arr Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct _tagged_arr
Cyc_Absynpp_typ2string( void* t){ return Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72);} struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0* v){ return
Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct _tagged_arr Cyc_Absynpp_prim2string(
void* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_prim2doc( p), 72);} struct
_tagged_arr Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc( p), 72);} struct _tagged_arr Cyc_Absynpp_scope2string( void*
sc){ void* _temp1579= sc; _LL1581: if( _temp1579 ==( void*) Cyc_Absyn_Static){
goto _LL1582;} else{ goto _LL1583;} _LL1583: if( _temp1579 ==( void*) Cyc_Absyn_Public){
goto _LL1584;} else{ goto _LL1585;} _LL1585: if( _temp1579 ==( void*) Cyc_Absyn_Extern){
goto _LL1586;} else{ goto _LL1587;} _LL1587: if( _temp1579 ==( void*) Cyc_Absyn_ExternC){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1579 ==( void*) Cyc_Absyn_Abstract){
goto _LL1590;} else{ goto _LL1580;} _LL1582: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL1584: return _tag_arr("public", sizeof( unsigned char),
7u); _LL1586: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL1588:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL1590: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL1580:;}
