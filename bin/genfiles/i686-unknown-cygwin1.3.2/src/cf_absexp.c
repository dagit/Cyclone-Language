 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern struct _tagged_arr Cyc_Core_new_string( int); extern int Cyc_Core_intcmp(
int, int); extern int Cyc_Core_ptrcmp( void**, void**); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); struct Cyc_std___sFILE;
extern unsigned char Cyc_std_FileCloseError[ 19u]; extern unsigned char Cyc_std_FileOpenError[
18u]; struct Cyc_std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_std_String_pa= 0; struct Cyc_std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_std_Int_pa= 1; struct
Cyc_std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_std_Double_pa=
2; struct Cyc_std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa=
3; struct Cyc_std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern struct _tagged_arr Cyc_std_aprintf( struct _tagged_arr fmt, struct
_tagged_arr); static const int Cyc_std_ShortPtr_sa= 0; struct Cyc_std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_std_UShortPtr_sa= 1; struct Cyc_std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_std_IntPtr_sa= 2; struct
Cyc_std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_std_UIntPtr_sa=
3; struct Cyc_std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_std_StringPtr_sa= 4; struct Cyc_std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_memq( struct Cyc_List_List*
l, void* x); extern int Cyc_std_zstrptrcmp( struct _tagged_arr*, struct
_tagged_arr*); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u];
extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict*
d, void* key, void** ans_place); extern void* Cyc_Dict_fold2_c( void*(* f)( void*,
void*, void*, void*, void*), void* inner_env, struct Cyc_Dict_Dict* d1, struct
Cyc_Dict_Dict* d2, void* accum); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const
int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
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
15u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern int Cyc_Tcutil_bits_only( void* t); struct
Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_arr* f1; } ; static const int Cyc_CfFlowInfo_TupleF= 1;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_Esc=
0; static const int Cyc_CfFlowInfo_Unesc= 1; static const int Cyc_CfFlowInfo_NoneIL=
0; static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_UnknownIS= 0; struct Cyc_CfFlowInfo_UnknownIS_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfFlowInfo_MustPointTo= 1;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI=
2; struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void* il); extern void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo); extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict(
void*, void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int
Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); extern
unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]; static const int Cyc_CfAbsexp_OrderedG=
0; static const int Cyc_CfAbsexp_UnorderedG= 1; static const int Cyc_CfAbsexp_OneofG=
2; extern void Cyc_CfAbsexp_check_absexp( void*); extern struct _tagged_arr Cyc_CfAbsexp_absexp2string(
void* ae, int depth); extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkUseAE( void*
ao); extern void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]="\000\000\000\000BadAbsexp";
void* Cyc_CfAbsexp_mkAnyOrderG(){ return( void*) Cyc_CfAbsexp_OrderedG;} static
const int Cyc_CfAbsexp_BottomAE= 0; static const int Cyc_CfAbsexp_SkipAE= 1;
static const int Cyc_CfAbsexp_UseAE= 0; struct Cyc_CfAbsexp_UseAE_struct{ int
tag; void* f1; } ; static const int Cyc_CfAbsexp_AssignAE= 1; struct Cyc_CfAbsexp_AssignAE_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_MallocAE= 2;
struct Cyc_CfAbsexp_MallocAE_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_CfAbsexp_StmtAE= 3; struct Cyc_CfAbsexp_StmtAE_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; } ; static const int Cyc_CfAbsexp_GroupAE= 4; struct Cyc_CfAbsexp_GroupAE_struct{
int tag; void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_CfAbsexp_UnknownOp=
0; static const int Cyc_CfAbsexp_AddressOp= 0; struct Cyc_CfAbsexp_AddressOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_DerefOp= 1; struct Cyc_CfAbsexp_DerefOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_MemberOp= 2; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_LocalOp= 3;
struct Cyc_CfAbsexp_LocalOp_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfAbsexp_MallocOp= 4; struct Cyc_CfAbsexp_MallocOp_struct{
int tag; struct Cyc_Absyn_Exp* f1; int f2; } ; static void Cyc_CfAbsexp_ok_address_arg(
void* ao){ void* _temp0= ao; _LL2: if(( unsigned int) _temp0 >  1u?*(( int*)
_temp0) ==  Cyc_CfAbsexp_LocalOp: 0){ goto _LL3;} else{ goto _LL4;} _LL4: if((
unsigned int) _temp0 >  1u?*(( int*) _temp0) ==  Cyc_CfAbsexp_MemberOp: 0){ goto
_LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 >  1u?*(( int*) _temp0)
==  Cyc_CfAbsexp_MallocOp: 0){ goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9;
_LL3: return; _LL5: return; _LL7: return; _LL9:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL1:;} static void Cyc_CfAbsexp_ok_member_arg( void* ao){ void* _temp10= ao;
_LL12: if(( unsigned int) _temp10 >  1u?*(( int*) _temp10) ==  Cyc_CfAbsexp_LocalOp:
0){ goto _LL13;} else{ goto _LL14;} _LL14: if(( unsigned int) _temp10 >  1u?*((
int*) _temp10) ==  Cyc_CfAbsexp_MemberOp: 0){ goto _LL15;} else{ goto _LL16;}
_LL16: if(( unsigned int) _temp10 >  1u?*(( int*) _temp10) ==  Cyc_CfAbsexp_MallocOp:
0){ goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp10 >  1u?*((
int*) _temp10) ==  Cyc_CfAbsexp_DerefOp: 0){ goto _LL19;} else{ goto _LL20;}
_LL20: goto _LL21; _LL13: return; _LL15: return; _LL17: return; _LL19: return;
_LL21:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL11:;} static void Cyc_CfAbsexp_ok_deref_arg(
void* ao){ void* _temp22= ao; _LL24: if(( unsigned int) _temp22 >  1u?*(( int*)
_temp22) ==  Cyc_CfAbsexp_LocalOp: 0){ goto _LL25;} else{ goto _LL26;} _LL26:
if(( unsigned int) _temp22 >  1u?*(( int*) _temp22) ==  Cyc_CfAbsexp_MemberOp: 0){
goto _LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp22 >  1u?*(( int*)
_temp22) ==  Cyc_CfAbsexp_DerefOp: 0){ goto _LL29;} else{ goto _LL30;} _LL30:
goto _LL31; _LL25: return; _LL27: return; _LL29: return; _LL31:( int) _throw((
void*) Cyc_CfAbsexp_BadAbsexp); _LL23:;} static void Cyc_CfAbsexp_ok_lvalue(
void* ao){ void* _temp32= ao; _LL34: if( _temp32 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL35;} else{ goto _LL36;} _LL36: if(( unsigned int) _temp32 >  1u?*(( int*)
_temp32) ==  Cyc_CfAbsexp_AddressOp: 0){ goto _LL37;} else{ goto _LL38;} _LL38:
goto _LL39; _LL35: goto _LL37; _LL37:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL39: return; _LL33:;} static int Cyc_CfAbsexp_num_mallocpts= 0; static struct
Cyc_Dict_Dict** Cyc_CfAbsexp_mallocpt_dict= 0; int Cyc_CfAbsexp_mallocpt_int(
struct Cyc_Absyn_Exp* e){ if( Cyc_CfAbsexp_mallocpt_dict ==  0){ struct Cyc_Dict_Dict*
d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)))
Cyc_Dict_empty)(( int(*)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp);
Cyc_CfAbsexp_mallocpt_dict=({ struct Cyc_Dict_Dict** _temp40=( struct Cyc_Dict_Dict**)
GC_malloc( sizeof( struct Cyc_Dict_Dict*) *  1); _temp40[ 0]= d; _temp40;});}{
int i= 0; if( !(( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key,
int* ans_place)) Cyc_Dict_lookup_bool)(*(( struct Cyc_Dict_Dict**) _check_null(
Cyc_CfAbsexp_mallocpt_dict)), e,& i)){*(( struct Cyc_Dict_Dict**) _check_null(
Cyc_CfAbsexp_mallocpt_dict))=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_Absyn_Exp* key, int data)) Cyc_Dict_insert)(*(( struct Cyc_Dict_Dict**)
_check_null( Cyc_CfAbsexp_mallocpt_dict)), e,( i= ++ Cyc_CfAbsexp_num_mallocpts));}
return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){ return( void*) Cyc_CfAbsexp_UnknownOp;}
void* Cyc_CfAbsexp_mkAddressOp( void* ao){ Cyc_CfAbsexp_ok_address_arg( ao);
return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct* _temp41=( struct Cyc_CfAbsexp_AddressOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp41[ 0]=({ struct
Cyc_CfAbsexp_AddressOp_struct _temp42; _temp42.tag= Cyc_CfAbsexp_AddressOp;
_temp42.f1=( void*) ao; _temp42;}); _temp41;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp43=( struct Cyc_CfAbsexp_LocalOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp43[ 0]=({ struct Cyc_CfAbsexp_LocalOp_struct _temp44; _temp44.tag= Cyc_CfAbsexp_LocalOp;
_temp44.f1= vd; _temp44;}); _temp43;});} void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f){ Cyc_CfAbsexp_ok_member_arg( ao); return( void*)({ struct Cyc_CfAbsexp_MemberOp_struct*
_temp45=( struct Cyc_CfAbsexp_MemberOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct));
_temp45[ 0]=({ struct Cyc_CfAbsexp_MemberOp_struct _temp46; _temp46.tag= Cyc_CfAbsexp_MemberOp;
_temp46.f1=( void*) ao; _temp46.f2=( void*) f; _temp46;}); _temp45;});} void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct*
_temp47=( struct Cyc_CfAbsexp_MallocOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp47[ 0]=({ struct Cyc_CfAbsexp_MallocOp_struct _temp48; _temp48.tag= Cyc_CfAbsexp_MallocOp;
_temp48.f1= e; _temp48.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp48;}); _temp47;});}
void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg( ao); return(
void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp49=( struct Cyc_CfAbsexp_DerefOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp49[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp50; _temp50.tag= Cyc_CfAbsexp_DerefOp; _temp50.f1=(
void*) ao; _temp50;}); _temp49;});} void* Cyc_CfAbsexp_mkBottomAE(){ return(
void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return( void*) Cyc_CfAbsexp_SkipAE;}
void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e){ return( void*)({ struct
Cyc_CfAbsexp_MallocAE_struct* _temp51=( struct Cyc_CfAbsexp_MallocAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp51[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp52; _temp52.tag= Cyc_CfAbsexp_MallocAE;
_temp52.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp53=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp53[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp54; _temp54.tag= Cyc_CfFlowInfo_MallocPt;
_temp54.f1= e; _temp54;}); _temp53;})); _temp52.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp52;}); _temp51;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp55=( struct Cyc_CfAbsexp_StmtAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp55[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp56; _temp56.tag= Cyc_CfAbsexp_StmtAE; _temp56.f1=
s; _temp56;}); _temp55;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){ void* _temp57=
ao; _LL59: if( _temp57 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL60;} else{
goto _LL61;} _LL61: goto _LL62; _LL60: return( void*) Cyc_CfAbsexp_SkipAE; _LL62:
return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp63=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp63[ 0]=({ struct Cyc_CfAbsexp_UseAE_struct
_temp64; _temp64.tag= Cyc_CfAbsexp_UseAE; _temp64.f1=( void*) ao; _temp64;});
_temp63;}); _LL58:;} struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp66=({ struct _tuple3 _temp65; _temp65.f1=
l; _temp65.f2= r; _temp65;}); void* _temp74; void* _temp76; void* _temp78; _LL68:
_LL77: _temp76= _temp66.f1; if( _temp76 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL75;} else{ goto _LL70;} _LL75: _temp74= _temp66.f2; if( _temp74 == (
void*) Cyc_CfAbsexp_UnknownOp){ goto _LL69;} else{ goto _LL70;} _LL70: _LL79:
_temp78= _temp66.f1; if( _temp78 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL71;}
else{ goto _LL72;} _LL72: goto _LL73; _LL69: return( void*) Cyc_CfAbsexp_SkipAE;
_LL71: return Cyc_CfAbsexp_mkUseAE( r); _LL73: Cyc_CfAbsexp_ok_lvalue( l);
return( void*)({ struct Cyc_CfAbsexp_AssignAE_struct* _temp80=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp80[ 0]=({ struct
Cyc_CfAbsexp_AssignAE_struct _temp81; _temp81.tag= Cyc_CfAbsexp_AssignAE;
_temp81.f1=( void*) l; _temp81.f2=( void*) r; _temp81;}); _temp80;}); _LL67:;}
struct _tuple4{ void* f1; void* f2; void* f3; } ; void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2){ struct _tuple4 _temp83=({ struct _tuple4 _temp82;
_temp82.f1= g; _temp82.f2= ae1; _temp82.f3= ae2; _temp82;}); void* _temp106;
void* _temp109; void* _temp111; void* _temp113; void* _temp115; void* _temp117;
void* _temp119; void* _temp121; void* _temp124; struct Cyc_List_List* _temp126;
void* _temp128; void* _temp130; struct Cyc_List_List* _temp132; void* _temp134;
void* _temp136; void* _temp139; struct Cyc_List_List* _temp141; void* _temp143;
void* _temp145; void* _temp147; struct Cyc_List_List* _temp149; void* _temp151;
void* _temp153; void* _temp156; struct Cyc_List_List* _temp158; void* _temp160;
void* _temp162; _LL85: _LL107: _temp106= _temp83.f3; if( _temp106 == ( void*)
Cyc_CfAbsexp_SkipAE){ goto _LL105;} else{ goto _LL87;} _LL105: if( g != ( void*)
Cyc_CfAbsexp_OneofG){ goto _LL86;} else{ goto _LL87;} _LL87: _LL110: _temp109=
_temp83.f2; if( _temp109 == ( void*) Cyc_CfAbsexp_SkipAE){ goto _LL108;} else{
goto _LL89;} _LL108: if( g != ( void*) Cyc_CfAbsexp_OneofG){ goto _LL88;} else{
goto _LL89;} _LL89: _LL114: _temp113= _temp83.f1; if( _temp113 == ( void*) Cyc_CfAbsexp_OneofG){
goto _LL112;} else{ goto _LL91;} _LL112: _temp111= _temp83.f3; if( _temp111 == (
void*) Cyc_CfAbsexp_BottomAE){ goto _LL90;} else{ goto _LL91;} _LL91: _LL118:
_temp117= _temp83.f1; if( _temp117 == ( void*) Cyc_CfAbsexp_OneofG){ goto _LL116;}
else{ goto _LL93;} _LL116: _temp115= _temp83.f2; if( _temp115 == ( void*) Cyc_CfAbsexp_BottomAE){
goto _LL92;} else{ goto _LL93;} _LL93: _LL122: _temp121= _temp83.f1; if(
_temp121 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL120;} else{ goto _LL95;}
_LL120: _temp119= _temp83.f2; if( _temp119 == ( void*) Cyc_CfAbsexp_BottomAE){
goto _LL94;} else{ goto _LL95;} _LL95: _LL137: _temp136= _temp83.f1; goto _LL131;
_LL131: _temp130= _temp83.f2; if(( unsigned int) _temp130 >  2u?*(( int*)
_temp130) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL135: _temp134=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp130)->f1; goto _LL133; _LL133: _temp132=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp130)->f2; goto _LL125;} else{ goto _LL97;} _LL125: _temp124= _temp83.f3;
if(( unsigned int) _temp124 >  2u?*(( int*) _temp124) ==  Cyc_CfAbsexp_GroupAE:
0){ _LL129: _temp128=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp124)->f1;
goto _LL127; _LL127: _temp126=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp124)->f2;
goto _LL123;} else{ goto _LL97;} _LL123: if( _temp136 ==  _temp134? _temp136 == 
_temp128: 0){ goto _LL96;} else{ goto _LL97;} _LL97: _LL146: _temp145= _temp83.f1;
goto _LL140; _LL140: _temp139= _temp83.f2; if(( unsigned int) _temp139 >  2u?*((
int*) _temp139) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL144: _temp143=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp139)->f1; goto _LL142; _LL142: _temp141=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp139)->f2; goto _LL138;} else{ goto
_LL99;} _LL138: if( _temp145 ==  _temp143? _temp145 != ( void*) Cyc_CfAbsexp_OrderedG:
0){ goto _LL98;} else{ goto _LL99;} _LL99: _LL154: _temp153= _temp83.f1; if(
_temp153 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL148;} else{ goto _LL101;}
_LL148: _temp147= _temp83.f2; if(( unsigned int) _temp147 >  2u?*(( int*)
_temp147) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL152: _temp151=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp147)->f1; if( _temp151 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL150;}
else{ goto _LL101;} _LL150: _temp149=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp147)->f2; goto _LL100;} else{ goto _LL101;} _LL101: _LL163: _temp162=
_temp83.f1; goto _LL157; _LL157: _temp156= _temp83.f3; if(( unsigned int)
_temp156 >  2u?*(( int*) _temp156) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL161:
_temp160=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp156)->f1; goto
_LL159; _LL159: _temp158=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp156)->f2;
goto _LL155;} else{ goto _LL103;} _LL155: if( _temp162 ==  _temp160){ goto
_LL102;} else{ goto _LL103;} _LL103: goto _LL104; _LL86: return ae1; _LL88:
return ae2; _LL90: return ae1; _LL92: return ae2; _LL94: return( void*) Cyc_CfAbsexp_BottomAE;
_LL96: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp164=( struct
Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp164[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp165; _temp165.tag= Cyc_CfAbsexp_GroupAE;
_temp165.f1=( void*) _temp136; _temp165.f2= Cyc_List_append( _temp132, _temp126);
_temp165;}); _temp164;}); _LL98: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp166=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp166[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp167; _temp167.tag= Cyc_CfAbsexp_GroupAE;
_temp167.f1=( void*) _temp145; _temp167.f2=({ struct Cyc_List_List* _temp168=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) *  1); _temp168[
0]=({ struct Cyc_List_List _temp169; _temp169.hd=( void*) ae2; _temp169.tl=
_temp141; _temp169;}); _temp168;}); _temp167;}); _temp166;}); _LL100: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp170=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp170[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp171; _temp171.tag= Cyc_CfAbsexp_GroupAE;
_temp171.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp171.f2= Cyc_List_append(
_temp149,({ struct Cyc_List_List* _temp172=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) *  1); _temp172[ 0]=({ struct Cyc_List_List
_temp173; _temp173.hd=( void*) ae2; _temp173.tl= 0; _temp173;}); _temp172;}));
_temp171;}); _temp170;}); _LL102: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp174=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp174[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp175; _temp175.tag= Cyc_CfAbsexp_GroupAE;
_temp175.f1=( void*) _temp162; _temp175.f2=({ struct Cyc_List_List* _temp176=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) *  1); _temp176[
0]=({ struct Cyc_List_List _temp177; _temp177.hd=( void*) ae1; _temp177.tl=
_temp158; _temp177;}); _temp176;}); _temp175;}); _temp174;}); _LL104: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp178=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp178[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp179; _temp179.tag= Cyc_CfAbsexp_GroupAE;
_temp179.f1=( void*) g; _temp179.f2=({ struct Cyc_List_List* _temp180=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) *  1); _temp180[ 0]=({
struct Cyc_List_List _temp181; _temp181.hd=( void*) ae1; _temp181.tl=({ struct
Cyc_List_List* _temp182=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
*  1); _temp182[ 0]=({ struct Cyc_List_List _temp183; _temp183.hd=( void*) ae2;
_temp183.tl= 0; _temp183;}); _temp182;}); _temp181;}); _temp180;}); _temp179;});
_temp178;}); _LL84:;} void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List*
ael){ if( ael ==  0){ return( void*) Cyc_CfAbsexp_SkipAE;} if((( struct Cyc_List_List*)
_check_null( ael))->tl ==  0){ return( void*)(( struct Cyc_List_List*)
_check_null( ael))->hd;} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp184=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp184[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp185; _temp185.tag= Cyc_CfAbsexp_GroupAE;
_temp185.f1=( void*) g; _temp185.f2= ael; _temp185;}); _temp184;});} int Cyc_CfAbsexp_isUnknownOp(
void* ao){ return ao == ( void*) Cyc_CfAbsexp_UnknownOp;} struct _tagged_arr Cyc_CfAbsexp_absop2string(
void* ao){ void* _temp186= ao; void* _temp200; struct Cyc_Absyn_Vardecl*
_temp202; void* _temp204; void* _temp206; int _temp208; struct Cyc_Absyn_Exp*
_temp210; void* _temp212; _LL188: if( _temp186 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL189;} else{ goto _LL190;} _LL190: if(( unsigned int) _temp186 >  1u?*((
int*) _temp186) ==  Cyc_CfAbsexp_AddressOp: 0){ _LL201: _temp200=( void*)((
struct Cyc_CfAbsexp_AddressOp_struct*) _temp186)->f1; goto _LL191;} else{ goto
_LL192;} _LL192: if(( unsigned int) _temp186 >  1u?*(( int*) _temp186) ==  Cyc_CfAbsexp_LocalOp:
0){ _LL203: _temp202=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp186)->f1; goto
_LL193;} else{ goto _LL194;} _LL194: if(( unsigned int) _temp186 >  1u?*(( int*)
_temp186) ==  Cyc_CfAbsexp_MemberOp: 0){ _LL207: _temp206=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp186)->f1; goto _LL205; _LL205: _temp204=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp186)->f2; goto _LL195;} else{ goto _LL196;} _LL196: if(( unsigned int)
_temp186 >  1u?*(( int*) _temp186) ==  Cyc_CfAbsexp_MallocOp: 0){ _LL211:
_temp210=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp186)->f1; goto _LL209;
_LL209: _temp208=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp186)->f2; goto
_LL197;} else{ goto _LL198;} _LL198: if(( unsigned int) _temp186 >  1u?*(( int*)
_temp186) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL213: _temp212=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp186)->f1; goto _LL199;} else{ goto _LL187;} _LL189: return _tag_arr("Unknown",
sizeof( unsigned char), 8u); _LL191: return( struct _tagged_arr)({ struct Cyc_std_String_pa_struct
_temp215; _temp215.tag= Cyc_std_String_pa; _temp215.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp200);{ void* _temp214[ 1u]={& _temp215}; Cyc_std_aprintf( _tag_arr("& %s",
sizeof( unsigned char), 5u), _tag_arr( _temp214, sizeof( void*), 1u));}});
_LL193: return*(* _temp202->name).f2; _LL195: { struct _tagged_arr fs;{ void*
_temp216= _temp204; struct _tagged_arr* _temp222; int _temp224; _LL218: if(*((
int*) _temp216) ==  Cyc_CfFlowInfo_StructF){ _LL223: _temp222=(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp216)->f1; goto _LL219;} else{ goto _LL220;} _LL220: if(*(( int*) _temp216)
==  Cyc_CfFlowInfo_TupleF){ _LL225: _temp224=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp216)->f1; goto _LL221;} else{ goto _LL217;} _LL219: fs=* _temp222; goto
_LL217; _LL221: fs=( struct _tagged_arr)({ struct Cyc_std_Int_pa_struct _temp227;
_temp227.tag= Cyc_std_Int_pa; _temp227.f1=( int)(( unsigned int) _temp224);{
void* _temp226[ 1u]={& _temp227}; Cyc_std_aprintf( _tag_arr("%d", sizeof(
unsigned char), 3u), _tag_arr( _temp226, sizeof( void*), 1u));}}); goto _LL217;
_LL217:;} return( struct _tagged_arr)({ struct Cyc_std_String_pa_struct _temp230;
_temp230.tag= Cyc_std_String_pa; _temp230.f1=( struct _tagged_arr) fs;{ struct
Cyc_std_String_pa_struct _temp229; _temp229.tag= Cyc_std_String_pa; _temp229.f1=(
struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp206);{ void* _temp228[ 2u]={&
_temp229,& _temp230}; Cyc_std_aprintf( _tag_arr("%s.%s", sizeof( unsigned char),
6u), _tag_arr( _temp228, sizeof( void*), 2u));}}});} _LL197: return( struct
_tagged_arr)({ struct Cyc_std_Int_pa_struct _temp232; _temp232.tag= Cyc_std_Int_pa;
_temp232.f1=( int)(( unsigned int) _temp208);{ void* _temp231[ 1u]={& _temp232};
Cyc_std_aprintf( _tag_arr("<mpt%d>", sizeof( unsigned char), 8u), _tag_arr(
_temp231, sizeof( void*), 1u));}}); _LL199: return( struct _tagged_arr)({ struct
Cyc_std_String_pa_struct _temp234; _temp234.tag= Cyc_std_String_pa; _temp234.f1=(
struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp212);{ void* _temp233[ 1u]={&
_temp234}; Cyc_std_aprintf( _tag_arr("(*%s)", sizeof( unsigned char), 6u),
_tag_arr( _temp233, sizeof( void*), 1u));}}); _LL187:;} struct _tagged_arr Cyc_CfAbsexp_absexp2string(
void* ae, int depth){ struct _tagged_arr ans= Cyc_Core_new_string( depth);{ int
i= 0; for( 0; i <  _get_arr_size( ans, sizeof( unsigned char)); ++ i){*((
unsigned char*) _check_unknown_subscript( ans, sizeof( unsigned char), i))=' ';}}{
void* _temp235= ae; void* _temp253; int _temp255; void* _temp257; struct Cyc_Absyn_Exp*
_temp259; void* _temp261; void* _temp263; struct Cyc_List_List* _temp265; void*
_temp267; struct Cyc_Absyn_Stmt* _temp269; _LL237: if( _temp235 == ( void*) Cyc_CfAbsexp_SkipAE){
goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int) _temp235 >  2u?*((
int*) _temp235) ==  Cyc_CfAbsexp_UseAE: 0){ _LL254: _temp253=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp235)->f1; goto _LL240;} else{ goto _LL241;}
_LL241: if(( unsigned int) _temp235 >  2u?*(( int*) _temp235) ==  Cyc_CfAbsexp_MallocAE:
0){ _LL258: _temp257=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp235)->f1;
if(*(( int*) _temp257) ==  Cyc_CfFlowInfo_MallocPt){ _LL260: _temp259=(( struct
Cyc_CfFlowInfo_MallocPt_struct*) _temp257)->f1; goto _LL256;} else{ goto _LL243;}
_LL256: _temp255=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp235)->f2; goto
_LL242;} else{ goto _LL243;} _LL243: if(( unsigned int) _temp235 >  2u?*(( int*)
_temp235) ==  Cyc_CfAbsexp_AssignAE: 0){ _LL264: _temp263=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp235)->f1; goto _LL262; _LL262: _temp261=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp235)->f2; goto _LL244;} else{ goto _LL245;} _LL245: if(( unsigned int)
_temp235 >  2u?*(( int*) _temp235) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL268:
_temp267=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp235)->f1; goto
_LL266; _LL266: _temp265=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp235)->f2;
goto _LL246;} else{ goto _LL247;} _LL247: if(( unsigned int) _temp235 >  2u?*((
int*) _temp235) ==  Cyc_CfAbsexp_StmtAE: 0){ _LL270: _temp269=(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp235)->f1; goto _LL248;} else{ goto _LL249;} _LL249: if( _temp235 == ( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL250;} else{ goto _LL251;} _LL251: goto _LL252;
_LL238: ans=({ struct Cyc_std_String_pa_struct _temp272; _temp272.tag= Cyc_std_String_pa;
_temp272.f1=( struct _tagged_arr) ans;{ void* _temp271[ 1u]={& _temp272}; Cyc_std_aprintf(
_tag_arr("%sskip", sizeof( unsigned char), 7u), _tag_arr( _temp271, sizeof( void*),
1u));}}); goto _LL236; _LL240: ans=({ struct Cyc_std_String_pa_struct _temp275;
_temp275.tag= Cyc_std_String_pa; _temp275.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp253);{ struct Cyc_std_String_pa_struct _temp274; _temp274.tag= Cyc_std_String_pa;
_temp274.f1=( struct _tagged_arr) ans;{ void* _temp273[ 2u]={& _temp274,&
_temp275}; Cyc_std_aprintf( _tag_arr("%suse %s", sizeof( unsigned char), 9u),
_tag_arr( _temp273, sizeof( void*), 2u));}}}); goto _LL236; _LL242: ans=({
struct Cyc_std_Int_pa_struct _temp278; _temp278.tag= Cyc_std_Int_pa; _temp278.f1=(
int)(( unsigned int) _temp255);{ struct Cyc_std_String_pa_struct _temp277;
_temp277.tag= Cyc_std_String_pa; _temp277.f1=( struct _tagged_arr) ans;{ void*
_temp276[ 2u]={& _temp277,& _temp278}; Cyc_std_aprintf( _tag_arr("%smalloc %i",
sizeof( unsigned char), 12u), _tag_arr( _temp276, sizeof( void*), 2u));}}});
goto _LL236; _LL244: ans=({ struct Cyc_std_String_pa_struct _temp282; _temp282.tag=
Cyc_std_String_pa; _temp282.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp261);{ struct Cyc_std_String_pa_struct _temp281; _temp281.tag= Cyc_std_String_pa;
_temp281.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp263);{ struct
Cyc_std_String_pa_struct _temp280; _temp280.tag= Cyc_std_String_pa; _temp280.f1=(
struct _tagged_arr) ans;{ void* _temp279[ 3u]={& _temp280,& _temp281,& _temp282};
Cyc_std_aprintf( _tag_arr("%sassign %s %s", sizeof( unsigned char), 15u),
_tag_arr( _temp279, sizeof( void*), 3u));}}}}); goto _LL236; _LL246:{ void*
_temp283= _temp267; _LL285: if( _temp283 == ( void*) Cyc_CfAbsexp_OrderedG){
goto _LL286;} else{ goto _LL287;} _LL287: if( _temp283 == ( void*) Cyc_CfAbsexp_UnorderedG){
goto _LL288;} else{ goto _LL289;} _LL289: if( _temp283 == ( void*) Cyc_CfAbsexp_OneofG){
goto _LL290;} else{ goto _LL284;} _LL286: ans=({ struct Cyc_std_String_pa_struct
_temp292; _temp292.tag= Cyc_std_String_pa; _temp292.f1=( struct _tagged_arr) ans;{
void* _temp291[ 1u]={& _temp292}; Cyc_std_aprintf( _tag_arr("%sordered", sizeof(
unsigned char), 10u), _tag_arr( _temp291, sizeof( void*), 1u));}}); goto _LL284;
_LL288: ans=({ struct Cyc_std_String_pa_struct _temp294; _temp294.tag= Cyc_std_String_pa;
_temp294.f1=( struct _tagged_arr) ans;{ void* _temp293[ 1u]={& _temp294}; Cyc_std_aprintf(
_tag_arr("%sunordered", sizeof( unsigned char), 12u), _tag_arr( _temp293,
sizeof( void*), 1u));}}); goto _LL284; _LL290: ans=({ struct Cyc_std_String_pa_struct
_temp296; _temp296.tag= Cyc_std_String_pa; _temp296.f1=( struct _tagged_arr) ans;{
void* _temp295[ 1u]={& _temp296}; Cyc_std_aprintf( _tag_arr("%soneof", sizeof(
unsigned char), 8u), _tag_arr( _temp295, sizeof( void*), 1u));}}); goto _LL284;
_LL284:;} for( 0; _temp265 !=  0; _temp265=(( struct Cyc_List_List*) _check_null(
_temp265))->tl){ ans=({ struct Cyc_std_String_pa_struct _temp299; _temp299.tag=
Cyc_std_String_pa; _temp299.f1=( struct _tagged_arr) Cyc_CfAbsexp_absexp2string((
void*)(( struct Cyc_List_List*) _check_null( _temp265))->hd, depth +  1);{
struct Cyc_std_String_pa_struct _temp298; _temp298.tag= Cyc_std_String_pa;
_temp298.f1=( struct _tagged_arr) ans;{ void* _temp297[ 2u]={& _temp298,&
_temp299}; Cyc_std_aprintf( _tag_arr("%s\n%s", sizeof( unsigned char), 6u),
_tag_arr( _temp297, sizeof( void*), 2u));}}});} goto _LL236; _LL248: ans=({
struct Cyc_std_String_pa_struct _temp301; _temp301.tag= Cyc_std_String_pa;
_temp301.f1=( struct _tagged_arr) ans;{ void* _temp300[ 1u]={& _temp301}; Cyc_std_aprintf(
_tag_arr("%s<<stmt>>", sizeof( unsigned char), 11u), _tag_arr( _temp300, sizeof(
void*), 1u));}}); goto _LL236; _LL250: ans=({ struct Cyc_std_String_pa_struct
_temp303; _temp303.tag= Cyc_std_String_pa; _temp303.f1=( struct _tagged_arr) ans;{
void* _temp302[ 1u]={& _temp303}; Cyc_std_aprintf( _tag_arr("%sbottom", sizeof(
unsigned char), 9u), _tag_arr( _temp302, sizeof( void*), 1u));}}); goto _LL236;
_LL252:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL236:;} return( struct
_tagged_arr) ans;} static void Cyc_CfAbsexp_check_absop( void* ao){ void*
_temp304= ao; void* _temp318; void* _temp320; void* _temp322; _LL306: if(
_temp304 == ( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL307;} else{ goto _LL308;}
_LL308: if(( unsigned int) _temp304 >  1u?*(( int*) _temp304) ==  Cyc_CfAbsexp_AddressOp:
0){ _LL319: _temp318=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp304)->f1;
goto _LL309;} else{ goto _LL310;} _LL310: if(( unsigned int) _temp304 >  1u?*((
int*) _temp304) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL321: _temp320=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp304)->f1; goto _LL311;} else{ goto _LL312;}
_LL312: if(( unsigned int) _temp304 >  1u?*(( int*) _temp304) ==  Cyc_CfAbsexp_MemberOp:
0){ _LL323: _temp322=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp304)->f1;
goto _LL313;} else{ goto _LL314;} _LL314: if(( unsigned int) _temp304 >  1u?*((
int*) _temp304) ==  Cyc_CfAbsexp_LocalOp: 0){ goto _LL315;} else{ goto _LL316;}
_LL316: if(( unsigned int) _temp304 >  1u?*(( int*) _temp304) ==  Cyc_CfAbsexp_MallocOp:
0){ goto _LL317;} else{ goto _LL305;} _LL307: return; _LL309: Cyc_CfAbsexp_check_absop(
_temp318); Cyc_CfAbsexp_ok_address_arg( _temp318); return; _LL311: Cyc_CfAbsexp_check_absop(
_temp320); Cyc_CfAbsexp_ok_deref_arg( _temp320); return; _LL313: Cyc_CfAbsexp_check_absop(
_temp322); Cyc_CfAbsexp_ok_member_arg( _temp322); return; _LL315: return; _LL317:
return; _LL305:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp324= ae;
void* _temp342; struct Cyc_Absyn_Stmt* _temp344; struct Cyc_List_List* _temp346;
void* _temp348; void* _temp350; void* _temp352; _LL326: if( _temp324 == ( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL327;} else{ goto _LL328;} _LL328: if( _temp324
== ( void*) Cyc_CfAbsexp_SkipAE){ goto _LL329;} else{ goto _LL330;} _LL330: if((
unsigned int) _temp324 >  2u?*(( int*) _temp324) ==  Cyc_CfAbsexp_MallocAE: 0){
_LL343: _temp342=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp324)->f1;
if(*(( int*) _temp342) ==  Cyc_CfFlowInfo_VarRoot){ goto _LL331;} else{ goto
_LL332;}} else{ goto _LL332;} _LL332: if(( unsigned int) _temp324 >  2u?*(( int*)
_temp324) ==  Cyc_CfAbsexp_MallocAE: 0){ goto _LL333;} else{ goto _LL334;}
_LL334: if(( unsigned int) _temp324 >  2u?*(( int*) _temp324) ==  Cyc_CfAbsexp_StmtAE:
0){ _LL345: _temp344=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp324)->f1; goto
_LL335;} else{ goto _LL336;} _LL336: if(( unsigned int) _temp324 >  2u?*(( int*)
_temp324) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL347: _temp346=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp324)->f2; goto _LL337;} else{ goto _LL338;} _LL338: if(( unsigned int)
_temp324 >  2u?*(( int*) _temp324) ==  Cyc_CfAbsexp_UseAE: 0){ _LL349: _temp348=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp324)->f1; goto _LL339;} else{
goto _LL340;} _LL340: if(( unsigned int) _temp324 >  2u?*(( int*) _temp324) == 
Cyc_CfAbsexp_AssignAE: 0){ _LL353: _temp352=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp324)->f1; goto _LL351; _LL351: _temp350=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp324)->f2; goto _LL341;} else{ goto _LL325;} _LL327: return; _LL329: return;
_LL331:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL333: return; _LL335:
return; _LL337: for( 0; _temp346 !=  0; _temp346=(( struct Cyc_List_List*)
_check_null( _temp346))->tl){ Cyc_CfAbsexp_check_absexp(( void*)(( struct Cyc_List_List*)
_check_null( _temp346))->hd);} return; _LL339: Cyc_CfAbsexp_check_absop(
_temp348); return; _LL341: Cyc_CfAbsexp_check_absop( _temp352); Cyc_CfAbsexp_check_absop(
_temp350); Cyc_CfAbsexp_ok_lvalue( _temp352); return; _LL325:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao); void* Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp354= ao; struct Cyc_Absyn_Vardecl*
_temp368; struct Cyc_Absyn_Exp* _temp370; void* _temp372; void* _temp374; void*
_temp376; _LL356: if(( unsigned int) _temp354 >  1u?*(( int*) _temp354) ==  Cyc_CfAbsexp_LocalOp:
0){ _LL369: _temp368=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp354)->f1; goto
_LL357;} else{ goto _LL358;} _LL358: if(( unsigned int) _temp354 >  1u?*(( int*)
_temp354) ==  Cyc_CfAbsexp_MallocOp: 0){ _LL371: _temp370=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp354)->f1; goto _LL359;} else{ goto _LL360;} _LL360: if(( unsigned int)
_temp354 >  1u?*(( int*) _temp354) ==  Cyc_CfAbsexp_MemberOp: 0){ _LL375:
_temp374=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp354)->f1; goto
_LL373; _LL373: _temp372=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp354)->f2; goto _LL361;} else{ goto _LL362;} _LL362: if(( unsigned int)
_temp354 >  1u?*(( int*) _temp354) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL377:
_temp376=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp354)->f1; goto
_LL363;} else{ goto _LL364;} _LL364: if( _temp354 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL365;} else{ goto _LL366;} _LL366: if(( unsigned int) _temp354 >  1u?*((
int*) _temp354) ==  Cyc_CfAbsexp_AddressOp: 0){ goto _LL367;} else{ goto _LL355;}
_LL357: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct* _temp378=(
struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct));
_temp378[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct _temp379; _temp379.tag=
Cyc_CfFlowInfo_MustPointTo; _temp379.f1=({ struct Cyc_CfFlowInfo_Place* _temp380=(
struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp380->root=( void*)(( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp381=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp381[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp382; _temp382.tag= Cyc_CfFlowInfo_VarRoot;
_temp382.f1= _temp368; _temp382;}); _temp381;})); _temp380->fields= 0; _temp380;});
_temp379;}); _temp378;}); _LL359: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp383=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp383[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp384; _temp384.tag= Cyc_CfFlowInfo_MustPointTo; _temp384.f1=({ struct Cyc_CfFlowInfo_Place*
_temp385=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp385->root=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct*
_temp386=( struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp386[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp387; _temp387.tag=
Cyc_CfFlowInfo_MallocPt; _temp387.f1= _temp370; _temp387;}); _temp386;}));
_temp385->fields= 0; _temp385;}); _temp384;}); _temp383;}); _LL361: { void*
_temp388= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp374); void* _temp389=
_temp388; struct Cyc_CfFlowInfo_Place* _temp395; struct Cyc_CfFlowInfo_Place
_temp397; struct Cyc_List_List* _temp398; void* _temp400; _LL391: if(*(( int*)
_temp389) ==  Cyc_CfFlowInfo_UnknownIS){ goto _LL392;} else{ goto _LL393;}
_LL393: if(*(( int*) _temp389) ==  Cyc_CfFlowInfo_MustPointTo){ _LL396: _temp395=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp389)->f1; _temp397=* _temp395;
_LL401: _temp400=( void*) _temp397.root; goto _LL399; _LL399: _temp398= _temp397.fields;
goto _LL394;} else{ goto _LL390;} _LL392: return _temp388; _LL394: return( void*)({
struct Cyc_CfFlowInfo_MustPointTo_struct* _temp402=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp402[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp403; _temp403.tag= Cyc_CfFlowInfo_MustPointTo;
_temp403.f1=({ struct Cyc_CfFlowInfo_Place* _temp404=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp404->root=( void*)
_temp400; _temp404->fields= Cyc_List_append( _temp398,({ struct Cyc_List_List*
_temp405=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp405->hd=( void*) _temp372; _temp405->tl= 0; _temp405;})); _temp404;});
_temp403;}); _temp402;}); _LL390:;} _LL363: { void* _temp406= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp376); void* _temp407= _temp406; void* _temp415; _LL409: if(*((
int*) _temp407) ==  Cyc_CfFlowInfo_LeafPI){ _LL416: _temp415=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp407)->f1; goto _LL410;} else{ goto _LL411;}
_LL411: if(*(( int*) _temp407) ==  Cyc_CfFlowInfo_TuplePI){ goto _LL412;} else{
goto _LL413;} _LL413: if(*(( int*) _temp407) ==  Cyc_CfFlowInfo_StructPI){ goto
_LL414;} else{ goto _LL408;} _LL410: return _temp415; _LL412: goto _LL414;
_LL414:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL408:;} _LL365: goto
_LL367; _LL367:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL355:;} void*
Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void*
_temp417= ao; struct Cyc_Absyn_Vardecl* _temp431; struct Cyc_Absyn_Exp* _temp433;
void* _temp435; void* _temp437; void* _temp439; void* _temp441; _LL419: if((
unsigned int) _temp417 >  1u?*(( int*) _temp417) ==  Cyc_CfAbsexp_LocalOp: 0){
_LL432: _temp431=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp417)->f1; goto
_LL420;} else{ goto _LL421;} _LL421: if(( unsigned int) _temp417 >  1u?*(( int*)
_temp417) ==  Cyc_CfAbsexp_MallocOp: 0){ _LL434: _temp433=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp417)->f1; goto _LL422;} else{ goto _LL423;} _LL423: if(( unsigned int)
_temp417 >  1u?*(( int*) _temp417) ==  Cyc_CfAbsexp_MemberOp: 0){ _LL438:
_temp437=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp417)->f1; goto
_LL436; _LL436: _temp435=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp417)->f2; goto _LL424;} else{ goto _LL425;} _LL425: if(( unsigned int)
_temp417 >  1u?*(( int*) _temp417) ==  Cyc_CfAbsexp_DerefOp: 0){ _LL440:
_temp439=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp417)->f1; goto
_LL426;} else{ goto _LL427;} _LL427: if( _temp417 == ( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL428;} else{ goto _LL429;} _LL429: if(( unsigned int) _temp417 >  1u?*((
int*) _temp417) ==  Cyc_CfAbsexp_AddressOp: 0){ _LL442: _temp441=( void*)((
struct Cyc_CfAbsexp_AddressOp_struct*) _temp417)->f1; goto _LL430;} else{ goto
_LL418;} _LL420: return Cyc_Dict_lookup( pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp443=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp443[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp444; _temp444.tag= Cyc_CfFlowInfo_VarRoot;
_temp444.f1= _temp431; _temp444;}); _temp443;})); _LL422: return Cyc_Dict_lookup(
pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp445=( struct
Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp445[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp446; _temp446.tag=
Cyc_CfFlowInfo_MallocPt; _temp446.f1= _temp433; _temp446;}); _temp445;}));
_LL424: { void* _temp447= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp437);
struct _tuple3 _temp449=({ struct _tuple3 _temp448; _temp448.f1= _temp447;
_temp448.f2= _temp435; _temp448;}); void* _temp459; void* _temp461; void*
_temp463; int _temp465; void* _temp467; struct Cyc_Dict_Dict* _temp469; void*
_temp471; struct _tagged_arr* _temp473; void* _temp475; struct Cyc_Dict_Dict*
_temp477; _LL451: _LL460: _temp459= _temp449.f1; if(*(( int*) _temp459) ==  Cyc_CfFlowInfo_LeafPI){
_LL462: _temp461=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp459)->f1;
if(*(( int*) _temp461) ==  Cyc_CfFlowInfo_UnknownIS){ goto _LL452;} else{ goto
_LL453;}} else{ goto _LL453;} _LL453: _LL468: _temp467= _temp449.f1; if(*(( int*)
_temp467) ==  Cyc_CfFlowInfo_TuplePI){ _LL470: _temp469=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp467)->f1; goto _LL464;} else{ goto _LL455;} _LL464: _temp463= _temp449.f2;
if(*(( int*) _temp463) ==  Cyc_CfFlowInfo_TupleF){ _LL466: _temp465=(( struct
Cyc_CfFlowInfo_TupleF_struct*) _temp463)->f1; goto _LL454;} else{ goto _LL455;}
_LL455: _LL476: _temp475= _temp449.f1; if(*(( int*) _temp475) ==  Cyc_CfFlowInfo_StructPI){
_LL478: _temp477=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp475)->f1; goto
_LL472;} else{ goto _LL457;} _LL472: _temp471= _temp449.f2; if(*(( int*)
_temp471) ==  Cyc_CfFlowInfo_StructF){ _LL474: _temp473=(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp471)->f1; goto _LL456;} else{ goto _LL457;} _LL457: goto _LL458; _LL452:
return _temp447; _LL454: return(( void*(*)( struct Cyc_Dict_Dict* d, int key))
Cyc_Dict_lookup)( _temp469, _temp465); _LL456: return(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp477, _temp473); _LL458:( int)
_throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL450:;} _LL426: { void* _temp479= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp439); void* _temp480= _temp479; void* _temp490; void* _temp492;
void* _temp494; void* _temp496; void* _temp498; struct Cyc_CfFlowInfo_Place*
_temp500; _LL482: if(*(( int*) _temp480) ==  Cyc_CfFlowInfo_LeafPI){ _LL491:
_temp490=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp480)->f1; if(*((
int*) _temp490) ==  Cyc_CfFlowInfo_UnknownIS){ _LL495: _temp494=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp490)->f1; goto _LL493; _LL493:
_temp492=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp490)->f2; if(
_temp492 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL483;} else{ goto _LL484;}}
else{ goto _LL484;}} else{ goto _LL484;} _LL484: if(*(( int*) _temp480) ==  Cyc_CfFlowInfo_LeafPI){
_LL497: _temp496=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp480)->f1;
if(*(( int*) _temp496) ==  Cyc_CfFlowInfo_UnknownIS){ goto _LL485;} else{ goto
_LL486;}} else{ goto _LL486;} _LL486: if(*(( int*) _temp480) ==  Cyc_CfFlowInfo_LeafPI){
_LL499: _temp498=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp480)->f1;
if(*(( int*) _temp498) ==  Cyc_CfFlowInfo_MustPointTo){ _LL501: _temp500=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp498)->f1; goto _LL487;} else{
goto _LL488;}} else{ goto _LL488;} _LL488: goto _LL489; _LL483: return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_NoneIL); _LL485: return
_temp479; _LL487: return Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp500);
_LL489:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL481:;} _LL428: return
Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL);
_LL430: return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct* _temp502=( struct
Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp502[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp503; _temp503.tag= Cyc_CfFlowInfo_LeafPI;
_temp503.f1=( void*) Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp441); _temp503;});
_temp502;}); _LL418:;} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo); struct Cyc_Dict_Dict* Cyc_CfAbsexp_assign_escape_f(
struct Cyc_List_List** escaping_states, void* key, void* b1, void* b2, struct
Cyc_Dict_Dict* accum){ return Cyc_Dict_insert( accum, key, Cyc_CfAbsexp_assign_escape(
escaping_states, b1, b2));} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp505=({
struct _tuple3 _temp504; _temp504.f1= old_pinfo; _temp504.f2= new_pinfo;
_temp504;}); void* _temp523; void* _temp525; void* _temp527; void* _temp529;
void* _temp531; void* _temp533; void* _temp535; void* _temp537; void* _temp539;
void* _temp541; void* _temp543; void* _temp545; void* _temp547; void* _temp549;
void* _temp551; void* _temp553; void* _temp555; void* _temp557; void* _temp559;
void* _temp561; void* _temp563; void* _temp565; void* _temp567; struct Cyc_Dict_Dict*
_temp569; void* _temp571; struct Cyc_Dict_Dict* _temp573; void* _temp575; struct
Cyc_Dict_Dict* _temp577; void* _temp579; struct Cyc_Dict_Dict* _temp581; _LL507:
_LL530: _temp529= _temp505.f1; if(*(( int*) _temp529) ==  Cyc_CfFlowInfo_LeafPI){
_LL532: _temp531=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp529)->f1;
if(*(( int*) _temp531) ==  Cyc_CfFlowInfo_MustPointTo){ goto _LL524;} else{ goto
_LL509;}} else{ goto _LL509;} _LL524: _temp523= _temp505.f2; if(*(( int*)
_temp523) ==  Cyc_CfFlowInfo_LeafPI){ _LL526: _temp525=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp523)->f1; if(*(( int*) _temp525) ==  Cyc_CfFlowInfo_UnknownIS){ _LL528:
_temp527=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp525)->f2; goto
_LL508;} else{ goto _LL509;}} else{ goto _LL509;} _LL509: _LL540: _temp539=
_temp505.f1; if(*(( int*) _temp539) ==  Cyc_CfFlowInfo_LeafPI){ _LL542: _temp541=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp539)->f1; if(*(( int*)
_temp541) ==  Cyc_CfFlowInfo_UnknownIS){ _LL544: _temp543=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp541)->f1; if( _temp543 == ( void*) Cyc_CfFlowInfo_Unesc){ goto _LL534;}
else{ goto _LL511;}} else{ goto _LL511;}} else{ goto _LL511;} _LL534: _temp533=
_temp505.f2; if(*(( int*) _temp533) ==  Cyc_CfFlowInfo_LeafPI){ _LL536: _temp535=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp533)->f1; if(*(( int*)
_temp535) ==  Cyc_CfFlowInfo_UnknownIS){ _LL538: _temp537=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp535)->f2; goto _LL510;} else{ goto _LL511;}} else{ goto _LL511;} _LL511:
_LL548: _temp547= _temp505.f1; if(*(( int*) _temp547) ==  Cyc_CfFlowInfo_LeafPI){
_LL550: _temp549=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp547)->f1;
if(*(( int*) _temp549) ==  Cyc_CfFlowInfo_MustPointTo){ goto _LL546;} else{ goto
_LL513;}} else{ goto _LL513;} _LL546: _temp545= _temp505.f2; if(*(( int*)
_temp545) ==  Cyc_CfFlowInfo_LeafPI){ goto _LL512;} else{ goto _LL513;} _LL513:
_LL554: _temp553= _temp505.f1; if(*(( int*) _temp553) ==  Cyc_CfFlowInfo_LeafPI){
_LL556: _temp555=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp553)->f1;
if(*(( int*) _temp555) ==  Cyc_CfFlowInfo_UnknownIS){ _LL558: _temp557=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp555)->f1; if( _temp557 == ( void*)
Cyc_CfFlowInfo_Unesc){ goto _LL552;} else{ goto _LL515;}} else{ goto _LL515;}}
else{ goto _LL515;} _LL552: _temp551= _temp505.f2; if(*(( int*) _temp551) == 
Cyc_CfFlowInfo_LeafPI){ goto _LL514;} else{ goto _LL515;} _LL515: _LL562:
_temp561= _temp505.f1; if(*(( int*) _temp561) ==  Cyc_CfFlowInfo_LeafPI){ _LL564:
_temp563=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp561)->f1; if(*((
int*) _temp563) ==  Cyc_CfFlowInfo_UnknownIS){ _LL566: _temp565=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp563)->f1; if( _temp565 == ( void*)
Cyc_CfFlowInfo_Esc){ goto _LL560;} else{ goto _LL517;}} else{ goto _LL517;}}
else{ goto _LL517;} _LL560: _temp559= _temp505.f2; if(*(( int*) _temp559) == 
Cyc_CfFlowInfo_LeafPI){ goto _LL516;} else{ goto _LL517;} _LL517: _LL572:
_temp571= _temp505.f1; if(*(( int*) _temp571) ==  Cyc_CfFlowInfo_TuplePI){
_LL574: _temp573=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp571)->f1; goto
_LL568;} else{ goto _LL519;} _LL568: _temp567= _temp505.f2; if(*(( int*)
_temp567) ==  Cyc_CfFlowInfo_TuplePI){ _LL570: _temp569=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp567)->f1; goto _LL518;} else{ goto _LL519;} _LL519: _LL580: _temp579=
_temp505.f1; if(*(( int*) _temp579) ==  Cyc_CfFlowInfo_StructPI){ _LL582:
_temp581=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp579)->f1; goto _LL576;}
else{ goto _LL521;} _LL576: _temp575= _temp505.f2; if(*(( int*) _temp575) == 
Cyc_CfFlowInfo_StructPI){ _LL578: _temp577=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp575)->f1; goto _LL520;} else{ goto _LL521;} _LL521: goto _LL522; _LL508:
_temp537= _temp527; goto _LL510; _LL510: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Unesc, _temp537); _LL512: goto _LL514; _LL514: return new_pinfo;
_LL516:* escaping_states=({ struct Cyc_List_List* _temp583=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp583->hd=( void*) new_pinfo;
_temp583->tl=* escaping_states; _temp583;}); return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL); _LL518: return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp584=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp584[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp585; _temp585.tag= Cyc_CfFlowInfo_TuplePI;
_temp585.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( struct Cyc_List_List**,
int, void*, void*, struct Cyc_Dict_Dict*), struct Cyc_List_List** inner_env,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2, struct Cyc_Dict_Dict* accum))
Cyc_Dict_fold2_c)(( struct Cyc_Dict_Dict*(*)( struct Cyc_List_List**
escaping_states, int key, void* b1, void* b2, struct Cyc_Dict_Dict* accum)) Cyc_CfAbsexp_assign_escape_f,
escaping_states, _temp573, _temp569,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp)); _temp585;}); _temp584;}); _LL520:
return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct* _temp586=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp586[ 0]=({
struct Cyc_CfFlowInfo_StructPI_struct _temp587; _temp587.tag= Cyc_CfFlowInfo_StructPI;
_temp587.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( struct Cyc_List_List**,
struct _tagged_arr*, void*, void*, struct Cyc_Dict_Dict*), struct Cyc_List_List**
inner_env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2, struct Cyc_Dict_Dict*
accum)) Cyc_Dict_fold2_c)(( struct Cyc_Dict_Dict*(*)( struct Cyc_List_List**
escaping_states, struct _tagged_arr* key, void* b1, void* b2, struct Cyc_Dict_Dict*
accum)) Cyc_CfAbsexp_assign_escape_f, escaping_states, _temp581, _temp577,((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_std_zstrptrcmp)); _temp587;}); _temp586;}); _LL522:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp588=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp588[ 0]=({ struct
Cyc_Core_Impossible_struct _temp589; _temp589.tag= Cyc_Core_Impossible; _temp589.f1=
_tag_arr("bad pinfos in assign_escape", sizeof( unsigned char), 28u); _temp589;});
_temp588;})); _LL506:;} int Cyc_CfAbsexp_is_ok_malloc_assign( void* lval, struct
Cyc_List_List* ok_mallocs){ void* _temp590= lval; struct Cyc_Absyn_Exp* _temp598;
void* _temp600; _LL592: if(( unsigned int) _temp590 >  1u?*(( int*) _temp590) == 
Cyc_CfAbsexp_MallocOp: 0){ _LL599: _temp598=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp590)->f1; goto _LL593;} else{ goto _LL594;} _LL594: if(( unsigned int)
_temp590 >  1u?*(( int*) _temp590) ==  Cyc_CfAbsexp_MemberOp: 0){ _LL601:
_temp600=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp590)->f1; goto
_LL595;} else{ goto _LL596;} _LL596: goto _LL597; _LL593: return(( int(*)(
struct Cyc_List_List* l, struct Cyc_Absyn_Exp* x)) Cyc_List_memq)( ok_mallocs,
_temp598); _LL595: return Cyc_CfAbsexp_is_ok_malloc_assign( _temp600, ok_mallocs);
_LL597: return 0; _LL591:;} struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; } ; static struct _tuple5 Cyc_CfAbsexp_unordered_mallocs( struct
_RegionHandle* rgn, void* ae, struct Cyc_List_List* ok_mallocs){ struct Cyc_List_List*
_temp602= 0; struct Cyc_List_List* _temp603= 0;{ void* _temp604= ae; void*
_temp626; void* _temp628; void* _temp630; struct Cyc_Absyn_Exp* _temp632; struct
Cyc_Absyn_Stmt* _temp634; struct Cyc_List_List* _temp636; void* _temp638; struct
Cyc_List_List* _temp640; void* _temp642; struct Cyc_List_List* _temp644; void*
_temp646; _LL606: if( _temp604 == ( void*) Cyc_CfAbsexp_BottomAE){ goto _LL607;}
else{ goto _LL608;} _LL608: if( _temp604 == ( void*) Cyc_CfAbsexp_SkipAE){ goto
_LL609;} else{ goto _LL610;} _LL610: if(( unsigned int) _temp604 >  2u?*(( int*)
_temp604) ==  Cyc_CfAbsexp_UseAE: 0){ goto _LL611;} else{ goto _LL612;} _LL612:
if(( unsigned int) _temp604 >  2u?*(( int*) _temp604) ==  Cyc_CfAbsexp_AssignAE:
0){ _LL629: _temp628=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp604)->f1;
goto _LL627; _LL627: _temp626=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp604)->f2; goto _LL613;} else{ goto _LL614;} _LL614: if(( unsigned int)
_temp604 >  2u?*(( int*) _temp604) ==  Cyc_CfAbsexp_MallocAE: 0){ _LL631:
_temp630=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp604)->f1; if(*((
int*) _temp630) ==  Cyc_CfFlowInfo_MallocPt){ _LL633: _temp632=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp630)->f1; goto _LL615;} else{ goto _LL616;}} else{ goto _LL616;} _LL616:
if(( unsigned int) _temp604 >  2u?*(( int*) _temp604) ==  Cyc_CfAbsexp_MallocAE:
0){ goto _LL617;} else{ goto _LL618;} _LL618: if(( unsigned int) _temp604 >  2u?*((
int*) _temp604) ==  Cyc_CfAbsexp_StmtAE: 0){ _LL635: _temp634=(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp604)->f1; goto _LL619;} else{ goto _LL620;} _LL620: if(( unsigned int)
_temp604 >  2u?*(( int*) _temp604) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL639:
_temp638=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp604)->f1; if(
_temp638 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL637;} else{ goto _LL622;}
_LL637: _temp636=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp604)->f2; goto
_LL621;} else{ goto _LL622;} _LL622: if(( unsigned int) _temp604 >  2u?*(( int*)
_temp604) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL643: _temp642=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp604)->f1; if( _temp642 == ( void*) Cyc_CfAbsexp_OneofG){ goto _LL641;}
else{ goto _LL624;} _LL641: _temp640=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp604)->f2; goto _LL623;} else{ goto _LL624;} _LL624: if(( unsigned int)
_temp604 >  2u?*(( int*) _temp604) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL647:
_temp646=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp604)->f1; if(
_temp646 == ( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL645;} else{ goto _LL605;}
_LL645: _temp644=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp604)->f2; goto
_LL625;} else{ goto _LL605;} _LL607: goto _LL605; _LL609: goto _LL605; _LL611:
goto _LL605; _LL613: if( Cyc_CfAbsexp_is_ok_malloc_assign( _temp628, ok_mallocs)){
_temp602=({ struct Cyc_List_List* _temp648=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp648->hd=( void*) ae;
_temp648->tl= _temp602; _temp648;});} goto _LL605; _LL615: _temp603=({ struct
Cyc_List_List* _temp649=( struct Cyc_List_List*) _region_malloc( rgn, sizeof(
struct Cyc_List_List)); _temp649->hd=( void*) _temp632; _temp649->tl= _temp603;
_temp649;}); goto _LL605; _LL617:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp650=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp650[ 0]=({ struct Cyc_Core_Impossible_struct _temp651; _temp651.tag= Cyc_Core_Impossible;
_temp651.f1= _tag_arr("local variable in MallocAE", sizeof( unsigned char), 27u);
_temp651;}); _temp650;})); _LL619: goto _LL605; _LL621: for( 0; _temp636 !=  0;
_temp636=(( struct Cyc_List_List*) _check_null( _temp636))->tl){ struct Cyc_List_List*
_temp654; struct Cyc_List_List* _temp656; struct _tuple5 _temp652= Cyc_CfAbsexp_unordered_mallocs(
rgn,( void*)(( struct Cyc_List_List*) _check_null( _temp636))->hd,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp603, ok_mallocs)); _LL657: _temp656= _temp652.f1; goto _LL655; _LL655:
_temp654= _temp652.f2; goto _LL653; _LL653: _temp602= Cyc_List_rappend( rgn,
_temp656, _temp602); _temp603=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn,
_temp654, _temp603);} goto _LL605; _LL623: goto _LL605; _LL625: for( 0; _temp644
!=  0; _temp644=(( struct Cyc_List_List*) _check_null( _temp644))->tl){ struct
Cyc_List_List* _temp660; struct Cyc_List_List* _temp662; struct _tuple5 _temp658=
Cyc_CfAbsexp_unordered_mallocs( rgn,( void*)(( struct Cyc_List_List*)
_check_null( _temp644))->hd, ok_mallocs); _LL663: _temp662= _temp658.f1; goto
_LL661; _LL661: _temp660= _temp658.f2; goto _LL659; _LL659: _temp602= Cyc_List_rappend(
rgn, _temp662, _temp602); _temp603=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp660, _temp603);} goto _LL605; _LL605:;} return({ struct _tuple5
_temp664; _temp664.f1= _temp602; _temp664.f2= _temp603; _temp664;});} void* Cyc_CfAbsexp_eval_absexp(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void*
ae, void* in_flow){ struct Cyc_Dict_Dict* pinfo_dict;{ void* _temp665= in_flow;
struct Cyc_Dict_Dict* _temp671; _LL667: if( _temp665 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL668;} else{ goto _LL669;} _LL669: if(( unsigned int) _temp665 >  1u?*((
int*) _temp665) ==  Cyc_CfFlowInfo_InitsFL: 0){ _LL672: _temp671=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp665)->f1; goto _LL670;} else{ goto _LL666;} _LL668: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL670: pinfo_dict= _temp671; goto _LL666; _LL666:;}{ void* _temp673= ae; void*
_temp693; void* _temp695; void* _temp697; void* _temp699; struct Cyc_Absyn_Stmt*
_temp701; struct Cyc_List_List* _temp703; void* _temp705; struct Cyc_List_List*
_temp707; void* _temp709; struct Cyc_List_List* _temp711; void* _temp713; _LL675:
if(( unsigned int) _temp673 >  2u?*(( int*) _temp673) ==  Cyc_CfAbsexp_AssignAE:
0){ _LL696: _temp695=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp673)->f1;
goto _LL694; _LL694: _temp693=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp673)->f2; goto _LL676;} else{ goto _LL677;} _LL677: if(( unsigned int)
_temp673 >  2u?*(( int*) _temp673) ==  Cyc_CfAbsexp_UseAE: 0){ _LL698: _temp697=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp673)->f1; goto _LL678;} else{
goto _LL679;} _LL679: if(( unsigned int) _temp673 >  2u?*(( int*) _temp673) == 
Cyc_CfAbsexp_MallocAE: 0){ _LL700: _temp699=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp673)->f1; goto _LL680;} else{ goto _LL681;} _LL681: if( _temp673 == ( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL682;} else{ goto _LL683;} _LL683: if( _temp673
== ( void*) Cyc_CfAbsexp_SkipAE){ goto _LL684;} else{ goto _LL685;} _LL685: if((
unsigned int) _temp673 >  2u?*(( int*) _temp673) ==  Cyc_CfAbsexp_StmtAE: 0){
_LL702: _temp701=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp673)->f1; goto
_LL686;} else{ goto _LL687;} _LL687: if(( unsigned int) _temp673 >  2u?*(( int*)
_temp673) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL706: _temp705=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp673)->f1; if( _temp705 == ( void*) Cyc_CfAbsexp_OrderedG){ goto _LL704;}
else{ goto _LL689;} _LL704: _temp703=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp673)->f2; goto _LL688;} else{ goto _LL689;} _LL689: if(( unsigned int)
_temp673 >  2u?*(( int*) _temp673) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL710:
_temp709=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp673)->f1; if(
_temp709 == ( void*) Cyc_CfAbsexp_OneofG){ goto _LL708;} else{ goto _LL691;}
_LL708: _temp707=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp673)->f2; goto
_LL690;} else{ goto _LL691;} _LL691: if(( unsigned int) _temp673 >  2u?*(( int*)
_temp673) ==  Cyc_CfAbsexp_GroupAE: 0){ _LL714: _temp713=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp673)->f1; if( _temp713 == ( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL712;}
else{ goto _LL674;} _LL712: _temp711=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp673)->f2; goto _LL692;} else{ goto _LL674;} _LL676:{ void* _temp715= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp695); struct Cyc_CfFlowInfo_Place* _temp721; _LL717: if(*(( int*)
_temp715) ==  Cyc_CfFlowInfo_UnknownIS){ goto _LL718;} else{ goto _LL719;}
_LL719: if(*(( int*) _temp715) ==  Cyc_CfFlowInfo_MustPointTo){ _LL722: _temp721=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp715)->f1; goto _LL720;} else{
goto _LL716;} _LL718: goto _LL716; _LL720: { void* _temp723= Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp721); void* _temp724= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp693);{ struct _tuple3 _temp726=({ struct _tuple3 _temp725; _temp725.f1=
_temp723; _temp725.f2= _temp724; _temp725;}); void* _temp734; void* _temp736;
void* _temp738; void* _temp740; void* _temp742; void* _temp744; void* _temp746;
void* _temp748; _LL728: _LL741: _temp740= _temp726.f1; if(*(( int*) _temp740) == 
Cyc_CfFlowInfo_TuplePI){ goto _LL735;} else{ goto _LL730;} _LL735: _temp734=
_temp726.f2; if(*(( int*) _temp734) ==  Cyc_CfFlowInfo_LeafPI){ _LL737: _temp736=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp734)->f1; if(*(( int*)
_temp736) ==  Cyc_CfFlowInfo_UnknownIS){ _LL739: _temp738=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp736)->f2; goto _LL729;} else{ goto _LL730;}} else{ goto _LL730;} _LL730:
_LL749: _temp748= _temp726.f1; if(*(( int*) _temp748) ==  Cyc_CfFlowInfo_StructPI){
goto _LL743;} else{ goto _LL732;} _LL743: _temp742= _temp726.f2; if(*(( int*)
_temp742) ==  Cyc_CfFlowInfo_LeafPI){ _LL745: _temp744=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp742)->f1; if(*(( int*) _temp744) ==  Cyc_CfFlowInfo_UnknownIS){ _LL747:
_temp746=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp744)->f2; goto
_LL731;} else{ goto _LL732;}} else{ goto _LL732;} _LL732: goto _LL733; _LL729:
_temp746= _temp738; goto _LL731; _LL731: _temp724= Cyc_CfFlowInfo_assign_unknown_dict(
_temp746, _temp723); goto _LL727; _LL733: goto _LL727; _LL727:;}{ struct Cyc_List_List*
_temp750= 0; void* _temp751= Cyc_CfAbsexp_assign_escape(& _temp750, _temp723,
_temp724); for( 0; _temp750 !=  0; _temp750=(( struct Cyc_List_List*)
_check_null( _temp750))->tl){ void* _temp752=( void*)(( struct Cyc_List_List*)
_check_null( _temp750))->hd; if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict,
_temp752)){({ void* _temp753[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("uninitialized value assigned to escaped place",
sizeof( unsigned char), 46u), _tag_arr( _temp753, sizeof( void*), 0u));});
return( void*) Cyc_CfFlowInfo_BottomFL;} pinfo_dict= Cyc_CfFlowInfo_escape_pointsto(
_temp752, pinfo_dict);} pinfo_dict= Cyc_CfFlowInfo_insert_place( pinfo_dict,
_temp721, _temp751);{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp754=({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp755=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp755[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp756; _temp756.tag= Cyc_CfFlowInfo_InitsFL;
_temp756.f1= pinfo_dict; _temp756;}); _temp755;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp754); return( void*) _temp754;}}} _LL716:;} _temp697= _temp693;
goto _LL678; _LL678: { void* _temp757= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp697); if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, _temp757)){ void*
_temp758= _temp697; struct Cyc_Absyn_Vardecl* _temp764; _LL760: if((
unsigned int) _temp758 >  1u?*(( int*) _temp758) ==  Cyc_CfAbsexp_LocalOp: 0){
_LL765: _temp764=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp758)->f1; goto
_LL761;} else{ goto _LL762;} _LL762: goto _LL763; _LL761: if( Cyc_Tcutil_bits_only((
void*) _temp764->type)){({ struct Cyc_std_String_pa_struct _temp767; _temp767.tag=
Cyc_std_String_pa; _temp767.f1=( struct _tagged_arr)*(* _temp764->name).f2;{
void* _temp766[ 1u]={& _temp767}; Cyc_Tcutil_warn( loc, _tag_arr("pointer-free %s may not be fully initialized",
sizeof( unsigned char), 45u), _tag_arr( _temp766, sizeof( void*), 1u));}}); goto
_LL759;} goto _LL763; _LL763:({ struct Cyc_std_String_pa_struct _temp769;
_temp769.tag= Cyc_std_String_pa; _temp769.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp697);{ void* _temp768[ 1u]={& _temp769}; Cyc_Tcutil_terr( loc, _tag_arr("%s must be fully initialized to be used here",
sizeof( unsigned char), 45u), _tag_arr( _temp768, sizeof( void*), 1u));}});
return( void*) Cyc_CfFlowInfo_BottomFL; _LL759:;}{ struct Cyc_Dict_Dict*
_temp770= Cyc_CfFlowInfo_escape_pointsto( _temp757, pinfo_dict); if( _temp770 == 
pinfo_dict){ return in_flow;}{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp771=({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp772=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp772[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp773; _temp773.tag= Cyc_CfFlowInfo_InitsFL;
_temp773.f1= pinfo_dict; _temp773;}); _temp772;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp771); return( void*) _temp771;}}} _LL680: return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp774=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp774[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp775; _temp775.tag= Cyc_CfFlowInfo_InitsFL;
_temp775.f1= Cyc_Dict_insert( pinfo_dict, _temp699, Cyc_Dict_lookup( env->roots,
_temp699)); _temp775;}); _temp774;}); _LL682: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL684: return in_flow; _LL686: return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp701, in_flow); _LL688: for( 0; _temp703 !=  0; _temp703=(( struct Cyc_List_List*)
_check_null( _temp703))->tl){ in_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)((
struct Cyc_List_List*) _check_null( _temp703))->hd, in_flow);} return in_flow;
_LL690: { void* out_flow=( void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp707 != 
0; _temp707=(( struct Cyc_List_List*) _check_null( _temp707))->tl){ out_flow=
Cyc_CfFlowInfo_join_flow( out_flow, Cyc_CfAbsexp_eval_absexp( env, loc,( void*)((
struct Cyc_List_List*) _check_null( _temp707))->hd, in_flow));} return out_flow;}
_LL692: { void* _temp776= in_flow; do { in_flow= _temp776;{ struct Cyc_List_List*
ael2= _temp711; for( 0; ael2 !=  0; ael2=(( struct Cyc_List_List*) _check_null(
ael2))->tl){ _temp776= Cyc_CfFlowInfo_join_flow( _temp776, Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( ael2))->hd, _temp776));}}} while (
! Cyc_CfFlowInfo_flow_lessthan_approx( _temp776, in_flow));{ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp777= in_flow; struct Cyc_Dict_Dict* _temp783; _LL779:
if( _temp777 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL780;} else{ goto
_LL781;} _LL781: if(( unsigned int) _temp777 >  1u?*(( int*) _temp777) ==  Cyc_CfFlowInfo_InitsFL:
0){ _LL784: _temp783=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp777)->f1;
goto _LL782;} else{ goto _LL778;} _LL780: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL782: pinfo_dict= _temp783; goto _LL778; _LL778:;}{ struct _RegionHandle
_temp785= _new_region(); struct _RegionHandle* spine_rgn=& _temp785;
_push_region( spine_rgn);{ struct Cyc_List_List* _temp788; struct Cyc_List_List*
_temp790; struct _tuple5 _temp786= Cyc_CfAbsexp_unordered_mallocs( spine_rgn, ae,
0); _LL791: _temp790= _temp786.f1; goto _LL789; _LL789: _temp788= _temp786.f2;
goto _LL787; _LL787: for( 0; _temp788 !=  0; _temp788=(( struct Cyc_List_List*)
_check_null( _temp788))->tl){ struct Cyc_CfFlowInfo_MallocPt_struct* _temp792=({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp793=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp793[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp794; _temp794.tag= Cyc_CfFlowInfo_MallocPt;
_temp794.f1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp788))->hd; _temp794;}); _temp793;}); pinfo_dict= Cyc_Dict_insert(
pinfo_dict,( void*) _temp792, Cyc_Dict_lookup( env->roots,( void*) _temp792));}
_temp776=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp795=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp795[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp796; _temp796.tag= Cyc_CfFlowInfo_InitsFL;
_temp796.f1= pinfo_dict; _temp796;}); _temp795;}); for( 0; _temp790 !=  0;
_temp790=(( struct Cyc_List_List*) _check_null( _temp790))->tl){ _temp776= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp790))->hd, _temp776);}};
_pop_region( spine_rgn);} return _temp776;}} _LL674:;}}
