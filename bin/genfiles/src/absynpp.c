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
Cyc_PP_textptr( tv->name); _LL91: return({ struct Cyc_PP_Doc*(* _temp98)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp101= Cyc_PP_textptr( tv->name);
struct Cyc_PP_Doc* _temp102= Cyc_PP_text( _tag_arr("::", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp103= Cyc_Absynpp_kind2doc( _temp96); struct Cyc_PP_Doc*
_temp99[ 3u]={ _temp101, _temp102, _temp103}; struct _tagged_arr _temp100={(
void*) _temp99,( void*) _temp99,( void*)( _temp99 + 3u)}; _temp98( _temp100);});
_LL93: return({ struct Cyc_PP_Doc*(* _temp104)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp107= Cyc_PP_textptr( tv->name); struct Cyc_PP_Doc*
_temp108= Cyc_PP_text( _tag_arr("/*::?*/", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp105[ 2u]={ _temp107, _temp108}; struct _tagged_arr _temp106={(
void*) _temp105,( void*) _temp105,( void*)( _temp105 + 2u)}; _temp104( _temp106);});
_LL87:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc( struct Cyc_List_List* tvs){
return Cyc_PP_egroup( _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_arr* Cyc_Absynpp_get_name(
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
void* att){ void* _temp109= att; _LL111: if( _temp109 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL112;} else{ goto _LL113;} _LL113: if( _temp109 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL114;} else{ goto _LL115;} _LL115: if( _temp109 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL116;} else{ goto _LL117;} _LL117: goto _LL118; _LL112: return Cyc_PP_nil_doc();
_LL114: return Cyc_PP_nil_doc(); _LL116: return Cyc_PP_nil_doc(); _LL118: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL110:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp119=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL121: if( _temp119 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL122;} else{ goto _LL123;} _LL123: if( _temp119 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL124;} else{ goto _LL125;} _LL125: if( _temp119 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL122: return Cyc_PP_text(
_tag_arr(" _stdcall ", sizeof( unsigned char), 11u)); _LL124: return Cyc_PP_text(
_tag_arr(" _cdecl ", sizeof( unsigned char), 9u)); _LL126: return Cyc_PP_text(
_tag_arr(" _fastcall ", sizeof( unsigned char), 12u)); _LL128: goto _LL120;
_LL120:;} return Cyc_PP_nil_doc();} struct Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 != 0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){
void* _temp129=( void*)(( struct Cyc_List_List*) _check_null( atts2))->hd;
_LL131: if( _temp129 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL132;} else{ goto
_LL133;} _LL133: if( _temp129 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL134;}
else{ goto _LL135;} _LL135: if( _temp129 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL136;} else{ goto _LL137;} _LL137: goto _LL138; _LL132: goto _LL130;
_LL134: goto _LL130; _LL136: goto _LL130; _LL138: hasatt= 1; goto _LL130; _LL130:;}}
if( ! hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc*(* _temp139)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp142= Cyc_PP_text(
_tag_arr("__declspec(", sizeof( unsigned char), 12u)); struct Cyc_PP_Doc*
_temp143= Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Absynpp_noncallatt2doc, atts)); struct Cyc_PP_Doc* _temp144=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp140[ 3u]={ _temp142, _temp143, _temp144}; struct _tagged_arr _temp141={(
void*) _temp140,( void*) _temp140,( void*)( _temp140 + 3u)}; _temp139( _temp141);});}
struct Cyc_PP_Doc* Cyc_Absynpp_att2doc( void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string(
a));} struct Cyc_PP_Doc* Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if(
atts == 0){ return Cyc_PP_nil_doc();} if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc(
atts);} return({ struct Cyc_PP_Doc*(* _temp145)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp148= Cyc_PP_text( _tag_arr(" __attribute__", sizeof(
unsigned char), 15u)); struct Cyc_PP_Doc* _temp149= Cyc_PP_group( _tag_arr("((",
sizeof( unsigned char), 3u), _tag_arr("))", sizeof( unsigned char), 3u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc,
atts)); struct Cyc_PP_Doc* _temp146[ 2u]={ _temp148, _temp149}; struct
_tagged_arr _temp147={( void*) _temp146,( void*) _temp146,( void*)( _temp146 + 2u)};
_temp145( _temp147);});} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp150=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; _LL152: if(( unsigned int) _temp150 > 1u?*(( int*)
_temp150) == Cyc_Absyn_Pointer_mod: 0){ goto _LL153;} else{ goto _LL154;} _LL154:
if(( unsigned int) _temp150 > 1u?*(( int*) _temp150) == Cyc_Absyn_Attributes_mod:
0){ goto _LL155;} else{ goto _LL156;} _LL156: goto _LL157; _LL153: return 1;
_LL155: if( ! Cyc_Absynpp_to_VC){ return 0;} return Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl); _LL157: return 0; _LL151:;}}
extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms == 0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d,(( struct Cyc_List_List*)
_check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=({ struct Cyc_PP_Doc*(*
_temp296)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp299= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp300= rest;
struct Cyc_PP_Doc* _temp301= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp297[ 3u]={ _temp299, _temp300, _temp301}; struct
_tagged_arr _temp298={( void*) _temp297,( void*) _temp297,( void*)( _temp297 + 3u)};
_temp296( _temp298);}); void* _temp158=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp172; void* _temp174; struct
Cyc_List_List* _temp176; int _temp178; struct Cyc_Position_Segment* _temp180;
struct Cyc_List_List* _temp182; struct Cyc_Absyn_Tqual _temp184; void* _temp186;
void* _temp188; _LL160: if( _temp158 ==( void*) Cyc_Absyn_Carray_mod){ goto
_LL161;} else{ goto _LL162;} _LL162: if(( unsigned int) _temp158 > 1u?*(( int*)
_temp158) == Cyc_Absyn_ConstArray_mod: 0){ _LL173: _temp172=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp158)->f1; goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int)
_temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_Function_mod: 0){ _LL175:
_temp174=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp158)->f1; goto
_LL165;} else{ goto _LL166;} _LL166: if(( unsigned int) _temp158 > 1u?*(( int*)
_temp158) == Cyc_Absyn_Attributes_mod: 0){ _LL177: _temp176=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp158)->f2; goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int)
_temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_TypeParams_mod: 0){ _LL183:
_temp182=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp158)->f1; goto _LL181;
_LL181: _temp180=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp158)->f2; goto
_LL179; _LL179: _temp178=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp158)->f3;
goto _LL169;} else{ goto _LL170;} _LL170: if(( unsigned int) _temp158 > 1u?*((
int*) _temp158) == Cyc_Absyn_Pointer_mod: 0){ _LL189: _temp188=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp158)->f1; goto _LL187; _LL187: _temp186=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp158)->f2; goto _LL185;
_LL185: _temp184=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp158)->f3; goto
_LL171;} else{ goto _LL159;} _LL161: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(*
_temp190)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp193= rest;
struct Cyc_PP_Doc* _temp194= Cyc_PP_text( _tag_arr("[]", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp191[ 2u]={ _temp193, _temp194}; struct _tagged_arr
_temp192={( void*) _temp191,( void*) _temp191,( void*)( _temp191 + 2u)};
_temp190( _temp192);}); _LL163: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp195)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp198= rest; struct Cyc_PP_Doc*
_temp199= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp200= Cyc_Absynpp_exp2doc( _temp172); struct Cyc_PP_Doc* _temp201= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp196[ 4u]={
_temp198, _temp199, _temp200, _temp201}; struct _tagged_arr _temp197={( void*)
_temp196,( void*) _temp196,( void*)( _temp196 + 4u)}; _temp195( _temp197);});
_LL165: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null(
tms))->tl)){ rest= p_rest;}{ void* _temp202= _temp174; struct Cyc_List_List*
_temp208; struct Cyc_Core_Opt* _temp210; struct Cyc_Absyn_VarargInfo* _temp212;
int _temp214; struct Cyc_List_List* _temp216; struct Cyc_Position_Segment*
_temp218; struct Cyc_List_List* _temp220; _LL204: if(*(( int*) _temp202) == Cyc_Absyn_WithTypes){
_LL217: _temp216=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f1; goto
_LL215; _LL215: _temp214=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f2;
goto _LL213; _LL213: _temp212=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f3;
goto _LL211; _LL211: _temp210=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f4;
goto _LL209; _LL209: _temp208=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f5;
goto _LL205;} else{ goto _LL206;} _LL206: if(*(( int*) _temp202) == Cyc_Absyn_NoTypes){
_LL221: _temp220=(( struct Cyc_Absyn_NoTypes_struct*) _temp202)->f1; goto _LL219;
_LL219: _temp218=(( struct Cyc_Absyn_NoTypes_struct*) _temp202)->f2; goto _LL207;}
else{ goto _LL203;} _LL205: return({ struct Cyc_PP_Doc*(* _temp222)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp225= rest; struct Cyc_PP_Doc*
_temp226= Cyc_Absynpp_funargs2doc( _temp216, _temp214, _temp212, _temp210,
_temp208); struct Cyc_PP_Doc* _temp223[ 2u]={ _temp225, _temp226}; struct
_tagged_arr _temp224={( void*) _temp223,( void*) _temp223,( void*)( _temp223 + 2u)};
_temp222( _temp224);}); _LL207: return({ struct Cyc_PP_Doc*(* _temp227)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp230= rest; struct Cyc_PP_Doc*
_temp231= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_PP_textptr, _temp220)); struct Cyc_PP_Doc* _temp228[ 2u]={
_temp230, _temp231}; struct _tagged_arr _temp229={( void*) _temp228,( void*)
_temp228,( void*)( _temp228 + 2u)}; _temp227( _temp229);}); _LL203:;} _LL167:
if( ! Cyc_Absynpp_to_VC){ if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp232)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp235= rest; struct Cyc_PP_Doc*
_temp236= Cyc_Absynpp_atts2doc( _temp176); struct Cyc_PP_Doc* _temp233[ 2u]={
_temp235, _temp236}; struct _tagged_arr _temp234={( void*) _temp233,( void*)
_temp233,( void*)( _temp233 + 2u)}; _temp232( _temp234);});} else{ if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ return({ struct Cyc_PP_Doc*(*
_temp237)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp240= Cyc_Absynpp_callconv2doc(
_temp176); struct Cyc_PP_Doc* _temp241= rest; struct Cyc_PP_Doc* _temp238[ 2u]={
_temp240, _temp241}; struct _tagged_arr _temp239={( void*) _temp238,( void*)
_temp238,( void*)( _temp238 + 2u)}; _temp237( _temp239);});} return rest;}
_LL169: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null(
tms))->tl)){ rest= p_rest;} if( _temp178){ return({ struct Cyc_PP_Doc*(*
_temp242)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp245= rest;
struct Cyc_PP_Doc* _temp246= Cyc_Absynpp_ktvars2doc( _temp182); struct Cyc_PP_Doc*
_temp243[ 2u]={ _temp245, _temp246}; struct _tagged_arr _temp244={( void*)
_temp243,( void*) _temp243,( void*)( _temp243 + 2u)}; _temp242( _temp244);});}
else{ return({ struct Cyc_PP_Doc*(* _temp247)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp250= rest; struct Cyc_PP_Doc* _temp251= Cyc_Absynpp_tvars2doc(
_temp182); struct Cyc_PP_Doc* _temp248[ 2u]={ _temp250, _temp251}; struct
_tagged_arr _temp249={( void*) _temp248,( void*) _temp248,( void*)( _temp248 + 2u)};
_temp247( _temp249);});} _LL171: { struct Cyc_PP_Doc* ptr;{ void* _temp252=
_temp188; struct Cyc_Absyn_Exp* _temp260; struct Cyc_Absyn_Exp* _temp262; _LL254:
if(( unsigned int) _temp252 > 1u?*(( int*) _temp252) == Cyc_Absyn_Nullable_ps: 0){
_LL261: _temp260=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp252)->f1; goto
_LL255;} else{ goto _LL256;} _LL256: if(( unsigned int) _temp252 > 1u?*(( int*)
_temp252) == Cyc_Absyn_NonNullable_ps: 0){ _LL263: _temp262=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp252)->f1; goto _LL257;} else{ goto _LL258;} _LL258: if( _temp252 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL259;} else{ goto _LL253;} _LL255: if( Cyc_Evexp_eval_const_uint_exp(
_temp260) == 1){ ptr= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc*(* _temp264)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp267= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp268= Cyc_PP_text( _tag_arr("{", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp269= Cyc_Absynpp_exp2doc( _temp260);
struct Cyc_PP_Doc* _temp270= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp265[ 4u]={ _temp267, _temp268, _temp269, _temp270};
struct _tagged_arr _temp266={( void*) _temp265,( void*) _temp265,( void*)(
_temp265 + 4u)}; _temp264( _temp266);});} goto _LL253; _LL257: if( Cyc_Evexp_eval_const_uint_exp(
_temp262) == 1){ ptr= Cyc_PP_text( _tag_arr("@", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc*(* _temp271)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp274= Cyc_PP_text( _tag_arr("@", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp275= Cyc_PP_text( _tag_arr("{", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp276= Cyc_Absynpp_exp2doc( _temp262);
struct Cyc_PP_Doc* _temp277= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp272[ 4u]={ _temp274, _temp275, _temp276, _temp277};
struct _tagged_arr _temp273={( void*) _temp272,( void*) _temp272,( void*)(
_temp272 + 4u)}; _temp271( _temp273);});} goto _LL253; _LL259: ptr= Cyc_PP_text(
_tag_arr("?", sizeof( unsigned char), 2u)); goto _LL253; _LL253:;}{ void*
_temp278= Cyc_Tcutil_compress( _temp186); _LL280: if( _temp278 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL281;} else{ goto _LL282;} _LL282: goto _LL283; _LL281: return({ struct
Cyc_PP_Doc*(* _temp284)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp287= ptr; struct Cyc_PP_Doc* _temp288= rest; struct Cyc_PP_Doc* _temp285[ 2u]={
_temp287, _temp288}; struct _tagged_arr _temp286={( void*) _temp285,( void*)
_temp285,( void*)( _temp285 + 2u)}; _temp284( _temp286);}); _LL283: return({
struct Cyc_PP_Doc*(* _temp289)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp292= ptr; struct Cyc_PP_Doc* _temp293= Cyc_Absynpp_ntyp2doc( _temp186);
struct Cyc_PP_Doc* _temp294= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp295= rest; struct Cyc_PP_Doc* _temp290[ 4u]={
_temp292, _temp293, _temp294, _temp295}; struct _tagged_arr _temp291={( void*)
_temp290,( void*) _temp290,( void*)( _temp290 + 4u)}; _temp289( _temp291);});
_LL279:;}} _LL159:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void*
_temp302= Cyc_Tcutil_compress( t); _LL304: if( _temp302 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL305;} else{ goto _LL306;} _LL306: goto _LL307; _LL305: return Cyc_PP_text(
_tag_arr("`H", sizeof( unsigned char), 3u)); _LL307: return Cyc_Absynpp_ntyp2doc(
t); _LL303:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp308= Cyc_Tcutil_compress(
t); void* _temp316; struct Cyc_List_List* _temp318; _LL310: if(( unsigned int)
_temp308 > 4u?*(( int*) _temp308) == Cyc_Absyn_AccessEff: 0){ _LL317: _temp316=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp308)->f1; goto _LL311;} else{
goto _LL312;} _LL312: if(( unsigned int) _temp308 > 4u?*(( int*) _temp308) ==
Cyc_Absyn_JoinEff: 0){ _LL319: _temp318=(( struct Cyc_Absyn_JoinEff_struct*)
_temp308)->f1; goto _LL313;} else{ goto _LL314;} _LL314: goto _LL315; _LL311:*
regions=({ struct Cyc_List_List* _temp320=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp320->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp316); _temp320->tl=* regions; _temp320;}); goto _LL309; _LL313: for( 0;
_temp318 != 0; _temp318=(( struct Cyc_List_List*) _check_null( _temp318))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp318))->hd);} goto _LL309; _LL315:* effects=({ struct Cyc_List_List*
_temp321=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp321->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp321->tl=* effects; _temp321;});
goto _LL309; _LL309:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("+", sizeof( unsigned char), 2u), effects);} else{ struct Cyc_PP_Doc*
_temp322= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u), regions);
return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("+", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
effects,({ struct Cyc_List_List* _temp323=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp323->hd=( void*) _temp322; _temp323->tl= 0;
_temp323;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp324= t; struct Cyc_Core_Opt* _temp370; int _temp372; struct Cyc_Core_Opt*
_temp374; struct Cyc_Core_Opt* _temp376; struct Cyc_Absyn_Tvar* _temp378; struct
Cyc_Absyn_TunionInfo _temp380; void* _temp382; struct Cyc_List_List* _temp384;
void* _temp386; struct Cyc_Absyn_TunionFieldInfo _temp388; struct Cyc_List_List*
_temp390; void* _temp392; void* _temp394; void* _temp396; struct Cyc_List_List*
_temp398; struct Cyc_List_List* _temp400; struct _tuple0* _temp402; struct Cyc_List_List*
_temp404; struct _tuple0* _temp406; struct Cyc_List_List* _temp408; struct Cyc_List_List*
_temp410; struct _tuple0* _temp412; struct Cyc_Core_Opt* _temp414; struct Cyc_List_List*
_temp416; struct _tuple0* _temp418; void* _temp420; _LL326: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_ArrayType: 0){ goto _LL327;}
else{ goto _LL328;} _LL328: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324)
== Cyc_Absyn_FnType: 0){ goto _LL329;} else{ goto _LL330;} _LL330: if((
unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_PointerType: 0){
goto _LL331;} else{ goto _LL332;} _LL332: if( _temp324 ==( void*) Cyc_Absyn_VoidType){
goto _LL333;} else{ goto _LL334;} _LL334: if(( unsigned int) _temp324 > 4u?*((
int*) _temp324) == Cyc_Absyn_Evar: 0){ _LL377: _temp376=(( struct Cyc_Absyn_Evar_struct*)
_temp324)->f1; goto _LL375; _LL375: _temp374=(( struct Cyc_Absyn_Evar_struct*)
_temp324)->f2; goto _LL373; _LL373: _temp372=(( struct Cyc_Absyn_Evar_struct*)
_temp324)->f3; goto _LL371; _LL371: _temp370=(( struct Cyc_Absyn_Evar_struct*)
_temp324)->f4; goto _LL335;} else{ goto _LL336;} _LL336: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_VarType: 0){ _LL379: _temp378=((
struct Cyc_Absyn_VarType_struct*) _temp324)->f1; goto _LL337;} else{ goto _LL338;}
_LL338: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_TunionType:
0){ _LL381: _temp380=(( struct Cyc_Absyn_TunionType_struct*) _temp324)->f1;
_LL387: _temp386=( void*) _temp380.tunion_info; goto _LL385; _LL385: _temp384=
_temp380.targs; goto _LL383; _LL383: _temp382=( void*) _temp380.rgn; goto _LL339;}
else{ goto _LL340;} _LL340: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324)
== Cyc_Absyn_TunionFieldType: 0){ _LL389: _temp388=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp324)->f1; _LL393: _temp392=( void*) _temp388.field_info; goto _LL391;
_LL391: _temp390= _temp388.targs; goto _LL341;} else{ goto _LL342;} _LL342: if((
unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_IntType: 0){ _LL397:
_temp396=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp324)->f1; goto _LL395;
_LL395: _temp394=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp324)->f2;
goto _LL343;} else{ goto _LL344;} _LL344: if( _temp324 ==( void*) Cyc_Absyn_FloatType){
goto _LL345;} else{ goto _LL346;} _LL346: if( _temp324 ==( void*) Cyc_Absyn_DoubleType){
goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int) _temp324 > 4u?*((
int*) _temp324) == Cyc_Absyn_TupleType: 0){ _LL399: _temp398=(( struct Cyc_Absyn_TupleType_struct*)
_temp324)->f1; goto _LL349;} else{ goto _LL350;} _LL350: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_StructType: 0){ _LL403: _temp402=((
struct Cyc_Absyn_StructType_struct*) _temp324)->f1; goto _LL401; _LL401:
_temp400=(( struct Cyc_Absyn_StructType_struct*) _temp324)->f2; goto _LL351;}
else{ goto _LL352;} _LL352: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324)
== Cyc_Absyn_UnionType: 0){ _LL407: _temp406=(( struct Cyc_Absyn_UnionType_struct*)
_temp324)->f1; goto _LL405; _LL405: _temp404=(( struct Cyc_Absyn_UnionType_struct*)
_temp324)->f2; goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_AnonStructType: 0){ _LL409:
_temp408=(( struct Cyc_Absyn_AnonStructType_struct*) _temp324)->f1; goto _LL355;}
else{ goto _LL356;} _LL356: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324)
== Cyc_Absyn_AnonUnionType: 0){ _LL411: _temp410=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp324)->f1; goto _LL357;} else{ goto _LL358;} _LL358: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_EnumType: 0){ _LL413: _temp412=((
struct Cyc_Absyn_EnumType_struct*) _temp324)->f1; goto _LL359;} else{ goto
_LL360;} _LL360: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_TypedefType:
0){ _LL419: _temp418=(( struct Cyc_Absyn_TypedefType_struct*) _temp324)->f1;
goto _LL417; _LL417: _temp416=(( struct Cyc_Absyn_TypedefType_struct*) _temp324)->f2;
goto _LL415; _LL415: _temp414=(( struct Cyc_Absyn_TypedefType_struct*) _temp324)->f3;
goto _LL361;} else{ goto _LL362;} _LL362: if(( unsigned int) _temp324 > 4u?*((
int*) _temp324) == Cyc_Absyn_RgnHandleType: 0){ _LL421: _temp420=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp324)->f1; goto _LL363;} else{ goto
_LL364;} _LL364: if( _temp324 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL365;} else{
goto _LL366;} _LL366: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324) ==
Cyc_Absyn_AccessEff: 0){ goto _LL367;} else{ goto _LL368;} _LL368: if((
unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_JoinEff: 0){ goto
_LL369;} else{ goto _LL325;} _LL327: return Cyc_PP_text( _tag_arr("[[[array]]]",
sizeof( unsigned char), 12u)); _LL329: return Cyc_PP_nil_doc(); _LL331: return
Cyc_PP_nil_doc(); _LL333: s= Cyc_PP_text( _tag_arr("void", sizeof( unsigned char),
5u)); goto _LL325; _LL335: if( _temp374 != 0){ return Cyc_Absynpp_ntyp2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp374))->v);} else{ s=({ struct
Cyc_PP_Doc*(* _temp422)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp425= Cyc_PP_text( _tag_arr("%", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp426= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp427= Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr(* _temp431)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp435= _tag_arr("%d", sizeof( unsigned char), 3u); struct Cyc_Stdio_Int_pa_struct
_temp436; _temp436.tag= Cyc_Stdio_Int_pa; _temp436.f1=( int)(( unsigned int)
_temp372);{ void* _temp434=( void*)& _temp436; void* _temp432[ 1u]={ _temp434};
struct _tagged_arr _temp433={( void*) _temp432,( void*) _temp432,( void*)(
_temp432 + 1u)}; _temp431( _temp435, _temp433);}})); struct Cyc_PP_Doc* _temp428=(
! Cyc_Absynpp_print_full_evars? 1: _temp370 == 0)? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp370))->v); struct Cyc_PP_Doc* _temp429=
Cyc_PP_text( _tag_arr(")::", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp430= _temp376 == 0? Cyc_PP_text( _tag_arr("?", sizeof( unsigned char), 2u)):
Cyc_Absynpp_kind2doc(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp376))->v);
struct Cyc_PP_Doc* _temp423[ 6u]={ _temp425, _temp426, _temp427, _temp428,
_temp429, _temp430}; struct _tagged_arr _temp424={( void*) _temp423,( void*)
_temp423,( void*)( _temp423 + 6u)}; _temp422( _temp424);});} goto _LL325; _LL337:
s= Cyc_PP_textptr( _temp378->name); if( Cyc_Absynpp_print_all_kinds){ s=({
struct Cyc_PP_Doc*(* _temp437)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp440= s; struct Cyc_PP_Doc* _temp441= Cyc_PP_text( _tag_arr("::", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp442= Cyc_Absynpp_ckind2doc(
_temp378->kind); struct Cyc_PP_Doc* _temp438[ 3u]={ _temp440, _temp441, _temp442};
struct _tagged_arr _temp439={( void*) _temp438,( void*) _temp438,( void*)(
_temp438 + 3u)}; _temp437( _temp439);});} if( Cyc_Absynpp_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar( _temp378): 0){ s=({ struct Cyc_PP_Doc*(* _temp443)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp446= Cyc_PP_text(
_tag_arr("_ /* ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp447= s;
struct Cyc_PP_Doc* _temp448= Cyc_PP_text( _tag_arr(" */", sizeof( unsigned char),
4u)); struct Cyc_PP_Doc* _temp444[ 3u]={ _temp446, _temp447, _temp448}; struct
_tagged_arr _temp445={( void*) _temp444,( void*) _temp444,( void*)( _temp444 + 3u)};
_temp443( _temp445);});} goto _LL325; _LL339:{ void* _temp449= _temp386; struct
Cyc_Absyn_UnknownTunionInfo _temp455; int _temp457; struct _tuple0* _temp459;
struct Cyc_Absyn_Tuniondecl* _temp461; struct Cyc_Absyn_Tuniondecl _temp463; int
_temp464; struct _tuple0* _temp466; _LL451: if(*(( int*) _temp449) == Cyc_Absyn_UnknownTunion){
_LL456: _temp455=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp449)->f1;
_LL460: _temp459= _temp455.name; goto _LL458; _LL458: _temp457= _temp455.is_xtunion;
goto _LL452;} else{ goto _LL453;} _LL453: if(*(( int*) _temp449) == Cyc_Absyn_KnownTunion){
_LL462: _temp461=(( struct Cyc_Absyn_KnownTunion_struct*) _temp449)->f1;
_temp463=* _temp461; _LL467: _temp466= _temp463.name; goto _LL465; _LL465:
_temp464= _temp463.is_xtunion; goto _LL454;} else{ goto _LL450;} _LL452:
_temp466= _temp459; _temp464= _temp457; goto _LL454; _LL454: { struct Cyc_PP_Doc*
_temp468= Cyc_PP_text( _temp464? _tag_arr("xtunion ", sizeof( unsigned char), 9u):
_tag_arr("tunion ", sizeof( unsigned char), 8u));{ void* _temp469= Cyc_Tcutil_compress(
_temp382); _LL471: if( _temp469 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL472;}
else{ goto _LL473;} _LL473: goto _LL474; _LL472: s=({ struct Cyc_PP_Doc*(*
_temp475)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp478=
_temp468; struct Cyc_PP_Doc* _temp479= Cyc_Absynpp_qvar2doc( _temp466); struct
Cyc_PP_Doc* _temp480= Cyc_Absynpp_tps2doc( _temp384); struct Cyc_PP_Doc*
_temp476[ 3u]={ _temp478, _temp479, _temp480}; struct _tagged_arr _temp477={(
void*) _temp476,( void*) _temp476,( void*)( _temp476 + 3u)}; _temp475( _temp477);});
goto _LL470; _LL474: s=({ struct Cyc_PP_Doc*(* _temp481)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp484= _temp468; struct Cyc_PP_Doc* _temp485=
Cyc_Absynpp_typ2doc( _temp382); struct Cyc_PP_Doc* _temp486= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp487= Cyc_Absynpp_qvar2doc(
_temp466); struct Cyc_PP_Doc* _temp488= Cyc_Absynpp_tps2doc( _temp384); struct
Cyc_PP_Doc* _temp482[ 5u]={ _temp484, _temp485, _temp486, _temp487, _temp488};
struct _tagged_arr _temp483={( void*) _temp482,( void*) _temp482,( void*)(
_temp482 + 5u)}; _temp481( _temp483);}); goto _LL470; _LL470:;} goto _LL450;}
_LL450:;} goto _LL325; _LL341:{ void* _temp489= _temp392; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp495; int _temp497; struct _tuple0* _temp499; struct _tuple0* _temp501;
struct Cyc_Absyn_Tunionfield* _temp503; struct Cyc_Absyn_Tunionfield _temp505;
struct _tuple0* _temp506; struct Cyc_Absyn_Tuniondecl* _temp508; struct Cyc_Absyn_Tuniondecl
_temp510; int _temp511; struct _tuple0* _temp513; _LL491: if(*(( int*) _temp489)
== Cyc_Absyn_UnknownTunionfield){ _LL496: _temp495=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp489)->f1; _LL502: _temp501= _temp495.tunion_name; goto _LL500; _LL500:
_temp499= _temp495.field_name; goto _LL498; _LL498: _temp497= _temp495.is_xtunion;
goto _LL492;} else{ goto _LL493;} _LL493: if(*(( int*) _temp489) == Cyc_Absyn_KnownTunionfield){
_LL509: _temp508=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp489)->f1;
_temp510=* _temp508; _LL514: _temp513= _temp510.name; goto _LL512; _LL512:
_temp511= _temp510.is_xtunion; goto _LL504; _LL504: _temp503=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp489)->f2; _temp505=* _temp503; _LL507: _temp506= _temp505.name; goto _LL494;}
else{ goto _LL490;} _LL492: _temp513= _temp501; _temp511= _temp497; _temp506=
_temp499; goto _LL494; _LL494: { struct Cyc_PP_Doc* _temp515= Cyc_PP_text(
_temp511? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc*(* _temp516)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp519= _temp515; struct Cyc_PP_Doc*
_temp520= Cyc_Absynpp_qvar2doc( _temp513); struct Cyc_PP_Doc* _temp521= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp522= Cyc_Absynpp_qvar2doc(
_temp506); struct Cyc_PP_Doc* _temp517[ 4u]={ _temp519, _temp520, _temp521,
_temp522}; struct _tagged_arr _temp518={( void*) _temp517,( void*) _temp517,(
void*)( _temp517 + 4u)}; _temp516( _temp518);}); goto _LL490;} _LL490:;} goto
_LL325; _LL343: { struct _tagged_arr sns; struct _tagged_arr ts;{ void* _temp523=
_temp396; _LL525: if( _temp523 ==( void*) Cyc_Absyn_Signed){ goto _LL526;} else{
goto _LL527;} _LL527: if( _temp523 ==( void*) Cyc_Absyn_Unsigned){ goto _LL528;}
else{ goto _LL524;} _LL526: sns= _tag_arr("", sizeof( unsigned char), 1u); goto
_LL524; _LL528: sns= _tag_arr("unsigned ", sizeof( unsigned char), 10u); goto
_LL524; _LL524:;}{ void* _temp529= _temp394; _LL531: if( _temp529 ==( void*) Cyc_Absyn_B1){
goto _LL532;} else{ goto _LL533;} _LL533: if( _temp529 ==( void*) Cyc_Absyn_B2){
goto _LL534;} else{ goto _LL535;} _LL535: if( _temp529 ==( void*) Cyc_Absyn_B4){
goto _LL536;} else{ goto _LL537;} _LL537: if( _temp529 ==( void*) Cyc_Absyn_B8){
goto _LL538;} else{ goto _LL530;} _LL532:{ void* _temp539= _temp396; _LL541: if(
_temp539 ==( void*) Cyc_Absyn_Signed){ goto _LL542;} else{ goto _LL543;} _LL543:
if( _temp539 ==( void*) Cyc_Absyn_Unsigned){ goto _LL544;} else{ goto _LL540;}
_LL542: sns= _tag_arr("signed ", sizeof( unsigned char), 8u); goto _LL540;
_LL544: goto _LL540; _LL540:;} ts= _tag_arr("char", sizeof( unsigned char), 5u);
goto _LL530; _LL534: ts= _tag_arr("short", sizeof( unsigned char), 6u); goto
_LL530; _LL536: ts= _tag_arr("int", sizeof( unsigned char), 4u); goto _LL530;
_LL538: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64", sizeof( unsigned char), 8u):
_tag_arr("long long", sizeof( unsigned char), 10u); goto _LL530; _LL530:;} s=
Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr(* _temp545)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp550= _tag_arr("%s%s", sizeof( unsigned char), 5u); struct Cyc_Stdio_String_pa_struct
_temp552; _temp552.tag= Cyc_Stdio_String_pa; _temp552.f1=( struct _tagged_arr)
sns;{ void* _temp548=( void*)& _temp552; struct Cyc_Stdio_String_pa_struct
_temp551; _temp551.tag= Cyc_Stdio_String_pa; _temp551.f1=( struct _tagged_arr)
ts;{ void* _temp549=( void*)& _temp551; void* _temp546[ 2u]={ _temp548, _temp549};
struct _tagged_arr _temp547={( void*) _temp546,( void*) _temp546,( void*)(
_temp546 + 2u)}; _temp545( _temp550, _temp547);}}})); goto _LL325;} _LL345: s=
Cyc_PP_text( _tag_arr("float", sizeof( unsigned char), 6u)); goto _LL325; _LL347:
s= Cyc_PP_text( _tag_arr("double", sizeof( unsigned char), 7u)); goto _LL325;
_LL349: s=({ struct Cyc_PP_Doc*(* _temp553)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp556= Cyc_PP_text( _tag_arr("$", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp557= Cyc_Absynpp_args2doc( _temp398); struct Cyc_PP_Doc*
_temp554[ 2u]={ _temp556, _temp557}; struct _tagged_arr _temp555={( void*)
_temp554,( void*) _temp554,( void*)( _temp554 + 2u)}; _temp553( _temp555);});
goto _LL325; _LL351: if( _temp402 == 0){ s=({ struct Cyc_PP_Doc*(* _temp558)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp561= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp562=
Cyc_Absynpp_tps2doc( _temp400); struct Cyc_PP_Doc* _temp559[ 2u]={ _temp561,
_temp562}; struct _tagged_arr _temp560={( void*) _temp559,( void*) _temp559,(
void*)( _temp559 + 2u)}; _temp558( _temp560);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp563)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp566= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp567=
_temp402 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)
_check_null( _temp402)); struct Cyc_PP_Doc* _temp568= Cyc_Absynpp_tps2doc(
_temp400); struct Cyc_PP_Doc* _temp564[ 3u]={ _temp566, _temp567, _temp568};
struct _tagged_arr _temp565={( void*) _temp564,( void*) _temp564,( void*)(
_temp564 + 3u)}; _temp563( _temp565);});} goto _LL325; _LL353: if( _temp406 == 0){
s=({ struct Cyc_PP_Doc*(* _temp569)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp572= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp573= Cyc_Absynpp_tps2doc( _temp404); struct Cyc_PP_Doc*
_temp570[ 2u]={ _temp572, _temp573}; struct _tagged_arr _temp571={( void*)
_temp570,( void*) _temp570,( void*)( _temp570 + 2u)}; _temp569( _temp571);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp574)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp577= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); struct Cyc_PP_Doc* _temp578= _temp406 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp406)); struct Cyc_PP_Doc*
_temp579= Cyc_Absynpp_tps2doc( _temp404); struct Cyc_PP_Doc* _temp575[ 3u]={
_temp577, _temp578, _temp579}; struct _tagged_arr _temp576={( void*) _temp575,(
void*) _temp575,( void*)( _temp575 + 3u)}; _temp574( _temp576);});} goto _LL325;
_LL355: s=({ struct Cyc_PP_Doc*(* _temp580)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp583= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); struct Cyc_PP_Doc* _temp584= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp408)); struct Cyc_PP_Doc* _temp585= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp581[ 3u]={ _temp583, _temp584,
_temp585}; struct _tagged_arr _temp582={( void*) _temp581,( void*) _temp581,(
void*)( _temp581 + 3u)}; _temp580( _temp582);}); goto _LL325; _LL357: s=({
struct Cyc_PP_Doc*(* _temp586)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp589= Cyc_PP_text( _tag_arr("union {", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp590= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc( _temp410));
struct Cyc_PP_Doc* _temp591= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp587[ 3u]={ _temp589, _temp590, _temp591}; struct
_tagged_arr _temp588={( void*) _temp587,( void*) _temp587,( void*)( _temp587 + 3u)};
_temp586( _temp588);}); goto _LL325; _LL359: s=({ struct Cyc_PP_Doc*(* _temp592)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp595= Cyc_PP_text(
_tag_arr("enum ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp596= Cyc_Absynpp_qvar2doc(
_temp412); struct Cyc_PP_Doc* _temp593[ 2u]={ _temp595, _temp596}; struct
_tagged_arr _temp594={( void*) _temp593,( void*) _temp593,( void*)( _temp593 + 2u)};
_temp592( _temp594);}); goto _LL325; _LL361: s=({ struct Cyc_PP_Doc*(* _temp597)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp600= Cyc_Absynpp_qvar2doc(
_temp418); struct Cyc_PP_Doc* _temp601= Cyc_Absynpp_tps2doc( _temp416); struct
Cyc_PP_Doc* _temp598[ 2u]={ _temp600, _temp601}; struct _tagged_arr _temp599={(
void*) _temp598,( void*) _temp598,( void*)( _temp598 + 2u)}; _temp597( _temp599);});
goto _LL325; _LL363: s=({ struct Cyc_PP_Doc*(* _temp602)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp605= Cyc_PP_text( _tag_arr("region_t<",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp606= Cyc_Absynpp_rgn2doc(
_temp420); struct Cyc_PP_Doc* _temp607= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp603[ 3u]={ _temp605, _temp606,
_temp607}; struct _tagged_arr _temp604={( void*) _temp603,( void*) _temp603,(
void*)( _temp603 + 3u)}; _temp602( _temp604);}); goto _LL325; _LL365: s= Cyc_Absynpp_rgn2doc(
t); goto _LL325; _LL367: s= Cyc_Absynpp_eff2doc( t); goto _LL325; _LL369: s= Cyc_Absynpp_eff2doc(
t); goto _LL325; _LL325:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct
_tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc(
struct _tuple8* cmp){ struct _tuple8 _temp610; void* _temp611; void* _temp613;
struct _tuple8* _temp608= cmp; _temp610=* _temp608; _LL614: _temp613= _temp610.f1;
goto _LL612; _LL612: _temp611= _temp610.f2; goto _LL609; _LL609: return({ struct
Cyc_PP_Doc*(* _temp615)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp618= Cyc_Absynpp_rgn2doc( _temp613); struct Cyc_PP_Doc* _temp619= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp620= Cyc_Absynpp_rgn2doc(
_temp611); struct Cyc_PP_Doc* _temp616[ 3u]={ _temp618, _temp619, _temp620};
struct _tagged_arr _temp617={( void*) _temp616,( void*) _temp616,( void*)(
_temp616 + 3u)}; _temp615( _temp617);});} struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List* po){ return Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp621=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp621->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp621;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp622=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp622=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp622,({
struct Cyc_List_List* _temp623=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp623->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp623->tl= 0; _temp623;}));} else{ if(
cyc_varargs != 0){ struct Cyc_PP_Doc* _temp624=({ struct Cyc_PP_Doc*(* _temp626)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp629= Cyc_PP_text(
_tag_arr("...", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp630= Cyc_Absynpp_typ2doc((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn); struct
Cyc_PP_Doc* _temp631=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->inject?
Cyc_PP_text( _tag_arr(" inject ", sizeof( unsigned char), 9u)): Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp632= Cyc_Absynpp_funarg2doc(({
struct _tuple1* _temp633=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp633->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp633->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp633->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp633;})); struct Cyc_PP_Doc* _temp627[ 4u]={ _temp629, _temp630, _temp631,
_temp632}; struct _tagged_arr _temp628={( void*) _temp627,( void*) _temp627,(
void*)( _temp627 + 4u)}; _temp626( _temp628);}); _temp622=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp622,({
struct Cyc_List_List* _temp625=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp625->hd=( void*) _temp624; _temp625->tl= 0;
_temp625;}));}}{ struct Cyc_PP_Doc* _temp634= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp622); if( effopt != 0){ _temp634=({ struct Cyc_PP_Doc*(*
_temp635)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp638=
_temp634; struct Cyc_PP_Doc* _temp639= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp640= Cyc_Absynpp_eff2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v); struct Cyc_PP_Doc* _temp636[ 3u]={
_temp638, _temp639, _temp640}; struct _tagged_arr _temp637={( void*) _temp636,(
void*) _temp636,( void*)( _temp636 + 3u)}; _temp635( _temp637);});} if( rgn_po
!= 0){ _temp634=({ struct Cyc_PP_Doc*(* _temp641)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp644= _temp634; struct Cyc_PP_Doc* _temp645= Cyc_PP_text(
_tag_arr(":", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp646= Cyc_Absynpp_rgnpo2doc(
rgn_po); struct Cyc_PP_Doc* _temp642[ 3u]={ _temp644, _temp645, _temp646};
struct _tagged_arr _temp643={( void*) _temp642,( void*) _temp642,( void*)(
_temp642 + 3u)}; _temp641( _temp643);});} return({ struct Cyc_PP_Doc*(* _temp647)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp650= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp651=
_temp634; struct Cyc_PP_Doc* _temp652= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp648[ 3u]={ _temp650, _temp651,
_temp652}; struct _tagged_arr _temp649={( void*) _temp648,( void*) _temp648,(
void*)( _temp648 + 3u)}; _temp647( _temp649);});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp653=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp653->f1=({ struct Cyc_Core_Opt*
_temp654=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp654->v=( void*)(* arg).f1; _temp654;}); _temp653->f2=(* arg).f2; _temp653->f3=(*
arg).f3; _temp653;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_arr* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp655= 0; int match;{ void*
_temp656=(* q).f1; struct Cyc_List_List* _temp664; struct Cyc_List_List*
_temp666; _LL658: if( _temp656 ==( void*) Cyc_Absyn_Loc_n){ goto _LL659;} else{
goto _LL660;} _LL660: if(( unsigned int) _temp656 > 1u?*(( int*) _temp656) ==
Cyc_Absyn_Rel_n: 0){ _LL665: _temp664=(( struct Cyc_Absyn_Rel_n_struct*)
_temp656)->f1; goto _LL661;} else{ goto _LL662;} _LL662: if(( unsigned int)
_temp656 > 1u?*(( int*) _temp656) == Cyc_Absyn_Abs_n: 0){ _LL667: _temp666=((
struct Cyc_Absyn_Abs_n_struct*) _temp656)->f1; goto _LL663;} else{ goto _LL657;}
_LL659: _temp664= 0; goto _LL661; _LL661: match= 0; _temp655= _temp664; goto
_LL657; _LL663: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp666, Cyc_Absynpp_curr_namespace):
0; _temp655=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp668=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp668->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp668->tl= _temp666;
_temp668;}): _temp666; goto _LL657; _LL657:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp655,({
struct Cyc_List_List* _temp669=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp669->hd=( void*)(* q).f2; _temp669->tl= 0; _temp669;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp655,({ struct Cyc_List_List* _temp670=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp670->hd=( void*)(* q).f2; _temp670->tl= 0;
_temp670;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp671=(* v).f1; struct Cyc_List_List* _temp681; struct Cyc_List_List*
_temp683; _LL673: if( _temp671 ==( void*) Cyc_Absyn_Loc_n){ goto _LL674;} else{
goto _LL675;} _LL675: if(( unsigned int) _temp671 > 1u?*(( int*) _temp671) ==
Cyc_Absyn_Rel_n: 0){ _LL682: _temp681=(( struct Cyc_Absyn_Rel_n_struct*)
_temp671)->f1; if( _temp681 == 0){ goto _LL676;} else{ goto _LL677;}} else{ goto
_LL677;} _LL677: if(( unsigned int) _temp671 > 1u?*(( int*) _temp671) == Cyc_Absyn_Abs_n:
0){ _LL684: _temp683=(( struct Cyc_Absyn_Abs_n_struct*) _temp671)->f1; goto
_LL678;} else{ goto _LL679;} _LL679: goto _LL680; _LL674: goto _LL676; _LL676:
return Cyc_Absynpp_var2doc((* v).f2); _LL678: if((( int(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp683, Cyc_Absynpp_curr_namespace)
== 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL680;} _LL680: return({
struct Cyc_PP_Doc*(* _temp685)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp688= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof( unsigned char),
23u)); struct Cyc_PP_Doc* _temp689= Cyc_Absynpp_qvar2doc( v); struct Cyc_PP_Doc*
_temp686[ 2u]={ _temp688, _temp689}; struct _tagged_arr _temp687={( void*)
_temp686,( void*) _temp686,( void*)( _temp686 + 2u)}; _temp685( _temp687);});
_LL672:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc(
void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp* e){ void* _temp690=( void*) e->r; void* _temp764; struct
Cyc_Absyn_Exp* _temp766; struct Cyc_Absyn_Exp* _temp768; _LL692: if(*(( int*)
_temp690) == Cyc_Absyn_Const_e){ goto _LL693;} else{ goto _LL694;} _LL694: if(*((
int*) _temp690) == Cyc_Absyn_Var_e){ goto _LL695;} else{ goto _LL696;} _LL696:
if(*(( int*) _temp690) == Cyc_Absyn_UnknownId_e){ goto _LL697;} else{ goto
_LL698;} _LL698: if(*(( int*) _temp690) == Cyc_Absyn_Primop_e){ _LL765: _temp764=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp690)->f1; goto _LL699;} else{
goto _LL700;} _LL700: if(*(( int*) _temp690) == Cyc_Absyn_AssignOp_e){ goto
_LL701;} else{ goto _LL702;} _LL702: if(*(( int*) _temp690) == Cyc_Absyn_Increment_e){
goto _LL703;} else{ goto _LL704;} _LL704: if(*(( int*) _temp690) == Cyc_Absyn_Conditional_e){
goto _LL705;} else{ goto _LL706;} _LL706: if(*(( int*) _temp690) == Cyc_Absyn_SeqExp_e){
goto _LL707;} else{ goto _LL708;} _LL708: if(*(( int*) _temp690) == Cyc_Absyn_UnknownCall_e){
goto _LL709;} else{ goto _LL710;} _LL710: if(*(( int*) _temp690) == Cyc_Absyn_FnCall_e){
goto _LL711;} else{ goto _LL712;} _LL712: if(*(( int*) _temp690) == Cyc_Absyn_Throw_e){
goto _LL713;} else{ goto _LL714;} _LL714: if(*(( int*) _temp690) == Cyc_Absyn_NoInstantiate_e){
_LL767: _temp766=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp690)->f1;
goto _LL715;} else{ goto _LL716;} _LL716: if(*(( int*) _temp690) == Cyc_Absyn_Instantiate_e){
_LL769: _temp768=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp690)->f1; goto
_LL717;} else{ goto _LL718;} _LL718: if(*(( int*) _temp690) == Cyc_Absyn_Cast_e){
goto _LL719;} else{ goto _LL720;} _LL720: if(*(( int*) _temp690) == Cyc_Absyn_New_e){
goto _LL721;} else{ goto _LL722;} _LL722: if(*(( int*) _temp690) == Cyc_Absyn_Address_e){
goto _LL723;} else{ goto _LL724;} _LL724: if(*(( int*) _temp690) == Cyc_Absyn_Sizeoftyp_e){
goto _LL725;} else{ goto _LL726;} _LL726: if(*(( int*) _temp690) == Cyc_Absyn_Sizeofexp_e){
goto _LL727;} else{ goto _LL728;} _LL728: if(*(( int*) _temp690) == Cyc_Absyn_Offsetof_e){
goto _LL729;} else{ goto _LL730;} _LL730: if(*(( int*) _temp690) == Cyc_Absyn_Deref_e){
goto _LL731;} else{ goto _LL732;} _LL732: if(*(( int*) _temp690) == Cyc_Absyn_StructMember_e){
goto _LL733;} else{ goto _LL734;} _LL734: if(*(( int*) _temp690) == Cyc_Absyn_StructArrow_e){
goto _LL735;} else{ goto _LL736;} _LL736: if(*(( int*) _temp690) == Cyc_Absyn_Subscript_e){
goto _LL737;} else{ goto _LL738;} _LL738: if(*(( int*) _temp690) == Cyc_Absyn_Tuple_e){
goto _LL739;} else{ goto _LL740;} _LL740: if(*(( int*) _temp690) == Cyc_Absyn_CompoundLit_e){
goto _LL741;} else{ goto _LL742;} _LL742: if(*(( int*) _temp690) == Cyc_Absyn_Array_e){
goto _LL743;} else{ goto _LL744;} _LL744: if(*(( int*) _temp690) == Cyc_Absyn_Comprehension_e){
goto _LL745;} else{ goto _LL746;} _LL746: if(*(( int*) _temp690) == Cyc_Absyn_Struct_e){
goto _LL747;} else{ goto _LL748;} _LL748: if(*(( int*) _temp690) == Cyc_Absyn_AnonStruct_e){
goto _LL749;} else{ goto _LL750;} _LL750: if(*(( int*) _temp690) == Cyc_Absyn_Tunion_e){
goto _LL751;} else{ goto _LL752;} _LL752: if(*(( int*) _temp690) == Cyc_Absyn_Enum_e){
goto _LL753;} else{ goto _LL754;} _LL754: if(*(( int*) _temp690) == Cyc_Absyn_Malloc_e){
goto _LL755;} else{ goto _LL756;} _LL756: if(*(( int*) _temp690) == Cyc_Absyn_UnresolvedMem_e){
goto _LL757;} else{ goto _LL758;} _LL758: if(*(( int*) _temp690) == Cyc_Absyn_StmtExp_e){
goto _LL759;} else{ goto _LL760;} _LL760: if(*(( int*) _temp690) == Cyc_Absyn_Codegen_e){
goto _LL761;} else{ goto _LL762;} _LL762: if(*(( int*) _temp690) == Cyc_Absyn_Fill_e){
goto _LL763;} else{ goto _LL691;} _LL693: return 10000; _LL695: return 10000;
_LL697: return 10000; _LL699: { void* _temp770= _temp764; _LL772: if( _temp770
==( void*) Cyc_Absyn_Plus){ goto _LL773;} else{ goto _LL774;} _LL774: if(
_temp770 ==( void*) Cyc_Absyn_Times){ goto _LL775;} else{ goto _LL776;} _LL776:
if( _temp770 ==( void*) Cyc_Absyn_Minus){ goto _LL777;} else{ goto _LL778;}
_LL778: if( _temp770 ==( void*) Cyc_Absyn_Div){ goto _LL779;} else{ goto _LL780;}
_LL780: if( _temp770 ==( void*) Cyc_Absyn_Mod){ goto _LL781;} else{ goto _LL782;}
_LL782: if( _temp770 ==( void*) Cyc_Absyn_Eq){ goto _LL783;} else{ goto _LL784;}
_LL784: if( _temp770 ==( void*) Cyc_Absyn_Neq){ goto _LL785;} else{ goto _LL786;}
_LL786: if( _temp770 ==( void*) Cyc_Absyn_Gt){ goto _LL787;} else{ goto _LL788;}
_LL788: if( _temp770 ==( void*) Cyc_Absyn_Lt){ goto _LL789;} else{ goto _LL790;}
_LL790: if( _temp770 ==( void*) Cyc_Absyn_Gte){ goto _LL791;} else{ goto _LL792;}
_LL792: if( _temp770 ==( void*) Cyc_Absyn_Lte){ goto _LL793;} else{ goto _LL794;}
_LL794: if( _temp770 ==( void*) Cyc_Absyn_Not){ goto _LL795;} else{ goto _LL796;}
_LL796: if( _temp770 ==( void*) Cyc_Absyn_Bitnot){ goto _LL797;} else{ goto
_LL798;} _LL798: if( _temp770 ==( void*) Cyc_Absyn_Bitand){ goto _LL799;} else{
goto _LL800;} _LL800: if( _temp770 ==( void*) Cyc_Absyn_Bitor){ goto _LL801;}
else{ goto _LL802;} _LL802: if( _temp770 ==( void*) Cyc_Absyn_Bitxor){ goto
_LL803;} else{ goto _LL804;} _LL804: if( _temp770 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL805;} else{ goto _LL806;} _LL806: if( _temp770 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL807;} else{ goto _LL808;} _LL808: if( _temp770 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL809;} else{ goto _LL810;} _LL810: if( _temp770 ==( void*) Cyc_Absyn_Size){
goto _LL811;} else{ goto _LL771;} _LL773: return 100; _LL775: return 110; _LL777:
return 100; _LL779: return 110; _LL781: return 110; _LL783: return 70; _LL785:
return 70; _LL787: return 80; _LL789: return 80; _LL791: return 80; _LL793:
return 80; _LL795: return 130; _LL797: return 130; _LL799: return 60; _LL801:
return 40; _LL803: return 50; _LL805: return 90; _LL807: return 80; _LL809:
return 80; _LL811: return 140; _LL771:;} _LL701: return 20; _LL703: return 130;
_LL705: return 30; _LL707: return 10; _LL709: return 140; _LL711: return 140;
_LL713: return 130; _LL715: return Cyc_Absynpp_exp_prec( _temp766); _LL717:
return Cyc_Absynpp_exp_prec( _temp768); _LL719: return 120; _LL721: return 15;
_LL723: return 130; _LL725: return 130; _LL727: return 130; _LL729: return 130;
_LL731: return 130; _LL733: return 140; _LL735: return 140; _LL737: return 140;
_LL739: return 150; _LL741: goto _LL743; _LL743: goto _LL745; _LL745: goto
_LL747; _LL747: goto _LL749; _LL749: goto _LL751; _LL751: goto _LL753; _LL753:
goto _LL755; _LL755: return 140; _LL757: return 140; _LL759: return 10000;
_LL761: return 140; _LL763: return 140; _LL691:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec(
e); struct Cyc_PP_Doc* s;{ void* _temp812=( void*) e->r; void* _temp886; struct
_tuple0* _temp888; struct _tuple0* _temp890; struct Cyc_List_List* _temp892;
void* _temp894; struct Cyc_Absyn_Exp* _temp896; struct Cyc_Core_Opt* _temp898;
struct Cyc_Absyn_Exp* _temp900; void* _temp902; struct Cyc_Absyn_Exp* _temp904;
struct Cyc_Absyn_Exp* _temp906; struct Cyc_Absyn_Exp* _temp908; struct Cyc_Absyn_Exp*
_temp910; struct Cyc_Absyn_Exp* _temp912; struct Cyc_Absyn_Exp* _temp914; struct
Cyc_List_List* _temp916; struct Cyc_Absyn_Exp* _temp918; struct Cyc_List_List*
_temp920; struct Cyc_Absyn_Exp* _temp922; struct Cyc_Absyn_Exp* _temp924; struct
Cyc_Absyn_Exp* _temp926; struct Cyc_Absyn_Exp* _temp928; struct Cyc_Absyn_Exp*
_temp930; void* _temp932; struct Cyc_Absyn_Exp* _temp934; struct Cyc_Absyn_Exp*
_temp936; struct Cyc_Absyn_Exp* _temp938; void* _temp940; struct Cyc_Absyn_Exp*
_temp942; struct _tagged_arr* _temp944; void* _temp946; struct Cyc_Absyn_Exp*
_temp948; struct _tagged_arr* _temp950; struct Cyc_Absyn_Exp* _temp952; struct
_tagged_arr* _temp954; struct Cyc_Absyn_Exp* _temp956; struct Cyc_Absyn_Exp*
_temp958; struct Cyc_Absyn_Exp* _temp960; struct Cyc_List_List* _temp962; struct
Cyc_List_List* _temp964; struct _tuple1* _temp966; struct Cyc_List_List*
_temp968; struct Cyc_Absyn_Exp* _temp970; struct Cyc_Absyn_Exp* _temp972; struct
Cyc_Absyn_Vardecl* _temp974; struct Cyc_List_List* _temp976; struct _tuple0*
_temp978; struct Cyc_List_List* _temp980; struct Cyc_Absyn_Tunionfield* _temp982;
struct Cyc_List_List* _temp984; struct _tuple0* _temp986; void* _temp988; struct
Cyc_Absyn_Exp* _temp990; struct Cyc_List_List* _temp992; struct Cyc_Core_Opt*
_temp994; struct Cyc_Absyn_Stmt* _temp996; struct Cyc_Absyn_Fndecl* _temp998;
struct Cyc_Absyn_Exp* _temp1000; _LL814: if(*(( int*) _temp812) == Cyc_Absyn_Const_e){
_LL887: _temp886=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp812)->f1;
goto _LL815;} else{ goto _LL816;} _LL816: if(*(( int*) _temp812) == Cyc_Absyn_Var_e){
_LL889: _temp888=(( struct Cyc_Absyn_Var_e_struct*) _temp812)->f1; goto _LL817;}
else{ goto _LL818;} _LL818: if(*(( int*) _temp812) == Cyc_Absyn_UnknownId_e){
_LL891: _temp890=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp812)->f1; goto
_LL819;} else{ goto _LL820;} _LL820: if(*(( int*) _temp812) == Cyc_Absyn_Primop_e){
_LL895: _temp894=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp812)->f1;
goto _LL893; _LL893: _temp892=(( struct Cyc_Absyn_Primop_e_struct*) _temp812)->f2;
goto _LL821;} else{ goto _LL822;} _LL822: if(*(( int*) _temp812) == Cyc_Absyn_AssignOp_e){
_LL901: _temp900=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp812)->f1; goto
_LL899; _LL899: _temp898=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp812)->f2;
goto _LL897; _LL897: _temp896=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp812)->f3;
goto _LL823;} else{ goto _LL824;} _LL824: if(*(( int*) _temp812) == Cyc_Absyn_Increment_e){
_LL905: _temp904=(( struct Cyc_Absyn_Increment_e_struct*) _temp812)->f1; goto
_LL903; _LL903: _temp902=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp812)->f2; goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp812)
== Cyc_Absyn_Conditional_e){ _LL911: _temp910=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp812)->f1; goto _LL909; _LL909: _temp908=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp812)->f2; goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp812)->f3; goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp812)
== Cyc_Absyn_SeqExp_e){ _LL915: _temp914=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp812)->f1; goto _LL913; _LL913: _temp912=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp812)->f2; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp812)
== Cyc_Absyn_UnknownCall_e){ _LL919: _temp918=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp812)->f1; goto _LL917; _LL917: _temp916=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp812)->f2; goto _LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp812)
== Cyc_Absyn_FnCall_e){ _LL923: _temp922=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp812)->f1; goto _LL921; _LL921: _temp920=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp812)->f2; goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp812)
== Cyc_Absyn_Throw_e){ _LL925: _temp924=(( struct Cyc_Absyn_Throw_e_struct*)
_temp812)->f1; goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp812)
== Cyc_Absyn_NoInstantiate_e){ _LL927: _temp926=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp812)->f1; goto _LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp812)
== Cyc_Absyn_Instantiate_e){ _LL929: _temp928=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp812)->f1; goto _LL839;} else{ goto _LL840;} _LL840: if(*(( int*) _temp812)
== Cyc_Absyn_Cast_e){ _LL933: _temp932=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp812)->f1; goto _LL931; _LL931: _temp930=(( struct Cyc_Absyn_Cast_e_struct*)
_temp812)->f2; goto _LL841;} else{ goto _LL842;} _LL842: if(*(( int*) _temp812)
== Cyc_Absyn_Address_e){ _LL935: _temp934=(( struct Cyc_Absyn_Address_e_struct*)
_temp812)->f1; goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp812)
== Cyc_Absyn_New_e){ _LL939: _temp938=(( struct Cyc_Absyn_New_e_struct*)
_temp812)->f1; goto _LL937; _LL937: _temp936=(( struct Cyc_Absyn_New_e_struct*)
_temp812)->f2; goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*) _temp812)
== Cyc_Absyn_Sizeoftyp_e){ _LL941: _temp940=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp812)->f1; goto _LL847;} else{ goto _LL848;} _LL848: if(*(( int*) _temp812)
== Cyc_Absyn_Sizeofexp_e){ _LL943: _temp942=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp812)->f1; goto _LL849;} else{ goto _LL850;} _LL850: if(*(( int*) _temp812)
== Cyc_Absyn_Offsetof_e){ _LL947: _temp946=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp812)->f1; goto _LL945; _LL945: _temp944=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp812)->f2; goto _LL851;} else{ goto _LL852;} _LL852: if(*(( int*) _temp812)
== Cyc_Absyn_Deref_e){ _LL949: _temp948=(( struct Cyc_Absyn_Deref_e_struct*)
_temp812)->f1; goto _LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp812)
== Cyc_Absyn_StructMember_e){ _LL953: _temp952=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp812)->f1; goto _LL951; _LL951: _temp950=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp812)->f2; goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp812)
== Cyc_Absyn_StructArrow_e){ _LL957: _temp956=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp812)->f1; goto _LL955; _LL955: _temp954=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp812)->f2; goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp812)
== Cyc_Absyn_Subscript_e){ _LL961: _temp960=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp812)->f1; goto _LL959; _LL959: _temp958=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp812)->f2; goto _LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp812)
== Cyc_Absyn_Tuple_e){ _LL963: _temp962=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp812)->f1; goto _LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp812)
== Cyc_Absyn_CompoundLit_e){ _LL967: _temp966=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp812)->f1; goto _LL965; _LL965: _temp964=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp812)->f2; goto _LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp812)
== Cyc_Absyn_Array_e){ _LL969: _temp968=(( struct Cyc_Absyn_Array_e_struct*)
_temp812)->f1; goto _LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp812)
== Cyc_Absyn_Comprehension_e){ _LL975: _temp974=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp812)->f1; goto _LL973; _LL973: _temp972=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp812)->f2; goto _LL971; _LL971: _temp970=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp812)->f3; goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp812)
== Cyc_Absyn_Struct_e){ _LL979: _temp978=(( struct Cyc_Absyn_Struct_e_struct*)
_temp812)->f1; goto _LL977; _LL977: _temp976=(( struct Cyc_Absyn_Struct_e_struct*)
_temp812)->f3; goto _LL869;} else{ goto _LL870;} _LL870: if(*(( int*) _temp812)
== Cyc_Absyn_AnonStruct_e){ _LL981: _temp980=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp812)->f2; goto _LL871;} else{ goto _LL872;} _LL872: if(*(( int*) _temp812)
== Cyc_Absyn_Tunion_e){ _LL985: _temp984=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp812)->f3; goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp812)->f5; goto _LL873;} else{ goto _LL874;} _LL874: if(*(( int*) _temp812)
== Cyc_Absyn_Enum_e){ _LL987: _temp986=(( struct Cyc_Absyn_Enum_e_struct*)
_temp812)->f1; goto _LL875;} else{ goto _LL876;} _LL876: if(*(( int*) _temp812)
== Cyc_Absyn_Malloc_e){ _LL991: _temp990=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp812)->f1; goto _LL989; _LL989: _temp988=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp812)->f2; goto _LL877;} else{ goto _LL878;} _LL878: if(*(( int*) _temp812)
== Cyc_Absyn_UnresolvedMem_e){ _LL995: _temp994=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp812)->f1; goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp812)->f2; goto _LL879;} else{ goto _LL880;} _LL880: if(*(( int*) _temp812)
== Cyc_Absyn_StmtExp_e){ _LL997: _temp996=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp812)->f1; goto _LL881;} else{ goto _LL882;} _LL882: if(*(( int*) _temp812)
== Cyc_Absyn_Codegen_e){ _LL999: _temp998=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp812)->f1; goto _LL883;} else{ goto _LL884;} _LL884: if(*(( int*) _temp812)
== Cyc_Absyn_Fill_e){ _LL1001: _temp1000=(( struct Cyc_Absyn_Fill_e_struct*)
_temp812)->f1; goto _LL885;} else{ goto _LL813;} _LL815: s= Cyc_Absynpp_cnst2doc(
_temp886); goto _LL813; _LL817: _temp890= _temp888; goto _LL819; _LL819: s= Cyc_Absynpp_qvar2doc(
_temp890); goto _LL813; _LL821: s= Cyc_Absynpp_primapp2doc( myprec, _temp894,
_temp892); goto _LL813; _LL823: s=({ struct Cyc_PP_Doc*(* _temp1002)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1005= Cyc_Absynpp_exp2doc_prec(
myprec, _temp900); struct Cyc_PP_Doc* _temp1006= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1007= _temp898 == 0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp898))->v); struct Cyc_PP_Doc* _temp1008=
Cyc_PP_text( _tag_arr("= ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp1009= Cyc_Absynpp_exp2doc_prec( myprec, _temp896); struct Cyc_PP_Doc*
_temp1003[ 5u]={ _temp1005, _temp1006, _temp1007, _temp1008, _temp1009}; struct
_tagged_arr _temp1004={( void*) _temp1003,( void*) _temp1003,( void*)( _temp1003
+ 5u)}; _temp1002( _temp1004);}); goto _LL813; _LL825: { struct Cyc_PP_Doc*
_temp1010= Cyc_Absynpp_exp2doc_prec( myprec, _temp904);{ void* _temp1011=
_temp902; _LL1013: if( _temp1011 ==( void*) Cyc_Absyn_PreInc){ goto _LL1014;}
else{ goto _LL1015;} _LL1015: if( _temp1011 ==( void*) Cyc_Absyn_PreDec){ goto
_LL1016;} else{ goto _LL1017;} _LL1017: if( _temp1011 ==( void*) Cyc_Absyn_PostInc){
goto _LL1018;} else{ goto _LL1019;} _LL1019: if( _temp1011 ==( void*) Cyc_Absyn_PostDec){
goto _LL1020;} else{ goto _LL1012;} _LL1014: s=({ struct Cyc_PP_Doc*(* _temp1021)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1024= Cyc_PP_text(
_tag_arr("++", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1025=
_temp1010; struct Cyc_PP_Doc* _temp1022[ 2u]={ _temp1024, _temp1025}; struct
_tagged_arr _temp1023={( void*) _temp1022,( void*) _temp1022,( void*)( _temp1022
+ 2u)}; _temp1021( _temp1023);}); goto _LL1012; _LL1016: s=({ struct Cyc_PP_Doc*(*
_temp1026)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1029= Cyc_PP_text(
_tag_arr("--", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1030=
_temp1010; struct Cyc_PP_Doc* _temp1027[ 2u]={ _temp1029, _temp1030}; struct
_tagged_arr _temp1028={( void*) _temp1027,( void*) _temp1027,( void*)( _temp1027
+ 2u)}; _temp1026( _temp1028);}); goto _LL1012; _LL1018: s=({ struct Cyc_PP_Doc*(*
_temp1031)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1034=
_temp1010; struct Cyc_PP_Doc* _temp1035= Cyc_PP_text( _tag_arr("++", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1032[ 2u]={ _temp1034, _temp1035};
struct _tagged_arr _temp1033={( void*) _temp1032,( void*) _temp1032,( void*)(
_temp1032 + 2u)}; _temp1031( _temp1033);}); goto _LL1012; _LL1020: s=({ struct
Cyc_PP_Doc*(* _temp1036)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1039= _temp1010; struct Cyc_PP_Doc* _temp1040= Cyc_PP_text( _tag_arr("--",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1037[ 2u]={ _temp1039,
_temp1040}; struct _tagged_arr _temp1038={( void*) _temp1037,( void*) _temp1037,(
void*)( _temp1037 + 2u)}; _temp1036( _temp1038);}); goto _LL1012; _LL1012:;}
goto _LL813;} _LL827:{ struct _tuple8 _temp1042=({ struct _tuple8 _temp1041;
_temp1041.f1=( void*) _temp908->r; _temp1041.f2=( void*) _temp906->r; _temp1041;});
_LL1044: goto _LL1045; _LL1045: s=({ struct Cyc_PP_Doc*(* _temp1046)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1049= Cyc_Absynpp_exp2doc_prec(
myprec, _temp910); struct Cyc_PP_Doc* _temp1050= Cyc_PP_text( _tag_arr(" ? ",
sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1051= Cyc_Absynpp_exp2doc_prec(
0, _temp908); struct Cyc_PP_Doc* _temp1052= Cyc_PP_text( _tag_arr(" : ", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1053= Cyc_Absynpp_exp2doc_prec(
myprec, _temp906); struct Cyc_PP_Doc* _temp1047[ 5u]={ _temp1049, _temp1050,
_temp1051, _temp1052, _temp1053}; struct _tagged_arr _temp1048={( void*)
_temp1047,( void*) _temp1047,( void*)( _temp1047 + 5u)}; _temp1046( _temp1048);});
goto _LL1043; _LL1043:;} goto _LL813; _LL829: s=({ struct Cyc_PP_Doc*(*
_temp1054)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1057= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1058= Cyc_Absynpp_exp2doc(
_temp914); struct Cyc_PP_Doc* _temp1059= Cyc_PP_text( _tag_arr(", ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1060= Cyc_Absynpp_exp2doc(
_temp912); struct Cyc_PP_Doc* _temp1061= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1055[ 5u]={ _temp1057, _temp1058,
_temp1059, _temp1060, _temp1061}; struct _tagged_arr _temp1056={( void*)
_temp1055,( void*) _temp1055,( void*)( _temp1055 + 5u)}; _temp1054( _temp1056);});
goto _LL813; _LL831: s=({ struct Cyc_PP_Doc*(* _temp1062)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1065= Cyc_Absynpp_exp2doc_prec( myprec,
_temp918); struct Cyc_PP_Doc* _temp1066= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1067= Cyc_Absynpp_exps2doc_prec(
20, _temp916); struct Cyc_PP_Doc* _temp1068= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1063[ 4u]={ _temp1065, _temp1066,
_temp1067, _temp1068}; struct _tagged_arr _temp1064={( void*) _temp1063,( void*)
_temp1063,( void*)( _temp1063 + 4u)}; _temp1062( _temp1064);}); goto _LL813;
_LL833: s=({ struct Cyc_PP_Doc*(* _temp1069)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1072= Cyc_Absynpp_exp2doc_prec( myprec, _temp922);
struct Cyc_PP_Doc* _temp1073= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1074= Cyc_Absynpp_exps2doc_prec( 20, _temp920);
struct Cyc_PP_Doc* _temp1075= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1070[ 4u]={ _temp1072, _temp1073, _temp1074,
_temp1075}; struct _tagged_arr _temp1071={( void*) _temp1070,( void*) _temp1070,(
void*)( _temp1070 + 4u)}; _temp1069( _temp1071);}); goto _LL813; _LL835: s=({
struct Cyc_PP_Doc*(* _temp1076)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1079= Cyc_PP_text( _tag_arr("throw ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp1080= Cyc_Absynpp_exp2doc_prec( myprec, _temp924); struct Cyc_PP_Doc*
_temp1077[ 2u]={ _temp1079, _temp1080}; struct _tagged_arr _temp1078={( void*)
_temp1077,( void*) _temp1077,( void*)( _temp1077 + 2u)}; _temp1076( _temp1078);});
goto _LL813; _LL837: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp926); goto _LL813;
_LL839: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp928); goto _LL813; _LL841: s=({
struct Cyc_PP_Doc*(* _temp1081)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1084= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1085= Cyc_Absynpp_typ2doc( _temp932); struct Cyc_PP_Doc* _temp1086= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1087= Cyc_Absynpp_exp2doc_prec(
myprec, _temp930); struct Cyc_PP_Doc* _temp1082[ 4u]={ _temp1084, _temp1085,
_temp1086, _temp1087}; struct _tagged_arr _temp1083={( void*) _temp1082,( void*)
_temp1082,( void*)( _temp1082 + 4u)}; _temp1081( _temp1083);}); goto _LL813;
_LL843: s=({ struct Cyc_PP_Doc*(* _temp1088)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1091= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1092= Cyc_Absynpp_exp2doc_prec( myprec, _temp934);
struct Cyc_PP_Doc* _temp1089[ 2u]={ _temp1091, _temp1092}; struct _tagged_arr
_temp1090={( void*) _temp1089,( void*) _temp1089,( void*)( _temp1089 + 2u)};
_temp1088( _temp1090);}); goto _LL813; _LL845: if( _temp938 == 0){ s=({ struct
Cyc_PP_Doc*(* _temp1093)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1096= Cyc_PP_text( _tag_arr("new ", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1097= Cyc_Absynpp_exp2doc_prec( myprec, _temp936); struct Cyc_PP_Doc*
_temp1094[ 2u]={ _temp1096, _temp1097}; struct _tagged_arr _temp1095={( void*)
_temp1094,( void*) _temp1094,( void*)( _temp1094 + 2u)}; _temp1093( _temp1095);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp1098)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1101= Cyc_PP_text( _tag_arr("rnew(", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp1102= Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*) _check_null( _temp938)); struct Cyc_PP_Doc* _temp1103= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1104= Cyc_Absynpp_exp2doc_prec(
myprec, _temp936); struct Cyc_PP_Doc* _temp1099[ 4u]={ _temp1101, _temp1102,
_temp1103, _temp1104}; struct _tagged_arr _temp1100={( void*) _temp1099,( void*)
_temp1099,( void*)( _temp1099 + 4u)}; _temp1098( _temp1100);});} goto _LL813;
_LL847: s=({ struct Cyc_PP_Doc*(* _temp1105)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1108= Cyc_PP_text( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1109= Cyc_Absynpp_typ2doc(
_temp940); struct Cyc_PP_Doc* _temp1110= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1106[ 3u]={ _temp1108, _temp1109,
_temp1110}; struct _tagged_arr _temp1107={( void*) _temp1106,( void*) _temp1106,(
void*)( _temp1106 + 3u)}; _temp1105( _temp1107);}); goto _LL813; _LL849: s=({
struct Cyc_PP_Doc*(* _temp1111)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1114= Cyc_PP_text( _tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp1115= Cyc_Absynpp_exp2doc( _temp942); struct Cyc_PP_Doc*
_temp1116= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1112[ 3u]={ _temp1114, _temp1115, _temp1116}; struct _tagged_arr _temp1113={(
void*) _temp1112,( void*) _temp1112,( void*)( _temp1112 + 3u)}; _temp1111(
_temp1113);}); goto _LL813; _LL851: s=({ struct Cyc_PP_Doc*(* _temp1117)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1120= Cyc_PP_text( _tag_arr("offsetof(",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1121= Cyc_Absynpp_typ2doc(
_temp946); struct Cyc_PP_Doc* _temp1122= Cyc_PP_text( _tag_arr(",", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1123= Cyc_PP_textptr( _temp944);
struct Cyc_PP_Doc* _temp1124= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1118[ 5u]={ _temp1120, _temp1121, _temp1122,
_temp1123, _temp1124}; struct _tagged_arr _temp1119={( void*) _temp1118,( void*)
_temp1118,( void*)( _temp1118 + 5u)}; _temp1117( _temp1119);}); goto _LL813;
_LL853: s=({ struct Cyc_PP_Doc*(* _temp1125)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1128= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1129= Cyc_Absynpp_exp2doc_prec( myprec, _temp948);
struct Cyc_PP_Doc* _temp1126[ 2u]={ _temp1128, _temp1129}; struct _tagged_arr
_temp1127={( void*) _temp1126,( void*) _temp1126,( void*)( _temp1126 + 2u)};
_temp1125( _temp1127);}); goto _LL813; _LL855: s=({ struct Cyc_PP_Doc*(*
_temp1130)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1133= Cyc_Absynpp_exp2doc_prec(
myprec, _temp952); struct Cyc_PP_Doc* _temp1134= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1135= Cyc_PP_textptr(
_temp950); struct Cyc_PP_Doc* _temp1131[ 3u]={ _temp1133, _temp1134, _temp1135};
struct _tagged_arr _temp1132={( void*) _temp1131,( void*) _temp1131,( void*)(
_temp1131 + 3u)}; _temp1130( _temp1132);}); goto _LL813; _LL857: s=({ struct Cyc_PP_Doc*(*
_temp1136)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1139= Cyc_Absynpp_exp2doc_prec(
myprec, _temp956); struct Cyc_PP_Doc* _temp1140= Cyc_PP_text( _tag_arr("->",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1141= Cyc_PP_textptr(
_temp954); struct Cyc_PP_Doc* _temp1137[ 3u]={ _temp1139, _temp1140, _temp1141};
struct _tagged_arr _temp1138={( void*) _temp1137,( void*) _temp1137,( void*)(
_temp1137 + 3u)}; _temp1136( _temp1138);}); goto _LL813; _LL859: s=({ struct Cyc_PP_Doc*(*
_temp1142)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1145= Cyc_Absynpp_exp2doc_prec(
myprec, _temp960); struct Cyc_PP_Doc* _temp1146= Cyc_PP_text( _tag_arr("[",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1147= Cyc_Absynpp_exp2doc(
_temp958); struct Cyc_PP_Doc* _temp1148= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1143[ 4u]={ _temp1145, _temp1146,
_temp1147, _temp1148}; struct _tagged_arr _temp1144={( void*) _temp1143,( void*)
_temp1143,( void*)( _temp1143 + 4u)}; _temp1142( _temp1144);}); goto _LL813;
_LL861: s=({ struct Cyc_PP_Doc*(* _temp1149)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1152= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp1153= Cyc_Absynpp_exps2doc_prec( 20, _temp962);
struct Cyc_PP_Doc* _temp1154= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1150[ 3u]={ _temp1152, _temp1153, _temp1154};
struct _tagged_arr _temp1151={( void*) _temp1150,( void*) _temp1150,( void*)(
_temp1150 + 3u)}; _temp1149( _temp1151);}); goto _LL813; _LL863: s=({ struct Cyc_PP_Doc*(*
_temp1155)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1158= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1159= Cyc_Absynpp_typ2doc((*
_temp966).f3); struct Cyc_PP_Doc* _temp1160= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1161= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp964)); struct Cyc_PP_Doc* _temp1156[ 4u]={ _temp1158, _temp1159, _temp1160,
_temp1161}; struct _tagged_arr _temp1157={( void*) _temp1156,( void*) _temp1156,(
void*)( _temp1156 + 4u)}; _temp1155( _temp1157);}); goto _LL813; _LL865: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp968)); goto _LL813; _LL867: s=({ struct Cyc_PP_Doc*(*
_temp1162)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1165= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1166=
Cyc_PP_text(*(* _temp974->name).f2); struct Cyc_PP_Doc* _temp1167= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1168= Cyc_Absynpp_exp2doc(
_temp972); struct Cyc_PP_Doc* _temp1169= Cyc_PP_text( _tag_arr(" : ", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1170= Cyc_Absynpp_exp2doc(
_temp970); struct Cyc_PP_Doc* _temp1171= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1163[ 7u]={ _temp1165, _temp1166,
_temp1167, _temp1168, _temp1169, _temp1170, _temp1171}; struct _tagged_arr
_temp1164={( void*) _temp1163,( void*) _temp1163,( void*)( _temp1163 + 7u)};
_temp1162( _temp1164);}); goto _LL813; _LL869: s=({ struct Cyc_PP_Doc*(*
_temp1172)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1175= Cyc_Absynpp_qvar2doc(
_temp978); struct Cyc_PP_Doc* _temp1176= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp976)); struct Cyc_PP_Doc* _temp1173[ 2u]={ _temp1175, _temp1176}; struct
_tagged_arr _temp1174={( void*) _temp1173,( void*) _temp1173,( void*)( _temp1173
+ 2u)}; _temp1172( _temp1174);}); goto _LL813; _LL871: s= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp980)); goto _LL813; _LL873: if( _temp984 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp982->name);} else{ s=({ struct Cyc_PP_Doc*(* _temp1177)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1180= Cyc_Absynpp_qvar2doc( _temp982->name);
struct Cyc_PP_Doc* _temp1181= Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char),
2u), _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc,
_temp984)); struct Cyc_PP_Doc* _temp1178[ 2u]={ _temp1180, _temp1181}; struct
_tagged_arr _temp1179={( void*) _temp1178,( void*) _temp1178,( void*)( _temp1178
+ 2u)}; _temp1177( _temp1179);});} goto _LL813; _LL875: s= Cyc_Absynpp_qvar2doc(
_temp986); goto _LL813; _LL877: if( _temp990 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp1182)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1185= Cyc_PP_text(
_tag_arr("malloc(sizeof(", sizeof( unsigned char), 15u)); struct Cyc_PP_Doc*
_temp1186= Cyc_Absynpp_typ2doc( _temp988); struct Cyc_PP_Doc* _temp1187= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1183[ 3u]={
_temp1185, _temp1186, _temp1187}; struct _tagged_arr _temp1184={( void*)
_temp1183,( void*) _temp1183,( void*)( _temp1183 + 3u)}; _temp1182( _temp1184);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp1188)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1191= Cyc_PP_text( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); struct Cyc_PP_Doc* _temp1192= Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*) _check_null( _temp990)); struct Cyc_PP_Doc* _temp1193= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1194= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1195=
Cyc_Absynpp_typ2doc( _temp988); struct Cyc_PP_Doc* _temp1196= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1189[ 6u]={
_temp1191, _temp1192, _temp1193, _temp1194, _temp1195, _temp1196}; struct
_tagged_arr _temp1190={( void*) _temp1189,( void*) _temp1189,( void*)( _temp1189
+ 6u)}; _temp1188( _temp1190);});} goto _LL813; _LL879: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp992)); goto _LL813; _LL881: s=({ struct Cyc_PP_Doc*(*
_temp1197)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1200= Cyc_PP_text(
_tag_arr("({ ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1201= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp996)); struct Cyc_PP_Doc* _temp1202= Cyc_PP_text(
_tag_arr(" })", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1198[ 3u]={
_temp1200, _temp1201, _temp1202}; struct _tagged_arr _temp1199={( void*)
_temp1198,( void*) _temp1198,( void*)( _temp1198 + 3u)}; _temp1197( _temp1199);});
goto _LL813; _LL883: s=({ struct Cyc_PP_Doc*(* _temp1203)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1206= Cyc_PP_text( _tag_arr("codegen(",
sizeof( unsigned char), 9u)); struct Cyc_PP_Doc* _temp1207= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp1209=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp1209->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1210=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1210[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1211; _temp1211.tag= Cyc_Absyn_Fn_d;
_temp1211.f1= _temp998; _temp1211;}); _temp1210;})); _temp1209->loc= e->loc;
_temp1209;}))); struct Cyc_PP_Doc* _temp1208= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1204[ 3u]={ _temp1206, _temp1207,
_temp1208}; struct _tagged_arr _temp1205={( void*) _temp1204,( void*) _temp1204,(
void*)( _temp1204 + 3u)}; _temp1203( _temp1205);}); goto _LL813; _LL885: s=({
struct Cyc_PP_Doc*(* _temp1212)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1215= Cyc_PP_text( _tag_arr("fill(", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp1216= Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1000)); struct
Cyc_PP_Doc* _temp1217= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1213[ 3u]={ _temp1215, _temp1216, _temp1217}; struct
_tagged_arr _temp1214={( void*) _temp1213,( void*) _temp1213,( void*)( _temp1213
+ 3u)}; _temp1212( _temp1214);}); goto _LL813; _LL813:;} if( inprec >= myprec){
s=({ struct Cyc_PP_Doc*(* _temp1218)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp1221= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1222= s; struct Cyc_PP_Doc* _temp1223= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1219[ 3u]={
_temp1221, _temp1222, _temp1223}; struct _tagged_arr _temp1220={( void*)
_temp1219,( void*) _temp1219,( void*)( _temp1219 + 3u)}; _temp1218( _temp1220);});}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void*
_temp1224= d; struct Cyc_Absyn_Exp* _temp1230; struct _tagged_arr* _temp1232;
_LL1226: if(*(( int*) _temp1224) == Cyc_Absyn_ArrayElement){ _LL1231: _temp1230=((
struct Cyc_Absyn_ArrayElement_struct*) _temp1224)->f1; goto _LL1227;} else{ goto
_LL1228;} _LL1228: if(*(( int*) _temp1224) == Cyc_Absyn_FieldName){ _LL1233:
_temp1232=(( struct Cyc_Absyn_FieldName_struct*) _temp1224)->f1; goto _LL1229;}
else{ goto _LL1225;} _LL1227: return({ struct Cyc_PP_Doc*(* _temp1234)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1237= Cyc_PP_text( _tag_arr(".[",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1238= Cyc_Absynpp_exp2doc(
_temp1230); struct Cyc_PP_Doc* _temp1239= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1235[ 3u]={ _temp1237, _temp1238,
_temp1239}; struct _tagged_arr _temp1236={( void*) _temp1235,( void*) _temp1235,(
void*)( _temp1235 + 3u)}; _temp1234( _temp1236);}); _LL1229: return({ struct Cyc_PP_Doc*(*
_temp1240)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1243= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1244= Cyc_PP_textptr(
_temp1232); struct Cyc_PP_Doc* _temp1241[ 2u]={ _temp1243, _temp1244}; struct
_tagged_arr _temp1242={( void*) _temp1241,( void*) _temp1241,( void*)( _temp1241
+ 2u)}; _temp1240( _temp1242);}); _LL1225:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);}
else{ return({ struct Cyc_PP_Doc*(* _temp1245)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1248= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* de).f1));
struct Cyc_PP_Doc* _temp1249= Cyc_Absynpp_exp2doc((* de).f2); struct Cyc_PP_Doc*
_temp1246[ 2u]={ _temp1248, _temp1249}; struct _tagged_arr _temp1247={( void*)
_temp1246,( void*) _temp1246,( void*)( _temp1246 + 2u)}; _temp1245( _temp1247);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp1250= c;
unsigned char _temp1268; void* _temp1270; short _temp1272; void* _temp1274; int
_temp1276; void* _temp1278; int _temp1280; void* _temp1282; long long _temp1284;
void* _temp1286; struct _tagged_arr _temp1288; struct _tagged_arr _temp1290;
_LL1252: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Char_c:
0){ _LL1271: _temp1270=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1250)->f1;
goto _LL1269; _LL1269: _temp1268=(( struct Cyc_Absyn_Char_c_struct*) _temp1250)->f2;
goto _LL1253;} else{ goto _LL1254;} _LL1254: if(( unsigned int) _temp1250 > 1u?*((
int*) _temp1250) == Cyc_Absyn_Short_c: 0){ _LL1275: _temp1274=( void*)(( struct
Cyc_Absyn_Short_c_struct*) _temp1250)->f1; goto _LL1273; _LL1273: _temp1272=((
struct Cyc_Absyn_Short_c_struct*) _temp1250)->f2; goto _LL1255;} else{ goto
_LL1256;} _LL1256: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Int_c:
0){ _LL1279: _temp1278=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1250)->f1;
if( _temp1278 ==( void*) Cyc_Absyn_Signed){ goto _LL1277;} else{ goto _LL1258;}
_LL1277: _temp1276=(( struct Cyc_Absyn_Int_c_struct*) _temp1250)->f2; goto
_LL1257;} else{ goto _LL1258;} _LL1258: if(( unsigned int) _temp1250 > 1u?*((
int*) _temp1250) == Cyc_Absyn_Int_c: 0){ _LL1283: _temp1282=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1250)->f1; if( _temp1282 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1281;} else{ goto _LL1260;} _LL1281: _temp1280=(( struct Cyc_Absyn_Int_c_struct*)
_temp1250)->f2; goto _LL1259;} else{ goto _LL1260;} _LL1260: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_LongLong_c: 0){ _LL1287:
_temp1286=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1250)->f1; goto
_LL1285; _LL1285: _temp1284=(( struct Cyc_Absyn_LongLong_c_struct*) _temp1250)->f2;
goto _LL1261;} else{ goto _LL1262;} _LL1262: if(( unsigned int) _temp1250 > 1u?*((
int*) _temp1250) == Cyc_Absyn_Float_c: 0){ _LL1289: _temp1288=(( struct Cyc_Absyn_Float_c_struct*)
_temp1250)->f1; goto _LL1263;} else{ goto _LL1264;} _LL1264: if( _temp1250 ==(
void*) Cyc_Absyn_Null_c){ goto _LL1265;} else{ goto _LL1266;} _LL1266: if((
unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_String_c: 0){
_LL1291: _temp1290=(( struct Cyc_Absyn_String_c_struct*) _temp1250)->f1; goto
_LL1267;} else{ goto _LL1251;} _LL1253: return Cyc_PP_text(( struct _tagged_arr)({
struct _tagged_arr(* _temp1292)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp1296= _tag_arr("'%s'", sizeof(
unsigned char), 5u); struct Cyc_Stdio_String_pa_struct _temp1297; _temp1297.tag=
Cyc_Stdio_String_pa; _temp1297.f1=( struct _tagged_arr) Cyc_Absynpp_char_escape(
_temp1268);{ void* _temp1295=( void*)& _temp1297; void* _temp1293[ 1u]={
_temp1295}; struct _tagged_arr _temp1294={( void*) _temp1293,( void*) _temp1293,(
void*)( _temp1293 + 1u)}; _temp1292( _temp1296, _temp1294);}})); _LL1255: return
Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr(* _temp1298)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1302= _tag_arr("%d", sizeof( unsigned char), 3u); struct Cyc_Stdio_Int_pa_struct
_temp1303; _temp1303.tag= Cyc_Stdio_Int_pa; _temp1303.f1=( int)(( unsigned int)((
int) _temp1272));{ void* _temp1301=( void*)& _temp1303; void* _temp1299[ 1u]={
_temp1301}; struct _tagged_arr _temp1300={( void*) _temp1299,( void*) _temp1299,(
void*)( _temp1299 + 1u)}; _temp1298( _temp1302, _temp1300);}})); _LL1257: return
Cyc_PP_text(( struct _tagged_arr) Cyc_Core_string_of_int( _temp1276)); _LL1259:
return Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr(* _temp1304)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp1308= _tag_arr("%u", sizeof( unsigned char), 3u); struct Cyc_Stdio_Int_pa_struct
_temp1309; _temp1309.tag= Cyc_Stdio_Int_pa; _temp1309.f1=( unsigned int)
_temp1280;{ void* _temp1307=( void*)& _temp1309; void* _temp1305[ 1u]={
_temp1307}; struct _tagged_arr _temp1306={( void*) _temp1305,( void*) _temp1305,(
void*)( _temp1305 + 1u)}; _temp1304( _temp1308, _temp1306);}})); _LL1261: return
Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char), 27u));
_LL1263: return Cyc_PP_text( _temp1288); _LL1265: return Cyc_PP_text( _tag_arr("null",
sizeof( unsigned char), 5u)); _LL1267: return({ struct Cyc_PP_Doc*(* _temp1310)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1313= Cyc_PP_text(
_tag_arr("\"", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1314= Cyc_PP_text(
Cyc_Absynpp_string_escape( _temp1290)); struct Cyc_PP_Doc* _temp1315= Cyc_PP_text(
_tag_arr("\"", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1311[ 3u]={
_temp1313, _temp1314, _temp1315}; struct _tagged_arr _temp1312={( void*)
_temp1311,( void*) _temp1311,( void*)( _temp1311 + 3u)}; _temp1310( _temp1312);});
_LL1251:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc( int inprec, void* p,
struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if(
p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1316=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1316[ 0]=({ struct Cyc_Core_Failure_struct _temp1317; _temp1317.tag= Cyc_Core_Failure;
_temp1317.f1=( struct _tagged_arr)({ struct _tagged_arr(* _temp1318)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1322= _tag_arr("Absynpp::primapp2doc Size: %s with bad args", sizeof(
unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp1323; _temp1323.tag=
Cyc_Stdio_String_pa; _temp1323.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps,
72);{ void* _temp1321=( void*)& _temp1323; void* _temp1319[ 1u]={ _temp1321};
struct _tagged_arr _temp1320={( void*) _temp1319,( void*) _temp1319,( void*)(
_temp1319 + 1u)}; _temp1318( _temp1322, _temp1320);}}); _temp1317;}); _temp1316;}));}{
struct Cyc_PP_Doc* _temp1324= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); return({ struct Cyc_PP_Doc*(*
_temp1325)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1328=
_temp1324; struct Cyc_PP_Doc* _temp1329= Cyc_PP_text( _tag_arr(".size", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp1326[ 2u]={ _temp1328, _temp1329};
struct _tagged_arr _temp1327={( void*) _temp1326,( void*) _temp1326,( void*)(
_temp1326 + 2u)}; _temp1325( _temp1327);});}} else{ struct Cyc_List_List* ds=((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1330=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1330[ 0]=({ struct Cyc_Core_Failure_struct _temp1331; _temp1331.tag= Cyc_Core_Failure;
_temp1331.f1=( struct _tagged_arr)({ struct _tagged_arr(* _temp1332)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1336= _tag_arr("Absynpp::primapp2doc: %s with no args", sizeof(
unsigned char), 38u); struct Cyc_Stdio_String_pa_struct _temp1337; _temp1337.tag=
Cyc_Stdio_String_pa; _temp1337.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps,
72);{ void* _temp1335=( void*)& _temp1337; void* _temp1333[ 1u]={ _temp1335};
struct _tagged_arr _temp1334={( void*) _temp1333,( void*) _temp1333,( void*)(
_temp1333 + 1u)}; _temp1332( _temp1336, _temp1334);}}); _temp1331;}); _temp1330;}));}
else{ if((( struct Cyc_List_List*) _check_null( ds))->tl == 0){ return({ struct
Cyc_PP_Doc*(* _temp1338)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1341= ps; struct Cyc_PP_Doc* _temp1342= Cyc_PP_text( _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1343=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_PP_Doc* _temp1339[ 3u]={
_temp1341, _temp1342, _temp1343}; struct _tagged_arr _temp1340={( void*)
_temp1339,( void*) _temp1339,( void*)( _temp1339 + 3u)}; _temp1338( _temp1340);});}
else{ if((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( ds))->tl))->tl != 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1344=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1344[ 0]=({ struct Cyc_Core_Failure_struct _temp1345; _temp1345.tag= Cyc_Core_Failure;
_temp1345.f1=( struct _tagged_arr)({ struct _tagged_arr(* _temp1346)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1350= _tag_arr("Absynpp::primapp2doc: %s with more than 2 args", sizeof(
unsigned char), 47u); struct Cyc_Stdio_String_pa_struct _temp1351; _temp1351.tag=
Cyc_Stdio_String_pa; _temp1351.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps,
72);{ void* _temp1349=( void*)& _temp1351; void* _temp1347[ 1u]={ _temp1349};
struct _tagged_arr _temp1348={( void*) _temp1347,( void*) _temp1347,( void*)(
_temp1347 + 1u)}; _temp1346( _temp1350, _temp1348);}}); _temp1345;}); _temp1344;}));}
else{ return({ struct Cyc_PP_Doc*(* _temp1352)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1355=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_PP_Doc* _temp1356= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1357= ps; struct Cyc_PP_Doc*
_temp1358= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1359=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( ds))->tl))->hd; struct Cyc_PP_Doc* _temp1353[ 5u]={
_temp1355, _temp1356, _temp1357, _temp1358, _temp1359}; struct _tagged_arr
_temp1354={( void*) _temp1353,( void*) _temp1353,( void*)( _temp1353 + 5u)};
_temp1352( _temp1354);});}}}}} struct _tagged_arr Cyc_Absynpp_prim2str( void* p){
void* _temp1360= p; _LL1362: if( _temp1360 ==( void*) Cyc_Absyn_Plus){ goto
_LL1363;} else{ goto _LL1364;} _LL1364: if( _temp1360 ==( void*) Cyc_Absyn_Times){
goto _LL1365;} else{ goto _LL1366;} _LL1366: if( _temp1360 ==( void*) Cyc_Absyn_Minus){
goto _LL1367;} else{ goto _LL1368;} _LL1368: if( _temp1360 ==( void*) Cyc_Absyn_Div){
goto _LL1369;} else{ goto _LL1370;} _LL1370: if( _temp1360 ==( void*) Cyc_Absyn_Mod){
goto _LL1371;} else{ goto _LL1372;} _LL1372: if( _temp1360 ==( void*) Cyc_Absyn_Eq){
goto _LL1373;} else{ goto _LL1374;} _LL1374: if( _temp1360 ==( void*) Cyc_Absyn_Neq){
goto _LL1375;} else{ goto _LL1376;} _LL1376: if( _temp1360 ==( void*) Cyc_Absyn_Gt){
goto _LL1377;} else{ goto _LL1378;} _LL1378: if( _temp1360 ==( void*) Cyc_Absyn_Lt){
goto _LL1379;} else{ goto _LL1380;} _LL1380: if( _temp1360 ==( void*) Cyc_Absyn_Gte){
goto _LL1381;} else{ goto _LL1382;} _LL1382: if( _temp1360 ==( void*) Cyc_Absyn_Lte){
goto _LL1383;} else{ goto _LL1384;} _LL1384: if( _temp1360 ==( void*) Cyc_Absyn_Not){
goto _LL1385;} else{ goto _LL1386;} _LL1386: if( _temp1360 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1387;} else{ goto _LL1388;} _LL1388: if( _temp1360 ==( void*) Cyc_Absyn_Bitand){
goto _LL1389;} else{ goto _LL1390;} _LL1390: if( _temp1360 ==( void*) Cyc_Absyn_Bitor){
goto _LL1391;} else{ goto _LL1392;} _LL1392: if( _temp1360 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1393;} else{ goto _LL1394;} _LL1394: if( _temp1360 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1395;} else{ goto _LL1396;} _LL1396: if( _temp1360 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1397;} else{ goto _LL1398;} _LL1398: if( _temp1360 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1399;} else{ goto _LL1400;} _LL1400: if( _temp1360 ==( void*) Cyc_Absyn_Size){
goto _LL1401;} else{ goto _LL1361;} _LL1363: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL1365: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL1367: return _tag_arr("-", sizeof( unsigned char), 2u); _LL1369: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL1371: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL1373: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL1375: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL1377: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL1379: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL1381: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL1383: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL1385: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL1387: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL1389: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL1391: return _tag_arr("|", sizeof( unsigned char), 2u); _LL1393: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL1395: return _tag_arr("<<",
sizeof( unsigned char), 3u); _LL1397: return _tag_arr(">>", sizeof(
unsigned char), 3u); _LL1399: return _tag_arr(">>>", sizeof( unsigned char), 4u);
_LL1401: return _tag_arr("size", sizeof( unsigned char), 5u); _LL1361:;} struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 ==
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp1402)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1405= Cyc_PP_textptr((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v); struct
Cyc_PP_Doc* _temp1406= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
struct Cyc_PP_Doc* _temp1407= Cyc_Absynpp_exp2doc((* t).f2); struct Cyc_PP_Doc*
_temp1403[ 3u]={ _temp1405, _temp1406, _temp1407}; struct _tagged_arr _temp1404={(
void*) _temp1403,( void*) _temp1403,( void*)( _temp1403 + 3u)}; _temp1402(
_temp1404);});}} int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void*
_temp1408=( void*) s->r; _LL1410: if(( unsigned int) _temp1408 > 1u?*(( int*)
_temp1408) == Cyc_Absyn_Decl_s: 0){ goto _LL1411;} else{ goto _LL1412;} _LL1412:
goto _LL1413; _LL1411: return 1; _LL1413: return 0; _LL1409:;} struct Cyc_PP_Doc*
Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void*
_temp1414=( void*) st->r; struct Cyc_Absyn_Exp* _temp1458; struct Cyc_Absyn_Stmt*
_temp1460; struct Cyc_Absyn_Stmt* _temp1462; struct Cyc_Absyn_Exp* _temp1464;
struct Cyc_Absyn_Stmt* _temp1466; struct Cyc_Absyn_Stmt* _temp1468; struct Cyc_Absyn_Exp*
_temp1470; struct Cyc_Absyn_Stmt* _temp1472; struct _tuple2 _temp1474; struct
Cyc_Absyn_Exp* _temp1476; struct _tagged_arr* _temp1478; struct Cyc_Absyn_Stmt*
_temp1480; struct _tuple2 _temp1482; struct Cyc_Absyn_Exp* _temp1484; struct
_tuple2 _temp1486; struct Cyc_Absyn_Exp* _temp1488; struct Cyc_Absyn_Exp*
_temp1490; struct Cyc_List_List* _temp1492; struct Cyc_Absyn_Exp* _temp1494;
struct Cyc_List_List* _temp1496; struct Cyc_Absyn_Exp* _temp1498; struct Cyc_List_List*
_temp1500; struct Cyc_List_List* _temp1502; struct Cyc_Absyn_Stmt* _temp1504;
struct Cyc_Absyn_Decl* _temp1506; struct Cyc_Absyn_Stmt* _temp1508; struct Cyc_Absyn_Stmt*
_temp1510; struct Cyc_Absyn_Stmt* _temp1512; struct _tagged_arr* _temp1514;
struct _tuple2 _temp1516; struct Cyc_Absyn_Exp* _temp1518; struct Cyc_Absyn_Stmt*
_temp1520; struct Cyc_List_List* _temp1522; struct Cyc_Absyn_Stmt* _temp1524;
struct Cyc_Absyn_Stmt* _temp1526; struct Cyc_Absyn_Vardecl* _temp1528; struct
Cyc_Absyn_Tvar* _temp1530; _LL1416: if( _temp1414 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1417;} else{ goto _LL1418;} _LL1418: if(( unsigned int) _temp1414 > 1u?*((
int*) _temp1414) == Cyc_Absyn_Exp_s: 0){ _LL1459: _temp1458=(( struct Cyc_Absyn_Exp_s_struct*)
_temp1414)->f1; goto _LL1419;} else{ goto _LL1420;} _LL1420: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Seq_s: 0){ _LL1463: _temp1462=((
struct Cyc_Absyn_Seq_s_struct*) _temp1414)->f1; goto _LL1461; _LL1461: _temp1460=((
struct Cyc_Absyn_Seq_s_struct*) _temp1414)->f2; goto _LL1421;} else{ goto
_LL1422;} _LL1422: if(( unsigned int) _temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Return_s:
0){ _LL1465: _temp1464=(( struct Cyc_Absyn_Return_s_struct*) _temp1414)->f1;
goto _LL1423;} else{ goto _LL1424;} _LL1424: if(( unsigned int) _temp1414 > 1u?*((
int*) _temp1414) == Cyc_Absyn_IfThenElse_s: 0){ _LL1471: _temp1470=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1414)->f1; goto _LL1469; _LL1469: _temp1468=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1414)->f2; goto _LL1467; _LL1467: _temp1466=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1414)->f3; goto _LL1425;} else{ goto _LL1426;} _LL1426: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_While_s: 0){ _LL1475: _temp1474=((
struct Cyc_Absyn_While_s_struct*) _temp1414)->f1; _LL1477: _temp1476= _temp1474.f1;
goto _LL1473; _LL1473: _temp1472=(( struct Cyc_Absyn_While_s_struct*) _temp1414)->f2;
goto _LL1427;} else{ goto _LL1428;} _LL1428: if(( unsigned int) _temp1414 > 1u?*((
int*) _temp1414) == Cyc_Absyn_Break_s: 0){ goto _LL1429;} else{ goto _LL1430;}
_LL1430: if(( unsigned int) _temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Continue_s:
0){ goto _LL1431;} else{ goto _LL1432;} _LL1432: if(( unsigned int) _temp1414 >
1u?*(( int*) _temp1414) == Cyc_Absyn_Goto_s: 0){ _LL1479: _temp1478=(( struct
Cyc_Absyn_Goto_s_struct*) _temp1414)->f1; goto _LL1433;} else{ goto _LL1434;}
_LL1434: if(( unsigned int) _temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_For_s:
0){ _LL1491: _temp1490=(( struct Cyc_Absyn_For_s_struct*) _temp1414)->f1; goto
_LL1487; _LL1487: _temp1486=(( struct Cyc_Absyn_For_s_struct*) _temp1414)->f2;
_LL1489: _temp1488= _temp1486.f1; goto _LL1483; _LL1483: _temp1482=(( struct Cyc_Absyn_For_s_struct*)
_temp1414)->f3; _LL1485: _temp1484= _temp1482.f1; goto _LL1481; _LL1481:
_temp1480=(( struct Cyc_Absyn_For_s_struct*) _temp1414)->f4; goto _LL1435;}
else{ goto _LL1436;} _LL1436: if(( unsigned int) _temp1414 > 1u?*(( int*)
_temp1414) == Cyc_Absyn_Switch_s: 0){ _LL1495: _temp1494=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1414)->f1; goto _LL1493; _LL1493: _temp1492=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1414)->f2; goto _LL1437;} else{ goto _LL1438;} _LL1438: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_SwitchC_s: 0){ _LL1499:
_temp1498=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp1414)->f1; goto _LL1497;
_LL1497: _temp1496=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp1414)->f2; goto
_LL1439;} else{ goto _LL1440;} _LL1440: if(( unsigned int) _temp1414 > 1u?*((
int*) _temp1414) == Cyc_Absyn_Fallthru_s: 0){ _LL1501: _temp1500=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1414)->f1; if( _temp1500 == 0){ goto _LL1441;} else{ goto _LL1442;}} else{
goto _LL1442;} _LL1442: if(( unsigned int) _temp1414 > 1u?*(( int*) _temp1414)
== Cyc_Absyn_Fallthru_s: 0){ _LL1503: _temp1502=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1414)->f1; goto _LL1443;} else{ goto _LL1444;} _LL1444: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Decl_s: 0){ _LL1507: _temp1506=((
struct Cyc_Absyn_Decl_s_struct*) _temp1414)->f1; goto _LL1505; _LL1505:
_temp1504=(( struct Cyc_Absyn_Decl_s_struct*) _temp1414)->f2; goto _LL1445;}
else{ goto _LL1446;} _LL1446: if(( unsigned int) _temp1414 > 1u?*(( int*)
_temp1414) == Cyc_Absyn_Cut_s: 0){ _LL1509: _temp1508=(( struct Cyc_Absyn_Cut_s_struct*)
_temp1414)->f1; goto _LL1447;} else{ goto _LL1448;} _LL1448: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Splice_s: 0){ _LL1511:
_temp1510=(( struct Cyc_Absyn_Splice_s_struct*) _temp1414)->f1; goto _LL1449;}
else{ goto _LL1450;} _LL1450: if(( unsigned int) _temp1414 > 1u?*(( int*)
_temp1414) == Cyc_Absyn_Label_s: 0){ _LL1515: _temp1514=(( struct Cyc_Absyn_Label_s_struct*)
_temp1414)->f1; goto _LL1513; _LL1513: _temp1512=(( struct Cyc_Absyn_Label_s_struct*)
_temp1414)->f2; goto _LL1451;} else{ goto _LL1452;} _LL1452: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Do_s: 0){ _LL1521: _temp1520=((
struct Cyc_Absyn_Do_s_struct*) _temp1414)->f1; goto _LL1517; _LL1517: _temp1516=((
struct Cyc_Absyn_Do_s_struct*) _temp1414)->f2; _LL1519: _temp1518= _temp1516.f1;
goto _LL1453;} else{ goto _LL1454;} _LL1454: if(( unsigned int) _temp1414 > 1u?*((
int*) _temp1414) == Cyc_Absyn_TryCatch_s: 0){ _LL1525: _temp1524=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1414)->f1; goto _LL1523; _LL1523: _temp1522=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1414)->f2; goto _LL1455;} else{ goto _LL1456;} _LL1456: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Region_s: 0){ _LL1531:
_temp1530=(( struct Cyc_Absyn_Region_s_struct*) _temp1414)->f1; goto _LL1529;
_LL1529: _temp1528=(( struct Cyc_Absyn_Region_s_struct*) _temp1414)->f2; goto
_LL1527; _LL1527: _temp1526=(( struct Cyc_Absyn_Region_s_struct*) _temp1414)->f3;
goto _LL1457;} else{ goto _LL1415;} _LL1417: s= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); goto _LL1415; _LL1419: s=({ struct Cyc_PP_Doc*(*
_temp1532)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1535= Cyc_Absynpp_exp2doc(
_temp1458); struct Cyc_PP_Doc* _temp1536= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1533[ 2u]={ _temp1535, _temp1536};
struct _tagged_arr _temp1534={( void*) _temp1533,( void*) _temp1533,( void*)(
_temp1533 + 2u)}; _temp1532( _temp1534);}); goto _LL1415; _LL1421: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1462)){ s=({ struct Cyc_PP_Doc*(* _temp1537)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1540= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1541= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1462)); struct Cyc_PP_Doc* _temp1542= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1543= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1544= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1545= Cyc_Absynpp_is_declaration( _temp1460)?({ struct Cyc_PP_Doc*(*
_temp1546)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1549= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1550= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1460)); struct Cyc_PP_Doc* _temp1551= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1552= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1547[ 4u]={ _temp1549, _temp1550, _temp1551, _temp1552};
struct _tagged_arr _temp1548={( void*) _temp1547,( void*) _temp1547,( void*)(
_temp1547 + 4u)}; _temp1546( _temp1548);}): Cyc_Absynpp_stmt2doc( _temp1460);
struct Cyc_PP_Doc* _temp1538[ 6u]={ _temp1540, _temp1541, _temp1542, _temp1543,
_temp1544, _temp1545}; struct _tagged_arr _temp1539={( void*) _temp1538,( void*)
_temp1538,( void*)( _temp1538 + 6u)}; _temp1537( _temp1539);});} else{ if( Cyc_Absynpp_is_declaration(
_temp1460)){ s=({ struct Cyc_PP_Doc*(* _temp1553)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1556= Cyc_Absynpp_stmt2doc( _temp1462); struct Cyc_PP_Doc*
_temp1557= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1558= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1559= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1460)); struct Cyc_PP_Doc* _temp1560= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1561= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1554[ 6u]={ _temp1556, _temp1557, _temp1558, _temp1559,
_temp1560, _temp1561}; struct _tagged_arr _temp1555={( void*) _temp1554,( void*)
_temp1554,( void*)( _temp1554 + 6u)}; _temp1553( _temp1555);});} else{ s=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,
_tag_arr("", sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1562=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1562->hd=(
void*) _temp1462; _temp1562->tl=({ struct Cyc_List_List* _temp1563=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1563->hd=( void*) _temp1460;
_temp1563->tl= 0; _temp1563;}); _temp1562;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1564=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1564->hd=( void*) _temp1462;
_temp1564->tl=({ struct Cyc_List_List* _temp1565=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1565->hd=( void*) _temp1460;
_temp1565->tl= 0; _temp1565;}); _temp1564;}));} goto _LL1415; _LL1423: if(
_temp1464 == 0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc*(* _temp1566)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1569= Cyc_PP_text( _tag_arr("return ", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1570= _temp1464 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1464)); struct
Cyc_PP_Doc* _temp1571= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1567[ 3u]={ _temp1569, _temp1570, _temp1571}; struct
_tagged_arr _temp1568={( void*) _temp1567,( void*) _temp1567,( void*)( _temp1567
+ 3u)}; _temp1566( _temp1568);});} goto _LL1415; _LL1425: { int print_else;{
void* _temp1572=( void*) _temp1466->r; _LL1574: if( _temp1572 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1575;} else{ goto _LL1576;} _LL1576: goto _LL1577; _LL1575: print_else=
0; goto _LL1573; _LL1577: print_else= 1; goto _LL1573; _LL1573:;} s=({ struct
Cyc_PP_Doc*(* _temp1578)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1581= Cyc_PP_text( _tag_arr("if (", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1582= Cyc_Absynpp_exp2doc( _temp1470); struct Cyc_PP_Doc*
_temp1583= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1584= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1601)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1604= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1605= Cyc_Absynpp_stmt2doc( _temp1468); struct Cyc_PP_Doc* _temp1602[ 2u]={
_temp1604, _temp1605}; struct _tagged_arr _temp1603={( void*) _temp1602,( void*)
_temp1602,( void*)( _temp1602 + 2u)}; _temp1601( _temp1603);})); struct Cyc_PP_Doc*
_temp1585= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1586= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1587=
print_else?({ struct Cyc_PP_Doc*(* _temp1588)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1591= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1592=
Cyc_PP_text( _tag_arr("else {", sizeof( unsigned char), 7u)); struct Cyc_PP_Doc*
_temp1593= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1596)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1599= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1600= Cyc_Absynpp_stmt2doc( _temp1466); struct Cyc_PP_Doc* _temp1597[ 2u]={
_temp1599, _temp1600}; struct _tagged_arr _temp1598={( void*) _temp1597,( void*)
_temp1597,( void*)( _temp1597 + 2u)}; _temp1596( _temp1598);})); struct Cyc_PP_Doc*
_temp1594= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1595= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1589[ 5u]={
_temp1591, _temp1592, _temp1593, _temp1594, _temp1595}; struct _tagged_arr
_temp1590={( void*) _temp1589,( void*) _temp1589,( void*)( _temp1589 + 5u)};
_temp1588( _temp1590);}): Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp1579[ 7u]={
_temp1581, _temp1582, _temp1583, _temp1584, _temp1585, _temp1586, _temp1587};
struct _tagged_arr _temp1580={( void*) _temp1579,( void*) _temp1579,( void*)(
_temp1579 + 7u)}; _temp1578( _temp1580);}); goto _LL1415;} _LL1427: s=({ struct
Cyc_PP_Doc*(* _temp1606)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1609= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp1610= Cyc_Absynpp_exp2doc( _temp1476); struct Cyc_PP_Doc*
_temp1611= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1612= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1615)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1618= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1619= Cyc_Absynpp_stmt2doc( _temp1472); struct Cyc_PP_Doc* _temp1616[ 2u]={
_temp1618, _temp1619}; struct _tagged_arr _temp1617={( void*) _temp1616,( void*)
_temp1616,( void*)( _temp1616 + 2u)}; _temp1615( _temp1617);})); struct Cyc_PP_Doc*
_temp1613= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1614= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1607[ 6u]={
_temp1609, _temp1610, _temp1611, _temp1612, _temp1613, _temp1614}; struct
_tagged_arr _temp1608={( void*) _temp1607,( void*) _temp1607,( void*)( _temp1607
+ 6u)}; _temp1606( _temp1608);}); goto _LL1415; _LL1429: s= Cyc_PP_text(
_tag_arr("break;", sizeof( unsigned char), 7u)); goto _LL1415; _LL1431: s= Cyc_PP_text(
_tag_arr("continue;", sizeof( unsigned char), 10u)); goto _LL1415; _LL1433: s=
Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr(* _temp1620)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1624= _tag_arr("goto %s;", sizeof( unsigned char), 9u); struct Cyc_Stdio_String_pa_struct
_temp1625; _temp1625.tag= Cyc_Stdio_String_pa; _temp1625.f1=( struct _tagged_arr)*
_temp1478;{ void* _temp1623=( void*)& _temp1625; void* _temp1621[ 1u]={
_temp1623}; struct _tagged_arr _temp1622={( void*) _temp1621,( void*) _temp1621,(
void*)( _temp1621 + 1u)}; _temp1620( _temp1624, _temp1622);}})); goto _LL1415;
_LL1435: s=({ struct Cyc_PP_Doc*(* _temp1626)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1629= Cyc_PP_text( _tag_arr("for(", sizeof(
unsigned char), 5u)); struct Cyc_PP_Doc* _temp1630= Cyc_Absynpp_exp2doc(
_temp1490); struct Cyc_PP_Doc* _temp1631= Cyc_PP_text( _tag_arr("; ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1632= Cyc_Absynpp_exp2doc(
_temp1488); struct Cyc_PP_Doc* _temp1633= Cyc_PP_text( _tag_arr("; ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1634= Cyc_Absynpp_exp2doc(
_temp1484); struct Cyc_PP_Doc* _temp1635= Cyc_PP_text( _tag_arr(") {", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1636= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp1639)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1642= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1643= Cyc_Absynpp_stmt2doc( _temp1480); struct Cyc_PP_Doc*
_temp1640[ 2u]={ _temp1642, _temp1643}; struct _tagged_arr _temp1641={( void*)
_temp1640,( void*) _temp1640,( void*)( _temp1640 + 2u)}; _temp1639( _temp1641);}));
struct Cyc_PP_Doc* _temp1637= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1638=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1627[ 10u]={ _temp1629, _temp1630, _temp1631, _temp1632, _temp1633,
_temp1634, _temp1635, _temp1636, _temp1637, _temp1638}; struct _tagged_arr
_temp1628={( void*) _temp1627,( void*) _temp1627,( void*)( _temp1627 + 10u)};
_temp1626( _temp1628);}); goto _LL1415; _LL1437: s=({ struct Cyc_PP_Doc*(*
_temp1644)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1647= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); struct Cyc_PP_Doc* _temp1648=
Cyc_Absynpp_exp2doc( _temp1494); struct Cyc_PP_Doc* _temp1649= Cyc_PP_text(
_tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1650= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1651= Cyc_Absynpp_switchclauses2doc( _temp1492); struct
Cyc_PP_Doc* _temp1652= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1653= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1645[ 7u]={
_temp1647, _temp1648, _temp1649, _temp1650, _temp1651, _temp1652, _temp1653};
struct _tagged_arr _temp1646={( void*) _temp1645,( void*) _temp1645,( void*)(
_temp1645 + 7u)}; _temp1644( _temp1646);}); goto _LL1415; _LL1439: s=({ struct
Cyc_PP_Doc*(* _temp1654)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1657= Cyc_PP_text( _tag_arr("switch \"C\" (", sizeof( unsigned char), 13u));
struct Cyc_PP_Doc* _temp1658= Cyc_Absynpp_exp2doc( _temp1498); struct Cyc_PP_Doc*
_temp1659= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1660= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1661= Cyc_Absynpp_switchCclauses2doc(
_temp1496); struct Cyc_PP_Doc* _temp1662= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1663= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1655[ 7u]={ _temp1657, _temp1658, _temp1659, _temp1660, _temp1661,
_temp1662, _temp1663}; struct _tagged_arr _temp1656={( void*) _temp1655,( void*)
_temp1655,( void*)( _temp1655 + 7u)}; _temp1654( _temp1656);}); goto _LL1415;
_LL1441: s= Cyc_PP_text( _tag_arr("fallthru;", sizeof( unsigned char), 10u));
goto _LL1415; _LL1443: s=({ struct Cyc_PP_Doc*(* _temp1664)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1667= Cyc_PP_text( _tag_arr("fallthru(",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1668= Cyc_Absynpp_exps2doc_prec(
20, _temp1502); struct Cyc_PP_Doc* _temp1669= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1665[ 3u]={ _temp1667,
_temp1668, _temp1669}; struct _tagged_arr _temp1666={( void*) _temp1665,( void*)
_temp1665,( void*)( _temp1665 + 3u)}; _temp1664( _temp1666);}); goto _LL1415;
_LL1445: s=({ struct Cyc_PP_Doc*(* _temp1670)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1673= Cyc_Absynpp_decl2doc( _temp1506); struct Cyc_PP_Doc*
_temp1674= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1675= Cyc_Absynpp_stmt2doc(
_temp1504); struct Cyc_PP_Doc* _temp1671[ 3u]={ _temp1673, _temp1674, _temp1675};
struct _tagged_arr _temp1672={( void*) _temp1671,( void*) _temp1671,( void*)(
_temp1671 + 3u)}; _temp1670( _temp1672);}); goto _LL1415; _LL1447: s=({ struct
Cyc_PP_Doc*(* _temp1676)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1679= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1680= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1508)); struct
Cyc_PP_Doc* _temp1677[ 2u]={ _temp1679, _temp1680}; struct _tagged_arr _temp1678={(
void*) _temp1677,( void*) _temp1677,( void*)( _temp1677 + 2u)}; _temp1676(
_temp1678);}); goto _LL1415; _LL1449: s=({ struct Cyc_PP_Doc*(* _temp1681)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1684= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1685=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1510)); struct Cyc_PP_Doc* _temp1682[
2u]={ _temp1684, _temp1685}; struct _tagged_arr _temp1683={( void*) _temp1682,(
void*) _temp1682,( void*)( _temp1682 + 2u)}; _temp1681( _temp1683);}); goto
_LL1415; _LL1451: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1512): 0){ s=({ struct Cyc_PP_Doc*(* _temp1686)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1689= Cyc_PP_textptr( _temp1514); struct Cyc_PP_Doc*
_temp1690= Cyc_PP_text( _tag_arr(": {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1691= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1692= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1512)); struct Cyc_PP_Doc* _temp1693= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1694= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1687[ 6u]={ _temp1689, _temp1690, _temp1691, _temp1692, _temp1693,
_temp1694}; struct _tagged_arr _temp1688={( void*) _temp1687,( void*) _temp1687,(
void*)( _temp1687 + 6u)}; _temp1686( _temp1688);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp1695)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1698= Cyc_PP_textptr(
_temp1514); struct Cyc_PP_Doc* _temp1699= Cyc_PP_text( _tag_arr(": ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1700= Cyc_Absynpp_stmt2doc(
_temp1512); struct Cyc_PP_Doc* _temp1696[ 3u]={ _temp1698, _temp1699, _temp1700};
struct _tagged_arr _temp1697={( void*) _temp1696,( void*) _temp1696,( void*)(
_temp1696 + 3u)}; _temp1695( _temp1697);});} goto _LL1415; _LL1453: s=({ struct
Cyc_PP_Doc*(* _temp1701)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1704= Cyc_PP_text( _tag_arr("do {", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1705= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1706= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1520)); struct Cyc_PP_Doc* _temp1707= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1708= Cyc_PP_text( _tag_arr("} while (", sizeof(
unsigned char), 10u)); struct Cyc_PP_Doc* _temp1709= Cyc_Absynpp_exp2doc(
_temp1518); struct Cyc_PP_Doc* _temp1710= Cyc_PP_text( _tag_arr(");", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1702[ 7u]={ _temp1704, _temp1705,
_temp1706, _temp1707, _temp1708, _temp1709, _temp1710}; struct _tagged_arr
_temp1703={( void*) _temp1702,( void*) _temp1702,( void*)( _temp1702 + 7u)};
_temp1701( _temp1703);}); goto _LL1415; _LL1455: s=({ struct Cyc_PP_Doc*(*
_temp1711)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1714= Cyc_PP_text(
_tag_arr("try {", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1715=
Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1716= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1524)); struct Cyc_PP_Doc* _temp1717= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1718= Cyc_PP_text( _tag_arr("} catch {", sizeof( unsigned char), 10u));
struct Cyc_PP_Doc* _temp1719= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1720=
Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc( _temp1522)); struct Cyc_PP_Doc*
_temp1721= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1722= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1712[ 9u]={
_temp1714, _temp1715, _temp1716, _temp1717, _temp1718, _temp1719, _temp1720,
_temp1721, _temp1722}; struct _tagged_arr _temp1713={( void*) _temp1712,( void*)
_temp1712,( void*)( _temp1712 + 9u)}; _temp1711( _temp1713);}); goto _LL1415;
_LL1457: s=({ struct Cyc_PP_Doc*(* _temp1723)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1726= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1727= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1530)); struct Cyc_PP_Doc* _temp1728= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1729= Cyc_Absynpp_qvar2doc(
_temp1528->name); struct Cyc_PP_Doc* _temp1730= Cyc_PP_text( _tag_arr("{",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1731= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1732= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1526));
struct Cyc_PP_Doc* _temp1733= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1734=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1724[ 9u]={ _temp1726, _temp1727, _temp1728, _temp1729, _temp1730,
_temp1731, _temp1732, _temp1733, _temp1734}; struct _tagged_arr _temp1725={(
void*) _temp1724,( void*) _temp1724,( void*)( _temp1724 + 9u)}; _temp1723(
_temp1725);}); goto _LL1415; _LL1415:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1735=( void*) p->r;
int _temp1767; void* _temp1769; unsigned char _temp1771; struct _tagged_arr
_temp1773; struct Cyc_Absyn_Vardecl* _temp1775; struct Cyc_List_List* _temp1777;
struct Cyc_Absyn_Pat* _temp1779; struct Cyc_Absyn_Vardecl* _temp1781; struct
_tuple0* _temp1783; struct Cyc_List_List* _temp1785; struct Cyc_List_List*
_temp1787; struct _tuple0* _temp1789; struct Cyc_List_List* _temp1791; struct
Cyc_List_List* _temp1793; struct _tuple0* _temp1795; struct Cyc_List_List*
_temp1797; struct Cyc_List_List* _temp1799; struct Cyc_Absyn_Structdecl*
_temp1801; struct Cyc_Absyn_Enumfield* _temp1803; struct Cyc_List_List*
_temp1805; struct Cyc_List_List* _temp1807; struct Cyc_Absyn_Tunionfield*
_temp1809; _LL1737: if( _temp1735 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1738;}
else{ goto _LL1739;} _LL1739: if( _temp1735 ==( void*) Cyc_Absyn_Null_p){ goto
_LL1740;} else{ goto _LL1741;} _LL1741: if(( unsigned int) _temp1735 > 2u?*((
int*) _temp1735) == Cyc_Absyn_Int_p: 0){ _LL1770: _temp1769=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1735)->f1; goto _LL1768; _LL1768: _temp1767=((
struct Cyc_Absyn_Int_p_struct*) _temp1735)->f2; goto _LL1742;} else{ goto
_LL1743;} _LL1743: if(( unsigned int) _temp1735 > 2u?*(( int*) _temp1735) == Cyc_Absyn_Char_p:
0){ _LL1772: _temp1771=(( struct Cyc_Absyn_Char_p_struct*) _temp1735)->f1; goto
_LL1744;} else{ goto _LL1745;} _LL1745: if(( unsigned int) _temp1735 > 2u?*((
int*) _temp1735) == Cyc_Absyn_Float_p: 0){ _LL1774: _temp1773=(( struct Cyc_Absyn_Float_p_struct*)
_temp1735)->f1; goto _LL1746;} else{ goto _LL1747;} _LL1747: if(( unsigned int)
_temp1735 > 2u?*(( int*) _temp1735) == Cyc_Absyn_Var_p: 0){ _LL1776: _temp1775=((
struct Cyc_Absyn_Var_p_struct*) _temp1735)->f1; goto _LL1748;} else{ goto
_LL1749;} _LL1749: if(( unsigned int) _temp1735 > 2u?*(( int*) _temp1735) == Cyc_Absyn_Tuple_p:
0){ _LL1778: _temp1777=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1735)->f1; goto
_LL1750;} else{ goto _LL1751;} _LL1751: if(( unsigned int) _temp1735 > 2u?*((
int*) _temp1735) == Cyc_Absyn_Pointer_p: 0){ _LL1780: _temp1779=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1735)->f1; goto _LL1752;} else{ goto _LL1753;} _LL1753: if(( unsigned int)
_temp1735 > 2u?*(( int*) _temp1735) == Cyc_Absyn_Reference_p: 0){ _LL1782:
_temp1781=(( struct Cyc_Absyn_Reference_p_struct*) _temp1735)->f1; goto _LL1754;}
else{ goto _LL1755;} _LL1755: if(( unsigned int) _temp1735 > 2u?*(( int*)
_temp1735) == Cyc_Absyn_UnknownId_p: 0){ _LL1784: _temp1783=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1735)->f1; goto _LL1756;} else{ goto _LL1757;} _LL1757: if(( unsigned int)
_temp1735 > 2u?*(( int*) _temp1735) == Cyc_Absyn_UnknownCall_p: 0){ _LL1790:
_temp1789=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1735)->f1; goto
_LL1788; _LL1788: _temp1787=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1735)->f2;
goto _LL1786; _LL1786: _temp1785=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1735)->f3; goto _LL1758;} else{ goto _LL1759;} _LL1759: if(( unsigned int)
_temp1735 > 2u?*(( int*) _temp1735) == Cyc_Absyn_UnknownFields_p: 0){ _LL1796:
_temp1795=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1735)->f1; goto
_LL1794; _LL1794: _temp1793=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1735)->f2; goto _LL1792; _LL1792: _temp1791=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1735)->f3; goto _LL1760;} else{ goto _LL1761;} _LL1761: if(( unsigned int)
_temp1735 > 2u?*(( int*) _temp1735) == Cyc_Absyn_Struct_p: 0){ _LL1802:
_temp1801=(( struct Cyc_Absyn_Struct_p_struct*) _temp1735)->f1; goto _LL1800;
_LL1800: _temp1799=(( struct Cyc_Absyn_Struct_p_struct*) _temp1735)->f3; goto
_LL1798; _LL1798: _temp1797=(( struct Cyc_Absyn_Struct_p_struct*) _temp1735)->f4;
goto _LL1762;} else{ goto _LL1763;} _LL1763: if(( unsigned int) _temp1735 > 2u?*((
int*) _temp1735) == Cyc_Absyn_Enum_p: 0){ _LL1804: _temp1803=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1735)->f2; goto _LL1764;} else{ goto _LL1765;} _LL1765: if(( unsigned int)
_temp1735 > 2u?*(( int*) _temp1735) == Cyc_Absyn_Tunion_p: 0){ _LL1810:
_temp1809=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1735)->f2; goto _LL1808;
_LL1808: _temp1807=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1735)->f3; goto
_LL1806; _LL1806: _temp1805=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1735)->f4;
goto _LL1766;} else{ goto _LL1736;} _LL1738: s= Cyc_PP_text( _tag_arr("_",
sizeof( unsigned char), 2u)); goto _LL1736; _LL1740: s= Cyc_PP_text( _tag_arr("null",
sizeof( unsigned char), 5u)); goto _LL1736; _LL1742: if( _temp1769 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr(*
_temp1811)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1815= _tag_arr("%d", sizeof( unsigned char), 3u); struct
Cyc_Stdio_Int_pa_struct _temp1816; _temp1816.tag= Cyc_Stdio_Int_pa; _temp1816.f1=(
int)(( unsigned int) _temp1767);{ void* _temp1814=( void*)& _temp1816; void*
_temp1812[ 1u]={ _temp1814}; struct _tagged_arr _temp1813={( void*) _temp1812,(
void*) _temp1812,( void*)( _temp1812 + 1u)}; _temp1811( _temp1815, _temp1813);}}));}
else{ s= Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr(* _temp1817)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp1821= _tag_arr("%u", sizeof( unsigned char), 3u); struct Cyc_Stdio_Int_pa_struct
_temp1822; _temp1822.tag= Cyc_Stdio_Int_pa; _temp1822.f1=( unsigned int)
_temp1767;{ void* _temp1820=( void*)& _temp1822; void* _temp1818[ 1u]={
_temp1820}; struct _tagged_arr _temp1819={( void*) _temp1818,( void*) _temp1818,(
void*)( _temp1818 + 1u)}; _temp1817( _temp1821, _temp1819);}}));} goto _LL1736;
_LL1744: s= Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr(* _temp1823)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp1827= _tag_arr("'%s'", sizeof( unsigned char), 5u); struct Cyc_Stdio_String_pa_struct
_temp1828; _temp1828.tag= Cyc_Stdio_String_pa; _temp1828.f1=( struct _tagged_arr)
Cyc_Absynpp_char_escape( _temp1771);{ void* _temp1826=( void*)& _temp1828; void*
_temp1824[ 1u]={ _temp1826}; struct _tagged_arr _temp1825={( void*) _temp1824,(
void*) _temp1824,( void*)( _temp1824 + 1u)}; _temp1823( _temp1827, _temp1825);}}));
goto _LL1736; _LL1746: s= Cyc_PP_text( _temp1773); goto _LL1736; _LL1748: s= Cyc_Absynpp_qvar2doc(
_temp1775->name); goto _LL1736; _LL1750: s=({ struct Cyc_PP_Doc*(* _temp1829)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1832= Cyc_PP_text(
_tag_arr("$(", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1833=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,
_tag_arr(",", sizeof( unsigned char), 2u), _temp1777); struct Cyc_PP_Doc*
_temp1834= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1830[ 3u]={ _temp1832, _temp1833, _temp1834}; struct _tagged_arr _temp1831={(
void*) _temp1830,( void*) _temp1830,( void*)( _temp1830 + 3u)}; _temp1829(
_temp1831);}); goto _LL1736; _LL1752: s=({ struct Cyc_PP_Doc*(* _temp1835)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1838= Cyc_PP_text(
_tag_arr("&", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1839= Cyc_Absynpp_pat2doc(
_temp1779); struct Cyc_PP_Doc* _temp1836[ 2u]={ _temp1838, _temp1839}; struct
_tagged_arr _temp1837={( void*) _temp1836,( void*) _temp1836,( void*)( _temp1836
+ 2u)}; _temp1835( _temp1837);}); goto _LL1736; _LL1754: s=({ struct Cyc_PP_Doc*(*
_temp1840)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1843= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1844= Cyc_Absynpp_qvar2doc(
_temp1781->name); struct Cyc_PP_Doc* _temp1841[ 2u]={ _temp1843, _temp1844};
struct _tagged_arr _temp1842={( void*) _temp1841,( void*) _temp1841,( void*)(
_temp1841 + 2u)}; _temp1840( _temp1842);}); goto _LL1736; _LL1756: s= Cyc_Absynpp_qvar2doc(
_temp1783); goto _LL1736; _LL1758: s=({ struct Cyc_PP_Doc*(* _temp1845)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1848= Cyc_Absynpp_qvar2doc(
_temp1789); struct Cyc_PP_Doc* _temp1849= Cyc_Absynpp_tvars2doc( _temp1787);
struct Cyc_PP_Doc* _temp1850= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char),
2u), _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1785)); struct Cyc_PP_Doc* _temp1846[ 3u]={ _temp1848, _temp1849, _temp1850};
struct _tagged_arr _temp1847={( void*) _temp1846,( void*) _temp1846,( void*)(
_temp1846 + 3u)}; _temp1845( _temp1847);}); goto _LL1736; _LL1760: s=({ struct
Cyc_PP_Doc*(* _temp1851)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1854= Cyc_Absynpp_qvar2doc( _temp1795); struct Cyc_PP_Doc* _temp1855= Cyc_Absynpp_tvars2doc(
_temp1793); struct Cyc_PP_Doc* _temp1856= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1791)); struct Cyc_PP_Doc* _temp1852[ 3u]={ _temp1854, _temp1855, _temp1856};
struct _tagged_arr _temp1853={( void*) _temp1852,( void*) _temp1852,( void*)(
_temp1852 + 3u)}; _temp1851( _temp1853);}); goto _LL1736; _LL1762: s=({ struct
Cyc_PP_Doc*(* _temp1857)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1860= _temp1801->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1801->name))->v); struct Cyc_PP_Doc*
_temp1861= Cyc_Absynpp_tvars2doc( _temp1799); struct Cyc_PP_Doc* _temp1862= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1797)); struct Cyc_PP_Doc* _temp1858[ 3u]={ _temp1860,
_temp1861, _temp1862}; struct _tagged_arr _temp1859={( void*) _temp1858,( void*)
_temp1858,( void*)( _temp1858 + 3u)}; _temp1857( _temp1859);}); goto _LL1736;
_LL1764: s= Cyc_Absynpp_qvar2doc( _temp1803->name); goto _LL1736; _LL1766: if(
_temp1805 == 0){ s= Cyc_Absynpp_qvar2doc( _temp1809->name);} else{ s=({ struct
Cyc_PP_Doc*(* _temp1863)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1866= Cyc_Absynpp_qvar2doc( _temp1809->name); struct Cyc_PP_Doc* _temp1867=
Cyc_Absynpp_tvars2doc( _temp1807); struct Cyc_PP_Doc* _temp1868= Cyc_PP_egroup(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1805)); struct Cyc_PP_Doc* _temp1864[ 3u]={ _temp1866,
_temp1867, _temp1868}; struct _tagged_arr _temp1865={( void*) _temp1864,( void*)
_temp1864,( void*)( _temp1864 + 3u)}; _temp1863( _temp1865);});} goto _LL1736;
_LL1736:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc*(* _temp1869)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1872= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1));
struct Cyc_PP_Doc* _temp1873= Cyc_Absynpp_pat2doc((* dp).f2); struct Cyc_PP_Doc*
_temp1870[ 2u]={ _temp1872, _temp1873}; struct _tagged_arr _temp1871={( void*)
_temp1870,( void*) _temp1870,( void*)( _temp1870 + 2u)}; _temp1869( _temp1871);});}
struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause*
c){ if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p:
0){ return({ struct Cyc_PP_Doc*(* _temp1874)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1877= Cyc_PP_text( _tag_arr("default: ", sizeof(
unsigned char), 10u)); struct Cyc_PP_Doc* _temp1878= Cyc_PP_nest( 2,({ struct
Cyc_PP_Doc*(* _temp1879)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1882= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1883= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp1880[ 2u]={ _temp1882, _temp1883}; struct
_tagged_arr _temp1881={( void*) _temp1880,( void*) _temp1880,( void*)( _temp1880
+ 2u)}; _temp1879( _temp1881);})); struct Cyc_PP_Doc* _temp1875[ 2u]={ _temp1877,
_temp1878}; struct _tagged_arr _temp1876={( void*) _temp1875,( void*) _temp1875,(
void*)( _temp1875 + 2u)}; _temp1874( _temp1876);});} else{ if( c->where_clause
== 0){ return({ struct Cyc_PP_Doc*(* _temp1884)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1887= Cyc_PP_text( _tag_arr("case ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp1888= Cyc_Absynpp_pat2doc( c->pattern);
struct Cyc_PP_Doc* _temp1889= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp1890= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp1891)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1894= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1895= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp1892[ 2u]={ _temp1894, _temp1895}; struct _tagged_arr _temp1893={( void*)
_temp1892,( void*) _temp1892,( void*)( _temp1892 + 2u)}; _temp1891( _temp1893);}));
struct Cyc_PP_Doc* _temp1885[ 4u]={ _temp1887, _temp1888, _temp1889, _temp1890};
struct _tagged_arr _temp1886={( void*) _temp1885,( void*) _temp1885,( void*)(
_temp1885 + 4u)}; _temp1884( _temp1886);});} else{ return({ struct Cyc_PP_Doc*(*
_temp1896)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1899= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1900=
Cyc_Absynpp_pat2doc( c->pattern); struct Cyc_PP_Doc* _temp1901= Cyc_PP_text(
_tag_arr(" && ", sizeof( unsigned char), 5u)); struct Cyc_PP_Doc* _temp1902= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( c->where_clause)); struct Cyc_PP_Doc*
_temp1903= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp1904= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1905)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1908= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1909= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc* _temp1906[ 2u]={
_temp1908, _temp1909}; struct _tagged_arr _temp1907={( void*) _temp1906,( void*)
_temp1906,( void*)( _temp1906 + 2u)}; _temp1905( _temp1907);})); struct Cyc_PP_Doc*
_temp1897[ 6u]={ _temp1899, _temp1900, _temp1901, _temp1902, _temp1903,
_temp1904}; struct _tagged_arr _temp1898={( void*) _temp1897,( void*) _temp1897,(
void*)( _temp1897 + 6u)}; _temp1896( _temp1898);});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1912;
struct Cyc_Absyn_Stmt* _temp1913; struct Cyc_Absyn_Exp* _temp1915; struct Cyc_Absyn_SwitchC_clause*
_temp1910= c; _temp1912=* _temp1910; _LL1916: _temp1915= _temp1912.cnst_exp;
goto _LL1914; _LL1914: _temp1913= _temp1912.body; goto _LL1911; _LL1911: if(
_temp1915 == 0){ return({ struct Cyc_PP_Doc*(* _temp1917)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1920= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1921= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp1922)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1925= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1926= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp1923[ 2u]={ _temp1925, _temp1926}; struct
_tagged_arr _temp1924={( void*) _temp1923,( void*) _temp1923,( void*)( _temp1923
+ 2u)}; _temp1922( _temp1924);})); struct Cyc_PP_Doc* _temp1918[ 2u]={ _temp1920,
_temp1921}; struct _tagged_arr _temp1919={( void*) _temp1918,( void*) _temp1918,(
void*)( _temp1918 + 2u)}; _temp1917( _temp1919);});} else{ return({ struct Cyc_PP_Doc*(*
_temp1927)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1930= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1931=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1915)); struct
Cyc_PP_Doc* _temp1932= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
struct Cyc_PP_Doc* _temp1933= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1934)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1937= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1938= Cyc_Absynpp_stmt2doc( _temp1913); struct Cyc_PP_Doc*
_temp1935[ 2u]={ _temp1937, _temp1938}; struct _tagged_arr _temp1936={( void*)
_temp1935,( void*) _temp1935,( void*)( _temp1935 + 2u)}; _temp1934( _temp1936);}));
struct Cyc_PP_Doc* _temp1928[ 4u]={ _temp1930, _temp1931, _temp1932, _temp1933};
struct _tagged_arr _temp1929={( void*) _temp1928,( void*) _temp1928,( void*)(
_temp1928 + 4u)}; _temp1927( _temp1929);});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc*(* _temp1939)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1942= Cyc_Absynpp_qvar2doc( f->name); struct Cyc_PP_Doc*
_temp1943= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1944= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( f->tag));
struct Cyc_PP_Doc* _temp1940[ 3u]={ _temp1942, _temp1943, _temp1944}; struct
_tagged_arr _temp1941={( void*) _temp1940,( void*) _temp1940,( void*)( _temp1940
+ 3u)}; _temp1939( _temp1941);});}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List* fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Enumfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc, _tag_arr(",", sizeof(
unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc( struct
Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static struct Cyc_PP_Doc*
Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc, _tag_arr(",",
sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1945=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1971; struct Cyc_Absyn_Structdecl* _temp1973;
struct Cyc_Absyn_Uniondecl* _temp1975; struct Cyc_Absyn_Vardecl* _temp1977;
struct Cyc_Absyn_Vardecl _temp1979; struct Cyc_List_List* _temp1980; struct Cyc_Absyn_Exp*
_temp1982; void* _temp1984; struct Cyc_Absyn_Tqual _temp1986; struct _tuple0*
_temp1988; void* _temp1990; struct Cyc_Absyn_Tuniondecl* _temp1992; struct Cyc_Absyn_Tuniondecl
_temp1994; int _temp1995; struct Cyc_Core_Opt* _temp1997; struct Cyc_List_List*
_temp1999; struct _tuple0* _temp2001; void* _temp2003; struct Cyc_Absyn_Exp*
_temp2005; struct Cyc_Absyn_Pat* _temp2007; struct Cyc_List_List* _temp2009;
struct Cyc_Absyn_Enumdecl* _temp2011; struct Cyc_Absyn_Enumdecl _temp2013;
struct Cyc_Core_Opt* _temp2014; struct _tuple0* _temp2016; void* _temp2018;
struct Cyc_Absyn_Typedefdecl* _temp2020; struct Cyc_List_List* _temp2022; struct
_tagged_arr* _temp2024; struct Cyc_List_List* _temp2026; struct _tuple0*
_temp2028; struct Cyc_List_List* _temp2030; _LL1947: if(*(( int*) _temp1945) ==
Cyc_Absyn_Fn_d){ _LL1972: _temp1971=(( struct Cyc_Absyn_Fn_d_struct*) _temp1945)->f1;
goto _LL1948;} else{ goto _LL1949;} _LL1949: if(*(( int*) _temp1945) == Cyc_Absyn_Struct_d){
_LL1974: _temp1973=(( struct Cyc_Absyn_Struct_d_struct*) _temp1945)->f1; goto
_LL1950;} else{ goto _LL1951;} _LL1951: if(*(( int*) _temp1945) == Cyc_Absyn_Union_d){
_LL1976: _temp1975=(( struct Cyc_Absyn_Union_d_struct*) _temp1945)->f1; goto
_LL1952;} else{ goto _LL1953;} _LL1953: if(*(( int*) _temp1945) == Cyc_Absyn_Var_d){
_LL1978: _temp1977=(( struct Cyc_Absyn_Var_d_struct*) _temp1945)->f1; _temp1979=*
_temp1977; _LL1991: _temp1990=( void*) _temp1979.sc; goto _LL1989; _LL1989:
_temp1988= _temp1979.name; goto _LL1987; _LL1987: _temp1986= _temp1979.tq; goto
_LL1985; _LL1985: _temp1984=( void*) _temp1979.type; goto _LL1983; _LL1983:
_temp1982= _temp1979.initializer; goto _LL1981; _LL1981: _temp1980= _temp1979.attributes;
goto _LL1954;} else{ goto _LL1955;} _LL1955: if(*(( int*) _temp1945) == Cyc_Absyn_Tunion_d){
_LL1993: _temp1992=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1945)->f1;
_temp1994=* _temp1992; _LL2004: _temp2003=( void*) _temp1994.sc; goto _LL2002;
_LL2002: _temp2001= _temp1994.name; goto _LL2000; _LL2000: _temp1999= _temp1994.tvs;
goto _LL1998; _LL1998: _temp1997= _temp1994.fields; goto _LL1996; _LL1996:
_temp1995= _temp1994.is_xtunion; goto _LL1956;} else{ goto _LL1957;} _LL1957:
if(*(( int*) _temp1945) == Cyc_Absyn_Let_d){ _LL2008: _temp2007=(( struct Cyc_Absyn_Let_d_struct*)
_temp1945)->f1; goto _LL2006; _LL2006: _temp2005=(( struct Cyc_Absyn_Let_d_struct*)
_temp1945)->f4; goto _LL1958;} else{ goto _LL1959;} _LL1959: if(*(( int*)
_temp1945) == Cyc_Absyn_Letv_d){ _LL2010: _temp2009=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1945)->f1; goto _LL1960;} else{ goto _LL1961;} _LL1961: if(*(( int*)
_temp1945) == Cyc_Absyn_Enum_d){ _LL2012: _temp2011=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1945)->f1; _temp2013=* _temp2011; _LL2019: _temp2018=( void*) _temp2013.sc;
goto _LL2017; _LL2017: _temp2016= _temp2013.name; goto _LL2015; _LL2015:
_temp2014= _temp2013.fields; goto _LL1962;} else{ goto _LL1963;} _LL1963: if(*((
int*) _temp1945) == Cyc_Absyn_Typedef_d){ _LL2021: _temp2020=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1945)->f1; goto _LL1964;} else{ goto _LL1965;} _LL1965: if(*(( int*)
_temp1945) == Cyc_Absyn_Namespace_d){ _LL2025: _temp2024=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1945)->f1; goto _LL2023; _LL2023: _temp2022=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1945)->f2; goto _LL1966;} else{ goto _LL1967;} _LL1967: if(*(( int*)
_temp1945) == Cyc_Absyn_Using_d){ _LL2029: _temp2028=(( struct Cyc_Absyn_Using_d_struct*)
_temp1945)->f1; goto _LL2027; _LL2027: _temp2026=(( struct Cyc_Absyn_Using_d_struct*)
_temp1945)->f2; goto _LL1968;} else{ goto _LL1969;} _LL1969: if(*(( int*)
_temp1945) == Cyc_Absyn_ExternC_d){ _LL2031: _temp2030=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1945)->f1; goto _LL1970;} else{ goto _LL1946;} _LL1948: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2063=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2063[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2064; _temp2064.tag= Cyc_Absyn_FnType; _temp2064.f1=({ struct Cyc_Absyn_FnInfo
_temp2065; _temp2065.tvars= _temp1971->tvs; _temp2065.effect= _temp1971->effect;
_temp2065.ret_typ=( void*)(( void*) _temp1971->ret_type); _temp2065.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1971->args); _temp2065.c_varargs=
_temp1971->c_varargs; _temp2065.cyc_varargs= _temp1971->cyc_varargs; _temp2065.rgn_po=
_temp1971->rgn_po; _temp2065.attributes= 0; _temp2065;}); _temp2064;});
_temp2063;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1971->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1971->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1971->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1971->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1971->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2056; _temp2056.q_const= 0; _temp2056.q_volatile= 0; _temp2056.q_restrict=
0; _temp2056;}), t,({ struct Cyc_Core_Opt* _temp2057=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2057->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2058)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2061=
beforenamedoc; struct Cyc_PP_Doc* _temp2062= namedoc; struct Cyc_PP_Doc*
_temp2059[ 2u]={ _temp2061, _temp2062}; struct _tagged_arr _temp2060={( void*)
_temp2059,( void*) _temp2059,( void*)( _temp2059 + 2u)}; _temp2058( _temp2060);});
_temp2057;})); struct Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc*(* _temp2044)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2047= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2048= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc*(* _temp2051)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2054= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2055= Cyc_Absynpp_stmt2doc(
_temp1971->body); struct Cyc_PP_Doc* _temp2052[ 2u]={ _temp2054, _temp2055};
struct _tagged_arr _temp2053={( void*) _temp2052,( void*) _temp2052,( void*)(
_temp2052 + 2u)}; _temp2051( _temp2053);})); struct Cyc_PP_Doc* _temp2049= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2050= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2045[ 4u]={ _temp2047, _temp2048, _temp2049,
_temp2050}; struct _tagged_arr _temp2046={( void*) _temp2045,( void*) _temp2045,(
void*)( _temp2045 + 4u)}; _temp2044( _temp2046);}); s=({ struct Cyc_PP_Doc*(*
_temp2032)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2035=
inlinedoc; struct Cyc_PP_Doc* _temp2036= scopedoc; struct Cyc_PP_Doc* _temp2037=
tqtddoc; struct Cyc_PP_Doc* _temp2038= bodydoc; struct Cyc_PP_Doc* _temp2033[ 4u]={
_temp2035, _temp2036, _temp2037, _temp2038}; struct _tagged_arr _temp2034={(
void*) _temp2033,( void*) _temp2033,( void*)( _temp2033 + 4u)}; _temp2032(
_temp2034);}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*(* _temp2039)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2042= attsdoc; struct
Cyc_PP_Doc* _temp2043= s; struct Cyc_PP_Doc* _temp2040[ 2u]={ _temp2042,
_temp2043}; struct _tagged_arr _temp2041={( void*) _temp2040,( void*) _temp2040,(
void*)( _temp2040 + 2u)}; _temp2039( _temp2041);});} goto _LL1946;}} _LL1950:
if( _temp1973->fields == 0){ s=({ struct Cyc_PP_Doc*(* _temp2066)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2069= Cyc_Absynpp_scope2doc((
void*) _temp1973->sc); struct Cyc_PP_Doc* _temp2070= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2071= _temp1973->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1973->name))->v);
struct Cyc_PP_Doc* _temp2072= Cyc_Absynpp_ktvars2doc( _temp1973->tvs); struct
Cyc_PP_Doc* _temp2073= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp2067[ 5u]={ _temp2069, _temp2070, _temp2071, _temp2072,
_temp2073}; struct _tagged_arr _temp2068={( void*) _temp2067,( void*) _temp2067,(
void*)( _temp2067 + 5u)}; _temp2066( _temp2068);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp2074)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2077= Cyc_Absynpp_scope2doc((
void*) _temp1973->sc); struct Cyc_PP_Doc* _temp2078= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2079= _temp1973->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1973->name))->v);
struct Cyc_PP_Doc* _temp2080= Cyc_Absynpp_ktvars2doc( _temp1973->tvs); struct
Cyc_PP_Doc* _temp2081= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
struct Cyc_PP_Doc* _temp2082= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2087)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2090= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2091= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1973->fields))->v); struct Cyc_PP_Doc*
_temp2088[ 2u]={ _temp2090, _temp2091}; struct _tagged_arr _temp2089={( void*)
_temp2088,( void*) _temp2088,( void*)( _temp2088 + 2u)}; _temp2087( _temp2089);}));
struct Cyc_PP_Doc* _temp2083= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2084=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2085= Cyc_Absynpp_atts2doc( _temp1973->attributes); struct Cyc_PP_Doc*
_temp2086= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2075[ 10u]={ _temp2077, _temp2078, _temp2079, _temp2080, _temp2081,
_temp2082, _temp2083, _temp2084, _temp2085, _temp2086}; struct _tagged_arr
_temp2076={( void*) _temp2075,( void*) _temp2075,( void*)( _temp2075 + 10u)};
_temp2074( _temp2076);});} goto _LL1946; _LL1952: if( _temp1975->fields == 0){ s=({
struct Cyc_PP_Doc*(* _temp2092)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2095= Cyc_Absynpp_scope2doc(( void*) _temp1975->sc); struct Cyc_PP_Doc*
_temp2096= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp2097= _temp1975->name == 0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp1975->name))->v); struct Cyc_PP_Doc* _temp2098= Cyc_Absynpp_tvars2doc(
_temp1975->tvs); struct Cyc_PP_Doc* _temp2099= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2093[ 5u]={ _temp2095,
_temp2096, _temp2097, _temp2098, _temp2099}; struct _tagged_arr _temp2094={(
void*) _temp2093,( void*) _temp2093,( void*)( _temp2093 + 5u)}; _temp2092(
_temp2094);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2100)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2103= Cyc_Absynpp_scope2doc(( void*)
_temp1975->sc); struct Cyc_PP_Doc* _temp2104= Cyc_PP_text( _tag_arr("union ",
sizeof( unsigned char), 7u)); struct Cyc_PP_Doc* _temp2105= _temp1975->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1975->name))->v);
struct Cyc_PP_Doc* _temp2106= Cyc_Absynpp_tvars2doc( _temp1975->tvs); struct Cyc_PP_Doc*
_temp2107= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2108= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2113)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2116= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2117= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1975->fields))->v); struct Cyc_PP_Doc* _temp2114[ 2u]={
_temp2116, _temp2117}; struct _tagged_arr _temp2115={( void*) _temp2114,( void*)
_temp2114,( void*)( _temp2114 + 2u)}; _temp2113( _temp2115);})); struct Cyc_PP_Doc*
_temp2109= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2110= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2111= Cyc_Absynpp_atts2doc(
_temp1975->attributes); struct Cyc_PP_Doc* _temp2112= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2101[ 10u]={ _temp2103,
_temp2104, _temp2105, _temp2106, _temp2107, _temp2108, _temp2109, _temp2110,
_temp2111, _temp2112}; struct _tagged_arr _temp2102={( void*) _temp2101,( void*)
_temp2101,( void*)( _temp2101 + 10u)}; _temp2100( _temp2102);});} goto _LL1946;
_LL1954: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc( _temp1988);
struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1980); struct Cyc_PP_Doc*
beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;} else{ void*
_temp2118= Cyc_Tcutil_compress( _temp1984); struct Cyc_Absyn_FnInfo _temp2124;
struct Cyc_List_List* _temp2126; _LL2120: if(( unsigned int) _temp2118 > 4u?*((
int*) _temp2118) == Cyc_Absyn_FnType: 0){ _LL2125: _temp2124=(( struct Cyc_Absyn_FnType_struct*)
_temp2118)->f1; _LL2127: _temp2126= _temp2124.attributes; goto _LL2121;} else{
goto _LL2122;} _LL2122: goto _LL2123; _LL2121: beforenamedoc= Cyc_Absynpp_callconv2doc(
_temp2126); goto _LL2119; _LL2123: beforenamedoc= Cyc_PP_nil_doc(); goto _LL2119;
_LL2119:;} s=({ struct Cyc_PP_Doc*(* _temp2128)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2131= Cyc_Absynpp_to_VC? attsdoc: Cyc_PP_nil_doc();
struct Cyc_PP_Doc* _temp2132= Cyc_Absynpp_scope2doc( _temp1990); struct Cyc_PP_Doc*
_temp2133= Cyc_Absynpp_tqtd2doc( _temp1986, _temp1984,({ struct Cyc_Core_Opt*
_temp2141=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2141->v=( void*)({ struct Cyc_PP_Doc*(* _temp2142)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2145= beforenamedoc; struct Cyc_PP_Doc*
_temp2146= sn; struct Cyc_PP_Doc* _temp2143[ 2u]={ _temp2145, _temp2146}; struct
_tagged_arr _temp2144={( void*) _temp2143,( void*) _temp2143,( void*)( _temp2143
+ 2u)}; _temp2142( _temp2144);}); _temp2141;})); struct Cyc_PP_Doc* _temp2134=
_temp1982 == 0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):({ struct
Cyc_PP_Doc*(* _temp2136)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2139= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp2140= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1982));
struct Cyc_PP_Doc* _temp2137[ 2u]={ _temp2139, _temp2140}; struct _tagged_arr
_temp2138={( void*) _temp2137,( void*) _temp2137,( void*)( _temp2137 + 2u)};
_temp2136( _temp2138);}); struct Cyc_PP_Doc* _temp2135= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2129[ 5u]={ _temp2131,
_temp2132, _temp2133, _temp2134, _temp2135}; struct _tagged_arr _temp2130={(
void*) _temp2129,( void*) _temp2129,( void*)( _temp2129 + 5u)}; _temp2128(
_temp2130);}); goto _LL1946;} _LL1956: if( _temp1997 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp2147)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2150= Cyc_Absynpp_scope2doc(
_temp2003); struct Cyc_PP_Doc* _temp2151= _temp1995? Cyc_PP_text( _tag_arr("xtunion ",
sizeof( unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp2152= _temp1995? Cyc_Absynpp_qvar2doc(
_temp2001): Cyc_Absynpp_typedef_name2doc( _temp2001); struct Cyc_PP_Doc*
_temp2153= Cyc_Absynpp_ktvars2doc( _temp1999); struct Cyc_PP_Doc* _temp2154= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2148[ 5u]={
_temp2150, _temp2151, _temp2152, _temp2153, _temp2154}; struct _tagged_arr
_temp2149={( void*) _temp2148,( void*) _temp2148,( void*)( _temp2148 + 5u)};
_temp2147( _temp2149);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2155)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2158= Cyc_Absynpp_scope2doc(
_temp2003); struct Cyc_PP_Doc* _temp2159= _temp1995? Cyc_PP_text( _tag_arr("xtunion ",
sizeof( unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp2160= _temp1995? Cyc_Absynpp_qvar2doc(
_temp2001): Cyc_Absynpp_typedef_name2doc( _temp2001); struct Cyc_PP_Doc*
_temp2161= Cyc_Absynpp_ktvars2doc( _temp1999); struct Cyc_PP_Doc* _temp2162= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2163= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc*(* _temp2166)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2169= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2170= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1997))->v);
struct Cyc_PP_Doc* _temp2167[ 2u]={ _temp2169, _temp2170}; struct _tagged_arr
_temp2168={( void*) _temp2167,( void*) _temp2167,( void*)( _temp2167 + 2u)};
_temp2166( _temp2168);})); struct Cyc_PP_Doc* _temp2164= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2165= Cyc_PP_text( _tag_arr("};", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp2156[ 8u]={ _temp2158, _temp2159, _temp2160,
_temp2161, _temp2162, _temp2163, _temp2164, _temp2165}; struct _tagged_arr
_temp2157={( void*) _temp2156,( void*) _temp2156,( void*)( _temp2156 + 8u)};
_temp2155( _temp2157);});} goto _LL1946; _LL1958: s=({ struct Cyc_PP_Doc*(*
_temp2171)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2174= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); struct Cyc_PP_Doc* _temp2175= Cyc_Absynpp_pat2doc(
_temp2007); struct Cyc_PP_Doc* _temp2176= Cyc_PP_text( _tag_arr(" = ", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp2177= Cyc_Absynpp_exp2doc(
_temp2005); struct Cyc_PP_Doc* _temp2178= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2172[ 5u]={ _temp2174, _temp2175,
_temp2176, _temp2177, _temp2178}; struct _tagged_arr _temp2173={( void*)
_temp2172,( void*) _temp2172,( void*)( _temp2172 + 5u)}; _temp2171( _temp2173);});
goto _LL1946; _LL1960: s=({ struct Cyc_PP_Doc*(* _temp2179)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2182= Cyc_PP_text( _tag_arr("let ", sizeof(
unsigned char), 5u)); struct Cyc_PP_Doc* _temp2183= Cyc_Absynpp_ids2doc(
_temp2009); struct Cyc_PP_Doc* _temp2184= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2180[ 3u]={ _temp2182, _temp2183,
_temp2184}; struct _tagged_arr _temp2181={( void*) _temp2180,( void*) _temp2180,(
void*)( _temp2180 + 3u)}; _temp2179( _temp2181);}); goto _LL1946; _LL1962: if(
_temp2014 == 0){ s=({ struct Cyc_PP_Doc*(* _temp2185)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2188= Cyc_Absynpp_scope2doc( _temp2018); struct Cyc_PP_Doc*
_temp2189= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp2190= Cyc_Absynpp_typedef_name2doc( _temp2016); struct Cyc_PP_Doc*
_temp2191= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2186[ 4u]={ _temp2188, _temp2189, _temp2190, _temp2191}; struct _tagged_arr
_temp2187={( void*) _temp2186,( void*) _temp2186,( void*)( _temp2186 + 4u)};
_temp2185( _temp2187);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2192)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2195= Cyc_Absynpp_scope2doc(
_temp2018); struct Cyc_PP_Doc* _temp2196= Cyc_PP_text( _tag_arr("enum ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp2197= Cyc_Absynpp_qvar2doc(
_temp2016); struct Cyc_PP_Doc* _temp2198= Cyc_PP_text( _tag_arr(" {", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp2199= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2202)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2205= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2206= Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2014))->v); struct Cyc_PP_Doc* _temp2203[
2u]={ _temp2205, _temp2206}; struct _tagged_arr _temp2204={( void*) _temp2203,(
void*) _temp2203,( void*)( _temp2203 + 2u)}; _temp2202( _temp2204);})); struct
Cyc_PP_Doc* _temp2200= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2201= Cyc_PP_text(
_tag_arr("};", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2193[ 7u]={
_temp2195, _temp2196, _temp2197, _temp2198, _temp2199, _temp2200, _temp2201};
struct _tagged_arr _temp2194={( void*) _temp2193,( void*) _temp2193,( void*)(
_temp2193 + 7u)}; _temp2192( _temp2194);});} goto _LL1946; _LL1964: s=({ struct
Cyc_PP_Doc*(* _temp2207)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2210= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
struct Cyc_PP_Doc* _temp2211= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2213; _temp2213.q_const= 0; _temp2213.q_volatile= 0; _temp2213.q_restrict=
0; _temp2213;}),( void*) _temp2020->defn,({ struct Cyc_Core_Opt* _temp2214=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2214->v=(
void*)({ struct Cyc_PP_Doc*(* _temp2215)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2218= Cyc_Absynpp_typedef_name2doc( _temp2020->name);
struct Cyc_PP_Doc* _temp2219= Cyc_Absynpp_tvars2doc( _temp2020->tvs); struct Cyc_PP_Doc*
_temp2216[ 2u]={ _temp2218, _temp2219}; struct _tagged_arr _temp2217={( void*)
_temp2216,( void*) _temp2216,( void*)( _temp2216 + 2u)}; _temp2215( _temp2217);});
_temp2214;})); struct Cyc_PP_Doc* _temp2212= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2208[ 3u]={ _temp2210, _temp2211,
_temp2212}; struct _tagged_arr _temp2209={( void*) _temp2208,( void*) _temp2208,(
void*)( _temp2208 + 3u)}; _temp2207( _temp2209);}); goto _LL1946; _LL1966: if(
Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add( _temp2024);} s=({
struct Cyc_PP_Doc*(* _temp2220)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2223= Cyc_PP_text( _tag_arr("namespace ", sizeof( unsigned char), 11u));
struct Cyc_PP_Doc* _temp2224= Cyc_PP_textptr( _temp2024); struct Cyc_PP_Doc*
_temp2225= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2226= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2227=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp2022); struct Cyc_PP_Doc* _temp2228= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2229= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2221[ 7u]={ _temp2223, _temp2224, _temp2225,
_temp2226, _temp2227, _temp2228, _temp2229}; struct _tagged_arr _temp2222={(
void*) _temp2221,( void*) _temp2221,( void*)( _temp2221 + 7u)}; _temp2220(
_temp2222);}); if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL1946; _LL1968: if( Cyc_Absynpp_print_using_stmts){ s=({ struct Cyc_PP_Doc*(*
_temp2230)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2233= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); struct Cyc_PP_Doc* _temp2234=
Cyc_Absynpp_qvar2doc( _temp2028); struct Cyc_PP_Doc* _temp2235= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2236= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2237=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc, _tag_arr("", sizeof( unsigned char), 1u), _temp2026);
struct Cyc_PP_Doc* _temp2238= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2239=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2231[ 7u]={ _temp2233, _temp2234, _temp2235, _temp2236, _temp2237,
_temp2238, _temp2239}; struct _tagged_arr _temp2232={( void*) _temp2231,( void*)
_temp2231,( void*)( _temp2231 + 7u)}; _temp2230( _temp2232);});} else{ s=({
struct Cyc_PP_Doc*(* _temp2240)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2243= Cyc_PP_text( _tag_arr("/* using ", sizeof( unsigned char), 10u));
struct Cyc_PP_Doc* _temp2244= Cyc_Absynpp_qvar2doc( _temp2028); struct Cyc_PP_Doc*
_temp2245= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp2246= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2247=(( struct
Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp2026); struct Cyc_PP_Doc*
_temp2248= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2249= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2241[
7u]={ _temp2243, _temp2244, _temp2245, _temp2246, _temp2247, _temp2248,
_temp2249}; struct _tagged_arr _temp2242={( void*) _temp2241,( void*) _temp2241,(
void*)( _temp2241 + 7u)}; _temp2240( _temp2242);});} goto _LL1946; _LL1970: if(
Cyc_Absynpp_print_externC_stmts){ s=({ struct Cyc_PP_Doc*(* _temp2250)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2253= Cyc_PP_text( _tag_arr("extern \"C\" {",
sizeof( unsigned char), 13u)); struct Cyc_PP_Doc* _temp2254= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2255=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc, _tag_arr("", sizeof( unsigned char), 1u), _temp2030);
struct Cyc_PP_Doc* _temp2256= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2257=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2251[ 5u]={ _temp2253, _temp2254, _temp2255, _temp2256, _temp2257}; struct
_tagged_arr _temp2252={( void*) _temp2251,( void*) _temp2251,( void*)( _temp2251
+ 5u)}; _temp2250( _temp2252);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2258)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2261= Cyc_PP_text(
_tag_arr("/* extern \"C\" { */", sizeof( unsigned char), 19u)); struct Cyc_PP_Doc*
_temp2262= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2263=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp2030); struct Cyc_PP_Doc* _temp2264= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2265= Cyc_PP_text( _tag_arr("/* } */", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp2259[ 5u]={ _temp2261, _temp2262,
_temp2263, _temp2264, _temp2265}; struct _tagged_arr _temp2260={( void*)
_temp2259,( void*) _temp2259,( void*)( _temp2259 + 5u)}; _temp2258( _temp2260);});}
goto _LL1946; _LL1946:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp2266= sc; _LL2268: if( _temp2266 ==( void*) Cyc_Absyn_Static){
goto _LL2269;} else{ goto _LL2270;} _LL2270: if( _temp2266 ==( void*) Cyc_Absyn_Public){
goto _LL2271;} else{ goto _LL2272;} _LL2272: if( _temp2266 ==( void*) Cyc_Absyn_Extern){
goto _LL2273;} else{ goto _LL2274;} _LL2274: if( _temp2266 ==( void*) Cyc_Absyn_ExternC){
goto _LL2275;} else{ goto _LL2276;} _LL2276: if( _temp2266 ==( void*) Cyc_Absyn_Abstract){
goto _LL2277;} else{ goto _LL2267;} _LL2269: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL2271: return Cyc_PP_nil_doc(); _LL2273: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL2275: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL2277:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL2267:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp2278= t; struct
Cyc_Absyn_Tvar* _temp2286; struct Cyc_List_List* _temp2288; _LL2280: if((
unsigned int) _temp2278 > 4u?*(( int*) _temp2278) == Cyc_Absyn_VarType: 0){
_LL2287: _temp2286=(( struct Cyc_Absyn_VarType_struct*) _temp2278)->f1; goto
_LL2281;} else{ goto _LL2282;} _LL2282: if(( unsigned int) _temp2278 > 4u?*((
int*) _temp2278) == Cyc_Absyn_JoinEff: 0){ _LL2289: _temp2288=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2278)->f1; goto _LL2283;} else{ goto _LL2284;} _LL2284: goto _LL2285;
_LL2281: return Cyc_Tcutil_is_temp_tvar( _temp2286); _LL2283: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2288); _LL2285: return 0; _LL2279:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp2290= t; struct Cyc_Absyn_Exp* _temp2304; struct Cyc_Absyn_Tqual _temp2306;
void* _temp2308; struct Cyc_Absyn_PtrInfo _temp2310; struct Cyc_Absyn_Conref*
_temp2312; struct Cyc_Absyn_Tqual _temp2314; struct Cyc_Absyn_Conref* _temp2316;
void* _temp2318; void* _temp2320; struct Cyc_Absyn_FnInfo _temp2322; struct Cyc_List_List*
_temp2324; struct Cyc_List_List* _temp2326; struct Cyc_Absyn_VarargInfo*
_temp2328; int _temp2330; struct Cyc_List_List* _temp2332; void* _temp2334;
struct Cyc_Core_Opt* _temp2336; struct Cyc_List_List* _temp2338; int _temp2340;
struct Cyc_Core_Opt* _temp2342; struct Cyc_Core_Opt* _temp2344; struct Cyc_Core_Opt*
_temp2346; struct Cyc_List_List* _temp2348; struct _tuple0* _temp2350; _LL2292:
if(( unsigned int) _temp2290 > 4u?*(( int*) _temp2290) == Cyc_Absyn_ArrayType: 0){
_LL2309: _temp2308=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2290)->f1;
goto _LL2307; _LL2307: _temp2306=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2290)->f2; goto _LL2305; _LL2305: _temp2304=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2290)->f3; goto _LL2293;} else{ goto _LL2294;} _LL2294: if(( unsigned int)
_temp2290 > 4u?*(( int*) _temp2290) == Cyc_Absyn_PointerType: 0){ _LL2311:
_temp2310=(( struct Cyc_Absyn_PointerType_struct*) _temp2290)->f1; _LL2321:
_temp2320=( void*) _temp2310.elt_typ; goto _LL2319; _LL2319: _temp2318=( void*)
_temp2310.rgn_typ; goto _LL2317; _LL2317: _temp2316= _temp2310.nullable; goto
_LL2315; _LL2315: _temp2314= _temp2310.tq; goto _LL2313; _LL2313: _temp2312=
_temp2310.bounds; goto _LL2295;} else{ goto _LL2296;} _LL2296: if(( unsigned int)
_temp2290 > 4u?*(( int*) _temp2290) == Cyc_Absyn_FnType: 0){ _LL2323: _temp2322=((
struct Cyc_Absyn_FnType_struct*) _temp2290)->f1; _LL2339: _temp2338= _temp2322.tvars;
goto _LL2337; _LL2337: _temp2336= _temp2322.effect; goto _LL2335; _LL2335:
_temp2334=( void*) _temp2322.ret_typ; goto _LL2333; _LL2333: _temp2332=
_temp2322.args; goto _LL2331; _LL2331: _temp2330= _temp2322.c_varargs; goto
_LL2329; _LL2329: _temp2328= _temp2322.cyc_varargs; goto _LL2327; _LL2327:
_temp2326= _temp2322.rgn_po; goto _LL2325; _LL2325: _temp2324= _temp2322.attributes;
goto _LL2297;} else{ goto _LL2298;} _LL2298: if(( unsigned int) _temp2290 > 4u?*((
int*) _temp2290) == Cyc_Absyn_Evar: 0){ _LL2345: _temp2344=(( struct Cyc_Absyn_Evar_struct*)
_temp2290)->f1; goto _LL2343; _LL2343: _temp2342=(( struct Cyc_Absyn_Evar_struct*)
_temp2290)->f2; goto _LL2341; _LL2341: _temp2340=(( struct Cyc_Absyn_Evar_struct*)
_temp2290)->f3; goto _LL2299;} else{ goto _LL2300;} _LL2300: if(( unsigned int)
_temp2290 > 4u?*(( int*) _temp2290) == Cyc_Absyn_TypedefType: 0){ _LL2351:
_temp2350=(( struct Cyc_Absyn_TypedefType_struct*) _temp2290)->f1; goto _LL2349;
_LL2349: _temp2348=(( struct Cyc_Absyn_TypedefType_struct*) _temp2290)->f2; goto
_LL2347; _LL2347: _temp2346=(( struct Cyc_Absyn_TypedefType_struct*) _temp2290)->f3;
goto _LL2301;} else{ goto _LL2302;} _LL2302: goto _LL2303; _LL2293: { struct Cyc_List_List*
_temp2354; void* _temp2356; struct Cyc_Absyn_Tqual _temp2358; struct _tuple4
_temp2352= Cyc_Absynpp_to_tms( _temp2306, _temp2308); _LL2359: _temp2358=
_temp2352.f1; goto _LL2357; _LL2357: _temp2356= _temp2352.f2; goto _LL2355;
_LL2355: _temp2354= _temp2352.f3; goto _LL2353; _LL2353: { void* tm; if(
_temp2304 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp2360=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2360[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2361; _temp2361.tag= Cyc_Absyn_ConstArray_mod;
_temp2361.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp2304); _temp2361;});
_temp2360;});} return({ struct _tuple4 _temp2362; _temp2362.f1= _temp2358;
_temp2362.f2= _temp2356; _temp2362.f3=({ struct Cyc_List_List* _temp2363=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2363->hd=(
void*) tm; _temp2363->tl= _temp2354; _temp2363;}); _temp2362;});}} _LL2295: {
struct Cyc_List_List* _temp2366; void* _temp2368; struct Cyc_Absyn_Tqual
_temp2370; struct _tuple4 _temp2364= Cyc_Absynpp_to_tms( _temp2314, _temp2320);
_LL2371: _temp2370= _temp2364.f1; goto _LL2369; _LL2369: _temp2368= _temp2364.f2;
goto _LL2367; _LL2367: _temp2366= _temp2364.f3; goto _LL2365; _LL2365: { void*
ps;{ struct _tuple8 _temp2373=({ struct _tuple8 _temp2372; _temp2372.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2316))->v; _temp2372.f2=( void*)( Cyc_Absyn_compress_conref( _temp2312))->v;
_temp2372;}); void* _temp2383; void* _temp2385; struct Cyc_Absyn_Exp* _temp2387;
void* _temp2389; int _temp2391; void* _temp2393; void* _temp2395; struct Cyc_Absyn_Exp*
_temp2397; void* _temp2399; int _temp2401; void* _temp2403; void* _temp2405;
_LL2375: _LL2390: _temp2389= _temp2373.f1; if(( unsigned int) _temp2389 > 1u?*((
int*) _temp2389) == Cyc_Absyn_Eq_constr: 0){ _LL2392: _temp2391=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2389)->f1; if( _temp2391 == 1){ goto _LL2384;}
else{ goto _LL2377;}} else{ goto _LL2377;} _LL2384: _temp2383= _temp2373.f2; if((
unsigned int) _temp2383 > 1u?*(( int*) _temp2383) == Cyc_Absyn_Eq_constr: 0){
_LL2386: _temp2385=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2383)->f1;
if(( unsigned int) _temp2385 > 1u?*(( int*) _temp2385) == Cyc_Absyn_Upper_b: 0){
_LL2388: _temp2387=(( struct Cyc_Absyn_Upper_b_struct*) _temp2385)->f1; goto
_LL2376;} else{ goto _LL2377;}} else{ goto _LL2377;} _LL2377: _LL2400: _temp2399=
_temp2373.f1; if(( unsigned int) _temp2399 > 1u?*(( int*) _temp2399) == Cyc_Absyn_Eq_constr:
0){ _LL2402: _temp2401=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2399)->f1;
if( _temp2401 == 0){ goto _LL2394;} else{ goto _LL2379;}} else{ goto _LL2379;}
_LL2394: _temp2393= _temp2373.f2; if(( unsigned int) _temp2393 > 1u?*(( int*)
_temp2393) == Cyc_Absyn_Eq_constr: 0){ _LL2396: _temp2395=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2393)->f1; if(( unsigned int) _temp2395 > 1u?*(( int*) _temp2395) == Cyc_Absyn_Upper_b:
0){ _LL2398: _temp2397=(( struct Cyc_Absyn_Upper_b_struct*) _temp2395)->f1; goto
_LL2378;} else{ goto _LL2379;}} else{ goto _LL2379;} _LL2379: _LL2404: _temp2403=
_temp2373.f2; if(( unsigned int) _temp2403 > 1u?*(( int*) _temp2403) == Cyc_Absyn_Eq_constr:
0){ _LL2406: _temp2405=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2403)->f1;
if( _temp2405 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2380;} else{ goto _LL2381;}}
else{ goto _LL2381;} _LL2381: goto _LL2382; _LL2376: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2407=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2407[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2408; _temp2408.tag=
Cyc_Absyn_Nullable_ps; _temp2408.f1= _temp2387; _temp2408;}); _temp2407;}); goto
_LL2374; _LL2378: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2409=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2409[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2410; _temp2410.tag= Cyc_Absyn_NonNullable_ps; _temp2410.f1= _temp2397;
_temp2410;}); _temp2409;}); goto _LL2374; _LL2380: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2374; _LL2382: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2374;
_LL2374:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2413=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2413[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2414; _temp2414.tag=
Cyc_Absyn_Pointer_mod; _temp2414.f1=( void*) ps; _temp2414.f2=( void*) _temp2318;
_temp2414.f3= tq; _temp2414;}); _temp2413;}); return({ struct _tuple4 _temp2411;
_temp2411.f1= _temp2370; _temp2411.f2= _temp2368; _temp2411.f3=({ struct Cyc_List_List*
_temp2412=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2412->hd=( void*) tm; _temp2412->tl= _temp2366; _temp2412;}); _temp2411;});}}}
_LL2297: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp2336 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp2336))->v)){ _temp2336= 0;
_temp2338= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_rewrite_temp_tvar,
_temp2338);}}{ struct Cyc_List_List* _temp2417; void* _temp2419; struct Cyc_Absyn_Tqual
_temp2421; struct _tuple4 _temp2415= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp2334); _LL2422: _temp2421= _temp2415.f1; goto _LL2420; _LL2420: _temp2419=
_temp2415.f2; goto _LL2418; _LL2418: _temp2417= _temp2415.f3; goto _LL2416;
_LL2416: { struct Cyc_List_List* tms= _temp2417; if( _temp2324 != 0? ! Cyc_Absynpp_to_VC:
0){ tms=({ struct Cyc_List_List* _temp2423=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2423->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp2424=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp2424[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp2425; _temp2425.tag= Cyc_Absyn_Attributes_mod; _temp2425.f1= 0; _temp2425.f2=
_temp2324; _temp2425;}); _temp2424;})); _temp2423->tl= tms; _temp2423;});} tms=({
struct Cyc_List_List* _temp2426=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2426->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2427=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2427[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2428; _temp2428.tag=
Cyc_Absyn_Function_mod; _temp2428.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2429=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2429[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2430; _temp2430.tag= Cyc_Absyn_WithTypes;
_temp2430.f1= _temp2332; _temp2430.f2= _temp2330; _temp2430.f3= _temp2328;
_temp2430.f4= _temp2336; _temp2430.f5= _temp2326; _temp2430;}); _temp2429;}));
_temp2428;}); _temp2427;})); _temp2426->tl= tms; _temp2426;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp2324 != 0; _temp2324=(( struct Cyc_List_List*) _check_null(
_temp2324))->tl){ void* _temp2431=( void*)(( struct Cyc_List_List*) _check_null(
_temp2324))->hd; _LL2433: if( _temp2431 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL2434;} else{ goto _LL2435;} _LL2435: if( _temp2431 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL2436;} else{ goto _LL2437;} _LL2437: if( _temp2431 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL2438;} else{ goto _LL2439;} _LL2439: goto _LL2440; _LL2434: goto _LL2436;
_LL2436: goto _LL2438; _LL2438: tms=({ struct Cyc_List_List* _temp2441=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2441->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp2442=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2442[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2443; _temp2443.tag= Cyc_Absyn_Attributes_mod;
_temp2443.f1= 0; _temp2443.f2=({ struct Cyc_List_List* _temp2444=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2444->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp2324))->hd); _temp2444->tl= 0;
_temp2444;}); _temp2443;}); _temp2442;})); _temp2441->tl= tms; _temp2441;});
goto AfterAtts; _LL2440: goto _LL2432; _LL2432:;}} AfterAtts: if( _temp2338 != 0){
tms=({ struct Cyc_List_List* _temp2445=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2445->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2446=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2446[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2447; _temp2447.tag= Cyc_Absyn_TypeParams_mod; _temp2447.f1= _temp2338;
_temp2447.f2= 0; _temp2447.f3= 1; _temp2447;}); _temp2446;})); _temp2445->tl=
tms; _temp2445;});} return({ struct _tuple4 _temp2448; _temp2448.f1= _temp2421;
_temp2448.f2= _temp2419; _temp2448.f3= tms; _temp2448;});}} _LL2299: if(
_temp2342 == 0){ return({ struct _tuple4 _temp2449; _temp2449.f1= tq; _temp2449.f2=
t; _temp2449.f3= 0; _temp2449;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2342))->v);} _LL2301: if( _temp2346 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp2450; _temp2450.f1=
tq; _temp2450.f2= t; _temp2450.f3= 0; _temp2450;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2346))->v);} _LL2303:
return({ struct _tuple4 _temp2451; _temp2451.f1= tq; _temp2451.f2= t; _temp2451.f3=
0; _temp2451;}); _LL2291:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp2454; void*
_temp2456; struct Cyc_Absyn_Tqual _temp2458; struct _tuple4 _temp2452= Cyc_Absynpp_to_tms(
tq, t); _LL2459: _temp2458= _temp2452.f1; goto _LL2457; _LL2457: _temp2456=
_temp2452.f2; goto _LL2455; _LL2455: _temp2454= _temp2452.f3; goto _LL2453;
_LL2453: _temp2454= Cyc_List_imp_rev( _temp2454); if( _temp2454 == 0? dopt == 0:
0){ return({ struct Cyc_PP_Doc*(* _temp2460)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2463= Cyc_Absynpp_tqual2doc( _temp2458); struct Cyc_PP_Doc*
_temp2464= Cyc_Absynpp_ntyp2doc( _temp2456); struct Cyc_PP_Doc* _temp2461[ 2u]={
_temp2463, _temp2464}; struct _tagged_arr _temp2462={( void*) _temp2461,( void*)
_temp2461,( void*)( _temp2461 + 2u)}; _temp2460( _temp2462);});} else{ return({
struct Cyc_PP_Doc*(* _temp2465)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2468= Cyc_Absynpp_tqual2doc( _temp2458); struct Cyc_PP_Doc* _temp2469= Cyc_Absynpp_ntyp2doc(
_temp2456); struct Cyc_PP_Doc* _temp2470= Cyc_PP_text( _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2471= Cyc_Absynpp_dtms2doc( dopt
== 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null(
dopt))->v, _temp2454); struct Cyc_PP_Doc* _temp2466[ 4u]={ _temp2468, _temp2469,
_temp2470, _temp2471}; struct _tagged_arr _temp2467={( void*) _temp2466,( void*)
_temp2466,( void*)( _temp2466 + 4u)}; _temp2465( _temp2467);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfield2doc( struct Cyc_Absyn_Structfield* f){ if( f->width != 0){
return({ struct Cyc_PP_Doc*(* _temp2472)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2475= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp2480=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2480->v=( void*) Cyc_PP_textptr( f->name); _temp2480;}));
struct Cyc_PP_Doc* _temp2476= Cyc_PP_text( _tag_arr(":", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2477= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( f->width))->v); struct Cyc_PP_Doc* _temp2478=
Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp2479= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2473[ 5u]={
_temp2475, _temp2476, _temp2477, _temp2478, _temp2479}; struct _tagged_arr
_temp2474={( void*) _temp2473,( void*) _temp2473,( void*)( _temp2473 + 5u)};
_temp2472( _temp2474);});} else{ return({ struct Cyc_PP_Doc*(* _temp2481)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2484= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2487=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2487->v=( void*) Cyc_PP_textptr(
f->name); _temp2487;})); struct Cyc_PP_Doc* _temp2485= Cyc_Absynpp_atts2doc( f->attributes);
struct Cyc_PP_Doc* _temp2486= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2482[ 3u]={ _temp2484, _temp2485, _temp2486};
struct _tagged_arr _temp2483={( void*) _temp2482,( void*) _temp2482,( void*)(
_temp2482 + 3u)}; _temp2481( _temp2483);});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc, _tag_arr("", sizeof(
unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc*(* _temp2488)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2491= Cyc_Absynpp_scope2doc((
void*) f->sc); struct Cyc_PP_Doc* _temp2492= Cyc_Absynpp_typedef_name2doc( f->name);
struct Cyc_PP_Doc* _temp2493= f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc(
f->tvs); struct Cyc_PP_Doc* _temp2494= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs); struct Cyc_PP_Doc* _temp2489[ 4u]={ _temp2491, _temp2492, _temp2493,
_temp2494}; struct _tagged_arr _temp2490={( void*) _temp2489,( void*) _temp2489,(
void*)( _temp2489 + 4u)}; _temp2488( _temp2490);});} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc, _tag_arr(",", sizeof(
unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=(( struct Cyc_List_List*)
_check_null( tdl))->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( tdl))->hd), 72, f);({ int(* _temp2495)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp2498= f; struct _tagged_arr _temp2499= _tag_arr("\n",
sizeof( unsigned char), 2u); void* _temp2496[ 0u]={}; struct _tagged_arr
_temp2497={( void*) _temp2496,( void*) _temp2496,( void*)( _temp2496 + 0u)};
_temp2495( _temp2498, _temp2499, _temp2497);});}} struct _tagged_arr Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql( _tag_arr("",
sizeof( unsigned char), 1u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_decl2doc,
tdl)), 72);} struct _tagged_arr Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp* e){
return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e), 72);} struct _tagged_arr
Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc( s), 72);} struct _tagged_arr Cyc_Absynpp_typ2string( void*
t){ return Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc( t), 72);} struct
_tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0* v){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_qvar2doc( v), 72);} struct _tagged_arr Cyc_Absynpp_prim2string( void*
p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_prim2doc( p), 72);} struct
_tagged_arr Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc( p), 72);} struct _tagged_arr Cyc_Absynpp_scope2string( void*
sc){ void* _temp2500= sc; _LL2502: if( _temp2500 ==( void*) Cyc_Absyn_Static){
goto _LL2503;} else{ goto _LL2504;} _LL2504: if( _temp2500 ==( void*) Cyc_Absyn_Public){
goto _LL2505;} else{ goto _LL2506;} _LL2506: if( _temp2500 ==( void*) Cyc_Absyn_Extern){
goto _LL2507;} else{ goto _LL2508;} _LL2508: if( _temp2500 ==( void*) Cyc_Absyn_ExternC){
goto _LL2509;} else{ goto _LL2510;} _LL2510: if( _temp2500 ==( void*) Cyc_Absyn_Abstract){
goto _LL2511;} else{ goto _LL2501;} _LL2503: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL2505: return _tag_arr("public", sizeof( unsigned char),
7u); _LL2507: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL2509:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL2511: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL2501:;}