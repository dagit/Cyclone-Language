 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
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
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const
int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_string source; struct Cyc_Position_Segment* seg; void* kind;
struct _tagged_string desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u];
extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct _tuple0{
void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
_tagged_string* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
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
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
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
_tagged_string f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Null_c= 0;
static const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static
const int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const
int Cyc_Absyn_Mod= 4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq=
6; static const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static
const int Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int
Cyc_Absyn_Not= 11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand=
13; static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
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
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 32; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 33; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 34; struct Cyc_Absyn_Fill_e_struct{
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
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
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
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct _tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl*
Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq); extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); static
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
struct _tagged_string*, struct Cyc_List_List*); extern void* Cyc_Tcenv_lookup_ordinary(
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
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct _tagged_string*, struct
Cyc_Absyn_Stmt**); struct _tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
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
struct _tagged_string); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set;
extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar* t); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void*
e2); extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e);
extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*, struct
Cyc_List_List*); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k);
extern int* Cyc_Tcutil_new_tvar_id(); extern void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*);
unsigned char Cyc_Tcenv_Env_error[ 14u]="\000\000\000\000Env_error"; void* Cyc_Tcenv_env_err(
struct _tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"Error: %.*s\n", _temp0.last_plus_one - _temp0.curr,
_temp0.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);(
void) _throw(( void*) Cyc_Tcenv_Env_error);} struct Cyc_Tcenv_Tenv; struct Cyc_Tcenv_Genv;
struct Cyc_Tcenv_CtrlEnv{ void* continue_stmt; void* break_stmt; struct _tuple3*
fallthru_clause; void* next_stmt; int try_depth; } ; struct Cyc_Tcenv_SharedFenv{
void* return_typ; struct Cyc_Dict_Dict* seen_labels; struct Cyc_SlowDict_Dict*
needed_labels; } ; struct Cyc_Tcenv_RgnPO{ struct Cyc_Absyn_Tvar* tvar; struct
Cyc_List_List* ancestors; } ; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv*
shared; struct Cyc_List_List* type_vars; struct Cyc_List_List* region_order;
struct Cyc_Dict_Dict* locals; struct Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv*
ctrl_env; void* capability; void* curr_rgn; } ; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){
Cyc_Tcutil_empty_var_set=({ struct Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1->v=( void*)(( struct Cyc_Set_Set*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)(
Cyc_String_zstrptrcmp); _temp1;});{ struct Cyc_Tcenv_Genv* _temp2= Cyc_Tcenv_empty_genv();
goto _LL3; _LL3: { struct Cyc_List_List* _temp4= 0; goto _LL5; _LL5: _temp2->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( _temp2->tuniondecls,(*
Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_xed);{ struct Cyc_Dict_Dict* _temp6=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp),
_temp4, _temp2); goto _LL7; _LL7: return({ struct Cyc_Tcenv_Tenv* _temp8=(
struct Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp8->ns=
_temp4; _temp8->ae= _temp6; _temp8->le= 0; _temp8;});}}}} static struct Cyc_Tcenv_Genv*
Cyc_Tcenv_lookup_namespace( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_string* n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
Cyc_Tcenv_resolve_namespace( te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns == 0){ return(( struct Cyc_List_List*(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp9=(
unsigned char*)"Tcenv::outer_namespace"; struct _tagged_string _temp10; _temp10.curr=
_temp9; _temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 23; _temp10;}));}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( ns)))->tl);} static void Cyc_Tcenv_check_repeat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss){
for( 0; nss != 0; nss=(( struct Cyc_List_List*) _check_null( nss))->tl){ struct
Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( nss))->hd); struct _handler_cons _temp11;
_push_handler(& _temp11);{ int _temp13= 0; if( setjmp( _temp11.handler)){
_temp13= 1;} if( ! _temp13){ lookup( ge2, v); Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp14=* v; xprintf("%.*s is ambiguous",
_temp14.last_plus_one - _temp14.curr, _temp14.curr);}));; _pop_handler();} else{
void* _temp12=( void*) _exn_thrown; void* _temp16= _temp12; _LL18: if( _temp16
== Cyc_Dict_Absent){ goto _LL19;} else{ goto _LL20;} _LL20: goto _LL21; _LL19:
goto _LL17; _LL21:( void) _throw( _temp16); _LL17:;}}} return;} static void* Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v){
struct Cyc_List_List* cns= te->ns; void* result; while( 1) { struct Cyc_Tcenv_Genv*
ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, cns);{ struct _handler_cons _temp22;
_push_handler(& _temp22);{ int _temp24= 0; if( setjmp( _temp22.handler)){
_temp24= 1;} if( ! _temp24){ result= lookup( ge, v);(( void(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss))
Cyc_Tcenv_check_repeat)( te, loc, lookup, v, ge->availables);{ void* _temp25=
result; _npop_handler( 0u); return _temp25;}; _pop_handler();} else{ void*
_temp23=( void*) _exn_thrown; void* _temp27= _temp23; _LL29: if( _temp27 == Cyc_Dict_Absent){
goto _LL30;} else{ goto _LL31;} _LL31: goto _LL32; _LL30: goto _LL28; _LL32:(
void) _throw( _temp27); _LL28:;}}}{ struct Cyc_List_List* _temp33= ge->availables;
goto _LL34; _LL34: for( 0; _temp33 != 0; _temp33=(( struct Cyc_List_List*)
_check_null( _temp33))->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp33))->hd);
struct _handler_cons _temp35; _push_handler(& _temp35);{ int _temp37= 0; if(
setjmp( _temp35.handler)){ _temp37= 1;} if( ! _temp37){ result= lookup( ge2, v);((
void(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)( te, loc, lookup, v,((
struct Cyc_List_List*) _check_null( _temp33))->tl);{ void* _temp38= result;
_npop_handler( 0u); return _temp38;}; _pop_handler();} else{ void* _temp36=(
void*) _exn_thrown; void* _temp40= _temp36; _LL42: if( _temp40 == Cyc_Dict_Absent){
goto _LL43;} else{ goto _LL44;} _LL44: goto _LL45; _LL43: goto _LL41; _LL45:(
void) _throw( _temp40); _LL41:;}}}} if( cns == 0){( void) _throw(( void*) Cyc_Dict_Absent);}
cns= Cyc_Tcenv_outer_namespace( cns);}} struct _tuple4{ void* f1; int f2; } ;
static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv* ge,
struct _tagged_string* v){ struct _tuple4* ans=(( struct _tuple4*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v);(* ans).f2=
1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp48;
struct _tagged_string* _temp49; void* _temp51; struct _tuple0* _temp46= q;
_temp48=* _temp46; _LL52: _temp51= _temp48.f1; goto _LL50; _LL50: _temp49=
_temp48.f2; goto _LL47; _LL47: { void* _temp53= _temp51; struct Cyc_List_List*
_temp63; struct Cyc_List_List* _temp65; struct Cyc_List_List _temp67; struct Cyc_List_List*
_temp68; struct _tagged_string* _temp70; struct Cyc_List_List* _temp72; _LL55:
if( _temp53 ==( void*) Cyc_Absyn_Loc_n){ goto _LL56;} else{ goto _LL57;} _LL57:
if(( unsigned int) _temp53 > 1u?*(( int*) _temp53) == Cyc_Absyn_Rel_n: 0){ _LL64:
_temp63=(( struct Cyc_Absyn_Rel_n_struct*) _temp53)->f1; if( _temp63 == 0){ goto
_LL58;} else{ goto _LL59;}} else{ goto _LL59;} _LL59: if(( unsigned int) _temp53
> 1u?*(( int*) _temp53) == Cyc_Absyn_Rel_n: 0){ _LL66: _temp65=(( struct Cyc_Absyn_Rel_n_struct*)
_temp53)->f1; if( _temp65 == 0){ goto _LL61;} else{ _temp67=* _temp65; _LL71:
_temp70=( struct _tagged_string*) _temp67.hd; goto _LL69; _LL69: _temp68=
_temp67.tl; goto _LL60;}} else{ goto _LL61;} _LL61: if(( unsigned int) _temp53 >
1u?*(( int*) _temp53) == Cyc_Absyn_Abs_n: 0){ _LL73: _temp72=(( struct Cyc_Absyn_Abs_n_struct*)
_temp53)->f1; goto _LL62;} else{ goto _LL54;} _LL56: goto _LL58; _LL58: return((
void*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_ordinary_global_f,
_temp49); _LL60: { struct Cyc_Tcenv_Genv* _temp74= Cyc_Tcenv_lookup_namespace(
te, loc, _temp70, _temp68); goto _LL75; _LL75: return Cyc_Tcenv_lookup_ordinary_global_f(
_temp74, _temp49);} _LL62: return Cyc_Tcenv_lookup_ordinary_global_f((( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, _temp72), _temp49); _LL54:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ struct Cyc_List_List* _temp76= te->ns;
goto _LL77; _LL77: { struct Cyc_List_List* _temp78= 0; goto _LL79; _LL79: while(
1) { struct Cyc_Tcenv_Genv* _temp80=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp76); goto _LL81;
_LL81:{ struct Cyc_List_List* _temp82= _temp80->availables; goto _LL83; _LL83:
for( 0; _temp82 != 0; _temp82=(( struct Cyc_List_List*) _check_null( _temp82))->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( _temp82))->hd); if((( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( ge2->namespaces, n)){ _temp78=({
struct Cyc_List_List* _temp84=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp84->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp76,({ struct Cyc_List_List*
_temp85=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp85->hd=( void*) n; _temp85->tl= ns; _temp85;})); _temp84->tl= _temp78;
_temp84;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt))
Cyc_Set_member)( _temp80->namespaces, n)){ _temp78=({ struct Cyc_List_List*
_temp86=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp86->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp76,({ struct Cyc_List_List* _temp87=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp87->hd=(
void*) n; _temp87->tl= ns; _temp87;})); _temp86->tl= _temp78; _temp86;});} if(
_temp78 != 0){ if((( struct Cyc_List_List*) _check_null( _temp78))->tl != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp88=*
n; xprintf("%.*s is ambiguous", _temp88.last_plus_one - _temp88.curr, _temp88.curr);}));}
return( struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp78))->hd;}
if( _temp76 == 0){( void) _throw(( void*) Cyc_Dict_Absent);} _temp76= Cyc_Tcenv_outer_namespace(
_temp76);}}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp91;
struct _tagged_string* _temp92; void* _temp94; struct _tuple0* _temp89= q;
_temp91=* _temp89; _LL95: _temp94= _temp91.f1; goto _LL93; _LL93: _temp92=
_temp91.f2; goto _LL90; _LL90: { void* _temp96= _temp94; struct Cyc_List_List*
_temp106; struct Cyc_List_List* _temp108; struct Cyc_List_List* _temp110; struct
Cyc_List_List _temp112; struct Cyc_List_List* _temp113; struct _tagged_string*
_temp115; _LL98: if( _temp96 ==( void*) Cyc_Absyn_Loc_n){ goto _LL99;} else{
goto _LL100;} _LL100: if(( unsigned int) _temp96 > 1u?*(( int*) _temp96) == Cyc_Absyn_Rel_n:
0){ _LL107: _temp106=(( struct Cyc_Absyn_Rel_n_struct*) _temp96)->f1; if(
_temp106 == 0){ goto _LL101;} else{ goto _LL102;}} else{ goto _LL102;} _LL102:
if(( unsigned int) _temp96 > 1u?*(( int*) _temp96) == Cyc_Absyn_Abs_n: 0){
_LL109: _temp108=(( struct Cyc_Absyn_Abs_n_struct*) _temp96)->f1; goto _LL103;}
else{ goto _LL104;} _LL104: if(( unsigned int) _temp96 > 1u?*(( int*) _temp96)
== Cyc_Absyn_Rel_n: 0){ _LL111: _temp110=(( struct Cyc_Absyn_Rel_n_struct*)
_temp96)->f1; if( _temp110 == 0){ goto _LL97;} else{ _temp112=* _temp110; _LL116:
_temp115=( struct _tagged_string*) _temp112.hd; goto _LL114; _LL114: _temp113=
_temp112.tl; goto _LL105;}} else{ goto _LL97;} _LL99: goto _LL101; _LL101:
return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_structdecl_f, _temp92); _LL103: { struct Cyc_Tcenv_Genv*
_temp117=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp108); goto _LL118; _LL118: return(( struct
Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( _temp117->structdecls, _temp92);} _LL105: { struct Cyc_Tcenv_Genv*
_temp119= Cyc_Tcenv_lookup_namespace( te, loc, _temp115, _temp113); goto _LL120;
_LL120: return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp119->structdecls, _temp92);}
_LL97:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp123; struct _tagged_string* _temp124; void* _temp126; struct _tuple0*
_temp121= q; _temp123=* _temp121; _LL127: _temp126= _temp123.f1; goto _LL125;
_LL125: _temp124= _temp123.f2; goto _LL122; _LL122: { void* _temp128= _temp126;
struct Cyc_List_List* _temp138; struct Cyc_List_List* _temp140; struct Cyc_List_List*
_temp142; struct Cyc_List_List _temp144; struct Cyc_List_List* _temp145; struct
_tagged_string* _temp147; _LL130: if( _temp128 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL131;} else{ goto _LL132;} _LL132: if(( unsigned int) _temp128 > 1u?*(( int*)
_temp128) == Cyc_Absyn_Rel_n: 0){ _LL139: _temp138=(( struct Cyc_Absyn_Rel_n_struct*)
_temp128)->f1; if( _temp138 == 0){ goto _LL133;} else{ goto _LL134;}} else{ goto
_LL134;} _LL134: if(( unsigned int) _temp128 > 1u?*(( int*) _temp128) == Cyc_Absyn_Abs_n:
0){ _LL141: _temp140=(( struct Cyc_Absyn_Abs_n_struct*) _temp128)->f1; goto
_LL135;} else{ goto _LL136;} _LL136: if(( unsigned int) _temp128 > 1u?*(( int*)
_temp128) == Cyc_Absyn_Rel_n: 0){ _LL143: _temp142=(( struct Cyc_Absyn_Rel_n_struct*)
_temp128)->f1; if( _temp142 == 0){ goto _LL129;} else{ _temp144=* _temp142;
_LL148: _temp147=( struct _tagged_string*) _temp144.hd; goto _LL146; _LL146:
_temp145= _temp144.tl; goto _LL137;}} else{ goto _LL129;} _LL131: goto _LL133;
_LL133: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(* lookup)( struct
Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp124); _LL135: { struct Cyc_Tcenv_Genv*
_temp149=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp140); goto _LL150; _LL150: return(( struct
Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( _temp149->uniondecls, _temp124);} _LL137: { struct Cyc_Tcenv_Genv*
_temp151= Cyc_Tcenv_lookup_namespace( te, loc, _temp147, _temp145); goto _LL152;
_LL152: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp151->uniondecls, _temp124);}
_LL129:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp155; struct _tagged_string* _temp156; void* _temp158; struct _tuple0*
_temp153= q; _temp155=* _temp153; _LL159: _temp158= _temp155.f1; goto _LL157;
_LL157: _temp156= _temp155.f2; goto _LL154; _LL154: { void* _temp160= _temp158;
struct Cyc_List_List* _temp170; struct Cyc_List_List* _temp172; struct Cyc_List_List
_temp174; struct Cyc_List_List* _temp175; struct _tagged_string* _temp177;
struct Cyc_List_List* _temp179; _LL162: if( _temp160 ==( void*) Cyc_Absyn_Loc_n){
goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int) _temp160 > 1u?*((
int*) _temp160) == Cyc_Absyn_Rel_n: 0){ _LL171: _temp170=(( struct Cyc_Absyn_Rel_n_struct*)
_temp160)->f1; if( _temp170 == 0){ goto _LL165;} else{ goto _LL166;}} else{ goto
_LL166;} _LL166: if(( unsigned int) _temp160 > 1u?*(( int*) _temp160) == Cyc_Absyn_Rel_n:
0){ _LL173: _temp172=(( struct Cyc_Absyn_Rel_n_struct*) _temp160)->f1; if(
_temp172 == 0){ goto _LL168;} else{ _temp174=* _temp172; _LL178: _temp177=(
struct _tagged_string*) _temp174.hd; goto _LL176; _LL176: _temp175= _temp174.tl;
goto _LL167;}} else{ goto _LL168;} _LL168: if(( unsigned int) _temp160 > 1u?*((
int*) _temp160) == Cyc_Absyn_Abs_n: 0){ _LL180: _temp179=(( struct Cyc_Absyn_Abs_n_struct*)
_temp160)->f1; goto _LL169;} else{ goto _LL161;} _LL163: goto _LL165; _LL165:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_tuniondecl_f, _temp156); _LL167: { struct Cyc_Tcenv_Genv*
_temp181= Cyc_Tcenv_lookup_namespace( te, loc, _temp177, _temp175); goto _LL182;
_LL182: return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp181->tuniondecls, _temp156);}
_LL169: { struct Cyc_Tcenv_Genv* _temp183=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp179);
goto _LL184; _LL184: return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp183->tuniondecls,
_temp156);} _LL161:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp187; struct _tagged_string* _temp188; void* _temp190; struct _tuple0*
_temp185= q; _temp187=* _temp185; _LL191: _temp190= _temp187.f1; goto _LL189;
_LL189: _temp188= _temp187.f2; goto _LL186; _LL186: { void* _temp192= _temp190;
struct Cyc_List_List* _temp202; struct Cyc_List_List* _temp204; struct Cyc_List_List
_temp206; struct Cyc_List_List* _temp207; struct _tagged_string* _temp209;
struct Cyc_List_List* _temp211; _LL194: if(( unsigned int) _temp192 > 1u?*(( int*)
_temp192) == Cyc_Absyn_Rel_n: 0){ _LL203: _temp202=(( struct Cyc_Absyn_Rel_n_struct*)
_temp192)->f1; if( _temp202 == 0){ goto _LL195;} else{ goto _LL196;}} else{ goto
_LL196;} _LL196: if( _temp192 ==( void*) Cyc_Absyn_Loc_n){ goto _LL197;} else{
goto _LL198;} _LL198: if(( unsigned int) _temp192 > 1u?*(( int*) _temp192) ==
Cyc_Absyn_Rel_n: 0){ _LL205: _temp204=(( struct Cyc_Absyn_Rel_n_struct*)
_temp192)->f1; if( _temp204 == 0){ goto _LL200;} else{ _temp206=* _temp204;
_LL210: _temp209=( struct _tagged_string*) _temp206.hd; goto _LL208; _LL208:
_temp207= _temp206.tl; goto _LL199;}} else{ goto _LL200;} _LL200: if((
unsigned int) _temp192 > 1u?*(( int*) _temp192) == Cyc_Absyn_Abs_n: 0){ _LL212:
_temp211=(( struct Cyc_Absyn_Abs_n_struct*) _temp192)->f1; goto _LL201;} else{
goto _LL193;} _LL195: { struct _handler_cons _temp213; _push_handler(& _temp213);{
int _temp215= 0; if( setjmp( _temp213.handler)){ _temp215= 1;} if( ! _temp215){{
struct Cyc_Core_Opt* _temp217=({ struct Cyc_Core_Opt* _temp216=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp216->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp188);
_temp216;}); _npop_handler( 0u); return _temp217;}; _pop_handler();} else{ void*
_temp214=( void*) _exn_thrown; void* _temp219= _temp214; _LL221: if( _temp219 ==
Cyc_Dict_Absent){ goto _LL222;} else{ goto _LL223;} _LL223: goto _LL224; _LL222:
return 0; _LL224:( void) _throw( _temp219); _LL220:;}}} _LL197: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp225=( unsigned char*)"lookup_xtuniondecl: impossible";
struct _tagged_string _temp226; _temp226.curr= _temp225; _temp226.base= _temp225;
_temp226.last_plus_one= _temp225 + 31; _temp226;})); return 0; _LL199: { struct
Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp227; _push_handler(& _temp227);{
int _temp229= 0; if( setjmp( _temp227.handler)){ _temp229= 1;} if( ! _temp229){
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp209, _temp207);; _pop_handler();}
else{ void* _temp228=( void*) _exn_thrown; void* _temp231= _temp228; _LL233: if(
_temp231 == Cyc_Dict_Absent){ goto _LL234;} else{ goto _LL235;} _LL235: goto
_LL236; _LL234: Cyc_Tcutil_terr( loc,({ unsigned char* _temp237=( unsigned char*)"bad qualified name for xtunion";
struct _tagged_string _temp238; _temp238.curr= _temp237; _temp238.base= _temp237;
_temp238.last_plus_one= _temp237 + 31; _temp238;}));( void) _throw(( void*) Cyc_Dict_Absent);
_LL236:( void) _throw( _temp231); _LL232:;}}} return({ struct Cyc_Core_Opt*
_temp239=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp239->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp188);
_temp239;});} _LL201: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp211); return({ struct Cyc_Core_Opt* _temp240=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp240->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp188); _temp240;});} _LL193:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp243; struct _tagged_string* _temp244; void* _temp246; struct _tuple0*
_temp241= q; _temp243=* _temp241; _LL247: _temp246= _temp243.f1; goto _LL245;
_LL245: _temp244= _temp243.f2; goto _LL242; _LL242: { void* _temp248= _temp246;
struct Cyc_List_List* _temp258; struct Cyc_List_List* _temp260; struct Cyc_List_List
_temp262; struct Cyc_List_List* _temp263; struct _tagged_string* _temp265;
struct Cyc_List_List* _temp267; _LL250: if( _temp248 ==( void*) Cyc_Absyn_Loc_n){
goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int) _temp248 > 1u?*((
int*) _temp248) == Cyc_Absyn_Rel_n: 0){ _LL259: _temp258=(( struct Cyc_Absyn_Rel_n_struct*)
_temp248)->f1; if( _temp258 == 0){ goto _LL253;} else{ goto _LL254;}} else{ goto
_LL254;} _LL254: if(( unsigned int) _temp248 > 1u?*(( int*) _temp248) == Cyc_Absyn_Rel_n:
0){ _LL261: _temp260=(( struct Cyc_Absyn_Rel_n_struct*) _temp248)->f1; if(
_temp260 == 0){ goto _LL256;} else{ _temp262=* _temp260; _LL266: _temp265=(
struct _tagged_string*) _temp262.hd; goto _LL264; _LL264: _temp263= _temp262.tl;
goto _LL255;}} else{ goto _LL256;} _LL256: if(( unsigned int) _temp248 > 1u?*((
int*) _temp248) == Cyc_Absyn_Abs_n: 0){ _LL268: _temp267=(( struct Cyc_Absyn_Abs_n_struct*)
_temp248)->f1; goto _LL257;} else{ goto _LL249;} _LL251: goto _LL253; _LL253:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_enumdecl_f, _temp244); _LL255: { struct Cyc_Tcenv_Genv*
_temp269= Cyc_Tcenv_lookup_namespace( te, loc, _temp265, _temp263); goto _LL270;
_LL270: return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp269->enumdecls, _temp244);} _LL257: {
struct Cyc_Tcenv_Genv* _temp271=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp267); goto _LL272;
_LL272: return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp271->enumdecls, _temp244);} _LL249:;}}
static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, v);}
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp275; struct _tagged_string* _temp276; void* _temp278; struct _tuple0*
_temp273= q; _temp275=* _temp273; _LL279: _temp278= _temp275.f1; goto _LL277;
_LL277: _temp276= _temp275.f2; goto _LL274; _LL274: { void* _temp280= _temp278;
struct Cyc_List_List* _temp290; struct Cyc_List_List* _temp292; struct Cyc_List_List
_temp294; struct Cyc_List_List* _temp295; struct _tagged_string* _temp297;
struct Cyc_List_List* _temp299; _LL282: if( _temp280 ==( void*) Cyc_Absyn_Loc_n){
goto _LL283;} else{ goto _LL284;} _LL284: if(( unsigned int) _temp280 > 1u?*((
int*) _temp280) == Cyc_Absyn_Rel_n: 0){ _LL291: _temp290=(( struct Cyc_Absyn_Rel_n_struct*)
_temp280)->f1; if( _temp290 == 0){ goto _LL285;} else{ goto _LL286;}} else{ goto
_LL286;} _LL286: if(( unsigned int) _temp280 > 1u?*(( int*) _temp280) == Cyc_Absyn_Rel_n:
0){ _LL293: _temp292=(( struct Cyc_Absyn_Rel_n_struct*) _temp280)->f1; if(
_temp292 == 0){ goto _LL288;} else{ _temp294=* _temp292; _LL298: _temp297=(
struct _tagged_string*) _temp294.hd; goto _LL296; _LL296: _temp295= _temp294.tl;
goto _LL287;}} else{ goto _LL288;} _LL288: if(( unsigned int) _temp280 > 1u?*((
int*) _temp280) == Cyc_Absyn_Abs_n: 0){ _LL300: _temp299=(( struct Cyc_Absyn_Abs_n_struct*)
_temp280)->f1; goto _LL289;} else{ goto _LL281;} _LL283: goto _LL285; _LL285:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_typedefdecl_f, _temp276); _LL287: { struct Cyc_Tcenv_Genv*
_temp301= Cyc_Tcenv_lookup_namespace( te, loc, _temp297, _temp295); goto _LL302;
_LL302: return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp301->typedefs, _temp276);}
_LL289: { struct Cyc_Tcenv_Genv* _temp303=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp299);
goto _LL304; _LL304: return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp303->typedefs, _temp276);}
_LL281:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp305=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp305->namespaces=( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Tcutil_empty_var_set))->v; _temp305->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp305->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp305->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp305->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp305->typedefs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp305->ordinaries=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
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
if( te->le == 0){( void) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( void) _throw((
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
po))->hd;}} return(( struct Cyc_Tcenv_RgnPO*(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ struct _tagged_string _temp376= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp374=( struct Cyc_Absyn_VarType_struct*)
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
result, _temp393, _temp397)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp407= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp403=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp403[ 0]=({ struct Cyc_Absyn_VarType_struct _temp404; _temp404.tag= Cyc_Absyn_VarType;
_temp404.f1= _temp397; _temp404;}); _temp403;})); struct _tagged_string _temp408=
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
goto _LL380; _LL386: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp414= Cyc_Absynpp_typ2string( _temp401); struct
_tagged_string _temp415= Cyc_Absynpp_typ2string( _temp399); xprintf("bad region lifetime assertion:: %.*s < %.*s",
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
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp421=( unsigned char*)"Tcenv: unexpected return_typ"; struct
_tagged_string _temp422; _temp422.curr= _temp421; _temp422.base= _temp421;
_temp422.last_plus_one= _temp421 + 29; _temp422;}));} return( void*)(( Cyc_Tcenv_get_fenv(
te))->shared)->return_typ;} struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv(
te))->type_vars;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp423=( unsigned char*)"Tcenv: unexpected add_type_vars"; struct
_tagged_string _temp424; _temp424.curr= _temp423; _temp424.base= _temp423;
_temp424.last_plus_one= _temp423 + 32; _temp424;}));}{ struct Cyc_Tcenv_Fenv*
_temp425= Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); goto _LL426;
_LL426: Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_List_List* _temp427=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs, _temp425->type_vars); goto _LL428; _LL428: Cyc_Tcutil_check_unique_tvars(
loc, _temp427); _temp425->type_vars= _temp427; return Cyc_Tcenv_put_fenv( te,
_temp425);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp429=( unsigned char*)"Tcenv: unexpected add_local_var"; struct
_tagged_string _temp430; _temp430.curr= _temp429; _temp430.base= _temp429;
_temp430.last_plus_one= _temp429 + 32; _temp430;}));}{ struct Cyc_Tcenv_Fenv*
_temp431= Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); goto _LL432;
_LL432: { struct _tagged_string* _temp433=(* vd->name).f2; goto _LL434; _LL434:{
void* _temp435=(* vd->name).f1; _LL437: if( _temp435 ==( void*) Cyc_Absyn_Loc_n){
goto _LL438;} else{ goto _LL439;} _LL439: goto _LL440; _LL438: goto _LL436;
_LL440:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp441=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp441[ 0]=({ struct Cyc_Core_Impossible_struct _temp442; _temp442.tag= Cyc_Core_Impossible;
_temp442.f1=({ unsigned char* _temp443=( unsigned char*)"add_local_var: called with Rel_n";
struct _tagged_string _temp444; _temp444.curr= _temp443; _temp444.base= _temp443;
_temp444.last_plus_one= _temp443 + 33; _temp444;}); _temp442;}); _temp441;}));
goto _LL436; _LL436:;} _temp431->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( _temp431->locals,
_temp433,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp445=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp445[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp446; _temp446.tag= Cyc_Absyn_Local_b; _temp446.f1= vd; _temp446;});
_temp445;})); return Cyc_Tcenv_put_fenv( te, _temp431);}}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp447=(
unsigned char*)"Tcenv: unexpected add_pat_var"; struct _tagged_string _temp448;
_temp448.curr= _temp447; _temp448.base= _temp447; _temp448.last_plus_one=
_temp447 + 30; _temp448;}));}{ struct Cyc_Tcenv_Fenv* _temp449= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); goto _LL450; _LL450: { struct _tagged_string* _temp451=(*
vd->name).f2; goto _LL452; _LL452: _temp449->locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
_temp449->locals, _temp451,( void*)({ struct Cyc_Absyn_Pat_b_struct* _temp453=(
struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp453[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp454; _temp454.tag= Cyc_Absyn_Pat_b;
_temp454.f1= vd; _temp454;}); _temp453;})); return Cyc_Tcenv_put_fenv( te,
_temp449);}}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp457; struct
_tagged_string* _temp458; void* _temp460; struct _tuple0* _temp455= q; _temp457=*
_temp455; _LL461: _temp460= _temp457.f1; goto _LL459; _LL459: _temp458= _temp457.f2;
goto _LL456; _LL456: { void* _temp462= _temp460; struct Cyc_List_List* _temp471;
_LL464: if( _temp462 ==( void*) Cyc_Absyn_Loc_n){ goto _LL465;} else{ goto
_LL466;} _LL466: if(( unsigned int) _temp462 > 1u?*(( int*) _temp462) == Cyc_Absyn_Rel_n:
0){ _LL472: _temp471=(( struct Cyc_Absyn_Rel_n_struct*) _temp462)->f1; if(
_temp471 == 0){ goto _LL470;} else{ goto _LL468;}} else{ goto _LL468;} _LL470:
if( te->le != 0){ goto _LL467;} else{ goto _LL468;} _LL468: goto _LL469; _LL465:
if( te->le == 0){( void) _throw(( void*) Cyc_Dict_Absent);} else{ goto _LL467;}
_LL467: { struct Cyc_Tcenv_Fenv* _temp473= Cyc_Tcenv_get_fenv( te); goto _LL474;
_LL474: { struct _handler_cons _temp475; _push_handler(& _temp475);{ int
_temp477= 0; if( setjmp( _temp475.handler)){ _temp477= 1;} if( ! _temp477){{
void* _temp480=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp478=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp478[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp479; _temp479.tag= Cyc_Tcenv_VarRes; _temp479.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp473->locals,
_temp458); _temp479;}); _temp478;}); _npop_handler( 0u); return _temp480;};
_pop_handler();} else{ void* _temp476=( void*) _exn_thrown; void* _temp482=
_temp476; _LL484: if( _temp482 == Cyc_Dict_Absent){ goto _LL485;} else{ goto
_LL486;} _LL486: goto _LL487; _LL485: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL487:( void) _throw( _temp482); _LL483:;}}}} _LL469: { struct
_handler_cons _temp488; _push_handler(& _temp488);{ int _temp490= 0; if( setjmp(
_temp488.handler)){ _temp490= 1;} if( ! _temp490){{ void* _temp491= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp491;}; _pop_handler();} else{ void*
_temp489=( void*) _exn_thrown; void* _temp493= _temp489; _LL495: if( _temp493 ==
Cyc_Dict_Absent){ goto _LL496;} else{ goto _LL497;} _LL497: goto _LL498; _LL496:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp499=
Cyc_Absynpp_qvar2string( q); xprintf("unbound identifer: %.*s", _temp499.last_plus_one
- _temp499.curr, _temp499.curr);})); return( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp500=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp500[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp501; _temp501.tag= Cyc_Tcenv_VarRes;
_temp501.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp501;}); _temp500;});
_LL498:( void) _throw( _temp493); _LL494:;}}} _LL463:;}} void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp502=( unsigned char*)"Tcenv: unexpected process_continue";
struct _tagged_string _temp503; _temp503.curr= _temp502; _temp503.base= _temp502;
_temp503.last_plus_one= _temp502 + 35; _temp503;}));}{ void* _temp504=( void*)((
Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt; struct Cyc_Absyn_Stmt*
_temp514; _LL506: if(( unsigned int) _temp504 > 3u?*(( int*) _temp504) == Cyc_Tcenv_Stmt_j:
0){ _LL515: _temp514=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp504)->f1; goto
_LL507;} else{ goto _LL508;} _LL508: if( _temp504 ==( void*) Cyc_Tcenv_NotLoop_j){
goto _LL509;} else{ goto _LL510;} _LL510: if( _temp504 ==( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL511;} else{ goto _LL512;} _LL512: if( _temp504 ==( void*) Cyc_Tcenv_FnEnd_j){
goto _LL513;} else{ goto _LL505;} _LL507: _temp514->non_local_preds=({ struct
Cyc_List_List* _temp516=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp516->hd=( void*) s; _temp516->tl= _temp514->non_local_preds; _temp516;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp514; return; _LL509: Cyc_Tcutil_terr( s->loc,({
unsigned char* _temp517=( unsigned char*)"continue not in a loop"; struct
_tagged_string _temp518; _temp518.curr= _temp517; _temp518.base= _temp517;
_temp518.last_plus_one= _temp517 + 23; _temp518;})); return; _LL511:(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp519=(
unsigned char*)"Tcenv: continue dest is CaseEnd"; struct _tagged_string _temp520;
_temp520.curr= _temp519; _temp520.base= _temp519; _temp520.last_plus_one=
_temp519 + 32; _temp520;})); return; _LL513:(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp521=( unsigned char*)"Tcenv: continue dest is CaseEnd";
struct _tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 32; _temp522;})); return; _LL505:;}} void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp523=( unsigned char*)"Tcenv: unexpected process_break";
struct _tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 32; _temp524;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); void* _temp525=( void*)( fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt* _temp535; _LL527: if(( unsigned int) _temp525 > 3u?*((
int*) _temp525) == Cyc_Tcenv_Stmt_j: 0){ _LL536: _temp535=(( struct Cyc_Tcenv_Stmt_j_struct*)
_temp525)->f1; goto _LL528;} else{ goto _LL529;} _LL529: if( _temp525 ==( void*)
Cyc_Tcenv_NotLoop_j){ goto _LL530;} else{ goto _LL531;} _LL531: if( _temp525 ==(
void*) Cyc_Tcenv_FnEnd_j){ goto _LL532;} else{ goto _LL533;} _LL533: if(
_temp525 ==( void*) Cyc_Tcenv_CaseEnd_j){ goto _LL534;} else{ goto _LL526;}
_LL528: _temp535->non_local_preds=({ struct Cyc_List_List* _temp537=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp537->hd=( void*) s; _temp537->tl=
_temp535->non_local_preds; _temp537;});* sopt=( struct Cyc_Absyn_Stmt*) _temp535;
return; _LL530: Cyc_Tcutil_terr( s->loc,({ unsigned char* _temp538=(
unsigned char*)"break not in a loop or switch"; struct _tagged_string _temp539;
_temp539.curr= _temp538; _temp539.base= _temp538; _temp539.last_plus_one=
_temp538 + 30; _temp539;})); return; _LL532: if(( void*)( fe->shared)->return_typ
!=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr( s->loc,({ unsigned char*
_temp540=( unsigned char*)"break causes function not to return a value"; struct
_tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 44; _temp541;}));} return; _LL534: Cyc_Tcutil_terr(
s->loc,({ unsigned char* _temp542=( unsigned char*)"break causes outer switch clause to implicitly fallthru";
struct _tagged_string _temp543; _temp543.curr= _temp542; _temp543.base= _temp542;
_temp543.last_plus_one= _temp542 + 56; _temp543;})); return; _LL526:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_string* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp544=(
unsigned char*)"Tcenv: unexpected process_goto"; struct _tagged_string _temp545;
_temp545.curr= _temp544; _temp545.base= _temp544; _temp545.last_plus_one=
_temp544 + 31; _temp545;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* _temp546=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels,
l); goto _LL547; _LL547: if( _temp546 == 0){ struct Cyc_SlowDict_Dict* _temp548=(
fe->shared)->needed_labels; goto _LL549; _LL549: { struct Cyc_Core_Opt* _temp550=((
struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string* key))
Cyc_SlowDict_lookup_opt)( _temp548, l); goto _LL551; _LL551: if( _temp550 == 0){
_temp550=({ struct Cyc_Core_Opt* _temp552=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp552->v=( void*) 0; _temp552;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( _temp548, l,({ struct
Cyc_List_List* _temp553=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp553->hd=( void*) s; _temp553->tl=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp550))->v; _temp553;}));}} else{(( struct Cyc_Absyn_Stmt*)((
struct Cyc_Core_Opt*) _check_null( _temp546))->v)->non_local_preds=({ struct Cyc_List_List*
_temp554=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp554->hd=( void*) s; _temp554->tl=(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*)
_check_null( _temp546))->v)->non_local_preds; _temp554;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*) _check_null( _temp546))->v);}}}
struct _tuple3* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause*** clauseopt){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp555=( unsigned char*)"Tcenv: unexpected process_fallthru"; struct
_tagged_string _temp556; _temp556.curr= _temp555; _temp556.base= _temp555;
_temp556.last_plus_one= _temp555 + 35; _temp556;}));}{ struct Cyc_Tcenv_Fenv*
_temp557= Cyc_Tcenv_get_fenv( te); goto _LL558; _LL558: { struct _tuple3*
_temp559=( _temp557->ctrl_env)->fallthru_clause; goto _LL560; _LL560: if(
_temp559 != 0){(((*(( struct _tuple3*) _check_null( _temp559))).f1)->body)->non_local_preds=({
struct Cyc_List_List* _temp561=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp561->hd=( void*) s; _temp561->tl=(((*(( struct
_tuple3*) _check_null( _temp559))).f1)->body)->non_local_preds; _temp561;});*
clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp562=( struct Cyc_Absyn_Switch_clause**)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) * 1); _temp562[ 0]=(*((
struct _tuple3*) _check_null( _temp559))).f1; _temp562;});} return _temp559;}}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause*
clause){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp563=( unsigned char*)"Tcenv: unexpected set_fallthru";
struct _tagged_string _temp564; _temp564.curr= _temp563; _temp564.base= _temp563;
_temp564.last_plus_one= _temp563 + 31; _temp564;}));}{ struct Cyc_List_List*
_temp565= 0; goto _LL566; _LL566: for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ _temp565=({ struct Cyc_List_List* _temp567=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp567->hd=( void*)(( void*)((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->type);
_temp567->tl= _temp565; _temp567;});} _temp565=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp565);{ struct Cyc_Tcenv_Fenv*
fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=({
struct _tuple3* _temp568=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp568->f1= clause; _temp568->f2= new_tvs; _temp568->f3= _temp565; _temp568;});
return Cyc_Tcenv_put_fenv( te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp569=( unsigned char*)"Tcenv: unexpected clear_fallthru";
struct _tagged_string _temp570; _temp570.curr= _temp569; _temp570.base= _temp569;
_temp570.last_plus_one= _temp569 + 33; _temp570;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=
0; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp571=( unsigned char*)"Tcenv: unexpected set_in_loop"; struct _tagged_string
_temp572; _temp572.curr= _temp571; _temp572.base= _temp571; _temp572.last_plus_one=
_temp571 + 30; _temp572;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp573=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp573[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp574; _temp574.tag= Cyc_Tcenv_Stmt_j; _temp574.f1= continue_dest; _temp574;});
_temp573;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp575=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp575[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp576; _temp576.tag= Cyc_Tcenv_Stmt_j;
_temp576.f1= continue_dest; _temp576;}); _temp575;}))); return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp577=( unsigned char*)"Tcenv: unexpected enter_try"; struct
_tagged_string _temp578; _temp578.curr= _temp577; _temp578.base= _temp577;
_temp578.last_plus_one= _temp577 + 28; _temp578;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te)); ++( fe->ctrl_env)->try_depth;
return Cyc_Tcenv_put_fenv( te, fe);}} int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp579=( unsigned char*)"Tcenv: unexpected get_try_depth";
struct _tagged_string _temp580; _temp580.curr= _temp579; _temp580.base= _temp579;
_temp580.last_plus_one= _temp579 + 32; _temp580;}));} return(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->try_depth;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp581=( unsigned char*)"Tcenv: unexpected set_in_switch";
struct _tagged_string _temp582; _temp582.curr= _temp581; _temp582.base= _temp581;
_temp582.last_plus_one= _temp581 + 32; _temp582;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)(( void*) Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp583=( unsigned char*)"Tcenv: unexpected set_next"; struct
_tagged_string _temp584; _temp584.curr= _temp583; _temp584.base= _temp583;
_temp584.last_plus_one= _temp583 + 27; _temp584;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=(
void*) j); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp585=( unsigned char*)"Tcenv: unexpected add_label"; struct
_tagged_string _temp586; _temp586.curr= _temp585; _temp586.base= _temp585;
_temp586.last_plus_one= _temp585 + 28; _temp586;}));}{ struct Cyc_Tcenv_Fenv*
_temp587= Cyc_Tcenv_get_fenv( te); goto _LL588; _LL588: { struct Cyc_SlowDict_Dict*
_temp589=( _temp587->shared)->needed_labels; goto _LL590; _LL590: { struct Cyc_Core_Opt*
_temp591=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict* d, struct
_tagged_string* key)) Cyc_SlowDict_lookup_opt)( _temp589, v); goto _LL592;
_LL592: if( _temp591 != 0){( _temp587->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_string* key)) Cyc_SlowDict_delete)(
_temp589, v);{ struct Cyc_List_List* _temp593=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp591))->v; goto _LL594; _LL594: s->non_local_preds=
_temp593; for( 0; _temp593 != 0; _temp593=(( struct Cyc_List_List*) _check_null(
_temp593))->tl){ void* _temp595=( void*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*)
_check_null( _temp593))->hd)->r; struct Cyc_Absyn_Stmt* _temp601; struct Cyc_Absyn_Stmt**
_temp603; struct _tagged_string* _temp604; _LL597: if(( unsigned int) _temp595 >
1u?*(( int*) _temp595) == Cyc_Absyn_Goto_s: 0){ _LL605: _temp604=(( struct Cyc_Absyn_Goto_s_struct*)
_temp595)->f1; goto _LL602; _LL602: _temp601=(( struct Cyc_Absyn_Goto_s_struct*)
_temp595)->f2; _temp603=&(( struct Cyc_Absyn_Goto_s_struct*) _temp595)->f2; goto
_LL598;} else{ goto _LL599;} _LL599: goto _LL600; _LL598:* _temp603=( struct Cyc_Absyn_Stmt*)
s; goto _LL596; _LL600:(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp606=( unsigned char*)"Tcenv: add_label backpatching of non-goto";
struct _tagged_string _temp607; _temp607.curr= _temp606; _temp607.base= _temp606;
_temp607.last_plus_one= _temp606 + 42; _temp607;})); goto _LL596; _LL596:;}}}
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)((
_temp587->shared)->seen_labels, v)){ Cyc_Tcutil_terr( s->loc,( struct
_tagged_string)({ struct _tagged_string _temp608=* v; xprintf("Repeated label: %.*s",
_temp608.last_plus_one - _temp608.curr, _temp608.curr);}));}( _temp587->shared)->seen_labels=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Stmt* data)) Cyc_Dict_insert)(( _temp587->shared)->seen_labels,
v, s); return te;}}}} int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*
te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp609=( unsigned char*)"Tcenv: unexpected get_encloser";
struct _tagged_string _temp610; _temp610.curr= _temp609; _temp610.base= _temp609;
_temp610.last_plus_one= _temp609 + 31; _temp610;}));} return( Cyc_Tcenv_get_fenv(
te))->encloser;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp611=(
unsigned char*)"Tcenv: unexpected set_encloser"; struct _tagged_string _temp612;
_temp612.curr= _temp611; _temp612.base= _temp611; _temp612.last_plus_one=
_temp611 + 31; _temp612;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp613=( unsigned char*)"Tcenv::add_region: unexpected add_region";
struct _tagged_string _temp614; _temp614.curr= _temp613; _temp614.base= _temp613;
_temp614.last_plus_one= _temp613 + 41; _temp614;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar*
tv;{ void* _temp615= Cyc_Tcutil_compress( r); struct Cyc_Absyn_Tvar* _temp621;
_LL617: if(( unsigned int) _temp615 > 4u?*(( int*) _temp615) == Cyc_Absyn_VarType:
0){ _LL622: _temp621=(( struct Cyc_Absyn_VarType_struct*) _temp615)->f1; goto
_LL618;} else{ goto _LL619;} _LL619: goto _LL620; _LL618: tv= _temp621; goto
_LL616; _LL620: tv=(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp623=( unsigned char*)"bad add region"; struct _tagged_string
_temp624; _temp624.curr= _temp623; _temp624.base= _temp623; _temp624.last_plus_one=
_temp623 + 15; _temp624;})); goto _LL616; _LL616:;} fe->region_order= Cyc_Tcenv_add_region_po(
fe->region_order, tv);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp625=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp625[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp626; _temp626.tag= Cyc_Absyn_JoinEff;
_temp626.f1=({ struct Cyc_List_List* _temp627=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp627->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp629=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp629[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp630; _temp630.tag= Cyc_Absyn_AccessEff;
_temp630.f1=( void*) r; _temp630;}); _temp629;})); _temp627->tl=({ struct Cyc_List_List*
_temp628=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp628->hd=( void*)(( void*) fe->capability); _temp628->tl= 0; _temp628;});
_temp627;}); _temp626;}); _temp625;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp631=( unsigned char*)"Tcenv: unexpected new_block"; struct _tagged_string
_temp632; _temp632.curr= _temp631; _temp632.base= _temp631; _temp632.last_plus_one=
_temp631 + 28; _temp632;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp640=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp640[ 0]=({ struct Cyc_Absyn_VarType_struct _temp641; _temp641.tag= Cyc_Absyn_VarType;
_temp641.f1= block_rgn; _temp641;}); _temp640;}); fe->type_vars=({ struct Cyc_List_List*
_temp633=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp633->hd=( void*) block_rgn; _temp633->tl= fe->type_vars; _temp633;}); Cyc_Tcutil_check_unique_tvars(
loc, fe->type_vars); fe->region_order= Cyc_Tcenv_add_region_po( fe->region_order,
block_rgn);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp634=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp634[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp635; _temp635.tag= Cyc_Absyn_JoinEff;
_temp635.f1=({ struct Cyc_List_List* _temp636=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp636->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp638=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp638[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp639; _temp639.tag= Cyc_Absyn_AccessEff;
_temp639.f1=( void*) block_typ; _temp639;}); _temp638;})); _temp636->tl=({
struct Cyc_List_List* _temp637=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp637->hd=( void*)(( void*) fe->capability); _temp637->tl=
0; _temp637;}); _temp636;}); _temp635;}); _temp634;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
struct Cyc_Absyn_Tvar* t= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); Cyc_Tcutil_add_tvar_identity(
t); return Cyc_Tcenv_new_named_block( loc, te, t);} void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return( void*) Cyc_Absyn_HeapRgn;}
return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;} void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* rgn){ if( te->le
!= 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv( te))->capability; if( Cyc_Tcutil_region_in_effect(
0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect( 1, rgn, capability)){
return;}} Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp642= Cyc_Absynpp_typ2string( rgn); xprintf("Expression accesses unavailable region %.*s",
_temp642.last_plus_one - _temp642.curr, _temp642.curr);}));} int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv* te, void* rt_a, void* rt_b){ if( rt_a ==( void*) Cyc_Absyn_HeapRgn){
return 1;} if( te->le != 0){ struct _tuple5 _temp644=({ struct _tuple5 _temp643;
_temp643.f1= Cyc_Tcutil_compress( rt_a); _temp643.f2= Cyc_Tcutil_compress( rt_b);
_temp643;}); void* _temp650; struct Cyc_Absyn_Tvar* _temp652; void* _temp654;
struct Cyc_Absyn_Tvar* _temp656; _LL646: _LL655: _temp654= _temp644.f1; if((
unsigned int) _temp654 > 4u?*(( int*) _temp654) == Cyc_Absyn_VarType: 0){ _LL657:
_temp656=(( struct Cyc_Absyn_VarType_struct*) _temp654)->f1; goto _LL651;} else{
goto _LL648;} _LL651: _temp650= _temp644.f2; if(( unsigned int) _temp650 > 4u?*((
int*) _temp650) == Cyc_Absyn_VarType: 0){ _LL653: _temp652=(( struct Cyc_Absyn_VarType_struct*)
_temp650)->f1; goto _LL647;} else{ goto _LL648;} _LL648: goto _LL649; _LL647:
return Cyc_Tcenv_check_region_outlives(( Cyc_Tcenv_get_fenv( te))->region_order,
_temp656, _temp652); _LL649: return 0; _LL645:;} return 0;} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp658=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp658[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp659; _temp659.tag= Cyc_Absyn_JoinEff;
_temp659.f1= 0; _temp659;}); _temp658;});} else{ struct Cyc_Tcenv_Fenv* _temp660=
Cyc_Tcenv_get_fenv( te); goto _LL661; _LL661: capability=( void*) _temp660->capability;}
if( ! Cyc_Tcutil_subset_effect( 1, eff, capability)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp662= Cyc_Absynpp_typ2string(
capability); struct _tagged_string _temp663= Cyc_Absynpp_typ2string( eff);
xprintf("Capability %.*s does not cover function's effect %.*s", _temp662.last_plus_one
- _temp662.curr, _temp662.curr, _temp663.last_plus_one - _temp663.curr, _temp663.curr);}));}}
void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* po){ for( 0; po != 0; po=(( struct Cyc_List_List*)
_check_null( po))->tl){ struct _tuple5 _temp666; void* _temp667; void* _temp669;
struct _tuple5* _temp664=( struct _tuple5*)(( struct Cyc_List_List*) _check_null(
po))->hd; _temp666=* _temp664; _LL670: _temp669= _temp666.f1; goto _LL668;
_LL668: _temp667= _temp666.f2; goto _LL665; _LL665: if( ! Cyc_Tcenv_region_outlives(
te, _temp669, _temp667)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp671= Cyc_Absynpp_typ2string( _temp669); struct
_tagged_string _temp672= Cyc_Absynpp_typ2string( _temp667); xprintf("function requires that region %.*s outlive %.*s",
_temp671.last_plus_one - _temp671.curr, _temp671.curr, _temp672.last_plus_one -
_temp672.curr, _temp672.curr);}));}}} struct _tuple6{ struct _tagged_string* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict*
locals=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar*
rgn0=({ struct Cyc_Absyn_Tvar* _temp734=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp734->name=({ struct _tagged_string*
_temp735=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp735[ 0]=( struct _tagged_string)({ struct _tagged_string _temp736=*(* fd->name).f2;
xprintf("`%.*s", _temp736.last_plus_one - _temp736.curr, _temp736.curr);});
_temp735;}); _temp734->identity= Cyc_Tcutil_new_tvar_id(); _temp734->kind=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind);
_temp734;}); struct Cyc_List_List* _temp674=({ struct Cyc_List_List* _temp673=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp673->hd=(
void*) rgn0; _temp673->tl= fd->tvs; _temp673;}); goto _LL675; _LL675: Cyc_Tcutil_check_unique_tvars(
loc, _temp674);{ struct Cyc_List_List* _temp676= Cyc_Tcenv_add_region_po( Cyc_Tcenv_initial_region_po(
loc, fd->tvs, fd->rgn_po), rgn0); goto _LL677; _LL677: { void* param_rgn=( void*)({
struct Cyc_Absyn_VarType_struct* _temp732=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp732[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp733; _temp733.tag= Cyc_Absyn_VarType; _temp733.f1= rgn0; _temp733;});
_temp732;}); struct Cyc_List_List* _temp678= 0; goto _LL679; _LL679:{ struct Cyc_List_List*
_temp680= fd->args; goto _LL681; _LL681: for( 0; _temp680 != 0; _temp680=((
struct Cyc_List_List*) _check_null( _temp680))->tl){ struct Cyc_Absyn_Vardecl*
_temp688=({ struct Cyc_Absyn_Vardecl* _temp682=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp682[ 0]=({ struct Cyc_Absyn_Vardecl
_temp683; _temp683.sc=( void*)(( void*) Cyc_Absyn_Public); _temp683.name=({
struct _tuple0* _temp686=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp686[ 0]=({ struct _tuple0 _temp687; _temp687.f1=( void*) Cyc_Absyn_Loc_n;
_temp687.f2=(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp680))->hd)).f1;
_temp687;}); _temp686;}); _temp683.tq=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( _temp680))->hd)).f2; _temp683.type=( void*)(*(( struct _tuple6*)((
struct Cyc_List_List*) _check_null( _temp680))->hd)).f3; _temp683.initializer= 0;
_temp683.rgn=({ struct Cyc_Core_Opt* _temp684=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp684[ 0]=({ struct Cyc_Core_Opt _temp685;
_temp685.v=( void*) param_rgn; _temp685;}); _temp684;}); _temp683.attributes= 0;
_temp683;}); _temp682;}); goto _LL689; _LL689: _temp678=({ struct Cyc_List_List*
_temp690=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp690[ 0]=({ struct Cyc_List_List _temp691; _temp691.hd=( void*) _temp688;
_temp691.tl= _temp678; _temp691;}); _temp690;}); locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
locals,(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp680))->hd)).f1,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp692=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp692[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp693; _temp693.tag= Cyc_Absyn_Param_b; _temp693.f1= _temp688; _temp693;});
_temp692;}));}} if( fd->cyc_varargs != 0){ int _temp696; void* _temp698; void*
_temp700; struct Cyc_Absyn_Tqual _temp702; struct Cyc_Core_Opt* _temp704; struct
Cyc_Absyn_VarargInfo _temp694=*(( struct Cyc_Absyn_VarargInfo*) _check_null( fd->cyc_varargs));
_LL705: _temp704= _temp694.name; goto _LL703; _LL703: _temp702= _temp694.tq;
goto _LL701; _LL701: _temp700=( void*) _temp694.type; goto _LL699; _LL699:
_temp698=( void*) _temp694.rgn; goto _LL697; _LL697: _temp696= _temp694.inject;
goto _LL695; _LL695: if( _temp704 != 0){ void* _temp706= Cyc_Absyn_tagged_typ(
_temp700, _temp698, _temp702); goto _LL707; _LL707: { struct Cyc_Absyn_Vardecl*
_temp714=({ struct Cyc_Absyn_Vardecl* _temp708=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp708[ 0]=({ struct Cyc_Absyn_Vardecl
_temp709; _temp709.sc=( void*)(( void*) Cyc_Absyn_Public); _temp709.name=({
struct _tuple0* _temp712=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp712[ 0]=({ struct _tuple0 _temp713; _temp713.f1=( void*) Cyc_Absyn_Loc_n;
_temp713.f2=( struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null(
_temp704))->v; _temp713;}); _temp712;}); _temp709.tq= Cyc_Absyn_empty_tqual();
_temp709.type=( void*) _temp706; _temp709.initializer= 0; _temp709.rgn=({ struct
Cyc_Core_Opt* _temp710=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)
* 1); _temp710[ 0]=({ struct Cyc_Core_Opt _temp711; _temp711.v=( void*)
param_rgn; _temp711;}); _temp710;}); _temp709.attributes= 0; _temp709;});
_temp708;}); goto _LL715; _LL715: _temp678=({ struct Cyc_List_List* _temp716=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp716->hd=(
void*) _temp714; _temp716->tl= _temp678; _temp716;}); locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
locals,( struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp704))->v,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp717=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp717[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp718; _temp718.tag= Cyc_Absyn_Param_b; _temp718.f1= _temp714; _temp718;});
_temp717;}));}} else{ Cyc_Tcutil_terr( loc,({ unsigned char* _temp719=(
unsigned char*)"missing name for varargs"; struct _tagged_string _temp720;
_temp720.curr= _temp719; _temp720.base= _temp719; _temp720.last_plus_one=
_temp719 + 25; _temp720;}));}} fd->param_vardecls=({ struct Cyc_Core_Opt*
_temp721=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp721[ 0]=({ struct Cyc_Core_Opt _temp722; _temp722.v=( void*) _temp678;
_temp722;}); _temp721;}); return({ struct Cyc_Tcenv_Fenv* _temp723=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp723->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp731=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp731->return_typ=( void*)(( void*) fd->ret_type); _temp731->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp731->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp); _temp731;});
_temp723->type_vars= _temp674; _temp723->region_order= _temp676; _temp723->locals=
locals; _temp723->encloser= fd->body; _temp723->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv*
_temp730=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv));
_temp730->continue_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j); _temp730->break_stmt=(
void*)(( void*) Cyc_Tcenv_NotLoop_j); _temp730->fallthru_clause= 0; _temp730->next_stmt=(
void*)(( void*) Cyc_Tcenv_FnEnd_j); _temp730->try_depth= 0; _temp730;});
_temp723->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp724=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp724[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp725; _temp725.tag= Cyc_Absyn_JoinEff;
_temp725.f1=({ struct Cyc_List_List* _temp726=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp726->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp728=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp728[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp729; _temp729.tag= Cyc_Absyn_AccessEff;
_temp729.f1=( void*) param_rgn; _temp729;}); _temp728;})); _temp726->tl=({
struct Cyc_List_List* _temp727=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp727->hd=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( fd->effect))->v); _temp727->tl= 0; _temp727;}); _temp726;});
_temp725;}); _temp724;})); _temp723->curr_rgn=( void*) param_rgn; _temp723;});}}}