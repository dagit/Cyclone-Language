 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_std___sFILE;
extern struct Cyc_std___sFILE* Cyc_std_stderr; extern int Cyc_std_fflush( struct
Cyc_std___sFILE*); extern unsigned char Cyc_std_FileCloseError[ 19u]; extern
unsigned char Cyc_std_FileOpenError[ 18u]; struct Cyc_std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_std_String_pa=
0; struct Cyc_std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_std_Int_pa= 1; struct Cyc_std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_std_Double_pa= 2; struct Cyc_std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa= 3; struct Cyc_std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_std_fprintf( struct Cyc_std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_std_ShortPtr_sa=
0; struct Cyc_std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_std_UShortPtr_sa= 1; struct Cyc_std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_std_IntPtr_sa= 2; struct Cyc_std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_std_UIntPtr_sa= 3; struct Cyc_std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_std_StringPtr_sa= 4; struct
Cyc_std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern
int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key); struct Cyc_Set_Set;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_SlowDict_Dict; extern unsigned char Cyc_SlowDict_Present[ 12u];
extern unsigned char Cyc_SlowDict_Absent[ 11u]; extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_empty( int(* comp)( void*, void*)); extern int Cyc_SlowDict_is_empty(
struct Cyc_SlowDict_Dict* d); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert(
struct Cyc_SlowDict_Dict* d, void* key, void* data); extern struct Cyc_Core_Opt*
Cyc_SlowDict_lookup_opt( struct Cyc_SlowDict_Dict* d, void* key); extern struct
Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict* d, void* key);
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct _tuple0{ void* f1; struct
_tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref;
struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_SwitchC_clause; struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0* tunion_name; struct _tuple0* field_name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{
int tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
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
15u]; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*);
extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct _tuple0* Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud; extern struct _tuple0*
Cyc_Absyn_null_pointer_exn_name; extern struct _tuple0* Cyc_Absyn_match_exn_name;
extern struct Cyc_Absyn_Tunionfield* Cyc_Absyn_null_pointer_exn_tuf; extern
struct Cyc_Absyn_Tunionfield* Cyc_Absyn_match_exn_tuf; extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); static
const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List*
Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tagged_arr*, struct Cyc_List_List*); extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
continue_dest); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds,
struct Cyc_Absyn_Switch_clause* clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next(
struct Cyc_Tcenv_Tenv*, void*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv* te); extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te); extern void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct _tagged_arr*, struct Cyc_Absyn_Stmt**);
struct _tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_arr*, struct Cyc_Absyn_Stmt*); extern int
Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar* name); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region(
struct Cyc_Tcenv_Tenv* te, void* r); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern int Cyc_Tcenv_region_outlives( struct Cyc_Tcenv_Tenv*,
void* r1, void* r2); extern void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set;
extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar* t); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void*
e2); extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e);
extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*, struct
Cyc_List_List*); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k);
extern int* Cyc_Tcutil_new_tvar_id(); extern void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*);
unsigned char Cyc_Tcenv_Env_error[ 14u]="\000\000\000\000Env_error"; void* Cyc_Tcenv_env_err(
struct _tagged_arr msg){({ struct Cyc_std_String_pa_struct _temp1; _temp1.tag=
Cyc_std_String_pa; _temp1.f1=( struct _tagged_arr) msg;{ void* _temp0[ 1u]={&
_temp1}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("Error: %s\n", sizeof(
unsigned char), 11u), _tag_arr( _temp0, sizeof( void*), 1u));}}); Cyc_std_fflush((
struct Cyc_std___sFILE*) Cyc_std_stderr);( int) _throw(( void*) Cyc_Tcenv_Env_error);}
struct Cyc_Tcenv_Tenv; struct Cyc_Tcenv_Genv; struct Cyc_Tcenv_CtrlEnv{ void*
continue_stmt; void* break_stmt; struct _tuple3* fallthru_clause; void*
next_stmt; int try_depth; } ; struct Cyc_Tcenv_SharedFenv{ void* return_typ;
struct Cyc_Dict_Dict* seen_labels; struct Cyc_SlowDict_Dict* needed_labels; } ;
struct Cyc_Tcenv_RgnPO{ struct Cyc_Absyn_Tvar* tvar; struct Cyc_List_List*
ancestors; } ; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv* shared;
struct Cyc_List_List* type_vars; struct Cyc_List_List* region_order; struct Cyc_Dict_Dict*
locals; struct Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv* ctrl_env;
void* capability; void* curr_rgn; } ; struct _tuple4{ void* f1; int f2; } ;
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({ struct
Cyc_Core_Opt* _temp2=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_std_zstrptrcmp); _temp2;});{ struct
Cyc_Tcenv_Genv* _temp3= Cyc_Tcenv_empty_genv(); struct Cyc_List_List* _temp4= 0;
_temp3->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( _temp3->tuniondecls,(*
Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_tud); _temp3->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple4* data)) Cyc_Dict_insert)(
_temp3->ordinaries,(* Cyc_Absyn_null_pointer_exn_name).f2,({ struct _tuple4*
_temp5=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp5->f1=( void*)({
struct Cyc_Tcenv_TunionRes_struct* _temp6=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp6[ 0]=({ struct Cyc_Tcenv_TunionRes_struct
_temp7; _temp7.tag= Cyc_Tcenv_TunionRes; _temp7.f1= Cyc_Absyn_exn_tud; _temp7.f2=
Cyc_Absyn_null_pointer_exn_tuf; _temp7;}); _temp6;}); _temp5->f2= 1; _temp5;}));
_temp3->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct _tuple4* data)) Cyc_Dict_insert)( _temp3->ordinaries,(*
Cyc_Absyn_match_exn_name).f2,({ struct _tuple4* _temp8=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp8->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct*
_temp9=( struct Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp9[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp10; _temp10.tag= Cyc_Tcenv_TunionRes;
_temp10.f1= Cyc_Absyn_exn_tud; _temp10.f2= Cyc_Absyn_match_exn_tuf; _temp10;});
_temp9;}); _temp8->f2= 1; _temp8;}));{ struct Cyc_Dict_Dict* ae=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), _temp4, _temp3);
return({ struct Cyc_Tcenv_Tenv* _temp11=( struct Cyc_Tcenv_Tenv*) GC_malloc(
sizeof( struct Cyc_Tcenv_Tenv)); _temp11->ns= _temp4; _temp11->ae= ae; _temp11->le=
0; _temp11;});}}} static struct Cyc_Tcenv_Genv* Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, Cyc_Tcenv_resolve_namespace(
te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns ==  0){ return(( struct Cyc_List_List*(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv::outer_namespace",
sizeof( unsigned char), 23u));} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( ns)))->tl);} static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List* n1, struct Cyc_List_List* n2){ if( n1 ==  n2){ return 1;}
for( 0; n1 !=  0; n1=(( struct Cyc_List_List*) _check_null( n1))->tl){ if( n2 == 
0){ return 0;} if( Cyc_std_zstrptrcmp(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( n1))->hd,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( n2))->hd) !=  0){ return 0;} n2=(( struct Cyc_List_List*)
_check_null( n2))->tl;} if( n2 !=  0){ return 0;} return 1;} static void Cyc_Tcenv_check_repeat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v, struct Cyc_List_List*
cns, struct Cyc_List_List* nss){ for( 0; nss !=  0; nss=(( struct Cyc_List_List*)
_check_null( nss))->tl){ if( ! Cyc_Tcenv_same_namespace( cns,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( nss))->hd)){ struct Cyc_Tcenv_Genv* ge2=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( nss))->hd); struct _handler_cons _temp12; _push_handler(& _temp12);{
int _temp14= 0; if( setjmp( _temp12.handler)){ _temp14= 1;} if( ! _temp14){
lookup( ge2, v);({ struct Cyc_std_String_pa_struct _temp16; _temp16.tag= Cyc_std_String_pa;
_temp16.f1=( struct _tagged_arr)* v;{ void* _temp15[ 1u]={& _temp16}; Cyc_Tcutil_terr(
loc, _tag_arr("%s is ambiguous", sizeof( unsigned char), 16u), _tag_arr( _temp15,
sizeof( void*), 1u));}});; _pop_handler();} else{ void* _temp13=( void*)
_exn_thrown; void* _temp18= _temp13; _LL20: if( _temp18 ==  Cyc_Dict_Absent){
goto _LL21;} else{ goto _LL22;} _LL22: goto _LL23; _LL21: goto _LL19; _LL23:(
void) _throw( _temp18); _LL19:;}}}} return;} static void* Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v){ struct Cyc_List_List*
cns= te->ns; while( 1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
cns);{ struct _handler_cons _temp24; _push_handler(& _temp24);{ int _temp26= 0;
if( setjmp( _temp24.handler)){ _temp26= 1;} if( ! _temp26){{ void* result=
lookup( ge, v); Cyc_Tcenv_check_repeat( te, loc, lookup, v, cns, ge->availables);{
void* _temp27= result; _npop_handler( 0u); return _temp27;}}; _pop_handler();}
else{ void* _temp25=( void*) _exn_thrown; void* _temp29= _temp25; _LL31: if(
_temp29 ==  Cyc_Dict_Absent){ goto _LL32;} else{ goto _LL33;} _LL33: goto _LL34;
_LL32: goto _LL30; _LL34:( void) _throw( _temp29); _LL30:;}}}{ struct Cyc_List_List*
_temp35= ge->availables; for( 0; _temp35 !=  0; _temp35=(( struct Cyc_List_List*)
_check_null( _temp35))->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp35))->hd);
struct _handler_cons _temp36; _push_handler(& _temp36);{ int _temp38= 0; if(
setjmp( _temp36.handler)){ _temp38= 1;} if( ! _temp38){{ void* result= lookup(
ge2, v); Cyc_Tcenv_check_repeat( te, loc, lookup, v,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( _temp35))->hd,(( struct Cyc_List_List*)
_check_null( _temp35))->tl);{ void* _temp39= result; _npop_handler( 0u); return
_temp39;}}; _pop_handler();} else{ void* _temp37=( void*) _exn_thrown; void*
_temp41= _temp37; _LL43: if( _temp41 ==  Cyc_Dict_Absent){ goto _LL44;} else{
goto _LL45;} _LL45: goto _LL46; _LL44: goto _LL42; _LL46:( void) _throw( _temp41);
_LL42:;}}}} if( cns ==  0){( int) _throw(( void*) Cyc_Dict_Absent);} cns= Cyc_Tcenv_outer_namespace(
cns);}} static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_arr* v){ struct _tuple4* ans=(( struct _tuple4*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v);(*
ans).f2= 1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp49; struct _tagged_arr* _temp50; void* _temp52; struct
_tuple0* _temp47= q; _temp49=* _temp47; _LL53: _temp52= _temp49.f1; goto _LL51;
_LL51: _temp50= _temp49.f2; goto _LL48; _LL48: { void* _temp54= _temp52; struct
Cyc_List_List* _temp64; struct Cyc_List_List* _temp66; struct Cyc_List_List
_temp68; struct Cyc_List_List* _temp69; struct _tagged_arr* _temp71; struct Cyc_List_List*
_temp73; _LL56: if( _temp54 == ( void*) Cyc_Absyn_Loc_n){ goto _LL57;} else{
goto _LL58;} _LL58: if(( unsigned int) _temp54 >  1u?*(( int*) _temp54) ==  Cyc_Absyn_Rel_n:
0){ _LL65: _temp64=(( struct Cyc_Absyn_Rel_n_struct*) _temp54)->f1; if( _temp64
==  0){ goto _LL59;} else{ goto _LL60;}} else{ goto _LL60;} _LL60: if((
unsigned int) _temp54 >  1u?*(( int*) _temp54) ==  Cyc_Absyn_Rel_n: 0){ _LL67:
_temp66=(( struct Cyc_Absyn_Rel_n_struct*) _temp54)->f1; if( _temp66 ==  0){
goto _LL62;} else{ _temp68=* _temp66; _LL72: _temp71=( struct _tagged_arr*)
_temp68.hd; goto _LL70; _LL70: _temp69= _temp68.tl; goto _LL61;}} else{ goto
_LL62;} _LL62: if(( unsigned int) _temp54 >  1u?*(( int*) _temp54) ==  Cyc_Absyn_Abs_n:
0){ _LL74: _temp73=(( struct Cyc_Absyn_Abs_n_struct*) _temp54)->f1; goto _LL63;}
else{ goto _LL55;} _LL57: goto _LL59; _LL59: return Cyc_Tcenv_scoped_lookup( te,
loc, Cyc_Tcenv_lookup_ordinary_global_f, _temp50); _LL61: { struct Cyc_Tcenv_Genv*
_temp75= Cyc_Tcenv_lookup_namespace( te, loc, _temp71, _temp69); return Cyc_Tcenv_lookup_ordinary_global_f(
_temp75, _temp50);} _LL63: return Cyc_Tcenv_lookup_ordinary_global_f((( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, _temp73), _temp50); _LL55:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ struct Cyc_List_List* _temp76= te->ns; struct Cyc_List_List*
_temp77= 0; while( 1) { struct Cyc_Tcenv_Genv* _temp78=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp76);{ struct Cyc_List_List* _temp79= _temp78->availables; for( 0; _temp79
!=  0; _temp79=(( struct Cyc_List_List*) _check_null( _temp79))->tl){ struct Cyc_Tcenv_Genv*
ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( _temp79))->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge2->namespaces, n)){ _temp77=({ struct Cyc_List_List*
_temp80=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp80->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp76,({ struct Cyc_List_List* _temp81=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp81->hd=(
void*) n; _temp81->tl= ns; _temp81;})); _temp80->tl= _temp77; _temp80;});}}} if(((
int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp78->namespaces, n)){ _temp77=({ struct Cyc_List_List* _temp82=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp82->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp76,({
struct Cyc_List_List* _temp83=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp83->hd=( void*) n; _temp83->tl= ns; _temp83;})); _temp82->tl=
_temp77; _temp82;});} if( _temp77 !=  0){ if((( struct Cyc_List_List*)
_check_null( _temp77))->tl !=  0){({ struct Cyc_std_String_pa_struct _temp85;
_temp85.tag= Cyc_std_String_pa; _temp85.f1=( struct _tagged_arr)* n;{ void*
_temp84[ 1u]={& _temp85}; Cyc_Tcutil_terr( loc, _tag_arr("%s is ambiguous",
sizeof( unsigned char), 16u), _tag_arr( _temp84, sizeof( void*), 1u));}});}
return( struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp77))->hd;}
if( _temp76 ==  0){( int) _throw(( void*) Cyc_Dict_Absent);} _temp76= Cyc_Tcenv_outer_namespace(
_temp76);}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp88;
struct _tagged_arr* _temp89; void* _temp91; struct _tuple0* _temp86= q; _temp88=*
_temp86; _LL92: _temp91= _temp88.f1; goto _LL90; _LL90: _temp89= _temp88.f2;
goto _LL87; _LL87: { void* _temp93= _temp91; struct Cyc_List_List* _temp103;
struct Cyc_List_List* _temp105; struct Cyc_List_List* _temp107; struct Cyc_List_List
_temp109; struct Cyc_List_List* _temp110; struct _tagged_arr* _temp112; _LL95:
if( _temp93 == ( void*) Cyc_Absyn_Loc_n){ goto _LL96;} else{ goto _LL97;} _LL97:
if(( unsigned int) _temp93 >  1u?*(( int*) _temp93) ==  Cyc_Absyn_Rel_n: 0){
_LL104: _temp103=(( struct Cyc_Absyn_Rel_n_struct*) _temp93)->f1; if( _temp103
==  0){ goto _LL98;} else{ goto _LL99;}} else{ goto _LL99;} _LL99: if((
unsigned int) _temp93 >  1u?*(( int*) _temp93) ==  Cyc_Absyn_Abs_n: 0){ _LL106:
_temp105=(( struct Cyc_Absyn_Abs_n_struct*) _temp93)->f1; goto _LL100;} else{
goto _LL101;} _LL101: if(( unsigned int) _temp93 >  1u?*(( int*) _temp93) == 
Cyc_Absyn_Rel_n: 0){ _LL108: _temp107=(( struct Cyc_Absyn_Rel_n_struct*) _temp93)->f1;
if( _temp107 ==  0){ goto _LL94;} else{ _temp109=* _temp107; _LL113: _temp112=(
struct _tagged_arr*) _temp109.hd; goto _LL111; _LL111: _temp110= _temp109.tl;
goto _LL102;}} else{ goto _LL94;} _LL96: goto _LL98; _LL98: return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp89);
_LL100: { struct Cyc_Tcenv_Genv* _temp114=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp105);
return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp114->structdecls, _temp89);} _LL102: {
struct Cyc_Tcenv_Genv* _temp115= Cyc_Tcenv_lookup_namespace( te, loc, _temp112,
_temp110); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp115->structdecls, _temp89);}
_LL94:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp118; struct _tagged_arr* _temp119; void* _temp121; struct _tuple0* _temp116=
q; _temp118=* _temp116; _LL122: _temp121= _temp118.f1; goto _LL120; _LL120:
_temp119= _temp118.f2; goto _LL117; _LL117: { void* _temp123= _temp121; struct
Cyc_List_List* _temp133; struct Cyc_List_List* _temp135; struct Cyc_List_List*
_temp137; struct Cyc_List_List _temp139; struct Cyc_List_List* _temp140; struct
_tagged_arr* _temp142; _LL125: if( _temp123 == ( void*) Cyc_Absyn_Loc_n){ goto
_LL126;} else{ goto _LL127;} _LL127: if(( unsigned int) _temp123 >  1u?*(( int*)
_temp123) ==  Cyc_Absyn_Rel_n: 0){ _LL134: _temp133=(( struct Cyc_Absyn_Rel_n_struct*)
_temp123)->f1; if( _temp133 ==  0){ goto _LL128;} else{ goto _LL129;}} else{
goto _LL129;} _LL129: if(( unsigned int) _temp123 >  1u?*(( int*) _temp123) == 
Cyc_Absyn_Abs_n: 0){ _LL136: _temp135=(( struct Cyc_Absyn_Abs_n_struct*)
_temp123)->f1; goto _LL130;} else{ goto _LL131;} _LL131: if(( unsigned int)
_temp123 >  1u?*(( int*) _temp123) ==  Cyc_Absyn_Rel_n: 0){ _LL138: _temp137=((
struct Cyc_Absyn_Rel_n_struct*) _temp123)->f1; if( _temp137 ==  0){ goto _LL124;}
else{ _temp139=* _temp137; _LL143: _temp142=( struct _tagged_arr*) _temp139.hd;
goto _LL141; _LL141: _temp140= _temp139.tl; goto _LL132;}} else{ goto _LL124;}
_LL126: goto _LL128; _LL128: return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp119);
_LL130: { struct Cyc_Tcenv_Genv* _temp144=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp135);
return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp144->uniondecls, _temp119);} _LL132: {
struct Cyc_Tcenv_Genv* _temp145= Cyc_Tcenv_lookup_namespace( te, loc, _temp142,
_temp140); return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp145->uniondecls, _temp119);}
_LL124:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp148; struct _tagged_arr* _temp149; void* _temp151; struct _tuple0* _temp146=
q; _temp148=* _temp146; _LL152: _temp151= _temp148.f1; goto _LL150; _LL150:
_temp149= _temp148.f2; goto _LL147; _LL147: { void* _temp153= _temp151; struct
Cyc_List_List* _temp163; struct Cyc_List_List* _temp165; struct Cyc_List_List
_temp167; struct Cyc_List_List* _temp168; struct _tagged_arr* _temp170; struct
Cyc_List_List* _temp172; _LL155: if( _temp153 == ( void*) Cyc_Absyn_Loc_n){ goto
_LL156;} else{ goto _LL157;} _LL157: if(( unsigned int) _temp153 >  1u?*(( int*)
_temp153) ==  Cyc_Absyn_Rel_n: 0){ _LL164: _temp163=(( struct Cyc_Absyn_Rel_n_struct*)
_temp153)->f1; if( _temp163 ==  0){ goto _LL158;} else{ goto _LL159;}} else{
goto _LL159;} _LL159: if(( unsigned int) _temp153 >  1u?*(( int*) _temp153) == 
Cyc_Absyn_Rel_n: 0){ _LL166: _temp165=(( struct Cyc_Absyn_Rel_n_struct*)
_temp153)->f1; if( _temp165 ==  0){ goto _LL161;} else{ _temp167=* _temp165;
_LL171: _temp170=( struct _tagged_arr*) _temp167.hd; goto _LL169; _LL169:
_temp168= _temp167.tl; goto _LL160;}} else{ goto _LL161;} _LL161: if((
unsigned int) _temp153 >  1u?*(( int*) _temp153) ==  Cyc_Absyn_Abs_n: 0){ _LL173:
_temp172=(( struct Cyc_Absyn_Abs_n_struct*) _temp153)->f1; goto _LL162;} else{
goto _LL154;} _LL156: goto _LL158; _LL158: return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_tuniondecl_f, _temp149);
_LL160: { struct Cyc_Tcenv_Genv* _temp174= Cyc_Tcenv_lookup_namespace( te, loc,
_temp170, _temp168); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp174->tuniondecls, _temp149);}
_LL162: { struct Cyc_Tcenv_Genv* _temp175=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp172);
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp175->tuniondecls, _temp149);} _LL154:;}}
static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls, v);}
struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp178;
struct _tagged_arr* _temp179; void* _temp181; struct _tuple0* _temp176= q;
_temp178=* _temp176; _LL182: _temp181= _temp178.f1; goto _LL180; _LL180:
_temp179= _temp178.f2; goto _LL177; _LL177: { void* _temp183= _temp181; struct
Cyc_List_List* _temp193; struct Cyc_List_List* _temp195; struct Cyc_List_List
_temp197; struct Cyc_List_List* _temp198; struct _tagged_arr* _temp200; struct
Cyc_List_List* _temp202; _LL185: if(( unsigned int) _temp183 >  1u?*(( int*)
_temp183) ==  Cyc_Absyn_Rel_n: 0){ _LL194: _temp193=(( struct Cyc_Absyn_Rel_n_struct*)
_temp183)->f1; if( _temp193 ==  0){ goto _LL186;} else{ goto _LL187;}} else{
goto _LL187;} _LL187: if( _temp183 == ( void*) Cyc_Absyn_Loc_n){ goto _LL188;}
else{ goto _LL189;} _LL189: if(( unsigned int) _temp183 >  1u?*(( int*) _temp183)
==  Cyc_Absyn_Rel_n: 0){ _LL196: _temp195=(( struct Cyc_Absyn_Rel_n_struct*)
_temp183)->f1; if( _temp195 ==  0){ goto _LL191;} else{ _temp197=* _temp195;
_LL201: _temp200=( struct _tagged_arr*) _temp197.hd; goto _LL199; _LL199:
_temp198= _temp197.tl; goto _LL190;}} else{ goto _LL191;} _LL191: if((
unsigned int) _temp183 >  1u?*(( int*) _temp183) ==  Cyc_Absyn_Abs_n: 0){ _LL203:
_temp202=(( struct Cyc_Absyn_Abs_n_struct*) _temp183)->f1; goto _LL192;} else{
goto _LL184;} _LL186: { struct _handler_cons _temp204; _push_handler(& _temp204);{
int _temp206= 0; if( setjmp( _temp204.handler)){ _temp206= 1;} if( ! _temp206){{
struct Cyc_Core_Opt* _temp208=({ struct Cyc_Core_Opt* _temp207=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp207->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp179);
_temp207;}); _npop_handler( 0u); return _temp208;}; _pop_handler();} else{ void*
_temp205=( void*) _exn_thrown; void* _temp210= _temp205; _LL212: if( _temp210 == 
Cyc_Dict_Absent){ goto _LL213;} else{ goto _LL214;} _LL214: goto _LL215; _LL213:
return 0; _LL215:( void) _throw( _temp210); _LL211:;}}} _LL188:({ void* _temp216[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("lookup_xtuniondecl: impossible", sizeof(
unsigned char), 31u), _tag_arr( _temp216, sizeof( void*), 0u));}); return 0;
_LL190: { struct Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp217;
_push_handler(& _temp217);{ int _temp219= 0; if( setjmp( _temp217.handler)){
_temp219= 1;} if( ! _temp219){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp200,
_temp198);; _pop_handler();} else{ void* _temp218=( void*) _exn_thrown; void*
_temp221= _temp218; _LL223: if( _temp221 ==  Cyc_Dict_Absent){ goto _LL224;}
else{ goto _LL225;} _LL225: goto _LL226; _LL224:({ void* _temp227[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad qualified name for xtunion", sizeof( unsigned char), 31u),
_tag_arr( _temp227, sizeof( void*), 0u));});( int) _throw(( void*) Cyc_Dict_Absent);
_LL226:( void) _throw( _temp221); _LL222:;}}} return({ struct Cyc_Core_Opt*
_temp228=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp228->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp179); _temp228;});}
_LL192: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp202); return({
struct Cyc_Core_Opt* _temp229=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp229->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp179); _temp229;});} _LL184:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp232; struct _tagged_arr* _temp233; void* _temp235; struct _tuple0* _temp230=
q; _temp232=* _temp230; _LL236: _temp235= _temp232.f1; goto _LL234; _LL234:
_temp233= _temp232.f2; goto _LL231; _LL231: { void* _temp237= _temp235; struct
Cyc_List_List* _temp247; struct Cyc_List_List* _temp249; struct Cyc_List_List
_temp251; struct Cyc_List_List* _temp252; struct _tagged_arr* _temp254; struct
Cyc_List_List* _temp256; _LL239: if( _temp237 == ( void*) Cyc_Absyn_Loc_n){ goto
_LL240;} else{ goto _LL241;} _LL241: if(( unsigned int) _temp237 >  1u?*(( int*)
_temp237) ==  Cyc_Absyn_Rel_n: 0){ _LL248: _temp247=(( struct Cyc_Absyn_Rel_n_struct*)
_temp237)->f1; if( _temp247 ==  0){ goto _LL242;} else{ goto _LL243;}} else{
goto _LL243;} _LL243: if(( unsigned int) _temp237 >  1u?*(( int*) _temp237) == 
Cyc_Absyn_Rel_n: 0){ _LL250: _temp249=(( struct Cyc_Absyn_Rel_n_struct*)
_temp237)->f1; if( _temp249 ==  0){ goto _LL245;} else{ _temp251=* _temp249;
_LL255: _temp254=( struct _tagged_arr*) _temp251.hd; goto _LL253; _LL253:
_temp252= _temp251.tl; goto _LL244;}} else{ goto _LL245;} _LL245: if((
unsigned int) _temp237 >  1u?*(( int*) _temp237) ==  Cyc_Absyn_Abs_n: 0){ _LL257:
_temp256=(( struct Cyc_Absyn_Abs_n_struct*) _temp237)->f1; goto _LL246;} else{
goto _LL238;} _LL240: goto _LL242; _LL242: return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_enumdecl_f, _temp233);
_LL244: { struct Cyc_Tcenv_Genv* _temp258= Cyc_Tcenv_lookup_namespace( te, loc,
_temp254, _temp252); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp258->enumdecls, _temp233);}
_LL246: { struct Cyc_Tcenv_Genv* _temp259=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp256);
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp259->enumdecls, _temp233);} _LL238:;}}
static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->typedefs, v);}
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp262; struct _tagged_arr* _temp263; void* _temp265; struct _tuple0* _temp260=
q; _temp262=* _temp260; _LL266: _temp265= _temp262.f1; goto _LL264; _LL264:
_temp263= _temp262.f2; goto _LL261; _LL261: { void* _temp267= _temp265; struct
Cyc_List_List* _temp277; struct Cyc_List_List* _temp279; struct Cyc_List_List
_temp281; struct Cyc_List_List* _temp282; struct _tagged_arr* _temp284; struct
Cyc_List_List* _temp286; _LL269: if( _temp267 == ( void*) Cyc_Absyn_Loc_n){ goto
_LL270;} else{ goto _LL271;} _LL271: if(( unsigned int) _temp267 >  1u?*(( int*)
_temp267) ==  Cyc_Absyn_Rel_n: 0){ _LL278: _temp277=(( struct Cyc_Absyn_Rel_n_struct*)
_temp267)->f1; if( _temp277 ==  0){ goto _LL272;} else{ goto _LL273;}} else{
goto _LL273;} _LL273: if(( unsigned int) _temp267 >  1u?*(( int*) _temp267) == 
Cyc_Absyn_Rel_n: 0){ _LL280: _temp279=(( struct Cyc_Absyn_Rel_n_struct*)
_temp267)->f1; if( _temp279 ==  0){ goto _LL275;} else{ _temp281=* _temp279;
_LL285: _temp284=( struct _tagged_arr*) _temp281.hd; goto _LL283; _LL283:
_temp282= _temp281.tl; goto _LL274;}} else{ goto _LL275;} _LL275: if((
unsigned int) _temp267 >  1u?*(( int*) _temp267) ==  Cyc_Absyn_Abs_n: 0){ _LL287:
_temp286=(( struct Cyc_Absyn_Abs_n_struct*) _temp267)->f1; goto _LL276;} else{
goto _LL268;} _LL270: goto _LL272; _LL272: return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_typedefdecl_f, _temp263);
_LL274: { struct Cyc_Tcenv_Genv* _temp288= Cyc_Tcenv_lookup_namespace( te, loc,
_temp284, _temp282); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp288->typedefs, _temp263);}
_LL276: { struct Cyc_Tcenv_Genv* _temp289=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp286);
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp289->typedefs, _temp263);} _LL268:;}}
struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp290=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp290->namespaces=( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Tcutil_empty_var_set))->v; _temp290->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_std_zstrptrcmp);
_temp290->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_std_zstrptrcmp);
_temp290->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_std_zstrptrcmp);
_temp290->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_std_zstrptrcmp); _temp290->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_std_zstrptrcmp); _temp290->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_std_zstrptrcmp);
_temp290->availables= 0; _temp290;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp291= f; void* _temp299; void* _temp301; void* _temp303;
void* _temp305; _LL293: if(*(( int*) _temp291) ==  Cyc_Tcenv_Outermost){ _LL300:
_temp299=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp291)->f1; goto
_LL294;} else{ goto _LL295;} _LL295: if(*(( int*) _temp291) ==  Cyc_Tcenv_Frame){
_LL302: _temp301=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp291)->f1; goto
_LL296;} else{ goto _LL297;} _LL297: if(*(( int*) _temp291) ==  Cyc_Tcenv_Hidden){
_LL306: _temp305=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp291)->f1; goto
_LL304; _LL304: _temp303=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp291)->f2;
goto _LL298;} else{ goto _LL292;} _LL294: return _temp299; _LL296: return
_temp301; _LL298: f= _temp303; goto _LL292; _LL292:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp307= f; void* _temp315; void* _temp317; void*
_temp319; void* _temp321; void* _temp323; _LL309: if(*(( int*) _temp307) ==  Cyc_Tcenv_Outermost){
_LL316: _temp315=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp307)->f1;
goto _LL310;} else{ goto _LL311;} _LL311: if(*(( int*) _temp307) ==  Cyc_Tcenv_Frame){
_LL320: _temp319=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp307)->f1; goto
_LL318; _LL318: _temp317=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp307)->f2;
goto _LL312;} else{ goto _LL313;} _LL313: if(*(( int*) _temp307) ==  Cyc_Tcenv_Hidden){
_LL324: _temp323=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp307)->f1; goto
_LL322; _LL322: _temp321=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp307)->f2;
goto _LL314;} else{ goto _LL308;} _LL310: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp325=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp325[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp326; _temp326.tag= Cyc_Tcenv_Outermost;
_temp326.f1=( void*) x; _temp326;}); _temp325;}); _LL312: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp327=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp327[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp328; _temp328.tag= Cyc_Tcenv_Frame; _temp328.f1=( void*) x; _temp328.f2=(
void*) _temp317; _temp328;}); _temp327;}); _LL314: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp329=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp329[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp330; _temp330.tag= Cyc_Tcenv_Hidden;
_temp330.f1=( void*) _temp323; _temp330.f2=( void*) Cyc_Tcenv_put_visible(
_temp321, x); _temp330;}); _temp329;}); _LL308:;} void* Cyc_Tcenv_apply_to_visible(
void*(* g)( void*), void* f){ return Cyc_Tcenv_put_visible( f, g( Cyc_Tcenv_get_visible(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp331=
f; void* _temp339; void* _temp341; void* _temp343; void* _temp345; void*
_temp347; _LL333: if(*(( int*) _temp331) ==  Cyc_Tcenv_Outermost){ _LL340:
_temp339=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp331)->f1; goto
_LL334;} else{ goto _LL335;} _LL335: if(*(( int*) _temp331) ==  Cyc_Tcenv_Frame){
_LL344: _temp343=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp331)->f1; goto
_LL342; _LL342: _temp341=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp331)->f2;
goto _LL336;} else{ goto _LL337;} _LL337: if(*(( int*) _temp331) ==  Cyc_Tcenv_Hidden){
_LL348: _temp347=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp331)->f1; goto
_LL346; _LL346: _temp345=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp331)->f2;
goto _LL338;} else{ goto _LL332;} _LL334: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp349=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp349[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp350; _temp350.tag= Cyc_Tcenv_Outermost;
_temp350.f1=( void*) g( _temp339); _temp350;}); _temp349;}); _LL336: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp351=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp351[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp352; _temp352.tag= Cyc_Tcenv_Frame; _temp352.f1=( void*) g( _temp343);
_temp352.f2=( void*) Cyc_Tcenv_map_frames( g, _temp341); _temp352;}); _temp351;});
_LL338: return( void*)({ struct Cyc_Tcenv_Hidden_struct* _temp353=( struct Cyc_Tcenv_Hidden_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct)); _temp353[ 0]=({ struct Cyc_Tcenv_Hidden_struct
_temp354; _temp354.tag= Cyc_Tcenv_Hidden; _temp354.f1=( void*) g( _temp347);
_temp354.f2=( void*) Cyc_Tcenv_map_frames( g, _temp345); _temp354;}); _temp353;});
_LL332:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le ==  0){( int) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le ==  0){( int) _throw((
void*) Cyc_Tcenv_Env_error);} return({ struct Cyc_Tcenv_Tenv* _temp355=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp355->ns= te->ns;
_temp355->ae= te->ae; _temp355->le=({ struct Cyc_Core_Opt* _temp356=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp356->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v, fe); _temp356;}); _temp355;});} static int Cyc_Tcenv_region_ancestor(
struct Cyc_List_List* ancestors, struct Cyc_Absyn_Tvar* r){{ struct Cyc_List_List*
a= ancestors; for( 0; a !=  0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
if( Cyc_Absyn_tvar_cmp((( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*)
_check_null( a))->hd)->tvar, r) ==  0){ return 1;}}}{ struct Cyc_List_List* a=
ancestors; for( 0; a !=  0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
if( Cyc_Tcenv_region_ancestor((( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*)
_check_null( a))->hd)->ancestors, r)){ return 1;}}} return 0;} static struct Cyc_Tcenv_RgnPO*
Cyc_Tcenv_find_region_in_po( struct Cyc_List_List* po, struct Cyc_Absyn_Tvar* r){
for( 0; po !=  0; po=(( struct Cyc_List_List*) _check_null( po))->tl){ if( Cyc_Absyn_tvar_cmp(((
struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null( po))->hd)->tvar,
r) ==  0){ return( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null(
po))->hd;}} return(( struct Cyc_Tcenv_RgnPO*(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)((
struct _tagged_arr)({ struct Cyc_std_String_pa_struct _temp358; _temp358.tag=
Cyc_std_String_pa; _temp358.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp359=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp359[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp360; _temp360.tag= Cyc_Absyn_VarType; _temp360.f1= r; _temp360;}); _temp359;}));{
void* _temp357[ 1u]={& _temp358}; Cyc_std_aprintf( _tag_arr("can't find region variable %s in po",
sizeof( unsigned char), 36u), _tag_arr( _temp357, sizeof( void*), 1u));}}));}
static int Cyc_Tcenv_check_region_outlives( struct Cyc_List_List* po, struct Cyc_Absyn_Tvar*
r1, struct Cyc_Absyn_Tvar* r2){ struct Cyc_List_List* r2_ancestors; while( po != 
0? Cyc_Absyn_tvar_cmp((( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*)
_check_null( po))->hd)->tvar, r2) !=  0: 0) { po=(( struct Cyc_List_List*)
_check_null( po))->tl;} if( po ==  0){ return 0;} return Cyc_Tcenv_region_ancestor(((
struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null( po))->hd)->ancestors,
r1);} struct _tuple5{ void* f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tcenv_initial_region_po(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_List_List*
po){ struct Cyc_List_List* result= 0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; if( Cyc_Tcutil_tvar_kind( tv) == (
void*) Cyc_Absyn_RgnKind){ result=({ struct Cyc_List_List* _temp361=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp361->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp362=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp362->tvar= tv; _temp362->ancestors= 0; _temp362;}); _temp361->tl= result;
_temp361;});}} for( 0; po !=  0; po=(( struct Cyc_List_List*) _check_null( po))->tl){
struct _tuple5 _temp363=*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( po))->hd); void* _temp371; void* _temp373; struct Cyc_Absyn_Tvar*
_temp375; void* _temp377; struct Cyc_Absyn_Tvar* _temp379; void* _temp381; void*
_temp383; _LL365: _LL372: _temp371= _temp363.f1; if( _temp371 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL366;} else{ goto _LL367;} _LL367: _LL378: _temp377= _temp363.f1; if((
unsigned int) _temp377 >  4u?*(( int*) _temp377) ==  Cyc_Absyn_VarType: 0){
_LL380: _temp379=(( struct Cyc_Absyn_VarType_struct*) _temp377)->f1; goto _LL374;}
else{ goto _LL369;} _LL374: _temp373= _temp363.f2; if(( unsigned int) _temp373 > 
4u?*(( int*) _temp373) ==  Cyc_Absyn_VarType: 0){ _LL376: _temp375=(( struct Cyc_Absyn_VarType_struct*)
_temp373)->f1; goto _LL368;} else{ goto _LL369;} _LL369: _LL384: _temp383=
_temp363.f1; goto _LL382; _LL382: _temp381= _temp363.f2; goto _LL370; _LL366:
goto _LL364; _LL368: if( Cyc_Absyn_tvar_cmp( _temp379, _temp375) ==  0? 1: Cyc_Tcenv_check_region_outlives(
result, _temp375, _temp379)){({ struct Cyc_std_String_pa_struct _temp387;
_temp387.tag= Cyc_std_String_pa; _temp387.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp390=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp390[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp391; _temp391.tag= Cyc_Absyn_VarType; _temp391.f1= _temp375; _temp391;});
_temp390;}));{ struct Cyc_std_String_pa_struct _temp386; _temp386.tag= Cyc_std_String_pa;
_temp386.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp388=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp388[ 0]=({ struct Cyc_Absyn_VarType_struct _temp389; _temp389.tag= Cyc_Absyn_VarType;
_temp389.f1= _temp379; _temp389;}); _temp388;}));{ void* _temp385[ 2u]={&
_temp386,& _temp387}; Cyc_Tcutil_terr( loc, _tag_arr("bad region lifetime assertion (circularity):%s < %s",
sizeof( unsigned char), 52u), _tag_arr( _temp385, sizeof( void*), 2u));}}});}
else{ struct Cyc_Tcenv_RgnPO* _temp392= Cyc_Tcenv_find_region_in_po( result,
_temp379); struct Cyc_Tcenv_RgnPO* _temp393= Cyc_Tcenv_find_region_in_po( result,
_temp375); _temp393->ancestors=({ struct Cyc_List_List* _temp394=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp394->hd=( void*) _temp392;
_temp394->tl= _temp393->ancestors; _temp394;});} goto _LL364; _LL370:({ struct
Cyc_std_String_pa_struct _temp397; _temp397.tag= Cyc_std_String_pa; _temp397.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( _temp381);{ struct Cyc_std_String_pa_struct
_temp396; _temp396.tag= Cyc_std_String_pa; _temp396.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp383);{ void* _temp395[ 2u]={& _temp396,& _temp397}; Cyc_Tcutil_terr( loc,
_tag_arr("bad region lifetime assertion:: %s < %s", sizeof( unsigned char), 40u),
_tag_arr( _temp395, sizeof( void*), 2u));}}}); goto _LL364; _LL364:;} return
result;} static struct Cyc_List_List* Cyc_Tcenv_add_region_po( struct Cyc_List_List*
po, struct Cyc_Absyn_Tvar* tv){ return({ struct Cyc_List_List* _temp398=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp398->hd=( void*)({
struct Cyc_Tcenv_RgnPO* _temp399=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof(
struct Cyc_Tcenv_RgnPO)); _temp399->tvar= tv; _temp399->ancestors= po; _temp399;});
_temp398->tl= po; _temp398;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl(
struct Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp400=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp400->shared= f->shared;
_temp400->type_vars= f->type_vars; _temp400->region_order= f->region_order;
_temp400->locals= f->locals; _temp400->encloser= f->encloser; _temp400->ctrl_env=
f->ctrl_env; _temp400->capability=( void*)(( void*) f->capability); _temp400->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp400;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl(
struct Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({
struct Cyc_Tcenv_Fenv* _temp401=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Fenv)); _temp401->shared= f->shared; _temp401->type_vars= f->type_vars;
_temp401->region_order= f->region_order; _temp401->locals= f->locals; _temp401->encloser=
f->encloser; _temp401->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp402=( struct
Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp402->continue_stmt=(
void*)(( void*) c->continue_stmt); _temp402->break_stmt=( void*)(( void*) c->break_stmt);
_temp402->fallthru_clause= c->fallthru_clause; _temp402->next_stmt=( void*)((
void*) c->next_stmt); _temp402->try_depth= c->try_depth; _temp402;}); _temp401->capability=(
void*)(( void*) f->capability); _temp401->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp401;});} void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv* te){ if( te->le
==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected return_typ",
sizeof( unsigned char), 29u));} return( void*)(( Cyc_Tcenv_get_fenv( te))->shared)->return_typ;}
struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv* te){
if( te->le ==  0){ return 0;} return( Cyc_Tcenv_get_fenv( te))->type_vars;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le ==  0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_type_vars",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp403= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_List_List*
_temp404=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, _temp403->type_vars); Cyc_Tcutil_check_unique_tvars(
loc, _temp404); _temp403->type_vars= _temp404; return Cyc_Tcenv_put_fenv( te,
_temp403);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le ==  0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_local_var",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp405= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp406=(* vd->name).f2;{ void*
_temp407=(* vd->name).f1; _LL409: if( _temp407 == ( void*) Cyc_Absyn_Loc_n){
goto _LL410;} else{ goto _LL411;} _LL411: goto _LL412; _LL410: goto _LL408;
_LL412:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp413=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp413[ 0]=({ struct Cyc_Core_Impossible_struct _temp414; _temp414.tag= Cyc_Core_Impossible;
_temp414.f1= _tag_arr("add_local_var: called with Rel_n", sizeof( unsigned char),
33u); _temp414;}); _temp413;})); goto _LL408; _LL408:;} _temp405->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void*
data)) Cyc_Dict_insert)( _temp405->locals, _temp406,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp415=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp415[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp416; _temp416.tag= Cyc_Absyn_Local_b;
_temp416.f1= vd; _temp416;}); _temp415;})); return Cyc_Tcenv_put_fenv( te,
_temp405);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le ==  0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_pat_var",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* _temp417= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp418=(* vd->name).f2; _temp417->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void*
data)) Cyc_Dict_insert)( _temp417->locals, _temp418,( void*)({ struct Cyc_Absyn_Pat_b_struct*
_temp419=( struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp419[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp420; _temp420.tag= Cyc_Absyn_Pat_b;
_temp420.f1= vd; _temp420;}); _temp419;})); return Cyc_Tcenv_put_fenv( te,
_temp417);}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp423; struct
_tagged_arr* _temp424; void* _temp426; struct _tuple0* _temp421= q; _temp423=*
_temp421; _LL427: _temp426= _temp423.f1; goto _LL425; _LL425: _temp424= _temp423.f2;
goto _LL422; _LL422: { void* _temp428= _temp426; struct Cyc_List_List* _temp437;
_LL430: if( _temp428 == ( void*) Cyc_Absyn_Loc_n){ goto _LL431;} else{ goto
_LL432;} _LL432: if(( unsigned int) _temp428 >  1u?*(( int*) _temp428) ==  Cyc_Absyn_Rel_n:
0){ _LL438: _temp437=(( struct Cyc_Absyn_Rel_n_struct*) _temp428)->f1; if(
_temp437 ==  0){ goto _LL436;} else{ goto _LL434;}} else{ goto _LL434;} _LL436:
if( te->le !=  0){ goto _LL433;} else{ goto _LL434;} _LL434: goto _LL435; _LL431:
if( te->le ==  0){( int) _throw(( void*) Cyc_Dict_Absent);} else{ goto _LL433;}
_LL433: { struct Cyc_Tcenv_Fenv* _temp439= Cyc_Tcenv_get_fenv( te); struct
_handler_cons _temp440; _push_handler(& _temp440);{ int _temp442= 0; if( setjmp(
_temp440.handler)){ _temp442= 1;} if( ! _temp442){{ void* _temp445=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp443=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp443[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp444; _temp444.tag= Cyc_Tcenv_VarRes; _temp444.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp439->locals,
_temp424); _temp444;}); _temp443;}); _npop_handler( 0u); return _temp445;};
_pop_handler();} else{ void* _temp441=( void*) _exn_thrown; void* _temp447=
_temp441; _LL449: if( _temp447 ==  Cyc_Dict_Absent){ goto _LL450;} else{ goto
_LL451;} _LL451: goto _LL452; _LL450: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL452:( void) _throw( _temp447); _LL448:;}}} _LL435: { struct
_handler_cons _temp453; _push_handler(& _temp453);{ int _temp455= 0; if( setjmp(
_temp453.handler)){ _temp455= 1;} if( ! _temp455){{ void* _temp456= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp456;}; _pop_handler();} else{ void*
_temp454=( void*) _exn_thrown; void* _temp458= _temp454; _LL460: if( _temp458 == 
Cyc_Dict_Absent){ goto _LL461;} else{ goto _LL462;} _LL462: goto _LL463; _LL461:({
struct Cyc_std_String_pa_struct _temp465; _temp465.tag= Cyc_std_String_pa;
_temp465.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{ void* _temp464[
1u]={& _temp465}; Cyc_Tcutil_terr( loc, _tag_arr("unbound identifier: %s",
sizeof( unsigned char), 23u), _tag_arr( _temp464, sizeof( void*), 1u));}});
return( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp466=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp466[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp467; _temp467.tag= Cyc_Tcenv_VarRes; _temp467.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b);
_temp467;}); _temp466;}); _LL463:( void) _throw( _temp458); _LL459:;}}} _LL429:;}}
void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Stmt** sopt){ if( te->le ==  0){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_continue",
sizeof( unsigned char), 35u));}{ void* _temp468=( void*)(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->continue_stmt; struct Cyc_Absyn_Stmt* _temp478; _LL470: if((
unsigned int) _temp468 >  3u?*(( int*) _temp468) ==  Cyc_Tcenv_Stmt_j: 0){
_LL479: _temp478=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp468)->f1; goto _LL471;}
else{ goto _LL472;} _LL472: if( _temp468 == ( void*) Cyc_Tcenv_NotLoop_j){ goto
_LL473;} else{ goto _LL474;} _LL474: if( _temp468 == ( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL475;} else{ goto _LL476;} _LL476: if( _temp468 == ( void*) Cyc_Tcenv_FnEnd_j){
goto _LL477;} else{ goto _LL469;} _LL471: _temp478->non_local_preds=({ struct
Cyc_List_List* _temp480=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp480->hd=( void*) s; _temp480->tl= _temp478->non_local_preds; _temp480;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp478; return; _LL473:({ void* _temp481[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("continue not in a loop", sizeof(
unsigned char), 23u), _tag_arr( _temp481, sizeof( void*), 0u));}); return;
_LL475:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd",
sizeof( unsigned char), 32u)); return; _LL477:(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd", sizeof(
unsigned char), 32u)); return; _LL469:;}} void Cyc_Tcenv_process_break( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt** sopt){ if(
te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_break",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv(
te); void* _temp482=( void*)( fe->ctrl_env)->break_stmt; struct Cyc_Absyn_Stmt*
_temp492; _LL484: if(( unsigned int) _temp482 >  3u?*(( int*) _temp482) ==  Cyc_Tcenv_Stmt_j:
0){ _LL493: _temp492=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp482)->f1; goto
_LL485;} else{ goto _LL486;} _LL486: if( _temp482 == ( void*) Cyc_Tcenv_NotLoop_j){
goto _LL487;} else{ goto _LL488;} _LL488: if( _temp482 == ( void*) Cyc_Tcenv_FnEnd_j){
goto _LL489;} else{ goto _LL490;} _LL490: if( _temp482 == ( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL491;} else{ goto _LL483;} _LL485: _temp492->non_local_preds=({ struct
Cyc_List_List* _temp494=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp494->hd=( void*) s; _temp494->tl= _temp492->non_local_preds; _temp494;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp492; return; _LL487:({ void* _temp495[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("break not in a loop or switch", sizeof(
unsigned char), 30u), _tag_arr( _temp495, sizeof( void*), 0u));}); return;
_LL489: if(( void*)( fe->shared)->return_typ != ( void*) Cyc_Absyn_VoidType){({
void* _temp496[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("break causes function not to return a value",
sizeof( unsigned char), 44u), _tag_arr( _temp496, sizeof( void*), 0u));});}
return; _LL491:({ void* _temp497[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("break causes outer switch clause to implicitly fallthru",
sizeof( unsigned char), 56u), _tag_arr( _temp497, sizeof( void*), 0u));});
return; _LL483:;}} void Cyc_Tcenv_process_goto( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, struct _tagged_arr* l, struct Cyc_Absyn_Stmt** sopt){
if( te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_goto", sizeof( unsigned char), 31u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te); struct Cyc_Core_Opt*
_temp498=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l); if( _temp498 ==  0){
struct Cyc_SlowDict_Dict* _temp499=( fe->shared)->needed_labels; struct Cyc_Core_Opt*
_temp500=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict* d, struct
_tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp499, l); if( _temp500 ==  0){
_temp500=({ struct Cyc_Core_Opt* _temp501=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp501->v=( void*) 0; _temp501;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_arr*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( _temp499, l,({ struct
Cyc_List_List* _temp502=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp502->hd=( void*) s; _temp502->tl=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp500))->v; _temp502;}));} else{(( struct Cyc_Absyn_Stmt*)((
struct Cyc_Core_Opt*) _check_null( _temp498))->v)->non_local_preds=({ struct Cyc_List_List*
_temp503=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp503->hd=( void*) s; _temp503->tl=(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*)
_check_null( _temp498))->v)->non_local_preds; _temp503;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*) _check_null( _temp498))->v);}}}
struct _tuple3* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause*** clauseopt){ if( te->le == 
0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_fallthru",
sizeof( unsigned char), 35u));}{ struct Cyc_Tcenv_Fenv* _temp504= Cyc_Tcenv_get_fenv(
te); struct _tuple3* _temp505=( _temp504->ctrl_env)->fallthru_clause; if(
_temp505 !=  0){(((*(( struct _tuple3*) _check_null( _temp505))).f1)->body)->non_local_preds=({
struct Cyc_List_List* _temp506=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp506->hd=( void*) s; _temp506->tl=(((*(( struct
_tuple3*) _check_null( _temp505))).f1)->body)->non_local_preds; _temp506;});*
clauseopt=( struct Cyc_Absyn_Switch_clause**)({ struct Cyc_Absyn_Switch_clause**
_temp507=( struct Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*)
*  1); _temp507[ 0]=(*(( struct _tuple3*) _check_null( _temp505))).f1; _temp507;});}
return _temp505;}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause*
clause){ if( te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_fallthru", sizeof( unsigned char), 31u));}{
struct Cyc_List_List* _temp508= 0; for( 0; vds !=  0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ _temp508=({ struct Cyc_List_List* _temp509=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp509->hd=( void*)(( void*)((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->type);
_temp509->tl= _temp508; _temp509;});} _temp508= Cyc_List_imp_rev( _temp508);{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));(
fe->ctrl_env)->fallthru_clause=({ struct _tuple3* _temp510=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp510->f1= clause; _temp510->f2= new_tvs;
_temp510->f3= _temp508; _temp510;}); return Cyc_Tcenv_put_fenv( te, fe);}}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv* te){ if(
te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected clear_fallthru",
sizeof( unsigned char), 33u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause= 0; return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==  0){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_loop",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp511=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp511[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp512; _temp512.tag= Cyc_Tcenv_Stmt_j; _temp512.f1= continue_dest; _temp512;});
_temp511;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp513=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp513[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp514; _temp514.tag= Cyc_Tcenv_Stmt_j;
_temp514.f1= continue_dest; _temp514;}); _temp513;}))); return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){
if( te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected enter_try", sizeof( unsigned char), 28u));}{ struct
Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te)); ++(
fe->ctrl_env)->try_depth; return Cyc_Tcenv_put_fenv( te, fe);}} int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv* te){ if( te->le ==  0){(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected get_try_depth", sizeof(
unsigned char), 32u));} return(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->try_depth;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct Cyc_Tcenv_Tenv* te){ if(
te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_switch",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)(
fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)
Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){ if( te->le ==  0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_next",
sizeof( unsigned char), 27u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=( void*) j); return
Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s){ if(
te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_label",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* _temp515= Cyc_Tcenv_get_fenv(
te); struct Cyc_SlowDict_Dict* _temp516=( _temp515->shared)->needed_labels;
struct Cyc_Core_Opt* _temp517=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp516, v); if(
_temp517 !=  0){( _temp515->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_arr* key)) Cyc_SlowDict_delete)(
_temp516, v);{ struct Cyc_List_List* _temp518=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp517))->v; s->non_local_preds= _temp518; for( 0;
_temp518 !=  0; _temp518=(( struct Cyc_List_List*) _check_null( _temp518))->tl){
void* _temp519=( void*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*)
_check_null( _temp518))->hd)->r; struct Cyc_Absyn_Stmt* _temp525; struct Cyc_Absyn_Stmt**
_temp527; _LL521: if(( unsigned int) _temp519 >  1u?*(( int*) _temp519) ==  Cyc_Absyn_Goto_s:
0){ _LL526: _temp525=(( struct Cyc_Absyn_Goto_s_struct*) _temp519)->f2; _temp527=&((
struct Cyc_Absyn_Goto_s_struct*) _temp519)->f2; goto _LL522;} else{ goto _LL523;}
_LL523: goto _LL524; _LL522:* _temp527=( struct Cyc_Absyn_Stmt*) s; goto _LL520;
_LL524:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: add_label backpatching of non-goto",
sizeof( unsigned char), 42u)); goto _LL520; _LL520:;}}} if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_member)(( _temp515->shared)->seen_labels,
v)){({ struct Cyc_std_String_pa_struct _temp529; _temp529.tag= Cyc_std_String_pa;
_temp529.f1=( struct _tagged_arr)* v;{ void* _temp528[ 1u]={& _temp529}; Cyc_Tcutil_terr(
s->loc, _tag_arr("Repeated label: %s", sizeof( unsigned char), 19u), _tag_arr(
_temp528, sizeof( void*), 1u));}});}( _temp515->shared)->seen_labels=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Stmt*
data)) Cyc_Dict_insert)(( _temp515->shared)->seen_labels, v, s); return te;}}
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv* te){ return(( int(*)(
struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)((( Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);}
struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv* te){ if(
te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected get_encloser",
sizeof( unsigned char), 31u));} return( Cyc_Tcenv_get_fenv( te))->encloser;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Stmt* s){ if( te->le ==  0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_encloser", sizeof( unsigned char), 31u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te));
fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){ if( te->le ==  0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv::add_region: unexpected add_region",
sizeof( unsigned char), 41u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar* tv;{ void* _temp530= Cyc_Tcutil_compress(
r); struct Cyc_Absyn_Tvar* _temp536; _LL532: if(( unsigned int) _temp530 >  4u?*((
int*) _temp530) ==  Cyc_Absyn_VarType: 0){ _LL537: _temp536=(( struct Cyc_Absyn_VarType_struct*)
_temp530)->f1; goto _LL533;} else{ goto _LL534;} _LL534: goto _LL535; _LL533: tv=
_temp536; goto _LL531; _LL535: tv=(( struct Cyc_Absyn_Tvar*(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("bad add region", sizeof(
unsigned char), 15u)); goto _LL531; _LL531:;} fe->region_order= Cyc_Tcenv_add_region_po(
fe->region_order, tv);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp538=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp538[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp539; _temp539.tag= Cyc_Absyn_JoinEff;
_temp539.f1=({ struct Cyc_List_List* _temp540=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp540->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp542=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp542[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp543; _temp543.tag= Cyc_Absyn_AccessEff;
_temp543.f1=( void*) r; _temp543;}); _temp542;})); _temp540->tl=({ struct Cyc_List_List*
_temp541=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp541->hd=( void*)(( void*) fe->capability); _temp541->tl= 0; _temp541;});
_temp540;}); _temp539;}); _temp538;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le == 
0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected new_block",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp551=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp551[ 0]=({ struct Cyc_Absyn_VarType_struct _temp552; _temp552.tag= Cyc_Absyn_VarType;
_temp552.f1= block_rgn; _temp552;}); _temp551;}); fe->type_vars=({ struct Cyc_List_List*
_temp544=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp544->hd=( void*) block_rgn; _temp544->tl= fe->type_vars; _temp544;}); Cyc_Tcutil_check_unique_tvars(
loc, fe->type_vars); fe->region_order= Cyc_Tcenv_add_region_po( fe->region_order,
block_rgn);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp545=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp545[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp546; _temp546.tag= Cyc_Absyn_JoinEff;
_temp546.f1=({ struct Cyc_List_List* _temp547=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp547->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp549=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp549[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp550; _temp550.tag= Cyc_Absyn_AccessEff;
_temp550.f1=( void*) block_typ; _temp550;}); _temp549;})); _temp547->tl=({
struct Cyc_List_List* _temp548=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp548->hd=( void*)(( void*) fe->capability); _temp548->tl=
0; _temp548;}); _temp547;}); _temp546;}); _temp545;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
struct Cyc_Absyn_Tvar* t= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); Cyc_Tcutil_add_tvar_identity(
t); return Cyc_Tcenv_new_named_block( loc, te, t);} void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv* te){ if( te->le ==  0){ return( void*) Cyc_Absyn_HeapRgn;}
return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;} void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* rgn){ if( te->le
!=  0){ void* capability=( void*)( Cyc_Tcenv_get_fenv( te))->capability; if( Cyc_Tcutil_region_in_effect(
0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect( 1, rgn, capability)){
return;}}({ struct Cyc_std_String_pa_struct _temp554; _temp554.tag= Cyc_std_String_pa;
_temp554.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( rgn);{ void* _temp553[
1u]={& _temp554}; Cyc_Tcutil_terr( loc, _tag_arr("Expression accesses unavailable region %s",
sizeof( unsigned char), 42u), _tag_arr( _temp553, sizeof( void*), 1u));}});} int
Cyc_Tcenv_region_outlives( struct Cyc_Tcenv_Tenv* te, void* rt_a, void* rt_b){
rt_a= Cyc_Tcutil_compress( rt_a); rt_b= Cyc_Tcutil_compress( rt_b); if( rt_a == (
void*) Cyc_Absyn_HeapRgn){ return 1;} if( te->le !=  0){ struct _tuple5 _temp556=({
struct _tuple5 _temp555; _temp555.f1= rt_a; _temp555.f2= rt_b; _temp555;}); void*
_temp562; struct Cyc_Absyn_Tvar* _temp564; void* _temp566; struct Cyc_Absyn_Tvar*
_temp568; _LL558: _LL567: _temp566= _temp556.f1; if(( unsigned int) _temp566 > 
4u?*(( int*) _temp566) ==  Cyc_Absyn_VarType: 0){ _LL569: _temp568=(( struct Cyc_Absyn_VarType_struct*)
_temp566)->f1; goto _LL563;} else{ goto _LL560;} _LL563: _temp562= _temp556.f2;
if(( unsigned int) _temp562 >  4u?*(( int*) _temp562) ==  Cyc_Absyn_VarType: 0){
_LL565: _temp564=(( struct Cyc_Absyn_VarType_struct*) _temp562)->f1; goto _LL559;}
else{ goto _LL560;} _LL560: goto _LL561; _LL559: return Cyc_Tcenv_check_region_outlives((
Cyc_Tcenv_get_fenv( te))->region_order, _temp568, _temp564); _LL561: return 0;
_LL557:;} return 0;} void Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* eff){ void* capability; if( te->le
==  0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp570=( struct
Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp570[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp571; _temp571.tag= Cyc_Absyn_JoinEff;
_temp571.f1= 0; _temp571;}); _temp570;});} else{ struct Cyc_Tcenv_Fenv* _temp572=
Cyc_Tcenv_get_fenv( te); capability=( void*) _temp572->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){({ struct Cyc_std_String_pa_struct _temp575; _temp575.tag=
Cyc_std_String_pa; _temp575.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( eff);{
struct Cyc_std_String_pa_struct _temp574; _temp574.tag= Cyc_std_String_pa;
_temp574.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( capability);{ void*
_temp573[ 2u]={& _temp574,& _temp575}; Cyc_Tcutil_terr( loc, _tag_arr("Capability %s does not cover function's effect %s",
sizeof( unsigned char), 50u), _tag_arr( _temp573, sizeof( void*), 2u));}}});}}
void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* po){ for( 0; po !=  0; po=(( struct Cyc_List_List*)
_check_null( po))->tl){ struct _tuple5 _temp578; void* _temp579; void* _temp581;
struct _tuple5* _temp576=( struct _tuple5*)(( struct Cyc_List_List*) _check_null(
po))->hd; _temp578=* _temp576; _LL582: _temp581= _temp578.f1; goto _LL580;
_LL580: _temp579= _temp578.f2; goto _LL577; _LL577: if( ! Cyc_Tcenv_region_outlives(
te, _temp581, _temp579)){({ struct Cyc_std_String_pa_struct _temp585; _temp585.tag=
Cyc_std_String_pa; _temp585.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp579);{ struct Cyc_std_String_pa_struct _temp584; _temp584.tag= Cyc_std_String_pa;
_temp584.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp581);{ void*
_temp583[ 2u]={& _temp584,& _temp585}; Cyc_Tcutil_terr( loc, _tag_arr("function requires that region %s outlive %s",
sizeof( unsigned char), 44u), _tag_arr( _temp583, sizeof( void*), 2u));}}});}}}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_std_zstrptrcmp);
struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar* _temp639=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp639->name=({ struct _tagged_arr*
_temp640=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp640[ 0]=( struct _tagged_arr)({ struct Cyc_std_String_pa_struct _temp642;
_temp642.tag= Cyc_std_String_pa; _temp642.f1=( struct _tagged_arr)*(* fd->name).f2;{
void* _temp641[ 1u]={& _temp642}; Cyc_std_aprintf( _tag_arr("`%s", sizeof(
unsigned char), 4u), _tag_arr( _temp641, sizeof( void*), 1u));}}); _temp640;});
_temp639->identity= Cyc_Tcutil_new_tvar_id(); _temp639->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp639;}); struct Cyc_List_List* _temp586=({ struct
Cyc_List_List* _temp638=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp638->hd=( void*) rgn0; _temp638->tl= fd->tvs; _temp638;}); Cyc_Tcutil_check_unique_tvars(
loc, _temp586);{ struct Cyc_List_List* _temp587= Cyc_Tcenv_add_region_po( Cyc_Tcenv_initial_region_po(
loc, fd->tvs, fd->rgn_po), rgn0); void* param_rgn=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp636=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp636[ 0]=({ struct Cyc_Absyn_VarType_struct _temp637; _temp637.tag= Cyc_Absyn_VarType;
_temp637.f1= rgn0; _temp637;}); _temp636;}); struct Cyc_List_List* _temp588= 0;{
struct Cyc_List_List* _temp589= fd->args; for( 0; _temp589 !=  0; _temp589=((
struct Cyc_List_List*) _check_null( _temp589))->tl){ struct Cyc_Absyn_Vardecl*
_temp590=({ struct Cyc_Absyn_Vardecl* _temp595=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) *  1); _temp595[ 0]=({ struct Cyc_Absyn_Vardecl
_temp596; _temp596.sc=( void*)(( void*) Cyc_Absyn_Public); _temp596.name=({
struct _tuple0* _temp599=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) * 
1); _temp599[ 0]=({ struct _tuple0 _temp600; _temp600.f1=( void*) Cyc_Absyn_Loc_n;
_temp600.f2=(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp589))->hd)).f1;
_temp600;}); _temp599;}); _temp596.tq=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( _temp589))->hd)).f2; _temp596.type=( void*)(*(( struct _tuple6*)((
struct Cyc_List_List*) _check_null( _temp589))->hd)).f3; _temp596.initializer= 0;
_temp596.rgn=({ struct Cyc_Core_Opt* _temp597=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) *  1); _temp597[ 0]=({ struct Cyc_Core_Opt _temp598;
_temp598.v=( void*) param_rgn; _temp598;}); _temp597;}); _temp596.attributes= 0;
_temp596;}); _temp595;}); _temp588=({ struct Cyc_List_List* _temp591=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) *  1); _temp591[ 0]=({
struct Cyc_List_List _temp592; _temp592.hd=( void*) _temp590; _temp592.tl=
_temp588; _temp592;}); _temp591;}); locals=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,(*((
struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp589))->hd)).f1,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp593=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp593[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp594; _temp594.tag= Cyc_Absyn_Param_b; _temp594.f1= _temp590; _temp594;});
_temp593;}));}} if( fd->cyc_varargs !=  0){ int _temp603; void* _temp605; void*
_temp607; struct Cyc_Absyn_Tqual _temp609; struct Cyc_Core_Opt* _temp611; struct
Cyc_Absyn_VarargInfo _temp601=*(( struct Cyc_Absyn_VarargInfo*) _check_null( fd->cyc_varargs));
_LL612: _temp611= _temp601.name; goto _LL610; _LL610: _temp609= _temp601.tq;
goto _LL608; _LL608: _temp607=( void*) _temp601.type; goto _LL606; _LL606:
_temp605=( void*) _temp601.rgn; goto _LL604; _LL604: _temp603= _temp601.inject;
goto _LL602; _LL602: if( _temp611 !=  0){ void* _temp613= Cyc_Absyn_tagged_typ(
_temp607, _temp605, _temp609); struct Cyc_Absyn_Vardecl* _temp614=({ struct Cyc_Absyn_Vardecl*
_temp618=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)
*  1); _temp618[ 0]=({ struct Cyc_Absyn_Vardecl _temp619; _temp619.sc=( void*)((
void*) Cyc_Absyn_Public); _temp619.name=({ struct _tuple0* _temp622=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0) *  1); _temp622[ 0]=({ struct
_tuple0 _temp623; _temp623.f1=( void*) Cyc_Absyn_Loc_n; _temp623.f2=( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp611))->v; _temp623;});
_temp622;}); _temp619.tq= Cyc_Absyn_empty_tqual(); _temp619.type=( void*)
_temp613; _temp619.initializer= 0; _temp619.rgn=({ struct Cyc_Core_Opt* _temp620=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) *  1); _temp620[ 0]=({
struct Cyc_Core_Opt _temp621; _temp621.v=( void*) param_rgn; _temp621;});
_temp620;}); _temp619.attributes= 0; _temp619;}); _temp618;}); _temp588=({
struct Cyc_List_List* _temp615=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp615->hd=( void*) _temp614; _temp615->tl= _temp588;
_temp615;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp611))->v,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp616=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp616[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp617; _temp617.tag= Cyc_Absyn_Param_b;
_temp617.f1= _temp614; _temp617;}); _temp616;}));} else{({ void* _temp624[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("missing name for varargs", sizeof( unsigned char),
25u), _tag_arr( _temp624, sizeof( void*), 0u));});}} fd->param_vardecls=({
struct Cyc_Core_Opt* _temp625=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) *  1); _temp625[ 0]=({ struct Cyc_Core_Opt _temp626; _temp626.v=(
void*) _temp588; _temp626;}); _temp625;}); return({ struct Cyc_Tcenv_Fenv*
_temp627=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp627->shared=({ struct Cyc_Tcenv_SharedFenv* _temp635=( struct Cyc_Tcenv_SharedFenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv)); _temp635->return_typ=( void*)((
void*) fd->ret_type); _temp635->seen_labels=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_std_zstrptrcmp);
_temp635->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_SlowDict_empty)( Cyc_std_zstrptrcmp);
_temp635;}); _temp627->type_vars= _temp586; _temp627->region_order= _temp587;
_temp627->locals= locals; _temp627->encloser= fd->body; _temp627->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp634=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp634->continue_stmt=( void*)(( void*)
Cyc_Tcenv_NotLoop_j); _temp634->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp634->fallthru_clause= 0; _temp634->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp634->try_depth= 0; _temp634;}); _temp627->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp628=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp628[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp629; _temp629.tag= Cyc_Absyn_JoinEff; _temp629.f1=({ struct Cyc_List_List*
_temp630=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp630->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp632=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp632[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp633; _temp633.tag= Cyc_Absyn_AccessEff;
_temp633.f1=( void*) param_rgn; _temp633;}); _temp632;})); _temp630->tl=({
struct Cyc_List_List* _temp631=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp631->hd=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( fd->effect))->v); _temp631->tl= 0; _temp631;}); _temp630;});
_temp629;}); _temp628;})); _temp627->curr_rgn=( void*) param_rgn; _temp627;});}}
