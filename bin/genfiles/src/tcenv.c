 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; }
; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
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
int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); struct Cyc_PP_Ppstate;
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
static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static const int
Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
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
Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod= 0; struct
Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{ int tag;
void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
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
19; static const int Cyc_Absyn_Printf= 20; static const int Cyc_Absyn_Fprintf=
21; static const int Cyc_Absyn_Xprintf= 22; static const int Cyc_Absyn_Scanf= 23;
static const int Cyc_Absyn_Fscanf= 24; static const int Cyc_Absyn_Sscanf= 25;
static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
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
struct _tagged_arr); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set;
extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar* t); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void*
e2); extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e);
extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*, struct
Cyc_List_List*); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k);
extern int* Cyc_Tcutil_new_tvar_id(); extern void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*);
unsigned char Cyc_Tcenv_Env_error[ 14u]="\000\000\000\000Env_error"; void* Cyc_Tcenv_env_err(
struct _tagged_arr msg){({ struct _tagged_arr _temp0= msg; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"Error: %.*s\n", _temp0.last_plus_one - _temp0.curr,
_temp0.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);(
int) _throw(( void*) Cyc_Tcenv_Env_error);} struct Cyc_Tcenv_Tenv; struct Cyc_Tcenv_Genv;
struct Cyc_Tcenv_CtrlEnv{ void* continue_stmt; void* break_stmt; struct _tuple3*
fallthru_clause; void* next_stmt; int try_depth; } ; struct Cyc_Tcenv_SharedFenv{
void* return_typ; struct Cyc_Dict_Dict* seen_labels; struct Cyc_SlowDict_Dict*
needed_labels; } ; struct Cyc_Tcenv_RgnPO{ struct Cyc_Absyn_Tvar* tvar; struct
Cyc_List_List* ancestors; } ; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv*
shared; struct Cyc_List_List* type_vars; struct Cyc_List_List* region_order;
struct Cyc_Dict_Dict* locals; struct Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv*
ctrl_env; void* capability; void* curr_rgn; } ; struct _tuple4{ void* f1; int f2;
} ; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({
struct Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp1;});{ struct Cyc_Tcenv_Genv* _temp2= Cyc_Tcenv_empty_genv(); goto _LL3;
_LL3: { struct Cyc_List_List* _temp4= 0; goto _LL5; _LL5: _temp2->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( _temp2->tuniondecls,(*
Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_tud); _temp2->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple4* data)) Cyc_Dict_insert)(
_temp2->ordinaries,(* Cyc_Absyn_null_pointer_exn_name).f2,({ struct _tuple4*
_temp6=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp6->f1=( void*)({
struct Cyc_Tcenv_TunionRes_struct* _temp7=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp7[ 0]=({ struct Cyc_Tcenv_TunionRes_struct
_temp8; _temp8.tag= Cyc_Tcenv_TunionRes; _temp8.f1= Cyc_Absyn_exn_tud; _temp8.f2=
Cyc_Absyn_null_pointer_exn_tuf; _temp8;}); _temp7;}); _temp6->f2= 1; _temp6;}));
_temp2->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct _tuple4* data)) Cyc_Dict_insert)( _temp2->ordinaries,(*
Cyc_Absyn_match_exn_name).f2,({ struct _tuple4* _temp9=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp9->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct*
_temp10=( struct Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp10[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp11; _temp11.tag= Cyc_Tcenv_TunionRes;
_temp11.f1= Cyc_Absyn_exn_tud; _temp11.f2= Cyc_Absyn_match_exn_tuf; _temp11;});
_temp10;}); _temp9->f2= 1; _temp9;}));{ struct Cyc_Dict_Dict* _temp12=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct
Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp),
_temp4, _temp2); goto _LL13; _LL13: return({ struct Cyc_Tcenv_Tenv* _temp14=(
struct Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp14->ns=
_temp4; _temp14->ae= _temp12; _temp14->le= 0; _temp14;});}}}} static struct Cyc_Tcenv_Genv*
Cyc_Tcenv_lookup_namespace( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr* n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
Cyc_Tcenv_resolve_namespace( te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns == 0){ return(( struct Cyc_List_List*(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv::outer_namespace",
sizeof( unsigned char), 23u));} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( ns)))->tl);} static void Cyc_Tcenv_check_repeat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v, struct Cyc_List_List*
nss){ for( 0; nss != 0; nss=(( struct Cyc_List_List*) _check_null( nss))->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( nss))->hd); struct _handler_cons _temp15;
_push_handler(& _temp15);{ int _temp17= 0; if( setjmp( _temp15.handler)){
_temp17= 1;} if( ! _temp17){ lookup( ge2, v); Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp18=* v; xprintf("%.*s is ambiguous",
_temp18.last_plus_one - _temp18.curr, _temp18.curr);}));; _pop_handler();} else{
void* _temp16=( void*) _exn_thrown; void* _temp20= _temp16; _LL22: if( _temp20
== Cyc_Dict_Absent){ goto _LL23;} else{ goto _LL24;} _LL24: goto _LL25; _LL23:
goto _LL21; _LL25:( void) _throw( _temp20); _LL21:;}}} return;} static void* Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v){ struct Cyc_List_List*
cns= te->ns; void* result; while( 1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
cns);{ struct _handler_cons _temp26; _push_handler(& _temp26);{ int _temp28= 0;
if( setjmp( _temp26.handler)){ _temp28= 1;} if( ! _temp28){ result= lookup( ge,
v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v,
struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)( te, loc, lookup, v, ge->availables);{
void* _temp29= result; _npop_handler( 0u); return _temp29;}; _pop_handler();}
else{ void* _temp27=( void*) _exn_thrown; void* _temp31= _temp27; _LL33: if(
_temp31 == Cyc_Dict_Absent){ goto _LL34;} else{ goto _LL35;} _LL35: goto _LL36;
_LL34: goto _LL32; _LL36:( void) _throw( _temp31); _LL32:;}}}{ struct Cyc_List_List*
_temp37= ge->availables; goto _LL38; _LL38: for( 0; _temp37 != 0; _temp37=((
struct Cyc_List_List*) _check_null( _temp37))->tl){ struct Cyc_Tcenv_Genv* ge2=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( _temp37))->hd); struct _handler_cons _temp39; _push_handler(&
_temp39);{ int _temp41= 0; if( setjmp( _temp39.handler)){ _temp41= 1;} if( !
_temp41){ result= lookup( ge2, v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)(
te, loc, lookup, v,(( struct Cyc_List_List*) _check_null( _temp37))->tl);{ void*
_temp42= result; _npop_handler( 0u); return _temp42;}; _pop_handler();} else{
void* _temp40=( void*) _exn_thrown; void* _temp44= _temp40; _LL46: if( _temp44
== Cyc_Dict_Absent){ goto _LL47;} else{ goto _LL48;} _LL48: goto _LL49; _LL47:
goto _LL45; _LL49:( void) _throw( _temp44); _LL45:;}}}} if( cns == 0){( int)
_throw(( void*) Cyc_Dict_Absent);} cns= Cyc_Tcenv_outer_namespace( cns);}}
static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv* ge,
struct _tagged_arr* v){ struct _tuple4* ans=(( struct _tuple4*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v);(* ans).f2= 1;
return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp52;
struct _tagged_arr* _temp53; void* _temp55; struct _tuple0* _temp50= q; _temp52=*
_temp50; _LL56: _temp55= _temp52.f1; goto _LL54; _LL54: _temp53= _temp52.f2;
goto _LL51; _LL51: { void* _temp57= _temp55; struct Cyc_List_List* _temp67;
struct Cyc_List_List* _temp69; struct Cyc_List_List _temp71; struct Cyc_List_List*
_temp72; struct _tagged_arr* _temp74; struct Cyc_List_List* _temp76; _LL59: if(
_temp57 ==( void*) Cyc_Absyn_Loc_n){ goto _LL60;} else{ goto _LL61;} _LL61: if((
unsigned int) _temp57 > 1u?*(( int*) _temp57) == Cyc_Absyn_Rel_n: 0){ _LL68:
_temp67=(( struct Cyc_Absyn_Rel_n_struct*) _temp57)->f1; if( _temp67 == 0){ goto
_LL62;} else{ goto _LL63;}} else{ goto _LL63;} _LL63: if(( unsigned int) _temp57
> 1u?*(( int*) _temp57) == Cyc_Absyn_Rel_n: 0){ _LL70: _temp69=(( struct Cyc_Absyn_Rel_n_struct*)
_temp57)->f1; if( _temp69 == 0){ goto _LL65;} else{ _temp71=* _temp69; _LL75:
_temp74=( struct _tagged_arr*) _temp71.hd; goto _LL73; _LL73: _temp72= _temp71.tl;
goto _LL64;}} else{ goto _LL65;} _LL65: if(( unsigned int) _temp57 > 1u?*(( int*)
_temp57) == Cyc_Absyn_Abs_n: 0){ _LL77: _temp76=(( struct Cyc_Absyn_Abs_n_struct*)
_temp57)->f1; goto _LL66;} else{ goto _LL58;} _LL60: goto _LL62; _LL62: return((
void*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_ordinary_global_f, _temp53);
_LL64: { struct Cyc_Tcenv_Genv* _temp78= Cyc_Tcenv_lookup_namespace( te, loc,
_temp74, _temp72); goto _LL79; _LL79: return Cyc_Tcenv_lookup_ordinary_global_f(
_temp78, _temp53);} _LL66: return Cyc_Tcenv_lookup_ordinary_global_f((( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, _temp76), _temp53); _LL58:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ struct Cyc_List_List* _temp80= te->ns; goto _LL81;
_LL81: { struct Cyc_List_List* _temp82= 0; goto _LL83; _LL83: while( 1) { struct
Cyc_Tcenv_Genv* _temp84=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp80); goto _LL85;
_LL85:{ struct Cyc_List_List* _temp86= _temp84->availables; goto _LL87; _LL87:
for( 0; _temp86 != 0; _temp86=(( struct Cyc_List_List*) _check_null( _temp86))->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( _temp86))->hd); if((( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( ge2->namespaces, n)){ _temp82=({
struct Cyc_List_List* _temp88=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp88->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp80,({ struct Cyc_List_List*
_temp89=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp89->hd=( void*) n; _temp89->tl= ns; _temp89;})); _temp88->tl= _temp82;
_temp88;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt))
Cyc_Set_member)( _temp84->namespaces, n)){ _temp82=({ struct Cyc_List_List*
_temp90=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp90->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp80,({ struct Cyc_List_List* _temp91=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp91->hd=(
void*) n; _temp91->tl= ns; _temp91;})); _temp90->tl= _temp82; _temp90;});} if(
_temp82 != 0){ if((( struct Cyc_List_List*) _check_null( _temp82))->tl != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp92=* n;
xprintf("%.*s is ambiguous", _temp92.last_plus_one - _temp92.curr, _temp92.curr);}));}
return( struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp82))->hd;}
if( _temp80 == 0){( int) _throw(( void*) Cyc_Dict_Absent);} _temp80= Cyc_Tcenv_outer_namespace(
_temp80);}}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp95;
struct _tagged_arr* _temp96; void* _temp98; struct _tuple0* _temp93= q; _temp95=*
_temp93; _LL99: _temp98= _temp95.f1; goto _LL97; _LL97: _temp96= _temp95.f2;
goto _LL94; _LL94: { void* _temp100= _temp98; struct Cyc_List_List* _temp110;
struct Cyc_List_List* _temp112; struct Cyc_List_List* _temp114; struct Cyc_List_List
_temp116; struct Cyc_List_List* _temp117; struct _tagged_arr* _temp119; _LL102:
if( _temp100 ==( void*) Cyc_Absyn_Loc_n){ goto _LL103;} else{ goto _LL104;}
_LL104: if(( unsigned int) _temp100 > 1u?*(( int*) _temp100) == Cyc_Absyn_Rel_n:
0){ _LL111: _temp110=(( struct Cyc_Absyn_Rel_n_struct*) _temp100)->f1; if(
_temp110 == 0){ goto _LL105;} else{ goto _LL106;}} else{ goto _LL106;} _LL106:
if(( unsigned int) _temp100 > 1u?*(( int*) _temp100) == Cyc_Absyn_Abs_n: 0){
_LL113: _temp112=(( struct Cyc_Absyn_Abs_n_struct*) _temp100)->f1; goto _LL107;}
else{ goto _LL108;} _LL108: if(( unsigned int) _temp100 > 1u?*(( int*) _temp100)
== Cyc_Absyn_Rel_n: 0){ _LL115: _temp114=(( struct Cyc_Absyn_Rel_n_struct*)
_temp100)->f1; if( _temp114 == 0){ goto _LL101;} else{ _temp116=* _temp114;
_LL120: _temp119=( struct _tagged_arr*) _temp116.hd; goto _LL118; _LL118:
_temp117= _temp116.tl; goto _LL109;}} else{ goto _LL101;} _LL103: goto _LL105;
_LL105: return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp96); _LL107: { struct Cyc_Tcenv_Genv*
_temp121=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp112); goto _LL122; _LL122: return(( struct
Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_lookup)( _temp121->structdecls, _temp96);} _LL109: { struct Cyc_Tcenv_Genv*
_temp123= Cyc_Tcenv_lookup_namespace( te, loc, _temp119, _temp117); goto _LL124;
_LL124: return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp123->structdecls, _temp96);}
_LL101:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp127; struct _tagged_arr* _temp128; void* _temp130; struct _tuple0* _temp125=
q; _temp127=* _temp125; _LL131: _temp130= _temp127.f1; goto _LL129; _LL129:
_temp128= _temp127.f2; goto _LL126; _LL126: { void* _temp132= _temp130; struct
Cyc_List_List* _temp142; struct Cyc_List_List* _temp144; struct Cyc_List_List*
_temp146; struct Cyc_List_List _temp148; struct Cyc_List_List* _temp149; struct
_tagged_arr* _temp151; _LL134: if( _temp132 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL135;} else{ goto _LL136;} _LL136: if(( unsigned int) _temp132 > 1u?*(( int*)
_temp132) == Cyc_Absyn_Rel_n: 0){ _LL143: _temp142=(( struct Cyc_Absyn_Rel_n_struct*)
_temp132)->f1; if( _temp142 == 0){ goto _LL137;} else{ goto _LL138;}} else{ goto
_LL138;} _LL138: if(( unsigned int) _temp132 > 1u?*(( int*) _temp132) == Cyc_Absyn_Abs_n:
0){ _LL145: _temp144=(( struct Cyc_Absyn_Abs_n_struct*) _temp132)->f1; goto
_LL139;} else{ goto _LL140;} _LL140: if(( unsigned int) _temp132 > 1u?*(( int*)
_temp132) == Cyc_Absyn_Rel_n: 0){ _LL147: _temp146=(( struct Cyc_Absyn_Rel_n_struct*)
_temp132)->f1; if( _temp146 == 0){ goto _LL133;} else{ _temp148=* _temp146;
_LL152: _temp151=( struct _tagged_arr*) _temp148.hd; goto _LL150; _LL150:
_temp149= _temp148.tl; goto _LL141;}} else{ goto _LL133;} _LL135: goto _LL137;
_LL137: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(* lookup)( struct
Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp128); _LL139: { struct Cyc_Tcenv_Genv*
_temp153=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp144); goto _LL154; _LL154: return(( struct
Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp153->uniondecls, _temp128);} _LL141: { struct Cyc_Tcenv_Genv* _temp155= Cyc_Tcenv_lookup_namespace(
te, loc, _temp151, _temp149); goto _LL156; _LL156: return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp155->uniondecls,
_temp128);} _LL133:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp159; struct _tagged_arr* _temp160; void* _temp162; struct _tuple0* _temp157=
q; _temp159=* _temp157; _LL163: _temp162= _temp159.f1; goto _LL161; _LL161:
_temp160= _temp159.f2; goto _LL158; _LL158: { void* _temp164= _temp162; struct
Cyc_List_List* _temp174; struct Cyc_List_List* _temp176; struct Cyc_List_List
_temp178; struct Cyc_List_List* _temp179; struct _tagged_arr* _temp181; struct
Cyc_List_List* _temp183; _LL166: if( _temp164 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL167;} else{ goto _LL168;} _LL168: if(( unsigned int) _temp164 > 1u?*(( int*)
_temp164) == Cyc_Absyn_Rel_n: 0){ _LL175: _temp174=(( struct Cyc_Absyn_Rel_n_struct*)
_temp164)->f1; if( _temp174 == 0){ goto _LL169;} else{ goto _LL170;}} else{ goto
_LL170;} _LL170: if(( unsigned int) _temp164 > 1u?*(( int*) _temp164) == Cyc_Absyn_Rel_n:
0){ _LL177: _temp176=(( struct Cyc_Absyn_Rel_n_struct*) _temp164)->f1; if(
_temp176 == 0){ goto _LL172;} else{ _temp178=* _temp176; _LL182: _temp181=(
struct _tagged_arr*) _temp178.hd; goto _LL180; _LL180: _temp179= _temp178.tl;
goto _LL171;}} else{ goto _LL172;} _LL172: if(( unsigned int) _temp164 > 1u?*((
int*) _temp164) == Cyc_Absyn_Abs_n: 0){ _LL184: _temp183=(( struct Cyc_Absyn_Abs_n_struct*)
_temp164)->f1; goto _LL173;} else{ goto _LL165;} _LL167: goto _LL169; _LL169:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_tuniondecl_f,
_temp160); _LL171: { struct Cyc_Tcenv_Genv* _temp185= Cyc_Tcenv_lookup_namespace(
te, loc, _temp181, _temp179); goto _LL186; _LL186: return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp185->tuniondecls,
_temp160);} _LL173: { struct Cyc_Tcenv_Genv* _temp187=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp183); goto _LL188; _LL188: return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp187->tuniondecls,
_temp160);} _LL165:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp191; struct _tagged_arr* _temp192; void* _temp194; struct _tuple0* _temp189=
q; _temp191=* _temp189; _LL195: _temp194= _temp191.f1; goto _LL193; _LL193:
_temp192= _temp191.f2; goto _LL190; _LL190: { void* _temp196= _temp194; struct
Cyc_List_List* _temp206; struct Cyc_List_List* _temp208; struct Cyc_List_List
_temp210; struct Cyc_List_List* _temp211; struct _tagged_arr* _temp213; struct
Cyc_List_List* _temp215; _LL198: if(( unsigned int) _temp196 > 1u?*(( int*)
_temp196) == Cyc_Absyn_Rel_n: 0){ _LL207: _temp206=(( struct Cyc_Absyn_Rel_n_struct*)
_temp196)->f1; if( _temp206 == 0){ goto _LL199;} else{ goto _LL200;}} else{ goto
_LL200;} _LL200: if( _temp196 ==( void*) Cyc_Absyn_Loc_n){ goto _LL201;} else{
goto _LL202;} _LL202: if(( unsigned int) _temp196 > 1u?*(( int*) _temp196) ==
Cyc_Absyn_Rel_n: 0){ _LL209: _temp208=(( struct Cyc_Absyn_Rel_n_struct*)
_temp196)->f1; if( _temp208 == 0){ goto _LL204;} else{ _temp210=* _temp208;
_LL214: _temp213=( struct _tagged_arr*) _temp210.hd; goto _LL212; _LL212:
_temp211= _temp210.tl; goto _LL203;}} else{ goto _LL204;} _LL204: if((
unsigned int) _temp196 > 1u?*(( int*) _temp196) == Cyc_Absyn_Abs_n: 0){ _LL216:
_temp215=(( struct Cyc_Absyn_Abs_n_struct*) _temp196)->f1; goto _LL205;} else{
goto _LL197;} _LL199: { struct _handler_cons _temp217; _push_handler(& _temp217);{
int _temp219= 0; if( setjmp( _temp217.handler)){ _temp219= 1;} if( ! _temp219){{
struct Cyc_Core_Opt* _temp221=({ struct Cyc_Core_Opt* _temp220=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp220->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp192);
_temp220;}); _npop_handler( 0u); return _temp221;}; _pop_handler();} else{ void*
_temp218=( void*) _exn_thrown; void* _temp223= _temp218; _LL225: if( _temp223 ==
Cyc_Dict_Absent){ goto _LL226;} else{ goto _LL227;} _LL227: goto _LL228; _LL226:
return 0; _LL228:( void) _throw( _temp223); _LL224:;}}} _LL201: Cyc_Tcutil_terr(
loc, _tag_arr("lookup_xtuniondecl: impossible", sizeof( unsigned char), 31u));
return 0; _LL203: { struct Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp229;
_push_handler(& _temp229);{ int _temp231= 0; if( setjmp( _temp229.handler)){
_temp231= 1;} if( ! _temp231){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp213,
_temp211);; _pop_handler();} else{ void* _temp230=( void*) _exn_thrown; void*
_temp233= _temp230; _LL235: if( _temp233 == Cyc_Dict_Absent){ goto _LL236;}
else{ goto _LL237;} _LL237: goto _LL238; _LL236: Cyc_Tcutil_terr( loc, _tag_arr("bad qualified name for xtunion",
sizeof( unsigned char), 31u));( int) _throw(( void*) Cyc_Dict_Absent); _LL238:(
void) _throw( _temp233); _LL234:;}}} return({ struct Cyc_Core_Opt* _temp239=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp239->v=(
void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp192); _temp239;});}
_LL205: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp215); return({
struct Cyc_Core_Opt* _temp240=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp240->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp192); _temp240;});} _LL197:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp243; struct _tagged_arr* _temp244; void* _temp246; struct _tuple0* _temp241=
q; _temp243=* _temp241; _LL247: _temp246= _temp243.f1; goto _LL245; _LL245:
_temp244= _temp243.f2; goto _LL242; _LL242: { void* _temp248= _temp246; struct
Cyc_List_List* _temp258; struct Cyc_List_List* _temp260; struct Cyc_List_List
_temp262; struct Cyc_List_List* _temp263; struct _tagged_arr* _temp265; struct
Cyc_List_List* _temp267; _LL250: if( _temp248 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL251;} else{ goto _LL252;} _LL252: if(( unsigned int) _temp248 > 1u?*(( int*)
_temp248) == Cyc_Absyn_Rel_n: 0){ _LL259: _temp258=(( struct Cyc_Absyn_Rel_n_struct*)
_temp248)->f1; if( _temp258 == 0){ goto _LL253;} else{ goto _LL254;}} else{ goto
_LL254;} _LL254: if(( unsigned int) _temp248 > 1u?*(( int*) _temp248) == Cyc_Absyn_Rel_n:
0){ _LL261: _temp260=(( struct Cyc_Absyn_Rel_n_struct*) _temp248)->f1; if(
_temp260 == 0){ goto _LL256;} else{ _temp262=* _temp260; _LL266: _temp265=(
struct _tagged_arr*) _temp262.hd; goto _LL264; _LL264: _temp263= _temp262.tl;
goto _LL255;}} else{ goto _LL256;} _LL256: if(( unsigned int) _temp248 > 1u?*((
int*) _temp248) == Cyc_Absyn_Abs_n: 0){ _LL268: _temp267=(( struct Cyc_Absyn_Abs_n_struct*)
_temp248)->f1; goto _LL257;} else{ goto _LL249;} _LL251: goto _LL253; _LL253:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_enumdecl_f,
_temp244); _LL255: { struct Cyc_Tcenv_Genv* _temp269= Cyc_Tcenv_lookup_namespace(
te, loc, _temp265, _temp263); goto _LL270; _LL270: return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp269->enumdecls,
_temp244);} _LL257: { struct Cyc_Tcenv_Genv* _temp271=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp267); goto _LL272; _LL272: return(( struct Cyc_Absyn_Enumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp271->enumdecls,
_temp244);} _LL249:;}} static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->typedefs,
v);} struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp275; struct _tagged_arr* _temp276; void* _temp278; struct _tuple0* _temp273=
q; _temp275=* _temp273; _LL279: _temp278= _temp275.f1; goto _LL277; _LL277:
_temp276= _temp275.f2; goto _LL274; _LL274: { void* _temp280= _temp278; struct
Cyc_List_List* _temp290; struct Cyc_List_List* _temp292; struct Cyc_List_List
_temp294; struct Cyc_List_List* _temp295; struct _tagged_arr* _temp297; struct
Cyc_List_List* _temp299; _LL282: if( _temp280 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL283;} else{ goto _LL284;} _LL284: if(( unsigned int) _temp280 > 1u?*(( int*)
_temp280) == Cyc_Absyn_Rel_n: 0){ _LL291: _temp290=(( struct Cyc_Absyn_Rel_n_struct*)
_temp280)->f1; if( _temp290 == 0){ goto _LL285;} else{ goto _LL286;}} else{ goto
_LL286;} _LL286: if(( unsigned int) _temp280 > 1u?*(( int*) _temp280) == Cyc_Absyn_Rel_n:
0){ _LL293: _temp292=(( struct Cyc_Absyn_Rel_n_struct*) _temp280)->f1; if(
_temp292 == 0){ goto _LL288;} else{ _temp294=* _temp292; _LL298: _temp297=(
struct _tagged_arr*) _temp294.hd; goto _LL296; _LL296: _temp295= _temp294.tl;
goto _LL287;}} else{ goto _LL288;} _LL288: if(( unsigned int) _temp280 > 1u?*((
int*) _temp280) == Cyc_Absyn_Abs_n: 0){ _LL300: _temp299=(( struct Cyc_Absyn_Abs_n_struct*)
_temp280)->f1; goto _LL289;} else{ goto _LL281;} _LL283: goto _LL285; _LL285:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_typedefdecl_f,
_temp276); _LL287: { struct Cyc_Tcenv_Genv* _temp301= Cyc_Tcenv_lookup_namespace(
te, loc, _temp297, _temp295); goto _LL302; _LL302: return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp301->typedefs,
_temp276);} _LL289: { struct Cyc_Tcenv_Genv* _temp303=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp299); goto _LL304; _LL304: return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp303->typedefs,
_temp276);} _LL281:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({
struct Cyc_Tcenv_Genv* _temp305=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Genv)); _temp305->namespaces=( struct Cyc_Set_Set*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Tcutil_empty_var_set))->v; _temp305->structdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp305->uniondecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp305->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp305->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp305->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp305->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp305->availables= 0; _temp305;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp306= f; void* _temp314; void* _temp316; void* _temp318;
void* _temp320; void* _temp322; _LL308: if(*(( int*) _temp306) == Cyc_Tcenv_Outermost){
_LL315: _temp314=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp306)->f1;
goto _LL309;} else{ goto _LL310;} _LL310: if(*(( int*) _temp306) == Cyc_Tcenv_Frame){
_LL319: _temp318=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp306)->f1; goto
_LL317; _LL317: _temp316=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp306)->f2;
goto _LL311;} else{ goto _LL312;} _LL312: if(*(( int*) _temp306) == Cyc_Tcenv_Hidden){
_LL323: _temp322=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp306)->f1; goto
_LL321; _LL321: _temp320=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp306)->f2;
goto _LL313;} else{ goto _LL307;} _LL309: return _temp314; _LL311: return
_temp318; _LL313: f= _temp320; goto _LL307; _LL307:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp324= f; void* _temp332; void* _temp334; void*
_temp336; void* _temp338; void* _temp340; _LL326: if(*(( int*) _temp324) == Cyc_Tcenv_Outermost){
_LL333: _temp332=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp324)->f1;
goto _LL327;} else{ goto _LL328;} _LL328: if(*(( int*) _temp324) == Cyc_Tcenv_Frame){
_LL337: _temp336=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp324)->f1; goto
_LL335; _LL335: _temp334=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp324)->f2;
goto _LL329;} else{ goto _LL330;} _LL330: if(*(( int*) _temp324) == Cyc_Tcenv_Hidden){
_LL341: _temp340=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp324)->f1; goto
_LL339; _LL339: _temp338=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp324)->f2;
goto _LL331;} else{ goto _LL325;} _LL327: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp342=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp342[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp343; _temp343.tag= Cyc_Tcenv_Outermost;
_temp343.f1=( void*) x; _temp343;}); _temp342;}); _LL329: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp344=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp344[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp345; _temp345.tag= Cyc_Tcenv_Frame; _temp345.f1=( void*) x; _temp345.f2=(
void*) _temp334; _temp345;}); _temp344;}); _LL331: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp346=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp346[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp347; _temp347.tag= Cyc_Tcenv_Hidden;
_temp347.f1=( void*) _temp340; _temp347.f2=( void*)(( void*(*)( void* f, void* x))
Cyc_Tcenv_put_visible)( _temp338, x); _temp347;}); _temp346;}); _LL325:;} void*
Cyc_Tcenv_apply_to_visible( void*(* g)( void*), void* f){ return(( void*(*)(
void* f, void* x)) Cyc_Tcenv_put_visible)( f, g((( void*(*)( void* f)) Cyc_Tcenv_get_visible)(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp348=
f; void* _temp356; void* _temp358; void* _temp360; void* _temp362; void*
_temp364; _LL350: if(*(( int*) _temp348) == Cyc_Tcenv_Outermost){ _LL357:
_temp356=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp348)->f1; goto
_LL351;} else{ goto _LL352;} _LL352: if(*(( int*) _temp348) == Cyc_Tcenv_Frame){
_LL361: _temp360=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp348)->f1; goto
_LL359; _LL359: _temp358=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp348)->f2;
goto _LL353;} else{ goto _LL354;} _LL354: if(*(( int*) _temp348) == Cyc_Tcenv_Hidden){
_LL365: _temp364=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp348)->f1; goto
_LL363; _LL363: _temp362=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp348)->f2;
goto _LL355;} else{ goto _LL349;} _LL351: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp366=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp366[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp367; _temp367.tag= Cyc_Tcenv_Outermost;
_temp367.f1=( void*) g( _temp356); _temp367;}); _temp366;}); _LL353: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp368=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp368[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp369; _temp369.tag= Cyc_Tcenv_Frame; _temp369.f1=( void*) g( _temp360);
_temp369.f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp358); _temp369;}); _temp368;}); _LL355: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp370=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp370[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp371; _temp371.tag= Cyc_Tcenv_Hidden;
_temp371.f1=( void*) g( _temp364); _temp371.f2=( void*)(( void*(*)( void*(* g)(
void*), void* f)) Cyc_Tcenv_map_frames)( g, _temp362); _temp371;}); _temp370;});
_LL349:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( int) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( int) _throw((
void*) Cyc_Tcenv_Env_error);} return({ struct Cyc_Tcenv_Tenv* _temp372=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp372->ns= te->ns;
_temp372->ae= te->ae; _temp372->le=({ struct Cyc_Core_Opt* _temp373=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp373->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v, fe); _temp373;}); _temp372;});} static int Cyc_Tcenv_region_ancestor(
struct Cyc_List_List* ancestors, struct Cyc_Absyn_Tvar* r){{ struct Cyc_List_List*
a= ancestors; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
if( Cyc_Absyn_tvar_cmp((( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*)
_check_null( a))->hd)->tvar, r) == 0){ return 1;}}}{ struct Cyc_List_List* a=
ancestors; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
if( Cyc_Tcenv_region_ancestor((( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*)
_check_null( a))->hd)->ancestors, r)){ return 1;}}} return 0;} static struct Cyc_Tcenv_RgnPO*
Cyc_Tcenv_find_region_in_po( struct Cyc_List_List* po, struct Cyc_Absyn_Tvar* r){
for( 0; po != 0; po=(( struct Cyc_List_List*) _check_null( po))->tl){ if( Cyc_Absyn_tvar_cmp(((
struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null( po))->hd)->tvar,
r) == 0){ return( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null(
po))->hd;}} return(( struct Cyc_Tcenv_RgnPO*(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)((
struct _tagged_arr)({ struct _tagged_arr _temp376= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Absyn_VarType_struct* _temp374=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp374[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp375; _temp375.tag= Cyc_Absyn_VarType; _temp375.f1= r; _temp375;}); _temp374;}));
xprintf("can't find region variable %.*s in po", _temp376.last_plus_one -
_temp376.curr, _temp376.curr);}));} static int Cyc_Tcenv_check_region_outlives(
struct Cyc_List_List* po, struct Cyc_Absyn_Tvar* r1, struct Cyc_Absyn_Tvar* r2){
struct Cyc_List_List* r2_ancestors; while( po != 0? Cyc_Absyn_tvar_cmp((( struct
Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null( po))->hd)->tvar, r2) !=
0: 0) { po=(( struct Cyc_List_List*) _check_null( po))->tl;} if( po == 0){
return 0;} return Cyc_Tcenv_region_ancestor((( struct Cyc_Tcenv_RgnPO*)(( struct
Cyc_List_List*) _check_null( po))->hd)->ancestors, r1);} struct _tuple5{ void*
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tcenv_initial_region_po(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_List_List*
po){ struct Cyc_List_List* result= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; if( Cyc_Tcutil_tvar_kind( tv) ==(
void*) Cyc_Absyn_RgnKind){ result=({ struct Cyc_List_List* _temp377=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp377->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp378=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp378->tvar= tv; _temp378->ancestors= 0; _temp378;}); _temp377->tl= result;
_temp377;});}} for( 0; po != 0; po=(( struct Cyc_List_List*) _check_null( po))->tl){
struct _tuple5 _temp379=*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( po))->hd); void* _temp387; void* _temp389; void* _temp391; struct
Cyc_Absyn_Tvar* _temp393; void* _temp395; struct Cyc_Absyn_Tvar* _temp397; void*
_temp399; void* _temp401; _LL381: _LL390: _temp389= _temp379.f1; if( _temp389 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL388;} else{ goto _LL383;} _LL388: _temp387=
_temp379.f2; goto _LL382; _LL383: _LL396: _temp395= _temp379.f1; if((
unsigned int) _temp395 > 4u?*(( int*) _temp395) == Cyc_Absyn_VarType: 0){ _LL398:
_temp397=(( struct Cyc_Absyn_VarType_struct*) _temp395)->f1; goto _LL392;} else{
goto _LL385;} _LL392: _temp391= _temp379.f2; if(( unsigned int) _temp391 > 4u?*((
int*) _temp391) == Cyc_Absyn_VarType: 0){ _LL394: _temp393=(( struct Cyc_Absyn_VarType_struct*)
_temp391)->f1; goto _LL384;} else{ goto _LL385;} _LL385: _LL402: _temp401=
_temp379.f1; goto _LL400; _LL400: _temp399= _temp379.f2; goto _LL386; _LL382:
goto _LL380; _LL384: if( Cyc_Absyn_tvar_cmp( _temp397, _temp393) == 0? 1: Cyc_Tcenv_check_region_outlives(
result, _temp393, _temp397)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp407= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp403=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp403[ 0]=({ struct Cyc_Absyn_VarType_struct _temp404; _temp404.tag= Cyc_Absyn_VarType;
_temp404.f1= _temp397; _temp404;}); _temp403;})); struct _tagged_arr _temp408=
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct* _temp405=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp405[ 0]=({ struct Cyc_Absyn_VarType_struct _temp406; _temp406.tag= Cyc_Absyn_VarType;
_temp406.f1= _temp393; _temp406;}); _temp405;})); xprintf("bad region lifetime assertion (circularity):%.*s < %.*s",
_temp407.last_plus_one - _temp407.curr, _temp407.curr, _temp408.last_plus_one -
_temp408.curr, _temp408.curr);}));} else{ struct Cyc_Tcenv_RgnPO* _temp409= Cyc_Tcenv_find_region_in_po(
result, _temp397); goto _LL410; _LL410: { struct Cyc_Tcenv_RgnPO* _temp411= Cyc_Tcenv_find_region_in_po(
result, _temp393); goto _LL412; _LL412: _temp411->ancestors=({ struct Cyc_List_List*
_temp413=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp413->hd=( void*) _temp409; _temp413->tl= _temp411->ancestors; _temp413;});}}
goto _LL380; _LL386: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp414= Cyc_Absynpp_typ2string( _temp401); struct _tagged_arr
_temp415= Cyc_Absynpp_typ2string( _temp399); xprintf("bad region lifetime assertion:: %.*s < %.*s",
_temp414.last_plus_one - _temp414.curr, _temp414.curr, _temp415.last_plus_one -
_temp415.curr, _temp415.curr);})); goto _LL380; _LL380:;} return result;} static
struct Cyc_List_List* Cyc_Tcenv_add_region_po( struct Cyc_List_List* po, struct
Cyc_Absyn_Tvar* tv){ return({ struct Cyc_List_List* _temp416=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp416->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp417=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp417->tvar= tv; _temp417->ancestors= po; _temp417;}); _temp416->tl= po;
_temp416;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl( struct
Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp418=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp418->shared= f->shared;
_temp418->type_vars= f->type_vars; _temp418->region_order= f->region_order;
_temp418->locals= f->locals; _temp418->encloser= f->encloser; _temp418->ctrl_env=
f->ctrl_env; _temp418->capability=( void*)(( void*) f->capability); _temp418->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp418;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl(
struct Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({
struct Cyc_Tcenv_Fenv* _temp419=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Fenv)); _temp419->shared= f->shared; _temp419->type_vars= f->type_vars;
_temp419->region_order= f->region_order; _temp419->locals= f->locals; _temp419->encloser=
f->encloser; _temp419->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp420=( struct
Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp420->continue_stmt=(
void*)(( void*) c->continue_stmt); _temp420->break_stmt=( void*)(( void*) c->break_stmt);
_temp420->fallthru_clause= c->fallthru_clause; _temp420->next_stmt=( void*)((
void*) c->next_stmt); _temp420->try_depth= c->try_depth; _temp420;}); _temp419->capability=(
void*)(( void*) f->capability); _temp419->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp419;});} void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected return_typ",
sizeof( unsigned char), 29u));} return( void*)(( Cyc_Tcenv_get_fenv( te))->shared)->return_typ;}
struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv( te))->type_vars;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_type_vars",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp421= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); goto _LL422; _LL422: Cyc_Tcutil_add_tvar_identities(
tvs);{ struct Cyc_List_List* _temp423=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, _temp421->type_vars); goto
_LL424; _LL424: Cyc_Tcutil_check_unique_tvars( loc, _temp423); _temp421->type_vars=
_temp423; return Cyc_Tcenv_put_fenv( te, _temp421);}}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_local_var",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp425= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); goto _LL426; _LL426: { struct _tagged_arr* _temp427=(*
vd->name).f2; goto _LL428; _LL428:{ void* _temp429=(* vd->name).f1; _LL431: if(
_temp429 ==( void*) Cyc_Absyn_Loc_n){ goto _LL432;} else{ goto _LL433;} _LL433:
goto _LL434; _LL432: goto _LL430; _LL434:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp435=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp435[ 0]=({ struct Cyc_Core_Impossible_struct _temp436; _temp436.tag= Cyc_Core_Impossible;
_temp436.f1= _tag_arr("add_local_var: called with Rel_n", sizeof( unsigned char),
33u); _temp436;}); _temp435;})); goto _LL430; _LL430:;} _temp425->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void*
data)) Cyc_Dict_insert)( _temp425->locals, _temp427,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp437=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp437[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp438; _temp438.tag= Cyc_Absyn_Local_b;
_temp438.f1= vd; _temp438;}); _temp437;})); return Cyc_Tcenv_put_fenv( te,
_temp425);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_pat_var",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* _temp439= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); goto _LL440; _LL440: { struct _tagged_arr* _temp441=(*
vd->name).f2; goto _LL442; _LL442: _temp439->locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(
_temp439->locals, _temp441,( void*)({ struct Cyc_Absyn_Pat_b_struct* _temp443=(
struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp443[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp444; _temp444.tag= Cyc_Absyn_Pat_b;
_temp444.f1= vd; _temp444;}); _temp443;})); return Cyc_Tcenv_put_fenv( te,
_temp439);}}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp447; struct
_tagged_arr* _temp448; void* _temp450; struct _tuple0* _temp445= q; _temp447=*
_temp445; _LL451: _temp450= _temp447.f1; goto _LL449; _LL449: _temp448= _temp447.f2;
goto _LL446; _LL446: { void* _temp452= _temp450; struct Cyc_List_List* _temp461;
_LL454: if( _temp452 ==( void*) Cyc_Absyn_Loc_n){ goto _LL455;} else{ goto
_LL456;} _LL456: if(( unsigned int) _temp452 > 1u?*(( int*) _temp452) == Cyc_Absyn_Rel_n:
0){ _LL462: _temp461=(( struct Cyc_Absyn_Rel_n_struct*) _temp452)->f1; if(
_temp461 == 0){ goto _LL460;} else{ goto _LL458;}} else{ goto _LL458;} _LL460:
if( te->le != 0){ goto _LL457;} else{ goto _LL458;} _LL458: goto _LL459; _LL455:
if( te->le == 0){( int) _throw(( void*) Cyc_Dict_Absent);} else{ goto _LL457;}
_LL457: { struct Cyc_Tcenv_Fenv* _temp463= Cyc_Tcenv_get_fenv( te); goto _LL464;
_LL464: { struct _handler_cons _temp465; _push_handler(& _temp465);{ int
_temp467= 0; if( setjmp( _temp465.handler)){ _temp467= 1;} if( ! _temp467){{
void* _temp470=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp468=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp468[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp469; _temp469.tag= Cyc_Tcenv_VarRes; _temp469.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp463->locals,
_temp448); _temp469;}); _temp468;}); _npop_handler( 0u); return _temp470;};
_pop_handler();} else{ void* _temp466=( void*) _exn_thrown; void* _temp472=
_temp466; _LL474: if( _temp472 == Cyc_Dict_Absent){ goto _LL475;} else{ goto
_LL476;} _LL476: goto _LL477; _LL475: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL477:( void) _throw( _temp472); _LL473:;}}}} _LL459: { struct
_handler_cons _temp478; _push_handler(& _temp478);{ int _temp480= 0; if( setjmp(
_temp478.handler)){ _temp480= 1;} if( ! _temp480){{ void* _temp481= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp481;}; _pop_handler();} else{ void*
_temp479=( void*) _exn_thrown; void* _temp483= _temp479; _LL485: if( _temp483 ==
Cyc_Dict_Absent){ goto _LL486;} else{ goto _LL487;} _LL487: goto _LL488; _LL486:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp489= Cyc_Absynpp_qvar2string(
q); xprintf("unbound identifier: %.*s", _temp489.last_plus_one - _temp489.curr,
_temp489.curr);})); return( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp490=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp490[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp491; _temp491.tag= Cyc_Tcenv_VarRes;
_temp491.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp491;}); _temp490;});
_LL488:( void) _throw( _temp483); _LL484:;}}} _LL453:;}} void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_continue", sizeof( unsigned char), 35u));}{
void* _temp492=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp502; _LL494: if(( unsigned int) _temp492 > 3u?*((
int*) _temp492) == Cyc_Tcenv_Stmt_j: 0){ _LL503: _temp502=(( struct Cyc_Tcenv_Stmt_j_struct*)
_temp492)->f1; goto _LL495;} else{ goto _LL496;} _LL496: if( _temp492 ==( void*)
Cyc_Tcenv_NotLoop_j){ goto _LL497;} else{ goto _LL498;} _LL498: if( _temp492 ==(
void*) Cyc_Tcenv_CaseEnd_j){ goto _LL499;} else{ goto _LL500;} _LL500: if(
_temp492 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL501;} else{ goto _LL493;} _LL495:
_temp502->non_local_preds=({ struct Cyc_List_List* _temp504=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp504->hd=( void*) s; _temp504->tl=
_temp502->non_local_preds; _temp504;});* sopt=( struct Cyc_Absyn_Stmt*) _temp502;
return; _LL497: Cyc_Tcutil_terr( s->loc, _tag_arr("continue not in a loop",
sizeof( unsigned char), 23u)); return; _LL499:(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd", sizeof(
unsigned char), 32u)); return; _LL501:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: continue dest is CaseEnd", sizeof( unsigned char), 32u));
return; _LL493:;}} void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_break",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv(
te); void* _temp505=( void*)( fe->ctrl_env)->break_stmt; struct Cyc_Absyn_Stmt*
_temp515; _LL507: if(( unsigned int) _temp505 > 3u?*(( int*) _temp505) == Cyc_Tcenv_Stmt_j:
0){ _LL516: _temp515=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp505)->f1; goto
_LL508;} else{ goto _LL509;} _LL509: if( _temp505 ==( void*) Cyc_Tcenv_NotLoop_j){
goto _LL510;} else{ goto _LL511;} _LL511: if( _temp505 ==( void*) Cyc_Tcenv_FnEnd_j){
goto _LL512;} else{ goto _LL513;} _LL513: if( _temp505 ==( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL514;} else{ goto _LL506;} _LL508: _temp515->non_local_preds=({ struct
Cyc_List_List* _temp517=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp517->hd=( void*) s; _temp517->tl= _temp515->non_local_preds; _temp517;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp515; return; _LL510: Cyc_Tcutil_terr( s->loc,
_tag_arr("break not in a loop or switch", sizeof( unsigned char), 30u)); return;
_LL512: if(( void*)( fe->shared)->return_typ !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(
s->loc, _tag_arr("break causes function not to return a value", sizeof(
unsigned char), 44u));} return; _LL514: Cyc_Tcutil_terr( s->loc, _tag_arr("break causes outer switch clause to implicitly fallthru",
sizeof( unsigned char), 56u)); return; _LL506:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_arr* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( int(*)( struct _tagged_arr
msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_goto", sizeof(
unsigned char), 31u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* _temp518=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
goto _LL519; _LL519: if( _temp518 == 0){ struct Cyc_SlowDict_Dict* _temp520=( fe->shared)->needed_labels;
goto _LL521; _LL521: { struct Cyc_Core_Opt* _temp522=(( struct Cyc_Core_Opt*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)(
_temp520, l); goto _LL523; _LL523: if( _temp522 == 0){ _temp522=({ struct Cyc_Core_Opt*
_temp524=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp524->v=( void*) 0; _temp524;});}( fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_arr* key, struct Cyc_List_List* data))
Cyc_SlowDict_insert)( _temp520, l,({ struct Cyc_List_List* _temp525=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp525->hd=( void*) s; _temp525->tl=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp522))->v;
_temp525;}));}} else{(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*)
_check_null( _temp518))->v)->non_local_preds=({ struct Cyc_List_List* _temp526=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp526->hd=(
void*) s; _temp526->tl=(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*)
_check_null( _temp518))->v)->non_local_preds; _temp526;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*) _check_null( _temp518))->v);}}}
struct _tuple3* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause*** clauseopt){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_fallthru",
sizeof( unsigned char), 35u));}{ struct Cyc_Tcenv_Fenv* _temp527= Cyc_Tcenv_get_fenv(
te); goto _LL528; _LL528: { struct _tuple3* _temp529=( _temp527->ctrl_env)->fallthru_clause;
goto _LL530; _LL530: if( _temp529 != 0){(((*(( struct _tuple3*) _check_null(
_temp529))).f1)->body)->non_local_preds=({ struct Cyc_List_List* _temp531=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp531->hd=(
void*) s; _temp531->tl=(((*(( struct _tuple3*) _check_null( _temp529))).f1)->body)->non_local_preds;
_temp531;});* clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp532=( struct
Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) *
1); _temp532[ 0]=(*(( struct _tuple3*) _check_null( _temp529))).f1; _temp532;});}
return _temp529;}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause*
clause){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_fallthru", sizeof( unsigned char), 31u));}{
struct Cyc_List_List* _temp533= 0; goto _LL534; _LL534: for( 0; vds != 0; vds=((
struct Cyc_List_List*) _check_null( vds))->tl){ _temp533=({ struct Cyc_List_List*
_temp535=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp535->hd=( void*)(( void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( vds))->hd)->type); _temp535->tl= _temp533; _temp535;});} _temp533=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp533);{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));(
fe->ctrl_env)->fallthru_clause=({ struct _tuple3* _temp536=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp536->f1= clause; _temp536->f2= new_tvs;
_temp536->f3= _temp533; _temp536;}); return Cyc_Tcenv_put_fenv( te, fe);}}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv* te){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected clear_fallthru",
sizeof( unsigned char), 33u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause= 0; return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le == 0){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_loop",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp537=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp537[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp538; _temp538.tag= Cyc_Tcenv_Stmt_j; _temp538.f1= continue_dest; _temp538;});
_temp537;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp539=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp539[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp540; _temp540.tag= Cyc_Tcenv_Stmt_j;
_temp540.f1= continue_dest; _temp540;}); _temp539;}))); return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected enter_try", sizeof( unsigned char), 28u));}{ struct
Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te)); ++(
fe->ctrl_env)->try_depth; return Cyc_Tcenv_put_fenv( te, fe);}} int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected get_try_depth", sizeof(
unsigned char), 32u));} return(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->try_depth;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct Cyc_Tcenv_Tenv* te){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_switch",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)(
fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)
Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_next",
sizeof( unsigned char), 27u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=( void*) j); return
Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_label",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* _temp541= Cyc_Tcenv_get_fenv(
te); goto _LL542; _LL542: { struct Cyc_SlowDict_Dict* _temp543=( _temp541->shared)->needed_labels;
goto _LL544; _LL544: { struct Cyc_Core_Opt* _temp545=(( struct Cyc_Core_Opt*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)(
_temp543, v); goto _LL546; _LL546: if( _temp545 != 0){( _temp541->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_arr*
key)) Cyc_SlowDict_delete)( _temp543, v);{ struct Cyc_List_List* _temp547=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp545))->v; goto
_LL548; _LL548: s->non_local_preds= _temp547; for( 0; _temp547 != 0; _temp547=((
struct Cyc_List_List*) _check_null( _temp547))->tl){ void* _temp549=( void*)((
struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*) _check_null( _temp547))->hd)->r;
struct Cyc_Absyn_Stmt* _temp555; struct Cyc_Absyn_Stmt** _temp557; struct
_tagged_arr* _temp558; _LL551: if(( unsigned int) _temp549 > 1u?*(( int*)
_temp549) == Cyc_Absyn_Goto_s: 0){ _LL559: _temp558=(( struct Cyc_Absyn_Goto_s_struct*)
_temp549)->f1; goto _LL556; _LL556: _temp555=(( struct Cyc_Absyn_Goto_s_struct*)
_temp549)->f2; _temp557=&(( struct Cyc_Absyn_Goto_s_struct*) _temp549)->f2; goto
_LL552;} else{ goto _LL553;} _LL553: goto _LL554; _LL552:* _temp557=( struct Cyc_Absyn_Stmt*)
s; goto _LL550; _LL554:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: add_label backpatching of non-goto", sizeof( unsigned char), 42u));
goto _LL550; _LL550:;}}} if((( int(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_member)(( _temp541->shared)->seen_labels, v)){ Cyc_Tcutil_terr(
s->loc,( struct _tagged_arr)({ struct _tagged_arr _temp560=* v; xprintf("Repeated label: %.*s",
_temp560.last_plus_one - _temp560.curr, _temp560.curr);}));}( _temp541->shared)->seen_labels=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Stmt* data)) Cyc_Dict_insert)(( _temp541->shared)->seen_labels,
v, s); return te;}}}} int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*
te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected get_encloser", sizeof(
unsigned char), 31u));} return( Cyc_Tcenv_get_fenv( te))->encloser;} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_encloser", sizeof( unsigned char), 31u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te));
fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv::add_region: unexpected add_region",
sizeof( unsigned char), 41u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar* tv;{ void* _temp561= Cyc_Tcutil_compress(
r); struct Cyc_Absyn_Tvar* _temp567; _LL563: if(( unsigned int) _temp561 > 4u?*((
int*) _temp561) == Cyc_Absyn_VarType: 0){ _LL568: _temp567=(( struct Cyc_Absyn_VarType_struct*)
_temp561)->f1; goto _LL564;} else{ goto _LL565;} _LL565: goto _LL566; _LL564: tv=
_temp567; goto _LL562; _LL566: tv=(( struct Cyc_Absyn_Tvar*(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("bad add region", sizeof(
unsigned char), 15u)); goto _LL562; _LL562:;} fe->region_order= Cyc_Tcenv_add_region_po(
fe->region_order, tv);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp569=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp569[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp570; _temp570.tag= Cyc_Absyn_JoinEff;
_temp570.f1=({ struct Cyc_List_List* _temp571=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp571->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp573=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp573[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp574; _temp574.tag= Cyc_Absyn_AccessEff;
_temp574.f1=( void*) r; _temp574;}); _temp573;})); _temp571->tl=({ struct Cyc_List_List*
_temp572=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp572->hd=( void*)(( void*) fe->capability); _temp572->tl= 0; _temp572;});
_temp571;}); _temp570;}); _temp569;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected new_block",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp582=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp582[ 0]=({ struct Cyc_Absyn_VarType_struct _temp583; _temp583.tag= Cyc_Absyn_VarType;
_temp583.f1= block_rgn; _temp583;}); _temp582;}); fe->type_vars=({ struct Cyc_List_List*
_temp575=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp575->hd=( void*) block_rgn; _temp575->tl= fe->type_vars; _temp575;}); Cyc_Tcutil_check_unique_tvars(
loc, fe->type_vars); fe->region_order= Cyc_Tcenv_add_region_po( fe->region_order,
block_rgn);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp576=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp576[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp577; _temp577.tag= Cyc_Absyn_JoinEff;
_temp577.f1=({ struct Cyc_List_List* _temp578=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp578->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp580=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp580[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp581; _temp581.tag= Cyc_Absyn_AccessEff;
_temp581.f1=( void*) block_typ; _temp581;}); _temp580;})); _temp578->tl=({
struct Cyc_List_List* _temp579=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp579->hd=( void*)(( void*) fe->capability); _temp579->tl=
0; _temp579;}); _temp578;}); _temp577;}); _temp576;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
struct Cyc_Absyn_Tvar* t= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); Cyc_Tcutil_add_tvar_identity(
t); return Cyc_Tcenv_new_named_block( loc, te, t);} void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return( void*) Cyc_Absyn_HeapRgn;}
return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;} void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* rgn){ if( te->le
!= 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv( te))->capability; if( Cyc_Tcutil_region_in_effect(
0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect( 1, rgn, capability)){
return;}} Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp584= Cyc_Absynpp_typ2string( rgn); xprintf("Expression accesses unavailable region %.*s",
_temp584.last_plus_one - _temp584.curr, _temp584.curr);}));} int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv* te, void* rt_a, void* rt_b){ if( rt_a ==( void*) Cyc_Absyn_HeapRgn){
return 1;} if( te->le != 0){ struct _tuple5 _temp586=({ struct _tuple5 _temp585;
_temp585.f1= Cyc_Tcutil_compress( rt_a); _temp585.f2= Cyc_Tcutil_compress( rt_b);
_temp585;}); void* _temp592; struct Cyc_Absyn_Tvar* _temp594; void* _temp596;
struct Cyc_Absyn_Tvar* _temp598; _LL588: _LL597: _temp596= _temp586.f1; if((
unsigned int) _temp596 > 4u?*(( int*) _temp596) == Cyc_Absyn_VarType: 0){ _LL599:
_temp598=(( struct Cyc_Absyn_VarType_struct*) _temp596)->f1; goto _LL593;} else{
goto _LL590;} _LL593: _temp592= _temp586.f2; if(( unsigned int) _temp592 > 4u?*((
int*) _temp592) == Cyc_Absyn_VarType: 0){ _LL595: _temp594=(( struct Cyc_Absyn_VarType_struct*)
_temp592)->f1; goto _LL589;} else{ goto _LL590;} _LL590: goto _LL591; _LL589:
return Cyc_Tcenv_check_region_outlives(( Cyc_Tcenv_get_fenv( te))->region_order,
_temp598, _temp594); _LL591: return 0; _LL587:;} return 0;} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp600=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp600[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp601; _temp601.tag= Cyc_Absyn_JoinEff;
_temp601.f1= 0; _temp601;}); _temp600;});} else{ struct Cyc_Tcenv_Fenv* _temp602=
Cyc_Tcenv_get_fenv( te); goto _LL603; _LL603: capability=( void*) _temp602->capability;}
if( ! Cyc_Tcutil_subset_effect( 1, eff, capability)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp604= Cyc_Absynpp_typ2string(
capability); struct _tagged_arr _temp605= Cyc_Absynpp_typ2string( eff); xprintf("Capability %.*s does not cover function's effect %.*s",
_temp604.last_plus_one - _temp604.curr, _temp604.curr, _temp605.last_plus_one -
_temp605.curr, _temp605.curr);}));}} void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
po){ for( 0; po != 0; po=(( struct Cyc_List_List*) _check_null( po))->tl){
struct _tuple5 _temp608; void* _temp609; void* _temp611; struct _tuple5*
_temp606=( struct _tuple5*)(( struct Cyc_List_List*) _check_null( po))->hd;
_temp608=* _temp606; _LL612: _temp611= _temp608.f1; goto _LL610; _LL610:
_temp609= _temp608.f2; goto _LL607; _LL607: if( ! Cyc_Tcenv_region_outlives( te,
_temp611, _temp609)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp613= Cyc_Absynpp_typ2string( _temp611); struct _tagged_arr
_temp614= Cyc_Absynpp_typ2string( _temp609); xprintf("function requires that region %.*s outlive %.*s",
_temp613.last_plus_one - _temp613.curr, _temp613.curr, _temp614.last_plus_one -
_temp614.curr, _temp614.curr);}));}}} struct _tuple6{ struct _tagged_arr* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict*
locals=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar*
rgn0=({ struct Cyc_Absyn_Tvar* _temp674=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp674->name=({ struct _tagged_arr* _temp675=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp675[ 0]=(
struct _tagged_arr)({ struct _tagged_arr _temp676=*(* fd->name).f2; xprintf("`%.*s",
_temp676.last_plus_one - _temp676.curr, _temp676.curr);}); _temp675;}); _temp674->identity=
Cyc_Tcutil_new_tvar_id(); _temp674->kind=(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp674;}); struct Cyc_List_List*
_temp616=({ struct Cyc_List_List* _temp615=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp615->hd=( void*) rgn0; _temp615->tl= fd->tvs;
_temp615;}); goto _LL617; _LL617: Cyc_Tcutil_check_unique_tvars( loc, _temp616);{
struct Cyc_List_List* _temp618= Cyc_Tcenv_add_region_po( Cyc_Tcenv_initial_region_po(
loc, fd->tvs, fd->rgn_po), rgn0); goto _LL619; _LL619: { void* param_rgn=( void*)({
struct Cyc_Absyn_VarType_struct* _temp672=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp672[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp673; _temp673.tag= Cyc_Absyn_VarType; _temp673.f1= rgn0; _temp673;});
_temp672;}); struct Cyc_List_List* _temp620= 0; goto _LL621; _LL621:{ struct Cyc_List_List*
_temp622= fd->args; goto _LL623; _LL623: for( 0; _temp622 != 0; _temp622=((
struct Cyc_List_List*) _check_null( _temp622))->tl){ struct Cyc_Absyn_Vardecl*
_temp630=({ struct Cyc_Absyn_Vardecl* _temp624=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp624[ 0]=({ struct Cyc_Absyn_Vardecl
_temp625; _temp625.sc=( void*)(( void*) Cyc_Absyn_Public); _temp625.name=({
struct _tuple0* _temp628=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp628[ 0]=({ struct _tuple0 _temp629; _temp629.f1=( void*) Cyc_Absyn_Loc_n;
_temp629.f2=(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp622))->hd)).f1;
_temp629;}); _temp628;}); _temp625.tq=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( _temp622))->hd)).f2; _temp625.type=( void*)(*(( struct _tuple6*)((
struct Cyc_List_List*) _check_null( _temp622))->hd)).f3; _temp625.initializer= 0;
_temp625.rgn=({ struct Cyc_Core_Opt* _temp626=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp626[ 0]=({ struct Cyc_Core_Opt _temp627;
_temp627.v=( void*) param_rgn; _temp627;}); _temp626;}); _temp625.attributes= 0;
_temp625;}); _temp624;}); goto _LL631; _LL631: _temp620=({ struct Cyc_List_List*
_temp632=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp632[ 0]=({ struct Cyc_List_List _temp633; _temp633.hd=( void*) _temp630;
_temp633.tl= _temp620; _temp633;}); _temp632;}); locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(
locals,(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp622))->hd)).f1,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp634=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp634[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp635; _temp635.tag= Cyc_Absyn_Param_b; _temp635.f1= _temp630; _temp635;});
_temp634;}));}} if( fd->cyc_varargs != 0){ int _temp638; void* _temp640; void*
_temp642; struct Cyc_Absyn_Tqual _temp644; struct Cyc_Core_Opt* _temp646; struct
Cyc_Absyn_VarargInfo _temp636=*(( struct Cyc_Absyn_VarargInfo*) _check_null( fd->cyc_varargs));
_LL647: _temp646= _temp636.name; goto _LL645; _LL645: _temp644= _temp636.tq;
goto _LL643; _LL643: _temp642=( void*) _temp636.type; goto _LL641; _LL641:
_temp640=( void*) _temp636.rgn; goto _LL639; _LL639: _temp638= _temp636.inject;
goto _LL637; _LL637: if( _temp646 != 0){ void* _temp648= Cyc_Absyn_tagged_typ(
_temp642, _temp640, _temp644); goto _LL649; _LL649: { struct Cyc_Absyn_Vardecl*
_temp656=({ struct Cyc_Absyn_Vardecl* _temp650=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp650[ 0]=({ struct Cyc_Absyn_Vardecl
_temp651; _temp651.sc=( void*)(( void*) Cyc_Absyn_Public); _temp651.name=({
struct _tuple0* _temp654=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp654[ 0]=({ struct _tuple0 _temp655; _temp655.f1=( void*) Cyc_Absyn_Loc_n;
_temp655.f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp646))->v;
_temp655;}); _temp654;}); _temp651.tq= Cyc_Absyn_empty_tqual(); _temp651.type=(
void*) _temp648; _temp651.initializer= 0; _temp651.rgn=({ struct Cyc_Core_Opt*
_temp652=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp652[ 0]=({ struct Cyc_Core_Opt _temp653; _temp653.v=( void*) param_rgn;
_temp653;}); _temp652;}); _temp651.attributes= 0; _temp651;}); _temp650;}); goto
_LL657; _LL657: _temp620=({ struct Cyc_List_List* _temp658=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp658->hd=( void*) _temp656;
_temp658->tl= _temp620; _temp658;}); locals=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,(
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp646))->v,( void*)({
struct Cyc_Absyn_Param_b_struct* _temp659=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp659[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp660; _temp660.tag= Cyc_Absyn_Param_b; _temp660.f1= _temp656; _temp660;});
_temp659;}));}} else{ Cyc_Tcutil_terr( loc, _tag_arr("missing name for varargs",
sizeof( unsigned char), 25u));}} fd->param_vardecls=({ struct Cyc_Core_Opt*
_temp661=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp661[ 0]=({ struct Cyc_Core_Opt _temp662; _temp662.v=( void*) _temp620;
_temp662;}); _temp661;}); return({ struct Cyc_Tcenv_Fenv* _temp663=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp663->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp671=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp671->return_typ=( void*)(( void*) fd->ret_type); _temp671->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp671->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_SlowDict_empty)(
Cyc_String_zstrptrcmp); _temp671;}); _temp663->type_vars= _temp616; _temp663->region_order=
_temp618; _temp663->locals= locals; _temp663->encloser= fd->body; _temp663->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp670=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp670->continue_stmt=( void*)(( void*)
Cyc_Tcenv_NotLoop_j); _temp670->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp670->fallthru_clause= 0; _temp670->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp670->try_depth= 0; _temp670;}); _temp663->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp664=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp664[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp665; _temp665.tag= Cyc_Absyn_JoinEff; _temp665.f1=({ struct Cyc_List_List*
_temp666=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp666->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp668=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp668[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp669; _temp669.tag= Cyc_Absyn_AccessEff;
_temp669.f1=( void*) param_rgn; _temp669;}); _temp668;})); _temp666->tl=({
struct Cyc_List_List* _temp667=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp667->hd=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( fd->effect))->v); _temp667->tl= 0; _temp667;}); _temp666;});
_temp665;}); _temp664;})); _temp663->curr_rgn=( void*) param_rgn; _temp663;});}}}