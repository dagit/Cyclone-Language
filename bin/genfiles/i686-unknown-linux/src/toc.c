 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE*
Cyc_Std_stderr; extern int Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_vfprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr ap); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern int
Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rrev(
struct _RegionHandle*, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_forall( int(* pred)( void*),
struct Cyc_List_List* x); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_strconcat( struct
_tagged_arr, struct _tagged_arr); struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void*
key); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
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
type; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars;
struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args; int
c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value; extern void* Cyc_Absyn_rel_ns_null;
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_star_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void*
Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_arr* name); extern void* Cyc_Absyn_strctq(
struct _tuple0* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_match_exn_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_arr* v,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp*
init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl( struct _tuple0*
x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue( struct
Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field(
struct Cyc_List_List*, struct _tagged_arr*); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*); static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify(
void*, void*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern
void* Cyc_Tcutil_snd_tqt( struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern struct _tuple0* Cyc_Toc_temp_var(); static struct Cyc_List_List* Cyc_Toc_result_decls=
0; static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter=
0; static int Cyc_Toc_temp_var_counter= 0; static int Cyc_Toc_fresh_label_counter=
0; unsigned char Cyc_Toc_Toc_Unimplemented[ 22u]="\000\000\000\000Toc_Unimplemented";
unsigned char Cyc_Toc_Toc_Impossible[ 19u]="\000\000\000\000Toc_Impossible";
static void* Cyc_Toc_unimp( struct _tagged_arr fmt, struct _tagged_arr ap){ Cyc_Std_vfprintf(
Cyc_Std_stderr, fmt, ap);({ void* _temp0[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp0, sizeof( void*), 0u));});
Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);( int) _throw(( void*)
Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos( struct _tagged_arr
fmt, struct _tagged_arr ap){ Cyc_Std_vfprintf( Cyc_Std_stderr, fmt, ap);({ void*
_temp1[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp1, sizeof( void*), 0u));}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Impossible);}
unsigned char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static
unsigned char _temp2[ 5u]="curr"; static struct _tagged_arr Cyc_Toc_curr_string={
_temp2, _temp2, _temp2 +  5u}; static struct _tagged_arr* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static unsigned char _temp3[ 4u]="tag"; static struct _tagged_arr Cyc_Toc_tag_string={
_temp3, _temp3, _temp3 +  4u}; static struct _tagged_arr* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _temp4[ 14u]="_handler_cons"; static struct _tagged_arr Cyc_Toc__handler_cons_string={
_temp4, _temp4, _temp4 +  14u}; static struct _tagged_arr* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp5[ 8u]="handler"; static
struct _tagged_arr Cyc_Toc_handler_string={ _temp5, _temp5, _temp5 +  8u};
static struct _tagged_arr* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static
unsigned char _temp6[ 14u]="_RegionHandle"; static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_temp6, _temp6, _temp6 +  14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string; static unsigned char _temp7[ 7u]="_throw"; static
struct _tagged_arr Cyc_Toc__throw_str={ _temp7, _temp7, _temp7 +  7u}; static
struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={ 1u,& Cyc_Toc__throw_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={ 0,( void*)((
void*)& Cyc_Toc__throw_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=&
Cyc_Toc__throw_ev; static unsigned char _temp9[ 7u]="setjmp"; static struct
_tagged_arr Cyc_Toc_setjmp_str={ _temp9, _temp9, _temp9 +  7u}; static struct
_tuple0 Cyc_Toc_setjmp_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={ 1u,& Cyc_Toc_setjmp_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={ 0,( void*)((
void*)& Cyc_Toc_setjmp_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev; static unsigned char _temp11[ 14u]="_push_handler"; static
struct _tagged_arr Cyc_Toc__push_handler_str={ _temp11, _temp11, _temp11 +  14u};
static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_handler_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1u,& Cyc_Toc__push_handler_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__push_handler_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp13[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp13, _temp13, _temp13 +  13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_handler_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__pop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=&
Cyc_Toc__pop_handler_ev; static unsigned char _temp15[ 12u]="_exn_thrown";
static struct _tagged_arr Cyc_Toc__exn_thrown_str={ _temp15, _temp15, _temp15 + 
12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1u,& Cyc_Toc__exn_thrown_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__exn_thrown_ev={ 0,( void*)(( void*)& Cyc_Toc__exn_thrown_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp17[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp17, _temp17, _temp17 +  14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev; static unsigned char _temp19[ 12u]="_check_null";
static struct _tagged_arr Cyc_Toc__check_null_str={ _temp19, _temp19, _temp19 + 
12u}; static struct _tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_null_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1u,& Cyc_Toc__check_null_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_null_ev={ 0,( void*)(( void*)& Cyc_Toc__check_null_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;
static unsigned char _temp21[ 28u]="_check_known_subscript_null"; static struct
_tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp21, _temp21, _temp21
+  28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str}; static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,& Cyc_Toc__check_known_subscript_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re), 0}; static struct
Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _temp23[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp23, _temp23,
_temp23 +  31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp25[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp25, _temp25, _temp25 + 
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=& Cyc_Toc__check_unknown_subscript_ev;
static unsigned char _temp27[ 12u]="_tagged_arr"; static struct _tagged_arr Cyc_Toc__tagged_arr_str={
_temp27, _temp27, _temp27 +  12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={ 1u,& Cyc_Toc__tagged_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__tagged_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=&
Cyc_Toc__tagged_arr_ev; static unsigned char _temp29[ 9u]="_tag_arr"; static
struct _tagged_arr Cyc_Toc__tag_arr_str={ _temp29, _temp29, _temp29 +  9u};
static struct _tuple0 Cyc_Toc__tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={
1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,( void*)(( void*)& Cyc_Toc__tag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=&
Cyc_Toc__tag_arr_ev; static unsigned char _temp31[ 14u]="_init_tag_arr"; static
struct _tagged_arr Cyc_Toc__init_tag_arr_str={ _temp31, _temp31, _temp31 +  14u};
static struct _tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__init_tag_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={
1u,& Cyc_Toc__init_tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__init_tag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=& Cyc_Toc__init_tag_arr_ev;
static unsigned char _temp33[ 11u]="_untag_arr"; static struct _tagged_arr Cyc_Toc__untag_arr_str={
_temp33, _temp33, _temp33 +  11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__untag_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;
static unsigned char _temp35[ 14u]="_get_arr_size"; static struct _tagged_arr
Cyc_Toc__get_arr_size_str={ _temp35, _temp35, _temp35 +  14u}; static struct
_tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={ 1u,& Cyc_Toc__get_arr_size_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={ 0,(
void*)(( void*)& Cyc_Toc__get_arr_size_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev; static unsigned char _temp37[ 17u]="_tagged_arr_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={ _temp37, _temp37,
_temp37 +  17u}; static struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_plus_re={ 1u,& Cyc_Toc__tagged_arr_plus_pr,( void*)(( void*)
0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_plus_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_plus_e=&
Cyc_Toc__tagged_arr_plus_ev; static unsigned char _temp39[ 25u]="_tagged_arr_inplace_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={ _temp39,
_temp39, _temp39 +  25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={ 1u,&
Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static unsigned char _temp41[ 30u]="_tagged_arr_inplace_plus_post"; static
struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp41, _temp41,
_temp41 +  30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp43[ 10u]="_cycalloc"; static struct _tagged_arr Cyc_Toc__cycalloc_str={
_temp43, _temp43, _temp43 +  10u}; static struct _tuple0 Cyc_Toc__cycalloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={ 1u,& Cyc_Toc__cycalloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={ 0,( void*)(( void*)& Cyc_Toc__cycalloc_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static unsigned char _temp45[ 17u]="_cycalloc_atomic"; static struct _tagged_arr
Cyc_Toc__cycalloc_atomic_str={ _temp45, _temp45, _temp45 +  17u}; static struct
_tuple0 Cyc_Toc__cycalloc_atomic_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={ 1u,& Cyc_Toc__cycalloc_atomic_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,( void*)(( void*)& Cyc_Toc__cycalloc_atomic_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev; static unsigned char
_temp47[ 15u]="_region_malloc"; static struct _tagged_arr Cyc_Toc__region_malloc_str={
_temp47, _temp47, _temp47 +  15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev; static unsigned char
_temp49[ 13u]="_check_times"; static struct _tagged_arr Cyc_Toc__check_times_str={
_temp49, _temp49, _temp49 +  13u}; static struct _tuple0 Cyc_Toc__check_times_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={ 1u,& Cyc_Toc__check_times_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_times_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev; static unsigned char _temp51[ 12u]="_new_region";
static struct _tagged_arr Cyc_Toc__new_region_str={ _temp51, _temp51, _temp51 + 
12u}; static struct _tuple0 Cyc_Toc__new_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__new_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1u,& Cyc_Toc__new_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__new_region_ev={ 0,( void*)(( void*)& Cyc_Toc__new_region_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;
static unsigned char _temp53[ 13u]="_push_region"; static struct _tagged_arr Cyc_Toc__push_region_str={
_temp53, _temp53, _temp53 +  13u}; static struct _tuple0 Cyc_Toc__push_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={ 1u,& Cyc_Toc__push_region_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={ 0,(
void*)(( void*)& Cyc_Toc__push_region_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev; static unsigned char _temp55[ 12u]="_pop_region";
static struct _tagged_arr Cyc_Toc__pop_region_str={ _temp55, _temp55, _temp55 + 
12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1u,& Cyc_Toc__pop_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__pop_region_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_region_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;
static struct Cyc_Absyn_StructType_struct Cyc_Toc_tagged_arr_typ_v={ 9u,( struct
_tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0}; static void* Cyc_Toc_tagged_arr_typ=(
void*)& Cyc_Toc_tagged_arr_typ_v; static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={
0, 0, 0}; static unsigned char _temp58[ 3u]="f0"; static struct _tagged_arr Cyc_Toc_f0={
_temp58, _temp58, _temp58 +  3u}; static struct _tagged_arr* Cyc_Toc_field_names_v[
1u]={& Cyc_Toc_f0}; static struct _tagged_arr Cyc_Toc_field_names={( void*)((
struct _tagged_arr**) Cyc_Toc_field_names_v),( void*)(( struct _tagged_arr**)
Cyc_Toc_field_names_v),( void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v) + 
1u}; static struct _tagged_arr* Cyc_Toc_fieldname( int i){ unsigned int fsz=
_get_arr_size( Cyc_Toc_field_names, sizeof( struct _tagged_arr*)); if( i >=  fsz){
Cyc_Toc_field_names=({ unsigned int _temp59=( unsigned int)( i +  1); struct
_tagged_arr** _temp60=( struct _tagged_arr**) _cycalloc( _check_times( sizeof(
struct _tagged_arr*), _temp59)); struct _tagged_arr _temp65= _tag_arr( _temp60,
sizeof( struct _tagged_arr*),( unsigned int)( i +  1));{ unsigned int _temp61=
_temp59; unsigned int j; for( j= 0; j <  _temp61; j ++){ _temp60[ j]= j <  fsz?*((
struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names, sizeof(
struct _tagged_arr*),( int) j)):({ struct _tagged_arr* _temp62=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp62[ 0]=( struct
_tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp64; _temp64.tag= Cyc_Std_Int_pa;
_temp64.f1=( int) j;{ void* _temp63[ 1u]={& _temp64}; Cyc_Std_aprintf( _tag_arr("f%d",
sizeof( unsigned char), 4u), _tag_arr( _temp63, sizeof( void*), 1u));}});
_temp62;});}}; _temp65;});} return*(( struct _tagged_arr**)
_check_unknown_subscript( Cyc_Toc_field_names, sizeof( struct _tagged_arr*), i));}
struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static
int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp66=( void*) e->r; void*
_temp92; unsigned char _temp94; void* _temp96; short _temp98; void* _temp100;
int _temp102; void* _temp104; long long _temp106; void* _temp108; struct Cyc_Absyn_Exp*
_temp110; struct Cyc_List_List* _temp112; struct Cyc_List_List* _temp114; struct
Cyc_List_List* _temp116; struct Cyc_List_List* _temp118; struct Cyc_List_List*
_temp120; _LL68: if(*(( int*) _temp66) ==  Cyc_Absyn_Const_e){ _LL93: _temp92=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp66)->f1; if(( unsigned int)
_temp92 >  1u?*(( int*) _temp92) ==  Cyc_Absyn_Char_c: 0){ _LL95: _temp94=((
struct Cyc_Absyn_Char_c_struct*) _temp92)->f2; goto _LL69;} else{ goto _LL70;}}
else{ goto _LL70;} _LL70: if(*(( int*) _temp66) ==  Cyc_Absyn_Const_e){ _LL97:
_temp96=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp66)->f1; if((
unsigned int) _temp96 >  1u?*(( int*) _temp96) ==  Cyc_Absyn_Short_c: 0){ _LL99:
_temp98=(( struct Cyc_Absyn_Short_c_struct*) _temp96)->f2; goto _LL71;} else{
goto _LL72;}} else{ goto _LL72;} _LL72: if(*(( int*) _temp66) ==  Cyc_Absyn_Const_e){
_LL101: _temp100=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp66)->f1; if((
unsigned int) _temp100 >  1u?*(( int*) _temp100) ==  Cyc_Absyn_Int_c: 0){ _LL103:
_temp102=(( struct Cyc_Absyn_Int_c_struct*) _temp100)->f2; goto _LL73;} else{
goto _LL74;}} else{ goto _LL74;} _LL74: if(*(( int*) _temp66) ==  Cyc_Absyn_Const_e){
_LL105: _temp104=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp66)->f1; if((
unsigned int) _temp104 >  1u?*(( int*) _temp104) ==  Cyc_Absyn_LongLong_c: 0){
_LL107: _temp106=(( struct Cyc_Absyn_LongLong_c_struct*) _temp104)->f2; goto
_LL75;} else{ goto _LL76;}} else{ goto _LL76;} _LL76: if(*(( int*) _temp66) == 
Cyc_Absyn_Const_e){ _LL109: _temp108=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp66)->f1; if( _temp108 == ( void*) Cyc_Absyn_Null_c){ goto _LL77;} else{
goto _LL78;}} else{ goto _LL78;} _LL78: if(*(( int*) _temp66) ==  Cyc_Absyn_Cast_e){
_LL111: _temp110=(( struct Cyc_Absyn_Cast_e_struct*) _temp66)->f2; goto _LL79;}
else{ goto _LL80;} _LL80: if(*(( int*) _temp66) ==  Cyc_Absyn_Tuple_e){ _LL113:
_temp112=(( struct Cyc_Absyn_Tuple_e_struct*) _temp66)->f1; goto _LL81;} else{
goto _LL82;} _LL82: if(*(( int*) _temp66) ==  Cyc_Absyn_Array_e){ _LL115:
_temp114=(( struct Cyc_Absyn_Array_e_struct*) _temp66)->f1; goto _LL83;} else{
goto _LL84;} _LL84: if(*(( int*) _temp66) ==  Cyc_Absyn_Struct_e){ _LL117:
_temp116=(( struct Cyc_Absyn_Struct_e_struct*) _temp66)->f3; goto _LL85;} else{
goto _LL86;} _LL86: if(*(( int*) _temp66) ==  Cyc_Absyn_CompoundLit_e){ _LL119:
_temp118=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp66)->f2; goto _LL87;}
else{ goto _LL88;} _LL88: if(*(( int*) _temp66) ==  Cyc_Absyn_UnresolvedMem_e){
_LL121: _temp120=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp66)->f2; goto
_LL89;} else{ goto _LL90;} _LL90: goto _LL91; _LL69: return _temp94 == '\000';
_LL71: return _temp98 ==  0; _LL73: return _temp102 ==  0; _LL75: return
_temp106 ==  0; _LL77: return 1; _LL79: return Cyc_Toc_is_zero( _temp110); _LL81:
return(( int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x))
Cyc_List_forall)( Cyc_Toc_is_zero, _temp112); _LL83: _temp116= _temp114; goto
_LL85; _LL85: _temp118= _temp116; goto _LL87; _LL87: _temp120= _temp118; goto
_LL89; _LL89: for( 0; _temp120 !=  0; _temp120=(( struct Cyc_List_List*)
_check_null( _temp120))->tl){ if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp120))->hd)).f2)){ return 0;}} return 1;
_LL91: return 0; _LL67:;} static int Cyc_Toc_is_nullable( void* t){ void*
_temp122= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp128; struct Cyc_Absyn_Conref*
_temp130; _LL124: if(( unsigned int) _temp122 >  4u?*(( int*) _temp122) ==  Cyc_Absyn_PointerType:
0){ _LL129: _temp128=(( struct Cyc_Absyn_PointerType_struct*) _temp122)->f1;
_LL131: _temp130= _temp128.nullable; goto _LL125;} else{ goto _LL126;} _LL126:
goto _LL127; _LL125: return(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp130); _LL127:({ void* _temp132[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_nullable", sizeof(
unsigned char), 12u), _tag_arr( _temp132, sizeof( void*), 0u));}); return 0;
_LL123:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x,
struct _tagged_arr tag){ return({ struct _tuple0* _temp133=( struct _tuple0*)
_cycalloc( sizeof( struct _tuple0)); _temp133->f1=(* x).f1; _temp133->f2=({
struct _tagged_arr* _temp134=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp134[ 0]=( struct _tagged_arr) Cyc_Std_strconcat(*(* x).f2,
tag); _temp134;}); _temp133;});} struct _tuple5{ struct _tagged_arr* f1; struct
Cyc_List_List* f2; } ; static struct _tagged_arr* Cyc_Toc_add_tuple_type( struct
Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp135= Cyc_Toc_tuple_types; for(
0; _temp135 !=  0; _temp135=(( struct Cyc_List_List*) _check_null( _temp135))->tl){
struct _tuple5 _temp138; struct Cyc_List_List* _temp139; struct _tagged_arr*
_temp141; struct _tuple5* _temp136=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp135))->hd; _temp138=* _temp136; _LL142: _temp141= _temp138.f1;
goto _LL140; _LL140: _temp139= _temp138.f2; goto _LL137; _LL137: { struct Cyc_List_List*
_temp143= tqs0; for( 0; _temp143 !=  0? _temp139 !=  0: 0;( _temp143=(( struct
Cyc_List_List*) _check_null( _temp143))->tl, _temp139=(( struct Cyc_List_List*)
_check_null( _temp139))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp143))->hd)).f2,( void*)(( struct Cyc_List_List*)
_check_null( _temp139))->hd)){ break;}} if( _temp143 ==  0? _temp139 ==  0: 0){
return _temp141;}}}}{ struct _tagged_arr* x=({ struct _tagged_arr* _temp157=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp157[ 0]=(
struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp159; _temp159.tag= Cyc_Std_Int_pa;
_temp159.f1=( int)(( unsigned int) Cyc_Toc_tuple_type_counter ++);{ void*
_temp158[ 1u]={& _temp159}; Cyc_Std_aprintf( _tag_arr("_tuple%d", sizeof(
unsigned char), 9u), _tag_arr( _temp158, sizeof( void*), 1u));}}); _temp157;});
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List*
_temp144= 0; struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 !=  0;( ts2=((
struct Cyc_List_List*) _check_null( ts2))->tl, i ++)){ _temp144=({ struct Cyc_List_List*
_temp145=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp145->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp146=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp146->name= Cyc_Toc_fieldname(
i); _temp146->tq= Cyc_Toc_mt_tq; _temp146->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts2))->hd); _temp146->width= 0; _temp146->attributes= 0; _temp146;});
_temp145->tl= _temp144; _temp145;});}} _temp144=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp144);{ struct Cyc_Absyn_Structdecl*
_temp147=({ struct Cyc_Absyn_Structdecl* _temp153=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp153->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp153->name=({ struct Cyc_Core_Opt* _temp155=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp155->v=( void*)({ struct _tuple0*
_temp156=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp156->f1=
Cyc_Absyn_rel_ns_null; _temp156->f2= x; _temp156;}); _temp155;}); _temp153->tvs=
0; _temp153->fields=({ struct Cyc_Core_Opt* _temp154=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp154->v=( void*) _temp144;
_temp154;}); _temp153->attributes= 0; _temp153;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp148=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp148->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp149=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp149[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp150; _temp150.tag= Cyc_Absyn_Struct_d; _temp150.f1=
_temp147; _temp150;}); _temp149;}), 0); _temp148->tl= Cyc_Toc_result_decls;
_temp148;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp151=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp151->hd=( void*)({ struct
_tuple5* _temp152=( struct _tuple5*) _cycalloc( sizeof( struct _tuple5));
_temp152->f1= x; _temp152->f2= ts; _temp152;}); _temp151->tl= Cyc_Toc_tuple_types;
_temp151;}); return x;}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp160=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp160->f1=( void*) Cyc_Absyn_Loc_n; _temp160->f2=({ struct _tagged_arr*
_temp161=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp161[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct _temp163;
_temp163.tag= Cyc_Std_Int_pa; _temp163.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp162[ 1u]={& _temp163}; Cyc_Std_aprintf( _tag_arr("_temp%d",
sizeof( unsigned char), 8u), _tag_arr( _temp162, sizeof( void*), 1u));}});
_temp161;}); _temp160;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp164=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp164[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp166; _temp166.tag= Cyc_Std_Int_pa; _temp166.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp165[ 1u]={& _temp166}; Cyc_Std_aprintf( _tag_arr("_LL%d",
sizeof( unsigned char), 6u), _tag_arr( _temp165, sizeof( void*), 1u));}});
_temp164;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp169; struct Cyc_Absyn_Tqual
_temp171; struct Cyc_Core_Opt* _temp173; struct _tuple1 _temp167=* a; _LL174:
_temp173= _temp167.f1; goto _LL172; _LL172: _temp171= _temp167.f2; goto _LL170;
_LL170: _temp169= _temp167.f3; goto _LL168; _LL168: return({ struct _tuple1*
_temp175=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp175->f1=
_temp173; _temp175->f2= _temp171; _temp175->f3= Cyc_Toc_typ_to_c( _temp169);
_temp175;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp178; struct Cyc_Absyn_Tqual _temp180; struct _tuple3 _temp176=* x;
_LL181: _temp180= _temp176.f1; goto _LL179; _LL179: _temp178= _temp176.f2; goto
_LL177; _LL177: return({ struct _tuple3* _temp182=( struct _tuple3*) _cycalloc(
sizeof( struct _tuple3)); _temp182->f1= _temp180; _temp182->f2= Cyc_Toc_typ_to_c(
_temp178); _temp182;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp183= t; struct Cyc_Absyn_Exp* _temp191; struct Cyc_Absyn_Tqual _temp193;
void* _temp195; struct Cyc_Core_Opt* _temp197; struct Cyc_Core_Opt _temp199;
void* _temp200; _LL185: if(( unsigned int) _temp183 >  4u?*(( int*) _temp183) == 
Cyc_Absyn_ArrayType: 0){ _LL196: _temp195=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp183)->f1; goto _LL194; _LL194: _temp193=(( struct Cyc_Absyn_ArrayType_struct*)
_temp183)->f2; goto _LL192; _LL192: _temp191=(( struct Cyc_Absyn_ArrayType_struct*)
_temp183)->f3; goto _LL186;} else{ goto _LL187;} _LL187: if(( unsigned int)
_temp183 >  4u?*(( int*) _temp183) ==  Cyc_Absyn_Evar: 0){ _LL198: _temp197=((
struct Cyc_Absyn_Evar_struct*) _temp183)->f2; if( _temp197 ==  0){ goto _LL189;}
else{ _temp199=* _temp197; _LL201: _temp200=( void*) _temp199.v; goto _LL188;}}
else{ goto _LL189;} _LL189: goto _LL190; _LL186: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp202=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp202[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp203; _temp203.tag= Cyc_Absyn_ArrayType;
_temp203.f1=( void*) Cyc_Toc_typ_to_c_array( _temp195); _temp203.f2= _temp193;
_temp203.f3= _temp191; _temp203;}); _temp202;}); _LL188: return Cyc_Toc_typ_to_c_array(
_temp200); _LL190: return Cyc_Toc_typ_to_c( t); _LL184:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp204=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp204->name= f->name; _temp204->tq= f->tq; _temp204->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp204->width= f->width; _temp204->attributes= f->attributes;
_temp204;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp205= t; struct
Cyc_Core_Opt* _temp259; struct Cyc_Core_Opt* _temp261; struct Cyc_Core_Opt
_temp263; void* _temp264; struct Cyc_Absyn_TunionInfo _temp266; void* _temp268;
struct Cyc_Absyn_Tuniondecl* _temp270; struct Cyc_Absyn_TunionFieldInfo _temp272;
void* _temp274; struct Cyc_Absyn_Tunionfield* _temp276; struct Cyc_Absyn_Tuniondecl*
_temp278; struct Cyc_Absyn_PtrInfo _temp280; struct Cyc_Absyn_Conref* _temp282;
struct Cyc_Absyn_Tqual _temp284; void* _temp286; struct Cyc_Absyn_Exp* _temp288;
struct Cyc_Absyn_Tqual _temp290; void* _temp292; struct Cyc_Absyn_FnInfo
_temp294; struct Cyc_List_List* _temp296; struct Cyc_Absyn_VarargInfo* _temp298;
int _temp300; struct Cyc_List_List* _temp302; void* _temp304; struct Cyc_List_List*
_temp306; struct Cyc_List_List* _temp308; struct Cyc_List_List* _temp310; struct
_tuple0* _temp312; struct _tuple0* _temp314; struct _tuple0* _temp316; struct
Cyc_Core_Opt* _temp318; struct Cyc_List_List* _temp320; struct _tuple0* _temp322;
void* _temp324; _LL207: if( _temp205 == ( void*) Cyc_Absyn_VoidType){ goto
_LL208;} else{ goto _LL209;} _LL209: if(( unsigned int) _temp205 >  4u?*(( int*)
_temp205) ==  Cyc_Absyn_Evar: 0){ _LL260: _temp259=(( struct Cyc_Absyn_Evar_struct*)
_temp205)->f2; if( _temp259 ==  0){ goto _LL210;} else{ goto _LL211;}} else{
goto _LL211;} _LL211: if(( unsigned int) _temp205 >  4u?*(( int*) _temp205) == 
Cyc_Absyn_Evar: 0){ _LL262: _temp261=(( struct Cyc_Absyn_Evar_struct*) _temp205)->f2;
if( _temp261 ==  0){ goto _LL213;} else{ _temp263=* _temp261; _LL265: _temp264=(
void*) _temp263.v; goto _LL212;}} else{ goto _LL213;} _LL213: if(( unsigned int)
_temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_VarType: 0){ goto _LL214;}
else{ goto _LL215;} _LL215: if(( unsigned int) _temp205 >  4u?*(( int*) _temp205)
==  Cyc_Absyn_TunionType: 0){ _LL267: _temp266=(( struct Cyc_Absyn_TunionType_struct*)
_temp205)->f1; _LL269: _temp268=( void*) _temp266.tunion_info; if(*(( int*)
_temp268) ==  Cyc_Absyn_KnownTunion){ _LL271: _temp270=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp268)->f1; goto _LL216;} else{ goto _LL217;}} else{ goto _LL217;} _LL217:
if(( unsigned int) _temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_TunionType:
0){ goto _LL218;} else{ goto _LL219;} _LL219: if(( unsigned int) _temp205 >  4u?*((
int*) _temp205) ==  Cyc_Absyn_TunionFieldType: 0){ _LL273: _temp272=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp205)->f1; _LL275: _temp274=( void*)
_temp272.field_info; if(*(( int*) _temp274) ==  Cyc_Absyn_KnownTunionfield){
_LL279: _temp278=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp274)->f1;
goto _LL277; _LL277: _temp276=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp274)->f2; goto _LL220;} else{ goto _LL221;}} else{ goto _LL221;} _LL221:
if(( unsigned int) _temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp205 >  4u?*((
int*) _temp205) ==  Cyc_Absyn_PointerType: 0){ _LL281: _temp280=(( struct Cyc_Absyn_PointerType_struct*)
_temp205)->f1; _LL287: _temp286=( void*) _temp280.elt_typ; goto _LL285; _LL285:
_temp284= _temp280.tq; goto _LL283; _LL283: _temp282= _temp280.bounds; goto
_LL224;} else{ goto _LL225;} _LL225: if(( unsigned int) _temp205 >  4u?*(( int*)
_temp205) ==  Cyc_Absyn_IntType: 0){ goto _LL226;} else{ goto _LL227;} _LL227:
if( _temp205 == ( void*) Cyc_Absyn_FloatType){ goto _LL228;} else{ goto _LL229;}
_LL229: if( _temp205 == ( void*) Cyc_Absyn_DoubleType){ goto _LL230;} else{ goto
_LL231;} _LL231: if(( unsigned int) _temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_ArrayType:
0){ _LL293: _temp292=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp205)->f1;
goto _LL291; _LL291: _temp290=(( struct Cyc_Absyn_ArrayType_struct*) _temp205)->f2;
goto _LL289; _LL289: _temp288=(( struct Cyc_Absyn_ArrayType_struct*) _temp205)->f3;
goto _LL232;} else{ goto _LL233;} _LL233: if(( unsigned int) _temp205 >  4u?*((
int*) _temp205) ==  Cyc_Absyn_FnType: 0){ _LL295: _temp294=(( struct Cyc_Absyn_FnType_struct*)
_temp205)->f1; _LL305: _temp304=( void*) _temp294.ret_typ; goto _LL303; _LL303:
_temp302= _temp294.args; goto _LL301; _LL301: _temp300= _temp294.c_varargs; goto
_LL299; _LL299: _temp298= _temp294.cyc_varargs; goto _LL297; _LL297: _temp296=
_temp294.attributes; goto _LL234;} else{ goto _LL235;} _LL235: if(( unsigned int)
_temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_TupleType: 0){ _LL307: _temp306=((
struct Cyc_Absyn_TupleType_struct*) _temp205)->f1; goto _LL236;} else{ goto
_LL237;} _LL237: if(( unsigned int) _temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_AnonStructType:
0){ _LL309: _temp308=(( struct Cyc_Absyn_AnonStructType_struct*) _temp205)->f1;
goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int) _temp205 >  4u?*((
int*) _temp205) ==  Cyc_Absyn_AnonUnionType: 0){ _LL311: _temp310=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp205)->f1; goto _LL240;} else{ goto _LL241;} _LL241: if(( unsigned int)
_temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_UnionType: 0){ _LL313: _temp312=((
struct Cyc_Absyn_UnionType_struct*) _temp205)->f1; goto _LL242;} else{ goto
_LL243;} _LL243: if(( unsigned int) _temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_StructType:
0){ _LL315: _temp314=(( struct Cyc_Absyn_StructType_struct*) _temp205)->f1; goto
_LL244;} else{ goto _LL245;} _LL245: if(( unsigned int) _temp205 >  4u?*(( int*)
_temp205) ==  Cyc_Absyn_EnumType: 0){ _LL317: _temp316=(( struct Cyc_Absyn_EnumType_struct*)
_temp205)->f1; goto _LL246;} else{ goto _LL247;} _LL247: if(( unsigned int)
_temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_TypedefType: 0){ _LL323:
_temp322=(( struct Cyc_Absyn_TypedefType_struct*) _temp205)->f1; goto _LL321;
_LL321: _temp320=(( struct Cyc_Absyn_TypedefType_struct*) _temp205)->f2; goto
_LL319; _LL319: _temp318=(( struct Cyc_Absyn_TypedefType_struct*) _temp205)->f3;
goto _LL248;} else{ goto _LL249;} _LL249: if(( unsigned int) _temp205 >  4u?*((
int*) _temp205) ==  Cyc_Absyn_RgnHandleType: 0){ _LL325: _temp324=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp205)->f1; goto _LL250;} else{ goto
_LL251;} _LL251: if( _temp205 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL252;}
else{ goto _LL253;} _LL253: if(( unsigned int) _temp205 >  4u?*(( int*) _temp205)
==  Cyc_Absyn_AccessEff: 0){ goto _LL254;} else{ goto _LL255;} _LL255: if((
unsigned int) _temp205 >  4u?*(( int*) _temp205) ==  Cyc_Absyn_JoinEff: 0){ goto
_LL256;} else{ goto _LL257;} _LL257: if(( unsigned int) _temp205 >  4u?*(( int*)
_temp205) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL258;} else{ goto _LL206;} _LL208:
return t; _LL210: return Cyc_Absyn_sint_t; _LL212: return Cyc_Toc_typ_to_c(
_temp264); _LL214: return Cyc_Absyn_void_star_typ(); _LL216: return Cyc_Absyn_void_star_typ();
_LL218:( int) _throw(({ void* _temp326[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u), _tag_arr( _temp326, sizeof( void*), 0u));}));
_LL220: if( _temp276->typs ==  0){ if( _temp278->is_xtunion){ return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp276->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL222:( int) _throw(({ void*
_temp327[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u), _tag_arr( _temp327, sizeof( void*), 0u));}));
_LL224: _temp286= Cyc_Toc_typ_to_c_array( _temp286);{ void* _temp328=( void*)(
Cyc_Absyn_compress_conref( _temp282))->v; void* _temp334; _LL330: if((
unsigned int) _temp328 >  1u?*(( int*) _temp328) ==  Cyc_Absyn_Eq_constr: 0){
_LL335: _temp334=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp328)->f1;
if( _temp334 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL331;} else{ goto _LL332;}}
else{ goto _LL332;} _LL332: goto _LL333; _LL331: return Cyc_Toc_tagged_arr_typ;
_LL333: return Cyc_Absyn_star_typ( _temp286,( void*) Cyc_Absyn_HeapRgn, _temp284);
_LL329:;} _LL226: return t; _LL228: return t; _LL230: return t; _LL232: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp336=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp336[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp337; _temp337.tag= Cyc_Absyn_ArrayType; _temp337.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp292); _temp337.f2= _temp290; _temp337.f3=
_temp288; _temp337;}); _temp336;}); _LL234: { struct Cyc_List_List* _temp338= 0;
for( 0; _temp296 !=  0; _temp296=(( struct Cyc_List_List*) _check_null( _temp296))->tl){
void* _temp339=( void*)(( struct Cyc_List_List*) _check_null( _temp296))->hd;
_LL341: if( _temp339 == ( void*) Cyc_Absyn_Noreturn_att){ goto _LL342;} else{
goto _LL343;} _LL343: if( _temp339 == ( void*) Cyc_Absyn_Const_att){ goto _LL344;}
else{ goto _LL345;} _LL345: if(( unsigned int) _temp339 >  16u?*(( int*)
_temp339) ==  Cyc_Absyn_Format_att: 0){ goto _LL346;} else{ goto _LL347;} _LL347:
goto _LL348; _LL342: goto _LL344; _LL344: goto _LL346; _LL346: continue; _LL348:
_temp338=({ struct Cyc_List_List* _temp349=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp349->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( _temp296))->hd); _temp349->tl= _temp338; _temp349;}); goto _LL340;
_LL340:;}{ struct Cyc_List_List* _temp350=(( struct Cyc_List_List*(*)( struct
_tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp302); if( _temp298 !=  0){ void* _temp351= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp298))->type,( void*)
Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq)); struct _tuple1* _temp352=({ struct _tuple1*
_temp354=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp354->f1=((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp298))->name; _temp354->f2=((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp298))->tq; _temp354->f3=
_temp351; _temp354;}); _temp350=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp350,({ struct Cyc_List_List*
_temp353=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp353->hd=( void*) _temp352; _temp353->tl= 0; _temp353;}));} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp355=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp355[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp356; _temp356.tag= Cyc_Absyn_FnType; _temp356.f1=({ struct Cyc_Absyn_FnInfo
_temp357; _temp357.tvars= 0; _temp357.effect= 0; _temp357.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp304); _temp357.args= _temp350; _temp357.c_varargs= _temp300; _temp357.cyc_varargs=
0; _temp357.rgn_po= 0; _temp357.attributes= _temp338; _temp357;}); _temp356;});
_temp355;});}} _LL236: _temp306=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp306);{ struct _tagged_arr* _temp358= Cyc_Toc_add_tuple_type( _temp306);
return Cyc_Absyn_strct( _temp358);} _LL238: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp359=( struct Cyc_Absyn_AnonStructType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp359[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp360; _temp360.tag= Cyc_Absyn_AnonStructType; _temp360.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp308); _temp360;}); _temp359;});
_LL240: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp361=(
struct Cyc_Absyn_AnonUnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp361[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp362; _temp362.tag=
Cyc_Absyn_AnonUnionType; _temp362.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp310); _temp362;}); _temp361;}); _LL242: if( _temp312 ==  0){({ void*
_temp363[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("anonymous union", sizeof( unsigned char), 16u), _tag_arr( _temp363,
sizeof( void*), 0u));});} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp312)); _LL244: if( _temp314 ==  0){({ void* _temp364[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("anonymous struct",
sizeof( unsigned char), 17u), _tag_arr( _temp364, sizeof( void*), 0u));});}
return Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp314)); _LL246:
return t; _LL248: if( _temp318 ==  0){ return({ void* _temp365[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("Toc::typ_to_c: unresolved TypedefType", sizeof( unsigned char), 38u),
_tag_arr( _temp365, sizeof( void*), 0u));});} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp366=( struct Cyc_Absyn_TypedefType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp366[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp367; _temp367.tag= Cyc_Absyn_TypedefType;
_temp367.f1= _temp322; _temp367.f2= 0; _temp367.f3=({ struct Cyc_Core_Opt*
_temp368=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp368->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp318))->v); _temp368;}); _temp367;}); _temp366;}); _LL250:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL252: return({ void* _temp369[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u), _tag_arr( _temp369, sizeof( void*), 0u));});
_LL254: return({ void* _temp370[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u), _tag_arr( _temp370, sizeof( void*), 0u));});
_LL256: return({ void* _temp371[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u), _tag_arr( _temp371, sizeof( void*), 0u));});
_LL258: return({ void* _temp372[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof( unsigned char), 55u), _tag_arr( _temp372, sizeof( void*), 0u));});
_LL206:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void* _temp373= t;
struct Cyc_Absyn_Tqual _temp379; void* _temp381; _LL375: if(( unsigned int)
_temp373 >  4u?*(( int*) _temp373) ==  Cyc_Absyn_ArrayType: 0){ _LL382: _temp381=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp373)->f1; goto _LL380; _LL380:
_temp379=(( struct Cyc_Absyn_ArrayType_struct*) _temp373)->f2; goto _LL376;}
else{ goto _LL377;} _LL377: goto _LL378; _LL376: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp381,( void*) Cyc_Absyn_HeapRgn, _temp379), e, l); _LL378: return Cyc_Absyn_cast_exp(
t, e, l); _LL374:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp383= Cyc_Tcutil_compress(
t); void* _temp421; struct Cyc_Absyn_Structdecl** _temp423; struct Cyc_List_List*
_temp425; struct Cyc_List_List* _temp427; struct Cyc_Absyn_TunionFieldInfo
_temp429; void* _temp431; struct Cyc_Absyn_Tunionfield* _temp433; struct Cyc_Absyn_Tuniondecl*
_temp435; struct Cyc_List_List* _temp437; _LL385: if( _temp383 == ( void*) Cyc_Absyn_VoidType){
goto _LL386;} else{ goto _LL387;} _LL387: if(( unsigned int) _temp383 >  4u?*((
int*) _temp383) ==  Cyc_Absyn_VarType: 0){ goto _LL388;} else{ goto _LL389;}
_LL389: if(( unsigned int) _temp383 >  4u?*(( int*) _temp383) ==  Cyc_Absyn_IntType:
0){ goto _LL390;} else{ goto _LL391;} _LL391: if(( unsigned int) _temp383 >  4u?*((
int*) _temp383) ==  Cyc_Absyn_EnumType: 0){ goto _LL392;} else{ goto _LL393;}
_LL393: if( _temp383 == ( void*) Cyc_Absyn_FloatType){ goto _LL394;} else{ goto
_LL395;} _LL395: if( _temp383 == ( void*) Cyc_Absyn_DoubleType){ goto _LL396;}
else{ goto _LL397;} _LL397: if(( unsigned int) _temp383 >  4u?*(( int*) _temp383)
==  Cyc_Absyn_FnType: 0){ goto _LL398;} else{ goto _LL399;} _LL399: if((
unsigned int) _temp383 >  4u?*(( int*) _temp383) ==  Cyc_Absyn_ArrayType: 0){
_LL422: _temp421=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp383)->f1;
goto _LL400;} else{ goto _LL401;} _LL401: if(( unsigned int) _temp383 >  4u?*((
int*) _temp383) ==  Cyc_Absyn_StructType: 0){ _LL424: _temp423=(( struct Cyc_Absyn_StructType_struct*)
_temp383)->f3; goto _LL402;} else{ goto _LL403;} _LL403: if(( unsigned int)
_temp383 >  4u?*(( int*) _temp383) ==  Cyc_Absyn_AnonStructType: 0){ _LL426:
_temp425=(( struct Cyc_Absyn_AnonStructType_struct*) _temp383)->f1; goto _LL404;}
else{ goto _LL405;} _LL405: if(( unsigned int) _temp383 >  4u?*(( int*) _temp383)
==  Cyc_Absyn_AnonUnionType: 0){ _LL428: _temp427=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp383)->f1; goto _LL406;} else{ goto _LL407;} _LL407: if(( unsigned int)
_temp383 >  4u?*(( int*) _temp383) ==  Cyc_Absyn_TunionFieldType: 0){ _LL430:
_temp429=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp383)->f1; _LL432:
_temp431=( void*) _temp429.field_info; if(*(( int*) _temp431) ==  Cyc_Absyn_KnownTunionfield){
_LL436: _temp435=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp431)->f1;
goto _LL434; _LL434: _temp433=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp431)->f2; goto _LL408;} else{ goto _LL409;}} else{ goto _LL409;} _LL409:
if(( unsigned int) _temp383 >  4u?*(( int*) _temp383) ==  Cyc_Absyn_TupleType: 0){
_LL438: _temp437=(( struct Cyc_Absyn_TupleType_struct*) _temp383)->f1; goto
_LL410;} else{ goto _LL411;} _LL411: if(( unsigned int) _temp383 >  4u?*(( int*)
_temp383) ==  Cyc_Absyn_TunionType: 0){ goto _LL412;} else{ goto _LL413;} _LL413:
if(( unsigned int) _temp383 >  4u?*(( int*) _temp383) ==  Cyc_Absyn_UnionType: 0){
goto _LL414;} else{ goto _LL415;} _LL415: if(( unsigned int) _temp383 >  4u?*((
int*) _temp383) ==  Cyc_Absyn_PointerType: 0){ goto _LL416;} else{ goto _LL417;}
_LL417: if(( unsigned int) _temp383 >  4u?*(( int*) _temp383) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL418;} else{ goto _LL419;} _LL419: goto _LL420; _LL386: return 1;
_LL388: return 0; _LL390: return 1; _LL392: return 1; _LL394: return 1; _LL396:
return 1; _LL398: return 1; _LL400: return Cyc_Toc_atomic_typ( _temp421); _LL402:
if( _temp423 ==  0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp423)))->fields
==  0){ return 0;}{ struct Cyc_List_List* _temp439=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp423)))->fields))->v; for( 0; _temp439 !=  0; _temp439=((
struct Cyc_List_List*) _check_null( _temp439))->tl){ if( ! Cyc_Toc_atomic_typ((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp439))->hd)->type)){ return 0;}}} return 1; _LL404: _temp427= _temp425; goto
_LL406; _LL406: for( 0; _temp427 !=  0; _temp427=(( struct Cyc_List_List*)
_check_null( _temp427))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp427))->hd)->type)){ return 0;}} return
1; _LL408: _temp437= _temp433->typs; goto _LL410; _LL410: for( 0; _temp437 !=  0;
_temp437=(( struct Cyc_List_List*) _check_null( _temp437))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp437))->hd)).f2)){
return 0;}} return 1; _LL412: return 0; _LL414: return 0; _LL416: return 0;
_LL418: return 0; _LL420:({ struct Cyc_Std_String_pa_struct _temp441; _temp441.tag=
Cyc_Std_String_pa; _temp441.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp440[ 1u]={& _temp441};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u), _tag_arr( _temp440, sizeof( void*), 1u));}});
return 0; _LL384:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp442=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp448; void* _temp450;
_LL444: if(( unsigned int) _temp442 >  4u?*(( int*) _temp442) ==  Cyc_Absyn_PointerType:
0){ _LL449: _temp448=(( struct Cyc_Absyn_PointerType_struct*) _temp442)->f1;
_LL451: _temp450=( void*) _temp448.elt_typ; goto _LL445;} else{ goto _LL446;}
_LL446: goto _LL447; _LL445: { void* _temp452= Cyc_Tcutil_compress( _temp450);
_LL454: if( _temp452 == ( void*) Cyc_Absyn_VoidType){ goto _LL455;} else{ goto
_LL456;} _LL456: goto _LL457; _LL455: return 1; _LL457: return 0; _LL453:;}
_LL447: return 0; _LL443:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp458= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp470; struct Cyc_Absyn_Structdecl* _temp472; struct Cyc_List_List* _temp473;
_LL460: if(( unsigned int) _temp458 >  4u?*(( int*) _temp458) ==  Cyc_Absyn_StructType:
0){ _LL471: _temp470=(( struct Cyc_Absyn_StructType_struct*) _temp458)->f3; if(
_temp470 ==  0){ goto _LL462;} else{ _temp472=* _temp470; goto _LL461;}} else{
goto _LL462;} _LL462: if(( unsigned int) _temp458 >  4u?*(( int*) _temp458) == 
Cyc_Absyn_AnonStructType: 0){ _LL474: _temp473=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp458)->f1; goto _LL463;} else{ goto _LL464;} _LL464: if(( unsigned int)
_temp458 >  4u?*(( int*) _temp458) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL465;}
else{ goto _LL466;} _LL466: if(( unsigned int) _temp458 >  4u?*(( int*) _temp458)
==  Cyc_Absyn_UnionType: 0){ goto _LL467;} else{ goto _LL468;} _LL468: goto
_LL469; _LL461: if( _temp472->fields ==  0){( int) _throw(({ void* _temp475[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u), _tag_arr( _temp475, sizeof( void*), 0u));}));} _temp473=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp472->fields))->v;
goto _LL463; _LL463: { struct Cyc_Absyn_Structfield* _temp476= Cyc_Absyn_lookup_field(
_temp473, f); if( _temp476 ==  0){({ struct Cyc_Std_String_pa_struct _temp478;
_temp478.tag= Cyc_Std_String_pa; _temp478.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)
_check_null( _temp476))->name;{ void* _temp477[ 1u]={& _temp478};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_field: bad field %s",
sizeof( unsigned char), 28u), _tag_arr( _temp477, sizeof( void*), 1u));}});}
return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp476))->type);} _LL465: return 0; _LL467: return 0; _LL469:(
int) _throw(({ struct Cyc_Std_String_pa_struct _temp480; _temp480.tag= Cyc_Std_String_pa;
_temp480.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp479[ 1u]={&
_temp480}; Cyc_Toc_toc_impos( _tag_arr("is_poly_field: bad type %s", sizeof(
unsigned char), 27u), _tag_arr( _temp479, sizeof( void*), 1u));}})); _LL459:;}
static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp* e){ void* _temp481=(
void*) e->r; struct _tagged_arr* _temp489; struct Cyc_Absyn_Exp* _temp491;
struct _tagged_arr* _temp493; struct Cyc_Absyn_Exp* _temp495; _LL483: if(*(( int*)
_temp481) ==  Cyc_Absyn_StructMember_e){ _LL492: _temp491=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp481)->f1; goto _LL490; _LL490: _temp489=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp481)->f2; goto _LL484;} else{ goto _LL485;} _LL485: if(*(( int*) _temp481)
==  Cyc_Absyn_StructArrow_e){ _LL496: _temp495=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp481)->f1; goto _LL494; _LL494: _temp493=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp481)->f2; goto _LL486;} else{ goto _LL487;} _LL487: goto _LL488; _LL484:
return Cyc_Toc_is_poly_field(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp491->topt))->v, _temp489); _LL486: { void* _temp497= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp495->topt))->v); struct Cyc_Absyn_PtrInfo
_temp503; void* _temp505; _LL499: if(( unsigned int) _temp497 >  4u?*(( int*)
_temp497) ==  Cyc_Absyn_PointerType: 0){ _LL504: _temp503=(( struct Cyc_Absyn_PointerType_struct*)
_temp497)->f1; _LL506: _temp505=( void*) _temp503.elt_typ; goto _LL500;} else{
goto _LL501;} _LL501: goto _LL502; _LL500: return Cyc_Toc_is_poly_field(
_temp505, _temp493); _LL502:({ struct Cyc_Std_String_pa_struct _temp508;
_temp508.tag= Cyc_Std_String_pa; _temp508.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp495->topt))->v);{ void*
_temp507[ 1u]={& _temp508};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_project:  bad type %s", sizeof(
unsigned char), 30u), _tag_arr( _temp507, sizeof( void*), 1u));}}); return 0;
_LL498:;} _LL488: return 0; _LL482:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_e,({
struct Cyc_List_List* _temp509=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp509->hd=( void*) s; _temp509->tl= 0; _temp509;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc__cycalloc_atomic_e,({ struct Cyc_List_List*
_temp510=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp510->hd=( void*) s; _temp510->tl= 0; _temp510;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){
return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr( s);} static struct
Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__region_malloc_e,({ struct Cyc_Absyn_Exp*
_temp511[ 2u]; _temp511[ 1u]= s; _temp511[ 0u]= rgn;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp511, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List*
_temp512=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp512->hd=( void*) e; _temp512->tl= 0; _temp512;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr( void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp*
e){ int is_string= 0;{ void* _temp513=( void*) e->r; void* _temp519; _LL515: if(*((
int*) _temp513) ==  Cyc_Absyn_Const_e){ _LL520: _temp519=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp513)->f1; if(( unsigned int) _temp519 >  1u?*(( int*) _temp519) ==  Cyc_Absyn_String_c:
0){ goto _LL516;} else{ goto _LL517;}} else{ goto _LL517;} _LL517: goto _LL518;
_LL516: is_string= 1; goto _LL514; _LL518: goto _LL514; _LL514:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp524=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp524[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp525; _temp525.tag= Cyc_Absyn_ArrayType; _temp525.f1=(
void*) Cyc_Absyn_uchar_t; _temp525.f2= Cyc_Toc_mt_tq; _temp525.f3=( struct Cyc_Absyn_Exp*)
sz; _temp525;}); _temp524;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp521=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp521->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp522=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp522[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp523; _temp523.tag= Cyc_Absyn_Var_d;
_temp523.f1= vd; _temp523;}); _temp522;}), 0); _temp521->tl= Cyc_Toc_result_decls;
_temp521;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct _tuple4* _temp526[ 3u]; _temp526[ 2u]=({ struct _tuple4* _temp529=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp529->f1= 0; _temp529->f2=
xplussz; _temp529;}); _temp526[ 1u]=({ struct _tuple4* _temp528=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp528->f1= 0; _temp528->f2= xexp;
_temp528;}); _temp526[ 0u]=({ struct _tuple4* _temp527=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp527->f1= 0; _temp527->f2= xexp;
_temp527;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp526, sizeof( struct _tuple4*), 3u));}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp530=( struct Cyc_Toc_Env*) _cycalloc( sizeof( struct
Cyc_Toc_Env)); _temp530->break_lab= 0; _temp530->continue_lab= 0; _temp530->fallthru_info=
0; _temp530->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp530->toplevel= 1;
_temp530;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp531=( struct Cyc_Toc_Env*) _cycalloc( sizeof(
struct Cyc_Toc_Env)); _temp531->break_lab= e->break_lab; _temp531->continue_lab=
e->continue_lab; _temp531->fallthru_info= e->fallthru_info; _temp531->varmap= e->varmap;
_temp531->toplevel= e->toplevel; _temp531;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp532=(* x).f1; _LL534:
if(( unsigned int) _temp532 >  1u?*(( int*) _temp532) ==  Cyc_Absyn_Rel_n: 0){
goto _LL535;} else{ goto _LL536;} _LL536: goto _LL537; _LL535:( int) _throw(({
struct Cyc_Std_String_pa_struct _temp539; _temp539.tag= Cyc_Std_String_pa;
_temp539.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( x);{ void* _temp538[
1u]={& _temp539}; Cyc_Toc_toc_impos( _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp538, sizeof( void*), 1u));}}));
_LL537: goto _LL533; _LL533:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2;
struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l, struct _tagged_arr*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp540= 0; for( 0; fallthru_binders != 
0; fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp540=({ struct Cyc_List_List* _temp541=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp541->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp541->tl=
_temp540; _temp541;});} _temp540=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp540);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp542=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp542->v=( void*) break_l; _temp542;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp543=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp543->v=( void*)({ struct _tuple6*
_temp544=( struct _tuple6*) _cycalloc( sizeof( struct _tuple6)); _temp544->f1=
fallthru_l; _temp544->f2= _temp540; _temp544->f3= next_case_env->varmap;
_temp544;}); _temp543;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp545=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp545->v=( void*)
break_l; _temp545;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp546=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp546->v=( void*)({ struct _tuple6* _temp547=( struct _tuple6*)
_cycalloc( sizeof( struct _tuple6)); _temp547->f1= next_l; _temp547->f2= 0;
_temp547->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp547;}); _temp546;});
return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt ==  0){({
void* _temp548[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp548, sizeof( void*), 0u));});} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt ==  0){({ void* _temp549[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp549, sizeof( void*), 0u));});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct
_tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple3*
_temp550=( struct _tuple3*) _cycalloc( sizeof( struct _tuple3)); _temp550->f1=
Cyc_Toc_mt_tq; _temp550->f2= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _temp550;});} static struct _tuple4* Cyc_Toc_add_designator(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e);
return({ struct _tuple4* _temp551=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp551->f1= 0; _temp551->f2= e; _temp551;});} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct _tuple0* x, void* struct_typ,
struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int
is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp552= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt ==  0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp552): Cyc_Toc_malloc_ptr( _temp552), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp552), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) -  1;{ struct Cyc_List_List* _temp553=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp553 !=  0; _temp553=((
struct Cyc_List_List*) _check_null( _temp553))->tl){ struct _tuple4 _temp556;
struct Cyc_Absyn_Exp* _temp557; struct Cyc_List_List* _temp559; struct _tuple4*
_temp554=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp553))->hd;
_temp556=* _temp554; _LL560: _temp559= _temp556.f1; goto _LL558; _LL558:
_temp557= _temp556.f2; goto _LL555; _LL555: { struct Cyc_Absyn_Exp* e_index; if(
_temp559 ==  0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp559))->tl !=  0){({ void* _temp561[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp561, sizeof( void*), 0u));});}{
void* _temp562=( void*)(( struct Cyc_List_List*) _check_null( _temp559))->hd;
void* _temp563= _temp562; struct Cyc_Absyn_Exp* _temp569; _LL565: if(*(( int*)
_temp563) ==  Cyc_Absyn_ArrayElement){ _LL570: _temp569=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp563)->f1; goto _LL566;} else{ goto _LL567;} _LL567: if(*(( int*) _temp563)
==  Cyc_Absyn_FieldName){ goto _LL568;} else{ goto _LL564;} _LL566: Cyc_Toc_exp_to_c(
nv, _temp569); e_index= _temp569; goto _LL564; _LL568: e_index=({ void* _temp571[
0u]={};(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_unimp)( _tag_arr("field name designators in array", sizeof(
unsigned char), 32u), _tag_arr( _temp571, sizeof( void*), 0u));}); goto _LL564;
_LL564:;}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0);
void* _temp572=( void*) _temp557->r; struct Cyc_List_List* _temp582; struct Cyc_Absyn_Exp*
_temp584; struct Cyc_Absyn_Exp* _temp586; struct Cyc_Absyn_Vardecl* _temp588;
struct Cyc_List_List* _temp590; void* _temp592; _LL574: if(*(( int*) _temp572)
==  Cyc_Absyn_Array_e){ _LL583: _temp582=(( struct Cyc_Absyn_Array_e_struct*)
_temp572)->f1; goto _LL575;} else{ goto _LL576;} _LL576: if(*(( int*) _temp572)
==  Cyc_Absyn_Comprehension_e){ _LL589: _temp588=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp572)->f1; goto _LL587; _LL587: _temp586=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp572)->f2; goto _LL585; _LL585: _temp584=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp572)->f3; goto _LL577;} else{ goto _LL578;} _LL578: if(*(( int*) _temp572)
==  Cyc_Absyn_AnonStruct_e){ _LL593: _temp592=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp572)->f1; goto _LL591; _LL591: _temp590=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp572)->f2; goto _LL579;} else{ goto _LL580;} _LL580: goto _LL581; _LL575: s=
Cyc_Toc_init_array( nv, lval, _temp582, s); goto _LL573; _LL577: s= Cyc_Toc_init_comprehension(
nv, lval, _temp588, _temp586, _temp584, s, 0); goto _LL573; _LL579: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp592, _temp590, s); goto _LL573; _LL581: Cyc_Toc_exp_to_c( nv,
_temp557); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp557, 0), s, 0); goto _LL573; _LL573:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp594= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp594, Cyc_Absyn_varb_exp( _temp594,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp617=( struct Cyc_Absyn_Local_b_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp617[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp618; _temp618.tag= Cyc_Absyn_Local_b; _temp618.f1= vd; _temp618;});
_temp617;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp594, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp594, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp594, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp594, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp595=( void*) e2->r; struct Cyc_List_List* _temp605; struct Cyc_Absyn_Exp*
_temp607; struct Cyc_Absyn_Exp* _temp609; struct Cyc_Absyn_Vardecl* _temp611;
struct Cyc_List_List* _temp613; void* _temp615; _LL597: if(*(( int*) _temp595)
==  Cyc_Absyn_Array_e){ _LL606: _temp605=(( struct Cyc_Absyn_Array_e_struct*)
_temp595)->f1; goto _LL598;} else{ goto _LL599;} _LL599: if(*(( int*) _temp595)
==  Cyc_Absyn_Comprehension_e){ _LL612: _temp611=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp595)->f1; goto _LL610; _LL610: _temp609=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp595)->f2; goto _LL608; _LL608: _temp607=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp595)->f3; goto _LL600;} else{ goto _LL601;} _LL601: if(*(( int*) _temp595)
==  Cyc_Absyn_AnonStruct_e){ _LL616: _temp615=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp595)->f1; goto _LL614; _LL614: _temp613=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp595)->f2; goto _LL602;} else{ goto _LL603;} _LL603: goto _LL604; _LL598:
body= Cyc_Toc_init_array( nv2, lval, _temp605, Cyc_Absyn_skip_stmt( 0)); goto
_LL596; _LL600: body= Cyc_Toc_init_comprehension( nv2, lval, _temp611, _temp609,
_temp607, Cyc_Absyn_skip_stmt( 0), 0); goto _LL596; _LL602: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp615, _temp613, Cyc_Absyn_skip_stmt( 0)); goto _LL596; _LL604: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL596; _LL596:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp594, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp619=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp619 !=  0; _temp619=(( struct Cyc_List_List*)
_check_null( _temp619))->tl){ struct _tuple4 _temp622; struct Cyc_Absyn_Exp*
_temp623; struct Cyc_List_List* _temp625; struct _tuple4* _temp620=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp619))->hd; _temp622=*
_temp620; _LL626: _temp625= _temp622.f1; goto _LL624; _LL624: _temp623= _temp622.f2;
goto _LL621; _LL621: if( _temp625 ==  0){({ void* _temp627[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp627, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp625))->tl !=  0){({ void* _temp628[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct", sizeof( unsigned char), 41u),
_tag_arr( _temp628, sizeof( void*), 0u));});}{ void* _temp629=( void*)(( struct
Cyc_List_List*) _check_null( _temp625))->hd; struct _tagged_arr* _temp635;
_LL631: if(*(( int*) _temp629) ==  Cyc_Absyn_FieldName){ _LL636: _temp635=((
struct Cyc_Absyn_FieldName_struct*) _temp629)->f1; goto _LL632;} else{ goto
_LL633;} _LL633: goto _LL634; _LL632: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp(
lhs, _temp635, 0);{ void* _temp637=( void*) _temp623->r; struct Cyc_List_List*
_temp647; struct Cyc_Absyn_Exp* _temp649; struct Cyc_Absyn_Exp* _temp651; struct
Cyc_Absyn_Vardecl* _temp653; struct Cyc_List_List* _temp655; void* _temp657;
_LL639: if(*(( int*) _temp637) ==  Cyc_Absyn_Array_e){ _LL648: _temp647=((
struct Cyc_Absyn_Array_e_struct*) _temp637)->f1; goto _LL640;} else{ goto _LL641;}
_LL641: if(*(( int*) _temp637) ==  Cyc_Absyn_Comprehension_e){ _LL654: _temp653=((
struct Cyc_Absyn_Comprehension_e_struct*) _temp637)->f1; goto _LL652; _LL652:
_temp651=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp637)->f2; goto _LL650;
_LL650: _temp649=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp637)->f3;
goto _LL642;} else{ goto _LL643;} _LL643: if(*(( int*) _temp637) ==  Cyc_Absyn_AnonStruct_e){
_LL658: _temp657=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp637)->f1;
goto _LL656; _LL656: _temp655=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp637)->f2;
goto _LL644;} else{ goto _LL645;} _LL645: goto _LL646; _LL640: s= Cyc_Toc_init_array(
nv, lval, _temp647, s); goto _LL638; _LL642: s= Cyc_Toc_init_comprehension( nv,
lval, _temp653, _temp651, _temp649, s, 0); goto _LL638; _LL644: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp657, _temp655, s); goto _LL638; _LL646: Cyc_Toc_exp_to_c( nv,
_temp623); if( Cyc_Toc_is_poly_field( struct_type, _temp635)){ _temp623= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp623, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp623, 0), 0), s, 0); goto _LL638; _LL638:;} goto
_LL630;} _LL634:( int) _throw(({ void* _temp659[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp659, sizeof( void*), 0u));})); _LL630:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp660= _new_region();
struct _RegionHandle* r=& _temp660; _push_region( r);{ struct Cyc_List_List*
_temp661=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp662= Cyc_Toc_add_tuple_type( _temp661); struct
_tuple0* _temp663= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp664= Cyc_Absyn_var_exp(
_temp663, 0); struct Cyc_Absyn_Stmt* _temp665= Cyc_Absyn_exp_stmt( _temp664, 0);
struct Cyc_Absyn_Exp*(* _temp666)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp667=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp667); for( 0; _temp667
!=  0;( _temp667=(( struct Cyc_List_List*) _check_null( _temp667))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp667))->hd; struct Cyc_Absyn_Exp* lval= _temp666( _temp664, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp668=( void*) e->r; struct Cyc_List_List*
_temp676; struct Cyc_Absyn_Exp* _temp678; struct Cyc_Absyn_Exp* _temp680; struct
Cyc_Absyn_Vardecl* _temp682; _LL670: if(*(( int*) _temp668) ==  Cyc_Absyn_Array_e){
_LL677: _temp676=(( struct Cyc_Absyn_Array_e_struct*) _temp668)->f1; goto _LL671;}
else{ goto _LL672;} _LL672: if(*(( int*) _temp668) ==  Cyc_Absyn_Comprehension_e){
_LL683: _temp682=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp668)->f1;
goto _LL681; _LL681: _temp680=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp668)->f2; goto _LL679; _LL679: _temp678=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp668)->f3; goto _LL673;} else{ goto _LL674;} _LL674: goto _LL675; _LL671:
_temp665= Cyc_Toc_init_array( nv, lval, _temp676, _temp665); goto _LL669; _LL673:
_temp665= Cyc_Toc_init_comprehension( nv, lval, _temp682, _temp680, _temp678,
_temp665, 0); goto _LL669; _LL675: Cyc_Toc_exp_to_c( nv, e); _temp665= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp666( _temp664, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp665, 0); goto _LL669; _LL669:;}}}{ struct Cyc_Absyn_Exp*
_temp684= Cyc_Toc_make_struct( nv, _temp663, Cyc_Absyn_strct( _temp662),
_temp665, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp684;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp685= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp686= Cyc_Absyn_var_exp( _temp685, 0); struct Cyc_Absyn_Stmt*
_temp687= Cyc_Absyn_exp_stmt( _temp686, 0); struct Cyc_Absyn_Exp*(* _temp688)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp689= _new_region(); struct _RegionHandle* r=& _temp689;
_push_region( r);{ struct Cyc_List_List* _temp690=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp690 !=  0; _temp690=(( struct Cyc_List_List*) _check_null( _temp690))->tl){
struct _tuple4 _temp693; struct Cyc_Absyn_Exp* _temp694; struct Cyc_List_List*
_temp696; struct _tuple4* _temp691=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp690))->hd; _temp693=* _temp691; _LL697: _temp696= _temp693.f1;
goto _LL695; _LL695: _temp694= _temp693.f2; goto _LL692; _LL692: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp694->topt))->v): 0; if( _temp696 ==  0){({ void* _temp698[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp698, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp696))->tl !=  0){ struct _tuple0*
_temp699= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp700= Cyc_Absyn_var_exp(
_temp699, 0); for( 0; _temp696 !=  0; _temp696=(( struct Cyc_List_List*)
_check_null( _temp696))->tl){ void* _temp701=( void*)(( struct Cyc_List_List*)
_check_null( _temp696))->hd; struct _tagged_arr* _temp707; _LL703: if(*(( int*)
_temp701) ==  Cyc_Absyn_FieldName){ _LL708: _temp707=(( struct Cyc_Absyn_FieldName_struct*)
_temp701)->f1; goto _LL704;} else{ goto _LL705;} _LL705: goto _LL706; _LL704:
if( Cyc_Toc_is_poly_field( struct_type, _temp707)){ _temp700= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp700, 0);} _temp687= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp688( _temp686, _temp707, 0), _temp700, 0), 0),
_temp687, 0); goto _LL702; _LL706:( int) _throw(({ void* _temp709[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp709, sizeof( void*), 0u));})); _LL702:;} Cyc_Toc_exp_to_c( nv, _temp694);
_temp687= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp700,
_temp694, 0), 0), _temp687, 0);} else{ void* _temp710=( void*)(( struct Cyc_List_List*)
_check_null( _temp696))->hd; struct _tagged_arr* _temp716; _LL712: if(*(( int*)
_temp710) ==  Cyc_Absyn_FieldName){ _LL717: _temp716=(( struct Cyc_Absyn_FieldName_struct*)
_temp710)->f1; goto _LL713;} else{ goto _LL714;} _LL714: goto _LL715; _LL713: {
struct Cyc_Absyn_Exp* lval= _temp688( _temp686, _temp716, 0);{ void* _temp718=(
void*) _temp694->r; struct Cyc_List_List* _temp728; struct Cyc_Absyn_Exp*
_temp730; struct Cyc_Absyn_Exp* _temp732; struct Cyc_Absyn_Vardecl* _temp734;
struct Cyc_List_List* _temp736; void* _temp738; _LL720: if(*(( int*) _temp718)
==  Cyc_Absyn_Array_e){ _LL729: _temp728=(( struct Cyc_Absyn_Array_e_struct*)
_temp718)->f1; goto _LL721;} else{ goto _LL722;} _LL722: if(*(( int*) _temp718)
==  Cyc_Absyn_Comprehension_e){ _LL735: _temp734=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp718)->f1; goto _LL733; _LL733: _temp732=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp718)->f2; goto _LL731; _LL731: _temp730=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp718)->f3; goto _LL723;} else{ goto _LL724;} _LL724: if(*(( int*) _temp718)
==  Cyc_Absyn_AnonStruct_e){ _LL739: _temp738=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp718)->f1; goto _LL737; _LL737: _temp736=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp718)->f2; goto _LL725;} else{ goto _LL726;} _LL726: goto _LL727; _LL721:
_temp687= Cyc_Toc_init_array( nv, lval, _temp728, _temp687); goto _LL719; _LL723:
_temp687= Cyc_Toc_init_comprehension( nv, lval, _temp734, _temp732, _temp730,
_temp687, 0); goto _LL719; _LL725: _temp687= Cyc_Toc_init_anon_struct( nv, lval,
_temp738, _temp736, _temp687); goto _LL719; _LL727: Cyc_Toc_exp_to_c( nv,
_temp694); if( Cyc_Toc_is_poly_field( struct_type, _temp716)){ _temp694= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp694, 0);} _temp687= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp694, 0), 0), _temp687, 0); goto _LL719; _LL719:;}
goto _LL711;} _LL715:( int) _throw(({ void* _temp740[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp740, sizeof( void*), 0u));})); _LL711:;}}}{ struct Cyc_Absyn_Exp* _temp741=
Cyc_Toc_make_struct( nv, _temp685, Cyc_Absyn_strctq( tdn), _temp687, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp741;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp742=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp742[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp743; _temp743.tag= Cyc_Absyn_Increment_e;
_temp743.f1= e1; _temp743.f2=( void*) incr; _temp743;}); _temp742;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp744=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp754; struct Cyc_Absyn_Exp*
_temp756; void* _temp758; struct _tagged_arr* _temp760; struct Cyc_Absyn_Exp*
_temp762; _LL746: if(*(( int*) _temp744) ==  Cyc_Absyn_StmtExp_e){ _LL755:
_temp754=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp744)->f1; goto _LL747;}
else{ goto _LL748;} _LL748: if(*(( int*) _temp744) ==  Cyc_Absyn_Cast_e){ _LL759:
_temp758=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp744)->f1; goto _LL757;
_LL757: _temp756=(( struct Cyc_Absyn_Cast_e_struct*) _temp744)->f2; goto _LL749;}
else{ goto _LL750;} _LL750: if(*(( int*) _temp744) ==  Cyc_Absyn_StructMember_e){
_LL763: _temp762=(( struct Cyc_Absyn_StructMember_e_struct*) _temp744)->f1; goto
_LL761; _LL761: _temp760=(( struct Cyc_Absyn_StructMember_e_struct*) _temp744)->f2;
goto _LL751;} else{ goto _LL752;} _LL752: goto _LL753; _LL747: Cyc_Toc_lvalue_assign_stmt(
_temp754, fs, f, f_env); goto _LL745; _LL749: Cyc_Toc_lvalue_assign( _temp756,
fs, f, f_env); goto _LL745; _LL751:( void*)( e1->r=( void*)(( void*) _temp762->r));
Cyc_Toc_lvalue_assign( e1,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp764=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp764->hd=( void*) _temp760; _temp764->tl= fs; _temp764;}), f, f_env); goto
_LL745; _LL753: if( ! Cyc_Absyn_is_lvalue( e1)){({ struct Cyc_Std_String_pa_struct
_temp766; _temp766.tag= Cyc_Std_String_pa; _temp766.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string(
e1);{ void* _temp765[ 1u]={& _temp766};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u), _tag_arr( _temp765, sizeof( void*), 1u));}});}{
struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs !=  0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd,
e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto
_LL745;} _LL745:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp767=( void*) s->r; struct Cyc_Absyn_Exp*
_temp777; struct Cyc_Absyn_Stmt* _temp779; struct Cyc_Absyn_Decl* _temp781;
struct Cyc_Absyn_Stmt* _temp783; _LL769: if(( unsigned int) _temp767 >  1u?*((
int*) _temp767) ==  Cyc_Absyn_Exp_s: 0){ _LL778: _temp777=(( struct Cyc_Absyn_Exp_s_struct*)
_temp767)->f1; goto _LL770;} else{ goto _LL771;} _LL771: if(( unsigned int)
_temp767 >  1u?*(( int*) _temp767) ==  Cyc_Absyn_Decl_s: 0){ _LL782: _temp781=((
struct Cyc_Absyn_Decl_s_struct*) _temp767)->f1; goto _LL780; _LL780: _temp779=((
struct Cyc_Absyn_Decl_s_struct*) _temp767)->f2; goto _LL772;} else{ goto _LL773;}
_LL773: if(( unsigned int) _temp767 >  1u?*(( int*) _temp767) ==  Cyc_Absyn_Seq_s:
0){ _LL784: _temp783=(( struct Cyc_Absyn_Seq_s_struct*) _temp767)->f2; goto
_LL774;} else{ goto _LL775;} _LL775: goto _LL776; _LL770: Cyc_Toc_lvalue_assign(
_temp777, fs, f, f_env); goto _LL768; _LL772: Cyc_Toc_lvalue_assign_stmt(
_temp779, fs, f, f_env); goto _LL768; _LL774: Cyc_Toc_lvalue_assign_stmt(
_temp783, fs, f, f_env); goto _LL768; _LL776:({ struct Cyc_Std_String_pa_struct
_temp786; _temp786.tag= Cyc_Std_String_pa; _temp786.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string(
s);{ void* _temp785[ 1u]={& _temp786};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign_stmt: %s", sizeof(
unsigned char), 23u), _tag_arr( _temp785, sizeof( void*), 1u));}}); goto _LL768;
_LL768:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct _RegionHandle* r2,
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x ==  0){ return 0;} result=({ struct
Cyc_List_List* _temp787=( struct Cyc_List_List*) _region_malloc( r2, sizeof(
struct Cyc_List_List)); _temp787->hd=( void*) f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd, env); _temp787->tl= 0; _temp787;}); prev= result; for( x=((
struct Cyc_List_List*) _check_null( x))->tl; x !=  0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){(( struct Cyc_List_List*) _check_null( prev))->tl=({
struct Cyc_List_List* _temp788=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp788->hd=( void*) f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd, env); _temp788->tl= 0; _temp788;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl;} return result;} static struct Cyc_List_List* Cyc_Toc_map_2c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region, f, env, x);} static struct _tuple4* Cyc_Toc_make_dle(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple4* _temp789=( struct _tuple4*)
_cycalloc( sizeof( struct _tuple4)); _temp789->f1= 0; _temp789->f2= e; _temp789;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type( void* t){ void* _temp790=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp796; _LL792: if((
unsigned int) _temp790 >  4u?*(( int*) _temp790) ==  Cyc_Absyn_PointerType: 0){
_LL797: _temp796=(( struct Cyc_Absyn_PointerType_struct*) _temp790)->f1; goto
_LL793;} else{ goto _LL794;} _LL794: goto _LL795; _LL793: return _temp796;
_LL795:( int) _throw(({ void* _temp798[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("get_ptr_typ: not a pointer!",
sizeof( unsigned char), 28u), _tag_arr( _temp798, sizeof( void*), 0u));}));
_LL791:;} struct _tuple8{ void* f1; void* f2; } ; struct _tuple9{ struct _tuple0*
f1; void* f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple10{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp799=( void*) e->r; if( e->topt ==  0){({
struct Cyc_Std_String_pa_struct _temp801; _temp801.tag= Cyc_Std_String_pa;
_temp801.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp800[ 1u]={&
_temp801};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: no type for %s", sizeof( unsigned char), 25u), _tag_arr(
_temp800, sizeof( void*), 1u));}}); return;}{ void* old_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v; void* _temp802= _temp799; void*
_temp882; void* _temp884; struct _tuple0* _temp886; struct _tuple0* _temp888;
struct Cyc_List_List* _temp890; void* _temp892; void* _temp894; struct Cyc_Absyn_Exp*
_temp896; struct Cyc_Absyn_Exp* _temp898; struct Cyc_Core_Opt* _temp900; struct
Cyc_Absyn_Exp* _temp902; struct Cyc_Absyn_Exp* _temp904; struct Cyc_Absyn_Exp*
_temp906; struct Cyc_Absyn_Exp* _temp908; struct Cyc_Absyn_Exp* _temp910; struct
Cyc_Absyn_Exp* _temp912; struct Cyc_List_List* _temp914; struct Cyc_Absyn_Exp*
_temp916; struct Cyc_Absyn_VarargCallInfo* _temp918; struct Cyc_List_List*
_temp920; struct Cyc_Absyn_Exp* _temp922; struct Cyc_Absyn_VarargCallInfo*
_temp924; struct Cyc_Absyn_VarargCallInfo _temp926; struct Cyc_Absyn_VarargInfo*
_temp927; struct Cyc_List_List* _temp929; int _temp931; struct Cyc_List_List*
_temp933; struct Cyc_Absyn_Exp* _temp935; struct Cyc_Absyn_Exp* _temp937; struct
Cyc_Absyn_Exp* _temp939; struct Cyc_List_List* _temp941; struct Cyc_Absyn_Exp*
_temp943; struct Cyc_Absyn_Exp* _temp945; void* _temp947; void** _temp949;
struct Cyc_Absyn_Exp* _temp950; struct Cyc_Absyn_Exp* _temp952; struct Cyc_Absyn_Exp*
_temp954; struct Cyc_Absyn_Exp* _temp956; void* _temp958; struct _tagged_arr*
_temp960; void* _temp962; struct Cyc_Absyn_Exp* _temp964; struct _tagged_arr*
_temp966; struct Cyc_Absyn_Exp* _temp968; struct _tagged_arr* _temp970; struct
Cyc_Absyn_Exp* _temp972; struct Cyc_Absyn_Exp* _temp974; struct Cyc_Absyn_Exp*
_temp976; struct Cyc_List_List* _temp978; struct Cyc_List_List* _temp980; struct
Cyc_Absyn_Exp* _temp982; struct Cyc_Absyn_Exp* _temp984; struct Cyc_Absyn_Vardecl*
_temp986; struct Cyc_Absyn_Structdecl* _temp988; struct Cyc_List_List* _temp990;
struct Cyc_Core_Opt* _temp992; struct _tuple0* _temp994; struct Cyc_List_List*
_temp996; void* _temp998; struct Cyc_Absyn_Tunionfield* _temp1000; struct Cyc_Absyn_Tuniondecl*
_temp1002; struct Cyc_List_List* _temp1004; struct Cyc_Absyn_Tunionfield*
_temp1006; struct Cyc_Absyn_Tuniondecl* _temp1008; struct Cyc_List_List*
_temp1010; void* _temp1012; struct Cyc_Absyn_Exp* _temp1014; struct Cyc_Absyn_Stmt*
_temp1016; struct Cyc_Absyn_Fndecl* _temp1018; _LL804: if(*(( int*) _temp802) == 
Cyc_Absyn_Const_e){ _LL883: _temp882=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp802)->f1; if( _temp882 == ( void*) Cyc_Absyn_Null_c){ goto _LL805;} else{
goto _LL806;}} else{ goto _LL806;} _LL806: if(*(( int*) _temp802) ==  Cyc_Absyn_Const_e){
goto _LL807;} else{ goto _LL808;} _LL808: if(*(( int*) _temp802) ==  Cyc_Absyn_Var_e){
_LL887: _temp886=(( struct Cyc_Absyn_Var_e_struct*) _temp802)->f1; goto _LL885;
_LL885: _temp884=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp802)->f2; goto
_LL809;} else{ goto _LL810;} _LL810: if(*(( int*) _temp802) ==  Cyc_Absyn_UnknownId_e){
_LL889: _temp888=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp802)->f1; goto
_LL811;} else{ goto _LL812;} _LL812: if(*(( int*) _temp802) ==  Cyc_Absyn_Primop_e){
_LL893: _temp892=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp802)->f1;
goto _LL891; _LL891: _temp890=(( struct Cyc_Absyn_Primop_e_struct*) _temp802)->f2;
goto _LL813;} else{ goto _LL814;} _LL814: if(*(( int*) _temp802) ==  Cyc_Absyn_Increment_e){
_LL897: _temp896=(( struct Cyc_Absyn_Increment_e_struct*) _temp802)->f1; goto
_LL895; _LL895: _temp894=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp802)->f2; goto _LL815;} else{ goto _LL816;} _LL816: if(*(( int*) _temp802)
==  Cyc_Absyn_AssignOp_e){ _LL903: _temp902=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp802)->f1; goto _LL901; _LL901: _temp900=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp802)->f2; goto _LL899; _LL899: _temp898=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp802)->f3; goto _LL817;} else{ goto _LL818;} _LL818: if(*(( int*) _temp802)
==  Cyc_Absyn_Conditional_e){ _LL909: _temp908=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp802)->f1; goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp802)->f2; goto _LL905; _LL905: _temp904=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp802)->f3; goto _LL819;} else{ goto _LL820;} _LL820: if(*(( int*) _temp802)
==  Cyc_Absyn_SeqExp_e){ _LL913: _temp912=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp802)->f1; goto _LL911; _LL911: _temp910=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp802)->f2; goto _LL821;} else{ goto _LL822;} _LL822: if(*(( int*) _temp802)
==  Cyc_Absyn_UnknownCall_e){ _LL917: _temp916=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp802)->f1; goto _LL915; _LL915: _temp914=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp802)->f2; goto _LL823;} else{ goto _LL824;} _LL824: if(*(( int*) _temp802)
==  Cyc_Absyn_FnCall_e){ _LL923: _temp922=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp802)->f1; goto _LL921; _LL921: _temp920=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp802)->f2; goto _LL919; _LL919: _temp918=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp802)->f3; if( _temp918 ==  0){ goto _LL825;} else{ goto _LL826;}} else{
goto _LL826;} _LL826: if(*(( int*) _temp802) ==  Cyc_Absyn_FnCall_e){ _LL936:
_temp935=(( struct Cyc_Absyn_FnCall_e_struct*) _temp802)->f1; goto _LL934;
_LL934: _temp933=(( struct Cyc_Absyn_FnCall_e_struct*) _temp802)->f2; goto
_LL925; _LL925: _temp924=(( struct Cyc_Absyn_FnCall_e_struct*) _temp802)->f3;
if( _temp924 ==  0){ goto _LL828;} else{ _temp926=* _temp924; _LL932: _temp931=
_temp926.num_varargs; goto _LL930; _LL930: _temp929= _temp926.injectors; goto
_LL928; _LL928: _temp927= _temp926.vai; goto _LL827;}} else{ goto _LL828;}
_LL828: if(*(( int*) _temp802) ==  Cyc_Absyn_Throw_e){ _LL938: _temp937=((
struct Cyc_Absyn_Throw_e_struct*) _temp802)->f1; goto _LL829;} else{ goto _LL830;}
_LL830: if(*(( int*) _temp802) ==  Cyc_Absyn_NoInstantiate_e){ _LL940: _temp939=((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp802)->f1; goto _LL831;} else{
goto _LL832;} _LL832: if(*(( int*) _temp802) ==  Cyc_Absyn_Instantiate_e){
_LL944: _temp943=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp802)->f1; goto
_LL942; _LL942: _temp941=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp802)->f2;
goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp802) ==  Cyc_Absyn_Cast_e){
_LL948: _temp947=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp802)->f1;
_temp949=( void**)&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp802)->f1);
goto _LL946; _LL946: _temp945=(( struct Cyc_Absyn_Cast_e_struct*) _temp802)->f2;
goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp802) ==  Cyc_Absyn_Address_e){
_LL951: _temp950=(( struct Cyc_Absyn_Address_e_struct*) _temp802)->f1; goto
_LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp802) ==  Cyc_Absyn_New_e){
_LL955: _temp954=(( struct Cyc_Absyn_New_e_struct*) _temp802)->f1; goto _LL953;
_LL953: _temp952=(( struct Cyc_Absyn_New_e_struct*) _temp802)->f2; goto _LL839;}
else{ goto _LL840;} _LL840: if(*(( int*) _temp802) ==  Cyc_Absyn_Sizeofexp_e){
_LL957: _temp956=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp802)->f1; goto
_LL841;} else{ goto _LL842;} _LL842: if(*(( int*) _temp802) ==  Cyc_Absyn_Sizeoftyp_e){
_LL959: _temp958=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp802)->f1;
goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp802) ==  Cyc_Absyn_Offsetof_e){
_LL963: _temp962=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp802)->f1;
goto _LL961; _LL961: _temp960=(( struct Cyc_Absyn_Offsetof_e_struct*) _temp802)->f2;
goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*) _temp802) ==  Cyc_Absyn_Deref_e){
_LL965: _temp964=(( struct Cyc_Absyn_Deref_e_struct*) _temp802)->f1; goto _LL847;}
else{ goto _LL848;} _LL848: if(*(( int*) _temp802) ==  Cyc_Absyn_StructMember_e){
_LL969: _temp968=(( struct Cyc_Absyn_StructMember_e_struct*) _temp802)->f1; goto
_LL967; _LL967: _temp966=(( struct Cyc_Absyn_StructMember_e_struct*) _temp802)->f2;
goto _LL849;} else{ goto _LL850;} _LL850: if(*(( int*) _temp802) ==  Cyc_Absyn_StructArrow_e){
_LL973: _temp972=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp802)->f1; goto
_LL971; _LL971: _temp970=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp802)->f2;
goto _LL851;} else{ goto _LL852;} _LL852: if(*(( int*) _temp802) ==  Cyc_Absyn_Subscript_e){
_LL977: _temp976=(( struct Cyc_Absyn_Subscript_e_struct*) _temp802)->f1; goto
_LL975; _LL975: _temp974=(( struct Cyc_Absyn_Subscript_e_struct*) _temp802)->f2;
goto _LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp802) ==  Cyc_Absyn_Tuple_e){
_LL979: _temp978=(( struct Cyc_Absyn_Tuple_e_struct*) _temp802)->f1; goto _LL855;}
else{ goto _LL856;} _LL856: if(*(( int*) _temp802) ==  Cyc_Absyn_Array_e){
_LL981: _temp980=(( struct Cyc_Absyn_Array_e_struct*) _temp802)->f1; goto _LL857;}
else{ goto _LL858;} _LL858: if(*(( int*) _temp802) ==  Cyc_Absyn_Comprehension_e){
_LL987: _temp986=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp802)->f1;
goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp802)->f2; goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp802)->f3; goto _LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp802)
==  Cyc_Absyn_Struct_e){ _LL995: _temp994=(( struct Cyc_Absyn_Struct_e_struct*)
_temp802)->f1; goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_Struct_e_struct*)
_temp802)->f2; goto _LL991; _LL991: _temp990=(( struct Cyc_Absyn_Struct_e_struct*)
_temp802)->f3; goto _LL989; _LL989: _temp988=(( struct Cyc_Absyn_Struct_e_struct*)
_temp802)->f4; goto _LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp802)
==  Cyc_Absyn_AnonStruct_e){ _LL999: _temp998=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp802)->f1; goto _LL997; _LL997: _temp996=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp802)->f2; goto _LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp802)
==  Cyc_Absyn_Tunion_e){ _LL1005: _temp1004=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp802)->f3; if( _temp1004 ==  0){ goto _LL1003;} else{ goto _LL866;} _LL1003:
_temp1002=(( struct Cyc_Absyn_Tunion_e_struct*) _temp802)->f4; goto _LL1001;
_LL1001: _temp1000=(( struct Cyc_Absyn_Tunion_e_struct*) _temp802)->f5; goto
_LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp802) ==  Cyc_Absyn_Tunion_e){
_LL1011: _temp1010=(( struct Cyc_Absyn_Tunion_e_struct*) _temp802)->f3; goto
_LL1009; _LL1009: _temp1008=(( struct Cyc_Absyn_Tunion_e_struct*) _temp802)->f4;
goto _LL1007; _LL1007: _temp1006=(( struct Cyc_Absyn_Tunion_e_struct*) _temp802)->f5;
goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp802) ==  Cyc_Absyn_Enum_e){
goto _LL869;} else{ goto _LL870;} _LL870: if(*(( int*) _temp802) ==  Cyc_Absyn_Malloc_e){
_LL1015: _temp1014=(( struct Cyc_Absyn_Malloc_e_struct*) _temp802)->f1; goto
_LL1013; _LL1013: _temp1012=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp802)->f2; goto _LL871;} else{ goto _LL872;} _LL872: if(*(( int*) _temp802)
==  Cyc_Absyn_StmtExp_e){ _LL1017: _temp1016=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp802)->f1; goto _LL873;} else{ goto _LL874;} _LL874: if(*(( int*) _temp802)
==  Cyc_Absyn_UnresolvedMem_e){ goto _LL875;} else{ goto _LL876;} _LL876: if(*((
int*) _temp802) ==  Cyc_Absyn_CompoundLit_e){ goto _LL877;} else{ goto _LL878;}
_LL878: if(*(( int*) _temp802) ==  Cyc_Absyn_Codegen_e){ _LL1019: _temp1018=((
struct Cyc_Absyn_Codegen_e_struct*) _temp802)->f1; goto _LL879;} else{ goto
_LL880;} _LL880: if(*(( int*) _temp802) ==  Cyc_Absyn_Fill_e){ goto _LL881;}
else{ goto _LL803;} _LL805: { struct Cyc_Absyn_Exp* _temp1020= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1020,
_temp1020))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1021[ 3u]; _temp1021[ 2u]=
_temp1020; _temp1021[ 1u]= _temp1020; _temp1021[ 0u]= _temp1020;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1021, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL803;} _LL807: goto _LL803; _LL809:{ struct _handler_cons
_temp1022; _push_handler(& _temp1022);{ int _temp1024= 0; if( setjmp( _temp1022.handler)){
_temp1024= 1;} if( ! _temp1024){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp886))->r));; _pop_handler();} else{ void* _temp1023=( void*) _exn_thrown;
void* _temp1026= _temp1023; _LL1028: if( _temp1026 ==  Cyc_Dict_Absent){ goto
_LL1029;} else{ goto _LL1030;} _LL1030: goto _LL1031; _LL1029:({ struct Cyc_Std_String_pa_struct
_temp1033; _temp1033.tag= Cyc_Std_String_pa; _temp1033.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp886);{ void* _temp1032[ 1u]={& _temp1033};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1032, sizeof( void*), 1u));}});
return; _LL1031:( void) _throw( _temp1026); _LL1027:;}}} goto _LL803; _LL811:({
void* _temp1034[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1034, sizeof( void*), 0u));}); goto _LL803; _LL813: { struct Cyc_List_List*
_temp1035=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp890);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp890);{
void* _temp1036= _temp892; _LL1038: if( _temp1036 == ( void*) Cyc_Absyn_Size){
goto _LL1039;} else{ goto _LL1040;} _LL1040: if( _temp1036 == ( void*) Cyc_Absyn_Plus){
goto _LL1041;} else{ goto _LL1042;} _LL1042: if( _temp1036 == ( void*) Cyc_Absyn_Minus){
goto _LL1043;} else{ goto _LL1044;} _LL1044: if( _temp1036 == ( void*) Cyc_Absyn_Eq){
goto _LL1045;} else{ goto _LL1046;} _LL1046: if( _temp1036 == ( void*) Cyc_Absyn_Neq){
goto _LL1047;} else{ goto _LL1048;} _LL1048: if( _temp1036 == ( void*) Cyc_Absyn_Gt){
goto _LL1049;} else{ goto _LL1050;} _LL1050: if( _temp1036 == ( void*) Cyc_Absyn_Gte){
goto _LL1051;} else{ goto _LL1052;} _LL1052: if( _temp1036 == ( void*) Cyc_Absyn_Lt){
goto _LL1053;} else{ goto _LL1054;} _LL1054: if( _temp1036 == ( void*) Cyc_Absyn_Lte){
goto _LL1055;} else{ goto _LL1056;} _LL1056: goto _LL1057; _LL1039: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp890))->hd;{
void* _temp1058= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1066; struct Cyc_Absyn_PtrInfo
_temp1068; struct Cyc_Absyn_Conref* _temp1070; void* _temp1072; _LL1060: if((
unsigned int) _temp1058 >  4u?*(( int*) _temp1058) ==  Cyc_Absyn_ArrayType: 0){
_LL1067: _temp1066=(( struct Cyc_Absyn_ArrayType_struct*) _temp1058)->f3; goto
_LL1061;} else{ goto _LL1062;} _LL1062: if(( unsigned int) _temp1058 >  4u?*((
int*) _temp1058) ==  Cyc_Absyn_PointerType: 0){ _LL1069: _temp1068=(( struct Cyc_Absyn_PointerType_struct*)
_temp1058)->f1; _LL1073: _temp1072=( void*) _temp1068.elt_typ; goto _LL1071;
_LL1071: _temp1070= _temp1068.bounds; goto _LL1063;} else{ goto _LL1064;}
_LL1064: goto _LL1065; _LL1061:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1066))->r)); goto _LL1059; _LL1063:{ void* _temp1074= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1070); struct Cyc_Absyn_Exp* _temp1080; _LL1076: if(
_temp1074 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1077;} else{ goto _LL1078;}
_LL1078: if(( unsigned int) _temp1074 >  1u?*(( int*) _temp1074) ==  Cyc_Absyn_Upper_b:
0){ _LL1081: _temp1080=(( struct Cyc_Absyn_Upper_b_struct*) _temp1074)->f1; goto
_LL1079;} else{ goto _LL1075;} _LL1077:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1082[ 2u]; _temp1082[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1072), 0); _temp1082[ 0u]=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp890))->hd;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1082, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r)); goto _LL1075;
_LL1079:( void*)( e->r=( void*)(( void*) _temp1080->r)); goto _LL1075; _LL1075:;}
goto _LL1059; _LL1065:({ struct Cyc_Std_String_pa_struct _temp1085; _temp1085.tag=
Cyc_Std_String_pa; _temp1085.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp1084; _temp1084.tag= Cyc_Std_String_pa; _temp1084.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
void* _temp1083[ 2u]={& _temp1084,& _temp1085};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u), _tag_arr( _temp1083, sizeof( void*), 2u));}}});
return; _LL1059:;} goto _LL1037;} _LL1041: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1035))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp890))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp890))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1086[ 3u]; _temp1086[ 2u]=
e2; _temp1086[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1086[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1086, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1037;} _LL1043: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1035))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp890))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp890))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1035))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0),
0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp* _temp1087[ 3u]; _temp1087[ 2u]= Cyc_Absyn_prim1_exp(( void*)
Cyc_Absyn_Minus, e2, 0); _temp1087[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1087[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1087, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}}
goto _LL1037;} _LL1045: goto _LL1047; _LL1047: goto _LL1049; _LL1049: goto
_LL1051; _LL1051: goto _LL1053; _LL1053: goto _LL1055; _LL1055: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp890))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp890))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1035))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1035))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1037;} _LL1057: goto
_LL1037; _LL1037:;} goto _LL803;} _LL815: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp896->topt))->v; Cyc_Toc_exp_to_c( nv, _temp896);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1088=( _temp894 == ( void*) Cyc_Absyn_PostInc?
1: _temp894 == ( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1089=( _temp894 == ( void*) Cyc_Absyn_PreInc?
1: _temp894 == ( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1088,({ struct Cyc_Absyn_Exp* _temp1090[ 3u];
_temp1090[ 2u]= Cyc_Absyn_signed_int_exp( _temp1089, 0); _temp1090[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1090[ 0u]= Cyc_Absyn_address_exp( _temp896,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1090, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ == (
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp896): 0){ Cyc_Toc_lvalue_assign(
_temp896, 0, Cyc_Toc_incr_lvalue, _temp894);( void*)( e->r=( void*)(( void*)
_temp896->r));} goto _LL803;}} _LL817: { int e1_poly= Cyc_Toc_is_poly_project(
_temp902); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp902->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp898->topt))->v; Cyc_Toc_exp_to_c( nv, _temp902); Cyc_Toc_exp_to_c(
nv, _temp898);{ int done= 0; if( _temp900 !=  0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1091=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp900))->v;
_LL1093: if( _temp1091 == ( void*) Cyc_Absyn_Plus){ goto _LL1094;} else{ goto
_LL1095;} _LL1095: if( _temp1091 == ( void*) Cyc_Absyn_Minus){ goto _LL1096;}
else{ goto _LL1097;} _LL1097: goto _LL1098; _LL1094: change= _temp898; goto
_LL1092; _LL1096: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp898,
0); goto _LL1092; _LL1098:( int) _throw(({ void* _temp1099[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u), _tag_arr(
_temp1099, sizeof( void*), 0u));})); goto _LL1092; _LL1092:;} done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp* _temp1100[ 3u]; _temp1100[ 2u]= change; _temp1100[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); _temp1100[ 0u]= Cyc_Absyn_address_exp(
_temp902, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1100, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}} if( !
done){ if( e1_poly){( void*)( _temp898->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*) _temp898->r, 0), 0))->r));}
if( ! Cyc_Absyn_is_lvalue( _temp902)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)( _temp902, 0,
Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1101=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7) *  1); _temp1101[ 0]=({ struct _tuple7
_temp1102; _temp1102.f1= _temp900; _temp1102.f2= _temp898; _temp1102;});
_temp1101;}));( void*)( e->r=( void*)(( void*) _temp902->r));}} goto _LL803;}}
_LL819: Cyc_Toc_exp_to_c( nv, _temp908); Cyc_Toc_exp_to_c( nv, _temp906); Cyc_Toc_exp_to_c(
nv, _temp904); goto _LL803; _LL821: Cyc_Toc_exp_to_c( nv, _temp912); Cyc_Toc_exp_to_c(
nv, _temp910); goto _LL803; _LL823: _temp922= _temp916; _temp920= _temp914; goto
_LL825; _LL825: Cyc_Toc_exp_to_c( nv, _temp922);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp920); goto _LL803; _LL827:{
struct _RegionHandle _temp1103= _new_region(); struct _RegionHandle* r=&
_temp1103; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp931, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp927->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1120=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1120[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1121; _temp1121.tag= Cyc_Absyn_ArrayType;
_temp1121.f1=( void*) cva_type; _temp1121.f2= Cyc_Toc_mt_tq; _temp1121.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1121;}); _temp1120;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp933); int num_normargs=
num_args -  _temp931; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i < 
num_normargs;( ++ i, _temp933=(( struct Cyc_List_List*) _check_null( _temp933))->tl)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp933))->hd); new_args=({ struct Cyc_List_List* _temp1104=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1104->hd=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp933))->hd);
_temp1104->tl= new_args; _temp1104;});}} new_args=({ struct Cyc_List_List*
_temp1105=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1105->hd=( void*) Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1106[ 3u]; _temp1106[ 2u]= num_varargs_exp; _temp1106[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1106[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1106, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1105->tl= new_args; _temp1105;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp935);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp935, new_args, 0), 0); if( _temp927->inject){ struct _tagged_arr vs=({
unsigned int _temp1116=( unsigned int) _temp931; struct _tuple0** _temp1117=(
struct _tuple0**) _region_malloc( r, _check_times( sizeof( struct _tuple0*),
_temp1116)); struct _tagged_arr _temp1119= _tag_arr( _temp1117, sizeof( struct
_tuple0*),( unsigned int) _temp931);{ unsigned int _temp1118= _temp1116;
unsigned int i; for( i= 0; i <  _temp1118; i ++){ _temp1117[ i]= Cyc_Toc_temp_var();}};
_temp1119;}); struct Cyc_List_List* _temp1107= 0;{ int i= _temp931 -  1; for( 0;
i >=  0; -- i){ _temp1107=({ struct Cyc_List_List* _temp1108=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1108->hd=( void*) Cyc_Toc_make_dle(
Cyc_Absyn_address_exp( Cyc_Absyn_var_exp(*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)), 0), 0)); _temp1108->tl=
_temp1107; _temp1108;});}} s= Cyc_Absyn_declare_stmt( argv, arr_type,( struct
Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0, _temp1107, 0), s, 0);{ int i= 0;
for( 0; _temp933 !=  0;((( _temp933=(( struct Cyc_List_List*) _check_null(
_temp933))->tl, _temp929=(( struct Cyc_List_List*) _check_null( _temp929))->tl)),
++ i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp933))->hd; void* arg_type=( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v; struct _tuple0* var=*(( struct _tuple0**)
_check_unknown_subscript( vs, sizeof( struct _tuple0*), i)); struct Cyc_Absyn_Exp*
varexp= Cyc_Absyn_var_exp( var, 0); struct Cyc_Absyn_Tunionfield _temp1111;
struct Cyc_List_List* _temp1112; struct _tuple0* _temp1114; struct Cyc_Absyn_Tunionfield*
_temp1109=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp929))->hd; _temp1111=* _temp1109; _LL1115: _temp1114= _temp1111.name; goto
_LL1113; _LL1113: _temp1112= _temp1111.typs; goto _LL1110; _LL1110: { void*
field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1112))->hd)).f2); Cyc_Toc_exp_to_c( nv, arg); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( varexp, Cyc_Toc_fieldname( 1),
0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1114, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1114, _tag_arr("_struct",
sizeof( unsigned char), 8u))), 0, s, 0);}}}} else{{ int i= 0; for( 0; _temp933
!=  0;( _temp933=(( struct Cyc_List_List*) _check_null( _temp933))->tl, ++ i)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp933))->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp933))->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL803; _LL829: Cyc_Toc_exp_to_c( nv,
_temp937);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp937), 0))->r)); goto _LL803; _LL831: Cyc_Toc_exp_to_c(
nv, _temp939); goto _LL803; _LL833: Cyc_Toc_exp_to_c( nv, _temp943); for( 0;
_temp941 !=  0; _temp941=(( struct Cyc_List_List*) _check_null( _temp941))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp941))->hd); if( k != ( void*) Cyc_Absyn_EffKind? k != ( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1122= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp941))->hd); _LL1124: if(( unsigned int) _temp1122 >  4u?*((
int*) _temp1122) ==  Cyc_Absyn_VarType: 0){ goto _LL1125;} else{ goto _LL1126;}
_LL1126: if(( unsigned int) _temp1122 >  4u?*(( int*) _temp1122) ==  Cyc_Absyn_TunionType:
0){ goto _LL1127;} else{ goto _LL1128;} _LL1128: goto _LL1129; _LL1125:
continue; _LL1127: continue; _LL1129:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp943, 0))->r)); goto _LL1123; _LL1123:;} break;}} goto _LL803; _LL835: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp945->topt))->v;
void* new_typ=* _temp949;* _temp949= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp945);{ struct _tuple8 _temp1131=({ struct _tuple8 _temp1130; _temp1130.f1=
Cyc_Tcutil_compress( old_t2); _temp1130.f2= Cyc_Tcutil_compress( new_typ);
_temp1130;}); void* _temp1139; struct Cyc_Absyn_PtrInfo _temp1141; void*
_temp1143; struct Cyc_Absyn_PtrInfo _temp1145; void* _temp1147; void* _temp1149;
struct Cyc_Absyn_PtrInfo _temp1151; _LL1133: _LL1144: _temp1143= _temp1131.f1;
if(( unsigned int) _temp1143 >  4u?*(( int*) _temp1143) ==  Cyc_Absyn_PointerType:
0){ _LL1146: _temp1145=(( struct Cyc_Absyn_PointerType_struct*) _temp1143)->f1;
goto _LL1140;} else{ goto _LL1135;} _LL1140: _temp1139= _temp1131.f2; if((
unsigned int) _temp1139 >  4u?*(( int*) _temp1139) ==  Cyc_Absyn_PointerType: 0){
_LL1142: _temp1141=(( struct Cyc_Absyn_PointerType_struct*) _temp1139)->f1; goto
_LL1134;} else{ goto _LL1135;} _LL1135: _LL1150: _temp1149= _temp1131.f1; if((
unsigned int) _temp1149 >  4u?*(( int*) _temp1149) ==  Cyc_Absyn_PointerType: 0){
_LL1152: _temp1151=(( struct Cyc_Absyn_PointerType_struct*) _temp1149)->f1; goto
_LL1148;} else{ goto _LL1137;} _LL1148: _temp1147= _temp1131.f2; if((
unsigned int) _temp1147 >  4u?*(( int*) _temp1147) ==  Cyc_Absyn_IntType: 0){
goto _LL1136;} else{ goto _LL1137;} _LL1137: goto _LL1138; _LL1134: { int
_temp1153=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0,
_temp1145.nullable); int _temp1154=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1141.nullable); void* _temp1155= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1145.bounds); void* _temp1156= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1141.bounds);{ struct _tuple8 _temp1158=({ struct
_tuple8 _temp1157; _temp1157.f1= _temp1155; _temp1157.f2= _temp1156; _temp1157;});
void* _temp1168; void* _temp1170; void* _temp1172; void* _temp1174; struct Cyc_Absyn_Exp*
_temp1176; void* _temp1178; struct Cyc_Absyn_Exp* _temp1180; void* _temp1182;
_LL1160: _LL1171: _temp1170= _temp1158.f1; if(( unsigned int) _temp1170 >  1u?*((
int*) _temp1170) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1169;} else{ goto _LL1162;}
_LL1169: _temp1168= _temp1158.f2; if(( unsigned int) _temp1168 >  1u?*(( int*)
_temp1168) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1161;} else{ goto _LL1162;}
_LL1162: _LL1175: _temp1174= _temp1158.f1; if(( unsigned int) _temp1174 >  1u?*((
int*) _temp1174) ==  Cyc_Absyn_Upper_b: 0){ _LL1177: _temp1176=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1174)->f1; goto _LL1173;} else{ goto _LL1164;} _LL1173: _temp1172=
_temp1158.f2; if( _temp1172 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1163;}
else{ goto _LL1164;} _LL1164: _LL1183: _temp1182= _temp1158.f1; if( _temp1182 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1179;} else{ goto _LL1166;} _LL1179:
_temp1178= _temp1158.f2; if(( unsigned int) _temp1178 >  1u?*(( int*) _temp1178)
==  Cyc_Absyn_Upper_b: 0){ _LL1181: _temp1180=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1178)->f1; goto _LL1165;} else{ goto _LL1166;} _LL1166: goto _LL1167;
_LL1161: if( _temp1153? ! _temp1154: 0){ if( nv->toplevel){({ void* _temp1184[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do NULL-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1184, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp949, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1185=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1185->hd=( void*) _temp945;
_temp1185->tl= 0; _temp1185;}), 0), 0))->r));} goto _LL1159; _LL1163: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1176, _temp945))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1186[ 3u]; _temp1186[ 2u]=
_temp1176; _temp1186[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1141.elt_typ), 0); _temp1186[ 0u]= _temp945;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1186, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1159; _LL1165: if( nv->toplevel){({ void* _temp1187[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1187, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1188= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1190[ 3u]; _temp1190[ 2u]= _temp1180; _temp1190[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1145.elt_typ), 0); _temp1190[ 0u]= _temp945;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1190, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1154){( void*)(
_temp1188->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1189=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1189->hd=( void*) Cyc_Absyn_copy_exp( _temp1188);
_temp1189->tl= 0; _temp1189;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp949, _temp1188, 0))->r)); goto _LL1159;} _LL1167: goto _LL1159; _LL1159:;}
goto _LL1132;} _LL1136:{ void* _temp1191= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1151.bounds); _LL1193: if( _temp1191 == ( void*) Cyc_Absyn_Unknown_b){ goto
_LL1194;} else{ goto _LL1195;} _LL1195: goto _LL1196; _LL1194:( void*)( _temp945->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp945->r,
_temp945->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1192; _LL1196: goto _LL1192;
_LL1192:;} goto _LL1132; _LL1138: goto _LL1132; _LL1132:;} goto _LL803;} _LL837:{
void* _temp1197=( void*) _temp950->r; struct Cyc_List_List* _temp1205; struct
_tuple0* _temp1207; struct Cyc_List_List* _temp1209; _LL1199: if(*(( int*)
_temp1197) ==  Cyc_Absyn_Struct_e){ _LL1208: _temp1207=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1197)->f1; goto _LL1206; _LL1206: _temp1205=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1197)->f3; goto _LL1200;} else{ goto _LL1201;} _LL1201: if(*(( int*)
_temp1197) ==  Cyc_Absyn_Tuple_e){ _LL1210: _temp1209=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1197)->f1; goto _LL1202;} else{ goto _LL1203;} _LL1203: goto _LL1204;
_LL1200: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1212;
_temp1212.tag= Cyc_Std_String_pa; _temp1212.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp950->loc);{ void* _temp1211[ 1u]={& _temp1212};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1211, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp950->topt))->v, 1, 0, _temp1205, _temp1207))->r)); goto
_LL1198; _LL1202: if( nv->toplevel){({ struct Cyc_Std_String_pa_struct _temp1214;
_temp1214.tag= Cyc_Std_String_pa; _temp1214.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp950->loc);{ void* _temp1213[ 1u]={& _temp1214};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1213, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1209))->r));
goto _LL1198; _LL1204: Cyc_Toc_exp_to_c( nv, _temp950); if( ! Cyc_Absyn_is_lvalue(
_temp950)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)(
_temp950, 0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp950, 0))->r));} goto _LL1198; _LL1198:;} goto _LL803; _LL839: if( nv->toplevel){({
struct Cyc_Std_String_pa_struct _temp1216; _temp1216.tag= Cyc_Std_String_pa;
_temp1216.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp952->loc);{
void* _temp1215[ 1u]={& _temp1216};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u), _tag_arr( _temp1215, sizeof( void*), 1u));}});}{ void*
_temp1217=( void*) _temp952->r; struct Cyc_List_List* _temp1229; struct Cyc_Absyn_Exp*
_temp1231; struct Cyc_Absyn_Exp* _temp1233; struct Cyc_Absyn_Vardecl* _temp1235;
struct Cyc_List_List* _temp1237; struct _tuple0* _temp1239; struct Cyc_List_List*
_temp1241; _LL1219: if(*(( int*) _temp1217) ==  Cyc_Absyn_Array_e){ _LL1230:
_temp1229=(( struct Cyc_Absyn_Array_e_struct*) _temp1217)->f1; goto _LL1220;}
else{ goto _LL1221;} _LL1221: if(*(( int*) _temp1217) ==  Cyc_Absyn_Comprehension_e){
_LL1236: _temp1235=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1217)->f1;
goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1217)->f2; goto _LL1232; _LL1232: _temp1231=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1217)->f3; goto _LL1222;} else{ goto _LL1223;} _LL1223: if(*(( int*)
_temp1217) ==  Cyc_Absyn_Struct_e){ _LL1240: _temp1239=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1217)->f1; goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1217)->f3; goto _LL1224;} else{ goto _LL1225;} _LL1225: if(*(( int*)
_temp1217) ==  Cyc_Absyn_Tuple_e){ _LL1242: _temp1241=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1217)->f1; goto _LL1226;} else{ goto _LL1227;} _LL1227: goto _LL1228;
_LL1220: { struct _tuple0* _temp1243= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1244= Cyc_Absyn_var_exp( _temp1243, 0); struct Cyc_Absyn_Stmt* _temp1245=
Cyc_Toc_init_array( nv, _temp1244, _temp1229, Cyc_Absyn_exp_stmt( _temp1244, 0));
void* old_elt_typ;{ void* _temp1246= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1252; struct Cyc_Absyn_Tqual _temp1254; void* _temp1256; _LL1248: if((
unsigned int) _temp1246 >  4u?*(( int*) _temp1246) ==  Cyc_Absyn_PointerType: 0){
_LL1253: _temp1252=(( struct Cyc_Absyn_PointerType_struct*) _temp1246)->f1;
_LL1257: _temp1256=( void*) _temp1252.elt_typ; goto _LL1255; _LL1255: _temp1254=
_temp1252.tq; goto _LL1249;} else{ goto _LL1250;} _LL1250: goto _LL1251; _LL1249:
old_elt_typ= _temp1256; goto _LL1247; _LL1251: old_elt_typ=({ void* _temp1258[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1258, sizeof( void*), 0u));}); goto
_LL1247; _LL1247:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1259= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1260= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1229), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp954 ==  0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1260);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp954); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1260);}{
struct Cyc_Absyn_Exp* _temp1261= Cyc_Absyn_cast_exp( _temp1259, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1243,
_temp1259,( struct Cyc_Absyn_Exp*) _temp1261, _temp1245, 0), 0))->r)); goto
_LL1218;}}} _LL1222: { int is_tagged_ptr= 0;{ void* _temp1262= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1268; struct Cyc_Absyn_Conref* _temp1270;
struct Cyc_Absyn_Tqual _temp1272; void* _temp1274; _LL1264: if(( unsigned int)
_temp1262 >  4u?*(( int*) _temp1262) ==  Cyc_Absyn_PointerType: 0){ _LL1269:
_temp1268=(( struct Cyc_Absyn_PointerType_struct*) _temp1262)->f1; _LL1275:
_temp1274=( void*) _temp1268.elt_typ; goto _LL1273; _LL1273: _temp1272=
_temp1268.tq; goto _LL1271; _LL1271: _temp1270= _temp1268.bounds; goto _LL1265;}
else{ goto _LL1266;} _LL1266: goto _LL1267; _LL1265: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1270) == ( void*) Cyc_Absyn_Unknown_b; goto _LL1263;
_LL1267:({ void* _temp1276[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1276, sizeof( void*), 0u));}); goto
_LL1263; _LL1263:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1231->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1233);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1235, Cyc_Absyn_var_exp( max, 0), _temp1231, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1277= _new_region(); struct _RegionHandle* r=&
_temp1277; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1297=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1297->hd=( void*)({ struct _tuple9* _temp1298=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1298->f1= max; _temp1298->f2=
Cyc_Absyn_uint_t; _temp1298->f3=( struct Cyc_Absyn_Exp*) _temp1233; _temp1298;});
_temp1297->tl= 0; _temp1297;}); struct Cyc_Absyn_Exp* ai; if( _temp954 ==  0){
ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp* _temp1278[ 2u]; _temp1278[ 1u]= Cyc_Absyn_var_exp( max, 0);
_temp1278[ 0u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1278, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp954); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_fncall_exp( Cyc_Toc__check_times_e,({ struct Cyc_Absyn_Exp* _temp1279[
2u]; _temp1279[ 1u]= Cyc_Absyn_var_exp( max, 0); _temp1279[ 0u]= Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1279, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0));}{ struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1280=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1280->hd=( void*)({ struct _tuple9* _temp1281=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1281->f1= a; _temp1281->f2=
ptr_typ; _temp1281->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1281;}); _temp1280->tl=
decls; _temp1280;}); if( is_tagged_ptr){ struct _tuple0* _temp1282= Cyc_Toc_temp_var();
void* _temp1283= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* _temp1284=
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1287[ 3u];
_temp1287[ 2u]= _temp1233; _temp1287[ 1u]= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
_temp1287[ 0u]= Cyc_Absyn_var_exp( a, 0);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1287, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); decls=({ struct Cyc_List_List* _temp1285=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1285->hd=( void*)({
struct _tuple9* _temp1286=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1286->f1= _temp1282; _temp1286->f2= _temp1283; _temp1286->f3=(
struct Cyc_Absyn_Exp*) _temp1284; _temp1286;}); _temp1285->tl= decls; _temp1285;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1282, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1288= decls; for( 0; _temp1288 !=  0;
_temp1288=(( struct Cyc_List_List*) _check_null( _temp1288))->tl){ struct Cyc_Absyn_Exp*
_temp1291; void* _temp1293; struct _tuple0* _temp1295; struct _tuple9 _temp1289=*((
struct _tuple9*)(( struct Cyc_List_List*) _check_null( _temp1288))->hd); _LL1296:
_temp1295= _temp1289.f1; goto _LL1294; _LL1294: _temp1293= _temp1289.f2; goto
_LL1292; _LL1292: _temp1291= _temp1289.f3; goto _LL1290; _LL1290: s= Cyc_Absyn_declare_stmt(
_temp1295, _temp1293, _temp1291, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1218;}}} _LL1224:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp952->topt))->v, 1, _temp954, _temp1237, _temp1239))->r)); goto _LL1218;
_LL1226:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp954,
_temp1241))->r)); goto _LL1218; _LL1228: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp952->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp954 ==  0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp954); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1299=( void*)
_temp952->r; struct Cyc_Absyn_Exp* _temp1305; void* _temp1307; _LL1301: if(*((
int*) _temp1299) ==  Cyc_Absyn_Cast_e){ _LL1308: _temp1307=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1299)->f1; goto _LL1306; _LL1306: _temp1305=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1299)->f2; goto _LL1302;} else{ goto _LL1303;} _LL1303: goto _LL1304;
_LL1302:{ struct _tuple8 _temp1310=({ struct _tuple8 _temp1309; _temp1309.f1=
Cyc_Tcutil_compress( _temp1307); _temp1309.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1305->topt))->v); _temp1309;}); void*
_temp1316; struct Cyc_Absyn_PtrInfo _temp1318; struct Cyc_Absyn_Conref*
_temp1320; void* _temp1322; struct Cyc_Absyn_PtrInfo _temp1324; struct Cyc_Absyn_Conref*
_temp1326; void* _temp1328; _LL1312: _LL1323: _temp1322= _temp1310.f1; if((
unsigned int) _temp1322 >  4u?*(( int*) _temp1322) ==  Cyc_Absyn_PointerType: 0){
_LL1325: _temp1324=(( struct Cyc_Absyn_PointerType_struct*) _temp1322)->f1;
_LL1329: _temp1328=( void*) _temp1324.elt_typ; goto _LL1327; _LL1327: _temp1326=
_temp1324.bounds; goto _LL1317;} else{ goto _LL1314;} _LL1317: _temp1316=
_temp1310.f2; if(( unsigned int) _temp1316 >  4u?*(( int*) _temp1316) ==  Cyc_Absyn_PointerType:
0){ _LL1319: _temp1318=(( struct Cyc_Absyn_PointerType_struct*) _temp1316)->f1;
_LL1321: _temp1320= _temp1318.bounds; goto _LL1313;} else{ goto _LL1314;}
_LL1314: goto _LL1315; _LL1313:{ struct _tuple8 _temp1331=({ struct _tuple8
_temp1330; _temp1330.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1326);
_temp1330.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1320); _temp1330;});
void* _temp1337; struct Cyc_Absyn_Exp* _temp1339; void* _temp1341; _LL1333:
_LL1342: _temp1341= _temp1331.f1; if( _temp1341 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1338;} else{ goto _LL1335;} _LL1338: _temp1337= _temp1331.f2; if((
unsigned int) _temp1337 >  1u?*(( int*) _temp1337) ==  Cyc_Absyn_Upper_b: 0){
_LL1340: _temp1339=(( struct Cyc_Absyn_Upper_b_struct*) _temp1337)->f1; goto
_LL1334;} else{ goto _LL1335;} _LL1335: goto _LL1336; _LL1334: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1343[ 4u]; _temp1343[ 3u]= _temp1339; _temp1343[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1328, 0); _temp1343[ 1u]= _temp1305; _temp1343[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1343, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1332; _LL1336: goto _LL1332; _LL1332:;} goto _LL1311;
_LL1315: goto _LL1311; _LL1311:;} goto _LL1300; _LL1304: goto _LL1300; _LL1300:;}
if( ! done){ struct _tuple0* _temp1344= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1345= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1344, 0), 0); struct Cyc_Absyn_Exp*
_temp1346= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp952);
_temp1345= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1344, 0), _temp1346, 0), _temp952, 0), _temp1345, 0);{
void* _temp1347= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1344,
_temp1347,( struct Cyc_Absyn_Exp*) mexp, _temp1345, 0), 0))->r));}} goto _LL1218;}}
_LL1218:;} goto _LL803; _LL841: Cyc_Toc_exp_to_c( nv, _temp956); goto _LL803;
_LL843:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1348=( struct Cyc_Absyn_Sizeoftyp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1348[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1349; _temp1349.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1349.f1=( void*) Cyc_Toc_typ_to_c_array( _temp958);
_temp1349;}); _temp1348;}))); goto _LL803; _LL845:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Offsetof_e_struct* _temp1350=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1350[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1351; _temp1351.tag= Cyc_Absyn_Offsetof_e;
_temp1351.f1=( void*) Cyc_Toc_typ_to_c_array( _temp962); _temp1351.f2= _temp960;
_temp1351;}); _temp1350;}))); goto _LL803; _LL847: { void* _temp1352= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp964->topt))->v);{ void*
_temp1353= _temp1352; struct Cyc_Absyn_PtrInfo _temp1359; struct Cyc_Absyn_Conref*
_temp1361; struct Cyc_Absyn_Tqual _temp1363; struct Cyc_Absyn_Conref* _temp1365;
void* _temp1367; void* _temp1369; _LL1355: if(( unsigned int) _temp1353 >  4u?*((
int*) _temp1353) ==  Cyc_Absyn_PointerType: 0){ _LL1360: _temp1359=(( struct Cyc_Absyn_PointerType_struct*)
_temp1353)->f1; _LL1370: _temp1369=( void*) _temp1359.elt_typ; goto _LL1368;
_LL1368: _temp1367=( void*) _temp1359.rgn_typ; goto _LL1366; _LL1366: _temp1365=
_temp1359.nullable; goto _LL1364; _LL1364: _temp1363= _temp1359.tq; goto _LL1362;
_LL1362: _temp1361= _temp1359.bounds; goto _LL1356;} else{ goto _LL1357;}
_LL1357: goto _LL1358; _LL1356:{ void* _temp1371= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1361); _LL1373: if(( unsigned int) _temp1371 >  1u?*(( int*) _temp1371) == 
Cyc_Absyn_Upper_b: 0){ goto _LL1374;} else{ goto _LL1375;} _LL1375: if(
_temp1371 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1376;} else{ goto _LL1372;}
_LL1374: Cyc_Toc_exp_to_c( nv, _temp964); if( Cyc_Toc_is_nullable( _temp1352)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp964->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1377=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1377->hd=( void*) _temp964;
_temp1377->tl= 0; _temp1377;}), 0), 0), 0))->r));} goto _LL1372; _LL1376: {
struct Cyc_Absyn_Exp* _temp1378= Cyc_Absyn_uint_exp( 0, 0); _temp1378->topt=({
struct Cyc_Core_Opt* _temp1379=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1379->v=( void*) Cyc_Absyn_uint_t; _temp1379;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp964, _temp1378, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1372;} _LL1372:;} goto _LL1354; _LL1358:({ void* _temp1380[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1380, sizeof( void*), 0u));}); goto _LL1354; _LL1354:;} goto _LL803;}
_LL849: Cyc_Toc_exp_to_c( nv, _temp968); if( Cyc_Toc_is_poly_project( e)){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r,
0), 0))->r));} goto _LL803; _LL851: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp972->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp972);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1383; struct Cyc_Absyn_Tqual _temp1385; struct Cyc_Absyn_Conref* _temp1387;
void* _temp1389; void* _temp1391; struct Cyc_Absyn_PtrInfo _temp1381= Cyc_Toc_get_ptr_type(
e1typ); _LL1392: _temp1391=( void*) _temp1381.elt_typ; goto _LL1390; _LL1390:
_temp1389=( void*) _temp1381.rgn_typ; goto _LL1388; _LL1388: _temp1387=
_temp1381.nullable; goto _LL1386; _LL1386: _temp1385= _temp1381.tq; goto _LL1384;
_LL1384: _temp1383= _temp1381.bounds; goto _LL1382; _LL1382:{ void* _temp1393=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1383); struct Cyc_Absyn_Exp*
_temp1399; _LL1395: if(( unsigned int) _temp1393 >  1u?*(( int*) _temp1393) == 
Cyc_Absyn_Upper_b: 0){ _LL1400: _temp1399=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1393)->f1; goto _LL1396;} else{ goto _LL1397;} _LL1397: if( _temp1393 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL1398;} else{ goto _LL1394;} _LL1396: if(
Cyc_Evexp_eval_const_uint_exp( _temp1399) <  1){({ void* _temp1401[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0", sizeof( unsigned char),
46u), _tag_arr( _temp1401, sizeof( void*), 0u));});}{ int possibly_null=(( int(*)(
int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1387); if( Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp972->topt))->v)){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp972->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1402=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1402->hd=( void*) _temp972;
_temp1402->tl= 0; _temp1402;}), 0), 0), _temp970, 0))->r));} goto _LL1394;}
_LL1398: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1391);( void*)( _temp972->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1385), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_Absyn_Exp* _temp1403[ 3u];
_temp1403[ 2u]= Cyc_Absyn_uint_exp( 0, 0); _temp1403[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1403[ 0u]= Cyc_Absyn_copy_exp( _temp972);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1403, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0))->r)); goto _LL1394;} _LL1394:;} if( is_poly){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL803;}} _LL853: { void* _temp1404= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp976->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp976); Cyc_Toc_exp_to_c( nv, _temp974);{ void* _temp1405= _temp1404; struct
Cyc_List_List* _temp1413; struct Cyc_Absyn_PtrInfo _temp1415; struct Cyc_Absyn_Conref*
_temp1417; struct Cyc_Absyn_Tqual _temp1419; struct Cyc_Absyn_Conref* _temp1421;
void* _temp1423; void* _temp1425; _LL1407: if(( unsigned int) _temp1405 >  4u?*((
int*) _temp1405) ==  Cyc_Absyn_TupleType: 0){ _LL1414: _temp1413=(( struct Cyc_Absyn_TupleType_struct*)
_temp1405)->f1; goto _LL1408;} else{ goto _LL1409;} _LL1409: if(( unsigned int)
_temp1405 >  4u?*(( int*) _temp1405) ==  Cyc_Absyn_PointerType: 0){ _LL1416:
_temp1415=(( struct Cyc_Absyn_PointerType_struct*) _temp1405)->f1; _LL1426:
_temp1425=( void*) _temp1415.elt_typ; goto _LL1424; _LL1424: _temp1423=( void*)
_temp1415.rgn_typ; goto _LL1422; _LL1422: _temp1421= _temp1415.nullable; goto
_LL1420; _LL1420: _temp1419= _temp1415.tq; goto _LL1418; _LL1418: _temp1417=
_temp1415.bounds; goto _LL1410;} else{ goto _LL1411;} _LL1411: goto _LL1412;
_LL1408: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp974) +  1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp976, Cyc_Toc_fieldname((
int) i), 0))->r)); goto _LL1406;} _LL1410:{ void* _temp1427= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1417); struct Cyc_Absyn_Exp* _temp1433; _LL1429: if((
unsigned int) _temp1427 >  1u?*(( int*) _temp1427) ==  Cyc_Absyn_Upper_b: 0){
_LL1434: _temp1433=(( struct Cyc_Absyn_Upper_b_struct*) _temp1427)->f1; goto
_LL1430;} else{ goto _LL1431;} _LL1431: if( _temp1427 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL1432;} else{ goto _LL1428;} _LL1430: _temp1433= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp(
_temp1433), 0);{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_def)( 0, _temp1421); void* ta1= Cyc_Toc_typ_to_c( _temp1425);
void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1419); if( possibly_null){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({ struct Cyc_Absyn_Exp* _temp1435[ 4u];
_temp1435[ 3u]= _temp974; _temp1435[ 2u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0);
_temp1435[ 1u]= _temp1433; _temp1435[ 0u]= _temp976;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1435, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0), 0), 0))->r));} else{( void*)( _temp974->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_notnull_e,({ struct Cyc_Absyn_Exp* _temp1436[ 2u];
_temp1436[ 1u]= Cyc_Absyn_copy_exp( _temp974); _temp1436[ 0u]= _temp1433;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1436, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r));} goto _LL1428;}
_LL1432: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1425);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1,
_temp1419), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({ struct
Cyc_Absyn_Exp* _temp1437[ 3u]; _temp1437[ 2u]= _temp974; _temp1437[ 1u]= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); _temp1437[ 0u]= _temp976;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1437, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1428;} _LL1428:;} goto _LL1406; _LL1412:({
void* _temp1438[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1438, sizeof( void*), 0u));}); goto
_LL1406; _LL1406:;} goto _LL803;} _LL855: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp978))->r));} else{ struct
Cyc_List_List* _temp1439=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp978); struct _tagged_arr* _temp1440= Cyc_Toc_add_tuple_type( _temp1439);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp978 !=  0;( _temp978=((
struct Cyc_List_List*) _check_null( _temp978))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp978))->hd);
dles=({ struct Cyc_List_List* _temp1441=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1441->hd=( void*)({ struct _tuple4*
_temp1442=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp1442->f1=
0; _temp1442->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp978))->hd; _temp1442;}); _temp1441->tl= dles; _temp1441;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL803; _LL857:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp980, 0))->r));{ struct Cyc_List_List* _temp1443= _temp980; for( 0;
_temp1443 !=  0; _temp1443=(( struct Cyc_List_List*) _check_null( _temp1443))->tl){
struct _tuple4 _temp1446; struct Cyc_Absyn_Exp* _temp1447; struct _tuple4*
_temp1444=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1443))->hd;
_temp1446=* _temp1444; _LL1448: _temp1447= _temp1446.f2; goto _LL1445; _LL1445:
Cyc_Toc_exp_to_c( nv, _temp1447);}} goto _LL803; _LL859: { unsigned int
_temp1449= Cyc_Evexp_eval_const_uint_exp( _temp984); Cyc_Toc_exp_to_c( nv,
_temp982);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp982)){
unsigned int i= 0; for( 0; i <  _temp1449; i ++){ es=({ struct Cyc_List_List*
_temp1450=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1450->hd=( void*)({ struct _tuple4* _temp1451=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1451->f1= 0; _temp1451->f2= _temp982; _temp1451;});
_temp1450->tl= es; _temp1450;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL803;}} _LL861: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp990, _temp994))->r));}
else{ if( _temp988 ==  0){({ void* _temp1452[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer",
sizeof( unsigned char), 37u), _tag_arr( _temp1452, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp988); struct _RegionHandle _temp1453= _new_region(); struct _RegionHandle*
rgn=& _temp1453; _push_region( rgn);{ struct Cyc_List_List* _temp1454=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp990,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1455= 0;{ struct Cyc_List_List*
_temp1456=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1456 !=  0; _temp1456=(( struct Cyc_List_List*) _check_null(
_temp1456))->tl){ struct Cyc_List_List* _temp1457= _temp1454; for( 0; _temp1457
!=  0; _temp1457=(( struct Cyc_List_List*) _check_null( _temp1457))->tl){ if((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1457))->hd)).f1 == (
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1456))->hd){
struct _tuple10 _temp1460; struct Cyc_Absyn_Exp* _temp1461; struct Cyc_Absyn_Structfield*
_temp1463; struct _tuple10* _temp1458=( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( _temp1457))->hd; _temp1460=* _temp1458; _LL1464: _temp1463=
_temp1460.f1; goto _LL1462; _LL1462: _temp1461= _temp1460.f2; goto _LL1459;
_LL1459: { void* _temp1465=( void*) _temp1463->type; Cyc_Toc_exp_to_c( nv,
_temp1461); if( Cyc_Toc_is_void_star( _temp1465)){( void*)( _temp1461->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1461->r, 0), 0))->r));} _temp1455=({ struct Cyc_List_List* _temp1466=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1466->hd=(
void*)({ struct _tuple4* _temp1467=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1467->f1= 0; _temp1467->f2= _temp1461; _temp1467;}); _temp1466->tl=
_temp1455; _temp1466;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1455), 0))->r));}; _pop_region( rgn);}} goto _LL803; _LL863: { struct Cyc_List_List*
fs;{ void* _temp1468= Cyc_Tcutil_compress( _temp998); struct Cyc_List_List*
_temp1474; _LL1470: if(( unsigned int) _temp1468 >  4u?*(( int*) _temp1468) == 
Cyc_Absyn_AnonStructType: 0){ _LL1475: _temp1474=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1468)->f1; goto _LL1471;} else{ goto _LL1472;} _LL1472: goto _LL1473;
_LL1471: fs= _temp1474; goto _LL1469; _LL1473: fs=({ struct Cyc_Std_String_pa_struct
_temp1477; _temp1477.tag= Cyc_Std_String_pa; _temp1477.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp998);{ void* _temp1476[ 1u]={& _temp1477};(( struct
Cyc_List_List*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("anon struct has type %s", sizeof( unsigned char), 24u), _tag_arr(
_temp1476, sizeof( void*), 1u));}}); goto _LL1469; _LL1469:;}{ struct
_RegionHandle _temp1478= _new_region(); struct _RegionHandle* rgn=& _temp1478;
_push_region( rgn);{ struct Cyc_List_List* _temp1479=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp996, fs); for( 0; _temp1479 !=  0; _temp1479=(( struct Cyc_List_List*)
_check_null( _temp1479))->tl){ struct _tuple10 _temp1482; struct Cyc_Absyn_Exp*
_temp1483; struct Cyc_Absyn_Structfield* _temp1485; struct _tuple10* _temp1480=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1479))->hd;
_temp1482=* _temp1480; _LL1486: _temp1485= _temp1482.f1; goto _LL1484; _LL1484:
_temp1483= _temp1482.f2; goto _LL1481; _LL1481: { void* _temp1487=( void*)
_temp1485->type; Cyc_Toc_exp_to_c( nv, _temp1483); if( Cyc_Toc_is_void_star(
_temp1487)){( void*)( _temp1483->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1483->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp996, 0))->r));}; _pop_region( rgn);}
goto _LL803;} _LL865: { struct _tuple0* qv= _temp1000->name; if( _temp1002->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1488= _temp1002->fields ==  0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1002->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1488))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1488))->hd)->typs ==  0){ tag_count ++;}
_temp1488=(( struct Cyc_List_List*) _check_null( _temp1488))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL803;} _LL867: { struct _tuple0* _temp1489= _temp1006->name; struct Cyc_List_List*
_temp1490= _temp1006->typs; struct _tuple0* _temp1491= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1492= Cyc_Absyn_var_exp( _temp1491, 0); void*
_temp1493= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1489, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1008->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1489, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1494= _temp1008->fields ==  0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1008->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1489,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1494))->hd)->name) !=  0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1494))->hd)->typs !=  0){ tag_count ++;}
_temp1494=(( struct Cyc_List_List*) _check_null( _temp1494))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1010
!=  0;( _temp1010=(( struct Cyc_List_List*) _check_null( _temp1010))->tl,
_temp1490=(( struct Cyc_List_List*) _check_null( _temp1490))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1010))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1490))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1495=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1495->hd=( void*)({ struct _tuple4* _temp1496=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp1496->f1= 0; _temp1496->f2= cur_e; _temp1496;});
_temp1495->tl= dles; _temp1495;});} dles=({ struct Cyc_List_List* _temp1497=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1497->hd=(
void*)({ struct _tuple4* _temp1498=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp1498->f1= 0; _temp1498->f2= tag_exp; _temp1498;}); _temp1497->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1497;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1499=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1499->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1489,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1499;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1500=({ struct Cyc_List_List* _temp1506=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1506->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1492, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1489, 0), 0); _temp1506->tl= 0; _temp1506;});{ int i=
1; for( 0; _temp1010 !=  0;((( _temp1010=(( struct Cyc_List_List*) _check_null(
_temp1010))->tl, i ++)), _temp1490=(( struct Cyc_List_List*) _check_null(
_temp1490))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1010))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1490))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp1501= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1492, Cyc_Toc_fieldname( i), 0), cur_e, 0);
_temp1500=({ struct Cyc_List_List* _temp1502=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1502->hd=( void*) _temp1501; _temp1502->tl=
_temp1500; _temp1502;});}}}{ struct Cyc_Absyn_Stmt* _temp1503= Cyc_Absyn_exp_stmt(
_temp1492, 0); struct Cyc_Absyn_Stmt* _temp1504= Cyc_Absyn_seq_stmts((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List*
_temp1505=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1505->hd=( void*) _temp1503; _temp1505->tl= _temp1500; _temp1505;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1491, _temp1493, 0, _temp1504, 0), 0))->r));}} goto _LL803;} _LL869: goto
_LL803; _LL871: { void* t_c= Cyc_Toc_typ_to_c( _temp1012); if( _temp1014 !=  0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1014); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1012, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL803;} _LL873: Cyc_Toc_stmt_to_c(
nv, _temp1016); goto _LL803; _LL875:({ void* _temp1507[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem",
sizeof( unsigned char), 14u), _tag_arr( _temp1507, sizeof( void*), 0u));}); goto
_LL803; _LL877:({ void* _temp1508[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u), _tag_arr( _temp1508, sizeof( void*), 0u));}); goto _LL803;
_LL879:({ void* _temp1509[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u),
_tag_arr( _temp1509, sizeof( void*), 0u));}); goto _LL803; _LL881:({ void*
_temp1510[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("fill", sizeof( unsigned char), 5u), _tag_arr( _temp1510, sizeof( void*),
0u));}); goto _LL803; _LL803:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2; } ;
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1511=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1547; struct Cyc_Absyn_Vardecl _temp1549; struct _tuple0* _temp1550; struct
Cyc_Absyn_Vardecl* _temp1552; int _temp1554; void* _temp1556; unsigned char
_temp1558; struct _tagged_arr _temp1560; struct Cyc_Absyn_Enumfield* _temp1562;
struct Cyc_Absyn_Enumdecl* _temp1564; struct Cyc_List_List* _temp1566; struct
Cyc_List_List* _temp1568; struct Cyc_Absyn_Tunionfield* _temp1570; struct Cyc_Absyn_Tuniondecl*
_temp1572; struct Cyc_Absyn_Pat* _temp1575; struct Cyc_Absyn_Pat _temp1577; void*
_temp1578; struct Cyc_List_List* _temp1580; struct Cyc_List_List* _temp1582;
struct Cyc_Absyn_Tunionfield* _temp1584; struct Cyc_Absyn_Tuniondecl* _temp1586;
struct Cyc_List_List* _temp1588; struct Cyc_List_List* _temp1590; struct Cyc_List_List*
_temp1592; struct Cyc_List_List* _temp1594; struct Cyc_Core_Opt* _temp1596;
struct Cyc_Absyn_Structdecl* _temp1598; struct Cyc_Absyn_Pat* _temp1600; _LL1513:
if(( unsigned int) _temp1511 >  2u?*(( int*) _temp1511) ==  Cyc_Absyn_Var_p: 0){
_LL1548: _temp1547=(( struct Cyc_Absyn_Var_p_struct*) _temp1511)->f1; _temp1549=*
_temp1547; _LL1551: _temp1550= _temp1549.name; goto _LL1514;} else{ goto _LL1515;}
_LL1515: if( _temp1511 == ( void*) Cyc_Absyn_Wild_p){ goto _LL1516;} else{ goto
_LL1517;} _LL1517: if(( unsigned int) _temp1511 >  2u?*(( int*) _temp1511) == 
Cyc_Absyn_Reference_p: 0){ _LL1553: _temp1552=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1511)->f1; goto _LL1518;} else{ goto _LL1519;} _LL1519: if( _temp1511 == (
void*) Cyc_Absyn_Null_p){ goto _LL1520;} else{ goto _LL1521;} _LL1521: if((
unsigned int) _temp1511 >  2u?*(( int*) _temp1511) ==  Cyc_Absyn_Int_p: 0){
_LL1557: _temp1556=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1511)->f1;
goto _LL1555; _LL1555: _temp1554=(( struct Cyc_Absyn_Int_p_struct*) _temp1511)->f2;
goto _LL1522;} else{ goto _LL1523;} _LL1523: if(( unsigned int) _temp1511 >  2u?*((
int*) _temp1511) ==  Cyc_Absyn_Char_p: 0){ _LL1559: _temp1558=(( struct Cyc_Absyn_Char_p_struct*)
_temp1511)->f1; goto _LL1524;} else{ goto _LL1525;} _LL1525: if(( unsigned int)
_temp1511 >  2u?*(( int*) _temp1511) ==  Cyc_Absyn_Float_p: 0){ _LL1561:
_temp1560=(( struct Cyc_Absyn_Float_p_struct*) _temp1511)->f1; goto _LL1526;}
else{ goto _LL1527;} _LL1527: if(( unsigned int) _temp1511 >  2u?*(( int*)
_temp1511) ==  Cyc_Absyn_Enum_p: 0){ _LL1565: _temp1564=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1511)->f1; goto _LL1563; _LL1563: _temp1562=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1511)->f2; goto _LL1528;} else{ goto _LL1529;} _LL1529: if(( unsigned int)
_temp1511 >  2u?*(( int*) _temp1511) ==  Cyc_Absyn_Tunion_p: 0){ _LL1573:
_temp1572=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1511)->f1; goto _LL1571;
_LL1571: _temp1570=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1511)->f2; goto
_LL1569; _LL1569: _temp1568=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1511)->f3;
goto _LL1567; _LL1567: _temp1566=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1511)->f4;
if( _temp1566 ==  0){ goto _LL1530;} else{ goto _LL1531;}} else{ goto _LL1531;}
_LL1531: if(( unsigned int) _temp1511 >  2u?*(( int*) _temp1511) ==  Cyc_Absyn_Pointer_p:
0){ _LL1576: _temp1575=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1511)->f1;
_temp1577=* _temp1575; _LL1579: _temp1578=( void*) _temp1577.r; if((
unsigned int) _temp1578 >  2u?*(( int*) _temp1578) ==  Cyc_Absyn_Tunion_p: 0){
_LL1587: _temp1586=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1578)->f1; goto
_LL1585; _LL1585: _temp1584=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1578)->f2;
goto _LL1583; _LL1583: _temp1582=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1578)->f3;
goto _LL1581; _LL1581: _temp1580=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1578)->f4;
goto _LL1574;} else{ goto _LL1533;}} else{ goto _LL1533;} _LL1574: if( _temp1580
!=  0){ goto _LL1532;} else{ goto _LL1533;} _LL1533: if(( unsigned int)
_temp1511 >  2u?*(( int*) _temp1511) ==  Cyc_Absyn_Tunion_p: 0){ _LL1589:
_temp1588=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1511)->f4; goto _LL1534;}
else{ goto _LL1535;} _LL1535: if(( unsigned int) _temp1511 >  2u?*(( int*)
_temp1511) ==  Cyc_Absyn_Tuple_p: 0){ _LL1591: _temp1590=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1511)->f1; goto _LL1536;} else{ goto _LL1537;} _LL1537: if(( unsigned int)
_temp1511 >  2u?*(( int*) _temp1511) ==  Cyc_Absyn_Struct_p: 0){ _LL1599:
_temp1598=(( struct Cyc_Absyn_Struct_p_struct*) _temp1511)->f1; goto _LL1597;
_LL1597: _temp1596=(( struct Cyc_Absyn_Struct_p_struct*) _temp1511)->f2; goto
_LL1595; _LL1595: _temp1594=(( struct Cyc_Absyn_Struct_p_struct*) _temp1511)->f3;
goto _LL1593; _LL1593: _temp1592=(( struct Cyc_Absyn_Struct_p_struct*) _temp1511)->f4;
goto _LL1538;} else{ goto _LL1539;} _LL1539: if(( unsigned int) _temp1511 >  2u?*((
int*) _temp1511) ==  Cyc_Absyn_Pointer_p: 0){ _LL1601: _temp1600=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1511)->f1; goto _LL1540;} else{ goto _LL1541;} _LL1541: if(( unsigned int)
_temp1511 >  2u?*(( int*) _temp1511) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL1542;}
else{ goto _LL1543;} _LL1543: if(( unsigned int) _temp1511 >  2u?*(( int*)
_temp1511) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL1544;} else{ goto _LL1545;}
_LL1545: if(( unsigned int) _temp1511 >  2u?*(( int*) _temp1511) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL1546;} else{ goto _LL1512;} _LL1514: nv= Cyc_Toc_add_varmap( nv,
_temp1550, r); goto _LL1516; _LL1516: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto
_LL1512; _LL1518: { struct _tuple0* _temp1602= Cyc_Toc_temp_var(); decls=({
struct Cyc_List_List* _temp1603=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1603->hd=( void*)({ struct _tuple12*
_temp1604=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1604->f1= _temp1602; _temp1604->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
t), Cyc_Toc_mt_tq); _temp1604;}); _temp1603->tl= decls; _temp1603;}); nv= Cyc_Toc_add_varmap(
nv, _temp1552->name, Cyc_Absyn_var_exp( _temp1602, 0)); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1602, 0), Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq), Cyc_Absyn_address_exp( path, 0), 0), 0),
Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1512;} _LL1520: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab); goto _LL1512; _LL1522:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp( _temp1556, _temp1554, 0),
succ_lab, fail_lab); goto _LL1512; _LL1524: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp(
_temp1558, 0), succ_lab, fail_lab); goto _LL1512; _LL1526: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_float_exp( _temp1560, 0), succ_lab, fail_lab); goto _LL1512;
_LL1528: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct*
_temp1605=( struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1605[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1606; _temp1606.tag= Cyc_Absyn_Enum_e;
_temp1606.f1= _temp1562->name; _temp1606.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1564; _temp1606.f3=( struct Cyc_Absyn_Enumfield*) _temp1562; _temp1606;});
_temp1605;}), 0), succ_lab, fail_lab); goto _LL1512; _LL1530: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1570->name, 0); if( ! _temp1572->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1512;} _LL1532: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1580);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1584->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1607=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1584->typs); struct Cyc_List_List*
_temp1608=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1580); for( 0; _temp1608 !=  0;((( _temp1608=((
struct Cyc_List_List*) _check_null( _temp1608))->tl, _temp1607=(( struct Cyc_List_List*)
_check_null( _temp1607))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1609=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1608))->hd;
if(( void*) _temp1609->r == ( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1610=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1607))->hd)).f2;
struct _tuple0* _temp1611= Cyc_Toc_temp_var(); void* _temp1612=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1609->topt))->v; void* _temp1613= Cyc_Toc_typ_to_c(
_temp1612); struct _tagged_arr* _temp1614= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1615= Cyc_Absyn_structarrow_exp( rcast, Cyc_Toc_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1610))){ _temp1615= Cyc_Absyn_cast_exp(
_temp1613, _temp1615, 0);} decls=({ struct Cyc_List_List* _temp1616=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1616->hd=( void*)({
struct _tuple12* _temp1617=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1617->f1= _temp1611; _temp1617->f2= _temp1613; _temp1617;});
_temp1616->tl= decls; _temp1616;});{ struct _tuple11 _temp1618= Cyc_Toc_xlate_pat(
nv, rgn, _temp1612, Cyc_Absyn_var_exp( _temp1611, 0), _temp1615, _temp1609,
succ_lab, fail_lab, decls); nv= _temp1618.f1; decls= _temp1618.f2;{ struct Cyc_Absyn_Stmt*
_temp1619= _temp1618.f3; struct Cyc_Absyn_Stmt* _temp1620= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1611, 0), _temp1615, 0); struct Cyc_Absyn_Stmt*
_temp1621= Cyc_Absyn_seq_stmt( _temp1620, _temp1619, 0); ss=({ struct Cyc_List_List*
_temp1622=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1622->hd=( void*) Cyc_Absyn_label_stmt( _temp1614, _temp1621, 0); _temp1622->tl=
ss; _temp1622;}); succ_lab= _temp1614;}}}} if( ss ==  0){ ss=({ struct Cyc_List_List*
_temp1623=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1623->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1623->tl= 0;
_temp1623;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1586->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1584->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1624=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1586->fields))->v; for( 0; _temp1624 !=  0; _temp1624=((
struct Cyc_List_List*) _check_null( _temp1624))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1625=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1624))->hd; if( _temp1625->typs ==  0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1584->name, 0); test_exp= max_tag >  0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1512;}} _LL1534: _temp1590= _temp1588; goto _LL1536; _LL1536: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1590);{ struct Cyc_List_List* _temp1626=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1590); for(
0; _temp1626 !=  0;( _temp1626=(( struct Cyc_List_List*) _check_null( _temp1626))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1627=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1626))->hd; if(( void*) _temp1627->r == ( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1628= Cyc_Toc_temp_var(); void* _temp1629=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1627->topt))->v; struct
_tagged_arr* _temp1630= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1631=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1631->hd=( void*)({ struct _tuple12* _temp1632=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1632->f1= _temp1628;
_temp1632->f2= Cyc_Toc_typ_to_c( _temp1629); _temp1632;}); _temp1631->tl= decls;
_temp1631;});{ struct _tuple11 _temp1633= Cyc_Toc_xlate_pat( nv, rgn, _temp1629,
Cyc_Absyn_var_exp( _temp1628, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp1627, succ_lab, fail_lab, decls); nv= _temp1633.f1; decls=
_temp1633.f2;{ struct Cyc_Absyn_Stmt* _temp1634= _temp1633.f3; struct Cyc_Absyn_Stmt*
_temp1635= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1628, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp1634, 0); ss=({ struct Cyc_List_List* _temp1636=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1636->hd=( void*) Cyc_Absyn_label_stmt(
_temp1630, _temp1635, 0); _temp1636->tl= ss; _temp1636;}); succ_lab= _temp1630;}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1637=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1637->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1637->tl= 0; _temp1637;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1512;} _LL1538: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1638=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1592); for( 0; _temp1638 !=  0; _temp1638=((
struct Cyc_List_List*) _check_null( _temp1638))->tl){ struct _tuple13* _temp1639=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1638))->hd; struct
Cyc_Absyn_Pat* _temp1640=(* _temp1639).f2; if(( void*) _temp1640->r == ( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1641=( void*)((
struct Cyc_List_List*) _check_null((* _temp1639).f1))->hd; struct _tagged_arr*
_temp1647; _LL1643: if(*(( int*) _temp1641) ==  Cyc_Absyn_FieldName){ _LL1648:
_temp1647=(( struct Cyc_Absyn_FieldName_struct*) _temp1641)->f1; goto _LL1644;}
else{ goto _LL1645;} _LL1645: goto _LL1646; _LL1644: f= _temp1647; goto _LL1642;
_LL1646:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1642:;}{ struct _tuple0*
_temp1649= Cyc_Toc_temp_var(); void* _temp1650=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1640->topt))->v; void* _temp1651= Cyc_Toc_typ_to_c( _temp1650);
struct _tagged_arr* _temp1652= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1653=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1653->hd=( void*)({ struct _tuple12* _temp1654=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1654->f1= _temp1649;
_temp1654->f2= _temp1651; _temp1654;}); _temp1653->tl= decls; _temp1653;});{
struct _tuple11 _temp1655= Cyc_Toc_xlate_pat( nv, rgn, _temp1650, Cyc_Absyn_var_exp(
_temp1649, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1640, succ_lab,
fail_lab, decls); nv= _temp1655.f1; decls= _temp1655.f2;{ struct Cyc_Absyn_Exp*
_temp1656= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1598->fields))->v, f)))->type)){
_temp1656= Cyc_Absyn_cast_exp( _temp1651, _temp1656, 0);}{ struct Cyc_Absyn_Stmt*
_temp1657= _temp1655.f3; struct Cyc_Absyn_Stmt* _temp1658= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1649, 0), _temp1656, 0),
_temp1657, 0); ss=({ struct Cyc_List_List* _temp1659=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1659->hd=( void*) Cyc_Absyn_label_stmt(
_temp1652, _temp1658, 0); _temp1659->tl= ss; _temp1659;}); succ_lab= _temp1652;}}}}}}}
if( ss ==  0){ ss=({ struct Cyc_List_List* _temp1660=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1660->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1660->tl= 0; _temp1660;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1512;} _LL1540: { struct _tuple0* _temp1661= Cyc_Toc_temp_var(); void*
_temp1662=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1600->topt))->v;
decls=({ struct Cyc_List_List* _temp1663=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1663->hd=( void*)({ struct _tuple12*
_temp1664=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1664->f1= _temp1661; _temp1664->f2= Cyc_Toc_typ_to_c( _temp1662); _temp1664;});
_temp1663->tl= decls; _temp1663;});{ struct _tuple11 _temp1665= Cyc_Toc_xlate_pat(
nv, rgn, _temp1662, Cyc_Absyn_var_exp( _temp1661, 0), Cyc_Absyn_deref_exp( path,
0), _temp1600, succ_lab, fail_lab, decls); nv= _temp1665.f1; decls= _temp1665.f2;{
struct Cyc_Absyn_Stmt* _temp1666= _temp1665.f3; struct Cyc_Absyn_Stmt* _temp1667=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1661, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1666, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1667, 0);} else{ s= _temp1667;} goto _LL1512;}}} _LL1542: s=({
void* _temp1668[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1668, sizeof( void*), 0u));}); goto _LL1512;
_LL1544: s=({ void* _temp1669[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1669, sizeof( void*), 0u));}); goto
_LL1512; _LL1546: s=({ void* _temp1670[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1670, sizeof( void*), 0u));}); goto
_LL1512; _LL1512:;} return({ struct _tuple11 _temp1671; _temp1671.f1= nv;
_temp1671.f2= decls; _temp1671.f3= s; _temp1671;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1672=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1672->f1= Cyc_Toc_fresh_label(); _temp1672->f2=
Cyc_Toc_fresh_label(); _temp1672->f3= sc; _temp1672;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1673=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1674= Cyc_Tcutil_compress( _temp1673); _LL1676: if(( unsigned int)
_temp1674 >  4u?*(( int*) _temp1674) ==  Cyc_Absyn_IntType: 0){ goto _LL1677;}
else{ goto _LL1678;} _LL1678: if(( unsigned int) _temp1674 >  4u?*(( int*)
_temp1674) ==  Cyc_Absyn_EnumType: 0){ goto _LL1679;} else{ goto _LL1680;}
_LL1680: goto _LL1681; _LL1677: goto _LL1679; _LL1679: leave_as_switch= 1; goto
_LL1675; _LL1681: leave_as_switch= 0; goto _LL1675; _LL1675:;}{ struct Cyc_List_List*
_temp1682= scs; for( 0; _temp1682 !=  0; _temp1682=(( struct Cyc_List_List*)
_check_null( _temp1682))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1682))->hd)->pat_vars))->v !=  0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1682))->hd)->where_clause !=  0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1683= scs; for( 0; _temp1683
!=  0; _temp1683=(( struct Cyc_List_List*) _check_null( _temp1683))->tl){ struct
Cyc_Absyn_Stmt* _temp1684=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1683))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1683))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1684, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1684);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1685= _new_region(); struct _RegionHandle* rgn=&
_temp1685; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1686= lscs; for( 0;
_temp1686 !=  0; _temp1686=(( struct Cyc_List_List*) _check_null( _temp1686))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1686))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1686))->tl ==  0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1686))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1686))->hd)).f2; if( sc->where_clause ==  0){ struct _tuple11
_temp1687= Cyc_Toc_xlate_pat( nv, rgn, _temp1673, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1688=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1688->hd=( void*)
_temp1687.f1; _temp1688->tl= nvs; _temp1688;}); decls= _temp1687.f2; test_stmts=({
struct Cyc_List_List* _temp1689=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1689->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1686))->hd)).f1,
_temp1687.f3, 0); _temp1689->tl= test_stmts; _temp1689;});} else{ struct Cyc_Absyn_Exp*
_temp1690=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1691= Cyc_Toc_fresh_label(); struct _tuple11 _temp1692= Cyc_Toc_xlate_pat(
nv, rgn, _temp1673, r, path, sc->pattern, _temp1691, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1692.f1, _temp1690); nvs=({ struct Cyc_List_List* _temp1693=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1693->hd=( void*)
_temp1692.f1; _temp1693->tl= nvs; _temp1693;}); decls= _temp1692.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1690, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1691, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1694=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1694->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1686))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1692.f3, s3, 0), 0); _temp1694->tl= test_stmts;
_temp1694;});}}}}{ struct Cyc_Absyn_Stmt* _temp1695= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs !=  0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl !=  0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1696=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1696, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1697=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1697, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1695, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls !=  0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple12 _temp1700; void* _temp1701; struct
_tuple0* _temp1703; struct _tuple12* _temp1698=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1700=* _temp1698; _LL1704: _temp1703= _temp1700.f1;
goto _LL1702; _LL1702: _temp1701= _temp1700.f2; goto _LL1699; _LL1699: res= Cyc_Absyn_declare_stmt(
_temp1703, _temp1701, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1705=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List) *  1); _temp1705[ 0]=({ struct Cyc_List_List
_temp1706; _temp1706.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n -  1), 0);
_temp1706.tl= 0; _temp1706;}); _temp1705;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n >  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1707=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1749; struct Cyc_Absyn_Stmt* _temp1751; struct Cyc_Absyn_Stmt* _temp1753;
struct Cyc_Absyn_Exp* _temp1755; struct Cyc_Absyn_Stmt* _temp1757; struct Cyc_Absyn_Stmt*
_temp1759; struct Cyc_Absyn_Exp* _temp1761; struct Cyc_Absyn_Stmt* _temp1763;
struct _tuple2 _temp1765; struct Cyc_Absyn_Exp* _temp1767; struct Cyc_Absyn_Stmt*
_temp1769; struct Cyc_Absyn_Stmt* _temp1771; struct Cyc_Absyn_Stmt* _temp1773;
struct Cyc_Absyn_Stmt* _temp1775; struct _tuple2 _temp1777; struct Cyc_Absyn_Exp*
_temp1779; struct _tuple2 _temp1781; struct Cyc_Absyn_Exp* _temp1783; struct Cyc_Absyn_Exp*
_temp1785; struct Cyc_List_List* _temp1787; struct Cyc_Absyn_Exp* _temp1789;
struct Cyc_Absyn_Switch_clause** _temp1791; struct Cyc_List_List* _temp1793;
struct Cyc_Absyn_Stmt* _temp1795; struct Cyc_Absyn_Decl* _temp1797; struct Cyc_Absyn_Stmt*
_temp1799; struct _tagged_arr* _temp1801; struct _tuple2 _temp1803; struct Cyc_Absyn_Exp*
_temp1805; struct Cyc_Absyn_Stmt* _temp1807; struct Cyc_List_List* _temp1809;
struct Cyc_Absyn_Stmt* _temp1811; struct Cyc_Absyn_Stmt* _temp1813; struct Cyc_Absyn_Vardecl*
_temp1815; struct Cyc_Absyn_Tvar* _temp1817; _LL1709: if( _temp1707 == ( void*)
Cyc_Absyn_Skip_s){ goto _LL1710;} else{ goto _LL1711;} _LL1711: if((
unsigned int) _temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_Exp_s: 0){
_LL1750: _temp1749=(( struct Cyc_Absyn_Exp_s_struct*) _temp1707)->f1; goto
_LL1712;} else{ goto _LL1713;} _LL1713: if(( unsigned int) _temp1707 >  1u?*((
int*) _temp1707) ==  Cyc_Absyn_Seq_s: 0){ _LL1754: _temp1753=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1707)->f1; goto _LL1752; _LL1752: _temp1751=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1707)->f2; goto _LL1714;} else{ goto _LL1715;} _LL1715: if(( unsigned int)
_temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_Return_s: 0){ _LL1756:
_temp1755=(( struct Cyc_Absyn_Return_s_struct*) _temp1707)->f1; goto _LL1716;}
else{ goto _LL1717;} _LL1717: if(( unsigned int) _temp1707 >  1u?*(( int*)
_temp1707) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1762: _temp1761=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1707)->f1; goto _LL1760; _LL1760: _temp1759=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1707)->f2; goto _LL1758; _LL1758: _temp1757=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1707)->f3; goto _LL1718;} else{ goto _LL1719;} _LL1719: if(( unsigned int)
_temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_While_s: 0){ _LL1766:
_temp1765=(( struct Cyc_Absyn_While_s_struct*) _temp1707)->f1; _LL1768:
_temp1767= _temp1765.f1; goto _LL1764; _LL1764: _temp1763=(( struct Cyc_Absyn_While_s_struct*)
_temp1707)->f2; goto _LL1720;} else{ goto _LL1721;} _LL1721: if(( unsigned int)
_temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_Break_s: 0){ _LL1770:
_temp1769=(( struct Cyc_Absyn_Break_s_struct*) _temp1707)->f1; goto _LL1722;}
else{ goto _LL1723;} _LL1723: if(( unsigned int) _temp1707 >  1u?*(( int*)
_temp1707) ==  Cyc_Absyn_Continue_s: 0){ _LL1772: _temp1771=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1707)->f1; goto _LL1724;} else{ goto _LL1725;} _LL1725: if(( unsigned int)
_temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_Goto_s: 0){ _LL1774:
_temp1773=(( struct Cyc_Absyn_Goto_s_struct*) _temp1707)->f2; goto _LL1726;}
else{ goto _LL1727;} _LL1727: if(( unsigned int) _temp1707 >  1u?*(( int*)
_temp1707) ==  Cyc_Absyn_For_s: 0){ _LL1786: _temp1785=(( struct Cyc_Absyn_For_s_struct*)
_temp1707)->f1; goto _LL1782; _LL1782: _temp1781=(( struct Cyc_Absyn_For_s_struct*)
_temp1707)->f2; _LL1784: _temp1783= _temp1781.f1; goto _LL1778; _LL1778:
_temp1777=(( struct Cyc_Absyn_For_s_struct*) _temp1707)->f3; _LL1780: _temp1779=
_temp1777.f1; goto _LL1776; _LL1776: _temp1775=(( struct Cyc_Absyn_For_s_struct*)
_temp1707)->f4; goto _LL1728;} else{ goto _LL1729;} _LL1729: if(( unsigned int)
_temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_Switch_s: 0){ _LL1790:
_temp1789=(( struct Cyc_Absyn_Switch_s_struct*) _temp1707)->f1; goto _LL1788;
_LL1788: _temp1787=(( struct Cyc_Absyn_Switch_s_struct*) _temp1707)->f2; goto
_LL1730;} else{ goto _LL1731;} _LL1731: if(( unsigned int) _temp1707 >  1u?*((
int*) _temp1707) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1794: _temp1793=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1707)->f1; goto _LL1792; _LL1792: _temp1791=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1707)->f2; goto _LL1732;} else{ goto _LL1733;} _LL1733: if(( unsigned int)
_temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_Decl_s: 0){ _LL1798:
_temp1797=(( struct Cyc_Absyn_Decl_s_struct*) _temp1707)->f1; goto _LL1796;
_LL1796: _temp1795=(( struct Cyc_Absyn_Decl_s_struct*) _temp1707)->f2; goto
_LL1734;} else{ goto _LL1735;} _LL1735: if(( unsigned int) _temp1707 >  1u?*((
int*) _temp1707) ==  Cyc_Absyn_Label_s: 0){ _LL1802: _temp1801=(( struct Cyc_Absyn_Label_s_struct*)
_temp1707)->f1; goto _LL1800; _LL1800: _temp1799=(( struct Cyc_Absyn_Label_s_struct*)
_temp1707)->f2; goto _LL1736;} else{ goto _LL1737;} _LL1737: if(( unsigned int)
_temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_Do_s: 0){ _LL1808: _temp1807=((
struct Cyc_Absyn_Do_s_struct*) _temp1707)->f1; goto _LL1804; _LL1804: _temp1803=((
struct Cyc_Absyn_Do_s_struct*) _temp1707)->f2; _LL1806: _temp1805= _temp1803.f1;
goto _LL1738;} else{ goto _LL1739;} _LL1739: if(( unsigned int) _temp1707 >  1u?*((
int*) _temp1707) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1812: _temp1811=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1707)->f1; goto _LL1810; _LL1810: _temp1809=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1707)->f2; goto _LL1740;} else{ goto _LL1741;} _LL1741: if(( unsigned int)
_temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_Region_s: 0){ _LL1818:
_temp1817=(( struct Cyc_Absyn_Region_s_struct*) _temp1707)->f1; goto _LL1816;
_LL1816: _temp1815=(( struct Cyc_Absyn_Region_s_struct*) _temp1707)->f2; goto
_LL1814; _LL1814: _temp1813=(( struct Cyc_Absyn_Region_s_struct*) _temp1707)->f3;
goto _LL1742;} else{ goto _LL1743;} _LL1743: if(( unsigned int) _temp1707 >  1u?*((
int*) _temp1707) ==  Cyc_Absyn_SwitchC_s: 0){ goto _LL1744;} else{ goto _LL1745;}
_LL1745: if(( unsigned int) _temp1707 >  1u?*(( int*) _temp1707) ==  Cyc_Absyn_Cut_s:
0){ goto _LL1746;} else{ goto _LL1747;} _LL1747: if(( unsigned int) _temp1707 > 
1u?*(( int*) _temp1707) ==  Cyc_Absyn_Splice_s: 0){ goto _LL1748;} else{ goto
_LL1708;} _LL1710: return; _LL1712: Cyc_Toc_exp_to_c( nv, _temp1749); return;
_LL1714: Cyc_Toc_stmt_to_c( nv, _temp1753); s= _temp1751; continue; _LL1716: {
struct Cyc_Core_Opt* topt= 0; if( _temp1755 !=  0){ topt=({ struct Cyc_Core_Opt*
_temp1819=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1819->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp1755))->topt))->v);
_temp1819;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp1755));} if( s->try_depth >  0){ if( topt !=  0){ struct _tuple0* _temp1820=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp1821= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp1820, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp1820,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp1755, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1821, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1718: Cyc_Toc_exp_to_c( nv, _temp1761); Cyc_Toc_stmt_to_c( nv,
_temp1759); s= _temp1757; continue; _LL1720: Cyc_Toc_exp_to_c( nv, _temp1767);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1763); return; _LL1722: if( nv->break_lab
!=  0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{
int dest_depth= _temp1769 ==  0? 0:(( struct Cyc_Absyn_Stmt*) _check_null(
_temp1769))->try_depth; Cyc_Toc_do_npop_before( s->try_depth -  dest_depth, s);
return;} _LL1724: if( nv->continue_lab !=  0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_goto_stmt(( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
nv->continue_lab))->v, 0))->r));} _temp1773= _temp1771; goto _LL1726; _LL1726:
Cyc_Toc_do_npop_before( s->try_depth - (( struct Cyc_Absyn_Stmt*) _check_null(
_temp1773))->try_depth, s); return; _LL1728: Cyc_Toc_exp_to_c( nv, _temp1785);
Cyc_Toc_exp_to_c( nv, _temp1783); Cyc_Toc_exp_to_c( nv, _temp1779); Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1775); return; _LL1730: Cyc_Toc_xlate_switch( nv, s,
_temp1789, _temp1787); return; _LL1732: if( nv->fallthru_info ==  0){( int)
_throw(({ void* _temp1822[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u), _tag_arr( _temp1822, sizeof( void*), 0u));}));}{
struct _tuple6 _temp1825; struct Cyc_Dict_Dict* _temp1826; struct Cyc_List_List*
_temp1828; struct _tagged_arr* _temp1830; struct _tuple6* _temp1823=( struct
_tuple6*)(( struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp1825=*
_temp1823; _LL1831: _temp1830= _temp1825.f1; goto _LL1829; _LL1829: _temp1828=
_temp1825.f2; goto _LL1827; _LL1827: _temp1826= _temp1825.f3; goto _LL1824;
_LL1824: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp1830, 0); Cyc_Toc_do_npop_before(
s->try_depth - ((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp1791)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp1832=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1828); struct Cyc_List_List* _temp1833=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp1793); for( 0; _temp1832 !=  0;(
_temp1832=(( struct Cyc_List_List*) _check_null( _temp1832))->tl, _temp1833=((
struct Cyc_List_List*) _check_null( _temp1833))->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1833))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( _temp1826,(
struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp1832))->hd),( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1833))->hd, 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL1734:{ void* _temp1834=(
void*) _temp1797->r; struct Cyc_Absyn_Vardecl* _temp1844; int _temp1846; struct
Cyc_Absyn_Exp* _temp1848; struct Cyc_Core_Opt* _temp1850; struct Cyc_Absyn_Pat*
_temp1852; struct Cyc_List_List* _temp1854; _LL1836: if(*(( int*) _temp1834) == 
Cyc_Absyn_Var_d){ _LL1845: _temp1844=(( struct Cyc_Absyn_Var_d_struct*)
_temp1834)->f1; goto _LL1837;} else{ goto _LL1838;} _LL1838: if(*(( int*)
_temp1834) ==  Cyc_Absyn_Let_d){ _LL1853: _temp1852=(( struct Cyc_Absyn_Let_d_struct*)
_temp1834)->f1; goto _LL1851; _LL1851: _temp1850=(( struct Cyc_Absyn_Let_d_struct*)
_temp1834)->f3; goto _LL1849; _LL1849: _temp1848=(( struct Cyc_Absyn_Let_d_struct*)
_temp1834)->f4; goto _LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_Let_d_struct*)
_temp1834)->f5; goto _LL1839;} else{ goto _LL1840;} _LL1840: if(*(( int*)
_temp1834) ==  Cyc_Absyn_Letv_d){ _LL1855: _temp1854=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1834)->f1; goto _LL1841;} else{ goto _LL1842;} _LL1842: goto _LL1843;
_LL1837: { struct Cyc_Toc_Env* _temp1856= Cyc_Toc_add_varmap( nv, _temp1844->name,
Cyc_Absyn_varb_exp( _temp1844->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1857=( struct Cyc_Absyn_Local_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1857[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1858; _temp1858.tag= Cyc_Absyn_Local_b;
_temp1858.f1= _temp1844; _temp1858;}); _temp1857;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1856, _temp1856, _temp1844, _temp1795); goto _LL1835;} _LL1839:{ void*
_temp1859=( void*) _temp1852->r; struct Cyc_Absyn_Vardecl* _temp1865; _LL1861:
if(( unsigned int) _temp1859 >  2u?*(( int*) _temp1859) ==  Cyc_Absyn_Var_p: 0){
_LL1866: _temp1865=(( struct Cyc_Absyn_Var_p_struct*) _temp1859)->f1; goto
_LL1862;} else{ goto _LL1863;} _LL1863: goto _LL1864; _LL1862: { struct _tuple0*
old_name= _temp1865->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1865->name= new_name; _temp1865->initializer=( struct Cyc_Absyn_Exp*)
_temp1848;( void*)( _temp1797->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1867=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1867[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1868; _temp1868.tag= Cyc_Absyn_Var_d;
_temp1868.f1= _temp1865; _temp1868;}); _temp1867;})));{ struct Cyc_Toc_Env*
_temp1869= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1870=( struct Cyc_Absyn_Local_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1870[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1871; _temp1871.tag= Cyc_Absyn_Local_b; _temp1871.f1=
_temp1865; _temp1871;}); _temp1870;}), 0)); Cyc_Toc_local_decl_to_c( _temp1869,
nv, _temp1865, _temp1795); goto _LL1860;}} _LL1864:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1852, _temp1850, _temp1848, _temp1846,
_temp1795))->r)); goto _LL1860; _LL1860:;} goto _LL1835; _LL1841: { struct Cyc_List_List*
_temp1872=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1854); if( _temp1872 ==  0){({ void* _temp1873[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1873, sizeof( void*), 0u));});}(
void*)( _temp1797->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1874=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1874[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1875; _temp1875.tag= Cyc_Absyn_Var_d;
_temp1875.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1872))->hd; _temp1875;}); _temp1874;}))); _temp1872=(( struct Cyc_List_List*)
_check_null( _temp1872))->tl; for( 0; _temp1872 !=  0; _temp1872=(( struct Cyc_List_List*)
_check_null( _temp1872))->tl){ struct Cyc_Absyn_Decl* _temp1876= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1877=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1877[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1878; _temp1878.tag= Cyc_Absyn_Var_d; _temp1878.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1872))->hd; _temp1878;}); _temp1877;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1876, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1835;} _LL1843:({
void* _temp1879[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1879, sizeof( void*), 0u));}); goto _LL1835;
_LL1835:;} return; _LL1736: s= _temp1799; continue; _LL1738: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1807); Cyc_Toc_exp_to_c( nv, _temp1805); return;
_LL1740: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1880=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1880->v=( void*) e_typ; _temp1880;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1811);{
struct Cyc_Absyn_Stmt* _temp1881= Cyc_Absyn_seq_stmt( _temp1811, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1882= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1883= Cyc_Absyn_var_exp(
_temp1882, 0); struct Cyc_Absyn_Vardecl* _temp1884= Cyc_Absyn_new_vardecl(
_temp1882, Cyc_Absyn_exn_typ, 0); _temp1883->topt=({ struct Cyc_Core_Opt*
_temp1885=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1885->v=( void*) Cyc_Absyn_exn_typ; _temp1885;});{ struct Cyc_Absyn_Pat*
_temp1886=({ struct Cyc_Absyn_Pat* _temp1902=( struct Cyc_Absyn_Pat*) _cycalloc(
sizeof( struct Cyc_Absyn_Pat)); _temp1902->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1904=( struct Cyc_Absyn_Var_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1904[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp1905; _temp1905.tag= Cyc_Absyn_Var_p;
_temp1905.f1= _temp1884; _temp1905;}); _temp1904;})); _temp1902->topt=({ struct
Cyc_Core_Opt* _temp1903=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1903->v=( void*) Cyc_Absyn_exn_typ; _temp1903;}); _temp1902->loc= 0;
_temp1902;}); struct Cyc_Absyn_Exp* _temp1887= Cyc_Absyn_throw_exp( _temp1883, 0);
_temp1887->topt=({ struct Cyc_Core_Opt* _temp1888=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1888->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp1888;});{ struct Cyc_Absyn_Stmt* _temp1889= Cyc_Absyn_exp_stmt( _temp1887,
0); struct Cyc_Absyn_Switch_clause* _temp1890=({ struct Cyc_Absyn_Switch_clause*
_temp1899=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp1899->pattern= _temp1886; _temp1899->pat_vars=({ struct Cyc_Core_Opt*
_temp1900=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1900->v=( void*)({ struct Cyc_List_List* _temp1901=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1901->hd=( void*) _temp1884;
_temp1901->tl= 0; _temp1901;}); _temp1900;}); _temp1899->where_clause= 0;
_temp1899->body= _temp1889; _temp1899->loc= 0; _temp1899;}); struct Cyc_Absyn_Stmt*
_temp1891= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1809,({ struct Cyc_List_List*
_temp1898=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1898->hd=( void*) _temp1890; _temp1898->tl= 0; _temp1898;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp1891);{ struct Cyc_Absyn_Exp* _temp1892= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp1897=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1897->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1897->tl= 0; _temp1897;}), 0); struct Cyc_Absyn_Stmt*
_temp1893= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp1896=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1896->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp1896->tl= 0; _temp1896;}), 0), 0); struct Cyc_Absyn_Exp* _temp1894= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp1895= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp1893, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp1894, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp1892, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp1895, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1881, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp1891, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1742: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1815->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1813);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_Absyn_Exp* _temp1906[ 1u]; _temp1906[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1906, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), Cyc_Absyn_seq_stmt(
_temp1813, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp* _temp1907[ 1u]; _temp1907[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1907, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), 0), 0), 0), 0))->r)); return;} _LL1744:({ void* _temp1908[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("switchC_s", sizeof( unsigned char), 10u), _tag_arr( _temp1908, sizeof(
void*), 0u));}); return; _LL1746:({ void* _temp1909[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof(
unsigned char), 4u), _tag_arr( _temp1909, sizeof( void*), 0u));}); return;
_LL1748:({ void* _temp1910[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof( unsigned char), 7u),
_tag_arr( _temp1910, sizeof( void*), 0u));}); return; _LL1708:;}} struct
_tuple15{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp1911= f->args; for( 0;
_temp1911 !=  0; _temp1911=(( struct Cyc_List_List*) _check_null( _temp1911))->tl){(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1911))->hd)).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1911))->hd)).f3);{ struct _tuple0* _temp1912=({ struct _tuple0* _temp1913=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp1913->f1=( void*) Cyc_Absyn_Loc_n;
_temp1913->f2=(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1911))->hd)).f1; _temp1913;}); nv= Cyc_Toc_add_varmap( nv, _temp1912, Cyc_Absyn_var_exp(
_temp1912, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name !=  0: 0){ int _temp1916; void* _temp1918;
struct Cyc_Absyn_Tqual _temp1920; struct Cyc_Core_Opt* _temp1922; struct Cyc_Absyn_VarargInfo
_temp1914=*(( struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs));
_LL1923: _temp1922= _temp1914.name; goto _LL1921; _LL1921: _temp1920= _temp1914.tq;
goto _LL1919; _LL1919: _temp1918=( void*) _temp1914.type; goto _LL1917; _LL1917:
_temp1916= _temp1914.inject; goto _LL1915; _LL1915: { void* _temp1924= Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ( _temp1918,( void*) Cyc_Absyn_HeapRgn, _temp1920)); struct
_tuple0* _temp1925=({ struct _tuple0* _temp1928=( struct _tuple0*) _cycalloc(
sizeof( struct _tuple0)); _temp1928->f1=( void*) Cyc_Absyn_Loc_n; _temp1928->f2=(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp1922))->v;
_temp1928;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp1926=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1926->hd=( void*)({ struct _tuple15* _temp1927=( struct _tuple15*)
_cycalloc( sizeof( struct _tuple15)); _temp1927->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp1922))->v; _temp1927->f2= _temp1920;
_temp1927->f3= _temp1924; _temp1927;}); _temp1926->tl= 0; _temp1926;})); nv= Cyc_Toc_add_varmap(
nv, _temp1925, Cyc_Absyn_var_exp( _temp1925, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp1929=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp1929 !=  0; _temp1929=((
struct Cyc_List_List*) _check_null( _temp1929))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1929))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1929))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp1930= s; _LL1932: if(
_temp1930 == ( void*) Cyc_Absyn_Abstract){ goto _LL1933;} else{ goto _LL1934;}
_LL1934: if( _temp1930 == ( void*) Cyc_Absyn_ExternC){ goto _LL1935;} else{ goto
_LL1936;} _LL1936: goto _LL1937; _LL1933: return( void*) Cyc_Absyn_Public;
_LL1935: return( void*) Cyc_Absyn_Extern; _LL1937: return s; _LL1931:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp1938=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 
0){ Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1939=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1939->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp1939;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp1940=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp1938).f2); if( _temp1940 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp1938).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1940))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp1938).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp1941=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1941->v=(
void*) _temp1938; _temp1941;}); if( s->fields !=  0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp1942=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp1942 !=  0; _temp1942=(( struct Cyc_List_List*)
_check_null( _temp1942))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1942))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1942))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp1943=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far ==  0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp1944=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1944->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);
_temp1944;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp1945=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp1943).f2); if( _temp1945 ==  0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp1943).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1945))->v)->fields
==  0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp1943).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp1946=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1946->v=(
void*) _temp1943; _temp1946;}); if( u->fields !=  0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp1947=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp1947 !=  0; _temp1947=(( struct Cyc_List_List*)
_check_null( _temp1947))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1947))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1947))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far ==  0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp1948=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1948->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp1948;});}{ struct
_tuple0* _temp1949= tud->name; if( tud->fields ==  0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp1949)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp1949));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp1950=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp1950 !=  0; _temp1950=(( struct Cyc_List_List*) _check_null(
_temp1950))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1950))->hd; if( f->typs ==  0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1951; _temp1951.q_const= 1;
_temp1951.q_volatile= 0; _temp1951.q_restrict= 0; _temp1951;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1952=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1952->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1953=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1953[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1954; _temp1954.tag= Cyc_Absyn_Var_d; _temp1954.f1= vd; _temp1954;});
_temp1953;}), 0); _temp1952->tl= Cyc_Toc_result_decls; _temp1952;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1955; _temp1955.q_const=
1; _temp1955.q_volatile= 0; _temp1955.q_restrict= 0; _temp1955;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1956=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1956->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1957=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1957[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1958; _temp1958.tag= Cyc_Absyn_Var_d; _temp1958.f1= vd; _temp1958;});
_temp1957;}), 0); _temp1956->tl= Cyc_Toc_result_decls; _temp1956;});{ struct Cyc_List_List*
_temp1959= 0; int i= 1;{ struct Cyc_List_List* _temp1960= f->typs; for( 0;
_temp1960 !=  0;( _temp1960=(( struct Cyc_List_List*) _check_null( _temp1960))->tl,
i ++)){ struct _tagged_arr* _temp1961= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp1962=({ struct Cyc_Absyn_Structfield* _temp1964=( struct Cyc_Absyn_Structfield*)
_cycalloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1964->name= _temp1961;
_temp1964->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp1960))->hd)).f1; _temp1964->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1960))->hd)).f2);
_temp1964->width= 0; _temp1964->attributes= 0; _temp1964;}); _temp1959=({ struct
Cyc_List_List* _temp1963=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1963->hd=( void*) _temp1962; _temp1963->tl= _temp1959; _temp1963;});}}
_temp1959=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1959); _temp1959=({ struct Cyc_List_List* _temp1965=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1965->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp1966=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1966->name= Cyc_Toc_tag_sp; _temp1966->tq= Cyc_Toc_mt_tq; _temp1966->type=(
void*) Cyc_Absyn_sint_t; _temp1966->width= 0; _temp1966->attributes= 0;
_temp1966;}); _temp1965->tl= _temp1959; _temp1965;});{ struct Cyc_Absyn_Structdecl*
_temp1967=({ struct Cyc_Absyn_Structdecl* _temp1971=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp1971->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp1971->name=({ struct Cyc_Core_Opt* _temp1973=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1973->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp1973;}); _temp1971->tvs= 0; _temp1971->fields=({ struct Cyc_Core_Opt*
_temp1972=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1972->v=( void*) _temp1959; _temp1972;}); _temp1971->attributes= 0;
_temp1971;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp1968=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1968->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp1969=(
struct Cyc_Absyn_Struct_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1969[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp1970; _temp1970.tag= Cyc_Absyn_Struct_d;
_temp1970.f1= _temp1967; _temp1970;}); _temp1969;}), 0); _temp1968->tl= Cyc_Toc_result_decls;
_temp1968;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far ==  0){ Cyc_Toc_xtunion_fields_so_far=({ struct
Cyc_Core_Opt* _temp1974=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp1974->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp1974;});} if( xd->fields
==  0){ return;}{ struct _tuple0* _temp1975= xd->name; struct Cyc_List_List*
_temp1976=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp1976 !=  0; _temp1976=(( struct Cyc_List_List*) _check_null(
_temp1976))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1976))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp1977= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) +  4, 0); struct Cyc_Absyn_ArrayType_struct* _temp1978=({ struct
Cyc_Absyn_ArrayType_struct* _temp2028=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2028[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2029; _temp2029.tag= Cyc_Absyn_ArrayType;
_temp2029.f1=( void*) Cyc_Absyn_uchar_t; _temp2029.f2= Cyc_Toc_mt_tq; _temp2029.f3=(
struct Cyc_Absyn_Exp*) _temp1977; _temp2029;}); _temp2028;}); struct Cyc_Core_Opt*
_temp1979=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp1987; int _temp1988; _LL1981: if( _temp1979 ==  0){ goto _LL1982;} else{
goto _LL1983;} _LL1983: if( _temp1979 ==  0){ goto _LL1985;} else{ _temp1987=*
_temp1979; _LL1989: _temp1988=( int) _temp1987.v; if( _temp1988 ==  0){ goto
_LL1984;} else{ goto _LL1985;}} _LL1985: goto _LL1986; _LL1982: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp1995; _temp1995.tag= Cyc_Std_String_pa; _temp1995.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Std_Int_pa_struct _temp1994; _temp1994.tag= Cyc_Std_Int_pa;
_temp1994.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp1993;
_temp1993.tag= Cyc_Std_Int_pa; _temp1993.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp1992; _temp1992.tag= Cyc_Std_Int_pa; _temp1992.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp1991; _temp1991.tag=
Cyc_Std_Int_pa; _temp1991.f1=( int)(( unsigned int) 0);{ void* _temp1990[ 5u]={&
_temp1991,& _temp1992,& _temp1993,& _temp1994,& _temp1995}; Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u), _tag_arr( _temp1990,
sizeof( void*), 5u));}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl* _temp1996= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp1978, initopt);( void*)( _temp1996->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp1997=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1997->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1998=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1998[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1999; _temp1999.tag= Cyc_Absyn_Var_d; _temp1999.f1= _temp1996; _temp1999;});
_temp1998;}), 0); _temp1997->tl= Cyc_Toc_result_decls; _temp1997;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc != ( void*) Cyc_Absyn_Extern));
if( f->typs !=  0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2000= f->typs; for( 0; _temp2000 !=  0;( _temp2000=(( struct Cyc_List_List*)
_check_null( _temp2000))->tl, i ++)){ struct _tagged_arr* _temp2001=({ struct
_tagged_arr* _temp2005=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp2005[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp2007; _temp2007.tag= Cyc_Std_Int_pa; _temp2007.f1=( int)(( unsigned int) i);{
void* _temp2006[ 1u]={& _temp2007}; Cyc_Std_aprintf( _tag_arr("f%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2006, sizeof( void*), 1u));}}); _temp2005;});
struct Cyc_Absyn_Structfield* _temp2002=({ struct Cyc_Absyn_Structfield*
_temp2004=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2004->name= _temp2001; _temp2004->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2000))->hd)).f1; _temp2004->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2000))->hd)).f2);
_temp2004->width= 0; _temp2004->attributes= 0; _temp2004;}); fields=({ struct
Cyc_List_List* _temp2003=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2003->hd=( void*) _temp2002; _temp2003->tl= fields; _temp2003;});}} fields=({
struct Cyc_List_List* _temp2008=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2008->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2009=( struct Cyc_Absyn_Structfield*) _cycalloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2009->name= Cyc_Toc_tag_sp; _temp2009->tq= Cyc_Toc_mt_tq; _temp2009->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2009->width=
0; _temp2009->attributes= 0; _temp2009;}); _temp2008->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2008;});{ struct Cyc_Absyn_Structdecl*
_temp2010=({ struct Cyc_Absyn_Structdecl* _temp2014=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2014->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2014->name=({ struct Cyc_Core_Opt* _temp2016=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp2016->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2016;}); _temp2014->tvs= 0; _temp2014->fields=({ struct Cyc_Core_Opt*
_temp2015=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp2015->v=( void*) fields; _temp2015;}); _temp2014->attributes= 0; _temp2014;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2011=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2011->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2012=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2012[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2013; _temp2013.tag= Cyc_Absyn_Struct_d;
_temp2013.f1= _temp2010; _temp2013;}); _temp2012;}), 0); _temp2011->tl= Cyc_Toc_result_decls;
_temp2011;});}} goto _LL1980;}} _LL1984: if(( void*) f->sc != ( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2017= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _temp2027; _temp2027.tag= Cyc_Std_String_pa;
_temp2027.f1=( struct _tagged_arr)* fn;{ struct Cyc_Std_Int_pa_struct _temp2026;
_temp2026.tag= Cyc_Std_Int_pa; _temp2026.f1=( int)(( unsigned int) 0);{ struct
Cyc_Std_Int_pa_struct _temp2025; _temp2025.tag= Cyc_Std_Int_pa; _temp2025.f1=(
int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct _temp2024; _temp2024.tag=
Cyc_Std_Int_pa; _temp2024.f1=( int)(( unsigned int) 0);{ struct Cyc_Std_Int_pa_struct
_temp2023; _temp2023.tag= Cyc_Std_Int_pa; _temp2023.f1=( int)(( unsigned int) 0);{
void* _temp2022[ 5u]={& _temp2023,& _temp2024,& _temp2025,& _temp2026,&
_temp2027}; Cyc_Std_aprintf( _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u),
_tag_arr( _temp2022, sizeof( void*), 5u));}}}}}}), 0); struct Cyc_Absyn_Vardecl*
_temp2018= Cyc_Absyn_new_vardecl( f->name,( void*) _temp1978,( struct Cyc_Absyn_Exp*)
_temp2017);( void*)( _temp2018->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2019=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2019->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2020=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2020[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2021; _temp2021.tag= Cyc_Absyn_Var_d; _temp2021.f1= _temp2018; _temp2021;});
_temp2020;}), 0); _temp2019->tl= Cyc_Toc_result_decls; _temp2019;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL1980; _LL1986: goto
_LL1980; _LL1980:;}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2030= ed->name;( void*)( ed->sc=(
void*)(( void*) Cyc_Absyn_Public)); if( ed->fields !=  0){ struct Cyc_List_List*
_temp2031=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; _temp2031 !=  0; _temp2031=(( struct Cyc_List_List*) _check_null(
_temp2031))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2031))->hd)->tag !=  0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2031))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2031))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer !=  0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2032=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2038; struct Cyc_Absyn_Exp* _temp2040; struct Cyc_Absyn_Vardecl* _temp2042;
_LL2034: if(*(( int*) _temp2032) ==  Cyc_Absyn_Comprehension_e){ _LL2043:
_temp2042=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2032)->f1; goto
_LL2041; _LL2041: _temp2040=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2032)->f2; goto _LL2039; _LL2039: _temp2038=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2032)->f3; goto _LL2035;} else{ goto _LL2036;} _LL2036: goto _LL2037;
_LL2035: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2042, _temp2040, _temp2038, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2033; _LL2037: if(( void*) vd->sc == ( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2044= Cyc_Toc_copy_env( init_nv);
_temp2044->toplevel= 1; Cyc_Toc_exp_to_c( _temp2044, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2033; _LL2033:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2045= _new_region(); struct _RegionHandle* prgn=& _temp2045;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2048; struct Cyc_List_List*
_temp2050; struct Cyc_Toc_Env* _temp2052; struct _tuple11 _temp2046= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2053: _temp2052= _temp2046.f1; goto _LL2051; _LL2051: _temp2050=
_temp2046.f2; goto _LL2049; _LL2049: _temp2048= _temp2046.f3; goto _LL2047;
_LL2047: Cyc_Toc_stmt_to_c( _temp2052, s);{ struct Cyc_Absyn_Stmt* _temp2054=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2048, _temp2054, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2055= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2056= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2055, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2048,
Cyc_Absyn_seq_stmt( _temp2056, _temp2054, 0), 0), 0);} for( 0; _temp2050 !=  0;
_temp2050=(( struct Cyc_List_List*) _check_null( _temp2050))->tl){ struct
_tuple12 _temp2059; void* _temp2060; struct _tuple0* _temp2062; struct _tuple12*
_temp2057=( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2050))->hd;
_temp2059=* _temp2057; _LL2063: _temp2062= _temp2059.f1; goto _LL2061; _LL2061:
_temp2060= _temp2059.f2; goto _LL2058; _LL2058: s= Cyc_Absyn_declare_stmt(
_temp2062, _temp2060, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){({ void* _temp2064[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u), _tag_arr( _temp2064, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2065=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2091; struct Cyc_Absyn_Fndecl* _temp2093; struct Cyc_Absyn_Structdecl*
_temp2095; struct Cyc_Absyn_Uniondecl* _temp2097; struct Cyc_Absyn_Tuniondecl*
_temp2099; struct Cyc_Absyn_Enumdecl* _temp2101; struct Cyc_Absyn_Typedefdecl*
_temp2103; struct Cyc_List_List* _temp2105; struct Cyc_List_List* _temp2107;
struct Cyc_List_List* _temp2109; _LL2067: if(*(( int*) _temp2065) ==  Cyc_Absyn_Var_d){
_LL2092: _temp2091=(( struct Cyc_Absyn_Var_d_struct*) _temp2065)->f1; goto
_LL2068;} else{ goto _LL2069;} _LL2069: if(*(( int*) _temp2065) ==  Cyc_Absyn_Fn_d){
_LL2094: _temp2093=(( struct Cyc_Absyn_Fn_d_struct*) _temp2065)->f1; goto
_LL2070;} else{ goto _LL2071;} _LL2071: if(*(( int*) _temp2065) ==  Cyc_Absyn_Let_d){
goto _LL2072;} else{ goto _LL2073;} _LL2073: if(*(( int*) _temp2065) ==  Cyc_Absyn_Letv_d){
goto _LL2074;} else{ goto _LL2075;} _LL2075: if(*(( int*) _temp2065) ==  Cyc_Absyn_Struct_d){
_LL2096: _temp2095=(( struct Cyc_Absyn_Struct_d_struct*) _temp2065)->f1; goto
_LL2076;} else{ goto _LL2077;} _LL2077: if(*(( int*) _temp2065) ==  Cyc_Absyn_Union_d){
_LL2098: _temp2097=(( struct Cyc_Absyn_Union_d_struct*) _temp2065)->f1; goto
_LL2078;} else{ goto _LL2079;} _LL2079: if(*(( int*) _temp2065) ==  Cyc_Absyn_Tunion_d){
_LL2100: _temp2099=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2065)->f1; goto
_LL2080;} else{ goto _LL2081;} _LL2081: if(*(( int*) _temp2065) ==  Cyc_Absyn_Enum_d){
_LL2102: _temp2101=(( struct Cyc_Absyn_Enum_d_struct*) _temp2065)->f1; goto
_LL2082;} else{ goto _LL2083;} _LL2083: if(*(( int*) _temp2065) ==  Cyc_Absyn_Typedef_d){
_LL2104: _temp2103=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2065)->f1; goto
_LL2084;} else{ goto _LL2085;} _LL2085: if(*(( int*) _temp2065) ==  Cyc_Absyn_Namespace_d){
_LL2106: _temp2105=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2065)->f2; goto
_LL2086;} else{ goto _LL2087;} _LL2087: if(*(( int*) _temp2065) ==  Cyc_Absyn_Using_d){
_LL2108: _temp2107=(( struct Cyc_Absyn_Using_d_struct*) _temp2065)->f2; goto
_LL2088;} else{ goto _LL2089;} _LL2089: if(*(( int*) _temp2065) ==  Cyc_Absyn_ExternC_d){
_LL2110: _temp2109=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2065)->f1; goto
_LL2090;} else{ goto _LL2066;} _LL2068: { struct _tuple0* _temp2111= _temp2091->name;
if(( void*) _temp2091->sc == ( void*) Cyc_Absyn_ExternC){ _temp2111=({ struct
_tuple0* _temp2112=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0));
_temp2112->f1= Cyc_Absyn_rel_ns_null; _temp2112->f2=(* _temp2111).f2; _temp2112;});}
if( _temp2091->initializer !=  0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2091->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2091->name,
Cyc_Absyn_varb_exp( _temp2111,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2113=( struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2113[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2114; _temp2114.tag= Cyc_Absyn_Global_b;
_temp2114.f1= _temp2091; _temp2114;}); _temp2113;}), 0)); _temp2091->name=
_temp2111;( void*)( _temp2091->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2091->sc));(
void*)( _temp2091->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2091->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2115=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2115->hd=( void*) d; _temp2115->tl=
Cyc_Toc_result_decls; _temp2115;}); goto _LL2066;} _LL2070: { struct _tuple0*
_temp2116= _temp2093->name; if(( void*) _temp2093->sc == ( void*) Cyc_Absyn_ExternC){
_temp2116=({ struct _tuple0* _temp2117=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp2117->f1= Cyc_Absyn_rel_ns_null; _temp2117->f2=(*
_temp2116).f2; _temp2117;});} nv= Cyc_Toc_add_varmap( nv, _temp2093->name, Cyc_Absyn_var_exp(
_temp2116, 0)); _temp2093->name= _temp2116; Cyc_Toc_fndecl_to_c( nv, _temp2093);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2118=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2118->hd=( void*) d; _temp2118->tl=
Cyc_Toc_result_decls; _temp2118;}); goto _LL2066;} _LL2072: goto _LL2074;
_LL2074:({ void* _temp2119[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2119, sizeof( void*), 0u));}); goto _LL2066;
_LL2076: Cyc_Toc_structdecl_to_c( _temp2095); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2120=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2120->hd=( void*) d; _temp2120->tl= Cyc_Toc_result_decls; _temp2120;});
goto _LL2066; _LL2078: Cyc_Toc_uniondecl_to_c( _temp2097); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2121=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp2121->hd=( void*) d; _temp2121->tl= Cyc_Toc_result_decls;
_temp2121;}); goto _LL2066; _LL2080: if( _temp2099->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2099);} else{ Cyc_Toc_tuniondecl_to_c( _temp2099);} goto _LL2066; _LL2082:
Cyc_Toc_enumdecl_to_c( nv, _temp2101); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2122=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp2122->hd=( void*) d; _temp2122->tl= Cyc_Toc_result_decls; _temp2122;});
goto _LL2066; _LL2084: _temp2103->name= _temp2103->name; _temp2103->tvs= 0;(
void*)( _temp2103->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2103->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2123=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp2123->hd=( void*) d; _temp2123->tl=
Cyc_Toc_result_decls; _temp2123;}); goto _LL2066; _LL2086: _temp2107= _temp2105;
goto _LL2088; _LL2088: _temp2109= _temp2107; goto _LL2090; _LL2090: nv= Cyc_Toc_decls_to_c(
nv, _temp2109, top); goto _LL2066; _LL2066:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
