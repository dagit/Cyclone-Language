 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
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
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const
int Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e=
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
Cyc_Absyn_exn_tud; extern struct _tuple0* Cyc_Absyn_null_pointer_exn_name;
extern struct _tuple0* Cyc_Absyn_match_exn_name; extern struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_null_pointer_exn_tuf; extern struct Cyc_Absyn_Tunionfield* Cyc_Absyn_match_exn_tuf;
extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq);
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*);
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
ctrl_env; void* capability; void* curr_rgn; } ; struct _tuple4{ void* f1; int f2;
} ; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({
struct Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp1;});{ struct Cyc_Tcenv_Genv* _temp2= Cyc_Tcenv_empty_genv(); goto _LL3;
_LL3: { struct Cyc_List_List* _temp4= 0; goto _LL5; _LL5: _temp2->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( _temp2->tuniondecls,(*
Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_tud); _temp2->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple4* data)) Cyc_Dict_insert)(
_temp2->ordinaries,(* Cyc_Absyn_null_pointer_exn_name).f2,({ struct _tuple4*
_temp6=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp6->f1=( void*)({
struct Cyc_Tcenv_TunionRes_struct* _temp7=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp7[ 0]=({ struct Cyc_Tcenv_TunionRes_struct
_temp8; _temp8.tag= Cyc_Tcenv_TunionRes; _temp8.f1= Cyc_Absyn_exn_tud; _temp8.f2=
Cyc_Absyn_null_pointer_exn_tuf; _temp8;}); _temp7;}); _temp6->f2= 1; _temp6;}));
_temp2->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct _tuple4* data)) Cyc_Dict_insert)( _temp2->ordinaries,(*
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
loc, struct _tagged_string* n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
Cyc_Tcenv_resolve_namespace( te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns == 0){ return(( struct Cyc_List_List*(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp15=(
unsigned char*)"Tcenv::outer_namespace"; struct _tagged_string _temp16; _temp16.curr=
_temp15; _temp16.base= _temp15; _temp16.last_plus_one= _temp15 + 23; _temp16;}));}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( ns)))->tl);} static void Cyc_Tcenv_check_repeat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss){
for( 0; nss != 0; nss=(( struct Cyc_List_List*) _check_null( nss))->tl){ struct
Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( nss))->hd); struct _handler_cons _temp17;
_push_handler(& _temp17);{ int _temp19= 0; if( setjmp( _temp17.handler)){
_temp19= 1;} if( ! _temp19){ lookup( ge2, v); Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp20=* v; xprintf("%.*s is ambiguous",
_temp20.last_plus_one - _temp20.curr, _temp20.curr);}));; _pop_handler();} else{
void* _temp18=( void*) _exn_thrown; void* _temp22= _temp18; _LL24: if( _temp22
== Cyc_Dict_Absent){ goto _LL25;} else{ goto _LL26;} _LL26: goto _LL27; _LL25:
goto _LL23; _LL27:( void) _throw( _temp22); _LL23:;}}} return;} static void* Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v){
struct Cyc_List_List* cns= te->ns; void* result; while( 1) { struct Cyc_Tcenv_Genv*
ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, cns);{ struct _handler_cons _temp28;
_push_handler(& _temp28);{ int _temp30= 0; if( setjmp( _temp28.handler)){
_temp30= 1;} if( ! _temp30){ result= lookup( ge, v);(( void(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss))
Cyc_Tcenv_check_repeat)( te, loc, lookup, v, ge->availables);{ void* _temp31=
result; _npop_handler( 0u); return _temp31;}; _pop_handler();} else{ void*
_temp29=( void*) _exn_thrown; void* _temp33= _temp29; _LL35: if( _temp33 == Cyc_Dict_Absent){
goto _LL36;} else{ goto _LL37;} _LL37: goto _LL38; _LL36: goto _LL34; _LL38:(
void) _throw( _temp33); _LL34:;}}}{ struct Cyc_List_List* _temp39= ge->availables;
goto _LL40; _LL40: for( 0; _temp39 != 0; _temp39=(( struct Cyc_List_List*)
_check_null( _temp39))->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp39))->hd);
struct _handler_cons _temp41; _push_handler(& _temp41);{ int _temp43= 0; if(
setjmp( _temp41.handler)){ _temp43= 1;} if( ! _temp43){ result= lookup( ge2, v);((
void(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)( te, loc, lookup, v,((
struct Cyc_List_List*) _check_null( _temp39))->tl);{ void* _temp44= result;
_npop_handler( 0u); return _temp44;}; _pop_handler();} else{ void* _temp42=(
void*) _exn_thrown; void* _temp46= _temp42; _LL48: if( _temp46 == Cyc_Dict_Absent){
goto _LL49;} else{ goto _LL50;} _LL50: goto _LL51; _LL49: goto _LL47; _LL51:(
void) _throw( _temp46); _LL47:;}}}} if( cns == 0){( void) _throw(( void*) Cyc_Dict_Absent);}
cns= Cyc_Tcenv_outer_namespace( cns);}} static void* Cyc_Tcenv_lookup_ordinary_global_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ struct _tuple4* ans=((
struct _tuple4*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
ge->ordinaries, v);(* ans).f2= 1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp54; struct _tagged_string* _temp55; void* _temp57; struct
_tuple0* _temp52= q; _temp54=* _temp52; _LL58: _temp57= _temp54.f1; goto _LL56;
_LL56: _temp55= _temp54.f2; goto _LL53; _LL53: { void* _temp59= _temp57; struct
Cyc_List_List* _temp69; struct Cyc_List_List* _temp71; struct Cyc_List_List
_temp73; struct Cyc_List_List* _temp74; struct _tagged_string* _temp76; struct
Cyc_List_List* _temp78; _LL61: if( _temp59 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL62;} else{ goto _LL63;} _LL63: if(( unsigned int) _temp59 > 1u?*(( int*)
_temp59) == Cyc_Absyn_Rel_n: 0){ _LL70: _temp69=(( struct Cyc_Absyn_Rel_n_struct*)
_temp59)->f1; if( _temp69 == 0){ goto _LL64;} else{ goto _LL65;}} else{ goto
_LL65;} _LL65: if(( unsigned int) _temp59 > 1u?*(( int*) _temp59) == Cyc_Absyn_Rel_n:
0){ _LL72: _temp71=(( struct Cyc_Absyn_Rel_n_struct*) _temp59)->f1; if( _temp71
== 0){ goto _LL67;} else{ _temp73=* _temp71; _LL77: _temp76=( struct
_tagged_string*) _temp73.hd; goto _LL75; _LL75: _temp74= _temp73.tl; goto _LL66;}}
else{ goto _LL67;} _LL67: if(( unsigned int) _temp59 > 1u?*(( int*) _temp59) ==
Cyc_Absyn_Abs_n: 0){ _LL79: _temp78=(( struct Cyc_Absyn_Abs_n_struct*) _temp59)->f1;
goto _LL68;} else{ goto _LL60;} _LL62: goto _LL64; _LL64: return(( void*(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_ordinary_global_f, _temp55); _LL66: { struct Cyc_Tcenv_Genv*
_temp80= Cyc_Tcenv_lookup_namespace( te, loc, _temp76, _temp74); goto _LL81;
_LL81: return Cyc_Tcenv_lookup_ordinary_global_f( _temp80, _temp55);} _LL68:
return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp78),
_temp55); _LL60:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_string* n,
struct Cyc_List_List* ns){ struct Cyc_List_List* _temp82= te->ns; goto _LL83;
_LL83: { struct Cyc_List_List* _temp84= 0; goto _LL85; _LL85: while( 1) { struct
Cyc_Tcenv_Genv* _temp86=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp82); goto _LL87;
_LL87:{ struct Cyc_List_List* _temp88= _temp86->availables; goto _LL89; _LL89:
for( 0; _temp88 != 0; _temp88=(( struct Cyc_List_List*) _check_null( _temp88))->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( _temp88))->hd); if((( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( ge2->namespaces, n)){ _temp84=({
struct Cyc_List_List* _temp90=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp90->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp82,({ struct Cyc_List_List*
_temp91=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp91->hd=( void*) n; _temp91->tl= ns; _temp91;})); _temp90->tl= _temp84;
_temp90;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt))
Cyc_Set_member)( _temp86->namespaces, n)){ _temp84=({ struct Cyc_List_List*
_temp92=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp92->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp82,({ struct Cyc_List_List* _temp93=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp93->hd=(
void*) n; _temp93->tl= ns; _temp93;})); _temp92->tl= _temp84; _temp92;});} if(
_temp84 != 0){ if((( struct Cyc_List_List*) _check_null( _temp84))->tl != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp94=*
n; xprintf("%.*s is ambiguous", _temp94.last_plus_one - _temp94.curr, _temp94.curr);}));}
return( struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp84))->hd;}
if( _temp82 == 0){( void) _throw(( void*) Cyc_Dict_Absent);} _temp82= Cyc_Tcenv_outer_namespace(
_temp82);}}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp97;
struct _tagged_string* _temp98; void* _temp100; struct _tuple0* _temp95= q;
_temp97=* _temp95; _LL101: _temp100= _temp97.f1; goto _LL99; _LL99: _temp98=
_temp97.f2; goto _LL96; _LL96: { void* _temp102= _temp100; struct Cyc_List_List*
_temp112; struct Cyc_List_List* _temp114; struct Cyc_List_List* _temp116; struct
Cyc_List_List _temp118; struct Cyc_List_List* _temp119; struct _tagged_string*
_temp121; _LL104: if( _temp102 ==( void*) Cyc_Absyn_Loc_n){ goto _LL105;} else{
goto _LL106;} _LL106: if(( unsigned int) _temp102 > 1u?*(( int*) _temp102) ==
Cyc_Absyn_Rel_n: 0){ _LL113: _temp112=(( struct Cyc_Absyn_Rel_n_struct*)
_temp102)->f1; if( _temp112 == 0){ goto _LL107;} else{ goto _LL108;}} else{ goto
_LL108;} _LL108: if(( unsigned int) _temp102 > 1u?*(( int*) _temp102) == Cyc_Absyn_Abs_n:
0){ _LL115: _temp114=(( struct Cyc_Absyn_Abs_n_struct*) _temp102)->f1; goto
_LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp102 > 1u?*(( int*)
_temp102) == Cyc_Absyn_Rel_n: 0){ _LL117: _temp116=(( struct Cyc_Absyn_Rel_n_struct*)
_temp102)->f1; if( _temp116 == 0){ goto _LL103;} else{ _temp118=* _temp116;
_LL122: _temp121=( struct _tagged_string*) _temp118.hd; goto _LL120; _LL120:
_temp119= _temp118.tl; goto _LL111;}} else{ goto _LL103;} _LL105: goto _LL107;
_LL107: return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp98); _LL109: { struct Cyc_Tcenv_Genv*
_temp123=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp114); goto _LL124; _LL124: return(( struct
Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( _temp123->structdecls, _temp98);} _LL111: { struct Cyc_Tcenv_Genv*
_temp125= Cyc_Tcenv_lookup_namespace( te, loc, _temp121, _temp119); goto _LL126;
_LL126: return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp125->structdecls, _temp98);}
_LL103:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp129; struct _tagged_string* _temp130; void* _temp132; struct _tuple0*
_temp127= q; _temp129=* _temp127; _LL133: _temp132= _temp129.f1; goto _LL131;
_LL131: _temp130= _temp129.f2; goto _LL128; _LL128: { void* _temp134= _temp132;
struct Cyc_List_List* _temp144; struct Cyc_List_List* _temp146; struct Cyc_List_List*
_temp148; struct Cyc_List_List _temp150; struct Cyc_List_List* _temp151; struct
_tagged_string* _temp153; _LL136: if( _temp134 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL137;} else{ goto _LL138;} _LL138: if(( unsigned int) _temp134 > 1u?*(( int*)
_temp134) == Cyc_Absyn_Rel_n: 0){ _LL145: _temp144=(( struct Cyc_Absyn_Rel_n_struct*)
_temp134)->f1; if( _temp144 == 0){ goto _LL139;} else{ goto _LL140;}} else{ goto
_LL140;} _LL140: if(( unsigned int) _temp134 > 1u?*(( int*) _temp134) == Cyc_Absyn_Abs_n:
0){ _LL147: _temp146=(( struct Cyc_Absyn_Abs_n_struct*) _temp134)->f1; goto
_LL141;} else{ goto _LL142;} _LL142: if(( unsigned int) _temp134 > 1u?*(( int*)
_temp134) == Cyc_Absyn_Rel_n: 0){ _LL149: _temp148=(( struct Cyc_Absyn_Rel_n_struct*)
_temp134)->f1; if( _temp148 == 0){ goto _LL135;} else{ _temp150=* _temp148;
_LL154: _temp153=( struct _tagged_string*) _temp150.hd; goto _LL152; _LL152:
_temp151= _temp150.tl; goto _LL143;}} else{ goto _LL135;} _LL137: goto _LL139;
_LL139: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(* lookup)( struct
Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp130); _LL141: { struct Cyc_Tcenv_Genv*
_temp155=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp146); goto _LL156; _LL156: return(( struct
Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( _temp155->uniondecls, _temp130);} _LL143: { struct Cyc_Tcenv_Genv*
_temp157= Cyc_Tcenv_lookup_namespace( te, loc, _temp153, _temp151); goto _LL158;
_LL158: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp157->uniondecls, _temp130);}
_LL135:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp161; struct _tagged_string* _temp162; void* _temp164; struct _tuple0*
_temp159= q; _temp161=* _temp159; _LL165: _temp164= _temp161.f1; goto _LL163;
_LL163: _temp162= _temp161.f2; goto _LL160; _LL160: { void* _temp166= _temp164;
struct Cyc_List_List* _temp176; struct Cyc_List_List* _temp178; struct Cyc_List_List
_temp180; struct Cyc_List_List* _temp181; struct _tagged_string* _temp183;
struct Cyc_List_List* _temp185; _LL168: if( _temp166 ==( void*) Cyc_Absyn_Loc_n){
goto _LL169;} else{ goto _LL170;} _LL170: if(( unsigned int) _temp166 > 1u?*((
int*) _temp166) == Cyc_Absyn_Rel_n: 0){ _LL177: _temp176=(( struct Cyc_Absyn_Rel_n_struct*)
_temp166)->f1; if( _temp176 == 0){ goto _LL171;} else{ goto _LL172;}} else{ goto
_LL172;} _LL172: if(( unsigned int) _temp166 > 1u?*(( int*) _temp166) == Cyc_Absyn_Rel_n:
0){ _LL179: _temp178=(( struct Cyc_Absyn_Rel_n_struct*) _temp166)->f1; if(
_temp178 == 0){ goto _LL174;} else{ _temp180=* _temp178; _LL184: _temp183=(
struct _tagged_string*) _temp180.hd; goto _LL182; _LL182: _temp181= _temp180.tl;
goto _LL173;}} else{ goto _LL174;} _LL174: if(( unsigned int) _temp166 > 1u?*((
int*) _temp166) == Cyc_Absyn_Abs_n: 0){ _LL186: _temp185=(( struct Cyc_Absyn_Abs_n_struct*)
_temp166)->f1; goto _LL175;} else{ goto _LL167;} _LL169: goto _LL171; _LL171:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_tuniondecl_f, _temp162); _LL173: { struct Cyc_Tcenv_Genv*
_temp187= Cyc_Tcenv_lookup_namespace( te, loc, _temp183, _temp181); goto _LL188;
_LL188: return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp187->tuniondecls, _temp162);}
_LL175: { struct Cyc_Tcenv_Genv* _temp189=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp185);
goto _LL190; _LL190: return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp189->tuniondecls,
_temp162);} _LL167:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp193; struct _tagged_string* _temp194; void* _temp196; struct _tuple0*
_temp191= q; _temp193=* _temp191; _LL197: _temp196= _temp193.f1; goto _LL195;
_LL195: _temp194= _temp193.f2; goto _LL192; _LL192: { void* _temp198= _temp196;
struct Cyc_List_List* _temp208; struct Cyc_List_List* _temp210; struct Cyc_List_List
_temp212; struct Cyc_List_List* _temp213; struct _tagged_string* _temp215;
struct Cyc_List_List* _temp217; _LL200: if(( unsigned int) _temp198 > 1u?*(( int*)
_temp198) == Cyc_Absyn_Rel_n: 0){ _LL209: _temp208=(( struct Cyc_Absyn_Rel_n_struct*)
_temp198)->f1; if( _temp208 == 0){ goto _LL201;} else{ goto _LL202;}} else{ goto
_LL202;} _LL202: if( _temp198 ==( void*) Cyc_Absyn_Loc_n){ goto _LL203;} else{
goto _LL204;} _LL204: if(( unsigned int) _temp198 > 1u?*(( int*) _temp198) ==
Cyc_Absyn_Rel_n: 0){ _LL211: _temp210=(( struct Cyc_Absyn_Rel_n_struct*)
_temp198)->f1; if( _temp210 == 0){ goto _LL206;} else{ _temp212=* _temp210;
_LL216: _temp215=( struct _tagged_string*) _temp212.hd; goto _LL214; _LL214:
_temp213= _temp212.tl; goto _LL205;}} else{ goto _LL206;} _LL206: if((
unsigned int) _temp198 > 1u?*(( int*) _temp198) == Cyc_Absyn_Abs_n: 0){ _LL218:
_temp217=(( struct Cyc_Absyn_Abs_n_struct*) _temp198)->f1; goto _LL207;} else{
goto _LL199;} _LL201: { struct _handler_cons _temp219; _push_handler(& _temp219);{
int _temp221= 0; if( setjmp( _temp219.handler)){ _temp221= 1;} if( ! _temp221){{
struct Cyc_Core_Opt* _temp223=({ struct Cyc_Core_Opt* _temp222=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp222->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp194);
_temp222;}); _npop_handler( 0u); return _temp223;}; _pop_handler();} else{ void*
_temp220=( void*) _exn_thrown; void* _temp225= _temp220; _LL227: if( _temp225 ==
Cyc_Dict_Absent){ goto _LL228;} else{ goto _LL229;} _LL229: goto _LL230; _LL228:
return 0; _LL230:( void) _throw( _temp225); _LL226:;}}} _LL203: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp231=( unsigned char*)"lookup_xtuniondecl: impossible";
struct _tagged_string _temp232; _temp232.curr= _temp231; _temp232.base= _temp231;
_temp232.last_plus_one= _temp231 + 31; _temp232;})); return 0; _LL205: { struct
Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp233; _push_handler(& _temp233);{
int _temp235= 0; if( setjmp( _temp233.handler)){ _temp235= 1;} if( ! _temp235){
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp215, _temp213);; _pop_handler();}
else{ void* _temp234=( void*) _exn_thrown; void* _temp237= _temp234; _LL239: if(
_temp237 == Cyc_Dict_Absent){ goto _LL240;} else{ goto _LL241;} _LL241: goto
_LL242; _LL240: Cyc_Tcutil_terr( loc,({ unsigned char* _temp243=( unsigned char*)"bad qualified name for xtunion";
struct _tagged_string _temp244; _temp244.curr= _temp243; _temp244.base= _temp243;
_temp244.last_plus_one= _temp243 + 31; _temp244;}));( void) _throw(( void*) Cyc_Dict_Absent);
_LL242:( void) _throw( _temp237); _LL238:;}}} return({ struct Cyc_Core_Opt*
_temp245=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp245->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp194);
_temp245;});} _LL207: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp217); return({ struct Cyc_Core_Opt* _temp246=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp246->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp194); _temp246;});} _LL199:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp249; struct _tagged_string* _temp250; void* _temp252; struct _tuple0*
_temp247= q; _temp249=* _temp247; _LL253: _temp252= _temp249.f1; goto _LL251;
_LL251: _temp250= _temp249.f2; goto _LL248; _LL248: { void* _temp254= _temp252;
struct Cyc_List_List* _temp264; struct Cyc_List_List* _temp266; struct Cyc_List_List
_temp268; struct Cyc_List_List* _temp269; struct _tagged_string* _temp271;
struct Cyc_List_List* _temp273; _LL256: if( _temp254 ==( void*) Cyc_Absyn_Loc_n){
goto _LL257;} else{ goto _LL258;} _LL258: if(( unsigned int) _temp254 > 1u?*((
int*) _temp254) == Cyc_Absyn_Rel_n: 0){ _LL265: _temp264=(( struct Cyc_Absyn_Rel_n_struct*)
_temp254)->f1; if( _temp264 == 0){ goto _LL259;} else{ goto _LL260;}} else{ goto
_LL260;} _LL260: if(( unsigned int) _temp254 > 1u?*(( int*) _temp254) == Cyc_Absyn_Rel_n:
0){ _LL267: _temp266=(( struct Cyc_Absyn_Rel_n_struct*) _temp254)->f1; if(
_temp266 == 0){ goto _LL262;} else{ _temp268=* _temp266; _LL272: _temp271=(
struct _tagged_string*) _temp268.hd; goto _LL270; _LL270: _temp269= _temp268.tl;
goto _LL261;}} else{ goto _LL262;} _LL262: if(( unsigned int) _temp254 > 1u?*((
int*) _temp254) == Cyc_Absyn_Abs_n: 0){ _LL274: _temp273=(( struct Cyc_Absyn_Abs_n_struct*)
_temp254)->f1; goto _LL263;} else{ goto _LL255;} _LL257: goto _LL259; _LL259:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_enumdecl_f, _temp250); _LL261: { struct Cyc_Tcenv_Genv*
_temp275= Cyc_Tcenv_lookup_namespace( te, loc, _temp271, _temp269); goto _LL276;
_LL276: return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp275->enumdecls, _temp250);} _LL263: {
struct Cyc_Tcenv_Genv* _temp277=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp273); goto _LL278;
_LL278: return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp277->enumdecls, _temp250);} _LL255:;}}
static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, v);}
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp281; struct _tagged_string* _temp282; void* _temp284; struct _tuple0*
_temp279= q; _temp281=* _temp279; _LL285: _temp284= _temp281.f1; goto _LL283;
_LL283: _temp282= _temp281.f2; goto _LL280; _LL280: { void* _temp286= _temp284;
struct Cyc_List_List* _temp296; struct Cyc_List_List* _temp298; struct Cyc_List_List
_temp300; struct Cyc_List_List* _temp301; struct _tagged_string* _temp303;
struct Cyc_List_List* _temp305; _LL288: if( _temp286 ==( void*) Cyc_Absyn_Loc_n){
goto _LL289;} else{ goto _LL290;} _LL290: if(( unsigned int) _temp286 > 1u?*((
int*) _temp286) == Cyc_Absyn_Rel_n: 0){ _LL297: _temp296=(( struct Cyc_Absyn_Rel_n_struct*)
_temp286)->f1; if( _temp296 == 0){ goto _LL291;} else{ goto _LL292;}} else{ goto
_LL292;} _LL292: if(( unsigned int) _temp286 > 1u?*(( int*) _temp286) == Cyc_Absyn_Rel_n:
0){ _LL299: _temp298=(( struct Cyc_Absyn_Rel_n_struct*) _temp286)->f1; if(
_temp298 == 0){ goto _LL294;} else{ _temp300=* _temp298; _LL304: _temp303=(
struct _tagged_string*) _temp300.hd; goto _LL302; _LL302: _temp301= _temp300.tl;
goto _LL293;}} else{ goto _LL294;} _LL294: if(( unsigned int) _temp286 > 1u?*((
int*) _temp286) == Cyc_Absyn_Abs_n: 0){ _LL306: _temp305=(( struct Cyc_Absyn_Abs_n_struct*)
_temp286)->f1; goto _LL295;} else{ goto _LL287;} _LL289: goto _LL291; _LL291:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_typedefdecl_f, _temp282); _LL293: { struct Cyc_Tcenv_Genv*
_temp307= Cyc_Tcenv_lookup_namespace( te, loc, _temp303, _temp301); goto _LL308;
_LL308: return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp307->typedefs, _temp282);}
_LL295: { struct Cyc_Tcenv_Genv* _temp309=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp305);
goto _LL310; _LL310: return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp309->typedefs, _temp282);}
_LL287:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp311=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp311->namespaces=( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Tcutil_empty_var_set))->v; _temp311->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp311->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp311->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp311->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp311->typedefs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp311->ordinaries=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp311->availables= 0; _temp311;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp312= f; void* _temp320; void* _temp322; void* _temp324;
void* _temp326; void* _temp328; _LL314: if(*(( int*) _temp312) == Cyc_Tcenv_Outermost){
_LL321: _temp320=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp312)->f1;
goto _LL315;} else{ goto _LL316;} _LL316: if(*(( int*) _temp312) == Cyc_Tcenv_Frame){
_LL325: _temp324=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp312)->f1; goto
_LL323; _LL323: _temp322=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp312)->f2;
goto _LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp312) == Cyc_Tcenv_Hidden){
_LL329: _temp328=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp312)->f1; goto
_LL327; _LL327: _temp326=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp312)->f2;
goto _LL319;} else{ goto _LL313;} _LL315: return _temp320; _LL317: return
_temp324; _LL319: f= _temp326; goto _LL313; _LL313:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp330= f; void* _temp338; void* _temp340; void*
_temp342; void* _temp344; void* _temp346; _LL332: if(*(( int*) _temp330) == Cyc_Tcenv_Outermost){
_LL339: _temp338=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp330)->f1;
goto _LL333;} else{ goto _LL334;} _LL334: if(*(( int*) _temp330) == Cyc_Tcenv_Frame){
_LL343: _temp342=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp330)->f1; goto
_LL341; _LL341: _temp340=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp330)->f2;
goto _LL335;} else{ goto _LL336;} _LL336: if(*(( int*) _temp330) == Cyc_Tcenv_Hidden){
_LL347: _temp346=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp330)->f1; goto
_LL345; _LL345: _temp344=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp330)->f2;
goto _LL337;} else{ goto _LL331;} _LL333: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp348=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp348[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp349; _temp349.tag= Cyc_Tcenv_Outermost;
_temp349.f1=( void*) x; _temp349;}); _temp348;}); _LL335: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp350=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp350[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp351; _temp351.tag= Cyc_Tcenv_Frame; _temp351.f1=( void*) x; _temp351.f2=(
void*) _temp340; _temp351;}); _temp350;}); _LL337: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp352=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp352[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp353; _temp353.tag= Cyc_Tcenv_Hidden;
_temp353.f1=( void*) _temp346; _temp353.f2=( void*)(( void*(*)( void* f, void* x))
Cyc_Tcenv_put_visible)( _temp344, x); _temp353;}); _temp352;}); _LL331:;} void*
Cyc_Tcenv_apply_to_visible( void*(* g)( void*), void* f){ return(( void*(*)(
void* f, void* x)) Cyc_Tcenv_put_visible)( f, g((( void*(*)( void* f)) Cyc_Tcenv_get_visible)(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp354=
f; void* _temp362; void* _temp364; void* _temp366; void* _temp368; void*
_temp370; _LL356: if(*(( int*) _temp354) == Cyc_Tcenv_Outermost){ _LL363:
_temp362=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp354)->f1; goto
_LL357;} else{ goto _LL358;} _LL358: if(*(( int*) _temp354) == Cyc_Tcenv_Frame){
_LL367: _temp366=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp354)->f1; goto
_LL365; _LL365: _temp364=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp354)->f2;
goto _LL359;} else{ goto _LL360;} _LL360: if(*(( int*) _temp354) == Cyc_Tcenv_Hidden){
_LL371: _temp370=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp354)->f1; goto
_LL369; _LL369: _temp368=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp354)->f2;
goto _LL361;} else{ goto _LL355;} _LL357: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp372=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp372[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp373; _temp373.tag= Cyc_Tcenv_Outermost;
_temp373.f1=( void*) g( _temp362); _temp373;}); _temp372;}); _LL359: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp374=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp374[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp375; _temp375.tag= Cyc_Tcenv_Frame; _temp375.f1=( void*) g( _temp366);
_temp375.f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp364); _temp375;}); _temp374;}); _LL361: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp376=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp376[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp377; _temp377.tag= Cyc_Tcenv_Hidden;
_temp377.f1=( void*) g( _temp370); _temp377.f2=( void*)(( void*(*)( void*(* g)(
void*), void* f)) Cyc_Tcenv_map_frames)( g, _temp368); _temp377;}); _temp376;});
_LL355:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( void) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( void) _throw((
void*) Cyc_Tcenv_Env_error);} return({ struct Cyc_Tcenv_Tenv* _temp378=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp378->ns= te->ns;
_temp378->ae= te->ae; _temp378->le=({ struct Cyc_Core_Opt* _temp379=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp379->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v, fe); _temp379;}); _temp378;});} static int Cyc_Tcenv_region_ancestor(
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
struct _tagged_string)({ struct _tagged_string _temp382= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp380=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp380[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp381; _temp381.tag= Cyc_Absyn_VarType; _temp381.f1= r; _temp381;}); _temp380;}));
xprintf("can't find region variable %.*s in po", _temp382.last_plus_one -
_temp382.curr, _temp382.curr);}));} static int Cyc_Tcenv_check_region_outlives(
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
void*) Cyc_Absyn_RgnKind){ result=({ struct Cyc_List_List* _temp383=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp383->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp384=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp384->tvar= tv; _temp384->ancestors= 0; _temp384;}); _temp383->tl= result;
_temp383;});}} for( 0; po != 0; po=(( struct Cyc_List_List*) _check_null( po))->tl){
struct _tuple5 _temp385=*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( po))->hd); void* _temp393; void* _temp395; void* _temp397; struct
Cyc_Absyn_Tvar* _temp399; void* _temp401; struct Cyc_Absyn_Tvar* _temp403; void*
_temp405; void* _temp407; _LL387: _LL396: _temp395= _temp385.f1; if( _temp395 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL394;} else{ goto _LL389;} _LL394: _temp393=
_temp385.f2; goto _LL388; _LL389: _LL402: _temp401= _temp385.f1; if((
unsigned int) _temp401 > 4u?*(( int*) _temp401) == Cyc_Absyn_VarType: 0){ _LL404:
_temp403=(( struct Cyc_Absyn_VarType_struct*) _temp401)->f1; goto _LL398;} else{
goto _LL391;} _LL398: _temp397= _temp385.f2; if(( unsigned int) _temp397 > 4u?*((
int*) _temp397) == Cyc_Absyn_VarType: 0){ _LL400: _temp399=(( struct Cyc_Absyn_VarType_struct*)
_temp397)->f1; goto _LL390;} else{ goto _LL391;} _LL391: _LL408: _temp407=
_temp385.f1; goto _LL406; _LL406: _temp405= _temp385.f2; goto _LL392; _LL388:
goto _LL386; _LL390: if( Cyc_Absyn_tvar_cmp( _temp403, _temp399) == 0? 1: Cyc_Tcenv_check_region_outlives(
result, _temp399, _temp403)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp413= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp409=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp409[ 0]=({ struct Cyc_Absyn_VarType_struct _temp410; _temp410.tag= Cyc_Absyn_VarType;
_temp410.f1= _temp403; _temp410;}); _temp409;})); struct _tagged_string _temp414=
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct* _temp411=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp411[ 0]=({ struct Cyc_Absyn_VarType_struct _temp412; _temp412.tag= Cyc_Absyn_VarType;
_temp412.f1= _temp399; _temp412;}); _temp411;})); xprintf("bad region lifetime assertion (circularity):%.*s < %.*s",
_temp413.last_plus_one - _temp413.curr, _temp413.curr, _temp414.last_plus_one -
_temp414.curr, _temp414.curr);}));} else{ struct Cyc_Tcenv_RgnPO* _temp415= Cyc_Tcenv_find_region_in_po(
result, _temp403); goto _LL416; _LL416: { struct Cyc_Tcenv_RgnPO* _temp417= Cyc_Tcenv_find_region_in_po(
result, _temp399); goto _LL418; _LL418: _temp417->ancestors=({ struct Cyc_List_List*
_temp419=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp419->hd=( void*) _temp415; _temp419->tl= _temp417->ancestors; _temp419;});}}
goto _LL386; _LL392: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp420= Cyc_Absynpp_typ2string( _temp407); struct
_tagged_string _temp421= Cyc_Absynpp_typ2string( _temp405); xprintf("bad region lifetime assertion:: %.*s < %.*s",
_temp420.last_plus_one - _temp420.curr, _temp420.curr, _temp421.last_plus_one -
_temp421.curr, _temp421.curr);})); goto _LL386; _LL386:;} return result;} static
struct Cyc_List_List* Cyc_Tcenv_add_region_po( struct Cyc_List_List* po, struct
Cyc_Absyn_Tvar* tv){ return({ struct Cyc_List_List* _temp422=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp422->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp423=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp423->tvar= tv; _temp423->ancestors= po; _temp423;}); _temp422->tl= po;
_temp422;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl( struct
Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp424=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp424->shared= f->shared;
_temp424->type_vars= f->type_vars; _temp424->region_order= f->region_order;
_temp424->locals= f->locals; _temp424->encloser= f->encloser; _temp424->ctrl_env=
f->ctrl_env; _temp424->capability=( void*)(( void*) f->capability); _temp424->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp424;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl(
struct Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({
struct Cyc_Tcenv_Fenv* _temp425=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Fenv)); _temp425->shared= f->shared; _temp425->type_vars= f->type_vars;
_temp425->region_order= f->region_order; _temp425->locals= f->locals; _temp425->encloser=
f->encloser; _temp425->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp426=( struct
Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp426->continue_stmt=(
void*)(( void*) c->continue_stmt); _temp426->break_stmt=( void*)(( void*) c->break_stmt);
_temp426->fallthru_clause= c->fallthru_clause; _temp426->next_stmt=( void*)((
void*) c->next_stmt); _temp426->try_depth= c->try_depth; _temp426;}); _temp425->capability=(
void*)(( void*) f->capability); _temp425->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp425;});} void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp427=( unsigned char*)"Tcenv: unexpected return_typ"; struct
_tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 29; _temp428;}));} return( void*)(( Cyc_Tcenv_get_fenv(
te))->shared)->return_typ;} struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv(
te))->type_vars;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp429=( unsigned char*)"Tcenv: unexpected add_type_vars"; struct
_tagged_string _temp430; _temp430.curr= _temp429; _temp430.base= _temp429;
_temp430.last_plus_one= _temp429 + 32; _temp430;}));}{ struct Cyc_Tcenv_Fenv*
_temp431= Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); goto _LL432;
_LL432: Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_List_List* _temp433=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs, _temp431->type_vars); goto _LL434; _LL434: Cyc_Tcutil_check_unique_tvars(
loc, _temp433); _temp431->type_vars= _temp433; return Cyc_Tcenv_put_fenv( te,
_temp431);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp435=( unsigned char*)"Tcenv: unexpected add_local_var"; struct
_tagged_string _temp436; _temp436.curr= _temp435; _temp436.base= _temp435;
_temp436.last_plus_one= _temp435 + 32; _temp436;}));}{ struct Cyc_Tcenv_Fenv*
_temp437= Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); goto _LL438;
_LL438: { struct _tagged_string* _temp439=(* vd->name).f2; goto _LL440; _LL440:{
void* _temp441=(* vd->name).f1; _LL443: if( _temp441 ==( void*) Cyc_Absyn_Loc_n){
goto _LL444;} else{ goto _LL445;} _LL445: goto _LL446; _LL444: goto _LL442;
_LL446:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp447=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp447[ 0]=({ struct Cyc_Core_Impossible_struct _temp448; _temp448.tag= Cyc_Core_Impossible;
_temp448.f1=({ unsigned char* _temp449=( unsigned char*)"add_local_var: called with Rel_n";
struct _tagged_string _temp450; _temp450.curr= _temp449; _temp450.base= _temp449;
_temp450.last_plus_one= _temp449 + 33; _temp450;}); _temp448;}); _temp447;}));
goto _LL442; _LL442:;} _temp437->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( _temp437->locals,
_temp439,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp451=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp451[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp452; _temp452.tag= Cyc_Absyn_Local_b; _temp452.f1= vd; _temp452;});
_temp451;})); return Cyc_Tcenv_put_fenv( te, _temp437);}}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp453=(
unsigned char*)"Tcenv: unexpected add_pat_var"; struct _tagged_string _temp454;
_temp454.curr= _temp453; _temp454.base= _temp453; _temp454.last_plus_one=
_temp453 + 30; _temp454;}));}{ struct Cyc_Tcenv_Fenv* _temp455= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); goto _LL456; _LL456: { struct _tagged_string* _temp457=(*
vd->name).f2; goto _LL458; _LL458: _temp455->locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
_temp455->locals, _temp457,( void*)({ struct Cyc_Absyn_Pat_b_struct* _temp459=(
struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp459[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp460; _temp460.tag= Cyc_Absyn_Pat_b;
_temp460.f1= vd; _temp460;}); _temp459;})); return Cyc_Tcenv_put_fenv( te,
_temp455);}}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp463; struct
_tagged_string* _temp464; void* _temp466; struct _tuple0* _temp461= q; _temp463=*
_temp461; _LL467: _temp466= _temp463.f1; goto _LL465; _LL465: _temp464= _temp463.f2;
goto _LL462; _LL462: { void* _temp468= _temp466; struct Cyc_List_List* _temp477;
_LL470: if( _temp468 ==( void*) Cyc_Absyn_Loc_n){ goto _LL471;} else{ goto
_LL472;} _LL472: if(( unsigned int) _temp468 > 1u?*(( int*) _temp468) == Cyc_Absyn_Rel_n:
0){ _LL478: _temp477=(( struct Cyc_Absyn_Rel_n_struct*) _temp468)->f1; if(
_temp477 == 0){ goto _LL476;} else{ goto _LL474;}} else{ goto _LL474;} _LL476:
if( te->le != 0){ goto _LL473;} else{ goto _LL474;} _LL474: goto _LL475; _LL471:
if( te->le == 0){( void) _throw(( void*) Cyc_Dict_Absent);} else{ goto _LL473;}
_LL473: { struct Cyc_Tcenv_Fenv* _temp479= Cyc_Tcenv_get_fenv( te); goto _LL480;
_LL480: { struct _handler_cons _temp481; _push_handler(& _temp481);{ int
_temp483= 0; if( setjmp( _temp481.handler)){ _temp483= 1;} if( ! _temp483){{
void* _temp486=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp484=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp484[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp485; _temp485.tag= Cyc_Tcenv_VarRes; _temp485.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp479->locals,
_temp464); _temp485;}); _temp484;}); _npop_handler( 0u); return _temp486;};
_pop_handler();} else{ void* _temp482=( void*) _exn_thrown; void* _temp488=
_temp482; _LL490: if( _temp488 == Cyc_Dict_Absent){ goto _LL491;} else{ goto
_LL492;} _LL492: goto _LL493; _LL491: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL493:( void) _throw( _temp488); _LL489:;}}}} _LL475: { struct
_handler_cons _temp494; _push_handler(& _temp494);{ int _temp496= 0; if( setjmp(
_temp494.handler)){ _temp496= 1;} if( ! _temp496){{ void* _temp497= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp497;}; _pop_handler();} else{ void*
_temp495=( void*) _exn_thrown; void* _temp499= _temp495; _LL501: if( _temp499 ==
Cyc_Dict_Absent){ goto _LL502;} else{ goto _LL503;} _LL503: goto _LL504; _LL502:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp505=
Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier: %.*s", _temp505.last_plus_one
- _temp505.curr, _temp505.curr);})); return( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp506=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp506[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp507; _temp507.tag= Cyc_Tcenv_VarRes;
_temp507.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp507;}); _temp506;});
_LL504:( void) _throw( _temp499); _LL500:;}}} _LL469:;}} void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp508=( unsigned char*)"Tcenv: unexpected process_continue";
struct _tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 35; _temp509;}));}{ void* _temp510=( void*)((
Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt; struct Cyc_Absyn_Stmt*
_temp520; _LL512: if(( unsigned int) _temp510 > 3u?*(( int*) _temp510) == Cyc_Tcenv_Stmt_j:
0){ _LL521: _temp520=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp510)->f1; goto
_LL513;} else{ goto _LL514;} _LL514: if( _temp510 ==( void*) Cyc_Tcenv_NotLoop_j){
goto _LL515;} else{ goto _LL516;} _LL516: if( _temp510 ==( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL517;} else{ goto _LL518;} _LL518: if( _temp510 ==( void*) Cyc_Tcenv_FnEnd_j){
goto _LL519;} else{ goto _LL511;} _LL513: _temp520->non_local_preds=({ struct
Cyc_List_List* _temp522=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp522->hd=( void*) s; _temp522->tl= _temp520->non_local_preds; _temp522;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp520; return; _LL515: Cyc_Tcutil_terr( s->loc,({
unsigned char* _temp523=( unsigned char*)"continue not in a loop"; struct
_tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 23; _temp524;})); return; _LL517:(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp525=(
unsigned char*)"Tcenv: continue dest is CaseEnd"; struct _tagged_string _temp526;
_temp526.curr= _temp525; _temp526.base= _temp525; _temp526.last_plus_one=
_temp525 + 32; _temp526;})); return; _LL519:(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp527=( unsigned char*)"Tcenv: continue dest is CaseEnd";
struct _tagged_string _temp528; _temp528.curr= _temp527; _temp528.base= _temp527;
_temp528.last_plus_one= _temp527 + 32; _temp528;})); return; _LL511:;}} void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp529=( unsigned char*)"Tcenv: unexpected process_break";
struct _tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 32; _temp530;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); void* _temp531=( void*)( fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt* _temp541; _LL533: if(( unsigned int) _temp531 > 3u?*((
int*) _temp531) == Cyc_Tcenv_Stmt_j: 0){ _LL542: _temp541=(( struct Cyc_Tcenv_Stmt_j_struct*)
_temp531)->f1; goto _LL534;} else{ goto _LL535;} _LL535: if( _temp531 ==( void*)
Cyc_Tcenv_NotLoop_j){ goto _LL536;} else{ goto _LL537;} _LL537: if( _temp531 ==(
void*) Cyc_Tcenv_FnEnd_j){ goto _LL538;} else{ goto _LL539;} _LL539: if(
_temp531 ==( void*) Cyc_Tcenv_CaseEnd_j){ goto _LL540;} else{ goto _LL532;}
_LL534: _temp541->non_local_preds=({ struct Cyc_List_List* _temp543=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp543->hd=( void*) s; _temp543->tl=
_temp541->non_local_preds; _temp543;});* sopt=( struct Cyc_Absyn_Stmt*) _temp541;
return; _LL536: Cyc_Tcutil_terr( s->loc,({ unsigned char* _temp544=(
unsigned char*)"break not in a loop or switch"; struct _tagged_string _temp545;
_temp545.curr= _temp544; _temp545.base= _temp544; _temp545.last_plus_one=
_temp544 + 30; _temp545;})); return; _LL538: if(( void*)( fe->shared)->return_typ
!=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr( s->loc,({ unsigned char*
_temp546=( unsigned char*)"break causes function not to return a value"; struct
_tagged_string _temp547; _temp547.curr= _temp546; _temp547.base= _temp546;
_temp547.last_plus_one= _temp546 + 44; _temp547;}));} return; _LL540: Cyc_Tcutil_terr(
s->loc,({ unsigned char* _temp548=( unsigned char*)"break causes outer switch clause to implicitly fallthru";
struct _tagged_string _temp549; _temp549.curr= _temp548; _temp549.base= _temp548;
_temp549.last_plus_one= _temp548 + 56; _temp549;})); return; _LL532:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_string* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp550=(
unsigned char*)"Tcenv: unexpected process_goto"; struct _tagged_string _temp551;
_temp551.curr= _temp550; _temp551.base= _temp550; _temp551.last_plus_one=
_temp550 + 31; _temp551;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* _temp552=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels,
l); goto _LL553; _LL553: if( _temp552 == 0){ struct Cyc_SlowDict_Dict* _temp554=(
fe->shared)->needed_labels; goto _LL555; _LL555: { struct Cyc_Core_Opt* _temp556=((
struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string* key))
Cyc_SlowDict_lookup_opt)( _temp554, l); goto _LL557; _LL557: if( _temp556 == 0){
_temp556=({ struct Cyc_Core_Opt* _temp558=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp558->v=( void*) 0; _temp558;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( _temp554, l,({ struct
Cyc_List_List* _temp559=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp559->hd=( void*) s; _temp559->tl=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp556))->v; _temp559;}));}} else{(( struct Cyc_Absyn_Stmt*)((
struct Cyc_Core_Opt*) _check_null( _temp552))->v)->non_local_preds=({ struct Cyc_List_List*
_temp560=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp560->hd=( void*) s; _temp560->tl=(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*)
_check_null( _temp552))->v)->non_local_preds; _temp560;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*) _check_null( _temp552))->v);}}}
struct _tuple3* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause*** clauseopt){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp561=( unsigned char*)"Tcenv: unexpected process_fallthru"; struct
_tagged_string _temp562; _temp562.curr= _temp561; _temp562.base= _temp561;
_temp562.last_plus_one= _temp561 + 35; _temp562;}));}{ struct Cyc_Tcenv_Fenv*
_temp563= Cyc_Tcenv_get_fenv( te); goto _LL564; _LL564: { struct _tuple3*
_temp565=( _temp563->ctrl_env)->fallthru_clause; goto _LL566; _LL566: if(
_temp565 != 0){(((*(( struct _tuple3*) _check_null( _temp565))).f1)->body)->non_local_preds=({
struct Cyc_List_List* _temp567=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp567->hd=( void*) s; _temp567->tl=(((*(( struct
_tuple3*) _check_null( _temp565))).f1)->body)->non_local_preds; _temp567;});*
clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp568=( struct Cyc_Absyn_Switch_clause**)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) * 1); _temp568[ 0]=(*((
struct _tuple3*) _check_null( _temp565))).f1; _temp568;});} return _temp565;}}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause*
clause){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp569=( unsigned char*)"Tcenv: unexpected set_fallthru";
struct _tagged_string _temp570; _temp570.curr= _temp569; _temp570.base= _temp569;
_temp570.last_plus_one= _temp569 + 31; _temp570;}));}{ struct Cyc_List_List*
_temp571= 0; goto _LL572; _LL572: for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ _temp571=({ struct Cyc_List_List* _temp573=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp573->hd=( void*)(( void*)((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->type);
_temp573->tl= _temp571; _temp573;});} _temp571=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp571);{ struct Cyc_Tcenv_Fenv*
fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=({
struct _tuple3* _temp574=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp574->f1= clause; _temp574->f2= new_tvs; _temp574->f3= _temp571; _temp574;});
return Cyc_Tcenv_put_fenv( te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp575=( unsigned char*)"Tcenv: unexpected clear_fallthru";
struct _tagged_string _temp576; _temp576.curr= _temp575; _temp576.base= _temp575;
_temp576.last_plus_one= _temp575 + 33; _temp576;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=
0; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp577=( unsigned char*)"Tcenv: unexpected set_in_loop"; struct _tagged_string
_temp578; _temp578.curr= _temp577; _temp578.base= _temp577; _temp578.last_plus_one=
_temp577 + 30; _temp578;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp579=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp579[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp580; _temp580.tag= Cyc_Tcenv_Stmt_j; _temp580.f1= continue_dest; _temp580;});
_temp579;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp581=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp581[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp582; _temp582.tag= Cyc_Tcenv_Stmt_j;
_temp582.f1= continue_dest; _temp582;}); _temp581;}))); return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp583=( unsigned char*)"Tcenv: unexpected enter_try"; struct
_tagged_string _temp584; _temp584.curr= _temp583; _temp584.base= _temp583;
_temp584.last_plus_one= _temp583 + 28; _temp584;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te)); ++( fe->ctrl_env)->try_depth;
return Cyc_Tcenv_put_fenv( te, fe);}} int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp585=( unsigned char*)"Tcenv: unexpected get_try_depth";
struct _tagged_string _temp586; _temp586.curr= _temp585; _temp586.base= _temp585;
_temp586.last_plus_one= _temp585 + 32; _temp586;}));} return(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->try_depth;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp587=( unsigned char*)"Tcenv: unexpected set_in_switch";
struct _tagged_string _temp588; _temp588.curr= _temp587; _temp588.base= _temp587;
_temp588.last_plus_one= _temp587 + 32; _temp588;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)(( void*) Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp589=( unsigned char*)"Tcenv: unexpected set_next"; struct
_tagged_string _temp590; _temp590.curr= _temp589; _temp590.base= _temp589;
_temp590.last_plus_one= _temp589 + 27; _temp590;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=(
void*) j); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp591=( unsigned char*)"Tcenv: unexpected add_label"; struct
_tagged_string _temp592; _temp592.curr= _temp591; _temp592.base= _temp591;
_temp592.last_plus_one= _temp591 + 28; _temp592;}));}{ struct Cyc_Tcenv_Fenv*
_temp593= Cyc_Tcenv_get_fenv( te); goto _LL594; _LL594: { struct Cyc_SlowDict_Dict*
_temp595=( _temp593->shared)->needed_labels; goto _LL596; _LL596: { struct Cyc_Core_Opt*
_temp597=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict* d, struct
_tagged_string* key)) Cyc_SlowDict_lookup_opt)( _temp595, v); goto _LL598;
_LL598: if( _temp597 != 0){( _temp593->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_string* key)) Cyc_SlowDict_delete)(
_temp595, v);{ struct Cyc_List_List* _temp599=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp597))->v; goto _LL600; _LL600: s->non_local_preds=
_temp599; for( 0; _temp599 != 0; _temp599=(( struct Cyc_List_List*) _check_null(
_temp599))->tl){ void* _temp601=( void*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*)
_check_null( _temp599))->hd)->r; struct Cyc_Absyn_Stmt* _temp607; struct Cyc_Absyn_Stmt**
_temp609; struct _tagged_string* _temp610; _LL603: if(( unsigned int) _temp601 >
1u?*(( int*) _temp601) == Cyc_Absyn_Goto_s: 0){ _LL611: _temp610=(( struct Cyc_Absyn_Goto_s_struct*)
_temp601)->f1; goto _LL608; _LL608: _temp607=(( struct Cyc_Absyn_Goto_s_struct*)
_temp601)->f2; _temp609=&(( struct Cyc_Absyn_Goto_s_struct*) _temp601)->f2; goto
_LL604;} else{ goto _LL605;} _LL605: goto _LL606; _LL604:* _temp609=( struct Cyc_Absyn_Stmt*)
s; goto _LL602; _LL606:(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp612=( unsigned char*)"Tcenv: add_label backpatching of non-goto";
struct _tagged_string _temp613; _temp613.curr= _temp612; _temp613.base= _temp612;
_temp613.last_plus_one= _temp612 + 42; _temp613;})); goto _LL602; _LL602:;}}}
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)((
_temp593->shared)->seen_labels, v)){ Cyc_Tcutil_terr( s->loc,( struct
_tagged_string)({ struct _tagged_string _temp614=* v; xprintf("Repeated label: %.*s",
_temp614.last_plus_one - _temp614.curr, _temp614.curr);}));}( _temp593->shared)->seen_labels=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Stmt* data)) Cyc_Dict_insert)(( _temp593->shared)->seen_labels,
v, s); return te;}}}} int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*
te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp615=( unsigned char*)"Tcenv: unexpected get_encloser";
struct _tagged_string _temp616; _temp616.curr= _temp615; _temp616.base= _temp615;
_temp616.last_plus_one= _temp615 + 31; _temp616;}));} return( Cyc_Tcenv_get_fenv(
te))->encloser;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char* _temp617=(
unsigned char*)"Tcenv: unexpected set_encloser"; struct _tagged_string _temp618;
_temp618.curr= _temp617; _temp618.base= _temp617; _temp618.last_plus_one=
_temp617 + 31; _temp618;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp619=( unsigned char*)"Tcenv::add_region: unexpected add_region";
struct _tagged_string _temp620; _temp620.curr= _temp619; _temp620.base= _temp619;
_temp620.last_plus_one= _temp619 + 41; _temp620;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar*
tv;{ void* _temp621= Cyc_Tcutil_compress( r); struct Cyc_Absyn_Tvar* _temp627;
_LL623: if(( unsigned int) _temp621 > 4u?*(( int*) _temp621) == Cyc_Absyn_VarType:
0){ _LL628: _temp627=(( struct Cyc_Absyn_VarType_struct*) _temp621)->f1; goto
_LL624;} else{ goto _LL625;} _LL625: goto _LL626; _LL624: tv= _temp627; goto
_LL622; _LL626: tv=(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({
unsigned char* _temp629=( unsigned char*)"bad add region"; struct _tagged_string
_temp630; _temp630.curr= _temp629; _temp630.base= _temp629; _temp630.last_plus_one=
_temp629 + 15; _temp630;})); goto _LL622; _LL622:;} fe->region_order= Cyc_Tcenv_add_region_po(
fe->region_order, tv);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp631=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp631[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp632; _temp632.tag= Cyc_Absyn_JoinEff;
_temp632.f1=({ struct Cyc_List_List* _temp633=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp633->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp635=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp635[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp636; _temp636.tag= Cyc_Absyn_AccessEff;
_temp636.f1=( void*) r; _temp636;}); _temp635;})); _temp633->tl=({ struct Cyc_List_List*
_temp634=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp634->hd=( void*)(( void*) fe->capability); _temp634->tl= 0; _temp634;});
_temp633;}); _temp632;}); _temp631;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(({ unsigned char*
_temp637=( unsigned char*)"Tcenv: unexpected new_block"; struct _tagged_string
_temp638; _temp638.curr= _temp637; _temp638.base= _temp637; _temp638.last_plus_one=
_temp637 + 28; _temp638;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp646=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp646[ 0]=({ struct Cyc_Absyn_VarType_struct _temp647; _temp647.tag= Cyc_Absyn_VarType;
_temp647.f1= block_rgn; _temp647;}); _temp646;}); fe->type_vars=({ struct Cyc_List_List*
_temp639=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp639->hd=( void*) block_rgn; _temp639->tl= fe->type_vars; _temp639;}); Cyc_Tcutil_check_unique_tvars(
loc, fe->type_vars); fe->region_order= Cyc_Tcenv_add_region_po( fe->region_order,
block_rgn);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp640=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp640[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp641; _temp641.tag= Cyc_Absyn_JoinEff;
_temp641.f1=({ struct Cyc_List_List* _temp642=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp642->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp644=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp644[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp645; _temp645.tag= Cyc_Absyn_AccessEff;
_temp645.f1=( void*) block_typ; _temp645;}); _temp644;})); _temp642->tl=({
struct Cyc_List_List* _temp643=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp643->hd=( void*)(( void*) fe->capability); _temp643->tl=
0; _temp643;}); _temp642;}); _temp641;}); _temp640;})));( void*)( fe->curr_rgn=(
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
_temp648= Cyc_Absynpp_typ2string( rgn); xprintf("Expression accesses unavailable region %.*s",
_temp648.last_plus_one - _temp648.curr, _temp648.curr);}));} int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv* te, void* rt_a, void* rt_b){ if( rt_a ==( void*) Cyc_Absyn_HeapRgn){
return 1;} if( te->le != 0){ struct _tuple5 _temp650=({ struct _tuple5 _temp649;
_temp649.f1= Cyc_Tcutil_compress( rt_a); _temp649.f2= Cyc_Tcutil_compress( rt_b);
_temp649;}); void* _temp656; struct Cyc_Absyn_Tvar* _temp658; void* _temp660;
struct Cyc_Absyn_Tvar* _temp662; _LL652: _LL661: _temp660= _temp650.f1; if((
unsigned int) _temp660 > 4u?*(( int*) _temp660) == Cyc_Absyn_VarType: 0){ _LL663:
_temp662=(( struct Cyc_Absyn_VarType_struct*) _temp660)->f1; goto _LL657;} else{
goto _LL654;} _LL657: _temp656= _temp650.f2; if(( unsigned int) _temp656 > 4u?*((
int*) _temp656) == Cyc_Absyn_VarType: 0){ _LL659: _temp658=(( struct Cyc_Absyn_VarType_struct*)
_temp656)->f1; goto _LL653;} else{ goto _LL654;} _LL654: goto _LL655; _LL653:
return Cyc_Tcenv_check_region_outlives(( Cyc_Tcenv_get_fenv( te))->region_order,
_temp662, _temp658); _LL655: return 0; _LL651:;} return 0;} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp664=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp664[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp665; _temp665.tag= Cyc_Absyn_JoinEff;
_temp665.f1= 0; _temp665;}); _temp664;});} else{ struct Cyc_Tcenv_Fenv* _temp666=
Cyc_Tcenv_get_fenv( te); goto _LL667; _LL667: capability=( void*) _temp666->capability;}
if( ! Cyc_Tcutil_subset_effect( 1, eff, capability)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp668= Cyc_Absynpp_typ2string(
capability); struct _tagged_string _temp669= Cyc_Absynpp_typ2string( eff);
xprintf("Capability %.*s does not cover function's effect %.*s", _temp668.last_plus_one
- _temp668.curr, _temp668.curr, _temp669.last_plus_one - _temp669.curr, _temp669.curr);}));}}
void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* po){ for( 0; po != 0; po=(( struct Cyc_List_List*)
_check_null( po))->tl){ struct _tuple5 _temp672; void* _temp673; void* _temp675;
struct _tuple5* _temp670=( struct _tuple5*)(( struct Cyc_List_List*) _check_null(
po))->hd; _temp672=* _temp670; _LL676: _temp675= _temp672.f1; goto _LL674;
_LL674: _temp673= _temp672.f2; goto _LL671; _LL671: if( ! Cyc_Tcenv_region_outlives(
te, _temp675, _temp673)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp677= Cyc_Absynpp_typ2string( _temp675); struct
_tagged_string _temp678= Cyc_Absynpp_typ2string( _temp673); xprintf("function requires that region %.*s outlive %.*s",
_temp677.last_plus_one - _temp677.curr, _temp677.curr, _temp678.last_plus_one -
_temp678.curr, _temp678.curr);}));}}} struct _tuple6{ struct _tagged_string* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict*
locals=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar*
rgn0=({ struct Cyc_Absyn_Tvar* _temp740=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp740->name=({ struct _tagged_string*
_temp741=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp741[ 0]=( struct _tagged_string)({ struct _tagged_string _temp742=*(* fd->name).f2;
xprintf("`%.*s", _temp742.last_plus_one - _temp742.curr, _temp742.curr);});
_temp741;}); _temp740->identity= Cyc_Tcutil_new_tvar_id(); _temp740->kind=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind);
_temp740;}); struct Cyc_List_List* _temp680=({ struct Cyc_List_List* _temp679=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp679->hd=(
void*) rgn0; _temp679->tl= fd->tvs; _temp679;}); goto _LL681; _LL681: Cyc_Tcutil_check_unique_tvars(
loc, _temp680);{ struct Cyc_List_List* _temp682= Cyc_Tcenv_add_region_po( Cyc_Tcenv_initial_region_po(
loc, fd->tvs, fd->rgn_po), rgn0); goto _LL683; _LL683: { void* param_rgn=( void*)({
struct Cyc_Absyn_VarType_struct* _temp738=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp738[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp739; _temp739.tag= Cyc_Absyn_VarType; _temp739.f1= rgn0; _temp739;});
_temp738;}); struct Cyc_List_List* _temp684= 0; goto _LL685; _LL685:{ struct Cyc_List_List*
_temp686= fd->args; goto _LL687; _LL687: for( 0; _temp686 != 0; _temp686=((
struct Cyc_List_List*) _check_null( _temp686))->tl){ struct Cyc_Absyn_Vardecl*
_temp694=({ struct Cyc_Absyn_Vardecl* _temp688=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp688[ 0]=({ struct Cyc_Absyn_Vardecl
_temp689; _temp689.sc=( void*)(( void*) Cyc_Absyn_Public); _temp689.name=({
struct _tuple0* _temp692=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp692[ 0]=({ struct _tuple0 _temp693; _temp693.f1=( void*) Cyc_Absyn_Loc_n;
_temp693.f2=(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp686))->hd)).f1;
_temp693;}); _temp692;}); _temp689.tq=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( _temp686))->hd)).f2; _temp689.type=( void*)(*(( struct _tuple6*)((
struct Cyc_List_List*) _check_null( _temp686))->hd)).f3; _temp689.initializer= 0;
_temp689.rgn=({ struct Cyc_Core_Opt* _temp690=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp690[ 0]=({ struct Cyc_Core_Opt _temp691;
_temp691.v=( void*) param_rgn; _temp691;}); _temp690;}); _temp689.attributes= 0;
_temp689;}); _temp688;}); goto _LL695; _LL695: _temp684=({ struct Cyc_List_List*
_temp696=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp696[ 0]=({ struct Cyc_List_List _temp697; _temp697.hd=( void*) _temp694;
_temp697.tl= _temp684; _temp697;}); _temp696;}); locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
locals,(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp686))->hd)).f1,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp698=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp698[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp699; _temp699.tag= Cyc_Absyn_Param_b; _temp699.f1= _temp694; _temp699;});
_temp698;}));}} if( fd->cyc_varargs != 0){ int _temp702; void* _temp704; void*
_temp706; struct Cyc_Absyn_Tqual _temp708; struct Cyc_Core_Opt* _temp710; struct
Cyc_Absyn_VarargInfo _temp700=*(( struct Cyc_Absyn_VarargInfo*) _check_null( fd->cyc_varargs));
_LL711: _temp710= _temp700.name; goto _LL709; _LL709: _temp708= _temp700.tq;
goto _LL707; _LL707: _temp706=( void*) _temp700.type; goto _LL705; _LL705:
_temp704=( void*) _temp700.rgn; goto _LL703; _LL703: _temp702= _temp700.inject;
goto _LL701; _LL701: if( _temp710 != 0){ void* _temp712= Cyc_Absyn_tagged_typ(
_temp706, _temp704, _temp708); goto _LL713; _LL713: { struct Cyc_Absyn_Vardecl*
_temp720=({ struct Cyc_Absyn_Vardecl* _temp714=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp714[ 0]=({ struct Cyc_Absyn_Vardecl
_temp715; _temp715.sc=( void*)(( void*) Cyc_Absyn_Public); _temp715.name=({
struct _tuple0* _temp718=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp718[ 0]=({ struct _tuple0 _temp719; _temp719.f1=( void*) Cyc_Absyn_Loc_n;
_temp719.f2=( struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null(
_temp710))->v; _temp719;}); _temp718;}); _temp715.tq= Cyc_Absyn_empty_tqual();
_temp715.type=( void*) _temp712; _temp715.initializer= 0; _temp715.rgn=({ struct
Cyc_Core_Opt* _temp716=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)
* 1); _temp716[ 0]=({ struct Cyc_Core_Opt _temp717; _temp717.v=( void*)
param_rgn; _temp717;}); _temp716;}); _temp715.attributes= 0; _temp715;});
_temp714;}); goto _LL721; _LL721: _temp684=({ struct Cyc_List_List* _temp722=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp722->hd=(
void*) _temp720; _temp722->tl= _temp684; _temp722;}); locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
locals,( struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null( _temp710))->v,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp723=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp723[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp724; _temp724.tag= Cyc_Absyn_Param_b; _temp724.f1= _temp720; _temp724;});
_temp723;}));}} else{ Cyc_Tcutil_terr( loc,({ unsigned char* _temp725=(
unsigned char*)"missing name for varargs"; struct _tagged_string _temp726;
_temp726.curr= _temp725; _temp726.base= _temp725; _temp726.last_plus_one=
_temp725 + 25; _temp726;}));}} fd->param_vardecls=({ struct Cyc_Core_Opt*
_temp727=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp727[ 0]=({ struct Cyc_Core_Opt _temp728; _temp728.v=( void*) _temp684;
_temp728;}); _temp727;}); return({ struct Cyc_Tcenv_Fenv* _temp729=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp729->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp737=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp737->return_typ=( void*)(( void*) fd->ret_type); _temp737->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp737->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp); _temp737;});
_temp729->type_vars= _temp680; _temp729->region_order= _temp682; _temp729->locals=
locals; _temp729->encloser= fd->body; _temp729->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv*
_temp736=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv));
_temp736->continue_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j); _temp736->break_stmt=(
void*)(( void*) Cyc_Tcenv_NotLoop_j); _temp736->fallthru_clause= 0; _temp736->next_stmt=(
void*)(( void*) Cyc_Tcenv_FnEnd_j); _temp736->try_depth= 0; _temp736;});
_temp729->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp730=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp730[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp731; _temp731.tag= Cyc_Absyn_JoinEff;
_temp731.f1=({ struct Cyc_List_List* _temp732=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp732->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp734=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp734[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp735; _temp735.tag= Cyc_Absyn_AccessEff;
_temp735.f1=( void*) param_rgn; _temp735;}); _temp734;})); _temp732->tl=({
struct Cyc_List_List* _temp733=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp733->hd=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( fd->effect))->v); _temp733->tl= 0; _temp733;}); _temp732;});
_temp731;}); _temp730;})); _temp729->curr_rgn=( void*) param_rgn; _temp729;});}}}