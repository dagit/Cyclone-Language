 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern void* Cyc_Core_identity( void*); extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern unsigned char
Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u];
struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{ int
tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{
int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{
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
name; struct Cyc_Absyn_Tqual tq; void* type; int inject; } ; struct Cyc_Absyn_FnInfo{
struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct
Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs;
struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern struct _tuple0* Cyc_Absyn_exn_name; extern
struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud; extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp*
init); extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct
Cyc_Core_Opt* eff_typ, void* ret_typ, struct Cyc_List_List* args, int c_varargs,
struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_List_List* rgn_po, struct
Cyc_List_List*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); extern void* Cyc_Dict_fold( void*(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d, void* accum); extern void Cyc_Dict_iter_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct
Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_Frame=
1; struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init();
struct Cyc_Interface_I; extern struct Cyc_Interface_I* Cyc_Interface_empty();
extern struct Cyc_Interface_I* Cyc_Interface_final(); extern struct Cyc_Interface_I*
Cyc_Interface_extract( struct Cyc_Dict_Dict* ae); struct _tuple3{ struct
_tagged_arr f1; struct _tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info);
extern struct Cyc_Interface_I* Cyc_Interface_merge( struct Cyc_Interface_I* i1,
struct Cyc_Interface_I* i2, struct _tuple3* info); extern struct Cyc_Interface_I*
Cyc_Interface_merge_list( struct Cyc_List_List* li, struct Cyc_List_List* linfo);
extern struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(*
get)( void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo); extern
void Cyc_Interface_print( struct Cyc_Interface_I*, struct Cyc_Std___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Std___sFILE*);
extern void Cyc_Interface_save( struct Cyc_Interface_I*, struct Cyc_Std___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Std___sFILE*);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f); extern struct _tagged_arr
Cyc_Absynpp_qvar2string( struct _tuple0*); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int var_default_init, struct Cyc_List_List* ds); extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_arr lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_arr, int, void*); void* read_fun_state; } ;
struct Cyc_Lexing_lex_tables{ struct _tagged_arr lex_base; struct _tagged_arr
lex_backtrk; struct _tagged_arr lex_default; struct _tagged_arr lex_trans;
struct _tagged_arr lex_check; } ; extern struct Cyc_List_List* Cyc_Parse_parse_file(
struct Cyc_Std___sFILE* f); struct Cyc_Declaration_spec; struct Cyc_Declarator;
struct Cyc_Abstractdeclarator; extern unsigned char Cyc_AbstractDeclarator_tok[
27u]; struct Cyc_AbstractDeclarator_tok_struct{ unsigned char* tag; struct Cyc_Abstractdeclarator*
f1; } ; extern unsigned char Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[
18u]; struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{ unsigned char* tag;
int f1; } ; extern unsigned char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{
unsigned char* tag; unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[
17u]; struct Cyc_DeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_DeclSpec_tok[ 17u]; struct Cyc_DeclSpec_tok_struct{
unsigned char* tag; struct Cyc_Declaration_spec* f1; } ; extern unsigned char
Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple4{ struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple4* f1;
} ; extern unsigned char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{
unsigned char* tag; struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[
23u]; struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple5{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple5* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct Cyc_InitDecl_tok_struct{
unsigned char* tag; struct _tuple4* f1; } ; extern unsigned char Cyc_InitializerList_tok[
24u]; struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple6{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple6* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple7{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple7* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple8{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple8* f1; } ; extern unsigned char Cyc_Rgnorder_tok[
17u]; struct Cyc_Rgnorder_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Short_tok[ 14u];
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; extern
unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{ unsigned char*
tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[ 18u];
struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[
28u]; struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchCClauseList_tok[
26u]; struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[ 25u]; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern unsigned char
Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ unsigned char* tag;
struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; extern int Cyc_Std_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2);
extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_is_function_type( void* t); extern unsigned char
Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl*
base; struct Cyc_Absyn_Tunionfield* field; } ; extern void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg0, struct _tagged_arr
msg1); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct
Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict* vardecls; struct Cyc_Dict_Dict*
xtunionfielddecls; } ; struct Cyc_Interface_I{ struct Cyc_Dict_Dict* imports;
struct Cyc_Dict_Dict* exports; } ; static struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){
return({ struct Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*)
_cycalloc( sizeof( struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp); _temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp0->enumdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); _temp0;});} static struct Cyc_Interface_Ienv*
Cyc_Interface_lazy_new_ienv(){ static struct Cyc_Interface_Ienv* ie= 0; if( ie
==  0){ ie=( struct Cyc_Interface_Ienv*) Cyc_Interface_new_ienv();} return(
struct Cyc_Interface_Ienv*) _check_null( ie);} static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f( void* ignored){ return Cyc_Interface_new_ienv();} static
struct Cyc_Interface_I* Cyc_Interface_skel2i( struct Cyc_Dict_Dict* skel){
return({ struct Cyc_Interface_I* _temp1=( struct Cyc_Interface_I*) _cycalloc(
sizeof( struct Cyc_Interface_I)); _temp1->imports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1;});} struct Cyc_Interface_I* Cyc_Interface_empty(){
return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*) _cycalloc(
sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp); _temp2->exports=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp2;});} struct Cyc_Interface_I* Cyc_Interface_final(){ struct Cyc_Interface_I*
_temp3= Cyc_Interface_empty(); struct Cyc_Interface_Ienv* _temp4= Cyc_Interface_new_ienv();
struct Cyc_Absyn_Tuniondecl* _temp5=({ struct Cyc_Absyn_Tuniondecl* _temp20=(
struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp20[ 0]=* Cyc_Absyn_exn_tud; _temp20;});( void*)( _temp5->sc=( void*)(( void*)
Cyc_Absyn_Public)); _temp4->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp4->tuniondecls,(* Cyc_Absyn_exn_name).f2, _temp5);{ struct Cyc_List_List*
_temp6=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_exn_tud->fields))->v;
for( 0; _temp6 !=  0; _temp6=(( struct Cyc_List_List*) _check_null( _temp6))->tl){
struct Cyc_Absyn_Tunionfield* _temp7=({ struct Cyc_Absyn_Tunionfield* _temp10=(
struct Cyc_Absyn_Tunionfield*) _cycalloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp10[ 0]=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp6))->hd); _temp10;});( void*)( _temp7->sc=( void*)(( void*)
Cyc_Absyn_Public));{ struct Cyc_Tcdecl_Xtunionfielddecl* _temp8=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp9=( struct Cyc_Tcdecl_Xtunionfielddecl*) _cycalloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp9->base= _temp5; _temp9->field= _temp7; _temp9;}); _temp4->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( _temp4->xtunionfielddecls,(*((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp6))->hd)->name).f2,
_temp8);}}} _temp3->exports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp4);{ struct Cyc_Interface_Ienv* _temp11= Cyc_Interface_new_ienv();
struct _tuple0* qmain=({ struct _tuple0* _temp17=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp17->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp18=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp18[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp19; _temp19.tag= Cyc_Absyn_Abs_n;
_temp19.f1= 0; _temp19;}); _temp18;}); _temp17->f2= _init_tag_arr(( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"main", sizeof(
unsigned char), 5u); _temp17;}); struct Cyc_Absyn_Vardecl* _temp12= Cyc_Absyn_new_vardecl(
qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({ struct Cyc_List_List*
_temp13=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp13->hd=( void*)({ struct _tuple1* _temp16=( struct _tuple1*) _cycalloc(
sizeof( struct _tuple1)); _temp16->f1= 0; _temp16->f2= Cyc_Absyn_empty_tqual();
_temp16->f3= Cyc_Absyn_uint_t; _temp16;}); _temp13->tl=({ struct Cyc_List_List*
_temp14=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp14->hd=( void*)({ struct _tuple1* _temp15=( struct _tuple1*) _cycalloc(
sizeof( struct _tuple1)); _temp15->f1= 0; _temp15->f2= Cyc_Absyn_empty_tqual();
_temp15->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp15;}); _temp14->tl= 0; _temp14;}); _temp13;}), 0,
0, 0, 0), 0);( void*)( _temp12->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp11->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp11->vardecls,(* qmain).f2,
_temp12); _temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp11); return _temp3;}} static void Cyc_Interface_err(
struct _tagged_arr msg){({ void* _temp21[ 0u]={}; Cyc_Tcutil_terr( 0, msg,
_tag_arr( _temp21, sizeof( void*), 0u));});} static void Cyc_Interface_fields_err(
struct _tagged_arr sc, struct _tagged_arr t, struct _tuple0* n){ Cyc_Interface_err((
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp25; _temp25.tag= Cyc_Std_String_pa;
_temp25.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( n);{ struct Cyc_Std_String_pa_struct
_temp24; _temp24.tag= Cyc_Std_String_pa; _temp24.f1=( struct _tagged_arr) t;{
struct Cyc_Std_String_pa_struct _temp23; _temp23.tag= Cyc_Std_String_pa; _temp23.f1=(
struct _tagged_arr) sc;{ void* _temp22[ 3u]={& _temp23,& _temp24,& _temp25}; Cyc_Std_aprintf(
_tag_arr("fields of %s %s %s have never been defined", sizeof( unsigned char),
43u), _tag_arr( _temp22, sizeof( void*), 3u));}}}}));} static void Cyc_Interface_body_err(
struct _tagged_arr sc, struct _tuple0* n){ Cyc_Interface_err(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp28; _temp28.tag= Cyc_Std_String_pa;
_temp28.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( n);{ struct Cyc_Std_String_pa_struct
_temp27; _temp27.tag= Cyc_Std_String_pa; _temp27.f1=( struct _tagged_arr) sc;{
void* _temp26[ 2u]={& _temp27,& _temp28}; Cyc_Std_aprintf( _tag_arr("the body of %s function %s has never been defined",
sizeof( unsigned char), 50u), _tag_arr( _temp26, sizeof( void*), 2u));}}}));}
static void Cyc_Interface_static_err( struct _tagged_arr obj1, struct _tuple0*
name1, struct _tagged_arr obj2, struct _tuple0* name2){ if( obj1.curr != ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Interface_err(( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp33; _temp33.tag= Cyc_Std_String_pa;
_temp33.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( name2);{ struct Cyc_Std_String_pa_struct
_temp32; _temp32.tag= Cyc_Std_String_pa; _temp32.f1=( struct _tagged_arr) obj2;{
struct Cyc_Std_String_pa_struct _temp31; _temp31.tag= Cyc_Std_String_pa; _temp31.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( name1);{ struct Cyc_Std_String_pa_struct
_temp30; _temp30.tag= Cyc_Std_String_pa; _temp30.f1=( struct _tagged_arr) obj1;{
void* _temp29[ 4u]={& _temp30,& _temp31,& _temp32,& _temp33}; Cyc_Std_aprintf(
_tag_arr("declaration of %s %s relies on static %s %s", sizeof( unsigned char),
44u), _tag_arr( _temp29, sizeof( void*), 4u));}}}}}));}} static void Cyc_Interface_abstract_err(
struct _tagged_arr obj1, struct _tuple0* name1, struct _tagged_arr obj2, struct
_tuple0* name2){ if( obj1.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
Cyc_Interface_err(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp38; _temp38.tag= Cyc_Std_String_pa; _temp38.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
name2);{ struct Cyc_Std_String_pa_struct _temp37; _temp37.tag= Cyc_Std_String_pa;
_temp37.f1=( struct _tagged_arr) obj2;{ struct Cyc_Std_String_pa_struct _temp36;
_temp36.tag= Cyc_Std_String_pa; _temp36.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
name1);{ struct Cyc_Std_String_pa_struct _temp35; _temp35.tag= Cyc_Std_String_pa;
_temp35.f1=( struct _tagged_arr) obj1;{ void* _temp34[ 4u]={& _temp35,& _temp36,&
_temp37,& _temp38}; Cyc_Std_aprintf( _tag_arr("declaration of %s %s relies on fields of abstract %s %s",
sizeof( unsigned char), 56u), _tag_arr( _temp34, sizeof( void*), 4u));}}}}}));}}
struct Cyc_Interface_Seen{ struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict*
unions; struct Cyc_Dict_Dict* tunions; } ; static struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){
return({ struct Cyc_Interface_Seen* _temp39=( struct Cyc_Interface_Seen*)
_cycalloc( sizeof( struct Cyc_Interface_Seen)); _temp39->structs=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp39->unions=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp39->tunions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp39;});} static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr
obj, struct _tuple0* name, void* t); static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l){ int
_temp40= 1; for( 0; l !=  0; l=(( struct Cyc_List_List*) _check_null( l))->tl){
if( ! Cyc_Interface_check_public_type( ae, seen, obj, name, f(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd))){ _temp40= 0;}} return _temp40;} static
int Cyc_Interface_check_public_structdecl( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Structdecl* d){ struct _handler_cons _temp41;
_push_handler(& _temp41);{ int _temp43= 0; if( setjmp( _temp41.handler)){
_temp43= 1;} if( ! _temp43){{ int _temp44=(( int(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u); return
_temp44;}; _pop_handler();} else{ void* _temp42=( void*) _exn_thrown; void*
_temp46= _temp42; _LL48: if( _temp46 ==  Cyc_Dict_Absent){ goto _LL49;} else{
goto _LL50;} _LL50: goto _LL51; _LL49: { int _temp52= 1; seen->structs=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)(
seen->structs,( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,
_temp52); if( d->fields !=  0){ struct Cyc_List_List* _temp53=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; for( 0; _temp53 !=  0;
_temp53=(( struct Cyc_List_List*) _check_null( _temp53))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen, _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp53))->hd)->type)){ _temp52= 0;}}} seen->structs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->structs,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp52); return _temp52;} _LL51:( void) _throw(
_temp46); _LL47:;}}} static int Cyc_Interface_check_public_uniondecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl* d){ struct
_handler_cons _temp54; _push_handler(& _temp54);{ int _temp56= 0; if( setjmp(
_temp54.handler)){ _temp56= 1;} if( ! _temp56){{ int _temp57=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp57;}; _pop_handler();} else{ void* _temp55=( void*) _exn_thrown;
void* _temp59= _temp55; _LL61: if( _temp59 ==  Cyc_Dict_Absent){ goto _LL62;}
else{ goto _LL63;} _LL63: goto _LL64; _LL62: { int _temp65= 1; seen->unions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->unions,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp65); if( d->fields !=  0){ struct Cyc_List_List*
_temp66=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( d->fields))->v;
for( 0; _temp66 !=  0; _temp66=(( struct Cyc_List_List*) _check_null( _temp66))->tl){
if( ! Cyc_Interface_check_public_type( ae, seen, _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,(
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp66))->hd)->type)){ _temp65= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp65);
return _temp65;} _LL64:( void) _throw( _temp59); _LL60:;}}} struct _tuple9{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple9* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl*
d){ struct _handler_cons _temp67; _push_handler(& _temp67);{ int _temp69= 0; if(
setjmp( _temp67.handler)){ _temp69= 1;} if( ! _temp69){{ int _temp70=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions,
d->name); _npop_handler( 0u); return _temp70;}; _pop_handler();} else{ void*
_temp68=( void*) _exn_thrown; void* _temp72= _temp68; _LL74: if( _temp72 ==  Cyc_Dict_Absent){
goto _LL75;} else{ goto _LL76;} _LL76: goto _LL77; _LL75: { int _temp78= 1; seen->tunions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->tunions, d->name, _temp78); if( d->fields !=  0){ struct
Cyc_List_List* _temp79=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; for( 0; _temp79 !=  0; _temp79=(( struct Cyc_List_List*)
_check_null( _temp79))->tl){ if( !(( int(*)( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0* name, void*(*
f)( struct _tuple9*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), d->name, Cyc_Interface_get_type1,((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp79))->hd)->typs)){
_temp78= 0;}}} seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name,
_temp78); return _temp78;} _LL77:( void) _throw( _temp72); _LL73:;}}} static int
Cyc_Interface_check_public_enumdecl( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Enumdecl* d){ return 1;} static int Cyc_Interface_check_public_typedefdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Typedefdecl*
d){ return Cyc_Interface_check_public_type( ae, seen,( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u), d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl*
d){ return Cyc_Interface_check_public_type( ae, seen, _tag_arr("variable",
sizeof( unsigned char), 9u), d->name,( void*) d->type);} static void* Cyc_Interface_get_type2(
struct _tuple1* x){ return(* x).f3;} static struct Cyc_List_List* Cyc_Interface_get_abs_ns(
void* ns){ void* _temp80= ns; struct Cyc_List_List* _temp86; _LL82: if((
unsigned int) _temp80 >  1u?*(( int*) _temp80) ==  Cyc_Absyn_Abs_n: 0){ _LL87:
_temp86=(( struct Cyc_Absyn_Abs_n_struct*) _temp80)->f1; goto _LL83;} else{ goto
_LL84;} _LL84: goto _LL85; _LL83: return _temp86; _LL85:( int) _throw(( void*)({
struct Cyc_Core_Invalid_argument_struct* _temp88=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp88[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp89; _temp89.tag= Cyc_Core_Invalid_argument;
_temp89.f1= _tag_arr("get_abs_ns", sizeof( unsigned char), 11u); _temp89;});
_temp88;})); _LL81:;} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void* t){ void* _temp90= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp114; void* _temp116; void* _temp118; struct Cyc_Core_Opt* _temp120; struct
Cyc_Core_Opt _temp122; void* _temp123; struct Cyc_Absyn_FnInfo _temp125; struct
Cyc_Absyn_VarargInfo* _temp127; struct Cyc_List_List* _temp129; void* _temp131;
struct Cyc_List_List* _temp133; struct Cyc_Absyn_Structdecl** _temp135; struct
Cyc_Absyn_Structdecl* _temp137; struct Cyc_List_List* _temp138; struct Cyc_Absyn_Uniondecl**
_temp140; struct Cyc_Absyn_Uniondecl* _temp142; struct Cyc_List_List* _temp143;
struct _tuple0* _temp145; struct Cyc_Absyn_TunionInfo _temp147; struct Cyc_List_List*
_temp149; void* _temp151; struct Cyc_Absyn_Tuniondecl* _temp153; struct Cyc_Absyn_TunionFieldInfo
_temp155; struct Cyc_List_List* _temp157; void* _temp159; struct Cyc_Absyn_Tunionfield*
_temp161; struct Cyc_Absyn_Tuniondecl* _temp163; _LL92: if(( unsigned int)
_temp90 >  4u?*(( int*) _temp90) ==  Cyc_Absyn_PointerType: 0){ _LL115: _temp114=((
struct Cyc_Absyn_PointerType_struct*) _temp90)->f1; _LL117: _temp116=( void*)
_temp114.elt_typ; goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int)
_temp90 >  4u?*(( int*) _temp90) ==  Cyc_Absyn_ArrayType: 0){ _LL119: _temp118=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp90)->f1; goto _LL95;} else{
goto _LL96;} _LL96: if(( unsigned int) _temp90 >  4u?*(( int*) _temp90) ==  Cyc_Absyn_TypedefType:
0){ _LL121: _temp120=(( struct Cyc_Absyn_TypedefType_struct*) _temp90)->f3; if(
_temp120 ==  0){ goto _LL98;} else{ _temp122=* _temp120; _LL124: _temp123=( void*)
_temp122.v; goto _LL97;}} else{ goto _LL98;} _LL98: if(( unsigned int) _temp90 > 
4u?*(( int*) _temp90) ==  Cyc_Absyn_FnType: 0){ _LL126: _temp125=(( struct Cyc_Absyn_FnType_struct*)
_temp90)->f1; _LL132: _temp131=( void*) _temp125.ret_typ; goto _LL130; _LL130:
_temp129= _temp125.args; goto _LL128; _LL128: _temp127= _temp125.cyc_varargs;
goto _LL99;} else{ goto _LL100;} _LL100: if(( unsigned int) _temp90 >  4u?*((
int*) _temp90) ==  Cyc_Absyn_TupleType: 0){ _LL134: _temp133=(( struct Cyc_Absyn_TupleType_struct*)
_temp90)->f1; goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int)
_temp90 >  4u?*(( int*) _temp90) ==  Cyc_Absyn_StructType: 0){ _LL139: _temp138=((
struct Cyc_Absyn_StructType_struct*) _temp90)->f2; goto _LL136; _LL136: _temp135=((
struct Cyc_Absyn_StructType_struct*) _temp90)->f3; if( _temp135 ==  0){ goto
_LL104;} else{ _temp137=* _temp135; goto _LL103;}} else{ goto _LL104;} _LL104:
if(( unsigned int) _temp90 >  4u?*(( int*) _temp90) ==  Cyc_Absyn_UnionType: 0){
_LL144: _temp143=(( struct Cyc_Absyn_UnionType_struct*) _temp90)->f2; goto
_LL141; _LL141: _temp140=(( struct Cyc_Absyn_UnionType_struct*) _temp90)->f3;
if( _temp140 ==  0){ goto _LL106;} else{ _temp142=* _temp140; goto _LL105;}}
else{ goto _LL106;} _LL106: if(( unsigned int) _temp90 >  4u?*(( int*) _temp90)
==  Cyc_Absyn_EnumType: 0){ _LL146: _temp145=(( struct Cyc_Absyn_EnumType_struct*)
_temp90)->f1; goto _LL107;} else{ goto _LL108;} _LL108: if(( unsigned int)
_temp90 >  4u?*(( int*) _temp90) ==  Cyc_Absyn_TunionType: 0){ _LL148: _temp147=((
struct Cyc_Absyn_TunionType_struct*) _temp90)->f1; _LL152: _temp151=( void*)
_temp147.tunion_info; if(*(( int*) _temp151) ==  Cyc_Absyn_KnownTunion){ _LL154:
_temp153=(( struct Cyc_Absyn_KnownTunion_struct*) _temp151)->f1; goto _LL150;}
else{ goto _LL110;} _LL150: _temp149= _temp147.targs; goto _LL109;} else{ goto
_LL110;} _LL110: if(( unsigned int) _temp90 >  4u?*(( int*) _temp90) ==  Cyc_Absyn_TunionFieldType:
0){ _LL156: _temp155=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp90)->f1;
_LL160: _temp159=( void*) _temp155.field_info; if(*(( int*) _temp159) ==  Cyc_Absyn_KnownTunionfield){
_LL164: _temp163=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp159)->f1;
goto _LL162; _LL162: _temp161=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp159)->f2; goto _LL158;} else{ goto _LL112;} _LL158: _temp157= _temp155.targs;
goto _LL111;} else{ goto _LL112;} _LL112: goto _LL113; _LL93: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp116); _LL95: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp118); _LL97: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp123); _LL99: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp129)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp131): 0; if( _temp127 !=  0){ void* _temp167; struct
Cyc_Absyn_VarargInfo _temp165=*(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp127)); _LL168: _temp167=( void*) _temp165.type; goto _LL166; _LL166: b= Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp167);} return b;} _LL101: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple9*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp133); _LL103: if(( void*)
_temp137->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,
_tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp137->name))->v); return 0;} return Cyc_Interface_check_public_type_list(
ae, seen, obj, name, Cyc_Core_identity, _temp138)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp137): 0; _LL105: if(( void*) _temp142->sc == ( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp142->name))->v);
return 0;} return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp143)? Cyc_Interface_check_public_uniondecl( ae, seen, _temp142): 0; _LL107: {
struct _tuple0 _temp171; struct _tagged_arr* _temp172; void* _temp174; struct
_tuple0* _temp169= _temp145; _temp171=* _temp169; _LL175: _temp174= _temp171.f1;
goto _LL173; _LL173: _temp172= _temp171.f2; goto _LL170; _LL170: { struct Cyc_List_List*
_temp176= Cyc_Interface_get_abs_ns( _temp174); struct Cyc_Absyn_Enumdecl* ed;{
struct _handler_cons _temp177; _push_handler(& _temp177);{ int _temp179= 0; if(
setjmp( _temp177.handler)){ _temp179= 1;} if( ! _temp179){{ struct Cyc_Tcenv_Genv*
_temp180=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp176); ed=*(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp180->enumdecls,
_temp172);}; _pop_handler();} else{ void* _temp178=( void*) _exn_thrown; void*
_temp182= _temp178; _LL184: if( _temp182 ==  Cyc_Dict_Absent){ goto _LL185;}
else{ goto _LL186;} _LL186: goto _LL187; _LL185:( int) _throw(( void*)({ struct
Cyc_Core_Invalid_argument_struct* _temp188=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp188[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp189; _temp189.tag= Cyc_Core_Invalid_argument;
_temp189.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp191;
_temp191.tag= Cyc_Std_String_pa; _temp191.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp145);{ void* _temp190[ 1u]={& _temp191}; Cyc_Std_aprintf( _tag_arr("check_public_type (can't find enum %s)",
sizeof( unsigned char), 39u), _tag_arr( _temp190, sizeof( void*), 1u));}});
_temp189;}); _temp188;})); _LL187:( void) _throw( _temp182); _LL183:;}}} if((
void*) ed->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj,
_temp145, _tag_arr("enum", sizeof( unsigned char), 5u), ed->name); return 0;}
return 1;}} _LL109: { struct _tuple0 _temp194; struct _tagged_arr* _temp195;
void* _temp197; struct _tuple0* _temp192= _temp153->name; _temp194=* _temp192;
_LL198: _temp197= _temp194.f1; goto _LL196; _LL196: _temp195= _temp194.f2; goto
_LL193; _LL193: { struct Cyc_List_List* _temp199= Cyc_Interface_get_abs_ns(
_temp197); struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons _temp200;
_push_handler(& _temp200);{ int _temp202= 0; if( setjmp( _temp200.handler)){
_temp202= 1;} if( ! _temp202){{ struct Cyc_Tcenv_Genv* _temp203=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( ae,
_temp199); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp203->tuniondecls, _temp195);};
_pop_handler();} else{ void* _temp201=( void*) _exn_thrown; void* _temp205=
_temp201; _LL207: if( _temp205 ==  Cyc_Dict_Absent){ goto _LL208;} else{ goto
_LL209;} _LL209: goto _LL210; _LL208:( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp211=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp211[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp212; _temp212.tag= Cyc_Core_Invalid_argument; _temp212.f1=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp214; _temp214.tag= Cyc_Std_String_pa;
_temp214.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp153->name);{
void* _temp213[ 1u]={& _temp214}; Cyc_Std_aprintf( _tag_arr("check_public_type (can't find [x]tunion %s)",
sizeof( unsigned char), 44u), _tag_arr( _temp213, sizeof( void*), 1u));}});
_temp212;}); _temp211;})); _LL210:( void) _throw( _temp205); _LL206:;}}} if((
void*) tud->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj,
name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name); return 0;}
return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp149)? Cyc_Interface_check_public_tuniondecl( ae, seen, tud): 0;}} _LL111: {
struct _tuple0 _temp217; struct _tagged_arr* _temp218; void* _temp220; struct
_tuple0* _temp215= _temp163->name; _temp217=* _temp215; _LL221: _temp220=
_temp217.f1; goto _LL219; _LL219: _temp218= _temp217.f2; goto _LL216; _LL216: {
struct Cyc_List_List* _temp222= Cyc_Interface_get_abs_ns( _temp220); struct Cyc_Absyn_Tuniondecl*
tud;{ struct _handler_cons _temp223; _push_handler(& _temp223);{ int _temp225= 0;
if( setjmp( _temp223.handler)){ _temp225= 1;} if( ! _temp225){{ struct Cyc_Tcenv_Genv*
_temp226=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp222); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp226->tuniondecls,
_temp218);}; _pop_handler();} else{ void* _temp224=( void*) _exn_thrown; void*
_temp228= _temp224; _LL230: if( _temp228 ==  Cyc_Dict_Absent){ goto _LL231;}
else{ goto _LL232;} _LL232: goto _LL233; _LL231:( int) _throw(( void*)({ struct
Cyc_Core_Invalid_argument_struct* _temp234=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp234[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp235; _temp235.tag= Cyc_Core_Invalid_argument;
_temp235.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp237;
_temp237.tag= Cyc_Std_String_pa; _temp237.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp163->name);{ void* _temp236[ 1u]={& _temp237}; Cyc_Std_aprintf( _tag_arr("check_public_type (can't find [x]tunion %s and search its fields)",
sizeof( unsigned char), 66u), _tag_arr( _temp236, sizeof( void*), 1u));}});
_temp235;}); _temp234;})); _LL233:( void) _throw( _temp228); _LL229:;}}} if( tud->fields
==  0){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp238=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp238[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp239; _temp239.tag= Cyc_Core_Invalid_argument; _temp239.f1=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp241; _temp241.tag= Cyc_Std_String_pa;
_temp241.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tud->name);{ void*
_temp240[ 1u]={& _temp241}; Cyc_Std_aprintf( _tag_arr("check_public_type ([x]tunion %s has no fields)",
sizeof( unsigned char), 47u), _tag_arr( _temp240, sizeof( void*), 1u));}});
_temp239;}); _temp238;}));}{ struct Cyc_Absyn_Tunionfield* tuf1= 0;{ struct Cyc_List_List*
_temp242=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp242 !=  0; _temp242=(( struct Cyc_List_List*) _check_null( _temp242))->tl){
if( Cyc_Std_strptrcmp((* _temp161->name).f2,(*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp242))->hd)->name).f2) ==  0){ tuf1=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp242))->hd); break;}}} if( tuf1 ==  0){( int) _throw(( void*)({
struct Cyc_Core_Invalid_argument_struct* _temp243=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp243[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp244; _temp244.tag= Cyc_Core_Invalid_argument;
_temp244.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp246;
_temp246.tag= Cyc_Std_String_pa; _temp246.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp161->name);{ void* _temp245[ 1u]={& _temp246}; Cyc_Std_aprintf( _tag_arr("check_public_type (can't find [x]tunionfield %s)",
sizeof( unsigned char), 49u), _tag_arr( _temp245, sizeof( void*), 1u));}});
_temp244;}); _temp243;}));}{ struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*)
_check_null( tuf1); if(( void*) tud->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tud->sc == ( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tuf->sc == ( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp248;
_temp248.tag= Cyc_Std_String_pa; _temp248.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp247[ 1u]={& _temp248}; Cyc_Std_aprintf( _tag_arr("field %s of",
sizeof( unsigned char), 12u), _tag_arr( _temp247, sizeof( void*), 1u));}}), tud->name);
return 0;} return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp157)? Cyc_Interface_check_public_tuniondecl( ae, seen, tud): 0;}}}} _LL113:
return 1; _LL91:;} struct _tuple10{ struct Cyc_Interface_Ienv* f1; struct Cyc_Interface_Ienv*
f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen* f5; struct Cyc_Interface_I*
f6; } ; static void Cyc_Interface_extract_structdecl( struct _tuple10* env,
struct _tagged_arr* x, struct Cyc_Absyn_Structdecl** dp){ struct _tuple10
_temp251; struct Cyc_Interface_Seen* _temp252; struct Cyc_Dict_Dict* _temp254;
int _temp256; struct Cyc_Interface_Ienv* _temp258; struct Cyc_Interface_Ienv*
_temp260; struct _tuple10* _temp249= env; _temp251=* _temp249; _LL261: _temp260=
_temp251.f1; goto _LL259; _LL259: _temp258= _temp251.f2; goto _LL257; _LL257:
_temp256= _temp251.f3; goto _LL255; _LL255: _temp254= _temp251.f4; goto _LL253;
_LL253: _temp252= _temp251.f5; goto _LL250; _LL250: { struct Cyc_Absyn_Structdecl*
_temp262=* dp; void* _temp263=( void*) _temp262->sc; _LL265: if( _temp263 == (
void*) Cyc_Absyn_Static){ goto _LL266;} else{ goto _LL267;} _LL267: if( _temp263
== ( void*) Cyc_Absyn_Abstract){ goto _LL268;} else{ goto _LL269;} _LL269: if(
_temp263 == ( void*) Cyc_Absyn_Public){ goto _LL270;} else{ goto _LL271;} _LL271:
if( _temp263 == ( void*) Cyc_Absyn_ExternC){ goto _LL272;} else{ goto _LL273;}
_LL273: if( _temp263 == ( void*) Cyc_Absyn_Extern){ goto _LL274;} else{ goto
_LL264;} _LL266: if( _temp256? _temp262->fields ==  0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("struct", sizeof(
unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp262->name))->v);} goto _LL264; _LL268: if( _temp262->fields ==  0){ if(
_temp256){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp262->name))->v);}} else{ _temp262=({ struct Cyc_Absyn_Structdecl*
_temp275=( struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp275[ 0]=* _temp262; _temp275;}); _temp262->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp254, _temp252, _temp262)){ _temp258->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp258->structdecls, x, _temp262);} goto _LL264;
_LL270: if( _temp262->fields ==  0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("struct", sizeof( unsigned char), 7u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp262->name))->v);
_temp262=({ struct Cyc_Absyn_Structdecl* _temp276=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp276[ 0]=* _temp262;
_temp276;});( void*)( _temp262->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_structdecl( _temp254, _temp252, _temp262)){ _temp258->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp258->structdecls, x,
_temp262);} goto _LL264; _LL272: goto _LL274; _LL274: if( Cyc_Interface_check_public_structdecl(
_temp254, _temp252, _temp262)){ _temp260->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp260->structdecls, x, _temp262);} goto _LL264;
_LL264:;}} static void Cyc_Interface_extract_uniondecl( struct _tuple10* env,
struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl** dp){ struct _tuple10
_temp279; struct Cyc_Interface_Seen* _temp280; struct Cyc_Dict_Dict* _temp282;
int _temp284; struct Cyc_Interface_Ienv* _temp286; struct Cyc_Interface_Ienv*
_temp288; struct _tuple10* _temp277= env; _temp279=* _temp277; _LL289: _temp288=
_temp279.f1; goto _LL287; _LL287: _temp286= _temp279.f2; goto _LL285; _LL285:
_temp284= _temp279.f3; goto _LL283; _LL283: _temp282= _temp279.f4; goto _LL281;
_LL281: _temp280= _temp279.f5; goto _LL278; _LL278: { struct Cyc_Absyn_Uniondecl*
_temp290=* dp; void* _temp291=( void*) _temp290->sc; _LL293: if( _temp291 == (
void*) Cyc_Absyn_Static){ goto _LL294;} else{ goto _LL295;} _LL295: if( _temp291
== ( void*) Cyc_Absyn_Abstract){ goto _LL296;} else{ goto _LL297;} _LL297: if(
_temp291 == ( void*) Cyc_Absyn_Public){ goto _LL298;} else{ goto _LL299;} _LL299:
if( _temp291 == ( void*) Cyc_Absyn_ExternC){ goto _LL300;} else{ goto _LL301;}
_LL301: if( _temp291 == ( void*) Cyc_Absyn_Extern){ goto _LL302;} else{ goto
_LL292;} _LL294: if( _temp284? _temp290->fields ==  0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp290->name))->v);} goto _LL292; _LL296: if( _temp290->fields ==  0){ if(
_temp284){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("union", sizeof( unsigned char), 6u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp290->name))->v);}} else{ _temp290=({ struct Cyc_Absyn_Uniondecl*
_temp303=( struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp303[ 0]=* _temp290; _temp303;}); _temp290->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp282, _temp280, _temp290)){ _temp286->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp286->uniondecls, x, _temp290);} goto _LL292;
_LL298: if( _temp290->fields ==  0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp290->name))->v);
_temp290=({ struct Cyc_Absyn_Uniondecl* _temp304=( struct Cyc_Absyn_Uniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp304[ 0]=* _temp290;
_temp304;});( void*)( _temp290->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_uniondecl( _temp282, _temp280, _temp290)){ _temp286->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp286->uniondecls, x,
_temp290);} goto _LL292; _LL300: goto _LL302; _LL302: if( Cyc_Interface_check_public_uniondecl(
_temp282, _temp280, _temp290)){ _temp288->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp288->uniondecls, x, _temp290);} goto _LL292;
_LL292:;}} static void Cyc_Interface_extract_enumdecl( struct _tuple10* env,
struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl** dp){ struct _tuple10 _temp307;
struct Cyc_Interface_Seen* _temp308; struct Cyc_Dict_Dict* _temp310; int
_temp312; struct Cyc_Interface_Ienv* _temp314; struct Cyc_Interface_Ienv*
_temp316; struct _tuple10* _temp305= env; _temp307=* _temp305; _LL317: _temp316=
_temp307.f1; goto _LL315; _LL315: _temp314= _temp307.f2; goto _LL313; _LL313:
_temp312= _temp307.f3; goto _LL311; _LL311: _temp310= _temp307.f4; goto _LL309;
_LL309: _temp308= _temp307.f5; goto _LL306; _LL306: { struct Cyc_Absyn_Enumdecl*
_temp318=* dp; void* _temp319=( void*) _temp318->sc; _LL321: if( _temp319 == (
void*) Cyc_Absyn_Static){ goto _LL322;} else{ goto _LL323;} _LL323: if( _temp319
== ( void*) Cyc_Absyn_Abstract){ goto _LL324;} else{ goto _LL325;} _LL325: if(
_temp319 == ( void*) Cyc_Absyn_Public){ goto _LL326;} else{ goto _LL327;} _LL327:
if( _temp319 == ( void*) Cyc_Absyn_ExternC){ goto _LL328;} else{ goto _LL329;}
_LL329: if( _temp319 == ( void*) Cyc_Absyn_Extern){ goto _LL330;} else{ goto
_LL320;} _LL322: if( _temp312? _temp318->fields ==  0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("enum", sizeof(
unsigned char), 5u), _temp318->name);} goto _LL320; _LL324: if( _temp318->fields
==  0){ if( _temp312){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof(
unsigned char), 9u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp318->name);}}
else{ _temp318=({ struct Cyc_Absyn_Enumdecl* _temp331=( struct Cyc_Absyn_Enumdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp331[ 0]=* _temp318;
_temp331;}); _temp318->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp310, _temp308, _temp318)){ _temp314->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp314->enumdecls, x, _temp318);} goto _LL320; _LL326:
if( _temp318->fields ==  0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("enum", sizeof( unsigned char), 5u),
_temp318->name); _temp318=({ struct Cyc_Absyn_Enumdecl* _temp332=( struct Cyc_Absyn_Enumdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp332[ 0]=* _temp318;
_temp332;});( void*)( _temp318->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_enumdecl( _temp310, _temp308, _temp318)){ _temp314->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp314->enumdecls, x,
_temp318);} goto _LL320; _LL328: goto _LL330; _LL330: if( Cyc_Interface_check_public_enumdecl(
_temp310, _temp308, _temp318)){ _temp316->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp316->enumdecls, x, _temp318);} goto _LL320; _LL320:;}}
static void Cyc_Interface_extract_xtunionfielddecl( struct Cyc_Interface_I* i,
struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield* f){ struct Cyc_List_List*
_temp333= 0;{ void* _temp334=(* f->name).f1; struct Cyc_List_List* _temp342;
struct Cyc_List_List* _temp344; _LL336: if(( unsigned int) _temp334 >  1u?*((
int*) _temp334) ==  Cyc_Absyn_Rel_n: 0){ _LL343: _temp342=(( struct Cyc_Absyn_Rel_n_struct*)
_temp334)->f1; goto _LL337;} else{ goto _LL338;} _LL338: if(( unsigned int)
_temp334 >  1u?*(( int*) _temp334) ==  Cyc_Absyn_Abs_n: 0){ _LL345: _temp344=((
struct Cyc_Absyn_Abs_n_struct*) _temp334)->f1; goto _LL339;} else{ goto _LL340;}
_LL340: goto _LL341; _LL337: _temp344= _temp342; goto _LL339; _LL339: _temp333=
_temp344; goto _LL335; _LL341: goto _LL335; _LL335:;}{ struct Cyc_Dict_Dict**
dict;{ void* _temp346=( void*) f->sc; _LL348: if( _temp346 == ( void*) Cyc_Absyn_Static){
goto _LL349;} else{ goto _LL350;} _LL350: if( _temp346 == ( void*) Cyc_Absyn_Extern){
goto _LL351;} else{ goto _LL352;} _LL352: if( _temp346 == ( void*) Cyc_Absyn_Public){
goto _LL353;} else{ goto _LL354;} _LL354: goto _LL355; _LL349: return; _LL351:
dict=& i->imports; goto _LL347; _LL353: dict=& i->exports; goto _LL347; _LL355:(
int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp356=(
struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp356[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp357; _temp357.tag=
Cyc_Core_Invalid_argument; _temp357.f1= _tag_arr("add_xtunionfielddecl", sizeof(
unsigned char), 21u); _temp357;}); _temp356;})); _LL347:;}{ struct Cyc_Interface_Ienv*
env;{ struct _handler_cons _temp358; _push_handler(& _temp358);{ int _temp360= 0;
if( setjmp( _temp358.handler)){ _temp360= 1;} if( ! _temp360){ env=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict,
_temp333);; _pop_handler();} else{ void* _temp359=( void*) _exn_thrown; void*
_temp362= _temp359; _LL364: if( _temp362 ==  Cyc_Dict_Absent){ goto _LL365;}
else{ goto _LL366;} _LL366: goto _LL367; _LL365: env= Cyc_Interface_new_ienv();*
dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* dict, _temp333, env);
goto _LL363; _LL367:( void) _throw( _temp362); _LL363:;}}} env->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( env->xtunionfielddecls,(*
f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl* _temp368=( struct Cyc_Tcdecl_Xtunionfielddecl*)
_cycalloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp368->base= d;
_temp368->field= f; _temp368;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple10* env, struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple10 _temp371; struct Cyc_Interface_I* _temp372; struct Cyc_Interface_Seen*
_temp374; struct Cyc_Dict_Dict* _temp376; int _temp378; struct Cyc_Interface_Ienv*
_temp380; struct Cyc_Interface_Ienv* _temp382; struct _tuple10* _temp369= env;
_temp371=* _temp369; _LL383: _temp382= _temp371.f1; goto _LL381; _LL381:
_temp380= _temp371.f2; goto _LL379; _LL379: _temp378= _temp371.f3; goto _LL377;
_LL377: _temp376= _temp371.f4; goto _LL375; _LL375: _temp374= _temp371.f5; goto
_LL373; _LL373: _temp372= _temp371.f6; goto _LL370; _LL370: { struct Cyc_Absyn_Tuniondecl*
_temp384=* dp; void* _temp385=( void*) _temp384->sc; _LL387: if( _temp385 == (
void*) Cyc_Absyn_Static){ goto _LL388;} else{ goto _LL389;} _LL389: if( _temp385
== ( void*) Cyc_Absyn_Abstract){ goto _LL390;} else{ goto _LL391;} _LL391: if(
_temp385 == ( void*) Cyc_Absyn_Public){ goto _LL392;} else{ goto _LL393;} _LL393:
if( _temp385 == ( void*) Cyc_Absyn_ExternC){ goto _LL394;} else{ goto _LL395;}
_LL395: if( _temp385 == ( void*) Cyc_Absyn_Extern){ goto _LL396;} else{ goto
_LL386;} _LL388: if(( ! _temp384->is_xtunion? _temp378: 0)? _temp384->fields == 
0: 0){ Cyc_Interface_fields_err( _tag_arr("static", sizeof( unsigned char), 7u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp384->name);} goto _LL386;
_LL390: if( _temp384->fields ==  0){ if( ! _temp384->is_xtunion? _temp378: 0){
Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char), 9u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp384->name);}} else{
_temp384=({ struct Cyc_Absyn_Tuniondecl* _temp397=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp397[ 0]=* _temp384;
_temp397;}); _temp384->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp376, _temp374, _temp384)){ _temp380->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp380->tuniondecls, x, _temp384);} goto _LL386;
_LL392: _temp384=({ struct Cyc_Absyn_Tuniondecl* _temp398=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp398[ 0]=* _temp384;
_temp398;}); if( ! _temp384->is_xtunion? _temp384->fields ==  0: 0){ Cyc_Interface_fields_err(
_tag_arr("public", sizeof( unsigned char), 7u), _tag_arr("tunion", sizeof(
unsigned char), 7u), _temp384->name);( void*)( _temp384->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl( _temp376,
_temp374, _temp384)){ if( _temp384->is_xtunion? _temp384->fields !=  0: 0){
struct Cyc_List_List* _temp399=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp384->fields))->v; _temp384->fields= 0;{ struct Cyc_List_List*
_temp400= _temp399; for( 0; _temp400 !=  0; _temp400=(( struct Cyc_List_List*)
_check_null( _temp400))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp372,
_temp384,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp400))->hd);}}} _temp380->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp380->tuniondecls, x, _temp384);} goto _LL386; _LL394:(
int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp401=(
struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp401[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp402; _temp402.tag=
Cyc_Core_Invalid_argument; _temp402.f1= _tag_arr("extract_tuniondecl", sizeof(
unsigned char), 19u); _temp402;}); _temp401;})); _LL396: if( Cyc_Interface_check_public_tuniondecl(
_temp376, _temp374, _temp384)){ if( _temp384->is_xtunion? _temp384->fields !=  0:
0){ _temp384=({ struct Cyc_Absyn_Tuniondecl* _temp403=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp403[ 0]=* _temp384;
_temp403;});{ struct Cyc_List_List* _temp404=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp384->fields))->v; _temp384->fields= 0;{ struct
Cyc_List_List* _temp405= _temp404; for( 0; _temp405 !=  0; _temp405=(( struct
Cyc_List_List*) _check_null( _temp405))->tl){ Cyc_Interface_extract_xtunionfielddecl(
_temp372, _temp384,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp405))->hd);}}}} _temp382->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp382->tuniondecls, x, _temp384);} goto _LL386;
_LL386:;}} static void Cyc_Interface_extract_typedef( struct _tuple10* env,
struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d){ struct _tuple10
_temp408; struct Cyc_Interface_Seen* _temp409; struct Cyc_Dict_Dict* _temp411;
struct Cyc_Interface_Ienv* _temp413; struct _tuple10* _temp406= env; _temp408=*
_temp406; _LL414: _temp413= _temp408.f2; goto _LL412; _LL412: _temp411= _temp408.f4;
goto _LL410; _LL410: _temp409= _temp408.f5; goto _LL407; _LL407: if( Cyc_Interface_check_public_typedefdecl(
_temp411, _temp409, d)){ _temp413->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp413->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple10* env, struct _tagged_arr* x, struct _tuple6* v){ struct _tuple10
_temp417; struct Cyc_Interface_Seen* _temp418; struct Cyc_Dict_Dict* _temp420;
int _temp422; struct Cyc_Interface_Ienv* _temp424; struct Cyc_Interface_Ienv*
_temp426; struct _tuple10* _temp415= env; _temp417=* _temp415; _LL427: _temp426=
_temp417.f1; goto _LL425; _LL425: _temp424= _temp417.f2; goto _LL423; _LL423:
_temp422= _temp417.f3; goto _LL421; _LL421: _temp420= _temp417.f4; goto _LL419;
_LL419: _temp418= _temp417.f5; goto _LL416; _LL416: { void* _temp428=(* v).f1;
void* _temp429= _temp428; void* _temp435; _LL431: if(*(( int*) _temp429) ==  Cyc_Tcenv_VarRes){
_LL436: _temp435=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp429)->f1; goto
_LL432;} else{ goto _LL433;} _LL433: goto _LL434; _LL432:{ void* _temp437=
_temp435; struct Cyc_Absyn_Fndecl* _temp445; struct Cyc_Absyn_Vardecl* _temp447;
_LL439: if(( unsigned int) _temp437 >  1u?*(( int*) _temp437) ==  Cyc_Absyn_Funname_b:
0){ _LL446: _temp445=(( struct Cyc_Absyn_Funname_b_struct*) _temp437)->f1; goto
_LL440;} else{ goto _LL441;} _LL441: if(( unsigned int) _temp437 >  1u?*(( int*)
_temp437) ==  Cyc_Absyn_Global_b: 0){ _LL448: _temp447=(( struct Cyc_Absyn_Global_b_struct*)
_temp437)->f1; goto _LL442;} else{ goto _LL443;} _LL443: goto _LL444; _LL440: {
struct Cyc_Absyn_Vardecl* _temp449=({ struct Cyc_Absyn_Vardecl* _temp450=(
struct Cyc_Absyn_Vardecl*) _cycalloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp450->sc=( void*)(( void*) _temp445->sc); _temp450->name= _temp445->name;
_temp450->tq=({ struct Cyc_Absyn_Tqual _temp451; _temp451.q_const= 0; _temp451.q_volatile=
0; _temp451.q_restrict= 0; _temp451;}); _temp450->type=( void*)(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp445->cached_typ))->v); _temp450->initializer=
0; _temp450->rgn= 0; _temp450->attributes= 0; _temp450;}); _temp422= 0; _temp447=
_temp449; goto _LL442;} _LL442: if( _temp447->initializer !=  0){ _temp447=({
struct Cyc_Absyn_Vardecl* _temp452=( struct Cyc_Absyn_Vardecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp452[ 0]=* _temp447; _temp452;});
_temp447->initializer= 0;}{ void* _temp453=( void*) _temp447->sc; _LL455: if(
_temp453 == ( void*) Cyc_Absyn_Static){ goto _LL456;} else{ goto _LL457;} _LL457:
if( _temp453 == ( void*) Cyc_Absyn_Abstract){ goto _LL458;} else{ goto _LL459;}
_LL459: if( _temp453 == ( void*) Cyc_Absyn_Public){ goto _LL460;} else{ goto
_LL461;} _LL461: if( _temp453 == ( void*) Cyc_Absyn_ExternC){ goto _LL462;}
else{ goto _LL463;} _LL463: if( _temp453 == ( void*) Cyc_Absyn_Extern){ goto
_LL464;} else{ goto _LL454;} _LL456: if( _temp422? Cyc_Tcutil_is_function_type((
void*) _temp447->type): 0){ Cyc_Interface_body_err( _tag_arr("static", sizeof(
unsigned char), 7u), _temp447->name);} goto _LL454; _LL458:( int) _throw(( void*)({
struct Cyc_Core_Invalid_argument_struct* _temp465=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp465[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp466; _temp466.tag= Cyc_Core_Invalid_argument;
_temp466.f1= _tag_arr("extract_ordinarie", sizeof( unsigned char), 18u);
_temp466;}); _temp465;})); _LL460: if( _temp422? Cyc_Tcutil_is_function_type((
void*) _temp447->type): 0){ Cyc_Interface_body_err( _tag_arr("public", sizeof(
unsigned char), 7u), _temp447->name);} if( Cyc_Interface_check_public_vardecl(
_temp420, _temp418, _temp447)){ _temp424->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data))
Cyc_Dict_insert)( _temp424->vardecls, x, _temp447);} goto _LL454; _LL462: goto
_LL464; _LL464: if( Cyc_Interface_check_public_vardecl( _temp420, _temp418,
_temp447)){ _temp426->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp426->vardecls, x, _temp447);} goto _LL454; _LL454:;} goto _LL438; _LL444:
goto _LL438; _LL438:;} goto _LL430; _LL434: goto _LL430; _LL430:;}} struct
_tuple11{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple11*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple11
_temp469; struct Cyc_Interface_Seen* _temp470; struct Cyc_Dict_Dict* _temp472;
int _temp474; struct Cyc_Interface_I* _temp476; struct _tuple11* _temp467= env_f;
_temp469=* _temp467; _LL477: _temp476= _temp469.f1; goto _LL475; _LL475:
_temp474= _temp469.f2; goto _LL473; _LL473: _temp472= _temp469.f3; goto _LL471;
_LL471: _temp470= _temp469.f4; goto _LL468; _LL468: { struct _tuple10 _temp478=({
struct _tuple10 _temp479; _temp479.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp476->imports,
ns); _temp479.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp476->exports, ns); _temp479.f3=
_temp474; _temp479.f4= _temp472; _temp479.f5= _temp470; _temp479.f6= _temp476;
_temp479;});(( void(*)( void(* f)( struct _tuple10*, struct _tagged_arr*, struct
Cyc_Absyn_Structdecl**), struct _tuple10* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_extract_structdecl,& _temp478, ge->structdecls);(( void(*)( void(*
f)( struct _tuple10*, struct _tagged_arr*, struct Cyc_Absyn_Uniondecl**), struct
_tuple10* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,&
_temp478, ge->uniondecls);(( void(*)( void(* f)( struct _tuple10*, struct
_tagged_arr*, struct Cyc_Absyn_Tuniondecl**), struct _tuple10* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,& _temp478, ge->tuniondecls);((
void(*)( void(* f)( struct _tuple10*, struct _tagged_arr*, struct Cyc_Absyn_Enumdecl**),
struct _tuple10* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,&
_temp478, ge->enumdecls);(( void(*)( void(* f)( struct _tuple10*, struct
_tagged_arr*, struct Cyc_Absyn_Typedefdecl*), struct _tuple10* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,& _temp478, ge->typedefs);((
void(*)( void(* f)( struct _tuple10*, struct _tagged_arr*, struct _tuple6*),
struct _tuple10* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,&
_temp478, ge->ordinaries);}} static struct Cyc_Interface_I* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_complete_defs){ struct _tuple11 _temp480=({
struct _tuple11 _temp481; _temp481.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict*
skel)) Cyc_Interface_skel2i)( ae); _temp481.f2= check_complete_defs; _temp481.f3=
ae; _temp481.f4= Cyc_Interface_new_seen(); _temp481;});(( void(*)( void(* f)(
struct _tuple11*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*), struct
_tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp480, ae); return _temp480.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_arr* msg1, struct
_tagged_arr msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple12{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_arr*); struct
_tagged_arr f4; struct _tagged_arr* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple12* env, struct _tagged_arr* x, void* y1){ struct _tuple12 _temp484;
struct _tagged_arr* _temp485; struct _tagged_arr _temp487; int(* _temp489)( void*,
void*, struct _tagged_arr*); struct Cyc_Dict_Dict* _temp491; int _temp493; int*
_temp495; struct _tuple12* _temp482= env; _temp484=* _temp482; _LL494: _temp493=
_temp484.f1; _temp495=( int*)&(* _temp482).f1; goto _LL492; _LL492: _temp491=
_temp484.f2; goto _LL490; _LL490: _temp489= _temp484.f3; goto _LL488; _LL488:
_temp487= _temp484.f4; goto _LL486; _LL486: _temp485= _temp484.f5; goto _LL483;
_LL483: { struct _handler_cons _temp496; _push_handler(& _temp496);{ int
_temp498= 0; if( setjmp( _temp496.handler)){ _temp498= 1;} if( ! _temp498){{
void* _temp499=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_lookup)( _temp491, x); if( ! _temp489( y1, _temp499, _temp485)){*
_temp495= 0;}}; _pop_handler();} else{ void* _temp497=( void*) _exn_thrown; void*
_temp501= _temp497; _LL503: if( _temp501 ==  Cyc_Dict_Absent){ goto _LL504;}
else{ goto _LL505;} _LL505: goto _LL506; _LL504: Cyc_Interface_check_err(
_temp485,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp509;
_temp509.tag= Cyc_Std_String_pa; _temp509.f1=( struct _tagged_arr)* x;{ struct
Cyc_Std_String_pa_struct _temp508; _temp508.tag= Cyc_Std_String_pa; _temp508.f1=(
struct _tagged_arr) _temp487;{ void* _temp507[ 2u]={& _temp508,& _temp509}; Cyc_Std_aprintf(
_tag_arr("%s %s is missing", sizeof( unsigned char), 17u), _tag_arr( _temp507,
sizeof( void*), 2u));}}}));* _temp495= 0; goto _LL502; _LL506:( void) _throw(
_temp501); _LL502:;}}}} static int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( void*, void*, struct
_tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg){ struct _tuple12
_temp510=({ struct _tuple12 _temp511; _temp511.f1= 1; _temp511.f2= dic2;
_temp511.f3= incl_f; _temp511.f4= t; _temp511.f5= msg; _temp511;});(( void(*)(
void(* f)( struct _tuple12*, struct _tagged_arr*, void*), struct _tuple12* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_dict_f,& _temp510,
dic1); return _temp510.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Structdecl*
_temp512= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); if( _temp512 ==  0){
return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 !=  _temp512){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp514; _temp514.tag=
Cyc_Std_String_pa; _temp514.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v);{ void*
_temp513[ 1u]={& _temp514}; Cyc_Std_aprintf( _tag_arr("declaration of struct %s discloses too much information",
sizeof( unsigned char), 56u), _tag_arr( _temp513, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl*
d0, struct Cyc_Absyn_Uniondecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Uniondecl*
_temp515= Cyc_Tcdecl_merge_uniondecl( d0, d1, 0, msg); if( _temp515 ==  0){
return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 !=  _temp515){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp517; _temp517.tag=
Cyc_Std_String_pa; _temp517.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v);{ void*
_temp516[ 1u]={& _temp517}; Cyc_Std_aprintf( _tag_arr("declaration of union %s discloses too much information",
sizeof( unsigned char), 55u), _tag_arr( _temp516, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl*
d0, struct Cyc_Absyn_Tuniondecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Tuniondecl*
_temp518= Cyc_Tcdecl_merge_tuniondecl( d0, d1, 0, msg); if( _temp518 ==  0){
return 0;} if(( struct Cyc_Absyn_Tuniondecl*) d0 !=  _temp518){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp520; _temp520.tag=
Cyc_Std_String_pa; _temp520.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( d1->name);{
void* _temp519[ 1u]={& _temp520}; Cyc_Std_aprintf( _tag_arr("declaration of tunion %s discloses too much information",
sizeof( unsigned char), 56u), _tag_arr( _temp519, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl*
d0, struct Cyc_Absyn_Enumdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Enumdecl*
_temp521= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); if( _temp521 ==  0){
return 0;} if(( struct Cyc_Absyn_Enumdecl*) d0 !=  _temp521){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp523; _temp523.tag=
Cyc_Std_String_pa; _temp523.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( d1->name);{
void* _temp522[ 1u]={& _temp523}; Cyc_Std_aprintf( _tag_arr("declaration of enum %s discloses too much information",
sizeof( unsigned char), 54u), _tag_arr( _temp522, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl*
d0, struct Cyc_Absyn_Vardecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Vardecl*
_temp524= Cyc_Tcdecl_merge_vardecl( d0, d1, 0, msg); if( _temp524 ==  0){ return
0;} if(( struct Cyc_Absyn_Vardecl*) d0 !=  _temp524){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp526; _temp526.tag=
Cyc_Std_String_pa; _temp526.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( d1->name);{
void* _temp525[ 1u]={& _temp526}; Cyc_Std_aprintf( _tag_arr("declaration of variable %s discloses too much information",
sizeof( unsigned char), 58u), _tag_arr( _temp525, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_typedefdecl( struct Cyc_Absyn_Typedefdecl*
d0, struct Cyc_Absyn_Typedefdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Typedefdecl*
_temp527= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); if( _temp527 ==  0){
return 0;} if(( struct Cyc_Absyn_Typedefdecl*) d0 !=  _temp527){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp529; _temp529.tag=
Cyc_Std_String_pa; _temp529.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( d1->name);{
void* _temp528[ 1u]={& _temp529}; Cyc_Std_aprintf( _tag_arr("declaration of typedef %s discloses too much information",
sizeof( unsigned char), 57u), _tag_arr( _temp528, sizeof( void*), 1u));}}));
return 0;} return 1;} static int Cyc_Interface_incl_xtunionfielddecl( struct Cyc_Tcdecl_Xtunionfielddecl*
d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp530= Cyc_Tcdecl_merge_xtunionfielddecl( d0, d1, 0, msg); if( _temp530 ==  0){
return 0;} if(( struct Cyc_Tcdecl_Xtunionfielddecl*) d0 !=  _temp530){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp532; _temp532.tag=
Cyc_Std_String_pa; _temp532.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
d1->field)->name);{ void* _temp531[ 1u]={& _temp532}; Cyc_Std_aprintf( _tag_arr("declaration of xtunionfield %s discloses too much information",
sizeof( unsigned char), 62u), _tag_arr( _temp531, sizeof( void*), 1u));}}));
return 0;} return 1;} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, void**(* merge_f)( void**, void**, struct Cyc_Position_Segment*, struct
_tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg); static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_arr* msg){ int _temp533=(( int(*)( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->structdecls, ie2->structdecls, Cyc_Interface_incl_structdecl, _tag_arr("struct",
sizeof( unsigned char), 7u), msg); int _temp534=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl, _tag_arr("union", sizeof( unsigned char), 6u), msg);
int _temp535=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->tuniondecls, ie2->tuniondecls, Cyc_Interface_incl_tuniondecl, _tag_arr("tunion",
sizeof( unsigned char), 7u), msg); int _temp536=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); int _temp537=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg) !=  0; int _temp538=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); int _temp539=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
Cyc_Interface_incl_xtunionfielddecl, _tag_arr("xtunionfield", sizeof(
unsigned char), 13u), msg); return((((( _temp533? _temp534: 0)? _temp535: 0)?
_temp536: 0)? _temp537: 0)? _temp538: 0)? _temp539: 0;} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; struct _tagged_arr* f3; } ; static void Cyc_Interface_incl_ns_f(
struct _tuple13* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){
struct _tuple13 _temp542; struct _tagged_arr* _temp543; struct Cyc_Dict_Dict*
_temp545; int _temp547; int* _temp549; struct _tuple13* _temp540= env; _temp542=*
_temp540; _LL548: _temp547= _temp542.f1; _temp549=( int*)&(* _temp540).f1; goto
_LL546; _LL546: _temp545= _temp542.f2; goto _LL544; _LL544: _temp543= _temp542.f3;
goto _LL541; _LL541: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons
_temp550; _push_handler(& _temp550);{ int _temp552= 0; if( setjmp( _temp550.handler)){
_temp552= 1;} if( ! _temp552){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp545, ns);; _pop_handler();}
else{ void* _temp551=( void*) _exn_thrown; void* _temp554= _temp551; _LL556: if(
_temp554 ==  Cyc_Dict_Absent){ goto _LL557;} else{ goto _LL558;} _LL558: goto
_LL559; _LL557: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL555; _LL559:( void)
_throw( _temp554); _LL555:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp543)){*
_temp549= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple13 _temp560=({
struct _tuple13 _temp561; _temp561.f1= 1; _temp561.f2= dic2; _temp561.f3= msg;
_temp561;});(( void(*)( void(* f)( struct _tuple13*, struct Cyc_List_List*,
struct Cyc_Interface_Ienv*), struct _tuple13* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_incl_ns_f,& _temp560, dic1); return _temp560.f1;} int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp562; _push_handler(& _temp562);{ int _temp564= 0; if(
setjmp( _temp562.handler)){ _temp564= 1;} if( ! _temp564){{ int _temp565= 1;
struct _tagged_arr* msg= 0; if( info !=  0){ msg=({ struct _tagged_arr* _temp566=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp566[ 0]=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp569; _temp569.tag=
Cyc_Std_String_pa; _temp569.f1=( struct _tagged_arr)(*(( struct _tuple3*)
_check_null( info))).f2;{ struct Cyc_Std_String_pa_struct _temp568; _temp568.tag=
Cyc_Std_String_pa; _temp568.f1=( struct _tagged_arr)(*(( struct _tuple3*)
_check_null( info))).f1;{ void* _temp567[ 2u]={& _temp568,& _temp569}; Cyc_Std_aprintf(
_tag_arr("checking inclusion of %s exports into %s exports,", sizeof(
unsigned char), 50u), _tag_arr( _temp567, sizeof( void*), 2u));}}}); _temp566;});}
if( ! Cyc_Interface_incl_ns( i1->exports, i2->exports, msg)){ _temp565= 0;} if(
info !=  0){ msg=({ struct _tagged_arr* _temp570=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp570[ 0]=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp573; _temp573.tag= Cyc_Std_String_pa;
_temp573.f1=( struct _tagged_arr)(*(( struct _tuple3*) _check_null( info))).f1;{
struct Cyc_Std_String_pa_struct _temp572; _temp572.tag= Cyc_Std_String_pa;
_temp572.f1=( struct _tagged_arr)(*(( struct _tuple3*) _check_null( info))).f2;{
void* _temp571[ 2u]={& _temp572,& _temp573}; Cyc_Std_aprintf( _tag_arr("checking inclusion of %s imports into %s imports,",
sizeof( unsigned char), 50u), _tag_arr( _temp571, sizeof( void*), 2u));}}});
_temp570;});} if( ! Cyc_Interface_incl_ns( i2->imports, i1->imports, msg)){
_temp565= 0;}{ int _temp574= _temp565; _npop_handler( 0u); return _temp574;}};
_pop_handler();} else{ void* _temp563=( void*) _exn_thrown; void* _temp576=
_temp563; _LL578: if( _temp576 ==  Cyc_Tcdecl_Incompatible){ goto _LL579;} else{
goto _LL580;} _LL580: goto _LL581; _LL579: return 0; _LL581:( void) _throw(
_temp576); _LL577:;}}} struct _tuple14{ int f1; struct Cyc_Dict_Dict* f2; struct
Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**, void**,
struct Cyc_Position_Segment*, struct _tagged_arr*); struct _tagged_arr f6;
struct _tagged_arr* f7; } ; void Cyc_Interface_compat_merge_dict_f( struct
_tuple14* env, struct _tagged_arr* x, void** y2){ struct _tuple14 _temp584;
struct _tagged_arr* _temp585; struct _tagged_arr _temp587; void**(* _temp589)(
void**, void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct Cyc_Dict_Dict*
_temp591; struct Cyc_Dict_Dict* _temp593; struct Cyc_Dict_Dict* _temp595; struct
Cyc_Dict_Dict** _temp597; int _temp598; int* _temp600; struct _tuple14* _temp582=
env; _temp584=* _temp582; _LL599: _temp598= _temp584.f1; _temp600=( int*)&(*
_temp582).f1; goto _LL596; _LL596: _temp595= _temp584.f2; _temp597=( struct Cyc_Dict_Dict**)&(*
_temp582).f2; goto _LL594; _LL594: _temp593= _temp584.f3; goto _LL592; _LL592:
_temp591= _temp584.f4; goto _LL590; _LL590: _temp589= _temp584.f5; goto _LL588;
_LL588: _temp587= _temp584.f6; goto _LL586; _LL586: _temp585= _temp584.f7; goto
_LL583; _LL583: { void** y;{ struct _handler_cons _temp601; _push_handler(&
_temp601);{ int _temp603= 0; if( setjmp( _temp601.handler)){ _temp603= 1;} if( !
_temp603){{ void** _temp604=(( void**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp593, x); void** _temp605= _temp589(
_temp604, y2, 0, _temp585); if( !(( unsigned int) _temp605)){* _temp600= 0;
_npop_handler( 0u); return;} y=( void**) _check_null( _temp605);}; _pop_handler();}
else{ void* _temp602=( void*) _exn_thrown; void* _temp607= _temp602; _LL609: if(
_temp607 ==  Cyc_Dict_Absent){ goto _LL610;} else{ goto _LL611;} _LL611: goto
_LL612; _LL610: y= y2; goto _LL608; _LL612:( void) _throw( _temp607); _LL608:;}}}{
struct _handler_cons _temp613; _push_handler(& _temp613);{ int _temp615= 0; if(
setjmp( _temp613.handler)){ _temp615= 1;} if( ! _temp615){{ void** _temp616=((
void**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp591, x); void** _temp617= _temp589( _temp616, y, 0, _temp585); if( _temp617
!= ( void**) _temp616){ if(( unsigned int) _temp617){ Cyc_Interface_check_err(
_temp585,( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp620;
_temp620.tag= Cyc_Std_String_pa; _temp620.f1=( struct _tagged_arr)* x;{ struct
Cyc_Std_String_pa_struct _temp619; _temp619.tag= Cyc_Std_String_pa; _temp619.f1=(
struct _tagged_arr) _temp587;{ void* _temp618[ 2u]={& _temp619,& _temp620}; Cyc_Std_aprintf(
_tag_arr("abstract %s %s is being imported as non-abstract", sizeof(
unsigned char), 49u), _tag_arr( _temp618, sizeof( void*), 2u));}}}));}* _temp600=
0;}}; _pop_handler();} else{ void* _temp614=( void*) _exn_thrown; void* _temp622=
_temp614; _LL624: if( _temp622 ==  Cyc_Dict_Absent){ goto _LL625;} else{ goto
_LL626;} _LL626: goto _LL627; _LL625: if(* _temp600){* _temp597=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void** data)) Cyc_Dict_insert)(*
_temp597, x, y);} goto _LL623; _LL627:( void) _throw( _temp622); _LL623:;}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg){ struct _tuple14 _temp628=({ struct _tuple14 _temp630;
_temp630.f1= 1; _temp630.f2= dic1; _temp630.f3= dic1; _temp630.f4= excl;
_temp630.f5= merge_f; _temp630.f6= t; _temp630.f7= msg; _temp630;});(( void(*)(
void(* f)( struct _tuple14*, struct _tagged_arr*, void**), struct _tuple14* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_dict_f,&
_temp628, dic2); if( _temp628.f1){ return({ struct Cyc_Core_Opt* _temp629=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp629->v=(
void*) _temp628.f2; _temp629;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_arr* msg){ struct Cyc_Core_Opt*
_temp631=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp632=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl,
_tag_arr("union", sizeof( unsigned char), 6u), msg); struct Cyc_Core_Opt*
_temp633=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)(
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
_tag_arr("tunion", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp634=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct
Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp635=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef", sizeof( unsigned char), 8u), msg); struct Cyc_Core_Opt*
_temp636=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt*
_temp637=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl, _tag_arr("xtunionfield",
sizeof( unsigned char), 13u), msg); if(((((( !(( unsigned int) _temp631)? 1: !((
unsigned int) _temp632))? 1: !(( unsigned int) _temp633))? 1: !(( unsigned int)
_temp634))? 1: !(( unsigned int) _temp635))? 1: !(( unsigned int) _temp636))? 1:
!(( unsigned int) _temp637)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp638=( struct Cyc_Interface_Ienv*) _cycalloc( sizeof( struct Cyc_Interface_Ienv));
_temp638->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp631))->v; _temp638->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp632))->v; _temp638->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp633))->v; _temp638->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp634))->v;
_temp638->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp635))->v; _temp638->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp636))->v; _temp638->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp637))->v; _temp638;});}}
struct _tuple15{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct Cyc_Dict_Dict* f4; struct _tagged_arr* f5; } ; void Cyc_Interface_compat_merge_ns_f(
struct _tuple15* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){
struct _tuple15 _temp641; struct _tagged_arr* _temp642; struct Cyc_Dict_Dict*
_temp644; struct Cyc_Dict_Dict* _temp646; struct Cyc_Dict_Dict* _temp648; struct
Cyc_Dict_Dict** _temp650; int _temp651; int* _temp653; struct _tuple15* _temp639=
env; _temp641=* _temp639; _LL652: _temp651= _temp641.f1; _temp653=( int*)&(*
_temp639).f1; goto _LL649; _LL649: _temp648= _temp641.f2; _temp650=( struct Cyc_Dict_Dict**)&(*
_temp639).f2; goto _LL647; _LL647: _temp646= _temp641.f3; goto _LL645; _LL645:
_temp644= _temp641.f4; goto _LL643; _LL643: _temp642= _temp641.f5; goto _LL640;
_LL640: { struct Cyc_Interface_Ienv* ie1; struct Cyc_Interface_Ienv* iexcl;{
struct _handler_cons _temp654; _push_handler(& _temp654);{ int _temp656= 0; if(
setjmp( _temp654.handler)){ _temp656= 1;} if( ! _temp656){ iexcl=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp644,
ns);; _pop_handler();} else{ void* _temp655=( void*) _exn_thrown; void* _temp658=
_temp655; _LL660: if( _temp658 ==  Cyc_Dict_Absent){ goto _LL661;} else{ goto
_LL662;} _LL662: goto _LL663; _LL661: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL659; _LL663:( void) _throw( _temp658); _LL659:;}}}{ struct _handler_cons
_temp664; _push_handler(& _temp664);{ int _temp666= 0; if( setjmp( _temp664.handler)){
_temp666= 1;} if( ! _temp666){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp646, ns);{ struct Cyc_Interface_Ienv*
_temp667= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp642); if( _temp667 ==  0){* _temp653= 0; _npop_handler( 0u); return;}
ie1=( struct Cyc_Interface_Ienv*) _check_null( _temp667);}; _pop_handler();}
else{ void* _temp665=( void*) _exn_thrown; void* _temp669= _temp665; _LL671: if(
_temp669 ==  Cyc_Dict_Absent){ goto _LL672;} else{ goto _LL673;} _LL673: goto
_LL674; _LL672: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL670; _LL674:( void)
_throw( _temp669); _LL670:;}}}{ struct Cyc_Interface_Ienv* _temp675= Cyc_Interface_compat_merge_ienv(
ie1, ie2, iexcl, _temp642); if( _temp675 ==  0){* _temp653= 0;} else{ if(*
_temp653){* _temp650=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp650, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp675));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_compat_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct _tagged_arr* msg){
struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); struct
_tuple15 _temp676=({ struct _tuple15 _temp678; _temp678.f1= 1; _temp678.f2= d;
_temp678.f3= dic1; _temp678.f4= excl; _temp678.f5= msg; _temp678;});(( void(*)(
void(* f)( struct _tuple15*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,&
_temp676, dic2); if( _temp676.f1){ return({ struct Cyc_Core_Opt* _temp677=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp677->v=(
void*) _temp676.f2; _temp677;});} else{ return 0;}} struct _tuple16{ int f1;
struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct _tagged_arr f4;
struct _tagged_arr* f5; } ; void Cyc_Interface_disj_merge_dict_f( struct
_tuple16* env, struct _tagged_arr* x, void* y){ struct _tuple16 _temp681; struct
_tagged_arr* _temp682; struct _tagged_arr _temp684; struct Cyc_Dict_Dict*
_temp686; struct Cyc_Dict_Dict* _temp688; struct Cyc_Dict_Dict** _temp690; int
_temp691; int* _temp693; struct _tuple16* _temp679= env; _temp681=* _temp679;
_LL692: _temp691= _temp681.f1; _temp693=( int*)&(* _temp679).f1; goto _LL689;
_LL689: _temp688= _temp681.f2; _temp690=( struct Cyc_Dict_Dict**)&(* _temp679).f2;
goto _LL687; _LL687: _temp686= _temp681.f3; goto _LL685; _LL685: _temp684=
_temp681.f4; goto _LL683; _LL683: _temp682= _temp681.f5; goto _LL680; _LL680:
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_member)(
_temp686, x)){ Cyc_Interface_check_err( _temp682,( struct _tagged_arr)({ struct
Cyc_Std_String_pa_struct _temp696; _temp696.tag= Cyc_Std_String_pa; _temp696.f1=(
struct _tagged_arr)* x;{ struct Cyc_Std_String_pa_struct _temp695; _temp695.tag=
Cyc_Std_String_pa; _temp695.f1=( struct _tagged_arr) _temp684;{ void* _temp694[
2u]={& _temp695,& _temp696}; Cyc_Std_aprintf( _tag_arr("%s %s is exported more than once",
sizeof( unsigned char), 33u), _tag_arr( _temp694, sizeof( void*), 2u));}}}));*
_temp693= 0;} else{ if(* _temp693){* _temp690=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(*
_temp690, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct
_tagged_arr* msg){ struct _tuple16 _temp697=({ struct _tuple16 _temp699;
_temp699.f1= 1; _temp699.f2= dic1; _temp699.f3= dic1; _temp699.f4= t; _temp699.f5=
msg; _temp699;});(( void(*)( void(* f)( struct _tuple16*, struct _tagged_arr*,
void*), struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_dict_f,&
_temp697, dic2); if( _temp697.f1){ return({ struct Cyc_Core_Opt* _temp698=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp698->v=(
void*) _temp697.f2; _temp698;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct _tagged_arr* msg){ struct Cyc_Core_Opt* _temp700=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Structdecl*(* merge_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp),
Cyc_Tcdecl_merge_structdecl, _tag_arr("struct", sizeof( unsigned char), 7u), msg);
struct Cyc_Core_Opt* _temp701=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->uniondecls, ie2->uniondecls, _tag_arr("union",
sizeof( unsigned char), 6u), msg); struct Cyc_Core_Opt* _temp702=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->tuniondecls, ie2->tuniondecls,
_tag_arr("[x]tunion", sizeof( unsigned char), 10u), msg); struct Cyc_Core_Opt*
_temp703=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->enumdecls, ie2->enumdecls, _tag_arr("enum", sizeof( unsigned char), 5u),
msg); struct Cyc_Core_Opt* _temp704=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp), Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef",
sizeof( unsigned char), 8u), msg); struct Cyc_Core_Opt* _temp705=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt*
_temp706=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls, _tag_arr("xtunionfield", sizeof(
unsigned char), 13u), msg); if(((((( !(( unsigned int) _temp700)? 1: !((
unsigned int) _temp701))? 1: !(( unsigned int) _temp702))? 1: !(( unsigned int)
_temp703))? 1: !(( unsigned int) _temp704))? 1: !(( unsigned int) _temp705))? 1:
!(( unsigned int) _temp706)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp707=( struct Cyc_Interface_Ienv*) _cycalloc( sizeof( struct Cyc_Interface_Ienv));
_temp707->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp700))->v; _temp707->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp701))->v; _temp707->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp702))->v; _temp707->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp703))->v;
_temp707->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp704))->v; _temp707->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp705))->v; _temp707->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp706))->v; _temp707;});}}
struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_arr* f4; } ; void Cyc_Interface_disj_merge_ns_f( struct _tuple17*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple17
_temp710; struct _tagged_arr* _temp711; struct Cyc_Dict_Dict* _temp713; struct
Cyc_Dict_Dict* _temp715; struct Cyc_Dict_Dict** _temp717; int _temp718; int*
_temp720; struct _tuple17* _temp708= env; _temp710=* _temp708; _LL719: _temp718=
_temp710.f1; _temp720=( int*)&(* _temp708).f1; goto _LL716; _LL716: _temp715=
_temp710.f2; _temp717=( struct Cyc_Dict_Dict**)&(* _temp708).f2; goto _LL714;
_LL714: _temp713= _temp710.f3; goto _LL712; _LL712: _temp711= _temp710.f4; goto
_LL709; _LL709: { struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons _temp721;
_push_handler(& _temp721);{ int _temp723= 0; if( setjmp( _temp721.handler)){
_temp723= 1;} if( ! _temp723){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp713, ns);; _pop_handler();}
else{ void* _temp722=( void*) _exn_thrown; void* _temp725= _temp722; _LL727: if(
_temp725 ==  Cyc_Dict_Absent){ goto _LL728;} else{ goto _LL729;} _LL729: goto
_LL730; _LL728: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL726; _LL730:( void)
_throw( _temp725); _LL726:;}}}{ struct Cyc_Interface_Ienv* _temp731= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp711); if( _temp731 ==  0){* _temp720= 0;} else{ if(* _temp720){*
_temp717=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp717, ns,( struct
Cyc_Interface_Ienv*) _check_null( _temp731));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){
struct _tuple17 _temp732=({ struct _tuple17 _temp734; _temp734.f1= 1; _temp734.f2=
dic1; _temp734.f3= dic1; _temp734.f4= msg; _temp734;});(( void(*)( void(* f)(
struct _tuple17*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct
_tuple17* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp732, dic2); if( _temp732.f1){ return({ struct Cyc_Core_Opt* _temp733=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp733->v=(
void*) _temp732.f2; _temp733;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp735; _push_handler(& _temp735);{ int _temp737= 0; if(
setjmp( _temp735.handler)){ _temp737= 1;} if( ! _temp737){{ struct _tagged_arr*
msg= 0; if( info !=  0){ msg=({ struct _tagged_arr* _temp738=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp738[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp741; _temp741.tag= Cyc_Std_String_pa;
_temp741.f1=( struct _tagged_arr)(*(( struct _tuple3*) _check_null( info))).f2;{
struct Cyc_Std_String_pa_struct _temp740; _temp740.tag= Cyc_Std_String_pa;
_temp740.f1=( struct _tagged_arr)(*(( struct _tuple3*) _check_null( info))).f1;{
void* _temp739[ 2u]={& _temp740,& _temp741}; Cyc_Std_aprintf( _tag_arr("merging exports of %s and %s,",
sizeof( unsigned char), 30u), _tag_arr( _temp739, sizeof( void*), 2u));}}});
_temp738;});}{ struct Cyc_Core_Opt* _temp742= Cyc_Interface_disj_merge_ns( i1->exports,
i2->exports, msg); if( _temp742 ==  0){ struct Cyc_Interface_I* _temp743= 0;
_npop_handler( 0u); return _temp743;} if( info !=  0){ msg=({ struct _tagged_arr*
_temp744=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp744[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp747;
_temp747.tag= Cyc_Std_String_pa; _temp747.f1=( struct _tagged_arr)(*(( struct
_tuple3*) _check_null( info))).f2;{ struct Cyc_Std_String_pa_struct _temp746;
_temp746.tag= Cyc_Std_String_pa; _temp746.f1=( struct _tagged_arr)(*(( struct
_tuple3*) _check_null( info))).f1;{ void* _temp745[ 2u]={& _temp746,& _temp747};
Cyc_Std_aprintf( _tag_arr("merging imports of %s and %s,", sizeof( unsigned char),
30u), _tag_arr( _temp745, sizeof( void*), 2u));}}}); _temp744;});}{ struct Cyc_Core_Opt*
_temp748= Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp742))->v, msg); if( _temp748 ==  0){
struct Cyc_Interface_I* _temp749= 0; _npop_handler( 0u); return _temp749;}{
struct Cyc_Interface_I* _temp751=({ struct Cyc_Interface_I* _temp750=( struct
Cyc_Interface_I*) _cycalloc( sizeof( struct Cyc_Interface_I)); _temp750->imports=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp748))->v;
_temp750->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp742))->v; _temp750;}); _npop_handler( 0u); return _temp751;}}}};
_pop_handler();} else{ void* _temp736=( void*) _exn_thrown; void* _temp753=
_temp736; _LL755: if( _temp753 ==  Cyc_Tcdecl_Incompatible){ goto _LL756;} else{
goto _LL757;} _LL757: goto _LL758; _LL756: return 0; _LL758:( void) _throw(
_temp753); _LL754:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list( struct
Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li ==  0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_arr* curr_info= linfo !=  0?( struct _tagged_arr*)(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl; if( linfo !=  0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; li !=  0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp759= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info !=  0? linfo !=  0: 0)?({ struct _tuple3* _temp764=( struct _tuple3*)
_cycalloc( sizeof( struct _tuple3)); _temp764->f1=*(( struct _tagged_arr*)
_check_null( curr_info)); _temp764->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp764;}): 0); if( _temp759 ==  0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp759); if( curr_info !=  0){
curr_info= linfo !=  0?({ struct _tagged_arr* _temp760=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp760[ 0]=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp763; _temp763.tag= Cyc_Std_String_pa;
_temp763.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd);{ struct Cyc_Std_String_pa_struct _temp762; _temp762.tag=
Cyc_Std_String_pa; _temp762.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_null( curr_info));{ void* _temp761[ 2u]={& _temp762,& _temp763}; Cyc_Std_aprintf(
_tag_arr("%s+%s", sizeof( unsigned char), 6u), _tag_arr( _temp761, sizeof( void*),
2u));}}}); _temp760;}): 0;} if( linfo !=  0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;}} return( struct Cyc_Interface_I*) curr_i;}} struct
Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(* get)(
void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo){ if( la ==  0){
return( struct Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I*
curr_i= get(( void*)(( struct Cyc_List_List*) _check_null( la))->hd); struct
_tagged_arr* curr_info= linfo !=  0?( struct _tagged_arr*)(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; la=(( struct Cyc_List_List*)
_check_null( la))->tl; if( linfo !=  0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; la !=  0; la=(( struct Cyc_List_List*)
_check_null( la))->tl){ struct Cyc_Interface_I* _temp765= Cyc_Interface_merge(
curr_i, get(( void*)(( struct Cyc_List_List*) _check_null( la))->hd),( curr_info
!=  0? linfo !=  0: 0)?({ struct _tuple3* _temp770=( struct _tuple3*) _cycalloc(
sizeof( struct _tuple3)); _temp770->f1=*(( struct _tagged_arr*) _check_null(
curr_info)); _temp770->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp770;}): 0); if( _temp765 ==  0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp765); if( curr_info !=  0){
curr_info= linfo !=  0?({ struct _tagged_arr* _temp766=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp766[ 0]=( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp769; _temp769.tag= Cyc_Std_String_pa;
_temp769.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd);{ struct Cyc_Std_String_pa_struct _temp768; _temp768.tag=
Cyc_Std_String_pa; _temp768.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_null( curr_info));{ void* _temp767[ 2u]={& _temp768,& _temp769}; Cyc_Std_aprintf(
_tag_arr("%s+%s", sizeof( unsigned char), 6u), _tag_arr( _temp767, sizeof( void*),
2u));}}}); _temp766;}): 0;} if( linfo !=  0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;}} return( struct Cyc_Interface_I*) curr_i;}} static
struct Cyc_List_List* Cyc_Interface_add_namespace( struct Cyc_List_List* tds,
struct Cyc_List_List* ns){ if( ns ==  0){ return tds;} return({ struct Cyc_List_List*
_temp771=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp771->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Namespace_d_struct*
_temp772=( struct Cyc_Absyn_Namespace_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp772[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp773; _temp773.tag= Cyc_Absyn_Namespace_d;
_temp773.f1=( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp773.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp773;}); _temp772;}), 0); _temp771->tl= 0; _temp771;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_arr* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp774=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp774->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp775=( struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp775[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp776; _temp776.tag= Cyc_Absyn_Struct_d;
_temp776.f1= d; _temp776;}); _temp775;}), 0); _temp774->tl= tds; _temp774;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp777=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp777[ 0]=* d; _temp777;}); d->fields=
0; if(( void*) d->sc != ( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp778=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp778->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp779=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp779[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp780; _temp780.tag= Cyc_Absyn_Struct_d; _temp780.f1=
d; _temp780;}); _temp779;}), 0); _temp778->tl= tds; _temp778;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp781=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp781->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp782=( struct
Cyc_Absyn_Union_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp782[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp783; _temp783.tag= Cyc_Absyn_Union_d;
_temp783.f1= d; _temp783;}); _temp782;}), 0); _temp781->tl= tds; _temp781;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp784=( struct Cyc_Absyn_Uniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp784[ 0]=* d; _temp784;}); d->fields=
0; if(( void*) d->sc != ( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp785=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp785->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp786=( struct Cyc_Absyn_Union_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp786[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp787; _temp787.tag= Cyc_Absyn_Union_d; _temp787.f1= d; _temp787;}); _temp786;}),
0); _temp785->tl= tds; _temp785;});} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp788=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp788->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp789=( struct Cyc_Absyn_Tunion_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp789[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp790; _temp790.tag= Cyc_Absyn_Tunion_d; _temp790.f1=
d; _temp790;}); _temp789;}), 0); _temp788->tl= tds; _temp788;});} static
unsigned char _temp791[ 2u]="_"; static struct _tagged_arr Cyc_Interface_us={
_temp791, _temp791, _temp791 +  2u}; static struct _tagged_arr* Cyc_Interface_us_p=&
Cyc_Interface_us; static struct _tuple9* Cyc_Interface_rewrite_tunionfield_type(
struct _tuple9* x){ return({ struct _tuple9* _temp792=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp792->f1= Cyc_Absyn_empty_tqual();
_temp792->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp793=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp793[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp794; _temp794.tag= Cyc_Absyn_VarType; _temp794.f1=({ struct Cyc_Absyn_Tvar*
_temp795=( struct Cyc_Absyn_Tvar*) _cycalloc( sizeof( struct Cyc_Absyn_Tvar));
_temp795->name= Cyc_Interface_us_p; _temp795->identity= 0; _temp795->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_BoxKind); _temp795;}); _temp794;}); _temp793;}); _temp792;});}
static struct Cyc_Absyn_Tunionfield* Cyc_Interface_rewrite_tunionfield( struct
Cyc_Absyn_Tunionfield* f){ f=({ struct Cyc_Absyn_Tunionfield* _temp796=( struct
Cyc_Absyn_Tunionfield*) _cycalloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp796[ 0]=* f; _temp796;}); f->typs=(( struct Cyc_List_List*(*)( struct
_tuple9*(* f)( struct _tuple9*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield_type,
f->typs); return f;} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
d=({ struct Cyc_Absyn_Tuniondecl* _temp797=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp797[ 0]=* d; _temp797;});
if( d->fields !=  0){ d->fields=({ struct Cyc_Core_Opt* _temp798=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp798->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v); _temp798;});} if(( void*) d->sc
!= ( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern));}
return({ struct Cyc_List_List* _temp799=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp799->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp800=( struct Cyc_Absyn_Tunion_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp800[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp801; _temp801.tag= Cyc_Absyn_Tunion_d; _temp801.f1=
d; _temp801;}); _temp800;}), 0); _temp799->tl= tds; _temp799;});} static struct
Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp802=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp802->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp803=( struct
Cyc_Absyn_Enum_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp803[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp804; _temp804.tag= Cyc_Absyn_Enum_d;
_temp804.f1= d; _temp804;}); _temp803;}), 0); _temp802->tl= tds; _temp802;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp805=( struct Cyc_Absyn_Enumdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp805[ 0]=* d; _temp805;}); d->fields= 0;
if(( void*) d->sc != ( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp806=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp806->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Enum_d_struct* _temp807=( struct Cyc_Absyn_Enum_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp807[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp808; _temp808.tag= Cyc_Absyn_Enum_d; _temp808.f1= d; _temp808;}); _temp807;}),
0); _temp806->tl= tds; _temp806;});} static struct Cyc_List_List* Cyc_Interface_add_typedef(
struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp809=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp809->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp810=( struct Cyc_Absyn_Typedef_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp810[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp811; _temp811.tag= Cyc_Absyn_Typedef_d; _temp811.f1=
d; _temp811;}); _temp810;}), 0); _temp809->tl= tds; _temp809;});} static struct
Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_arr* x, struct Cyc_Absyn_Vardecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp812=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp812->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp813=( struct
Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp813[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp814; _temp814.tag= Cyc_Absyn_Var_d;
_temp814.f1= d; _temp814;}); _temp813;}), 0); _temp812->tl= tds; _temp812;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp817; struct Cyc_Absyn_Tunionfield*
_temp818; struct Cyc_Absyn_Tuniondecl* _temp820; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp815= d; _temp817=* _temp815; _LL821: _temp820= _temp817.base; goto _LL819;
_LL819: _temp818= _temp817.field; goto _LL816; _LL816: _temp820=({ struct Cyc_Absyn_Tuniondecl*
_temp822=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp822[ 0]=* _temp820; _temp822;}); _temp820->fields=({ struct Cyc_Core_Opt*
_temp823=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp823->v=( void*)({ struct Cyc_List_List* _temp824=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp824->hd=( void*) _temp818;
_temp824->tl= 0; _temp824;}); _temp823;});( void*)( _temp820->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp825=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp825->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp826=( struct Cyc_Absyn_Tunion_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp826[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp827; _temp827.tag= Cyc_Absyn_Tunion_d; _temp827.f1=
_temp820; _temp827;}); _temp826;}), 0); _temp825->tl= tds; _temp825;});} static
struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl_header( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp830; struct Cyc_Absyn_Tunionfield*
_temp831; struct Cyc_Absyn_Tuniondecl* _temp833; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp828= d; _temp830=* _temp828; _LL834: _temp833= _temp830.base; goto _LL832;
_LL832: _temp831= _temp830.field; goto _LL829; _LL829: _temp833=({ struct Cyc_Absyn_Tuniondecl*
_temp835=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp835[ 0]=* _temp833; _temp835;}); _temp831= Cyc_Interface_rewrite_tunionfield(
_temp831);( void*)( _temp831->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp833->fields=({
struct Cyc_Core_Opt* _temp836=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp836->v=( void*)({ struct Cyc_List_List* _temp837=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp837->hd=( void*)
_temp831; _temp837->tl= 0; _temp837;}); _temp836;});( void*)( _temp833->sc=(
void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp838=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp838->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp839=(
struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp839[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp840; _temp840.tag= Cyc_Absyn_Tunion_d;
_temp840.f1= _temp833; _temp840;}); _temp839;}), 0); _temp838->tl= tds; _temp838;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Std___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp841= 0; _temp841=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header,
ie->structdecls, _temp841); _temp841=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp841); _temp841=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp841); _temp841=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp841); if( _temp841 !=  0){ _temp841=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp841); _temp841= Cyc_Interface_add_namespace(
_temp841, ns); Cyc_Absynpp_decllist2file( _temp841, f);}} static void Cyc_Interface_print_ns_xtunionfielddecl_headers(
struct Cyc_Std___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp842= 0; _temp842=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl_header, ie->xtunionfielddecls,
_temp842); if( _temp842 !=  0){ _temp842=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp842); _temp842= Cyc_Interface_add_namespace(
_temp842, ns); Cyc_Absynpp_decllist2file( _temp842, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Std___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp843= 0; _temp843=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Typedefdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls, _temp843); if(
_temp843 !=  0){ _temp843=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp843); _temp843= Cyc_Interface_add_namespace( _temp843,
ns); Cyc_Absynpp_decllist2file( _temp843, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Std___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp844= 0; _temp844=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls, _temp844);
_temp844=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl, ie->uniondecls,
_temp844); _temp844=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp844); _temp844=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp844); _temp844=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp844); _temp844=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp844); if( _temp844 !=  0){ _temp844=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp844); _temp844= Cyc_Interface_add_namespace(
_temp844, ns); Cyc_Absynpp_decllist2file( _temp844, f);}} void Cyc_Interface_print(
struct Cyc_Interface_I* i, struct Cyc_Std___sFILE* f){ Cyc_Absynpp_set_params(&
Cyc_Absynpp_cyci_params_r);({ void* _temp845[ 0u]={}; Cyc_Std_fprintf( f,
_tag_arr("/****** needed (headers) ******/\n", sizeof( unsigned char), 34u),
_tag_arr( _temp845, sizeof( void*), 0u));});(( void(*)( void(* f)( struct Cyc_Std___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Std___sFILE* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers, f, i->imports);({
void* _temp846[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** provided (headers) ******/\n",
sizeof( unsigned char), 37u), _tag_arr( _temp846, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->exports);({ void* _temp847[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("/****** needed (headers of xtunionfielddecls) ******/\n",
sizeof( unsigned char), 55u), _tag_arr( _temp847, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->imports);({ void* _temp848[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** provided (headers of xtunionfielddecls) ******/\n",
sizeof( unsigned char), 58u), _tag_arr( _temp848, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->exports);({ void* _temp849[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** provided (typedefs) ******/\n",
sizeof( unsigned char), 38u), _tag_arr( _temp849, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_typedefs,
f, i->exports);({ void* _temp850[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** needed (declarations) ******/\n",
sizeof( unsigned char), 40u), _tag_arr( _temp850, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->imports);({ void* _temp851[ 0u]={}; Cyc_Std_fprintf( f, _tag_arr("\n/****** provided (declarations) ******/\n",
sizeof( unsigned char), 42u), _tag_arr( _temp851, sizeof( void*), 0u));});((
void(*)( void(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Std___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->exports);} struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Std___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp852= Cyc_Parse_parse_file( f);
struct Cyc_Tcenv_Tenv* _temp853= Cyc_Tcenv_tc_init(); Cyc_Tc_tc( _temp853, 0,
_temp852); return Cyc_Interface_gen_extract( _temp853->ae, 0);}} void Cyc_Interface_save(
struct Cyc_Interface_I* i, struct Cyc_Std___sFILE* f){ Cyc_Interface_print( i, f);}
struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Std___sFILE* f){ return
Cyc_Interface_parse( f);}
