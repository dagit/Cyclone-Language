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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct Cyc_std___sFILE; extern unsigned char Cyc_std_FileCloseError[ 19u];
extern unsigned char Cyc_std_FileOpenError[ 18u]; struct Cyc_std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_std_String_pa=
0; struct Cyc_std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_std_Int_pa= 1; struct Cyc_std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_std_Double_pa= 2; struct Cyc_std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa= 3; struct Cyc_std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_std_ShortPtr_sa= 0;
struct Cyc_std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_std_UShortPtr_sa=
1; struct Cyc_std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_std_IntPtr_sa= 2; struct Cyc_std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_std_UIntPtr_sa= 3; struct Cyc_std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_std_StringPtr_sa= 4; struct
Cyc_std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern int Cyc_Evexp_okay_szofarg( void* t); void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e); void* Cyc_Evexp_promote_const( void* cn){ void* _temp0=
cn; unsigned char _temp8; void* _temp10; short _temp12; void* _temp14; _LL2: if((
unsigned int) _temp0 >  1u?*(( int*) _temp0) ==  Cyc_Absyn_Char_c: 0){ _LL11:
_temp10=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp0)->f1; goto _LL9; _LL9:
_temp8=(( struct Cyc_Absyn_Char_c_struct*) _temp0)->f2; goto _LL3;} else{ goto
_LL4;} _LL4: if(( unsigned int) _temp0 >  1u?*(( int*) _temp0) ==  Cyc_Absyn_Short_c:
0){ _LL15: _temp14=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp0)->f1;
goto _LL13; _LL13: _temp12=(( struct Cyc_Absyn_Short_c_struct*) _temp0)->f2;
goto _LL5;} else{ goto _LL6;} _LL6: goto _LL7; _LL3: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp16=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp16[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp17; _temp17.tag= Cyc_Absyn_Int_c;
_temp17.f1=( void*) _temp10; _temp17.f2=( int) _temp8; _temp17;}); _temp16;});
_LL5: return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp18=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp18[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp19; _temp19.tag= Cyc_Absyn_Int_c; _temp19.f1=( void*) _temp14; _temp19.f2=(
int) _temp12; _temp19;}); _temp18;}); _LL7: return cn; _LL1:;} unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e){ void* _temp20= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp26; _LL22: if(( unsigned int) _temp20 >  1u?*(( int*) _temp20) == 
Cyc_Absyn_Int_c: 0){ _LL27: _temp26=(( struct Cyc_Absyn_Int_c_struct*) _temp20)->f2;
goto _LL23;} else{ goto _LL24;} _LL24: goto _LL25; _LL23: return( unsigned int)
_temp26; _LL25:({ void* _temp28[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("expecting unsigned int",
sizeof( unsigned char), 23u), _tag_arr( _temp28, sizeof( void*), 0u));}); return
0u; _LL21:;} int Cyc_Evexp_eval_const_bool_exp( struct Cyc_Absyn_Exp* e){ void*
_temp29= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e)); int _temp39;
long long _temp41; _LL31: if(( unsigned int) _temp29 >  1u?*(( int*) _temp29) == 
Cyc_Absyn_Int_c: 0){ _LL40: _temp39=(( struct Cyc_Absyn_Int_c_struct*) _temp29)->f2;
goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp29 >  1u?*(( int*)
_temp29) ==  Cyc_Absyn_LongLong_c: 0){ _LL42: _temp41=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp29)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if( _temp29 == ( void*) Cyc_Absyn_Null_c){
goto _LL36;} else{ goto _LL37;} _LL37: goto _LL38; _LL32: return _temp39 !=  0;
_LL34: return _temp41 !=  0; _LL36: return 0; _LL38:({ void* _temp43[ 0u]={};
Cyc_Tcutil_terr( e->loc, _tag_arr("expecting bool", sizeof( unsigned char), 15u),
_tag_arr( _temp43, sizeof( void*), 0u));}); return 0; _LL30:;} struct _tuple3{
struct Cyc_Absyn_Tqual f1; void* f2; } ; int Cyc_Evexp_okay_szofarg( void* t){
void* _temp44= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp96; struct
Cyc_Core_Opt* _temp98; struct Cyc_Core_Opt _temp100; void* _temp101; struct Cyc_Absyn_TunionFieldInfo
_temp103; void* _temp105; struct Cyc_Absyn_Tunionfield* _temp107; struct Cyc_List_List*
_temp109; struct Cyc_Absyn_PtrInfo _temp111; struct Cyc_Absyn_Conref* _temp113;
struct Cyc_Absyn_Exp* _temp115; void* _temp117; struct Cyc_Absyn_Structdecl**
_temp119; struct Cyc_List_List* _temp121; struct Cyc_Absyn_Uniondecl** _temp123;
struct Cyc_List_List* _temp125; struct Cyc_List_List* _temp127; _LL46: if(
_temp44 == ( void*) Cyc_Absyn_VoidType){ goto _LL47;} else{ goto _LL48;} _LL48:
if(( unsigned int) _temp44 >  4u?*(( int*) _temp44) ==  Cyc_Absyn_VarType: 0){
_LL97: _temp96=(( struct Cyc_Absyn_VarType_struct*) _temp44)->f1; goto _LL49;}
else{ goto _LL50;} _LL50: if(( unsigned int) _temp44 >  4u?*(( int*) _temp44) == 
Cyc_Absyn_Evar: 0){ _LL99: _temp98=(( struct Cyc_Absyn_Evar_struct*) _temp44)->f1;
if( _temp98 ==  0){ goto _LL52;} else{ _temp100=* _temp98; _LL102: _temp101=(
void*) _temp100.v; if( _temp101 == ( void*) Cyc_Absyn_BoxKind){ goto _LL51;}
else{ goto _LL52;}}} else{ goto _LL52;} _LL52: if(( unsigned int) _temp44 >  4u?*((
int*) _temp44) ==  Cyc_Absyn_Evar: 0){ goto _LL53;} else{ goto _LL54;} _LL54:
if(( unsigned int) _temp44 >  4u?*(( int*) _temp44) ==  Cyc_Absyn_TunionType: 0){
goto _LL55;} else{ goto _LL56;} _LL56: if(( unsigned int) _temp44 >  4u?*(( int*)
_temp44) ==  Cyc_Absyn_TunionFieldType: 0){ _LL104: _temp103=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp44)->f1; _LL106: _temp105=( void*) _temp103.field_info; if(*(( int*)
_temp105) ==  Cyc_Absyn_KnownTunionfield){ _LL108: _temp107=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp105)->f2; goto _LL57;} else{ goto _LL58;}} else{ goto _LL58;} _LL58: if((
unsigned int) _temp44 >  4u?*(( int*) _temp44) ==  Cyc_Absyn_TupleType: 0){
_LL110: _temp109=(( struct Cyc_Absyn_TupleType_struct*) _temp44)->f1; goto _LL59;}
else{ goto _LL60;} _LL60: if(( unsigned int) _temp44 >  4u?*(( int*) _temp44) == 
Cyc_Absyn_TunionFieldType: 0){ goto _LL61;} else{ goto _LL62;} _LL62: if((
unsigned int) _temp44 >  4u?*(( int*) _temp44) ==  Cyc_Absyn_PointerType: 0){
_LL112: _temp111=(( struct Cyc_Absyn_PointerType_struct*) _temp44)->f1; _LL114:
_temp113= _temp111.bounds; goto _LL63;} else{ goto _LL64;} _LL64: if((
unsigned int) _temp44 >  4u?*(( int*) _temp44) ==  Cyc_Absyn_IntType: 0){ goto
_LL65;} else{ goto _LL66;} _LL66: if( _temp44 == ( void*) Cyc_Absyn_FloatType){
goto _LL67;} else{ goto _LL68;} _LL68: if( _temp44 == ( void*) Cyc_Absyn_DoubleType){
goto _LL69;} else{ goto _LL70;} _LL70: if(( unsigned int) _temp44 >  4u?*(( int*)
_temp44) ==  Cyc_Absyn_ArrayType: 0){ _LL118: _temp117=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp44)->f1; goto _LL116; _LL116: _temp115=(( struct Cyc_Absyn_ArrayType_struct*)
_temp44)->f3; goto _LL71;} else{ goto _LL72;} _LL72: if(( unsigned int) _temp44
>  4u?*(( int*) _temp44) ==  Cyc_Absyn_FnType: 0){ goto _LL73;} else{ goto _LL74;}
_LL74: if(( unsigned int) _temp44 >  4u?*(( int*) _temp44) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL75;} else{ goto _LL76;} _LL76: if(( unsigned int) _temp44 >  4u?*((
int*) _temp44) ==  Cyc_Absyn_StructType: 0){ _LL120: _temp119=(( struct Cyc_Absyn_StructType_struct*)
_temp44)->f3; goto _LL77;} else{ goto _LL78;} _LL78: if(( unsigned int) _temp44
>  4u?*(( int*) _temp44) ==  Cyc_Absyn_AnonStructType: 0){ _LL122: _temp121=((
struct Cyc_Absyn_AnonStructType_struct*) _temp44)->f1; goto _LL79;} else{ goto
_LL80;} _LL80: if(( unsigned int) _temp44 >  4u?*(( int*) _temp44) ==  Cyc_Absyn_UnionType:
0){ _LL126: _temp125=(( struct Cyc_Absyn_UnionType_struct*) _temp44)->f2; goto
_LL124; _LL124: _temp123=(( struct Cyc_Absyn_UnionType_struct*) _temp44)->f3;
goto _LL81;} else{ goto _LL82;} _LL82: if(( unsigned int) _temp44 >  4u?*(( int*)
_temp44) ==  Cyc_Absyn_AnonUnionType: 0){ _LL128: _temp127=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp44)->f1; goto _LL83;} else{ goto _LL84;} _LL84: if(( unsigned int) _temp44
>  4u?*(( int*) _temp44) ==  Cyc_Absyn_EnumType: 0){ goto _LL85;} else{ goto
_LL86;} _LL86: if( _temp44 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL87;} else{
goto _LL88;} _LL88: if(( unsigned int) _temp44 >  4u?*(( int*) _temp44) ==  Cyc_Absyn_JoinEff:
0){ goto _LL89;} else{ goto _LL90;} _LL90: if(( unsigned int) _temp44 >  4u?*((
int*) _temp44) ==  Cyc_Absyn_AccessEff: 0){ goto _LL91;} else{ goto _LL92;}
_LL92: if(( unsigned int) _temp44 >  4u?*(( int*) _temp44) ==  Cyc_Absyn_RgnsEff:
0){ goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp44 >  4u?*((
int*) _temp44) ==  Cyc_Absyn_TypedefType: 0){ goto _LL95;} else{ goto _LL45;}
_LL47: return 0; _LL49: { void* _temp129= Cyc_Absyn_conref_val( _temp96->kind);
_LL131: if( _temp129 == ( void*) Cyc_Absyn_BoxKind){ goto _LL132;} else{ goto
_LL133;} _LL133: goto _LL134; _LL132: return 1; _LL134: return 0; _LL130:;}
_LL51: return 1; _LL53: return 0; _LL55: return 1; _LL57: _temp109= _temp107->typs;
goto _LL59; _LL59: for( 0; _temp109 !=  0; _temp109=(( struct Cyc_List_List*)
_check_null( _temp109))->tl){ if( ! Cyc_Evexp_okay_szofarg((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp109))->hd)).f2)){ return 0;}} return 1;
_LL61: return 0; _LL63: { void* _temp135=( void*)( Cyc_Absyn_compress_conref(
_temp113))->v; void* _temp143; _LL137: if(( unsigned int) _temp135 >  1u?*(( int*)
_temp135) ==  Cyc_Absyn_Eq_constr: 0){ _LL144: _temp143=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp135)->f1; if( _temp143 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL138;}
else{ goto _LL139;}} else{ goto _LL139;} _LL139: if(( unsigned int) _temp135 > 
1u?*(( int*) _temp135) ==  Cyc_Absyn_Eq_constr: 0){ goto _LL140;} else{ goto
_LL141;} _LL141: goto _LL142; _LL138: return 1; _LL140: return 1; _LL142: return
0; _LL136:;} _LL65: return 1; _LL67: return 1; _LL69: return 1; _LL71: return
_temp115 !=  0; _LL73: return 0; _LL75: return 1; _LL77: if( _temp119 ==  0){
return({ void* _temp145[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("szof on unchecked StructType",
sizeof( unsigned char), 29u), _tag_arr( _temp145, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* _temp146=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp119)); if( _temp146->fields ==  0){ return 0;} _temp121=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp146->fields))->v;
goto _LL79;} _LL79: for( 0; _temp121 !=  0; _temp121=(( struct Cyc_List_List*)
_check_null( _temp121))->tl){ if( ! Cyc_Evexp_okay_szofarg(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp121))->hd)->type)){ return 0;}} return
1; _LL81: if( _temp123 ==  0){ return({ void* _temp147[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("szof on unchecked UnionType",
sizeof( unsigned char), 28u), _tag_arr( _temp147, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Uniondecl* _temp148=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp123)); if( _temp148->fields ==  0){ return 0;} _temp127=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp148->fields))->v;
goto _LL83;} _LL83: for( 0; _temp127 !=  0; _temp127=(( struct Cyc_List_List*)
_check_null( _temp127))->tl){ if( ! Cyc_Evexp_okay_szofarg(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp127))->hd)->type)){ return 0;}} return
1; _LL85: return 1; _LL87: return 0; _LL89: return 0; _LL91: return 0; _LL93:
return 0; _LL95: return({ void* _temp149[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(( struct _tagged_arr)({ struct
Cyc_std_String_pa_struct _temp151; _temp151.tag= Cyc_std_String_pa; _temp151.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp150[ 1u]={& _temp151};
Cyc_std_aprintf( _tag_arr("szof typedeftype %s", sizeof( unsigned char), 20u),
_tag_arr( _temp150, sizeof( void*), 1u));}}), _tag_arr( _temp149, sizeof( void*),
0u));}); _LL45:;} struct _tuple4{ void* f1; void* f2; } ; void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple4 _temp153=({ struct _tuple4 _temp152; _temp152.f1= p;
_temp152.f2= cn; _temp152;}); void* _temp167; void* _temp169; int _temp171; void*
_temp173; void* _temp175; void* _temp177; int _temp179; void* _temp181; void*
_temp183; void* _temp185; int _temp187; void* _temp189; void* _temp191; void*
_temp193; _LL155: _LL168: _temp167= _temp153.f1; if( _temp167 == ( void*) Cyc_Absyn_Plus){
goto _LL156;} else{ goto _LL157;} _LL157: _LL176: _temp175= _temp153.f1; if(
_temp175 == ( void*) Cyc_Absyn_Minus){ goto _LL170;} else{ goto _LL159;} _LL170:
_temp169= _temp153.f2; if(( unsigned int) _temp169 >  1u?*(( int*) _temp169) == 
Cyc_Absyn_Int_c: 0){ _LL174: _temp173=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp169)->f1; goto _LL172; _LL172: _temp171=(( struct Cyc_Absyn_Int_c_struct*)
_temp169)->f2; goto _LL158;} else{ goto _LL159;} _LL159: _LL184: _temp183=
_temp153.f1; if( _temp183 == ( void*) Cyc_Absyn_Bitnot){ goto _LL178;} else{
goto _LL161;} _LL178: _temp177= _temp153.f2; if(( unsigned int) _temp177 >  1u?*((
int*) _temp177) ==  Cyc_Absyn_Int_c: 0){ _LL182: _temp181=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp177)->f1; goto _LL180; _LL180: _temp179=(( struct Cyc_Absyn_Int_c_struct*)
_temp177)->f2; goto _LL160;} else{ goto _LL161;} _LL161: _LL190: _temp189=
_temp153.f1; if( _temp189 == ( void*) Cyc_Absyn_Not){ goto _LL186;} else{ goto
_LL163;} _LL186: _temp185= _temp153.f2; if(( unsigned int) _temp185 >  1u?*((
int*) _temp185) ==  Cyc_Absyn_Int_c: 0){ _LL188: _temp187=(( struct Cyc_Absyn_Int_c_struct*)
_temp185)->f2; goto _LL162;} else{ goto _LL163;} _LL163: _LL194: _temp193=
_temp153.f1; if( _temp193 == ( void*) Cyc_Absyn_Not){ goto _LL192;} else{ goto
_LL165;} _LL192: _temp191= _temp153.f2; if( _temp191 == ( void*) Cyc_Absyn_Null_c){
goto _LL164;} else{ goto _LL165;} _LL165: goto _LL166; _LL156: return cn; _LL158:
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp195=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp195[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp196; _temp196.tag= Cyc_Absyn_Int_c; _temp196.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp196.f2= - _temp171; _temp196;}); _temp195;}); _LL160: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp197=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp197[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp198; _temp198.tag= Cyc_Absyn_Int_c; _temp198.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp198.f2= ~ _temp179; _temp198;}); _temp197;}); _LL162: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp199=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp199[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp200; _temp200.tag= Cyc_Absyn_Int_c; _temp200.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp200.f2= _temp187 ==  0? 1: 0; _temp200;}); _temp199;}); _LL164: return(
void*)({ struct Cyc_Absyn_Int_c_struct* _temp201=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp201[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp202; _temp202.tag= Cyc_Absyn_Int_c; _temp202.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp202.f2= 1; _temp202;}); _temp201;}); _LL166:({ void* _temp203[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("bad constant expression", sizeof( unsigned char), 24u),
_tag_arr( _temp203, sizeof( void*), 0u));}); return cn; _LL154:;} struct _tuple5{
void* f1; int f2; } ; void* Cyc_Evexp_eval_const_binprimop( void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp204= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e1)); void* _temp205= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e2));
void* s1; void* s2; int i1; int i2;{ void* _temp206= _temp204; int _temp212;
void* _temp214; _LL208: if(( unsigned int) _temp206 >  1u?*(( int*) _temp206) == 
Cyc_Absyn_Int_c: 0){ _LL215: _temp214=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp206)->f1; goto _LL213; _LL213: _temp212=(( struct Cyc_Absyn_Int_c_struct*)
_temp206)->f2; goto _LL209;} else{ goto _LL210;} _LL210: goto _LL211; _LL209: s1=
_temp214; i1= _temp212; goto _LL207; _LL211:({ void* _temp216[ 0u]={}; Cyc_Tcutil_terr(
e1->loc, _tag_arr("bad constant expression", sizeof( unsigned char), 24u),
_tag_arr( _temp216, sizeof( void*), 0u));}); return _temp204; _LL207:;}{ void*
_temp217= _temp205; int _temp223; void* _temp225; _LL219: if(( unsigned int)
_temp217 >  1u?*(( int*) _temp217) ==  Cyc_Absyn_Int_c: 0){ _LL226: _temp225=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp217)->f1; goto _LL224; _LL224:
_temp223=(( struct Cyc_Absyn_Int_c_struct*) _temp217)->f2; goto _LL220;} else{
goto _LL221;} _LL221: goto _LL222; _LL220: s2= _temp225; i2= _temp223; goto
_LL218; _LL222:({ void* _temp227[ 0u]={}; Cyc_Tcutil_terr( e2->loc, _tag_arr("bad constant expression",
sizeof( unsigned char), 24u), _tag_arr( _temp227, sizeof( void*), 0u));});
return _temp204; _LL218:;}{ void* _temp228= p; _LL230: if( _temp228 == ( void*)
Cyc_Absyn_Div){ goto _LL231;} else{ goto _LL232;} _LL232: if( _temp228 == ( void*)
Cyc_Absyn_Mod){ goto _LL233;} else{ goto _LL234;} _LL234: goto _LL235; _LL231:
goto _LL233; _LL233: if( i2 ==  0){({ void* _temp236[ 0u]={}; Cyc_Tcutil_terr(
e2->loc, _tag_arr("division by zero in constant expression", sizeof(
unsigned char), 40u), _tag_arr( _temp236, sizeof( void*), 0u));}); return
_temp204;} goto _LL229; _LL235: goto _LL229; _LL229:;}{ int has_u_arg= s1 == (
void*) Cyc_Absyn_Unsigned? 1: s2 == ( void*) Cyc_Absyn_Unsigned; unsigned int u1=(
unsigned int) i1; unsigned int u2=( unsigned int) i2; int i3= 0; unsigned int u3=
0; int b3= 1; int use_i3= 0; int use_u3= 0; int use_b3= 0;{ struct _tuple5
_temp238=({ struct _tuple5 _temp237; _temp237.f1= p; _temp237.f2= has_u_arg;
_temp237;}); int _temp294; void* _temp296; int _temp298; void* _temp300; int
_temp302; void* _temp304; int _temp306; void* _temp308; int _temp310; void*
_temp312; int _temp314; void* _temp316; int _temp318; void* _temp320; int
_temp322; void* _temp324; int _temp326; void* _temp328; int _temp330; void*
_temp332; void* _temp334; void* _temp336; int _temp338; void* _temp340; int
_temp342; void* _temp344; int _temp346; void* _temp348; int _temp350; void*
_temp352; int _temp354; void* _temp356; int _temp358; void* _temp360; int
_temp362; void* _temp364; int _temp366; void* _temp368; void* _temp370; void*
_temp372; void* _temp374; void* _temp376; void* _temp378; void* _temp380; _LL240:
_LL297: _temp296= _temp238.f1; if( _temp296 == ( void*) Cyc_Absyn_Plus){ goto
_LL295;} else{ goto _LL242;} _LL295: _temp294= _temp238.f2; if( _temp294 ==  0){
goto _LL241;} else{ goto _LL242;} _LL242: _LL301: _temp300= _temp238.f1; if(
_temp300 == ( void*) Cyc_Absyn_Times){ goto _LL299;} else{ goto _LL244;} _LL299:
_temp298= _temp238.f2; if( _temp298 ==  0){ goto _LL243;} else{ goto _LL244;}
_LL244: _LL305: _temp304= _temp238.f1; if( _temp304 == ( void*) Cyc_Absyn_Minus){
goto _LL303;} else{ goto _LL246;} _LL303: _temp302= _temp238.f2; if( _temp302 == 
0){ goto _LL245;} else{ goto _LL246;} _LL246: _LL309: _temp308= _temp238.f1; if(
_temp308 == ( void*) Cyc_Absyn_Div){ goto _LL307;} else{ goto _LL248;} _LL307:
_temp306= _temp238.f2; if( _temp306 ==  0){ goto _LL247;} else{ goto _LL248;}
_LL248: _LL313: _temp312= _temp238.f1; if( _temp312 == ( void*) Cyc_Absyn_Mod){
goto _LL311;} else{ goto _LL250;} _LL311: _temp310= _temp238.f2; if( _temp310 == 
0){ goto _LL249;} else{ goto _LL250;} _LL250: _LL317: _temp316= _temp238.f1; if(
_temp316 == ( void*) Cyc_Absyn_Plus){ goto _LL315;} else{ goto _LL252;} _LL315:
_temp314= _temp238.f2; if( _temp314 ==  1){ goto _LL251;} else{ goto _LL252;}
_LL252: _LL321: _temp320= _temp238.f1; if( _temp320 == ( void*) Cyc_Absyn_Times){
goto _LL319;} else{ goto _LL254;} _LL319: _temp318= _temp238.f2; if( _temp318 == 
1){ goto _LL253;} else{ goto _LL254;} _LL254: _LL325: _temp324= _temp238.f1; if(
_temp324 == ( void*) Cyc_Absyn_Minus){ goto _LL323;} else{ goto _LL256;} _LL323:
_temp322= _temp238.f2; if( _temp322 ==  1){ goto _LL255;} else{ goto _LL256;}
_LL256: _LL329: _temp328= _temp238.f1; if( _temp328 == ( void*) Cyc_Absyn_Div){
goto _LL327;} else{ goto _LL258;} _LL327: _temp326= _temp238.f2; if( _temp326 == 
1){ goto _LL257;} else{ goto _LL258;} _LL258: _LL333: _temp332= _temp238.f1; if(
_temp332 == ( void*) Cyc_Absyn_Mod){ goto _LL331;} else{ goto _LL260;} _LL331:
_temp330= _temp238.f2; if( _temp330 ==  1){ goto _LL259;} else{ goto _LL260;}
_LL260: _LL335: _temp334= _temp238.f1; if( _temp334 == ( void*) Cyc_Absyn_Eq){
goto _LL261;} else{ goto _LL262;} _LL262: _LL337: _temp336= _temp238.f1; if(
_temp336 == ( void*) Cyc_Absyn_Neq){ goto _LL263;} else{ goto _LL264;} _LL264:
_LL341: _temp340= _temp238.f1; if( _temp340 == ( void*) Cyc_Absyn_Gt){ goto
_LL339;} else{ goto _LL266;} _LL339: _temp338= _temp238.f2; if( _temp338 ==  0){
goto _LL265;} else{ goto _LL266;} _LL266: _LL345: _temp344= _temp238.f1; if(
_temp344 == ( void*) Cyc_Absyn_Lt){ goto _LL343;} else{ goto _LL268;} _LL343:
_temp342= _temp238.f2; if( _temp342 ==  0){ goto _LL267;} else{ goto _LL268;}
_LL268: _LL349: _temp348= _temp238.f1; if( _temp348 == ( void*) Cyc_Absyn_Gte){
goto _LL347;} else{ goto _LL270;} _LL347: _temp346= _temp238.f2; if( _temp346 == 
0){ goto _LL269;} else{ goto _LL270;} _LL270: _LL353: _temp352= _temp238.f1; if(
_temp352 == ( void*) Cyc_Absyn_Lte){ goto _LL351;} else{ goto _LL272;} _LL351:
_temp350= _temp238.f2; if( _temp350 ==  0){ goto _LL271;} else{ goto _LL272;}
_LL272: _LL357: _temp356= _temp238.f1; if( _temp356 == ( void*) Cyc_Absyn_Gt){
goto _LL355;} else{ goto _LL274;} _LL355: _temp354= _temp238.f2; if( _temp354 == 
1){ goto _LL273;} else{ goto _LL274;} _LL274: _LL361: _temp360= _temp238.f1; if(
_temp360 == ( void*) Cyc_Absyn_Lt){ goto _LL359;} else{ goto _LL276;} _LL359:
_temp358= _temp238.f2; if( _temp358 ==  1){ goto _LL275;} else{ goto _LL276;}
_LL276: _LL365: _temp364= _temp238.f1; if( _temp364 == ( void*) Cyc_Absyn_Gte){
goto _LL363;} else{ goto _LL278;} _LL363: _temp362= _temp238.f2; if( _temp362 == 
1){ goto _LL277;} else{ goto _LL278;} _LL278: _LL369: _temp368= _temp238.f1; if(
_temp368 == ( void*) Cyc_Absyn_Lte){ goto _LL367;} else{ goto _LL280;} _LL367:
_temp366= _temp238.f2; if( _temp366 ==  1){ goto _LL279;} else{ goto _LL280;}
_LL280: _LL371: _temp370= _temp238.f1; if( _temp370 == ( void*) Cyc_Absyn_Bitand){
goto _LL281;} else{ goto _LL282;} _LL282: _LL373: _temp372= _temp238.f1; if(
_temp372 == ( void*) Cyc_Absyn_Bitor){ goto _LL283;} else{ goto _LL284;} _LL284:
_LL375: _temp374= _temp238.f1; if( _temp374 == ( void*) Cyc_Absyn_Bitxor){ goto
_LL285;} else{ goto _LL286;} _LL286: _LL377: _temp376= _temp238.f1; if( _temp376
== ( void*) Cyc_Absyn_Bitlshift){ goto _LL287;} else{ goto _LL288;} _LL288:
_LL379: _temp378= _temp238.f1; if( _temp378 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL289;} else{ goto _LL290;} _LL290: _LL381: _temp380= _temp238.f1; if(
_temp380 == ( void*) Cyc_Absyn_Bitarshift){ goto _LL291;} else{ goto _LL292;}
_LL292: goto _LL293; _LL241: i3= i1 +  i2; use_i3= 1; goto _LL239; _LL243: i3=
i1 *  i2; use_i3= 1; goto _LL239; _LL245: i3= i1 -  i2; use_i3= 1; goto _LL239;
_LL247: i3= i1 /  i2; use_i3= 1; goto _LL239; _LL249: i3= i1 %  i2; use_i3= 1;
goto _LL239; _LL251: u3= u1 +  u2; use_u3= 1; goto _LL239; _LL253: u3= u1 *  u2;
use_u3= 1; goto _LL239; _LL255: u3= u1 -  u2; use_u3= 1; goto _LL239; _LL257: u3=
u1 /  u2; use_u3= 1; goto _LL239; _LL259: u3= u1 %  u2; use_u3= 1; goto _LL239;
_LL261: b3= i1 ==  i2; use_b3= 1; goto _LL239; _LL263: b3= i1 !=  i2; use_b3= 1;
goto _LL239; _LL265: b3= i1 >  i2; use_b3= 1; goto _LL239; _LL267: b3= i1 <  i2;
use_b3= 1; goto _LL239; _LL269: b3= i1 >=  i2; use_b3= 1; goto _LL239; _LL271:
b3= i1 <=  i2; use_b3= 1; goto _LL239; _LL273: b3= u1 >  u2; use_b3= 1; goto
_LL239; _LL275: b3= u1 <  u2; use_b3= 1; goto _LL239; _LL277: b3= u1 >=  u2;
use_b3= 1; goto _LL239; _LL279: b3= u1 <=  u2; use_b3= 1; goto _LL239; _LL281:
u3= u1 &  u2; use_u3= 1; goto _LL239; _LL283: u3= u1 |  u2; use_u3= 1; goto
_LL239; _LL285: u3= u1 ^  u2; use_u3= 1; goto _LL239; _LL287: u3= u1 <<  u2;
use_u3= 1; goto _LL239; _LL289: u3= u1 >>  u2; use_u3= 1; goto _LL239; _LL291:({
void* _temp382[ 0u]={}; Cyc_Tcutil_terr( e1->loc, _tag_arr(">>> NOT IMPLEMENTED",
sizeof( unsigned char), 20u), _tag_arr( _temp382, sizeof( void*), 0u));});
return _temp204; _LL293:({ void* _temp383[ 0u]={}; Cyc_Tcutil_terr( e1->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u), _tag_arr(
_temp383, sizeof( void*), 0u));}); return _temp204; _LL239:;} if( use_i3){
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp384=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp384[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp385; _temp385.tag= Cyc_Absyn_Int_c; _temp385.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp385.f2= i3; _temp385;}); _temp384;});} if( use_u3){ return( void*)({ struct
Cyc_Absyn_Int_c_struct* _temp386=( struct Cyc_Absyn_Int_c_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_c_struct)); _temp386[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp387; _temp387.tag= Cyc_Absyn_Int_c; _temp387.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp387.f2=( int) u3; _temp387;}); _temp386;});} if( use_b3){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp388=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp388[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp389; _temp389.tag= Cyc_Absyn_Int_c; _temp389.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp389.f2= b3? 1: 0; _temp389;}); _temp388;});}( int) _throw(( void*)({ struct
Cyc_Core_Unreachable_struct* _temp390=( struct Cyc_Core_Unreachable_struct*)
GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct)); _temp390[ 0]=({ struct
Cyc_Core_Unreachable_struct _temp391; _temp391.tag= Cyc_Core_Unreachable;
_temp391.f1= _tag_arr("Evexp::eval_const_binop", sizeof( unsigned char), 24u);
_temp391;}); _temp390;}));}} void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp*
e){ void* cn;{ void* _temp392=( void*) e->r; void* _temp412; struct Cyc_Absyn_Exp*
_temp414; struct Cyc_Absyn_Exp* _temp416; struct Cyc_Absyn_Exp* _temp418; struct
Cyc_List_List* _temp420; void* _temp422; struct Cyc_Absyn_Exp* _temp424; void*
_temp426; struct Cyc_Absyn_Enumfield* _temp428; _LL394: if(*(( int*) _temp392)
==  Cyc_Absyn_Const_e){ _LL413: _temp412=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp392)->f1; goto _LL395;} else{ goto _LL396;} _LL396: if(*(( int*) _temp392)
==  Cyc_Absyn_Conditional_e){ _LL419: _temp418=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp392)->f1; goto _LL417; _LL417: _temp416=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp392)->f2; goto _LL415; _LL415: _temp414=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp392)->f3; goto _LL397;} else{ goto _LL398;} _LL398: if(*(( int*) _temp392)
==  Cyc_Absyn_Primop_e){ _LL423: _temp422=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp392)->f1; goto _LL421; _LL421: _temp420=(( struct Cyc_Absyn_Primop_e_struct*)
_temp392)->f2; goto _LL399;} else{ goto _LL400;} _LL400: if(*(( int*) _temp392)
==  Cyc_Absyn_Sizeoftyp_e){ goto _LL401;} else{ goto _LL402;} _LL402: if(*(( int*)
_temp392) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL403;} else{ goto _LL404;} _LL404:
if(*(( int*) _temp392) ==  Cyc_Absyn_Offsetof_e){ goto _LL405;} else{ goto
_LL406;} _LL406: if(*(( int*) _temp392) ==  Cyc_Absyn_Cast_e){ _LL427: _temp426=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp392)->f1; goto _LL425; _LL425:
_temp424=(( struct Cyc_Absyn_Cast_e_struct*) _temp392)->f2; goto _LL407;} else{
goto _LL408;} _LL408: if(*(( int*) _temp392) ==  Cyc_Absyn_Enum_e){ _LL429:
_temp428=(( struct Cyc_Absyn_Enum_e_struct*) _temp392)->f3; goto _LL409;} else{
goto _LL410;} _LL410: goto _LL411; _LL395: return _temp412; _LL397: cn= Cyc_Evexp_eval_const_bool_exp(
_temp418)? Cyc_Evexp_eval_const_exp( _temp416): Cyc_Evexp_eval_const_exp(
_temp414); goto _LL393; _LL399: if( _temp420 ==  0){({ void* _temp430[ 0u]={};
Cyc_Tcutil_terr( e->loc, _tag_arr("bad static expression (no args to primop)",
sizeof( unsigned char), 42u), _tag_arr( _temp430, sizeof( void*), 0u));});
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp431=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp431[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp432; _temp432.tag= Cyc_Absyn_Int_c; _temp432.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp432.f2= 0; _temp432;}); _temp431;});} if((( struct Cyc_List_List*)
_check_null( _temp420))->tl ==  0){ cn= Cyc_Evexp_eval_const_unprimop( _temp422,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp420))->hd);}
else{ if((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp420))->tl))->tl !=  0){({ void* _temp433[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("bad static expression (too many args to primop)", sizeof(
unsigned char), 48u), _tag_arr( _temp433, sizeof( void*), 0u));}); return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp434=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp434[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp435; _temp435.tag= Cyc_Absyn_Int_c; _temp435.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp435.f2= 0; _temp435;}); _temp434;});} else{ cn= Cyc_Evexp_eval_const_binprimop(
_temp422,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp420))->hd,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp420))->tl))->hd);}} goto _LL393; _LL401: goto _LL403; _LL403:({
void* _temp436[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("sizeof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 62u), _tag_arr( _temp436, sizeof( void*), 0u));});
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp437=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp437[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp438; _temp438.tag= Cyc_Absyn_Int_c; _temp438.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp438.f2= 0; _temp438;}); _temp437;}); _LL405:({ void* _temp439[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("offsetof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 64u), _tag_arr( _temp439, sizeof( void*), 0u));});
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp440=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp440[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp441; _temp441.tag= Cyc_Absyn_Int_c; _temp441.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp441.f2= 0; _temp441;}); _temp440;}); _LL407: cn= Cyc_Evexp_eval_const_exp(
_temp424);{ struct _tuple4 _temp443=({ struct _tuple4 _temp442; _temp442.f1= Cyc_Tcutil_compress(
_temp426); _temp442.f2= cn; _temp442;}); void* _temp453; unsigned char _temp455;
void* _temp457; void* _temp459; void* _temp461; void* _temp463; void* _temp465;
short _temp467; void* _temp469; void* _temp471; void* _temp473; void* _temp475;
void* _temp477; int _temp479; void* _temp481; void* _temp483; void* _temp485;
void* _temp487; _LL445: _LL460: _temp459= _temp443.f1; if(( unsigned int)
_temp459 >  4u?*(( int*) _temp459) ==  Cyc_Absyn_IntType: 0){ _LL464: _temp463=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp459)->f1; goto _LL462; _LL462:
_temp461=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp459)->f2; goto _LL454;}
else{ goto _LL447;} _LL454: _temp453= _temp443.f2; if(( unsigned int) _temp453 > 
1u?*(( int*) _temp453) ==  Cyc_Absyn_Char_c: 0){ _LL458: _temp457=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp453)->f1; goto _LL456; _LL456: _temp455=((
struct Cyc_Absyn_Char_c_struct*) _temp453)->f2; goto _LL446;} else{ goto _LL447;}
_LL447: _LL472: _temp471= _temp443.f1; if(( unsigned int) _temp471 >  4u?*(( int*)
_temp471) ==  Cyc_Absyn_IntType: 0){ _LL476: _temp475=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp471)->f1; goto _LL474; _LL474: _temp473=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp471)->f2; goto _LL466;} else{ goto _LL449;} _LL466: _temp465= _temp443.f2;
if(( unsigned int) _temp465 >  1u?*(( int*) _temp465) ==  Cyc_Absyn_Short_c: 0){
_LL470: _temp469=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp465)->f1;
goto _LL468; _LL468: _temp467=(( struct Cyc_Absyn_Short_c_struct*) _temp465)->f2;
goto _LL448;} else{ goto _LL449;} _LL449: _LL484: _temp483= _temp443.f1; if((
unsigned int) _temp483 >  4u?*(( int*) _temp483) ==  Cyc_Absyn_IntType: 0){
_LL488: _temp487=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp483)->f1;
goto _LL486; _LL486: _temp485=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp483)->f2; goto _LL478;} else{ goto _LL451;} _LL478: _temp477= _temp443.f2;
if(( unsigned int) _temp477 >  1u?*(( int*) _temp477) ==  Cyc_Absyn_Int_c: 0){
_LL482: _temp481=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp477)->f1; goto
_LL480; _LL480: _temp479=(( struct Cyc_Absyn_Int_c_struct*) _temp477)->f2; goto
_LL450;} else{ goto _LL451;} _LL451: goto _LL452; _LL446: _temp475= _temp463;
_temp473= _temp461; _temp469= _temp457; _temp467=( short) _temp455; goto _LL448;
_LL448: _temp487= _temp475; _temp485= _temp473; _temp481= _temp469; _temp479=(
int) _temp467; goto _LL450; _LL450: if( _temp487 !=  _temp481){ cn=( void*)({
struct Cyc_Absyn_Int_c_struct* _temp489=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp489[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp490; _temp490.tag= Cyc_Absyn_Int_c; _temp490.f1=( void*) _temp487; _temp490.f2=
_temp479; _temp490;}); _temp489;});} goto _LL444; _LL452:({ struct Cyc_std_String_pa_struct
_temp492; _temp492.tag= Cyc_std_String_pa; _temp492.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp426);{ void* _temp491[ 1u]={& _temp492}; Cyc_Tcutil_terr( e->loc, _tag_arr("eval_const: cannot cast to %s",
sizeof( unsigned char), 30u), _tag_arr( _temp491, sizeof( void*), 1u));}});
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp493=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp493[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp494; _temp494.tag= Cyc_Absyn_Int_c; _temp494.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp494.f2= 0; _temp494;}); _temp493;}); _LL444:;} goto _LL393; _LL409: return
Cyc_Evexp_eval_const_exp(( struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)
_check_null( _temp428))->tag)); _LL411:({ void* _temp495[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("bad static expression", sizeof( unsigned char), 22u), _tag_arr(
_temp495, sizeof( void*), 0u));}); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp496=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp496[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp497; _temp497.tag= Cyc_Absyn_Int_c;
_temp497.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp497.f2= 0; _temp497;});
_temp496;}); _LL393:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp498=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp498[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp499; _temp499.tag= Cyc_Absyn_Const_e;
_temp499.f1=( void*) cn; _temp499;}); _temp498;}))); return cn;}
