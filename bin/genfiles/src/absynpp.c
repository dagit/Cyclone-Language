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
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); struct Cyc_Stdio___sFILE;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
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
_temp0, _temp0, _temp0 +  4u}; struct _tagged_arr* Cyc_Absynpp_cyc_stringptr=&
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
_check_null(* l))->tl ==  0){* l= 0;} else{ Cyc_Absynpp_suppr_last(&(( struct
Cyc_List_List*) _check_null(* l))->tl);}} static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)( struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
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
unsigned char), 3u); default: _LL12: if( c >= ' '? c <= '~': 0){ struct
_tagged_arr _temp14= Cyc_Core_new_string( 1);*(( unsigned char*)
_check_unknown_subscript( _temp14, sizeof( unsigned char), 0))= c; return(
struct _tagged_arr) _temp14;} else{ struct _tagged_arr _temp15= Cyc_Core_new_string(
4); int j= 0;*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript(
_temp15, sizeof( unsigned char), j ++))=( unsigned char)('0' + (( unsigned char)
c >>  6 &  7));*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))=( unsigned char)('0' + ( c >>  3 &  7));*(( unsigned char*)
_check_unknown_subscript( _temp15, sizeof( unsigned char), j ++))=(
unsigned char)('0' + ( c &  7)); return( struct _tagged_arr) _temp15;}}} static
int Cyc_Absynpp_special( struct _tagged_arr s){ int sz=( int)( _get_arr_size( s,
sizeof( unsigned char)) -  1);{ int i= 0; for( 0; i <  sz; i ++){ unsigned char
c=*(( const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
i)); if((( c <= ' '? 1: c >= '~')? 1: c == '"')? 1: c == '\\'){ return 1;}}}
return 0;} struct _tagged_arr Cyc_Absynpp_string_escape( struct _tagged_arr s){
if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)( _get_arr_size( s,
sizeof( unsigned char)) -  1); if( n >  0?*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), n)) == '\000': 0){ n --;}{
int len= 0;{ int i= 0; for( 0; i <=  n; i ++){ unsigned char _temp16=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); _LL18:
if( _temp16 == '\a'){ goto _LL19;} else{ goto _LL20;} _LL20: if( _temp16 == '\b'){
goto _LL21;} else{ goto _LL22;} _LL22: if( _temp16 == '\f'){ goto _LL23;} else{
goto _LL24;} _LL24: if( _temp16 == '\n'){ goto _LL25;} else{ goto _LL26;} _LL26:
if( _temp16 == '\r'){ goto _LL27;} else{ goto _LL28;} _LL28: if( _temp16 == '\t'){
goto _LL29;} else{ goto _LL30;} _LL30: if( _temp16 == '\v'){ goto _LL31;} else{
goto _LL32;} _LL32: if( _temp16 == '\\'){ goto _LL33;} else{ goto _LL34;} _LL34:
if( _temp16 == '"'){ goto _LL35;} else{ goto _LL36;} _LL36: goto _LL37; _LL19:
goto _LL21; _LL21: goto _LL23; _LL23: goto _LL25; _LL25: goto _LL27; _LL27: goto
_LL29; _LL29: goto _LL31; _LL31: goto _LL33; _LL33: goto _LL35; _LL35: len += 2;
goto _LL17; _LL37: if( _temp16 >= ' '? _temp16 <= '~': 0){ len ++;} else{ len +=
4;} goto _LL17; _LL17:;}}{ struct _tagged_arr t= Cyc_Core_new_string( len); int
j= 0;{ int i= 0; for( 0; i <=  n; i ++){ unsigned char _temp38=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); _LL40:
if( _temp38 == '\a'){ goto _LL41;} else{ goto _LL42;} _LL42: if( _temp38 == '\b'){
goto _LL43;} else{ goto _LL44;} _LL44: if( _temp38 == '\f'){ goto _LL45;} else{
goto _LL46;} _LL46: if( _temp38 == '\n'){ goto _LL47;} else{ goto _LL48;} _LL48:
if( _temp38 == '\r'){ goto _LL49;} else{ goto _LL50;} _LL50: if( _temp38 == '\t'){
goto _LL51;} else{ goto _LL52;} _LL52: if( _temp38 == '\v'){ goto _LL53;} else{
goto _LL54;} _LL54: if( _temp38 == '\\'){ goto _LL55;} else{ goto _LL56;} _LL56:
if( _temp38 == '"'){ goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL41:*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
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
goto _LL39; _LL59: if( _temp38 >= ' '? _temp38 <= '~': 0){*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))= _temp38;} else{*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))=(
unsigned char)('0' + ( _temp38 >>  6 &  7));*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))=( unsigned char)('0'
+ ( _temp38 >>  3 &  7));*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))=( unsigned char)('0' + ( _temp38 &  7));} goto
_LL39; _LL39:;}} return( struct _tagged_arr) t;}}}} static unsigned char _temp60[
9u]="restrict"; static struct _tagged_arr Cyc_Absynpp_restrict_string={ _temp60,
_temp60, _temp60 +  9u}; static unsigned char _temp61[ 9u]="volatile"; static
struct _tagged_arr Cyc_Absynpp_volatile_string={ _temp61, _temp61, _temp61 +  9u};
static unsigned char _temp62[ 6u]="const"; static struct _tagged_arr Cyc_Absynpp_const_str={
_temp62, _temp62, _temp62 +  6u}; static struct _tagged_arr* Cyc_Absynpp_restrict_sp=&
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
_temp66= k; _LL68: if( _temp66 == ( void*) Cyc_Absyn_AnyKind){ goto _LL69;}
else{ goto _LL70;} _LL70: if( _temp66 == ( void*) Cyc_Absyn_MemKind){ goto _LL71;}
else{ goto _LL72;} _LL72: if( _temp66 == ( void*) Cyc_Absyn_BoxKind){ goto _LL73;}
else{ goto _LL74;} _LL74: if( _temp66 == ( void*) Cyc_Absyn_RgnKind){ goto _LL75;}
else{ goto _LL76;} _LL76: if( _temp66 == ( void*) Cyc_Absyn_EffKind){ goto _LL77;}
else{ goto _LL67;} _LL69: return _tag_arr("A", sizeof( unsigned char), 2u);
_LL71: return _tag_arr("M", sizeof( unsigned char), 2u); _LL73: return _tag_arr("B",
sizeof( unsigned char), 2u); _LL75: return _tag_arr("R", sizeof( unsigned char),
2u); _LL77: return _tag_arr("E", sizeof( unsigned char), 2u); _LL67:;} struct
_tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void* _temp78=(
void*)( Cyc_Absyn_compress_conref( c))->v; void* _temp84; _LL80: if((
unsigned int) _temp78 >  1u?*(( int*) _temp78) ==  Cyc_Absyn_Eq_constr: 0){
_LL85: _temp84=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp78)->f1; goto
_LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL81: return Cyc_Absynpp_kind2string(
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
_temp96; _LL88: if(( unsigned int) _temp86 >  1u?*(( int*) _temp86) ==  Cyc_Absyn_Eq_constr:
0){ _LL95: _temp94=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp86)->f1;
if( _temp94 == ( void*) Cyc_Absyn_BoxKind){ goto _LL89;} else{ goto _LL90;}}
else{ goto _LL90;} _LL90: if(( unsigned int) _temp86 >  1u?*(( int*) _temp86) == 
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
void* att){ void* _temp100= att; _LL102: if( _temp100 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL103;} else{ goto _LL104;} _LL104: if( _temp100 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL105;} else{ goto _LL106;} _LL106: if( _temp100 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL107;} else{ goto _LL108;} _LL108: goto _LL109; _LL103: return Cyc_PP_nil_doc();
_LL105: return Cyc_PP_nil_doc(); _LL107: return Cyc_PP_nil_doc(); _LL109: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL101:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts !=  0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp110=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL112: if( _temp110 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp110 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp110 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL117;} else{ goto _LL118;} _LL118: goto _LL119; _LL113: return Cyc_PP_text(
_tag_arr(" _stdcall ", sizeof( unsigned char), 11u)); _LL115: return Cyc_PP_text(
_tag_arr(" _cdecl ", sizeof( unsigned char), 9u)); _LL117: return Cyc_PP_text(
_tag_arr(" _fastcall ", sizeof( unsigned char), 12u)); _LL119: goto _LL111;
_LL111:;} return Cyc_PP_nil_doc();} struct Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 !=  0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){
void* _temp120=( void*)(( struct Cyc_List_List*) _check_null( atts2))->hd;
_LL122: if( _temp120 == ( void*) Cyc_Absyn_Stdcall_att){ goto _LL123;} else{
goto _LL124;} _LL124: if( _temp120 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL125;}
else{ goto _LL126;} _LL126: if( _temp120 == ( void*) Cyc_Absyn_Fastcall_att){
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
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts ==  0){ return Cyc_PP_nil_doc();}
if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc( atts);} return({
struct Cyc_PP_Doc* _temp131[ 2u]; _temp131[ 1u]= Cyc_PP_group( _tag_arr("((",
sizeof( unsigned char), 3u), _tag_arr("))", sizeof( unsigned char), 3u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc,
atts)); _temp131[ 0u]= Cyc_PP_text( _tag_arr(" __attribute__", sizeof(
unsigned char), 15u)); Cyc_PP_cat( _tag_arr( _temp131, sizeof( struct Cyc_PP_Doc*),
2u));});} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List* tms){ if( tms
==  0){ return 0;}{ void* _temp132=( void*)(( struct Cyc_List_List*) _check_null(
tms))->hd; _LL134: if(( unsigned int) _temp132 >  1u?*(( int*) _temp132) ==  Cyc_Absyn_Pointer_mod:
0){ goto _LL135;} else{ goto _LL136;} _LL136: if(( unsigned int) _temp132 >  1u?*((
int*) _temp132) ==  Cyc_Absyn_Attributes_mod: 0){ goto _LL137;} else{ goto
_LL138;} _LL138: goto _LL139; _LL135: return 1; _LL137: if( ! Cyc_Absynpp_to_VC){
return 0;} return Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl); _LL139: return 0; _LL133:;}} extern struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct
Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms ==  0){ return d;}{ struct
Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d,(( struct Cyc_List_List*) _check_null(
tms))->tl); struct Cyc_PP_Doc* p_rest=({ struct Cyc_PP_Doc* _temp222[ 3u];
_temp222[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp222[
1u]= rest; _temp222[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp222, sizeof( struct Cyc_PP_Doc*), 3u));}); void*
_temp140=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp*
_temp154; void* _temp156; struct Cyc_List_List* _temp158; int _temp160; struct
Cyc_Position_Segment* _temp162; struct Cyc_List_List* _temp164; struct Cyc_Absyn_Tqual
_temp166; void* _temp168; void* _temp170; _LL142: if( _temp140 == ( void*) Cyc_Absyn_Carray_mod){
goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int) _temp140 >  1u?*((
int*) _temp140) ==  Cyc_Absyn_ConstArray_mod: 0){ _LL155: _temp154=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp140)->f1; goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int)
_temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_Function_mod: 0){ _LL157:
_temp156=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp140)->f1; goto
_LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp140 >  1u?*(( int*)
_temp140) ==  Cyc_Absyn_Attributes_mod: 0){ _LL159: _temp158=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp140)->f2; goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int)
_temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_TypeParams_mod: 0){ _LL165:
_temp164=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f1; goto _LL163;
_LL163: _temp162=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f2; goto
_LL161; _LL161: _temp160=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp140)->f3;
goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp140 >  1u?*((
int*) _temp140) ==  Cyc_Absyn_Pointer_mod: 0){ _LL171: _temp170=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f1; goto _LL169; _LL169:
_temp168=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f2; goto
_LL167; _LL167: _temp166=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp140)->f3;
goto _LL153;} else{ goto _LL141;} _LL143: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc* _temp172[ 2u]; _temp172[ 1u]= Cyc_PP_text( _tag_arr("[]", sizeof(
unsigned char), 3u)); _temp172[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp172,
sizeof( struct Cyc_PP_Doc*), 2u));}); _LL145: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc* _temp173[ 4u]; _temp173[ 3u]= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); _temp173[ 2u]= Cyc_Absynpp_exp2doc( _temp154); _temp173[ 1u]=
Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u)); _temp173[ 0u]= rest;
Cyc_PP_cat( _tag_arr( _temp173, sizeof( struct Cyc_PP_Doc*), 4u));}); _LL147:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;}{ void* _temp174= _temp156; struct Cyc_List_List* _temp180; struct
Cyc_Core_Opt* _temp182; struct Cyc_Absyn_VarargInfo* _temp184; int _temp186;
struct Cyc_List_List* _temp188; struct Cyc_Position_Segment* _temp190; struct
Cyc_List_List* _temp192; _LL176: if(*(( int*) _temp174) ==  Cyc_Absyn_WithTypes){
_LL189: _temp188=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f1; goto
_LL187; _LL187: _temp186=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f2;
goto _LL185; _LL185: _temp184=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f3;
goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f4;
goto _LL181; _LL181: _temp180=(( struct Cyc_Absyn_WithTypes_struct*) _temp174)->f5;
goto _LL177;} else{ goto _LL178;} _LL178: if(*(( int*) _temp174) ==  Cyc_Absyn_NoTypes){
_LL193: _temp192=(( struct Cyc_Absyn_NoTypes_struct*) _temp174)->f1; goto _LL191;
_LL191: _temp190=(( struct Cyc_Absyn_NoTypes_struct*) _temp174)->f2; goto _LL179;}
else{ goto _LL175;} _LL177: return({ struct Cyc_PP_Doc* _temp194[ 2u]; _temp194[
1u]= Cyc_Absynpp_funargs2doc( _temp188, _temp186, _temp184, _temp182, _temp180);
_temp194[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp194, sizeof( struct Cyc_PP_Doc*),
2u));}); _LL179: return({ struct Cyc_PP_Doc* _temp195[ 2u]; _temp195[ 1u]= Cyc_PP_group(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, _temp192)); _temp195[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp195,
sizeof( struct Cyc_PP_Doc*), 2u));}); _LL175:;} _LL149: if( ! Cyc_Absynpp_to_VC){
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} return({ struct Cyc_PP_Doc* _temp196[ 2u]; _temp196[ 1u]= Cyc_Absynpp_atts2doc(
_temp158); _temp196[ 0u]= rest; Cyc_PP_cat( _tag_arr( _temp196, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
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
_temp210; _LL202: if(( unsigned int) _temp200 >  1u?*(( int*) _temp200) ==  Cyc_Absyn_Nullable_ps:
0){ _LL209: _temp208=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp200)->f1;
goto _LL203;} else{ goto _LL204;} _LL204: if(( unsigned int) _temp200 >  1u?*((
int*) _temp200) ==  Cyc_Absyn_NonNullable_ps: 0){ _LL211: _temp210=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp200)->f1; goto _LL205;} else{ goto _LL206;} _LL206: if( _temp200 == ( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL207;} else{ goto _LL201;} _LL203: if( Cyc_Evexp_eval_const_uint_exp(
_temp208) ==  1){ ptr= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc* _temp212[ 4u]; _temp212[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp212[ 2u]= Cyc_Absynpp_exp2doc(
_temp208); _temp212[ 1u]= Cyc_PP_text( _tag_arr("{", sizeof( unsigned char), 2u));
_temp212[ 0u]= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp212, sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL201; _LL205:
if( Cyc_Evexp_eval_const_uint_exp( _temp210) ==  1){ ptr= Cyc_PP_text( _tag_arr("@",
sizeof( unsigned char), 2u));} else{ ptr=({ struct Cyc_PP_Doc* _temp213[ 4u];
_temp213[ 3u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp213[
2u]= Cyc_Absynpp_exp2doc( _temp210); _temp213[ 1u]= Cyc_PP_text( _tag_arr("{",
sizeof( unsigned char), 2u)); _temp213[ 0u]= Cyc_PP_text( _tag_arr("@", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp213, sizeof( struct Cyc_PP_Doc*),
4u));});} goto _LL201; _LL207: ptr= Cyc_PP_text( _tag_arr("?", sizeof(
unsigned char), 2u)); goto _LL201; _LL201:;}{ void* _temp214= Cyc_Tcutil_compress(
_temp168); _LL216: if( _temp214 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL217;}
else{ goto _LL218;} _LL218: goto _LL219; _LL217: return({ struct Cyc_PP_Doc*
_temp220[ 2u]; _temp220[ 1u]= rest; _temp220[ 0u]= ptr; Cyc_PP_cat( _tag_arr(
_temp220, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL219: return({ struct Cyc_PP_Doc*
_temp221[ 4u]; _temp221[ 3u]= rest; _temp221[ 2u]= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); _temp221[ 1u]= Cyc_Absynpp_typ2doc( _temp168);
_temp221[ 0u]= ptr; Cyc_PP_cat( _tag_arr( _temp221, sizeof( struct Cyc_PP_Doc*),
4u));}); _LL215:;}} _LL141:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){
void* _temp223= Cyc_Tcutil_compress( t); _LL225: if( _temp223 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL226;} else{ goto _LL227;} _LL227: goto _LL228; _LL226: return Cyc_PP_text(
_tag_arr("`H", sizeof( unsigned char), 3u)); _LL228: return Cyc_Absynpp_ntyp2doc(
t); _LL224:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
rgions, struct Cyc_List_List** effects, void* t){ void* _temp229= Cyc_Tcutil_compress(
t); void* _temp237; struct Cyc_List_List* _temp239; _LL231: if(( unsigned int)
_temp229 >  4u?*(( int*) _temp229) ==  Cyc_Absyn_AccessEff: 0){ _LL238: _temp237=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp229)->f1; goto _LL232;} else{
goto _LL233;} _LL233: if(( unsigned int) _temp229 >  4u?*(( int*) _temp229) == 
Cyc_Absyn_JoinEff: 0){ _LL240: _temp239=(( struct Cyc_Absyn_JoinEff_struct*)
_temp229)->f1; goto _LL234;} else{ goto _LL235;} _LL235: goto _LL236; _LL232:*
rgions=({ struct Cyc_List_List* _temp241=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp241->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp237); _temp241->tl=* rgions; _temp241;}); goto _LL230; _LL234: for( 0;
_temp239 !=  0; _temp239=(( struct Cyc_List_List*) _check_null( _temp239))->tl){
Cyc_Absynpp_effects2docs( rgions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp239))->hd);} goto _LL230; _LL236:* effects=({ struct Cyc_List_List*
_temp242=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp242->hd=( void*) Cyc_Absynpp_typ2doc( t); _temp242->tl=* effects; _temp242;});
goto _LL230; _LL230:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* rgions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
rgions,& effects, t); rgions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( rgions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( rgions ==  0? effects !=  0: 0){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("+", sizeof( unsigned char), 2u), effects);} else{ struct Cyc_PP_Doc*
_temp243= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u), rgions);
return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("+", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
effects,({ struct Cyc_List_List* _temp244=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp244->hd=( void*) _temp243; _temp244->tl= 0;
_temp244;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp245= t; struct Cyc_Core_Opt* _temp293; int _temp295; struct Cyc_Core_Opt*
_temp297; struct Cyc_Core_Opt* _temp299; struct Cyc_Absyn_Tvar* _temp301; struct
Cyc_Absyn_TunionInfo _temp303; void* _temp305; struct Cyc_List_List* _temp307;
void* _temp309; struct Cyc_Absyn_TunionFieldInfo _temp311; struct Cyc_List_List*
_temp313; void* _temp315; void* _temp317; void* _temp319; struct Cyc_List_List*
_temp321; struct Cyc_List_List* _temp323; struct _tuple0* _temp325; struct Cyc_List_List*
_temp327; struct _tuple0* _temp329; struct Cyc_List_List* _temp331; struct Cyc_List_List*
_temp333; struct _tuple0* _temp335; struct Cyc_Core_Opt* _temp337; struct Cyc_List_List*
_temp339; struct _tuple0* _temp341; void* _temp343; void* _temp345; _LL247: if((
unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_ArrayType: 0){
goto _LL248;} else{ goto _LL249;} _LL249: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_FnType: 0){ goto _LL250;} else{ goto _LL251;}
_LL251: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_PointerType:
0){ goto _LL252;} else{ goto _LL253;} _LL253: if( _temp245 == ( void*) Cyc_Absyn_VoidType){
goto _LL254;} else{ goto _LL255;} _LL255: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_Evar: 0){ _LL300: _temp299=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f1; goto _LL298; _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f2; goto _LL296; _LL296: _temp295=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f3; goto _LL294; _LL294: _temp293=(( struct Cyc_Absyn_Evar_struct*)
_temp245)->f4; goto _LL256;} else{ goto _LL257;} _LL257: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_VarType: 0){ _LL302: _temp301=((
struct Cyc_Absyn_VarType_struct*) _temp245)->f1; goto _LL258;} else{ goto _LL259;}
_LL259: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_TunionType:
0){ _LL304: _temp303=(( struct Cyc_Absyn_TunionType_struct*) _temp245)->f1;
_LL310: _temp309=( void*) _temp303.tunion_info; goto _LL308; _LL308: _temp307=
_temp303.targs; goto _LL306; _LL306: _temp305=( void*) _temp303.rgn; goto _LL260;}
else{ goto _LL261;} _LL261: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_TunionFieldType: 0){ _LL312: _temp311=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp245)->f1; _LL316: _temp315=( void*) _temp311.field_info; goto _LL314;
_LL314: _temp313= _temp311.targs; goto _LL262;} else{ goto _LL263;} _LL263: if((
unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_IntType: 0){
_LL320: _temp319=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp245)->f1;
goto _LL318; _LL318: _temp317=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp245)->f2; goto _LL264;} else{ goto _LL265;} _LL265: if( _temp245 == ( void*)
Cyc_Absyn_FloatType){ goto _LL266;} else{ goto _LL267;} _LL267: if( _temp245 == (
void*) Cyc_Absyn_DoubleType){ goto _LL268;} else{ goto _LL269;} _LL269: if((
unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_TupleType: 0){
_LL322: _temp321=(( struct Cyc_Absyn_TupleType_struct*) _temp245)->f1; goto
_LL270;} else{ goto _LL271;} _LL271: if(( unsigned int) _temp245 >  4u?*(( int*)
_temp245) ==  Cyc_Absyn_StructType: 0){ _LL326: _temp325=(( struct Cyc_Absyn_StructType_struct*)
_temp245)->f1; goto _LL324; _LL324: _temp323=(( struct Cyc_Absyn_StructType_struct*)
_temp245)->f2; goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_UnionType: 0){ _LL330: _temp329=((
struct Cyc_Absyn_UnionType_struct*) _temp245)->f1; goto _LL328; _LL328: _temp327=((
struct Cyc_Absyn_UnionType_struct*) _temp245)->f2; goto _LL274;} else{ goto
_LL275;} _LL275: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_AnonStructType:
0){ _LL332: _temp331=(( struct Cyc_Absyn_AnonStructType_struct*) _temp245)->f1;
goto _LL276;} else{ goto _LL277;} _LL277: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_AnonUnionType: 0){ _LL334: _temp333=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp245)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_EnumType: 0){ _LL336: _temp335=((
struct Cyc_Absyn_EnumType_struct*) _temp245)->f1; goto _LL280;} else{ goto
_LL281;} _LL281: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_TypedefType:
0){ _LL342: _temp341=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f1;
goto _LL340; _LL340: _temp339=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f2;
goto _LL338; _LL338: _temp337=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f3;
goto _LL282;} else{ goto _LL283;} _LL283: if(( unsigned int) _temp245 >  4u?*((
int*) _temp245) ==  Cyc_Absyn_RgnHandleType: 0){ _LL344: _temp343=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp245)->f1; goto _LL284;} else{ goto
_LL285;} _LL285: if( _temp245 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL286;}
else{ goto _LL287;} _LL287: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_RgnsEff: 0){ _LL346: _temp345=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp245)->f1; goto _LL288;} else{ goto _LL289;} _LL289: if(( unsigned int)
_temp245 >  4u?*(( int*) _temp245) ==  Cyc_Absyn_AccessEff: 0){ goto _LL290;}
else{ goto _LL291;} _LL291: if(( unsigned int) _temp245 >  4u?*(( int*) _temp245)
==  Cyc_Absyn_JoinEff: 0){ goto _LL292;} else{ goto _LL246;} _LL248: return Cyc_PP_text(
_tag_arr("[[[array]]]", sizeof( unsigned char), 12u)); _LL250: return Cyc_PP_nil_doc();
_LL252: return Cyc_PP_nil_doc(); _LL254: s= Cyc_PP_text( _tag_arr("void",
sizeof( unsigned char), 5u)); goto _LL246; _LL256: if( _temp297 !=  0){ return
Cyc_Absynpp_ntyp2doc(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp297))->v);}
else{ s=({ struct Cyc_PP_Doc* _temp347[ 6u]; _temp347[ 5u]= _temp299 ==  0? Cyc_PP_text(
_tag_arr("?", sizeof( unsigned char), 2u)): Cyc_Absynpp_kind2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp299))->v); _temp347[ 4u]= Cyc_PP_text(
_tag_arr(")::", sizeof( unsigned char), 4u)); _temp347[ 3u]=( ! Cyc_Absynpp_print_full_evars?
1: _temp293 ==  0)? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp293))->v);
_temp347[ 2u]= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp349; _temp349.tag= Cyc_Stdio_Int_pa; _temp349.f1=( int)(( unsigned int)
_temp295);{ void* _temp348[ 1u]={& _temp349}; Cyc_Stdio_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp348, sizeof( void*), 1u));}}));
_temp347[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); _temp347[
0u]= Cyc_PP_text( _tag_arr("%", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp347, sizeof( struct Cyc_PP_Doc*), 6u));});} goto _LL246; _LL258:
s= Cyc_PP_textptr( _temp301->name); if( Cyc_Absynpp_print_all_kinds){ s=({
struct Cyc_PP_Doc* _temp350[ 3u]; _temp350[ 2u]= Cyc_Absynpp_ckind2doc( _temp301->kind);
_temp350[ 1u]= Cyc_PP_text( _tag_arr("::", sizeof( unsigned char), 3u));
_temp350[ 0u]= s; Cyc_PP_cat( _tag_arr( _temp350, sizeof( struct Cyc_PP_Doc*), 3u));});}
if( Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar( _temp301): 0){ s=({
struct Cyc_PP_Doc* _temp351[ 3u]; _temp351[ 2u]= Cyc_PP_text( _tag_arr(" */",
sizeof( unsigned char), 4u)); _temp351[ 1u]= s; _temp351[ 0u]= Cyc_PP_text(
_tag_arr("_ /* ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp351,
sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL260:{ void* _temp352=
_temp309; struct Cyc_Absyn_UnknownTunionInfo _temp358; int _temp360; struct
_tuple0* _temp362; struct Cyc_Absyn_Tuniondecl* _temp364; struct Cyc_Absyn_Tuniondecl
_temp366; int _temp367; struct _tuple0* _temp369; _LL354: if(*(( int*) _temp352)
==  Cyc_Absyn_UnknownTunion){ _LL359: _temp358=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp352)->f1; _LL363: _temp362= _temp358.name; goto _LL361; _LL361: _temp360=
_temp358.is_xtunion; goto _LL355;} else{ goto _LL356;} _LL356: if(*(( int*)
_temp352) ==  Cyc_Absyn_KnownTunion){ _LL365: _temp364=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp352)->f1; _temp366=* _temp364; _LL370: _temp369= _temp366.name; goto _LL368;
_LL368: _temp367= _temp366.is_xtunion; goto _LL357;} else{ goto _LL353;} _LL355:
_temp369= _temp362; _temp367= _temp360; goto _LL357; _LL357: { struct Cyc_PP_Doc*
_temp371= Cyc_PP_text( _temp367? _tag_arr("xtunion ", sizeof( unsigned char), 9u):
_tag_arr("tunion ", sizeof( unsigned char), 8u));{ void* _temp372= Cyc_Tcutil_compress(
_temp305); _LL374: if( _temp372 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL375;}
else{ goto _LL376;} _LL376: goto _LL377; _LL375: s=({ struct Cyc_PP_Doc*
_temp378[ 3u]; _temp378[ 2u]= Cyc_Absynpp_tps2doc( _temp307); _temp378[ 1u]= Cyc_Absynpp_qvar2doc(
_temp369); _temp378[ 0u]= _temp371; Cyc_PP_cat( _tag_arr( _temp378, sizeof(
struct Cyc_PP_Doc*), 3u));}); goto _LL373; _LL377: s=({ struct Cyc_PP_Doc*
_temp379[ 5u]; _temp379[ 4u]= Cyc_Absynpp_tps2doc( _temp307); _temp379[ 3u]= Cyc_Absynpp_qvar2doc(
_temp369); _temp379[ 2u]= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp379[ 1u]= Cyc_Absynpp_typ2doc( _temp305); _temp379[ 0u]= _temp371; Cyc_PP_cat(
_tag_arr( _temp379, sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL373; _LL373:;}
goto _LL353;} _LL353:;} goto _LL246; _LL262:{ void* _temp380= _temp315; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp386; int _temp388; struct _tuple0*
_temp390; struct _tuple0* _temp392; struct Cyc_Absyn_Tunionfield* _temp394;
struct Cyc_Absyn_Tunionfield _temp396; struct _tuple0* _temp397; struct Cyc_Absyn_Tuniondecl*
_temp399; struct Cyc_Absyn_Tuniondecl _temp401; int _temp402; struct _tuple0*
_temp404; _LL382: if(*(( int*) _temp380) ==  Cyc_Absyn_UnknownTunionfield){
_LL387: _temp386=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp380)->f1;
_LL393: _temp392= _temp386.tunion_name; goto _LL391; _LL391: _temp390= _temp386.field_name;
goto _LL389; _LL389: _temp388= _temp386.is_xtunion; goto _LL383;} else{ goto
_LL384;} _LL384: if(*(( int*) _temp380) ==  Cyc_Absyn_KnownTunionfield){ _LL400:
_temp399=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp380)->f1; _temp401=*
_temp399; _LL405: _temp404= _temp401.name; goto _LL403; _LL403: _temp402=
_temp401.is_xtunion; goto _LL395; _LL395: _temp394=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp380)->f2; _temp396=* _temp394; _LL398: _temp397= _temp396.name; goto _LL385;}
else{ goto _LL381;} _LL383: _temp404= _temp392; _temp402= _temp388; _temp397=
_temp390; goto _LL385; _LL385: { struct Cyc_PP_Doc* _temp406= Cyc_PP_text(
_temp402? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc* _temp407[ 4u]; _temp407[ 3u]=
Cyc_Absynpp_qvar2doc( _temp397); _temp407[ 2u]= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); _temp407[ 1u]= Cyc_Absynpp_qvar2doc( _temp404);
_temp407[ 0u]= _temp406; Cyc_PP_cat( _tag_arr( _temp407, sizeof( struct Cyc_PP_Doc*),
4u));}); goto _LL381;} _LL381:;} goto _LL246; _LL264: { struct _tagged_arr sns;
struct _tagged_arr ts;{ void* _temp408= _temp319; _LL410: if( _temp408 == ( void*)
Cyc_Absyn_Signed){ goto _LL411;} else{ goto _LL412;} _LL412: if( _temp408 == (
void*) Cyc_Absyn_Unsigned){ goto _LL413;} else{ goto _LL409;} _LL411: sns=
_tag_arr("", sizeof( unsigned char), 1u); goto _LL409; _LL413: sns= _tag_arr("unsigned ",
sizeof( unsigned char), 10u); goto _LL409; _LL409:;}{ void* _temp414= _temp317;
_LL416: if( _temp414 == ( void*) Cyc_Absyn_B1){ goto _LL417;} else{ goto _LL418;}
_LL418: if( _temp414 == ( void*) Cyc_Absyn_B2){ goto _LL419;} else{ goto _LL420;}
_LL420: if( _temp414 == ( void*) Cyc_Absyn_B4){ goto _LL421;} else{ goto _LL422;}
_LL422: if( _temp414 == ( void*) Cyc_Absyn_B8){ goto _LL423;} else{ goto _LL415;}
_LL417:{ void* _temp424= _temp319; _LL426: if( _temp424 == ( void*) Cyc_Absyn_Signed){
goto _LL427;} else{ goto _LL428;} _LL428: if( _temp424 == ( void*) Cyc_Absyn_Unsigned){
goto _LL429;} else{ goto _LL425;} _LL427: sns= _tag_arr("signed ", sizeof(
unsigned char), 8u); goto _LL425; _LL429: goto _LL425; _LL425:;} ts= _tag_arr("char",
sizeof( unsigned char), 5u); goto _LL415; _LL419: ts= _tag_arr("short", sizeof(
unsigned char), 6u); goto _LL415; _LL421: ts= _tag_arr("int", sizeof(
unsigned char), 4u); goto _LL415; _LL423: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64",
sizeof( unsigned char), 8u): _tag_arr("long long", sizeof( unsigned char), 10u);
goto _LL415; _LL415:;} s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp432; _temp432.tag= Cyc_Stdio_String_pa; _temp432.f1=( struct _tagged_arr)
ts;{ struct Cyc_Stdio_String_pa_struct _temp431; _temp431.tag= Cyc_Stdio_String_pa;
_temp431.f1=( struct _tagged_arr) sns;{ void* _temp430[ 2u]={& _temp431,&
_temp432}; Cyc_Stdio_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp430, sizeof( void*), 2u));}}})); goto _LL246;} _LL266: s= Cyc_PP_text(
_tag_arr("float", sizeof( unsigned char), 6u)); goto _LL246; _LL268: s= Cyc_PP_text(
_tag_arr("double", sizeof( unsigned char), 7u)); goto _LL246; _LL270: s=({
struct Cyc_PP_Doc* _temp433[ 2u]; _temp433[ 1u]= Cyc_Absynpp_args2doc( _temp321);
_temp433[ 0u]= Cyc_PP_text( _tag_arr("$", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp433, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL272:
if( _temp325 ==  0){ s=({ struct Cyc_PP_Doc* _temp434[ 2u]; _temp434[ 1u]= Cyc_Absynpp_tps2doc(
_temp323); _temp434[ 0u]= Cyc_PP_text( _tag_arr("struct ", sizeof( unsigned char),
8u)); Cyc_PP_cat( _tag_arr( _temp434, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ s=({ struct Cyc_PP_Doc* _temp435[ 3u]; _temp435[ 2u]= Cyc_Absynpp_tps2doc(
_temp323); _temp435[ 1u]= _temp325 ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp325)); _temp435[ 0u]= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp435, sizeof( struct Cyc_PP_Doc*),
3u));});} goto _LL246; _LL274: if( _temp329 ==  0){ s=({ struct Cyc_PP_Doc*
_temp436[ 2u]; _temp436[ 1u]= Cyc_Absynpp_tps2doc( _temp327); _temp436[ 0u]= Cyc_PP_text(
_tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp436,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp437[ 3u];
_temp437[ 2u]= Cyc_Absynpp_tps2doc( _temp327); _temp437[ 1u]= _temp329 ==  0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp329));
_temp437[ 0u]= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); Cyc_PP_cat(
_tag_arr( _temp437, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL246; _LL276:
s=({ struct Cyc_PP_Doc* _temp438[ 3u]; _temp438[ 2u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp438[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp331)); _temp438[ 0u]= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp438, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL278: s=({ struct Cyc_PP_Doc* _temp439[ 3u]; _temp439[ 2u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp439[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_structfields2doc( _temp333)); _temp439[ 0u]= Cyc_PP_text(
_tag_arr("union {", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp439,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL280: s=({ struct Cyc_PP_Doc*
_temp440[ 2u]; _temp440[ 1u]= Cyc_Absynpp_qvar2doc( _temp335); _temp440[ 0u]=
Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp440, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL282: s=({
struct Cyc_PP_Doc* _temp441[ 2u]; _temp441[ 1u]= Cyc_Absynpp_tps2doc( _temp339);
_temp441[ 0u]= Cyc_Absynpp_qvar2doc( _temp341); Cyc_PP_cat( _tag_arr( _temp441,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL246; _LL284: s=({ struct Cyc_PP_Doc*
_temp442[ 3u]; _temp442[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof( unsigned char),
2u)); _temp442[ 1u]= Cyc_Absynpp_rgn2doc( _temp343); _temp442[ 0u]= Cyc_PP_text(
_tag_arr("region_t<", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp442, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL246; _LL286: s= Cyc_Absynpp_rgn2doc(
t); goto _LL246; _LL288: s=({ struct Cyc_PP_Doc* _temp443[ 3u]; _temp443[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp443[ 1u]= Cyc_Absynpp_typ2doc(
_temp345); _temp443[ 0u]= Cyc_PP_text( _tag_arr("regions(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp443, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL246; _LL290: goto _LL292; _LL292: s= Cyc_Absynpp_eff2doc( t);
goto _LL246; _LL246:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct
Cyc_Core_Opt* vo){ return vo ==  0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct _tuple8{
void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc( struct
_tuple8* cmp){ struct _tuple8 _temp446; void* _temp447; void* _temp449; struct
_tuple8* _temp444= cmp; _temp446=* _temp444; _LL450: _temp449= _temp446.f1; goto
_LL448; _LL448: _temp447= _temp446.f2; goto _LL445; _LL445: return({ struct Cyc_PP_Doc*
_temp451[ 3u]; _temp451[ 2u]= Cyc_Absynpp_rgn2doc( _temp447); _temp451[ 1u]= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); _temp451[ 0u]= Cyc_Absynpp_rgn2doc(
_temp449); Cyc_PP_cat( _tag_arr( _temp451, sizeof( struct Cyc_PP_Doc*), 3u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc( struct Cyc_List_List* po){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 ==  0? 0:({ struct Cyc_Core_Opt* _temp452=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp452->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp452;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp453=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp453=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp453,({
struct Cyc_List_List* _temp454=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp454->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp454->tl= 0; _temp454;}));} else{ if(
cyc_varargs !=  0){ struct Cyc_PP_Doc* _temp455=({ struct Cyc_PP_Doc* _temp457[
4u]; _temp457[ 3u]= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp458=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp458->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp458->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp458->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp458;})); _temp457[ 2u]=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text( _tag_arr(" inject ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u));
_temp457[ 1u]= Cyc_Absynpp_typ2doc(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->rgn); _temp457[ 0u]= Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); Cyc_PP_cat( _tag_arr( _temp457, sizeof( struct Cyc_PP_Doc*),
4u));}); _temp453=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp453,({ struct Cyc_List_List* _temp456=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp456->hd=(
void*) _temp455; _temp456->tl= 0; _temp456;}));}}{ struct Cyc_PP_Doc* _temp459=
Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u), _temp453); if(
effopt !=  0){ _temp459=({ struct Cyc_PP_Doc* _temp460[ 3u]; _temp460[ 2u]= Cyc_Absynpp_eff2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp460[ 1u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp460[ 0u]= _temp459; Cyc_PP_cat(
_tag_arr( _temp460, sizeof( struct Cyc_PP_Doc*), 3u));});} if( rgn_po !=  0){
_temp459=({ struct Cyc_PP_Doc* _temp461[ 3u]; _temp461[ 2u]= Cyc_Absynpp_rgnpo2doc(
rgn_po); _temp461[ 1u]= Cyc_PP_text( _tag_arr(":", sizeof( unsigned char), 2u));
_temp461[ 0u]= _temp459; Cyc_PP_cat( _tag_arr( _temp461, sizeof( struct Cyc_PP_Doc*),
3u));});} return({ struct Cyc_PP_Doc* _temp462[ 3u]; _temp462[ 2u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp462[ 1u]= _temp459; _temp462[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp462, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp463=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp463->f1=({ struct Cyc_Core_Opt*
_temp464=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp464->v=( void*)(* arg).f1; _temp464;}); _temp463->f2=(* arg).f2; _temp463->f3=(*
arg).f3; _temp463;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_arr* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp465= 0; int match;{ void*
_temp466=(* q).f1; struct Cyc_List_List* _temp474; struct Cyc_List_List*
_temp476; _LL468: if( _temp466 == ( void*) Cyc_Absyn_Loc_n){ goto _LL469;} else{
goto _LL470;} _LL470: if(( unsigned int) _temp466 >  1u?*(( int*) _temp466) == 
Cyc_Absyn_Rel_n: 0){ _LL475: _temp474=(( struct Cyc_Absyn_Rel_n_struct*)
_temp466)->f1; goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int)
_temp466 >  1u?*(( int*) _temp466) ==  Cyc_Absyn_Abs_n: 0){ _LL477: _temp476=((
struct Cyc_Absyn_Abs_n_struct*) _temp466)->f1; goto _LL473;} else{ goto _LL467;}
_LL469: _temp474= 0; goto _LL471; _LL471: match= 0; _temp465= _temp474; goto
_LL467; _LL473: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp476, Cyc_Absynpp_curr_namespace):
0; _temp465=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp478=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp478->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp478->tl= _temp476;
_temp478;}): _temp476; goto _LL467; _LL467:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp465,({
struct Cyc_List_List* _temp479=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp479->hd=( void*)(* q).f2; _temp479->tl= 0; _temp479;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp465,({ struct Cyc_List_List* _temp480=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp480->hd=( void*)(* q).f2; _temp480->tl= 0;
_temp480;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp481=(* v).f1; struct Cyc_List_List* _temp491; struct Cyc_List_List*
_temp493; _LL483: if( _temp481 == ( void*) Cyc_Absyn_Loc_n){ goto _LL484;} else{
goto _LL485;} _LL485: if(( unsigned int) _temp481 >  1u?*(( int*) _temp481) == 
Cyc_Absyn_Rel_n: 0){ _LL492: _temp491=(( struct Cyc_Absyn_Rel_n_struct*)
_temp481)->f1; if( _temp491 ==  0){ goto _LL486;} else{ goto _LL487;}} else{
goto _LL487;} _LL487: if(( unsigned int) _temp481 >  1u?*(( int*) _temp481) == 
Cyc_Absyn_Abs_n: 0){ _LL494: _temp493=(( struct Cyc_Absyn_Abs_n_struct*)
_temp481)->f1; goto _LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL484:
goto _LL486; _LL486: return Cyc_Absynpp_var2doc((* v).f2); _LL488: if((( int(*)(
int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp493,
Cyc_Absynpp_curr_namespace) ==  0){ return Cyc_Absynpp_var2doc((* v).f2);} else{
goto _LL490;} _LL490: return({ struct Cyc_PP_Doc* _temp495[ 2u]; _temp495[ 1u]=
Cyc_Absynpp_qvar2doc( v); _temp495[ 0u]= Cyc_PP_text( _tag_arr("/* bad namespace : */ ",
sizeof( unsigned char), 23u)); Cyc_PP_cat( _tag_arr( _temp495, sizeof( struct
Cyc_PP_Doc*), 2u));}); _LL482:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}}
struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*
e){ void* _temp496=( void*) e->r; void* _temp570; struct Cyc_Absyn_Exp* _temp572;
struct Cyc_Absyn_Exp* _temp574; _LL498: if(*(( int*) _temp496) ==  Cyc_Absyn_Const_e){
goto _LL499;} else{ goto _LL500;} _LL500: if(*(( int*) _temp496) ==  Cyc_Absyn_Var_e){
goto _LL501;} else{ goto _LL502;} _LL502: if(*(( int*) _temp496) ==  Cyc_Absyn_UnknownId_e){
goto _LL503;} else{ goto _LL504;} _LL504: if(*(( int*) _temp496) ==  Cyc_Absyn_Primop_e){
_LL571: _temp570=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp496)->f1;
goto _LL505;} else{ goto _LL506;} _LL506: if(*(( int*) _temp496) ==  Cyc_Absyn_AssignOp_e){
goto _LL507;} else{ goto _LL508;} _LL508: if(*(( int*) _temp496) ==  Cyc_Absyn_Increment_e){
goto _LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp496) ==  Cyc_Absyn_Conditional_e){
goto _LL511;} else{ goto _LL512;} _LL512: if(*(( int*) _temp496) ==  Cyc_Absyn_SeqExp_e){
goto _LL513;} else{ goto _LL514;} _LL514: if(*(( int*) _temp496) ==  Cyc_Absyn_UnknownCall_e){
goto _LL515;} else{ goto _LL516;} _LL516: if(*(( int*) _temp496) ==  Cyc_Absyn_FnCall_e){
goto _LL517;} else{ goto _LL518;} _LL518: if(*(( int*) _temp496) ==  Cyc_Absyn_Throw_e){
goto _LL519;} else{ goto _LL520;} _LL520: if(*(( int*) _temp496) ==  Cyc_Absyn_NoInstantiate_e){
_LL573: _temp572=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp496)->f1;
goto _LL521;} else{ goto _LL522;} _LL522: if(*(( int*) _temp496) ==  Cyc_Absyn_Instantiate_e){
_LL575: _temp574=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp496)->f1; goto
_LL523;} else{ goto _LL524;} _LL524: if(*(( int*) _temp496) ==  Cyc_Absyn_Cast_e){
goto _LL525;} else{ goto _LL526;} _LL526: if(*(( int*) _temp496) ==  Cyc_Absyn_New_e){
goto _LL527;} else{ goto _LL528;} _LL528: if(*(( int*) _temp496) ==  Cyc_Absyn_Address_e){
goto _LL529;} else{ goto _LL530;} _LL530: if(*(( int*) _temp496) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL531;} else{ goto _LL532;} _LL532: if(*(( int*) _temp496) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL533;} else{ goto _LL534;} _LL534: if(*(( int*) _temp496) ==  Cyc_Absyn_Offsetof_e){
goto _LL535;} else{ goto _LL536;} _LL536: if(*(( int*) _temp496) ==  Cyc_Absyn_Deref_e){
goto _LL537;} else{ goto _LL538;} _LL538: if(*(( int*) _temp496) ==  Cyc_Absyn_StructMember_e){
goto _LL539;} else{ goto _LL540;} _LL540: if(*(( int*) _temp496) ==  Cyc_Absyn_StructArrow_e){
goto _LL541;} else{ goto _LL542;} _LL542: if(*(( int*) _temp496) ==  Cyc_Absyn_Subscript_e){
goto _LL543;} else{ goto _LL544;} _LL544: if(*(( int*) _temp496) ==  Cyc_Absyn_Tuple_e){
goto _LL545;} else{ goto _LL546;} _LL546: if(*(( int*) _temp496) ==  Cyc_Absyn_CompoundLit_e){
goto _LL547;} else{ goto _LL548;} _LL548: if(*(( int*) _temp496) ==  Cyc_Absyn_Array_e){
goto _LL549;} else{ goto _LL550;} _LL550: if(*(( int*) _temp496) ==  Cyc_Absyn_Comprehension_e){
goto _LL551;} else{ goto _LL552;} _LL552: if(*(( int*) _temp496) ==  Cyc_Absyn_Struct_e){
goto _LL553;} else{ goto _LL554;} _LL554: if(*(( int*) _temp496) ==  Cyc_Absyn_AnonStruct_e){
goto _LL555;} else{ goto _LL556;} _LL556: if(*(( int*) _temp496) ==  Cyc_Absyn_Tunion_e){
goto _LL557;} else{ goto _LL558;} _LL558: if(*(( int*) _temp496) ==  Cyc_Absyn_Enum_e){
goto _LL559;} else{ goto _LL560;} _LL560: if(*(( int*) _temp496) ==  Cyc_Absyn_Malloc_e){
goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp496) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL563;} else{ goto _LL564;} _LL564: if(*(( int*) _temp496) ==  Cyc_Absyn_StmtExp_e){
goto _LL565;} else{ goto _LL566;} _LL566: if(*(( int*) _temp496) ==  Cyc_Absyn_Codegen_e){
goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp496) ==  Cyc_Absyn_Fill_e){
goto _LL569;} else{ goto _LL497;} _LL499: return 10000; _LL501: return 10000;
_LL503: return 10000; _LL505: { void* _temp576= _temp570; _LL578: if( _temp576
== ( void*) Cyc_Absyn_Plus){ goto _LL579;} else{ goto _LL580;} _LL580: if(
_temp576 == ( void*) Cyc_Absyn_Times){ goto _LL581;} else{ goto _LL582;} _LL582:
if( _temp576 == ( void*) Cyc_Absyn_Minus){ goto _LL583;} else{ goto _LL584;}
_LL584: if( _temp576 == ( void*) Cyc_Absyn_Div){ goto _LL585;} else{ goto _LL586;}
_LL586: if( _temp576 == ( void*) Cyc_Absyn_Mod){ goto _LL587;} else{ goto _LL588;}
_LL588: if( _temp576 == ( void*) Cyc_Absyn_Eq){ goto _LL589;} else{ goto _LL590;}
_LL590: if( _temp576 == ( void*) Cyc_Absyn_Neq){ goto _LL591;} else{ goto _LL592;}
_LL592: if( _temp576 == ( void*) Cyc_Absyn_Gt){ goto _LL593;} else{ goto _LL594;}
_LL594: if( _temp576 == ( void*) Cyc_Absyn_Lt){ goto _LL595;} else{ goto _LL596;}
_LL596: if( _temp576 == ( void*) Cyc_Absyn_Gte){ goto _LL597;} else{ goto _LL598;}
_LL598: if( _temp576 == ( void*) Cyc_Absyn_Lte){ goto _LL599;} else{ goto _LL600;}
_LL600: if( _temp576 == ( void*) Cyc_Absyn_Not){ goto _LL601;} else{ goto _LL602;}
_LL602: if( _temp576 == ( void*) Cyc_Absyn_Bitnot){ goto _LL603;} else{ goto
_LL604;} _LL604: if( _temp576 == ( void*) Cyc_Absyn_Bitand){ goto _LL605;} else{
goto _LL606;} _LL606: if( _temp576 == ( void*) Cyc_Absyn_Bitor){ goto _LL607;}
else{ goto _LL608;} _LL608: if( _temp576 == ( void*) Cyc_Absyn_Bitxor){ goto
_LL609;} else{ goto _LL610;} _LL610: if( _temp576 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL611;} else{ goto _LL612;} _LL612: if( _temp576 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL613;} else{ goto _LL614;} _LL614: if( _temp576 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL615;} else{ goto _LL616;} _LL616: if( _temp576 == ( void*) Cyc_Absyn_Size){
goto _LL617;} else{ goto _LL577;} _LL579: return 100; _LL581: return 110; _LL583:
return 100; _LL585: return 110; _LL587: return 110; _LL589: return 70; _LL591:
return 70; _LL593: return 80; _LL595: return 80; _LL597: return 80; _LL599:
return 80; _LL601: return 130; _LL603: return 130; _LL605: return 60; _LL607:
return 40; _LL609: return 50; _LL611: return 90; _LL613: return 80; _LL615:
return 80; _LL617: return 140; _LL577:;} _LL507: return 20; _LL509: return 130;
_LL511: return 30; _LL513: return 10; _LL515: return 140; _LL517: return 140;
_LL519: return 130; _LL521: return Cyc_Absynpp_exp_prec( _temp572); _LL523:
return Cyc_Absynpp_exp_prec( _temp574); _LL525: return 120; _LL527: return 15;
_LL529: return 130; _LL531: return 130; _LL533: return 130; _LL535: return 130;
_LL537: return 130; _LL539: return 140; _LL541: return 140; _LL543: return 140;
_LL545: return 150; _LL547: goto _LL549; _LL549: goto _LL551; _LL551: goto
_LL553; _LL553: goto _LL555; _LL555: goto _LL557; _LL557: goto _LL559; _LL559:
goto _LL561; _LL561: return 140; _LL563: return 140; _LL565: return 10000;
_LL567: return 140; _LL569: return 140; _LL497:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec(
e); struct Cyc_PP_Doc* s;{ void* _temp618=( void*) e->r; void* _temp692; struct
_tuple0* _temp694; struct _tuple0* _temp696; struct Cyc_List_List* _temp698;
void* _temp700; struct Cyc_Absyn_Exp* _temp702; struct Cyc_Core_Opt* _temp704;
struct Cyc_Absyn_Exp* _temp706; void* _temp708; struct Cyc_Absyn_Exp* _temp710;
struct Cyc_Absyn_Exp* _temp712; struct Cyc_Absyn_Exp* _temp714; struct Cyc_Absyn_Exp*
_temp716; struct Cyc_Absyn_Exp* _temp718; struct Cyc_Absyn_Exp* _temp720; struct
Cyc_List_List* _temp722; struct Cyc_Absyn_Exp* _temp724; struct Cyc_List_List*
_temp726; struct Cyc_Absyn_Exp* _temp728; struct Cyc_Absyn_Exp* _temp730; struct
Cyc_Absyn_Exp* _temp732; struct Cyc_Absyn_Exp* _temp734; struct Cyc_Absyn_Exp*
_temp736; void* _temp738; struct Cyc_Absyn_Exp* _temp740; struct Cyc_Absyn_Exp*
_temp742; struct Cyc_Absyn_Exp* _temp744; void* _temp746; struct Cyc_Absyn_Exp*
_temp748; struct _tagged_arr* _temp750; void* _temp752; struct Cyc_Absyn_Exp*
_temp754; struct _tagged_arr* _temp756; struct Cyc_Absyn_Exp* _temp758; struct
_tagged_arr* _temp760; struct Cyc_Absyn_Exp* _temp762; struct Cyc_Absyn_Exp*
_temp764; struct Cyc_Absyn_Exp* _temp766; struct Cyc_List_List* _temp768; struct
Cyc_List_List* _temp770; struct _tuple1* _temp772; struct Cyc_List_List*
_temp774; struct Cyc_Absyn_Exp* _temp776; struct Cyc_Absyn_Exp* _temp778; struct
Cyc_Absyn_Vardecl* _temp780; struct Cyc_List_List* _temp782; struct _tuple0*
_temp784; struct Cyc_List_List* _temp786; struct Cyc_Absyn_Tunionfield* _temp788;
struct Cyc_List_List* _temp790; struct _tuple0* _temp792; void* _temp794; struct
Cyc_Absyn_Exp* _temp796; struct Cyc_List_List* _temp798; struct Cyc_Core_Opt*
_temp800; struct Cyc_Absyn_Stmt* _temp802; struct Cyc_Absyn_Fndecl* _temp804;
struct Cyc_Absyn_Exp* _temp806; _LL620: if(*(( int*) _temp618) ==  Cyc_Absyn_Const_e){
_LL693: _temp692=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp618)->f1;
goto _LL621;} else{ goto _LL622;} _LL622: if(*(( int*) _temp618) ==  Cyc_Absyn_Var_e){
_LL695: _temp694=(( struct Cyc_Absyn_Var_e_struct*) _temp618)->f1; goto _LL623;}
else{ goto _LL624;} _LL624: if(*(( int*) _temp618) ==  Cyc_Absyn_UnknownId_e){
_LL697: _temp696=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp618)->f1; goto
_LL625;} else{ goto _LL626;} _LL626: if(*(( int*) _temp618) ==  Cyc_Absyn_Primop_e){
_LL701: _temp700=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp618)->f1;
goto _LL699; _LL699: _temp698=(( struct Cyc_Absyn_Primop_e_struct*) _temp618)->f2;
goto _LL627;} else{ goto _LL628;} _LL628: if(*(( int*) _temp618) ==  Cyc_Absyn_AssignOp_e){
_LL707: _temp706=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp618)->f1; goto
_LL705; _LL705: _temp704=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp618)->f2;
goto _LL703; _LL703: _temp702=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp618)->f3;
goto _LL629;} else{ goto _LL630;} _LL630: if(*(( int*) _temp618) ==  Cyc_Absyn_Increment_e){
_LL711: _temp710=(( struct Cyc_Absyn_Increment_e_struct*) _temp618)->f1; goto
_LL709; _LL709: _temp708=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp618)->f2; goto _LL631;} else{ goto _LL632;} _LL632: if(*(( int*) _temp618)
==  Cyc_Absyn_Conditional_e){ _LL717: _temp716=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp618)->f1; goto _LL715; _LL715: _temp714=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp618)->f2; goto _LL713; _LL713: _temp712=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp618)->f3; goto _LL633;} else{ goto _LL634;} _LL634: if(*(( int*) _temp618)
==  Cyc_Absyn_SeqExp_e){ _LL721: _temp720=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp618)->f1; goto _LL719; _LL719: _temp718=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp618)->f2; goto _LL635;} else{ goto _LL636;} _LL636: if(*(( int*) _temp618)
==  Cyc_Absyn_UnknownCall_e){ _LL725: _temp724=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp618)->f1; goto _LL723; _LL723: _temp722=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp618)->f2; goto _LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp618)
==  Cyc_Absyn_FnCall_e){ _LL729: _temp728=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp618)->f1; goto _LL727; _LL727: _temp726=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp618)->f2; goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp618)
==  Cyc_Absyn_Throw_e){ _LL731: _temp730=(( struct Cyc_Absyn_Throw_e_struct*)
_temp618)->f1; goto _LL641;} else{ goto _LL642;} _LL642: if(*(( int*) _temp618)
==  Cyc_Absyn_NoInstantiate_e){ _LL733: _temp732=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp618)->f1; goto _LL643;} else{ goto _LL644;} _LL644: if(*(( int*) _temp618)
==  Cyc_Absyn_Instantiate_e){ _LL735: _temp734=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp618)->f1; goto _LL645;} else{ goto _LL646;} _LL646: if(*(( int*) _temp618)
==  Cyc_Absyn_Cast_e){ _LL739: _temp738=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp618)->f1; goto _LL737; _LL737: _temp736=(( struct Cyc_Absyn_Cast_e_struct*)
_temp618)->f2; goto _LL647;} else{ goto _LL648;} _LL648: if(*(( int*) _temp618)
==  Cyc_Absyn_Address_e){ _LL741: _temp740=(( struct Cyc_Absyn_Address_e_struct*)
_temp618)->f1; goto _LL649;} else{ goto _LL650;} _LL650: if(*(( int*) _temp618)
==  Cyc_Absyn_New_e){ _LL745: _temp744=(( struct Cyc_Absyn_New_e_struct*)
_temp618)->f1; goto _LL743; _LL743: _temp742=(( struct Cyc_Absyn_New_e_struct*)
_temp618)->f2; goto _LL651;} else{ goto _LL652;} _LL652: if(*(( int*) _temp618)
==  Cyc_Absyn_Sizeoftyp_e){ _LL747: _temp746=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp618)->f1; goto _LL653;} else{ goto _LL654;} _LL654: if(*(( int*) _temp618)
==  Cyc_Absyn_Sizeofexp_e){ _LL749: _temp748=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp618)->f1; goto _LL655;} else{ goto _LL656;} _LL656: if(*(( int*) _temp618)
==  Cyc_Absyn_Offsetof_e){ _LL753: _temp752=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp618)->f1; goto _LL751; _LL751: _temp750=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp618)->f2; goto _LL657;} else{ goto _LL658;} _LL658: if(*(( int*) _temp618)
==  Cyc_Absyn_Deref_e){ _LL755: _temp754=(( struct Cyc_Absyn_Deref_e_struct*)
_temp618)->f1; goto _LL659;} else{ goto _LL660;} _LL660: if(*(( int*) _temp618)
==  Cyc_Absyn_StructMember_e){ _LL759: _temp758=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp618)->f1; goto _LL757; _LL757: _temp756=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp618)->f2; goto _LL661;} else{ goto _LL662;} _LL662: if(*(( int*) _temp618)
==  Cyc_Absyn_StructArrow_e){ _LL763: _temp762=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp618)->f1; goto _LL761; _LL761: _temp760=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp618)->f2; goto _LL663;} else{ goto _LL664;} _LL664: if(*(( int*) _temp618)
==  Cyc_Absyn_Subscript_e){ _LL767: _temp766=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp618)->f1; goto _LL765; _LL765: _temp764=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp618)->f2; goto _LL665;} else{ goto _LL666;} _LL666: if(*(( int*) _temp618)
==  Cyc_Absyn_Tuple_e){ _LL769: _temp768=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp618)->f1; goto _LL667;} else{ goto _LL668;} _LL668: if(*(( int*) _temp618)
==  Cyc_Absyn_CompoundLit_e){ _LL773: _temp772=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp618)->f1; goto _LL771; _LL771: _temp770=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp618)->f2; goto _LL669;} else{ goto _LL670;} _LL670: if(*(( int*) _temp618)
==  Cyc_Absyn_Array_e){ _LL775: _temp774=(( struct Cyc_Absyn_Array_e_struct*)
_temp618)->f1; goto _LL671;} else{ goto _LL672;} _LL672: if(*(( int*) _temp618)
==  Cyc_Absyn_Comprehension_e){ _LL781: _temp780=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp618)->f1; goto _LL779; _LL779: _temp778=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp618)->f2; goto _LL777; _LL777: _temp776=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp618)->f3; goto _LL673;} else{ goto _LL674;} _LL674: if(*(( int*) _temp618)
==  Cyc_Absyn_Struct_e){ _LL785: _temp784=(( struct Cyc_Absyn_Struct_e_struct*)
_temp618)->f1; goto _LL783; _LL783: _temp782=(( struct Cyc_Absyn_Struct_e_struct*)
_temp618)->f3; goto _LL675;} else{ goto _LL676;} _LL676: if(*(( int*) _temp618)
==  Cyc_Absyn_AnonStruct_e){ _LL787: _temp786=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp618)->f2; goto _LL677;} else{ goto _LL678;} _LL678: if(*(( int*) _temp618)
==  Cyc_Absyn_Tunion_e){ _LL791: _temp790=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp618)->f3; goto _LL789; _LL789: _temp788=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp618)->f5; goto _LL679;} else{ goto _LL680;} _LL680: if(*(( int*) _temp618)
==  Cyc_Absyn_Enum_e){ _LL793: _temp792=(( struct Cyc_Absyn_Enum_e_struct*)
_temp618)->f1; goto _LL681;} else{ goto _LL682;} _LL682: if(*(( int*) _temp618)
==  Cyc_Absyn_Malloc_e){ _LL797: _temp796=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp618)->f1; goto _LL795; _LL795: _temp794=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp618)->f2; goto _LL683;} else{ goto _LL684;} _LL684: if(*(( int*) _temp618)
==  Cyc_Absyn_UnresolvedMem_e){ _LL801: _temp800=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp618)->f1; goto _LL799; _LL799: _temp798=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp618)->f2; goto _LL685;} else{ goto _LL686;} _LL686: if(*(( int*) _temp618)
==  Cyc_Absyn_StmtExp_e){ _LL803: _temp802=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp618)->f1; goto _LL687;} else{ goto _LL688;} _LL688: if(*(( int*) _temp618)
==  Cyc_Absyn_Codegen_e){ _LL805: _temp804=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp618)->f1; goto _LL689;} else{ goto _LL690;} _LL690: if(*(( int*) _temp618)
==  Cyc_Absyn_Fill_e){ _LL807: _temp806=(( struct Cyc_Absyn_Fill_e_struct*)
_temp618)->f1; goto _LL691;} else{ goto _LL619;} _LL621: s= Cyc_Absynpp_cnst2doc(
_temp692); goto _LL619; _LL623: _temp696= _temp694; goto _LL625; _LL625: s= Cyc_Absynpp_qvar2doc(
_temp696); goto _LL619; _LL627: s= Cyc_Absynpp_primapp2doc( myprec, _temp700,
_temp698); goto _LL619; _LL629: s=({ struct Cyc_PP_Doc* _temp808[ 5u]; _temp808[
4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp702); _temp808[ 3u]= Cyc_PP_text(
_tag_arr("= ", sizeof( unsigned char), 3u)); _temp808[ 2u]= _temp704 ==  0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp704))->v); _temp808[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp808[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp706); Cyc_PP_cat( _tag_arr( _temp808, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL619; _LL631: { struct Cyc_PP_Doc* _temp809= Cyc_Absynpp_exp2doc_prec(
myprec, _temp710);{ void* _temp810= _temp708; _LL812: if( _temp810 == ( void*)
Cyc_Absyn_PreInc){ goto _LL813;} else{ goto _LL814;} _LL814: if( _temp810 == (
void*) Cyc_Absyn_PreDec){ goto _LL815;} else{ goto _LL816;} _LL816: if( _temp810
== ( void*) Cyc_Absyn_PostInc){ goto _LL817;} else{ goto _LL818;} _LL818: if(
_temp810 == ( void*) Cyc_Absyn_PostDec){ goto _LL819;} else{ goto _LL811;}
_LL813: s=({ struct Cyc_PP_Doc* _temp820[ 2u]; _temp820[ 1u]= _temp809; _temp820[
0u]= Cyc_PP_text( _tag_arr("++", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp820, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL811; _LL815: s=({
struct Cyc_PP_Doc* _temp821[ 2u]; _temp821[ 1u]= _temp809; _temp821[ 0u]= Cyc_PP_text(
_tag_arr("--", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr( _temp821,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL811; _LL817: s=({ struct Cyc_PP_Doc*
_temp822[ 2u]; _temp822[ 1u]= Cyc_PP_text( _tag_arr("++", sizeof( unsigned char),
3u)); _temp822[ 0u]= _temp809; Cyc_PP_cat( _tag_arr( _temp822, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL811; _LL819: s=({ struct Cyc_PP_Doc* _temp823[ 2u];
_temp823[ 1u]= Cyc_PP_text( _tag_arr("--", sizeof( unsigned char), 3u));
_temp823[ 0u]= _temp809; Cyc_PP_cat( _tag_arr( _temp823, sizeof( struct Cyc_PP_Doc*),
2u));}); goto _LL811; _LL811:;} goto _LL619;} _LL633:{ struct _tuple8 _temp825=({
struct _tuple8 _temp824; _temp824.f1=( void*) _temp714->r; _temp824.f2=( void*)
_temp712->r; _temp824;}); _LL827: goto _LL828; _LL828: s=({ struct Cyc_PP_Doc*
_temp829[ 5u]; _temp829[ 4u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp712);
_temp829[ 3u]= Cyc_PP_text( _tag_arr(" : ", sizeof( unsigned char), 4u));
_temp829[ 2u]= Cyc_Absynpp_exp2doc_prec( 0, _temp714); _temp829[ 1u]= Cyc_PP_text(
_tag_arr(" ? ", sizeof( unsigned char), 4u)); _temp829[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp716); Cyc_PP_cat( _tag_arr( _temp829, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL826; _LL826:;} goto _LL619; _LL635: s=({ struct Cyc_PP_Doc*
_temp830[ 5u]; _temp830[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp830[ 3u]= Cyc_Absynpp_exp2doc( _temp718); _temp830[ 2u]= Cyc_PP_text(
_tag_arr(", ", sizeof( unsigned char), 3u)); _temp830[ 1u]= Cyc_Absynpp_exp2doc(
_temp720); _temp830[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp830, sizeof( struct Cyc_PP_Doc*), 5u));}); goto
_LL619; _LL637: s=({ struct Cyc_PP_Doc* _temp831[ 4u]; _temp831[ 3u]= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); _temp831[ 2u]= Cyc_Absynpp_exps2doc_prec(
20, _temp722); _temp831[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); _temp831[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp724); Cyc_PP_cat(
_tag_arr( _temp831, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL619; _LL639: s=({
struct Cyc_PP_Doc* _temp832[ 4u]; _temp832[ 3u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp832[ 2u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp726); _temp832[ 1u]= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
_temp832[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp728); Cyc_PP_cat( _tag_arr(
_temp832, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL619; _LL641: s=({ struct
Cyc_PP_Doc* _temp833[ 2u]; _temp833[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp730); _temp833[ 0u]= Cyc_PP_text( _tag_arr("throw ", sizeof( unsigned char),
7u)); Cyc_PP_cat( _tag_arr( _temp833, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL619; _LL643: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp732); goto _LL619;
_LL645: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp734); goto _LL619; _LL647: s=({
struct Cyc_PP_Doc* _temp834[ 4u]; _temp834[ 3u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp736); _temp834[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp834[ 1u]= Cyc_Absynpp_typ2doc( _temp738); _temp834[ 0u]=
Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr(
_temp834, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL619; _LL649: s=({ struct
Cyc_PP_Doc* _temp835[ 2u]; _temp835[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec,
_temp740); _temp835[ 0u]= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp835, sizeof( struct Cyc_PP_Doc*), 2u));}); goto
_LL619; _LL651: if( _temp744 ==  0){ s=({ struct Cyc_PP_Doc* _temp836[ 2u];
_temp836[ 1u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp742); _temp836[ 0u]= Cyc_PP_text(
_tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp836,
sizeof( struct Cyc_PP_Doc*), 2u));});} else{ s=({ struct Cyc_PP_Doc* _temp837[ 4u];
_temp837[ 3u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp742); _temp837[ 2u]= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); _temp837[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp744)); _temp837[ 0u]= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp837,
sizeof( struct Cyc_PP_Doc*), 4u));});} goto _LL619; _LL653: s=({ struct Cyc_PP_Doc*
_temp838[ 3u]; _temp838[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp838[ 1u]= Cyc_Absynpp_typ2doc( _temp746); _temp838[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp838,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL619; _LL655: s=({ struct Cyc_PP_Doc*
_temp839[ 3u]; _temp839[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp839[ 1u]= Cyc_Absynpp_exp2doc( _temp748); _temp839[ 0u]= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp839,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL619; _LL657: s=({ struct Cyc_PP_Doc*
_temp840[ 5u]; _temp840[ 4u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp840[ 3u]= Cyc_PP_textptr( _temp750); _temp840[ 2u]= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); _temp840[ 1u]= Cyc_Absynpp_typ2doc(
_temp752); _temp840[ 0u]= Cyc_PP_text( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp840, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL619; _LL659: s=({ struct Cyc_PP_Doc* _temp841[ 2u]; _temp841[ 1u]=
Cyc_Absynpp_exp2doc_prec( myprec, _temp754); _temp841[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp841,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL619; _LL661: s=({ struct Cyc_PP_Doc*
_temp842[ 3u]; _temp842[ 2u]= Cyc_PP_textptr( _temp756); _temp842[ 1u]= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); _temp842[ 0u]= Cyc_Absynpp_exp2doc_prec(
myprec, _temp758); Cyc_PP_cat( _tag_arr( _temp842, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL619; _LL663: s=({ struct Cyc_PP_Doc* _temp843[ 3u]; _temp843[ 2u]=
Cyc_PP_textptr( _temp760); _temp843[ 1u]= Cyc_PP_text( _tag_arr("->", sizeof(
unsigned char), 3u)); _temp843[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp762);
Cyc_PP_cat( _tag_arr( _temp843, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL619; _LL665: s=({ struct Cyc_PP_Doc* _temp844[ 4u]; _temp844[ 3u]= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); _temp844[ 2u]= Cyc_Absynpp_exp2doc(
_temp764); _temp844[ 1u]= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u));
_temp844[ 0u]= Cyc_Absynpp_exp2doc_prec( myprec, _temp766); Cyc_PP_cat( _tag_arr(
_temp844, sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL619; _LL667: s=({ struct
Cyc_PP_Doc* _temp845[ 3u]; _temp845[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); _temp845[ 1u]= Cyc_Absynpp_exps2doc_prec( 20, _temp768);
_temp845[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp845, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL619; _LL669: s=({
struct Cyc_PP_Doc* _temp846[ 4u]; _temp846[ 3u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp770)); _temp846[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
_temp846[ 1u]= Cyc_Absynpp_typ2doc((* _temp772).f3); _temp846[ 0u]= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp846,
sizeof( struct Cyc_PP_Doc*), 4u));}); goto _LL619; _LL671: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp774)); goto _LL619; _LL673: s=({ struct Cyc_PP_Doc*
_temp847[ 7u]; _temp847[ 6u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp847[ 5u]= Cyc_Absynpp_exp2doc( _temp776); _temp847[ 4u]= Cyc_PP_text(
_tag_arr(" : ", sizeof( unsigned char), 4u)); _temp847[ 3u]= Cyc_Absynpp_exp2doc(
_temp778); _temp847[ 2u]= Cyc_PP_text( _tag_arr(" < ", sizeof( unsigned char), 4u));
_temp847[ 1u]= Cyc_PP_text(*(* _temp780->name).f2); _temp847[ 0u]= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp847,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL619; _LL675: s=({ struct Cyc_PP_Doc*
_temp848[ 2u]; _temp848[ 1u]= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char),
2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp782));
_temp848[ 0u]= Cyc_Absynpp_qvar2doc( _temp784); Cyc_PP_cat( _tag_arr( _temp848,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL619; _LL677: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp786)); goto _LL619; _LL679: if( _temp790 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp788->name);} else{ s=({ struct Cyc_PP_Doc* _temp849[ 2u]; _temp849[ 1u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp790)); _temp849[ 0u]= Cyc_Absynpp_qvar2doc( _temp788->name);
Cyc_PP_cat( _tag_arr( _temp849, sizeof( struct Cyc_PP_Doc*), 2u));});} goto
_LL619; _LL681: s= Cyc_Absynpp_qvar2doc( _temp792); goto _LL619; _LL683: if(
_temp796 ==  0){ s=({ struct Cyc_PP_Doc* _temp850[ 3u]; _temp850[ 2u]= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); _temp850[ 1u]= Cyc_Absynpp_typ2doc(
_temp794); _temp850[ 0u]= Cyc_PP_text( _tag_arr("malloc(sizeof(", sizeof(
unsigned char), 15u)); Cyc_PP_cat( _tag_arr( _temp850, sizeof( struct Cyc_PP_Doc*),
3u));});} else{ s=({ struct Cyc_PP_Doc* _temp851[ 6u]; _temp851[ 5u]= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); _temp851[ 4u]= Cyc_Absynpp_typ2doc(
_temp794); _temp851[ 3u]= Cyc_PP_text( _tag_arr("sizeof(", sizeof( unsigned char),
8u)); _temp851[ 2u]= Cyc_PP_text( _tag_arr(",", sizeof( unsigned char), 2u));
_temp851[ 1u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null(
_temp796)); _temp851[ 0u]= Cyc_PP_text( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp851, sizeof( struct Cyc_PP_Doc*),
6u));});} goto _LL619; _LL685: s= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp798)); goto _LL619; _LL687: s=({ struct Cyc_PP_Doc* _temp852[ 3u]; _temp852[
2u]= Cyc_PP_text( _tag_arr(" })", sizeof( unsigned char), 4u)); _temp852[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp802)); _temp852[ 0u]= Cyc_PP_text(
_tag_arr("({ ", sizeof( unsigned char), 4u)); Cyc_PP_cat( _tag_arr( _temp852,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL619; _LL689: s=({ struct Cyc_PP_Doc*
_temp853[ 3u]; _temp853[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp853[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl*
_temp854=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp854->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp855=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp855[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp856; _temp856.tag= Cyc_Absyn_Fn_d;
_temp856.f1= _temp804; _temp856;}); _temp855;})); _temp854->loc= e->loc;
_temp854;}))); _temp853[ 0u]= Cyc_PP_text( _tag_arr("codegen(", sizeof(
unsigned char), 9u)); Cyc_PP_cat( _tag_arr( _temp853, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL619; _LL691: s=({ struct Cyc_PP_Doc* _temp857[ 3u]; _temp857[ 2u]=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); _temp857[ 1u]= Cyc_PP_nest(
2, Cyc_Absynpp_exp2doc( _temp806)); _temp857[ 0u]= Cyc_PP_text( _tag_arr("fill(",
sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp857, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL619; _LL619:;} if( inprec >=  myprec){ s=({ struct Cyc_PP_Doc*
_temp858[ 3u]; _temp858[ 2u]= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); _temp858[ 1u]= s; _temp858[ 0u]= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp858, sizeof( struct Cyc_PP_Doc*),
3u));});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){
void* _temp859= d; struct Cyc_Absyn_Exp* _temp865; struct _tagged_arr* _temp867;
_LL861: if(*(( int*) _temp859) ==  Cyc_Absyn_ArrayElement){ _LL866: _temp865=((
struct Cyc_Absyn_ArrayElement_struct*) _temp859)->f1; goto _LL862;} else{ goto
_LL863;} _LL863: if(*(( int*) _temp859) ==  Cyc_Absyn_FieldName){ _LL868:
_temp867=(( struct Cyc_Absyn_FieldName_struct*) _temp859)->f1; goto _LL864;}
else{ goto _LL860;} _LL862: return({ struct Cyc_PP_Doc* _temp869[ 3u]; _temp869[
2u]= Cyc_PP_text( _tag_arr("]", sizeof( unsigned char), 2u)); _temp869[ 1u]= Cyc_Absynpp_exp2doc(
_temp865); _temp869[ 0u]= Cyc_PP_text( _tag_arr(".[", sizeof( unsigned char), 3u));
Cyc_PP_cat( _tag_arr( _temp869, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL864:
return({ struct Cyc_PP_Doc* _temp870[ 2u]; _temp870[ 1u]= Cyc_PP_textptr(
_temp867); _temp870[ 0u]= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char), 2u));
Cyc_PP_cat( _tag_arr( _temp870, sizeof( struct Cyc_PP_Doc*), 2u));}); _LL860:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 ==  0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({ struct Cyc_PP_Doc*
_temp871[ 2u]; _temp871[ 1u]= Cyc_Absynpp_exp2doc((* de).f2); _temp871[ 0u]= Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof( unsigned char),
2u), _tag_arr("=", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); Cyc_PP_cat( _tag_arr( _temp871, sizeof( struct Cyc_PP_Doc*), 2u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp872= c;
unsigned char _temp890; void* _temp892; short _temp894; void* _temp896; int
_temp898; void* _temp900; int _temp902; void* _temp904; long long _temp906; void*
_temp908; struct _tagged_arr _temp910; struct _tagged_arr _temp912; _LL874: if((
unsigned int) _temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_Char_c: 0){
_LL893: _temp892=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp872)->f1; goto
_LL891; _LL891: _temp890=(( struct Cyc_Absyn_Char_c_struct*) _temp872)->f2; goto
_LL875;} else{ goto _LL876;} _LL876: if(( unsigned int) _temp872 >  1u?*(( int*)
_temp872) ==  Cyc_Absyn_Short_c: 0){ _LL897: _temp896=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp872)->f1; goto _LL895; _LL895: _temp894=(( struct Cyc_Absyn_Short_c_struct*)
_temp872)->f2; goto _LL877;} else{ goto _LL878;} _LL878: if(( unsigned int)
_temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_Int_c: 0){ _LL901: _temp900=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp872)->f1; if( _temp900 == ( void*)
Cyc_Absyn_Signed){ goto _LL899;} else{ goto _LL880;} _LL899: _temp898=(( struct
Cyc_Absyn_Int_c_struct*) _temp872)->f2; goto _LL879;} else{ goto _LL880;} _LL880:
if(( unsigned int) _temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_Int_c: 0){
_LL905: _temp904=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp872)->f1; if(
_temp904 == ( void*) Cyc_Absyn_Unsigned){ goto _LL903;} else{ goto _LL882;}
_LL903: _temp902=(( struct Cyc_Absyn_Int_c_struct*) _temp872)->f2; goto _LL881;}
else{ goto _LL882;} _LL882: if(( unsigned int) _temp872 >  1u?*(( int*) _temp872)
==  Cyc_Absyn_LongLong_c: 0){ _LL909: _temp908=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp872)->f1; goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp872)->f2; goto _LL883;} else{ goto _LL884;} _LL884: if(( unsigned int)
_temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_Float_c: 0){ _LL911: _temp910=((
struct Cyc_Absyn_Float_c_struct*) _temp872)->f1; goto _LL885;} else{ goto _LL886;}
_LL886: if( _temp872 == ( void*) Cyc_Absyn_Null_c){ goto _LL887;} else{ goto
_LL888;} _LL888: if(( unsigned int) _temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_String_c:
0){ _LL913: _temp912=(( struct Cyc_Absyn_String_c_struct*) _temp872)->f1; goto
_LL889;} else{ goto _LL873;} _LL875: return Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp915; _temp915.tag= Cyc_Stdio_String_pa;
_temp915.f1=( struct _tagged_arr) Cyc_Absynpp_char_escape( _temp890);{ void*
_temp914[ 1u]={& _temp915}; Cyc_Stdio_aprintf( _tag_arr("'%s'", sizeof(
unsigned char), 5u), _tag_arr( _temp914, sizeof( void*), 1u));}})); _LL877:
return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp917; _temp917.tag= Cyc_Stdio_Int_pa; _temp917.f1=( int)(( unsigned int)((
int) _temp894));{ void* _temp916[ 1u]={& _temp917}; Cyc_Stdio_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp916, sizeof( void*), 1u));}}));
_LL879: return Cyc_PP_text(( struct _tagged_arr) Cyc_Core_string_of_int(
_temp898)); _LL881: return Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp919; _temp919.tag= Cyc_Stdio_Int_pa; _temp919.f1=( unsigned int) _temp902;{
void* _temp918[ 1u]={& _temp919}; Cyc_Stdio_aprintf( _tag_arr("%u", sizeof(
unsigned char), 3u), _tag_arr( _temp918, sizeof( void*), 1u));}})); _LL883:
return Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char),
27u)); _LL885: return Cyc_PP_text( _temp910); _LL887: return Cyc_PP_text(
_tag_arr("NULL", sizeof( unsigned char), 5u)); _LL889: return({ struct Cyc_PP_Doc*
_temp920[ 3u]; _temp920[ 2u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char),
2u)); _temp920[ 1u]= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp912));
_temp920[ 0u]= Cyc_PP_text( _tag_arr("\"", sizeof( unsigned char), 2u)); Cyc_PP_cat(
_tag_arr( _temp920, sizeof( struct Cyc_PP_Doc*), 3u));}); _LL873:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == ( void*) Cyc_Absyn_Size){ if(
es ==  0? 1:(( struct Cyc_List_List*) _check_null( es))->tl !=  0){( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp921=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp921[ 0]=({ struct Cyc_Core_Failure_struct
_temp922; _temp922.tag= Cyc_Core_Failure; _temp922.f1=( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp924; _temp924.tag= Cyc_Stdio_String_pa;
_temp924.f1=( struct _tagged_arr) Cyc_PP_string_of_doc( ps, 72);{ void* _temp923[
1u]={& _temp924}; Cyc_Stdio_aprintf( _tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof( unsigned char), 44u), _tag_arr( _temp923, sizeof( void*), 1u));}});
_temp922;}); _temp921;}));}{ struct Cyc_PP_Doc* _temp925= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
return({ struct Cyc_PP_Doc* _temp926[ 2u]; _temp926[ 1u]= Cyc_PP_text( _tag_arr(".size",
sizeof( unsigned char), 6u)); _temp926[ 0u]= _temp925; Cyc_PP_cat( _tag_arr(
_temp926, sizeof( struct Cyc_PP_Doc*), 2u));});}} else{ struct Cyc_List_List* ds=((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds ==  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp927=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp927[ 0]=({ struct Cyc_Core_Failure_struct _temp928; _temp928.tag= Cyc_Core_Failure;
_temp928.f1=( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp930;
_temp930.tag= Cyc_Stdio_String_pa; _temp930.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp929[ 1u]={& _temp930}; Cyc_Stdio_aprintf( _tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof( unsigned char), 38u), _tag_arr( _temp929, sizeof( void*), 1u));}});
_temp928;}); _temp927;}));} else{ if((( struct Cyc_List_List*) _check_null( ds))->tl
==  0){ return({ struct Cyc_PP_Doc* _temp931[ 3u]; _temp931[ 2u]=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; _temp931[ 1u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp931[ 0u]= ps; Cyc_PP_cat(
_tag_arr( _temp931, sizeof( struct Cyc_PP_Doc*), 3u));});} else{ if((( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->tl
!=  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp932=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp932[ 0]=({ struct Cyc_Core_Failure_struct _temp933; _temp933.tag= Cyc_Core_Failure;
_temp933.f1=( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp935;
_temp935.tag= Cyc_Stdio_String_pa; _temp935.f1=( struct _tagged_arr) Cyc_PP_string_of_doc(
ps, 72);{ void* _temp934[ 1u]={& _temp935}; Cyc_Stdio_aprintf( _tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof( unsigned char), 47u), _tag_arr( _temp934, sizeof( void*), 1u));}});
_temp933;}); _temp932;}));} else{ return({ struct Cyc_PP_Doc* _temp936[ 5u];
_temp936[ 4u]=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( ds))->tl))->hd; _temp936[ 3u]= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); _temp936[ 2u]= ps; _temp936[ 1u]=
Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); _temp936[ 0u]=( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds))->hd; Cyc_PP_cat(
_tag_arr( _temp936, sizeof( struct Cyc_PP_Doc*), 5u));});}}}}} struct
_tagged_arr Cyc_Absynpp_prim2str( void* p){ void* _temp937= p; _LL939: if(
_temp937 == ( void*) Cyc_Absyn_Plus){ goto _LL940;} else{ goto _LL941;} _LL941:
if( _temp937 == ( void*) Cyc_Absyn_Times){ goto _LL942;} else{ goto _LL943;}
_LL943: if( _temp937 == ( void*) Cyc_Absyn_Minus){ goto _LL944;} else{ goto
_LL945;} _LL945: if( _temp937 == ( void*) Cyc_Absyn_Div){ goto _LL946;} else{
goto _LL947;} _LL947: if( _temp937 == ( void*) Cyc_Absyn_Mod){ goto _LL948;}
else{ goto _LL949;} _LL949: if( _temp937 == ( void*) Cyc_Absyn_Eq){ goto _LL950;}
else{ goto _LL951;} _LL951: if( _temp937 == ( void*) Cyc_Absyn_Neq){ goto _LL952;}
else{ goto _LL953;} _LL953: if( _temp937 == ( void*) Cyc_Absyn_Gt){ goto _LL954;}
else{ goto _LL955;} _LL955: if( _temp937 == ( void*) Cyc_Absyn_Lt){ goto _LL956;}
else{ goto _LL957;} _LL957: if( _temp937 == ( void*) Cyc_Absyn_Gte){ goto _LL958;}
else{ goto _LL959;} _LL959: if( _temp937 == ( void*) Cyc_Absyn_Lte){ goto _LL960;}
else{ goto _LL961;} _LL961: if( _temp937 == ( void*) Cyc_Absyn_Not){ goto _LL962;}
else{ goto _LL963;} _LL963: if( _temp937 == ( void*) Cyc_Absyn_Bitnot){ goto
_LL964;} else{ goto _LL965;} _LL965: if( _temp937 == ( void*) Cyc_Absyn_Bitand){
goto _LL966;} else{ goto _LL967;} _LL967: if( _temp937 == ( void*) Cyc_Absyn_Bitor){
goto _LL968;} else{ goto _LL969;} _LL969: if( _temp937 == ( void*) Cyc_Absyn_Bitxor){
goto _LL970;} else{ goto _LL971;} _LL971: if( _temp937 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL972;} else{ goto _LL973;} _LL973: if( _temp937 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL974;} else{ goto _LL975;} _LL975: if( _temp937 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL976;} else{ goto _LL977;} _LL977: if( _temp937 == ( void*) Cyc_Absyn_Size){
goto _LL978;} else{ goto _LL938;} _LL940: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL942: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL944: return _tag_arr("-", sizeof( unsigned char), 2u); _LL946: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL948: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL950: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL952: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL954: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL956: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL958: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL960: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL962: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL964: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL966: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL968: return _tag_arr("|", sizeof( unsigned char), 2u); _LL970: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL972: return _tag_arr("<<", sizeof(
unsigned char), 3u); _LL974: return _tag_arr(">>", sizeof( unsigned char), 3u);
_LL976: return _tag_arr(">>>", sizeof( unsigned char), 4u); _LL978: return
_tag_arr("size", sizeof( unsigned char), 5u); _LL938:;} struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc(
void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str( p));} struct _tuple9{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc(
struct _tuple9* t){ if((* t).f1 ==  0){ return Cyc_Absynpp_exp2doc((* t).f2);}
else{ return({ struct Cyc_PP_Doc* _temp979[ 3u]; _temp979[ 2u]= Cyc_Absynpp_exp2doc((*
t).f2); _temp979[ 1u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp979[ 0u]= Cyc_PP_textptr(( struct _tagged_arr*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v); Cyc_PP_cat( _tag_arr( _temp979, sizeof( struct Cyc_PP_Doc*),
3u));});}} int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void*
_temp980=( void*) s->r; _LL982: if(( unsigned int) _temp980 >  1u?*(( int*)
_temp980) ==  Cyc_Absyn_Decl_s: 0){ goto _LL983;} else{ goto _LL984;} _LL984:
goto _LL985; _LL983: return 1; _LL985: return 0; _LL981:;} struct Cyc_PP_Doc*
Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void*
_temp986=( void*) st->r; struct Cyc_Absyn_Exp* _temp1030; struct Cyc_Absyn_Stmt*
_temp1032; struct Cyc_Absyn_Stmt* _temp1034; struct Cyc_Absyn_Exp* _temp1036;
struct Cyc_Absyn_Stmt* _temp1038; struct Cyc_Absyn_Stmt* _temp1040; struct Cyc_Absyn_Exp*
_temp1042; struct Cyc_Absyn_Stmt* _temp1044; struct _tuple2 _temp1046; struct
Cyc_Absyn_Exp* _temp1048; struct _tagged_arr* _temp1050; struct Cyc_Absyn_Stmt*
_temp1052; struct _tuple2 _temp1054; struct Cyc_Absyn_Exp* _temp1056; struct
_tuple2 _temp1058; struct Cyc_Absyn_Exp* _temp1060; struct Cyc_Absyn_Exp*
_temp1062; struct Cyc_List_List* _temp1064; struct Cyc_Absyn_Exp* _temp1066;
struct Cyc_List_List* _temp1068; struct Cyc_Absyn_Exp* _temp1070; struct Cyc_List_List*
_temp1072; struct Cyc_List_List* _temp1074; struct Cyc_Absyn_Stmt* _temp1076;
struct Cyc_Absyn_Decl* _temp1078; struct Cyc_Absyn_Stmt* _temp1080; struct Cyc_Absyn_Stmt*
_temp1082; struct Cyc_Absyn_Stmt* _temp1084; struct _tagged_arr* _temp1086;
struct _tuple2 _temp1088; struct Cyc_Absyn_Exp* _temp1090; struct Cyc_Absyn_Stmt*
_temp1092; struct Cyc_List_List* _temp1094; struct Cyc_Absyn_Stmt* _temp1096;
struct Cyc_Absyn_Stmt* _temp1098; struct Cyc_Absyn_Vardecl* _temp1100; struct
Cyc_Absyn_Tvar* _temp1102; _LL988: if( _temp986 == ( void*) Cyc_Absyn_Skip_s){
goto _LL989;} else{ goto _LL990;} _LL990: if(( unsigned int) _temp986 >  1u?*((
int*) _temp986) ==  Cyc_Absyn_Exp_s: 0){ _LL1031: _temp1030=(( struct Cyc_Absyn_Exp_s_struct*)
_temp986)->f1; goto _LL991;} else{ goto _LL992;} _LL992: if(( unsigned int)
_temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_Seq_s: 0){ _LL1035: _temp1034=((
struct Cyc_Absyn_Seq_s_struct*) _temp986)->f1; goto _LL1033; _LL1033: _temp1032=((
struct Cyc_Absyn_Seq_s_struct*) _temp986)->f2; goto _LL993;} else{ goto _LL994;}
_LL994: if(( unsigned int) _temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_Return_s:
0){ _LL1037: _temp1036=(( struct Cyc_Absyn_Return_s_struct*) _temp986)->f1; goto
_LL995;} else{ goto _LL996;} _LL996: if(( unsigned int) _temp986 >  1u?*(( int*)
_temp986) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1043: _temp1042=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp986)->f1; goto _LL1041; _LL1041: _temp1040=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp986)->f2; goto _LL1039; _LL1039: _temp1038=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp986)->f3; goto _LL997;} else{ goto _LL998;} _LL998: if(( unsigned int)
_temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_While_s: 0){ _LL1047: _temp1046=((
struct Cyc_Absyn_While_s_struct*) _temp986)->f1; _LL1049: _temp1048= _temp1046.f1;
goto _LL1045; _LL1045: _temp1044=(( struct Cyc_Absyn_While_s_struct*) _temp986)->f2;
goto _LL999;} else{ goto _LL1000;} _LL1000: if(( unsigned int) _temp986 >  1u?*((
int*) _temp986) ==  Cyc_Absyn_Break_s: 0){ goto _LL1001;} else{ goto _LL1002;}
_LL1002: if(( unsigned int) _temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_Continue_s:
0){ goto _LL1003;} else{ goto _LL1004;} _LL1004: if(( unsigned int) _temp986 > 
1u?*(( int*) _temp986) ==  Cyc_Absyn_Goto_s: 0){ _LL1051: _temp1050=(( struct
Cyc_Absyn_Goto_s_struct*) _temp986)->f1; goto _LL1005;} else{ goto _LL1006;}
_LL1006: if(( unsigned int) _temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_For_s:
0){ _LL1063: _temp1062=(( struct Cyc_Absyn_For_s_struct*) _temp986)->f1; goto
_LL1059; _LL1059: _temp1058=(( struct Cyc_Absyn_For_s_struct*) _temp986)->f2;
_LL1061: _temp1060= _temp1058.f1; goto _LL1055; _LL1055: _temp1054=(( struct Cyc_Absyn_For_s_struct*)
_temp986)->f3; _LL1057: _temp1056= _temp1054.f1; goto _LL1053; _LL1053:
_temp1052=(( struct Cyc_Absyn_For_s_struct*) _temp986)->f4; goto _LL1007;} else{
goto _LL1008;} _LL1008: if(( unsigned int) _temp986 >  1u?*(( int*) _temp986) == 
Cyc_Absyn_Switch_s: 0){ _LL1067: _temp1066=(( struct Cyc_Absyn_Switch_s_struct*)
_temp986)->f1; goto _LL1065; _LL1065: _temp1064=(( struct Cyc_Absyn_Switch_s_struct*)
_temp986)->f2; goto _LL1009;} else{ goto _LL1010;} _LL1010: if(( unsigned int)
_temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_SwitchC_s: 0){ _LL1071:
_temp1070=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp986)->f1; goto _LL1069;
_LL1069: _temp1068=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp986)->f2; goto
_LL1011;} else{ goto _LL1012;} _LL1012: if(( unsigned int) _temp986 >  1u?*((
int*) _temp986) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1073: _temp1072=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp986)->f1; if( _temp1072 ==  0){ goto _LL1013;} else{ goto _LL1014;}} else{
goto _LL1014;} _LL1014: if(( unsigned int) _temp986 >  1u?*(( int*) _temp986) == 
Cyc_Absyn_Fallthru_s: 0){ _LL1075: _temp1074=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp986)->f1; goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( unsigned int)
_temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_Decl_s: 0){ _LL1079: _temp1078=((
struct Cyc_Absyn_Decl_s_struct*) _temp986)->f1; goto _LL1077; _LL1077: _temp1076=((
struct Cyc_Absyn_Decl_s_struct*) _temp986)->f2; goto _LL1017;} else{ goto
_LL1018;} _LL1018: if(( unsigned int) _temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_Cut_s:
0){ _LL1081: _temp1080=(( struct Cyc_Absyn_Cut_s_struct*) _temp986)->f1; goto
_LL1019;} else{ goto _LL1020;} _LL1020: if(( unsigned int) _temp986 >  1u?*((
int*) _temp986) ==  Cyc_Absyn_Splice_s: 0){ _LL1083: _temp1082=(( struct Cyc_Absyn_Splice_s_struct*)
_temp986)->f1; goto _LL1021;} else{ goto _LL1022;} _LL1022: if(( unsigned int)
_temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_Label_s: 0){ _LL1087: _temp1086=((
struct Cyc_Absyn_Label_s_struct*) _temp986)->f1; goto _LL1085; _LL1085:
_temp1084=(( struct Cyc_Absyn_Label_s_struct*) _temp986)->f2; goto _LL1023;}
else{ goto _LL1024;} _LL1024: if(( unsigned int) _temp986 >  1u?*(( int*)
_temp986) ==  Cyc_Absyn_Do_s: 0){ _LL1093: _temp1092=(( struct Cyc_Absyn_Do_s_struct*)
_temp986)->f1; goto _LL1089; _LL1089: _temp1088=(( struct Cyc_Absyn_Do_s_struct*)
_temp986)->f2; _LL1091: _temp1090= _temp1088.f1; goto _LL1025;} else{ goto
_LL1026;} _LL1026: if(( unsigned int) _temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_TryCatch_s:
0){ _LL1097: _temp1096=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp986)->f1;
goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp986)->f2; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(( unsigned int)
_temp986 >  1u?*(( int*) _temp986) ==  Cyc_Absyn_Region_s: 0){ _LL1103:
_temp1102=(( struct Cyc_Absyn_Region_s_struct*) _temp986)->f1; goto _LL1101;
_LL1101: _temp1100=(( struct Cyc_Absyn_Region_s_struct*) _temp986)->f2; goto
_LL1099; _LL1099: _temp1098=(( struct Cyc_Absyn_Region_s_struct*) _temp986)->f3;
goto _LL1029;} else{ goto _LL987;} _LL989: s= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); goto _LL987; _LL991: s=({ struct Cyc_PP_Doc* _temp1104[ 2u];
_temp1104[ 1u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
_temp1104[ 0u]= Cyc_Absynpp_exp2doc( _temp1030); Cyc_PP_cat( _tag_arr( _temp1104,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL987; _LL993: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1034)){ s=({ struct Cyc_PP_Doc* _temp1105[
6u]; _temp1105[ 5u]= Cyc_Absynpp_is_declaration( _temp1032)?({ struct Cyc_PP_Doc*
_temp1106[ 4u]; _temp1106[ 3u]= Cyc_PP_line_doc(); _temp1106[ 2u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1106[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1032)); _temp1106[ 0u]= Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1106, sizeof( struct Cyc_PP_Doc*), 4u));}): Cyc_Absynpp_stmt2doc(
_temp1032); _temp1105[ 4u]= Cyc_PP_line_doc(); _temp1105[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1105[ 2u]= Cyc_PP_line_doc();
_temp1105[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1034)); _temp1105[ 0u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); Cyc_PP_cat( _tag_arr(
_temp1105, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ if( Cyc_Absynpp_is_declaration(
_temp1032)){ s=({ struct Cyc_PP_Doc* _temp1107[ 6u]; _temp1107[ 5u]= Cyc_PP_line_doc();
_temp1107[ 4u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1107[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1032)); _temp1107[ 2u]=
Cyc_PP_text( _tag_arr("{ ", sizeof( unsigned char), 3u)); _temp1107[ 1u]= Cyc_PP_line_doc();
_temp1107[ 0u]= Cyc_Absynpp_stmt2doc( _temp1034); Cyc_PP_cat( _tag_arr(
_temp1107, sizeof( struct Cyc_PP_Doc*), 6u));});} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1108=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1108->hd=( void*) _temp1034;
_temp1108->tl=({ struct Cyc_List_List* _temp1109=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1109->hd=( void*) _temp1032;
_temp1109->tl= 0; _temp1109;}); _temp1108;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1110=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1110->hd=( void*) _temp1034;
_temp1110->tl=({ struct Cyc_List_List* _temp1111=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1111->hd=( void*) _temp1032;
_temp1111->tl= 0; _temp1111;}); _temp1110;}));} goto _LL987; _LL995: if(
_temp1036 ==  0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc* _temp1112[ 3u]; _temp1112[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1112[ 1u]= _temp1036 ==  0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1036)); _temp1112[
0u]= Cyc_PP_text( _tag_arr("return ", sizeof( unsigned char), 8u)); Cyc_PP_cat(
_tag_arr( _temp1112, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL987; _LL997: {
int print_else;{ void* _temp1113=( void*) _temp1038->r; _LL1115: if( _temp1113
== ( void*) Cyc_Absyn_Skip_s){ goto _LL1116;} else{ goto _LL1117;} _LL1117: goto
_LL1118; _LL1116: print_else= 0; goto _LL1114; _LL1118: print_else= 1; goto
_LL1114; _LL1114:;} s=({ struct Cyc_PP_Doc* _temp1119[ 7u]; _temp1119[ 6u]=
print_else?({ struct Cyc_PP_Doc* _temp1121[ 5u]; _temp1121[ 4u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1121[ 3u]= Cyc_PP_line_doc();
_temp1121[ 2u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1122[ 2u]; _temp1122[
1u]= Cyc_Absynpp_stmt2doc( _temp1038); _temp1122[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1122, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1121[ 1u]= Cyc_PP_text(
_tag_arr("else {", sizeof( unsigned char), 7u)); _temp1121[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1121, sizeof( struct Cyc_PP_Doc*), 5u));}): Cyc_PP_nil_doc();
_temp1119[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1119[ 4u]= Cyc_PP_line_doc(); _temp1119[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1120[ 2u]; _temp1120[ 1u]= Cyc_Absynpp_stmt2doc( _temp1040); _temp1120[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1120, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1119[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1119[ 1u]= Cyc_Absynpp_exp2doc( _temp1042); _temp1119[ 0u]= Cyc_PP_text(
_tag_arr("if (", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1119,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL987;} _LL999: s=({ struct Cyc_PP_Doc*
_temp1123[ 6u]; _temp1123[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1123[ 4u]= Cyc_PP_line_doc(); _temp1123[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1124[ 2u]; _temp1124[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1044); _temp1124[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1124,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1123[ 2u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1123[ 1u]= Cyc_Absynpp_exp2doc( _temp1048);
_temp1123[ 0u]= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u));
Cyc_PP_cat( _tag_arr( _temp1123, sizeof( struct Cyc_PP_Doc*), 6u));}); goto
_LL987; _LL1001: s= Cyc_PP_text( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL987; _LL1003: s= Cyc_PP_text( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL987; _LL1005: s= Cyc_PP_text(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp1126; _temp1126.tag= Cyc_Stdio_String_pa;
_temp1126.f1=( struct _tagged_arr)* _temp1050;{ void* _temp1125[ 1u]={&
_temp1126}; Cyc_Stdio_aprintf( _tag_arr("goto %s;", sizeof( unsigned char), 9u),
_tag_arr( _temp1125, sizeof( void*), 1u));}})); goto _LL987; _LL1007: s=({
struct Cyc_PP_Doc* _temp1127[ 10u]; _temp1127[ 9u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1127[ 8u]= Cyc_PP_line_doc(); _temp1127[ 7u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1128[ 2u]; _temp1128[ 1u]= Cyc_Absynpp_stmt2doc(
_temp1052); _temp1128[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1128,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1127[ 6u]= Cyc_PP_text( _tag_arr(") {",
sizeof( unsigned char), 4u)); _temp1127[ 5u]= Cyc_Absynpp_exp2doc( _temp1056);
_temp1127[ 4u]= Cyc_PP_text( _tag_arr("; ", sizeof( unsigned char), 3u));
_temp1127[ 3u]= Cyc_Absynpp_exp2doc( _temp1060); _temp1127[ 2u]= Cyc_PP_text(
_tag_arr("; ", sizeof( unsigned char), 3u)); _temp1127[ 1u]= Cyc_Absynpp_exp2doc(
_temp1062); _temp1127[ 0u]= Cyc_PP_text( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1127, sizeof( struct Cyc_PP_Doc*), 10u));});
goto _LL987; _LL1009: s=({ struct Cyc_PP_Doc* _temp1129[ 7u]; _temp1129[ 6u]=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1129[ 5u]= Cyc_PP_line_doc();
_temp1129[ 4u]= Cyc_Absynpp_switchclauses2doc( _temp1064); _temp1129[ 3u]= Cyc_PP_line_doc();
_temp1129[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1129[ 1u]= Cyc_Absynpp_exp2doc( _temp1066); _temp1129[ 0u]= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); Cyc_PP_cat( _tag_arr(
_temp1129, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL987; _LL1011: s=({
struct Cyc_PP_Doc* _temp1130[ 7u]; _temp1130[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1130[ 5u]= Cyc_PP_line_doc(); _temp1130[ 4u]=
Cyc_Absynpp_switchCclauses2doc( _temp1068); _temp1130[ 3u]= Cyc_PP_line_doc();
_temp1130[ 2u]= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u));
_temp1130[ 1u]= Cyc_Absynpp_exp2doc( _temp1070); _temp1130[ 0u]= Cyc_PP_text(
_tag_arr("switch \"C\" (", sizeof( unsigned char), 13u)); Cyc_PP_cat( _tag_arr(
_temp1130, sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL987; _LL1013: s= Cyc_PP_text(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL987; _LL1015: s=({
struct Cyc_PP_Doc* _temp1131[ 3u]; _temp1131[ 2u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1131[ 1u]= Cyc_Absynpp_exps2doc_prec( 20,
_temp1074); _temp1131[ 0u]= Cyc_PP_text( _tag_arr("fallthru(", sizeof(
unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1131, sizeof( struct Cyc_PP_Doc*),
3u));}); goto _LL987; _LL1017: s=({ struct Cyc_PP_Doc* _temp1132[ 3u]; _temp1132[
2u]= Cyc_Absynpp_stmt2doc( _temp1076); _temp1132[ 1u]= Cyc_PP_line_doc();
_temp1132[ 0u]= Cyc_Absynpp_decl2doc( _temp1078); Cyc_PP_cat( _tag_arr(
_temp1132, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL987; _LL1019: s=({
struct Cyc_PP_Doc* _temp1133[ 2u]; _temp1133[ 1u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1080)); _temp1133[ 0u]= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char),
5u)); Cyc_PP_cat( _tag_arr( _temp1133, sizeof( struct Cyc_PP_Doc*), 2u));});
goto _LL987; _LL1021: s=({ struct Cyc_PP_Doc* _temp1134[ 2u]; _temp1134[ 1u]=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1082)); _temp1134[ 0u]= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); Cyc_PP_cat( _tag_arr(
_temp1134, sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL987; _LL1023: if( Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration( _temp1084): 0){ s=({ struct Cyc_PP_Doc* _temp1135[ 6u];
_temp1135[ 5u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1135[ 4u]= Cyc_PP_line_doc(); _temp1135[ 3u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1084)); _temp1135[ 2u]= Cyc_PP_line_doc(); _temp1135[ 1u]= Cyc_PP_text(
_tag_arr(": {", sizeof( unsigned char), 4u)); _temp1135[ 0u]= Cyc_PP_textptr(
_temp1086); Cyc_PP_cat( _tag_arr( _temp1135, sizeof( struct Cyc_PP_Doc*), 6u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1136[ 3u]; _temp1136[ 2u]= Cyc_Absynpp_stmt2doc(
_temp1084); _temp1136[ 1u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); _temp1136[ 0u]= Cyc_PP_textptr( _temp1086); Cyc_PP_cat( _tag_arr(
_temp1136, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL987; _LL1025: s=({
struct Cyc_PP_Doc* _temp1137[ 7u]; _temp1137[ 6u]= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); _temp1137[ 5u]= Cyc_Absynpp_exp2doc( _temp1090);
_temp1137[ 4u]= Cyc_PP_text( _tag_arr("} while (", sizeof( unsigned char), 10u));
_temp1137[ 3u]= Cyc_PP_line_doc(); _temp1137[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1092)); _temp1137[ 1u]= Cyc_PP_line_doc(); _temp1137[ 0u]= Cyc_PP_text(
_tag_arr("do {", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1137,
sizeof( struct Cyc_PP_Doc*), 7u));}); goto _LL987; _LL1027: s=({ struct Cyc_PP_Doc*
_temp1138[ 9u]; _temp1138[ 8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); _temp1138[ 7u]= Cyc_PP_line_doc(); _temp1138[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1094)); _temp1138[ 5u]= Cyc_PP_line_doc(); _temp1138[ 4u]= Cyc_PP_text(
_tag_arr("} catch {", sizeof( unsigned char), 10u)); _temp1138[ 3u]= Cyc_PP_line_doc();
_temp1138[ 2u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1096)); _temp1138[ 1u]=
Cyc_PP_line_doc(); _temp1138[ 0u]= Cyc_PP_text( _tag_arr("try {", sizeof(
unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1138, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL987; _LL1029: s=({ struct Cyc_PP_Doc* _temp1139[ 9u]; _temp1139[
8u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); _temp1139[ 7u]=
Cyc_PP_line_doc(); _temp1139[ 6u]= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1098)); _temp1139[ 5u]= Cyc_PP_line_doc(); _temp1139[ 4u]= Cyc_PP_text(
_tag_arr("{", sizeof( unsigned char), 2u)); _temp1139[ 3u]= Cyc_Absynpp_qvar2doc(
_temp1100->name); _temp1139[ 2u]= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); _temp1139[ 1u]= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1102)); _temp1139[ 0u]= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); Cyc_PP_cat( _tag_arr( _temp1139, sizeof( struct Cyc_PP_Doc*),
9u));}); goto _LL987; _LL987:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1140=( void*) p->r;
int _temp1172; void* _temp1174; unsigned char _temp1176; struct _tagged_arr
_temp1178; struct Cyc_Absyn_Vardecl* _temp1180; struct Cyc_List_List* _temp1182;
struct Cyc_Absyn_Pat* _temp1184; struct Cyc_Absyn_Vardecl* _temp1186; struct
_tuple0* _temp1188; struct Cyc_List_List* _temp1190; struct Cyc_List_List*
_temp1192; struct _tuple0* _temp1194; struct Cyc_List_List* _temp1196; struct
Cyc_List_List* _temp1198; struct _tuple0* _temp1200; struct Cyc_List_List*
_temp1202; struct Cyc_List_List* _temp1204; struct Cyc_Absyn_Structdecl*
_temp1206; struct Cyc_Absyn_Enumfield* _temp1208; struct Cyc_List_List*
_temp1210; struct Cyc_List_List* _temp1212; struct Cyc_Absyn_Tunionfield*
_temp1214; _LL1142: if( _temp1140 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1143;}
else{ goto _LL1144;} _LL1144: if( _temp1140 == ( void*) Cyc_Absyn_Null_p){ goto
_LL1145;} else{ goto _LL1146;} _LL1146: if(( unsigned int) _temp1140 >  2u?*((
int*) _temp1140) ==  Cyc_Absyn_Int_p: 0){ _LL1175: _temp1174=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1140)->f1; goto _LL1173; _LL1173: _temp1172=((
struct Cyc_Absyn_Int_p_struct*) _temp1140)->f2; goto _LL1147;} else{ goto
_LL1148;} _LL1148: if(( unsigned int) _temp1140 >  2u?*(( int*) _temp1140) == 
Cyc_Absyn_Char_p: 0){ _LL1177: _temp1176=(( struct Cyc_Absyn_Char_p_struct*)
_temp1140)->f1; goto _LL1149;} else{ goto _LL1150;} _LL1150: if(( unsigned int)
_temp1140 >  2u?*(( int*) _temp1140) ==  Cyc_Absyn_Float_p: 0){ _LL1179:
_temp1178=(( struct Cyc_Absyn_Float_p_struct*) _temp1140)->f1; goto _LL1151;}
else{ goto _LL1152;} _LL1152: if(( unsigned int) _temp1140 >  2u?*(( int*)
_temp1140) ==  Cyc_Absyn_Var_p: 0){ _LL1181: _temp1180=(( struct Cyc_Absyn_Var_p_struct*)
_temp1140)->f1; goto _LL1153;} else{ goto _LL1154;} _LL1154: if(( unsigned int)
_temp1140 >  2u?*(( int*) _temp1140) ==  Cyc_Absyn_Tuple_p: 0){ _LL1183:
_temp1182=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1140)->f1; goto _LL1155;}
else{ goto _LL1156;} _LL1156: if(( unsigned int) _temp1140 >  2u?*(( int*)
_temp1140) ==  Cyc_Absyn_Pointer_p: 0){ _LL1185: _temp1184=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1140)->f1; goto _LL1157;} else{ goto _LL1158;} _LL1158: if(( unsigned int)
_temp1140 >  2u?*(( int*) _temp1140) ==  Cyc_Absyn_Reference_p: 0){ _LL1187:
_temp1186=(( struct Cyc_Absyn_Reference_p_struct*) _temp1140)->f1; goto _LL1159;}
else{ goto _LL1160;} _LL1160: if(( unsigned int) _temp1140 >  2u?*(( int*)
_temp1140) ==  Cyc_Absyn_UnknownId_p: 0){ _LL1189: _temp1188=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1140)->f1; goto _LL1161;} else{ goto _LL1162;} _LL1162: if(( unsigned int)
_temp1140 >  2u?*(( int*) _temp1140) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL1195:
_temp1194=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1140)->f1; goto
_LL1193; _LL1193: _temp1192=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1140)->f2;
goto _LL1191; _LL1191: _temp1190=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1140)->f3; goto _LL1163;} else{ goto _LL1164;} _LL1164: if(( unsigned int)
_temp1140 >  2u?*(( int*) _temp1140) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL1201:
_temp1200=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1140)->f1; goto
_LL1199; _LL1199: _temp1198=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1140)->f2; goto _LL1197; _LL1197: _temp1196=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1140)->f3; goto _LL1165;} else{ goto _LL1166;} _LL1166: if(( unsigned int)
_temp1140 >  2u?*(( int*) _temp1140) ==  Cyc_Absyn_Struct_p: 0){ _LL1207:
_temp1206=(( struct Cyc_Absyn_Struct_p_struct*) _temp1140)->f1; goto _LL1205;
_LL1205: _temp1204=(( struct Cyc_Absyn_Struct_p_struct*) _temp1140)->f3; goto
_LL1203; _LL1203: _temp1202=(( struct Cyc_Absyn_Struct_p_struct*) _temp1140)->f4;
goto _LL1167;} else{ goto _LL1168;} _LL1168: if(( unsigned int) _temp1140 >  2u?*((
int*) _temp1140) ==  Cyc_Absyn_Enum_p: 0){ _LL1209: _temp1208=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1140)->f2; goto _LL1169;} else{ goto _LL1170;} _LL1170: if(( unsigned int)
_temp1140 >  2u?*(( int*) _temp1140) ==  Cyc_Absyn_Tunion_p: 0){ _LL1215:
_temp1214=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1140)->f2; goto _LL1213;
_LL1213: _temp1212=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1140)->f3; goto
_LL1211; _LL1211: _temp1210=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1140)->f4;
goto _LL1171;} else{ goto _LL1141;} _LL1143: s= Cyc_PP_text( _tag_arr("_",
sizeof( unsigned char), 2u)); goto _LL1141; _LL1145: s= Cyc_PP_text( _tag_arr("NULL",
sizeof( unsigned char), 5u)); goto _LL1141; _LL1147: if( _temp1174 == ( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp1217; _temp1217.tag= Cyc_Stdio_Int_pa; _temp1217.f1=( int)(( unsigned int)
_temp1172);{ void* _temp1216[ 1u]={& _temp1217}; Cyc_Stdio_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp1216, sizeof( void*), 1u));}}));}
else{ s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp1219; _temp1219.tag= Cyc_Stdio_Int_pa; _temp1219.f1=( unsigned int)
_temp1172;{ void* _temp1218[ 1u]={& _temp1219}; Cyc_Stdio_aprintf( _tag_arr("%u",
sizeof( unsigned char), 3u), _tag_arr( _temp1218, sizeof( void*), 1u));}}));}
goto _LL1141; _LL1149: s= Cyc_PP_text(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp1221; _temp1221.tag= Cyc_Stdio_String_pa; _temp1221.f1=( struct _tagged_arr)
Cyc_Absynpp_char_escape( _temp1176);{ void* _temp1220[ 1u]={& _temp1221}; Cyc_Stdio_aprintf(
_tag_arr("'%s'", sizeof( unsigned char), 5u), _tag_arr( _temp1220, sizeof( void*),
1u));}})); goto _LL1141; _LL1151: s= Cyc_PP_text( _temp1178); goto _LL1141;
_LL1153: s= Cyc_Absynpp_qvar2doc( _temp1180->name); goto _LL1141; _LL1155: s=({
struct Cyc_PP_Doc* _temp1222[ 3u]; _temp1222[ 2u]= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); _temp1222[ 1u]=(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc, _tag_arr(",", sizeof( unsigned char), 2u),
_temp1182); _temp1222[ 0u]= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); Cyc_PP_cat( _tag_arr( _temp1222, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1141; _LL1157: s=({ struct Cyc_PP_Doc* _temp1223[ 2u]; _temp1223[ 1u]=
Cyc_Absynpp_pat2doc( _temp1184); _temp1223[ 0u]= Cyc_PP_text( _tag_arr("&",
sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1223, sizeof( struct
Cyc_PP_Doc*), 2u));}); goto _LL1141; _LL1159: s=({ struct Cyc_PP_Doc* _temp1224[
2u]; _temp1224[ 1u]= Cyc_Absynpp_qvar2doc( _temp1186->name); _temp1224[ 0u]= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_PP_cat( _tag_arr( _temp1224,
sizeof( struct Cyc_PP_Doc*), 2u));}); goto _LL1141; _LL1161: s= Cyc_Absynpp_qvar2doc(
_temp1188); goto _LL1141; _LL1163: s=({ struct Cyc_PP_Doc* _temp1225[ 3u];
_temp1225[ 2u]= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp1190));
_temp1225[ 1u]= Cyc_Absynpp_tvars2doc( _temp1192); _temp1225[ 0u]= Cyc_Absynpp_qvar2doc(
_temp1194); Cyc_PP_cat( _tag_arr( _temp1225, sizeof( struct Cyc_PP_Doc*), 3u));});
goto _LL1141; _LL1165: s=({ struct Cyc_PP_Doc* _temp1226[ 3u]; _temp1226[ 2u]=
Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1196)); _temp1226[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1198); _temp1226[ 0u]= Cyc_Absynpp_qvar2doc( _temp1200); Cyc_PP_cat(
_tag_arr( _temp1226, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1141; _LL1167:
s=({ struct Cyc_PP_Doc* _temp1227[ 3u]; _temp1227[ 2u]= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1202)); _temp1227[ 1u]= Cyc_Absynpp_tvars2doc( _temp1204); _temp1227[ 0u]=
_temp1206->name ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1206->name))->v); Cyc_PP_cat( _tag_arr(
_temp1227, sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1141; _LL1169: s= Cyc_Absynpp_qvar2doc(
_temp1208->name); goto _LL1141; _LL1171: if( _temp1210 ==  0){ s= Cyc_Absynpp_qvar2doc(
_temp1214->name);} else{ s=({ struct Cyc_PP_Doc* _temp1228[ 3u]; _temp1228[ 2u]=
Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1210)); _temp1228[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1212); _temp1228[ 0u]= Cyc_Absynpp_qvar2doc( _temp1214->name); Cyc_PP_cat(
_tag_arr( _temp1228, sizeof( struct Cyc_PP_Doc*), 3u));});} goto _LL1141;
_LL1141:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc* _temp1229[ 2u]; _temp1229[ 1u]= Cyc_Absynpp_pat2doc((*
dp).f2); _temp1229[ 0u]= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1)); Cyc_PP_cat( _tag_arr(
_temp1229, sizeof( struct Cyc_PP_Doc*), 2u));});} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause ==  0?( void*)( c->pattern)->r
== ( void*) Cyc_Absyn_Wild_p: 0){ return({ struct Cyc_PP_Doc* _temp1230[ 2u];
_temp1230[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1231[ 2u]; _temp1231[
1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1231[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1231, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1230[ 0u]= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1230, sizeof( struct Cyc_PP_Doc*), 2u));});} else{ if( c->where_clause == 
0){ return({ struct Cyc_PP_Doc* _temp1232[ 4u]; _temp1232[ 3u]= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc* _temp1233[ 2u]; _temp1233[ 1u]= Cyc_Absynpp_stmt2doc( c->body);
_temp1233[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1233, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1232[ 2u]= Cyc_PP_text( _tag_arr(": ",
sizeof( unsigned char), 3u)); _temp1232[ 1u]= Cyc_Absynpp_pat2doc( c->pattern);
_temp1232[ 0u]= Cyc_PP_text( _tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat(
_tag_arr( _temp1232, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ return({
struct Cyc_PP_Doc* _temp1234[ 6u]; _temp1234[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1235[ 2u]; _temp1235[ 1u]= Cyc_Absynpp_stmt2doc( c->body); _temp1235[ 0u]=
Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1235, sizeof( struct Cyc_PP_Doc*),
2u));})); _temp1234[ 4u]= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
_temp1234[ 3u]= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( c->where_clause));
_temp1234[ 2u]= Cyc_PP_text( _tag_arr(" && ", sizeof( unsigned char), 5u));
_temp1234[ 1u]= Cyc_Absynpp_pat2doc( c->pattern); _temp1234[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1234,
sizeof( struct Cyc_PP_Doc*), 6u));});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1238;
struct Cyc_Absyn_Stmt* _temp1239; struct Cyc_Absyn_Exp* _temp1241; struct Cyc_Absyn_SwitchC_clause*
_temp1236= c; _temp1238=* _temp1236; _LL1242: _temp1241= _temp1238.cnst_exp;
goto _LL1240; _LL1240: _temp1239= _temp1238.body; goto _LL1237; _LL1237: if(
_temp1241 ==  0){ return({ struct Cyc_PP_Doc* _temp1243[ 2u]; _temp1243[ 1u]=
Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1244[ 2u]; _temp1244[ 1u]= Cyc_Absynpp_stmt2doc(
c->body); _temp1244[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1244,
sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1243[ 0u]= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr( _temp1243, sizeof( struct
Cyc_PP_Doc*), 2u));});} else{ return({ struct Cyc_PP_Doc* _temp1245[ 4u];
_temp1245[ 3u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1246[ 2u]; _temp1246[
1u]= Cyc_Absynpp_stmt2doc( _temp1239); _temp1246[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat(
_tag_arr( _temp1246, sizeof( struct Cyc_PP_Doc*), 2u));})); _temp1245[ 2u]= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); _temp1245[ 1u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1241)); _temp1245[ 0u]= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); Cyc_PP_cat( _tag_arr( _temp1245,
sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag ==  0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc* _temp1247[ 3u]; _temp1247[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->tag)); _temp1247[ 1u]= Cyc_PP_text(
_tag_arr(" = ", sizeof( unsigned char), 4u)); _temp1247[ 0u]= Cyc_Absynpp_qvar2doc(
f->name); Cyc_PP_cat( _tag_arr( _temp1247, sizeof( struct Cyc_PP_Doc*), 3u));});}}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fs){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,
_tag_arr(",", sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1248=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1274; struct Cyc_Absyn_Structdecl* _temp1276;
struct Cyc_Absyn_Uniondecl* _temp1278; struct Cyc_Absyn_Vardecl* _temp1280;
struct Cyc_Absyn_Vardecl _temp1282; struct Cyc_List_List* _temp1283; struct Cyc_Absyn_Exp*
_temp1285; void* _temp1287; struct Cyc_Absyn_Tqual _temp1289; struct _tuple0*
_temp1291; void* _temp1293; struct Cyc_Absyn_Tuniondecl* _temp1295; struct Cyc_Absyn_Tuniondecl
_temp1297; int _temp1298; struct Cyc_Core_Opt* _temp1300; struct Cyc_List_List*
_temp1302; struct _tuple0* _temp1304; void* _temp1306; struct Cyc_Absyn_Exp*
_temp1308; struct Cyc_Absyn_Pat* _temp1310; struct Cyc_List_List* _temp1312;
struct Cyc_Absyn_Enumdecl* _temp1314; struct Cyc_Absyn_Enumdecl _temp1316;
struct Cyc_Core_Opt* _temp1317; struct _tuple0* _temp1319; void* _temp1321;
struct Cyc_Absyn_Typedefdecl* _temp1323; struct Cyc_List_List* _temp1325; struct
_tagged_arr* _temp1327; struct Cyc_List_List* _temp1329; struct _tuple0*
_temp1331; struct Cyc_List_List* _temp1333; _LL1250: if(*(( int*) _temp1248) == 
Cyc_Absyn_Fn_d){ _LL1275: _temp1274=(( struct Cyc_Absyn_Fn_d_struct*) _temp1248)->f1;
goto _LL1251;} else{ goto _LL1252;} _LL1252: if(*(( int*) _temp1248) ==  Cyc_Absyn_Struct_d){
_LL1277: _temp1276=(( struct Cyc_Absyn_Struct_d_struct*) _temp1248)->f1; goto
_LL1253;} else{ goto _LL1254;} _LL1254: if(*(( int*) _temp1248) ==  Cyc_Absyn_Union_d){
_LL1279: _temp1278=(( struct Cyc_Absyn_Union_d_struct*) _temp1248)->f1; goto
_LL1255;} else{ goto _LL1256;} _LL1256: if(*(( int*) _temp1248) ==  Cyc_Absyn_Var_d){
_LL1281: _temp1280=(( struct Cyc_Absyn_Var_d_struct*) _temp1248)->f1; _temp1282=*
_temp1280; _LL1294: _temp1293=( void*) _temp1282.sc; goto _LL1292; _LL1292:
_temp1291= _temp1282.name; goto _LL1290; _LL1290: _temp1289= _temp1282.tq; goto
_LL1288; _LL1288: _temp1287=( void*) _temp1282.type; goto _LL1286; _LL1286:
_temp1285= _temp1282.initializer; goto _LL1284; _LL1284: _temp1283= _temp1282.attributes;
goto _LL1257;} else{ goto _LL1258;} _LL1258: if(*(( int*) _temp1248) ==  Cyc_Absyn_Tunion_d){
_LL1296: _temp1295=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1248)->f1;
_temp1297=* _temp1295; _LL1307: _temp1306=( void*) _temp1297.sc; goto _LL1305;
_LL1305: _temp1304= _temp1297.name; goto _LL1303; _LL1303: _temp1302= _temp1297.tvs;
goto _LL1301; _LL1301: _temp1300= _temp1297.fields; goto _LL1299; _LL1299:
_temp1298= _temp1297.is_xtunion; goto _LL1259;} else{ goto _LL1260;} _LL1260:
if(*(( int*) _temp1248) ==  Cyc_Absyn_Let_d){ _LL1311: _temp1310=(( struct Cyc_Absyn_Let_d_struct*)
_temp1248)->f1; goto _LL1309; _LL1309: _temp1308=(( struct Cyc_Absyn_Let_d_struct*)
_temp1248)->f4; goto _LL1261;} else{ goto _LL1262;} _LL1262: if(*(( int*)
_temp1248) ==  Cyc_Absyn_Letv_d){ _LL1313: _temp1312=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1248)->f1; goto _LL1263;} else{ goto _LL1264;} _LL1264: if(*(( int*)
_temp1248) ==  Cyc_Absyn_Enum_d){ _LL1315: _temp1314=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1248)->f1; _temp1316=* _temp1314; _LL1322: _temp1321=( void*) _temp1316.sc;
goto _LL1320; _LL1320: _temp1319= _temp1316.name; goto _LL1318; _LL1318:
_temp1317= _temp1316.fields; goto _LL1265;} else{ goto _LL1266;} _LL1266: if(*((
int*) _temp1248) ==  Cyc_Absyn_Typedef_d){ _LL1324: _temp1323=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1248)->f1; goto _LL1267;} else{ goto _LL1268;} _LL1268: if(*(( int*)
_temp1248) ==  Cyc_Absyn_Namespace_d){ _LL1328: _temp1327=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1248)->f1; goto _LL1326; _LL1326: _temp1325=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1248)->f2; goto _LL1269;} else{ goto _LL1270;} _LL1270: if(*(( int*)
_temp1248) ==  Cyc_Absyn_Using_d){ _LL1332: _temp1331=(( struct Cyc_Absyn_Using_d_struct*)
_temp1248)->f1; goto _LL1330; _LL1330: _temp1329=(( struct Cyc_Absyn_Using_d_struct*)
_temp1248)->f2; goto _LL1271;} else{ goto _LL1272;} _LL1272: if(*(( int*)
_temp1248) ==  Cyc_Absyn_ExternC_d){ _LL1334: _temp1333=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1248)->f1; goto _LL1273;} else{ goto _LL1249;} _LL1251: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp1342=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1342[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1343; _temp1343.tag= Cyc_Absyn_FnType; _temp1343.f1=({ struct Cyc_Absyn_FnInfo
_temp1344; _temp1344.tvars= _temp1274->tvs; _temp1344.effect= _temp1274->effect;
_temp1344.ret_typ=( void*)(( void*) _temp1274->ret_type); _temp1344.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1274->args); _temp1344.c_varargs=
_temp1274->c_varargs; _temp1344.cyc_varargs= _temp1274->cyc_varargs; _temp1344.rgn_po=
_temp1274->rgn_po; _temp1344.attributes= 0; _temp1344;}); _temp1343;});
_temp1342;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1274->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1274->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1274->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1274->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1274->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1339; _temp1339.q_const= 0; _temp1339.q_volatile= 0; _temp1339.q_restrict=
0; _temp1339;}), t,({ struct Cyc_Core_Opt* _temp1340=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1340->v=( void*)({ struct Cyc_PP_Doc*
_temp1341[ 2u]; _temp1341[ 1u]= namedoc; _temp1341[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1341, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1340;})); struct
Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc* _temp1337[ 4u]; _temp1337[ 3u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1337[ 2u]= Cyc_PP_line_doc();
_temp1337[ 1u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc* _temp1338[ 2u]; _temp1338[
1u]= Cyc_Absynpp_stmt2doc( _temp1274->body); _temp1338[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1338, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1337[ 0u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); Cyc_PP_cat(
_tag_arr( _temp1337, sizeof( struct Cyc_PP_Doc*), 4u));}); s=({ struct Cyc_PP_Doc*
_temp1335[ 4u]; _temp1335[ 3u]= bodydoc; _temp1335[ 2u]= tqtddoc; _temp1335[ 1u]=
scopedoc; _temp1335[ 0u]= inlinedoc; Cyc_PP_cat( _tag_arr( _temp1335, sizeof(
struct Cyc_PP_Doc*), 4u));}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*
_temp1336[ 2u]; _temp1336[ 1u]= s; _temp1336[ 0u]= attsdoc; Cyc_PP_cat( _tag_arr(
_temp1336, sizeof( struct Cyc_PP_Doc*), 2u));});} goto _LL1249;}} _LL1253: if(
_temp1276->fields ==  0){ s=({ struct Cyc_PP_Doc* _temp1345[ 5u]; _temp1345[ 4u]=
Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); _temp1345[ 3u]= Cyc_Absynpp_ktvars2doc(
_temp1276->tvs); _temp1345[ 2u]= _temp1276->name ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1276->name))->v); _temp1345[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1345[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1276->sc); Cyc_PP_cat( _tag_arr( _temp1345, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ s=({ struct Cyc_PP_Doc* _temp1346[ 10u]; _temp1346[ 9u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1346[ 8u]= Cyc_Absynpp_atts2doc(
_temp1276->attributes); _temp1346[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); _temp1346[ 6u]= Cyc_PP_line_doc(); _temp1346[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1347[ 2u]; _temp1347[ 1u]= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1276->fields))->v);
_temp1347[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1347, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1346[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1346[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1276->tvs);
_temp1346[ 2u]= _temp1276->name ==  0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1276->name))->v); _temp1346[ 1u]= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); _temp1346[ 0u]= Cyc_Absynpp_scope2doc((
void*) _temp1276->sc); Cyc_PP_cat( _tag_arr( _temp1346, sizeof( struct Cyc_PP_Doc*),
10u));});} goto _LL1249; _LL1255: if( _temp1278->fields ==  0){ s=({ struct Cyc_PP_Doc*
_temp1348[ 5u]; _temp1348[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1348[ 3u]= Cyc_Absynpp_tvars2doc( _temp1278->tvs); _temp1348[ 2u]=
_temp1278->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1278->name))->v); _temp1348[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1348[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1278->sc);
Cyc_PP_cat( _tag_arr( _temp1348, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({
struct Cyc_PP_Doc* _temp1349[ 10u]; _temp1349[ 9u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1349[ 8u]= Cyc_Absynpp_atts2doc( _temp1278->attributes);
_temp1349[ 7u]= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u));
_temp1349[ 6u]= Cyc_PP_line_doc(); _temp1349[ 5u]= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*
_temp1350[ 2u]; _temp1350[ 1u]= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1278->fields))->v); _temp1350[ 0u]= Cyc_PP_line_doc();
Cyc_PP_cat( _tag_arr( _temp1350, sizeof( struct Cyc_PP_Doc*), 2u));}));
_temp1349[ 4u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1349[ 3u]= Cyc_Absynpp_tvars2doc( _temp1278->tvs); _temp1349[ 2u]=
_temp1278->name ==  0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):
Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1278->name))->v); _temp1349[ 1u]= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); _temp1349[ 0u]= Cyc_Absynpp_scope2doc(( void*) _temp1278->sc);
Cyc_PP_cat( _tag_arr( _temp1349, sizeof( struct Cyc_PP_Doc*), 10u));});} goto
_LL1249; _LL1257: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp1291); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1283); struct
Cyc_PP_Doc* beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;}
else{ void* _temp1351= Cyc_Tcutil_compress( _temp1287); struct Cyc_Absyn_FnInfo
_temp1357; struct Cyc_List_List* _temp1359; _LL1353: if(( unsigned int)
_temp1351 >  4u?*(( int*) _temp1351) ==  Cyc_Absyn_FnType: 0){ _LL1358:
_temp1357=(( struct Cyc_Absyn_FnType_struct*) _temp1351)->f1; _LL1360: _temp1359=
_temp1357.attributes; goto _LL1354;} else{ goto _LL1355;} _LL1355: goto _LL1356;
_LL1354: beforenamedoc= Cyc_Absynpp_callconv2doc( _temp1359); goto _LL1352;
_LL1356: beforenamedoc= Cyc_PP_nil_doc(); goto _LL1352; _LL1352:;} s=({ struct
Cyc_PP_Doc* _temp1361[ 5u]; _temp1361[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); _temp1361[ 3u]= _temp1285 ==  0? Cyc_PP_text( _tag_arr("",
sizeof( unsigned char), 1u)):({ struct Cyc_PP_Doc* _temp1364[ 2u]; _temp1364[ 1u]=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1285)); _temp1364[
0u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_PP_cat(
_tag_arr( _temp1364, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1361[ 2u]= Cyc_Absynpp_tqtd2doc(
_temp1289, _temp1287,({ struct Cyc_Core_Opt* _temp1362=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1362->v=( void*)({ struct Cyc_PP_Doc*
_temp1363[ 2u]; _temp1363[ 1u]= sn; _temp1363[ 0u]= beforenamedoc; Cyc_PP_cat(
_tag_arr( _temp1363, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1362;}));
_temp1361[ 1u]= Cyc_Absynpp_scope2doc( _temp1293); _temp1361[ 0u]= Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc(); Cyc_PP_cat( _tag_arr( _temp1361, sizeof( struct Cyc_PP_Doc*),
5u));}); goto _LL1249;} _LL1259: if( _temp1300 ==  0){ s=({ struct Cyc_PP_Doc*
_temp1365[ 5u]; _temp1365[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1365[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1302); _temp1365[ 2u]=
_temp1298? Cyc_Absynpp_qvar2doc( _temp1304): Cyc_Absynpp_typedef_name2doc(
_temp1304); _temp1365[ 1u]= _temp1298? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1365[ 0u]= Cyc_Absynpp_scope2doc( _temp1306); Cyc_PP_cat( _tag_arr(
_temp1365, sizeof( struct Cyc_PP_Doc*), 5u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1366[ 8u]; _temp1366[ 7u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1366[ 6u]= Cyc_PP_line_doc(); _temp1366[ 5u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1367[ 2u]; _temp1367[ 1u]= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1300))->v);
_temp1367[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1367, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1366[ 4u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1366[ 3u]= Cyc_Absynpp_ktvars2doc( _temp1302);
_temp1366[ 2u]= _temp1298? Cyc_Absynpp_qvar2doc( _temp1304): Cyc_Absynpp_typedef_name2doc(
_temp1304); _temp1366[ 1u]= _temp1298? Cyc_PP_text( _tag_arr("xtunion ", sizeof(
unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof( unsigned char), 8u));
_temp1366[ 0u]= Cyc_Absynpp_scope2doc( _temp1306); Cyc_PP_cat( _tag_arr(
_temp1366, sizeof( struct Cyc_PP_Doc*), 8u));});} goto _LL1249; _LL1261: s=({
struct Cyc_PP_Doc* _temp1368[ 5u]; _temp1368[ 4u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1368[ 3u]= Cyc_Absynpp_exp2doc( _temp1308);
_temp1368[ 2u]= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
_temp1368[ 1u]= Cyc_Absynpp_pat2doc( _temp1310); _temp1368[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1368,
sizeof( struct Cyc_PP_Doc*), 5u));}); goto _LL1249; _LL1263: s=({ struct Cyc_PP_Doc*
_temp1369[ 3u]; _temp1369[ 2u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1369[ 1u]= Cyc_Absynpp_ids2doc( _temp1312); _temp1369[ 0u]= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); Cyc_PP_cat( _tag_arr( _temp1369,
sizeof( struct Cyc_PP_Doc*), 3u));}); goto _LL1249; _LL1265: if( _temp1317 ==  0){
s=({ struct Cyc_PP_Doc* _temp1370[ 4u]; _temp1370[ 3u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1370[ 2u]= Cyc_Absynpp_typedef_name2doc(
_temp1319); _temp1370[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char),
6u)); _temp1370[ 0u]= Cyc_Absynpp_scope2doc( _temp1321); Cyc_PP_cat( _tag_arr(
_temp1370, sizeof( struct Cyc_PP_Doc*), 4u));});} else{ s=({ struct Cyc_PP_Doc*
_temp1371[ 7u]; _temp1371[ 6u]= Cyc_PP_text( _tag_arr("};", sizeof(
unsigned char), 3u)); _temp1371[ 5u]= Cyc_PP_line_doc(); _temp1371[ 4u]= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc* _temp1372[ 2u]; _temp1372[ 1u]= Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1317))->v);
_temp1372[ 0u]= Cyc_PP_line_doc(); Cyc_PP_cat( _tag_arr( _temp1372, sizeof(
struct Cyc_PP_Doc*), 2u));})); _temp1371[ 3u]= Cyc_PP_text( _tag_arr(" {",
sizeof( unsigned char), 3u)); _temp1371[ 2u]= Cyc_Absynpp_qvar2doc( _temp1319);
_temp1371[ 1u]= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u));
_temp1371[ 0u]= Cyc_Absynpp_scope2doc( _temp1321); Cyc_PP_cat( _tag_arr(
_temp1371, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1249; _LL1267: s=({
struct Cyc_PP_Doc* _temp1373[ 3u]; _temp1373[ 2u]= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); _temp1373[ 1u]= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp1374; _temp1374.q_const= 0; _temp1374.q_volatile= 0; _temp1374.q_restrict=
0; _temp1374;}),( void*) _temp1323->defn,({ struct Cyc_Core_Opt* _temp1375=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1375->v=(
void*)({ struct Cyc_PP_Doc* _temp1376[ 2u]; _temp1376[ 1u]= Cyc_Absynpp_tvars2doc(
_temp1323->tvs); _temp1376[ 0u]= Cyc_Absynpp_typedef_name2doc( _temp1323->name);
Cyc_PP_cat( _tag_arr( _temp1376, sizeof( struct Cyc_PP_Doc*), 2u));}); _temp1375;}));
_temp1373[ 0u]= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
Cyc_PP_cat( _tag_arr( _temp1373, sizeof( struct Cyc_PP_Doc*), 3u));}); goto
_LL1249; _LL1269: if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add(
_temp1327);} s=({ struct Cyc_PP_Doc* _temp1377[ 7u]; _temp1377[ 6u]= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); _temp1377[ 5u]= Cyc_PP_line_doc();
_temp1377[ 4u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1325); _temp1377[ 3u]= Cyc_PP_line_doc();
_temp1377[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1377[ 1u]= Cyc_PP_textptr( _temp1327); _temp1377[ 0u]= Cyc_PP_text(
_tag_arr("namespace ", sizeof( unsigned char), 11u)); Cyc_PP_cat( _tag_arr(
_temp1377, sizeof( struct Cyc_PP_Doc*), 7u));}); if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();} goto _LL1249; _LL1271: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc* _temp1378[ 7u]; _temp1378[ 6u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1378[ 5u]= Cyc_PP_line_doc(); _temp1378[ 4u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1329); _temp1378[ 3u]= Cyc_PP_line_doc();
_temp1378[ 2u]= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
_temp1378[ 1u]= Cyc_Absynpp_qvar2doc( _temp1331); _temp1378[ 0u]= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); Cyc_PP_cat( _tag_arr( _temp1378,
sizeof( struct Cyc_PP_Doc*), 7u));});} else{ s=({ struct Cyc_PP_Doc* _temp1379[
7u]; _temp1379[ 6u]= Cyc_PP_text( _tag_arr("/* } */", sizeof( unsigned char), 8u));
_temp1379[ 5u]= Cyc_PP_line_doc(); _temp1379[ 4u]=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1329); _temp1379[ 3u]= Cyc_PP_line_doc();
_temp1379[ 2u]= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u));
_temp1379[ 1u]= Cyc_Absynpp_qvar2doc( _temp1331); _temp1379[ 0u]= Cyc_PP_text(
_tag_arr("/* using ", sizeof( unsigned char), 10u)); Cyc_PP_cat( _tag_arr(
_temp1379, sizeof( struct Cyc_PP_Doc*), 7u));});} goto _LL1249; _LL1273: if( Cyc_Absynpp_print_externC_stmts){
s=({ struct Cyc_PP_Doc* _temp1380[ 5u]; _temp1380[ 4u]= Cyc_PP_text( _tag_arr("}",
sizeof( unsigned char), 2u)); _temp1380[ 3u]= Cyc_PP_line_doc(); _temp1380[ 2u]=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1333); _temp1380[ 1u]= Cyc_PP_line_doc();
_temp1380[ 0u]= Cyc_PP_text( _tag_arr("extern \"C\" {", sizeof( unsigned char),
13u)); Cyc_PP_cat( _tag_arr( _temp1380, sizeof( struct Cyc_PP_Doc*), 5u));});}
else{ s=({ struct Cyc_PP_Doc* _temp1381[ 5u]; _temp1381[ 4u]= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); _temp1381[ 3u]= Cyc_PP_line_doc();
_temp1381[ 2u]=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1333); _temp1381[ 1u]= Cyc_PP_line_doc();
_temp1381[ 0u]= Cyc_PP_text( _tag_arr("/* extern \"C\" { */", sizeof(
unsigned char), 19u)); Cyc_PP_cat( _tag_arr( _temp1381, sizeof( struct Cyc_PP_Doc*),
5u));});} goto _LL1249; _LL1249:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp1382= sc; _LL1384: if( _temp1382 == ( void*) Cyc_Absyn_Static){
goto _LL1385;} else{ goto _LL1386;} _LL1386: if( _temp1382 == ( void*) Cyc_Absyn_Public){
goto _LL1387;} else{ goto _LL1388;} _LL1388: if( _temp1382 == ( void*) Cyc_Absyn_Extern){
goto _LL1389;} else{ goto _LL1390;} _LL1390: if( _temp1382 == ( void*) Cyc_Absyn_ExternC){
goto _LL1391;} else{ goto _LL1392;} _LL1392: if( _temp1382 == ( void*) Cyc_Absyn_Abstract){
goto _LL1393;} else{ goto _LL1383;} _LL1385: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL1387: return Cyc_PP_nil_doc(); _LL1389: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL1391: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL1393:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL1383:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp1394= t; struct
Cyc_Absyn_Tvar* _temp1402; struct Cyc_List_List* _temp1404; _LL1396: if((
unsigned int) _temp1394 >  4u?*(( int*) _temp1394) ==  Cyc_Absyn_VarType: 0){
_LL1403: _temp1402=(( struct Cyc_Absyn_VarType_struct*) _temp1394)->f1; goto
_LL1397;} else{ goto _LL1398;} _LL1398: if(( unsigned int) _temp1394 >  4u?*((
int*) _temp1394) ==  Cyc_Absyn_JoinEff: 0){ _LL1405: _temp1404=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1394)->f1; goto _LL1399;} else{ goto _LL1400;} _LL1400: goto _LL1401;
_LL1397: return Cyc_Tcutil_is_temp_tvar( _temp1402); _LL1399: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp1404); _LL1401: return 0; _LL1395:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp1406= t; struct Cyc_Absyn_Exp* _temp1420; struct Cyc_Absyn_Tqual _temp1422;
void* _temp1424; struct Cyc_Absyn_PtrInfo _temp1426; struct Cyc_Absyn_Conref*
_temp1428; struct Cyc_Absyn_Tqual _temp1430; struct Cyc_Absyn_Conref* _temp1432;
void* _temp1434; void* _temp1436; struct Cyc_Absyn_FnInfo _temp1438; struct Cyc_List_List*
_temp1440; struct Cyc_List_List* _temp1442; struct Cyc_Absyn_VarargInfo*
_temp1444; int _temp1446; struct Cyc_List_List* _temp1448; void* _temp1450;
struct Cyc_Core_Opt* _temp1452; struct Cyc_List_List* _temp1454; int _temp1456;
struct Cyc_Core_Opt* _temp1458; struct Cyc_Core_Opt* _temp1460; struct Cyc_Core_Opt*
_temp1462; struct Cyc_List_List* _temp1464; struct _tuple0* _temp1466; _LL1408:
if(( unsigned int) _temp1406 >  4u?*(( int*) _temp1406) ==  Cyc_Absyn_ArrayType:
0){ _LL1425: _temp1424=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1406)->f1;
goto _LL1423; _LL1423: _temp1422=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1406)->f2; goto _LL1421; _LL1421: _temp1420=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1406)->f3; goto _LL1409;} else{ goto _LL1410;} _LL1410: if(( unsigned int)
_temp1406 >  4u?*(( int*) _temp1406) ==  Cyc_Absyn_PointerType: 0){ _LL1427:
_temp1426=(( struct Cyc_Absyn_PointerType_struct*) _temp1406)->f1; _LL1437:
_temp1436=( void*) _temp1426.elt_typ; goto _LL1435; _LL1435: _temp1434=( void*)
_temp1426.rgn_typ; goto _LL1433; _LL1433: _temp1432= _temp1426.nullable; goto
_LL1431; _LL1431: _temp1430= _temp1426.tq; goto _LL1429; _LL1429: _temp1428=
_temp1426.bounds; goto _LL1411;} else{ goto _LL1412;} _LL1412: if(( unsigned int)
_temp1406 >  4u?*(( int*) _temp1406) ==  Cyc_Absyn_FnType: 0){ _LL1439:
_temp1438=(( struct Cyc_Absyn_FnType_struct*) _temp1406)->f1; _LL1455: _temp1454=
_temp1438.tvars; goto _LL1453; _LL1453: _temp1452= _temp1438.effect; goto
_LL1451; _LL1451: _temp1450=( void*) _temp1438.ret_typ; goto _LL1449; _LL1449:
_temp1448= _temp1438.args; goto _LL1447; _LL1447: _temp1446= _temp1438.c_varargs;
goto _LL1445; _LL1445: _temp1444= _temp1438.cyc_varargs; goto _LL1443; _LL1443:
_temp1442= _temp1438.rgn_po; goto _LL1441; _LL1441: _temp1440= _temp1438.attributes;
goto _LL1413;} else{ goto _LL1414;} _LL1414: if(( unsigned int) _temp1406 >  4u?*((
int*) _temp1406) ==  Cyc_Absyn_Evar: 0){ _LL1461: _temp1460=(( struct Cyc_Absyn_Evar_struct*)
_temp1406)->f1; goto _LL1459; _LL1459: _temp1458=(( struct Cyc_Absyn_Evar_struct*)
_temp1406)->f2; goto _LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_Evar_struct*)
_temp1406)->f3; goto _LL1415;} else{ goto _LL1416;} _LL1416: if(( unsigned int)
_temp1406 >  4u?*(( int*) _temp1406) ==  Cyc_Absyn_TypedefType: 0){ _LL1467:
_temp1466=(( struct Cyc_Absyn_TypedefType_struct*) _temp1406)->f1; goto _LL1465;
_LL1465: _temp1464=(( struct Cyc_Absyn_TypedefType_struct*) _temp1406)->f2; goto
_LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_TypedefType_struct*) _temp1406)->f3;
goto _LL1417;} else{ goto _LL1418;} _LL1418: goto _LL1419; _LL1409: { struct Cyc_List_List*
_temp1470; void* _temp1472; struct Cyc_Absyn_Tqual _temp1474; struct _tuple4
_temp1468= Cyc_Absynpp_to_tms( _temp1422, _temp1424); _LL1475: _temp1474=
_temp1468.f1; goto _LL1473; _LL1473: _temp1472= _temp1468.f2; goto _LL1471;
_LL1471: _temp1470= _temp1468.f3; goto _LL1469; _LL1469: { void* tm; if(
_temp1420 ==  0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp1476=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp1476[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp1477; _temp1477.tag= Cyc_Absyn_ConstArray_mod;
_temp1477.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1420); _temp1477;});
_temp1476;});} return({ struct _tuple4 _temp1478; _temp1478.f1= _temp1474;
_temp1478.f2= _temp1472; _temp1478.f3=({ struct Cyc_List_List* _temp1479=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1479->hd=(
void*) tm; _temp1479->tl= _temp1470; _temp1479;}); _temp1478;});}} _LL1411: {
struct Cyc_List_List* _temp1482; void* _temp1484; struct Cyc_Absyn_Tqual
_temp1486; struct _tuple4 _temp1480= Cyc_Absynpp_to_tms( _temp1430, _temp1436);
_LL1487: _temp1486= _temp1480.f1; goto _LL1485; _LL1485: _temp1484= _temp1480.f2;
goto _LL1483; _LL1483: _temp1482= _temp1480.f3; goto _LL1481; _LL1481: { void*
ps;{ struct _tuple8 _temp1489=({ struct _tuple8 _temp1488; _temp1488.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1432))->v; _temp1488.f2=( void*)( Cyc_Absyn_compress_conref( _temp1428))->v;
_temp1488;}); void* _temp1499; void* _temp1501; struct Cyc_Absyn_Exp* _temp1503;
void* _temp1505; int _temp1507; void* _temp1509; void* _temp1511; struct Cyc_Absyn_Exp*
_temp1513; void* _temp1515; int _temp1517; void* _temp1519; void* _temp1521;
_LL1491: _LL1506: _temp1505= _temp1489.f1; if(( unsigned int) _temp1505 >  1u?*((
int*) _temp1505) ==  Cyc_Absyn_Eq_constr: 0){ _LL1508: _temp1507=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp1505)->f1; if( _temp1507 ==  1){ goto _LL1500;}
else{ goto _LL1493;}} else{ goto _LL1493;} _LL1500: _temp1499= _temp1489.f2; if((
unsigned int) _temp1499 >  1u?*(( int*) _temp1499) ==  Cyc_Absyn_Eq_constr: 0){
_LL1502: _temp1501=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1499)->f1;
if(( unsigned int) _temp1501 >  1u?*(( int*) _temp1501) ==  Cyc_Absyn_Upper_b: 0){
_LL1504: _temp1503=(( struct Cyc_Absyn_Upper_b_struct*) _temp1501)->f1; goto
_LL1492;} else{ goto _LL1493;}} else{ goto _LL1493;} _LL1493: _LL1516: _temp1515=
_temp1489.f1; if(( unsigned int) _temp1515 >  1u?*(( int*) _temp1515) ==  Cyc_Absyn_Eq_constr:
0){ _LL1518: _temp1517=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1515)->f1;
if( _temp1517 ==  0){ goto _LL1510;} else{ goto _LL1495;}} else{ goto _LL1495;}
_LL1510: _temp1509= _temp1489.f2; if(( unsigned int) _temp1509 >  1u?*(( int*)
_temp1509) ==  Cyc_Absyn_Eq_constr: 0){ _LL1512: _temp1511=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1509)->f1; if(( unsigned int) _temp1511 >  1u?*(( int*) _temp1511) ==  Cyc_Absyn_Upper_b:
0){ _LL1514: _temp1513=(( struct Cyc_Absyn_Upper_b_struct*) _temp1511)->f1; goto
_LL1494;} else{ goto _LL1495;}} else{ goto _LL1495;} _LL1495: _LL1520: _temp1519=
_temp1489.f2; if(( unsigned int) _temp1519 >  1u?*(( int*) _temp1519) ==  Cyc_Absyn_Eq_constr:
0){ _LL1522: _temp1521=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1519)->f1;
if( _temp1521 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1496;} else{ goto
_LL1497;}} else{ goto _LL1497;} _LL1497: goto _LL1498; _LL1492: ps=( void*)({
struct Cyc_Absyn_Nullable_ps_struct* _temp1523=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp1523[ 0]=({
struct Cyc_Absyn_Nullable_ps_struct _temp1524; _temp1524.tag= Cyc_Absyn_Nullable_ps;
_temp1524.f1= _temp1503; _temp1524;}); _temp1523;}); goto _LL1490; _LL1494: ps=(
void*)({ struct Cyc_Absyn_NonNullable_ps_struct* _temp1525=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp1525[ 0]=({
struct Cyc_Absyn_NonNullable_ps_struct _temp1526; _temp1526.tag= Cyc_Absyn_NonNullable_ps;
_temp1526.f1= _temp1513; _temp1526;}); _temp1525;}); goto _LL1490; _LL1496: ps=(
void*) Cyc_Absyn_TaggedArray_ps; goto _LL1490; _LL1498: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL1490; _LL1490:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp1529=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp1529[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp1530; _temp1530.tag=
Cyc_Absyn_Pointer_mod; _temp1530.f1=( void*) ps; _temp1530.f2=( void*) _temp1434;
_temp1530.f3= tq; _temp1530;}); _temp1529;}); return({ struct _tuple4 _temp1527;
_temp1527.f1= _temp1486; _temp1527.f2= _temp1484; _temp1527.f3=({ struct Cyc_List_List*
_temp1528=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1528->hd=( void*) tm; _temp1528->tl= _temp1482; _temp1528;}); _temp1527;});}}}
_LL1413: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp1452 ==  0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1452))->v)){ _temp1452= 0;
_temp1454= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_rewrite_temp_tvar,
_temp1454);}}{ struct Cyc_List_List* _temp1533; void* _temp1535; struct Cyc_Absyn_Tqual
_temp1537; struct _tuple4 _temp1531= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp1450); _LL1538: _temp1537= _temp1531.f1; goto _LL1536; _LL1536: _temp1535=
_temp1531.f2; goto _LL1534; _LL1534: _temp1533= _temp1531.f3; goto _LL1532;
_LL1532: { struct Cyc_List_List* tms= _temp1533; if( _temp1440 !=  0? ! Cyc_Absynpp_to_VC:
0){ tms=({ struct Cyc_List_List* _temp1539=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1539->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp1540=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp1540[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp1541; _temp1541.tag= Cyc_Absyn_Attributes_mod; _temp1541.f1= 0; _temp1541.f2=
_temp1440; _temp1541;}); _temp1540;})); _temp1539->tl= tms; _temp1539;});} tms=({
struct Cyc_List_List* _temp1542=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1542->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp1543=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp1543[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp1544; _temp1544.tag=
Cyc_Absyn_Function_mod; _temp1544.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp1545=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp1545[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp1546; _temp1546.tag= Cyc_Absyn_WithTypes;
_temp1546.f1= _temp1448; _temp1546.f2= _temp1446; _temp1546.f3= _temp1444;
_temp1546.f4= _temp1452; _temp1546.f5= _temp1442; _temp1546;}); _temp1545;}));
_temp1544;}); _temp1543;})); _temp1542->tl= tms; _temp1542;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp1440 !=  0; _temp1440=(( struct Cyc_List_List*) _check_null(
_temp1440))->tl){ void* _temp1547=( void*)(( struct Cyc_List_List*) _check_null(
_temp1440))->hd; _LL1549: if( _temp1547 == ( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1550;} else{ goto _LL1551;} _LL1551: if( _temp1547 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL1552;} else{ goto _LL1553;} _LL1553: if( _temp1547 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL1554;} else{ goto _LL1555;} _LL1555: goto _LL1556; _LL1550: goto _LL1552;
_LL1552: goto _LL1554; _LL1554: tms=({ struct Cyc_List_List* _temp1557=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1557->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp1558=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp1558[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp1559; _temp1559.tag= Cyc_Absyn_Attributes_mod;
_temp1559.f1= 0; _temp1559.f2=({ struct Cyc_List_List* _temp1560=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1560->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp1440))->hd); _temp1560->tl= 0;
_temp1560;}); _temp1559;}); _temp1558;})); _temp1557->tl= tms; _temp1557;});
goto AfterAtts; _LL1556: goto _LL1548; _LL1548:;}} AfterAtts: if( _temp1454 != 
0){ tms=({ struct Cyc_List_List* _temp1561=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1561->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp1562=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp1562[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp1563; _temp1563.tag= Cyc_Absyn_TypeParams_mod; _temp1563.f1= _temp1454;
_temp1563.f2= 0; _temp1563.f3= 1; _temp1563;}); _temp1562;})); _temp1561->tl=
tms; _temp1561;});} return({ struct _tuple4 _temp1564; _temp1564.f1= _temp1537;
_temp1564.f2= _temp1535; _temp1564.f3= tms; _temp1564;});}} _LL1415: if(
_temp1458 ==  0){ return({ struct _tuple4 _temp1565; _temp1565.f1= tq; _temp1565.f2=
t; _temp1565.f3= 0; _temp1565;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1458))->v);} _LL1417: if( _temp1462 == 
0? 1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp1566;
_temp1566.f1= tq; _temp1566.f2= t; _temp1566.f3= 0; _temp1566;});} else{ return
Cyc_Absynpp_to_tms( tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1462))->v);}
_LL1419: return({ struct _tuple4 _temp1567; _temp1567.f1= tq; _temp1567.f2= t;
_temp1567.f3= 0; _temp1567;}); _LL1407:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp1570; void* _temp1572; struct Cyc_Absyn_Tqual _temp1574; struct _tuple4
_temp1568= Cyc_Absynpp_to_tms( tq, t); _LL1575: _temp1574= _temp1568.f1; goto
_LL1573; _LL1573: _temp1572= _temp1568.f2; goto _LL1571; _LL1571: _temp1570=
_temp1568.f3; goto _LL1569; _LL1569: _temp1570= Cyc_List_imp_rev( _temp1570);
if( _temp1570 ==  0? dopt ==  0: 0){ return({ struct Cyc_PP_Doc* _temp1576[ 2u];
_temp1576[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1572); _temp1576[ 0u]= Cyc_Absynpp_tqual2doc(
_temp1574); Cyc_PP_cat( _tag_arr( _temp1576, sizeof( struct Cyc_PP_Doc*), 2u));});}
else{ return({ struct Cyc_PP_Doc* _temp1577[ 4u]; _temp1577[ 3u]= Cyc_Absynpp_dtms2doc(
dopt ==  0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( dopt))->v, _temp1570); _temp1577[ 2u]= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); _temp1577[ 1u]= Cyc_Absynpp_ntyp2doc( _temp1572);
_temp1577[ 0u]= Cyc_Absynpp_tqual2doc( _temp1574); Cyc_PP_cat( _tag_arr(
_temp1577, sizeof( struct Cyc_PP_Doc*), 4u));});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width !=  0){ return({ struct Cyc_PP_Doc*
_temp1578[ 5u]; _temp1578[ 4u]= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); _temp1578[ 3u]= Cyc_Absynpp_atts2doc( f->attributes); _temp1578[ 2u]= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( f->width)); _temp1578[ 1u]= Cyc_PP_text(
_tag_arr(":", sizeof( unsigned char), 2u)); _temp1578[ 0u]= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp1579=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1579->v=( void*) Cyc_PP_textptr(
f->name); _temp1579;})); Cyc_PP_cat( _tag_arr( _temp1578, sizeof( struct Cyc_PP_Doc*),
5u));});} else{ return({ struct Cyc_PP_Doc* _temp1580[ 3u]; _temp1580[ 2u]= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); _temp1580[ 1u]= Cyc_Absynpp_atts2doc(
f->attributes); _temp1580[ 0u]= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp1581=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1581->v=( void*) Cyc_PP_textptr( f->name); _temp1581;}));
Cyc_PP_cat( _tag_arr( _temp1580, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct
Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,
_tag_arr("", sizeof( unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc* _temp1582[ 4u];
_temp1582[ 3u]= f->typs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc( f->typs);
_temp1582[ 2u]= f->tvs ==  0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc( f->tvs);
_temp1582[ 1u]= Cyc_Absynpp_typedef_name2doc( f->name); _temp1582[ 0u]= Cyc_Absynpp_scope2doc((
void*) f->sc); Cyc_PP_cat( _tag_arr( _temp1582, sizeof( struct Cyc_PP_Doc*), 4u));});}
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Tunionfield*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,
_tag_arr(",", sizeof( unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl !=  0; tdl=((
struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd), 72, f);({
void* _temp1583[ 0u]={}; Cyc_Stdio_fprintf( f, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1583, sizeof( void*), 0u));});}} struct
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
sc){ void* _temp1584= sc; _LL1586: if( _temp1584 == ( void*) Cyc_Absyn_Static){
goto _LL1587;} else{ goto _LL1588;} _LL1588: if( _temp1584 == ( void*) Cyc_Absyn_Public){
goto _LL1589;} else{ goto _LL1590;} _LL1590: if( _temp1584 == ( void*) Cyc_Absyn_Extern){
goto _LL1591;} else{ goto _LL1592;} _LL1592: if( _temp1584 == ( void*) Cyc_Absyn_ExternC){
goto _LL1593;} else{ goto _LL1594;} _LL1594: if( _temp1584 == ( void*) Cyc_Absyn_Abstract){
goto _LL1595;} else{ goto _LL1585;} _LL1587: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL1589: return _tag_arr("public", sizeof( unsigned char),
7u); _LL1591: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL1593:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL1595: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL1585:;}
