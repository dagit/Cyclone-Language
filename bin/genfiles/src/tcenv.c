 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
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
_sfile_to_file( Cyc_Stdio_stderr),"Error: %.*s\n", _get_arr_size( _temp0, 1u),
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
_temp1;});{ struct Cyc_Tcenv_Genv* _temp2= Cyc_Tcenv_empty_genv(); struct Cyc_List_List*
_temp3= 0; _temp2->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
_temp2->tuniondecls,(* Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_tud); _temp2->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple4* data)) Cyc_Dict_insert)( _temp2->ordinaries,(* Cyc_Absyn_null_pointer_exn_name).f2,({
struct _tuple4* _temp4=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp4->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp5=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp5[ 0]=({ struct Cyc_Tcenv_TunionRes_struct
_temp6; _temp6.tag= Cyc_Tcenv_TunionRes; _temp6.f1= Cyc_Absyn_exn_tud; _temp6.f2=
Cyc_Absyn_null_pointer_exn_tuf; _temp6;}); _temp5;}); _temp4->f2= 1; _temp4;}));
_temp2->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct _tuple4* data)) Cyc_Dict_insert)( _temp2->ordinaries,(*
Cyc_Absyn_match_exn_name).f2,({ struct _tuple4* _temp7=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp7->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct*
_temp8=( struct Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp8[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp9; _temp9.tag= Cyc_Tcenv_TunionRes;
_temp9.f1= Cyc_Absyn_exn_tud; _temp9.f2= Cyc_Absyn_match_exn_tuf; _temp9;});
_temp8;}); _temp7->f2= 1; _temp7;}));{ struct Cyc_Dict_Dict* _temp10=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct
Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp),
_temp3, _temp2); return({ struct Cyc_Tcenv_Tenv* _temp11=( struct Cyc_Tcenv_Tenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp11->ns= _temp3; _temp11->ae=
_temp10; _temp11->le= 0; _temp11;});}}} static struct Cyc_Tcenv_Genv* Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, Cyc_Tcenv_resolve_namespace(
te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
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
struct Cyc_List_List*) _check_null( nss))->hd); struct _handler_cons _temp12;
_push_handler(& _temp12);{ int _temp14= 0; if( setjmp( _temp12.handler)){
_temp14= 1;} if( ! _temp14){ lookup( ge2, v); Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp15=* v; xprintf("%.*s is ambiguous",
_get_arr_size( _temp15, 1u), _temp15.curr);}));; _pop_handler();} else{ void*
_temp13=( void*) _exn_thrown; void* _temp17= _temp13; _LL19: if( _temp17 == Cyc_Dict_Absent){
goto _LL20;} else{ goto _LL21;} _LL21: goto _LL22; _LL20: goto _LL18; _LL22:(
void) _throw( _temp17); _LL18:;}}} return;} static void* Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v){ struct Cyc_List_List*
cns= te->ns; void* result; while( 1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
cns);{ struct _handler_cons _temp23; _push_handler(& _temp23);{ int _temp25= 0;
if( setjmp( _temp23.handler)){ _temp25= 1;} if( ! _temp25){ result= lookup( ge,
v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v,
struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)( te, loc, lookup, v, ge->availables);{
void* _temp26= result; _npop_handler( 0u); return _temp26;}; _pop_handler();}
else{ void* _temp24=( void*) _exn_thrown; void* _temp28= _temp24; _LL30: if(
_temp28 == Cyc_Dict_Absent){ goto _LL31;} else{ goto _LL32;} _LL32: goto _LL33;
_LL31: goto _LL29; _LL33:( void) _throw( _temp28); _LL29:;}}}{ struct Cyc_List_List*
_temp34= ge->availables; for( 0; _temp34 != 0; _temp34=(( struct Cyc_List_List*)
_check_null( _temp34))->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp34))->hd);
struct _handler_cons _temp35; _push_handler(& _temp35);{ int _temp37= 0; if(
setjmp( _temp35.handler)){ _temp37= 1;} if( ! _temp37){ result= lookup( ge2, v);((
void(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v,
struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)( te, loc, lookup, v,((
struct Cyc_List_List*) _check_null( _temp34))->tl);{ void* _temp38= result;
_npop_handler( 0u); return _temp38;}; _pop_handler();} else{ void* _temp36=(
void*) _exn_thrown; void* _temp40= _temp36; _LL42: if( _temp40 == Cyc_Dict_Absent){
goto _LL43;} else{ goto _LL44;} _LL44: goto _LL45; _LL43: goto _LL41; _LL45:(
void) _throw( _temp40); _LL41:;}}}} if( cns == 0){( int) _throw(( void*) Cyc_Dict_Absent);}
cns= Cyc_Tcenv_outer_namespace( cns);}} static void* Cyc_Tcenv_lookup_ordinary_global_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ struct _tuple4* ans=(( struct
_tuple4*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->ordinaries, v);(* ans).f2= 1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp48; struct _tagged_arr* _temp49; void* _temp51; struct
_tuple0* _temp46= q; _temp48=* _temp46; _LL52: _temp51= _temp48.f1; goto _LL50;
_LL50: _temp49= _temp48.f2; goto _LL47; _LL47: { void* _temp53= _temp51; struct
Cyc_List_List* _temp63; struct Cyc_List_List* _temp65; struct Cyc_List_List
_temp67; struct Cyc_List_List* _temp68; struct _tagged_arr* _temp70; struct Cyc_List_List*
_temp72; _LL55: if( _temp53 ==( void*) Cyc_Absyn_Loc_n){ goto _LL56;} else{ goto
_LL57;} _LL57: if(( unsigned int) _temp53 > 1u?*(( int*) _temp53) == Cyc_Absyn_Rel_n:
0){ _LL64: _temp63=(( struct Cyc_Absyn_Rel_n_struct*) _temp53)->f1; if( _temp63
== 0){ goto _LL58;} else{ goto _LL59;}} else{ goto _LL59;} _LL59: if((
unsigned int) _temp53 > 1u?*(( int*) _temp53) == Cyc_Absyn_Rel_n: 0){ _LL66:
_temp65=(( struct Cyc_Absyn_Rel_n_struct*) _temp53)->f1; if( _temp65 == 0){ goto
_LL61;} else{ _temp67=* _temp65; _LL71: _temp70=( struct _tagged_arr*) _temp67.hd;
goto _LL69; _LL69: _temp68= _temp67.tl; goto _LL60;}} else{ goto _LL61;} _LL61:
if(( unsigned int) _temp53 > 1u?*(( int*) _temp53) == Cyc_Absyn_Abs_n: 0){ _LL73:
_temp72=(( struct Cyc_Absyn_Abs_n_struct*) _temp53)->f1; goto _LL62;} else{ goto
_LL54;} _LL56: goto _LL58; _LL58: return(( void*(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_ordinary_global_f, _temp49); _LL60: { struct Cyc_Tcenv_Genv*
_temp74= Cyc_Tcenv_lookup_namespace( te, loc, _temp70, _temp68); return Cyc_Tcenv_lookup_ordinary_global_f(
_temp74, _temp49);} _LL62: return Cyc_Tcenv_lookup_ordinary_global_f((( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, _temp72), _temp49); _LL54:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ struct Cyc_List_List* _temp75= te->ns; struct Cyc_List_List*
_temp76= 0; while( 1) { struct Cyc_Tcenv_Genv* _temp77=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp75);{ struct Cyc_List_List* _temp78= _temp77->availables; for( 0; _temp78
!= 0; _temp78=(( struct Cyc_List_List*) _check_null( _temp78))->tl){ struct Cyc_Tcenv_Genv*
ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( _temp78))->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge2->namespaces, n)){ _temp76=({ struct Cyc_List_List*
_temp79=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp79->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp75,({ struct Cyc_List_List* _temp80=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp80->hd=(
void*) n; _temp80->tl= ns; _temp80;})); _temp79->tl= _temp76; _temp79;});}}} if(((
int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp77->namespaces, n)){ _temp76=({ struct Cyc_List_List* _temp81=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp81->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp75,({
struct Cyc_List_List* _temp82=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp82->hd=( void*) n; _temp82->tl= ns; _temp82;})); _temp81->tl=
_temp76; _temp81;});} if( _temp76 != 0){ if((( struct Cyc_List_List*)
_check_null( _temp76))->tl != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp83=* n; xprintf("%.*s is ambiguous", _get_arr_size(
_temp83, 1u), _temp83.curr);}));} return( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( _temp76))->hd;} if( _temp75 == 0){( int) _throw(( void*) Cyc_Dict_Absent);}
_temp75= Cyc_Tcenv_outer_namespace( _temp75);}} static struct Cyc_Absyn_Structdecl**
Cyc_Tcenv_lookup_structdecl_f( struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){
return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->structdecls, v);} struct Cyc_Absyn_Structdecl**
Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp86; struct _tagged_arr* _temp87;
void* _temp89; struct _tuple0* _temp84= q; _temp86=* _temp84; _LL90: _temp89=
_temp86.f1; goto _LL88; _LL88: _temp87= _temp86.f2; goto _LL85; _LL85: { void*
_temp91= _temp89; struct Cyc_List_List* _temp101; struct Cyc_List_List* _temp103;
struct Cyc_List_List* _temp105; struct Cyc_List_List _temp107; struct Cyc_List_List*
_temp108; struct _tagged_arr* _temp110; _LL93: if( _temp91 ==( void*) Cyc_Absyn_Loc_n){
goto _LL94;} else{ goto _LL95;} _LL95: if(( unsigned int) _temp91 > 1u?*(( int*)
_temp91) == Cyc_Absyn_Rel_n: 0){ _LL102: _temp101=(( struct Cyc_Absyn_Rel_n_struct*)
_temp91)->f1; if( _temp101 == 0){ goto _LL96;} else{ goto _LL97;}} else{ goto
_LL97;} _LL97: if(( unsigned int) _temp91 > 1u?*(( int*) _temp91) == Cyc_Absyn_Abs_n:
0){ _LL104: _temp103=(( struct Cyc_Absyn_Abs_n_struct*) _temp91)->f1; goto _LL98;}
else{ goto _LL99;} _LL99: if(( unsigned int) _temp91 > 1u?*(( int*) _temp91) ==
Cyc_Absyn_Rel_n: 0){ _LL106: _temp105=(( struct Cyc_Absyn_Rel_n_struct*) _temp91)->f1;
if( _temp105 == 0){ goto _LL92;} else{ _temp107=* _temp105; _LL111: _temp110=(
struct _tagged_arr*) _temp107.hd; goto _LL109; _LL109: _temp108= _temp107.tl;
goto _LL100;}} else{ goto _LL92;} _LL94: goto _LL96; _LL96: return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp87);
_LL98: { struct Cyc_Tcenv_Genv* _temp112=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp103);
return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp112->structdecls, _temp87);} _LL100: {
struct Cyc_Tcenv_Genv* _temp113= Cyc_Tcenv_lookup_namespace( te, loc, _temp110,
_temp108); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp113->structdecls, _temp87);}
_LL92:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp116; struct _tagged_arr* _temp117; void* _temp119; struct _tuple0* _temp114=
q; _temp116=* _temp114; _LL120: _temp119= _temp116.f1; goto _LL118; _LL118:
_temp117= _temp116.f2; goto _LL115; _LL115: { void* _temp121= _temp119; struct
Cyc_List_List* _temp131; struct Cyc_List_List* _temp133; struct Cyc_List_List*
_temp135; struct Cyc_List_List _temp137; struct Cyc_List_List* _temp138; struct
_tagged_arr* _temp140; _LL123: if( _temp121 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL124;} else{ goto _LL125;} _LL125: if(( unsigned int) _temp121 > 1u?*(( int*)
_temp121) == Cyc_Absyn_Rel_n: 0){ _LL132: _temp131=(( struct Cyc_Absyn_Rel_n_struct*)
_temp121)->f1; if( _temp131 == 0){ goto _LL126;} else{ goto _LL127;}} else{ goto
_LL127;} _LL127: if(( unsigned int) _temp121 > 1u?*(( int*) _temp121) == Cyc_Absyn_Abs_n:
0){ _LL134: _temp133=(( struct Cyc_Absyn_Abs_n_struct*) _temp121)->f1; goto
_LL128;} else{ goto _LL129;} _LL129: if(( unsigned int) _temp121 > 1u?*(( int*)
_temp121) == Cyc_Absyn_Rel_n: 0){ _LL136: _temp135=(( struct Cyc_Absyn_Rel_n_struct*)
_temp121)->f1; if( _temp135 == 0){ goto _LL122;} else{ _temp137=* _temp135;
_LL141: _temp140=( struct _tagged_arr*) _temp137.hd; goto _LL139; _LL139:
_temp138= _temp137.tl; goto _LL130;}} else{ goto _LL122;} _LL124: goto _LL126;
_LL126: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(* lookup)( struct
Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp117); _LL128: { struct Cyc_Tcenv_Genv*
_temp142=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp133); return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp142->uniondecls,
_temp117);} _LL130: { struct Cyc_Tcenv_Genv* _temp143= Cyc_Tcenv_lookup_namespace(
te, loc, _temp140, _temp138); return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp143->uniondecls,
_temp117);} _LL122:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp146; struct _tagged_arr* _temp147; void* _temp149; struct _tuple0* _temp144=
q; _temp146=* _temp144; _LL150: _temp149= _temp146.f1; goto _LL148; _LL148:
_temp147= _temp146.f2; goto _LL145; _LL145: { void* _temp151= _temp149; struct
Cyc_List_List* _temp161; struct Cyc_List_List* _temp163; struct Cyc_List_List
_temp165; struct Cyc_List_List* _temp166; struct _tagged_arr* _temp168; struct
Cyc_List_List* _temp170; _LL153: if( _temp151 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL154;} else{ goto _LL155;} _LL155: if(( unsigned int) _temp151 > 1u?*(( int*)
_temp151) == Cyc_Absyn_Rel_n: 0){ _LL162: _temp161=(( struct Cyc_Absyn_Rel_n_struct*)
_temp151)->f1; if( _temp161 == 0){ goto _LL156;} else{ goto _LL157;}} else{ goto
_LL157;} _LL157: if(( unsigned int) _temp151 > 1u?*(( int*) _temp151) == Cyc_Absyn_Rel_n:
0){ _LL164: _temp163=(( struct Cyc_Absyn_Rel_n_struct*) _temp151)->f1; if(
_temp163 == 0){ goto _LL159;} else{ _temp165=* _temp163; _LL169: _temp168=(
struct _tagged_arr*) _temp165.hd; goto _LL167; _LL167: _temp166= _temp165.tl;
goto _LL158;}} else{ goto _LL159;} _LL159: if(( unsigned int) _temp151 > 1u?*((
int*) _temp151) == Cyc_Absyn_Abs_n: 0){ _LL171: _temp170=(( struct Cyc_Absyn_Abs_n_struct*)
_temp151)->f1; goto _LL160;} else{ goto _LL152;} _LL154: goto _LL156; _LL156:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_tuniondecl_f,
_temp147); _LL158: { struct Cyc_Tcenv_Genv* _temp172= Cyc_Tcenv_lookup_namespace(
te, loc, _temp168, _temp166); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp172->tuniondecls,
_temp147);} _LL160: { struct Cyc_Tcenv_Genv* _temp173=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp170); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp173->tuniondecls, _temp147);}
_LL152:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp176; struct _tagged_arr* _temp177; void* _temp179; struct _tuple0* _temp174=
q; _temp176=* _temp174; _LL180: _temp179= _temp176.f1; goto _LL178; _LL178:
_temp177= _temp176.f2; goto _LL175; _LL175: { void* _temp181= _temp179; struct
Cyc_List_List* _temp191; struct Cyc_List_List* _temp193; struct Cyc_List_List
_temp195; struct Cyc_List_List* _temp196; struct _tagged_arr* _temp198; struct
Cyc_List_List* _temp200; _LL183: if(( unsigned int) _temp181 > 1u?*(( int*)
_temp181) == Cyc_Absyn_Rel_n: 0){ _LL192: _temp191=(( struct Cyc_Absyn_Rel_n_struct*)
_temp181)->f1; if( _temp191 == 0){ goto _LL184;} else{ goto _LL185;}} else{ goto
_LL185;} _LL185: if( _temp181 ==( void*) Cyc_Absyn_Loc_n){ goto _LL186;} else{
goto _LL187;} _LL187: if(( unsigned int) _temp181 > 1u?*(( int*) _temp181) ==
Cyc_Absyn_Rel_n: 0){ _LL194: _temp193=(( struct Cyc_Absyn_Rel_n_struct*)
_temp181)->f1; if( _temp193 == 0){ goto _LL189;} else{ _temp195=* _temp193;
_LL199: _temp198=( struct _tagged_arr*) _temp195.hd; goto _LL197; _LL197:
_temp196= _temp195.tl; goto _LL188;}} else{ goto _LL189;} _LL189: if((
unsigned int) _temp181 > 1u?*(( int*) _temp181) == Cyc_Absyn_Abs_n: 0){ _LL201:
_temp200=(( struct Cyc_Absyn_Abs_n_struct*) _temp181)->f1; goto _LL190;} else{
goto _LL182;} _LL184: { struct _handler_cons _temp202; _push_handler(& _temp202);{
int _temp204= 0; if( setjmp( _temp202.handler)){ _temp204= 1;} if( ! _temp204){{
struct Cyc_Core_Opt* _temp206=({ struct Cyc_Core_Opt* _temp205=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp205->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp177);
_temp205;}); _npop_handler( 0u); return _temp206;}; _pop_handler();} else{ void*
_temp203=( void*) _exn_thrown; void* _temp208= _temp203; _LL210: if( _temp208 ==
Cyc_Dict_Absent){ goto _LL211;} else{ goto _LL212;} _LL212: goto _LL213; _LL211:
return 0; _LL213:( void) _throw( _temp208); _LL209:;}}} _LL186: Cyc_Tcutil_terr(
loc, _tag_arr("lookup_xtuniondecl: impossible", sizeof( unsigned char), 31u));
return 0; _LL188: { struct Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp214;
_push_handler(& _temp214);{ int _temp216= 0; if( setjmp( _temp214.handler)){
_temp216= 1;} if( ! _temp216){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp198,
_temp196);; _pop_handler();} else{ void* _temp215=( void*) _exn_thrown; void*
_temp218= _temp215; _LL220: if( _temp218 == Cyc_Dict_Absent){ goto _LL221;}
else{ goto _LL222;} _LL222: goto _LL223; _LL221: Cyc_Tcutil_terr( loc, _tag_arr("bad qualified name for xtunion",
sizeof( unsigned char), 31u));( int) _throw(( void*) Cyc_Dict_Absent); _LL223:(
void) _throw( _temp218); _LL219:;}}} return({ struct Cyc_Core_Opt* _temp224=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp224->v=(
void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp177); _temp224;});}
_LL190: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp200); return({
struct Cyc_Core_Opt* _temp225=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp225->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp177); _temp225;});} _LL182:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp228; struct _tagged_arr* _temp229; void* _temp231; struct _tuple0* _temp226=
q; _temp228=* _temp226; _LL232: _temp231= _temp228.f1; goto _LL230; _LL230:
_temp229= _temp228.f2; goto _LL227; _LL227: { void* _temp233= _temp231; struct
Cyc_List_List* _temp243; struct Cyc_List_List* _temp245; struct Cyc_List_List
_temp247; struct Cyc_List_List* _temp248; struct _tagged_arr* _temp250; struct
Cyc_List_List* _temp252; _LL235: if( _temp233 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL236;} else{ goto _LL237;} _LL237: if(( unsigned int) _temp233 > 1u?*(( int*)
_temp233) == Cyc_Absyn_Rel_n: 0){ _LL244: _temp243=(( struct Cyc_Absyn_Rel_n_struct*)
_temp233)->f1; if( _temp243 == 0){ goto _LL238;} else{ goto _LL239;}} else{ goto
_LL239;} _LL239: if(( unsigned int) _temp233 > 1u?*(( int*) _temp233) == Cyc_Absyn_Rel_n:
0){ _LL246: _temp245=(( struct Cyc_Absyn_Rel_n_struct*) _temp233)->f1; if(
_temp245 == 0){ goto _LL241;} else{ _temp247=* _temp245; _LL251: _temp250=(
struct _tagged_arr*) _temp247.hd; goto _LL249; _LL249: _temp248= _temp247.tl;
goto _LL240;}} else{ goto _LL241;} _LL241: if(( unsigned int) _temp233 > 1u?*((
int*) _temp233) == Cyc_Absyn_Abs_n: 0){ _LL253: _temp252=(( struct Cyc_Absyn_Abs_n_struct*)
_temp233)->f1; goto _LL242;} else{ goto _LL234;} _LL236: goto _LL238; _LL238:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_enumdecl_f,
_temp229); _LL240: { struct Cyc_Tcenv_Genv* _temp254= Cyc_Tcenv_lookup_namespace(
te, loc, _temp250, _temp248); return(( struct Cyc_Absyn_Enumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp254->enumdecls,
_temp229);} _LL242: { struct Cyc_Tcenv_Genv* _temp255=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp252); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp255->enumdecls, _temp229);}
_LL234:;}} static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->typedefs,
v);} struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp258; struct _tagged_arr* _temp259; void* _temp261; struct _tuple0* _temp256=
q; _temp258=* _temp256; _LL262: _temp261= _temp258.f1; goto _LL260; _LL260:
_temp259= _temp258.f2; goto _LL257; _LL257: { void* _temp263= _temp261; struct
Cyc_List_List* _temp273; struct Cyc_List_List* _temp275; struct Cyc_List_List
_temp277; struct Cyc_List_List* _temp278; struct _tagged_arr* _temp280; struct
Cyc_List_List* _temp282; _LL265: if( _temp263 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp263 > 1u?*(( int*)
_temp263) == Cyc_Absyn_Rel_n: 0){ _LL274: _temp273=(( struct Cyc_Absyn_Rel_n_struct*)
_temp263)->f1; if( _temp273 == 0){ goto _LL268;} else{ goto _LL269;}} else{ goto
_LL269;} _LL269: if(( unsigned int) _temp263 > 1u?*(( int*) _temp263) == Cyc_Absyn_Rel_n:
0){ _LL276: _temp275=(( struct Cyc_Absyn_Rel_n_struct*) _temp263)->f1; if(
_temp275 == 0){ goto _LL271;} else{ _temp277=* _temp275; _LL281: _temp280=(
struct _tagged_arr*) _temp277.hd; goto _LL279; _LL279: _temp278= _temp277.tl;
goto _LL270;}} else{ goto _LL271;} _LL271: if(( unsigned int) _temp263 > 1u?*((
int*) _temp263) == Cyc_Absyn_Abs_n: 0){ _LL283: _temp282=(( struct Cyc_Absyn_Abs_n_struct*)
_temp263)->f1; goto _LL272;} else{ goto _LL264;} _LL266: goto _LL268; _LL268:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_typedefdecl_f,
_temp259); _LL270: { struct Cyc_Tcenv_Genv* _temp284= Cyc_Tcenv_lookup_namespace(
te, loc, _temp280, _temp278); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp284->typedefs,
_temp259);} _LL272: { struct Cyc_Tcenv_Genv* _temp285=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp282); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp285->typedefs, _temp259);}
_LL264:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp286=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp286->namespaces=( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Tcutil_empty_var_set))->v; _temp286->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp286->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp286->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp286->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp286->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp286->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp286->availables= 0; _temp286;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp287= f; void* _temp295; void* _temp297; void* _temp299;
void* _temp301; void* _temp303; _LL289: if(*(( int*) _temp287) == Cyc_Tcenv_Outermost){
_LL296: _temp295=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp287)->f1;
goto _LL290;} else{ goto _LL291;} _LL291: if(*(( int*) _temp287) == Cyc_Tcenv_Frame){
_LL300: _temp299=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp287)->f1; goto
_LL298; _LL298: _temp297=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp287)->f2;
goto _LL292;} else{ goto _LL293;} _LL293: if(*(( int*) _temp287) == Cyc_Tcenv_Hidden){
_LL304: _temp303=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp287)->f1; goto
_LL302; _LL302: _temp301=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp287)->f2;
goto _LL294;} else{ goto _LL288;} _LL290: return _temp295; _LL292: return
_temp299; _LL294: f= _temp301; goto _LL288; _LL288:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp305= f; void* _temp313; void* _temp315; void*
_temp317; void* _temp319; void* _temp321; _LL307: if(*(( int*) _temp305) == Cyc_Tcenv_Outermost){
_LL314: _temp313=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp305)->f1;
goto _LL308;} else{ goto _LL309;} _LL309: if(*(( int*) _temp305) == Cyc_Tcenv_Frame){
_LL318: _temp317=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp305)->f1; goto
_LL316; _LL316: _temp315=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp305)->f2;
goto _LL310;} else{ goto _LL311;} _LL311: if(*(( int*) _temp305) == Cyc_Tcenv_Hidden){
_LL322: _temp321=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp305)->f1; goto
_LL320; _LL320: _temp319=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp305)->f2;
goto _LL312;} else{ goto _LL306;} _LL308: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp323=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp323[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp324; _temp324.tag= Cyc_Tcenv_Outermost;
_temp324.f1=( void*) x; _temp324;}); _temp323;}); _LL310: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp325=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp325[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp326; _temp326.tag= Cyc_Tcenv_Frame; _temp326.f1=( void*) x; _temp326.f2=(
void*) _temp315; _temp326;}); _temp325;}); _LL312: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp327=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp327[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp328; _temp328.tag= Cyc_Tcenv_Hidden;
_temp328.f1=( void*) _temp321; _temp328.f2=( void*)(( void*(*)( void* f, void* x))
Cyc_Tcenv_put_visible)( _temp319, x); _temp328;}); _temp327;}); _LL306:;} void*
Cyc_Tcenv_apply_to_visible( void*(* g)( void*), void* f){ return(( void*(*)(
void* f, void* x)) Cyc_Tcenv_put_visible)( f, g((( void*(*)( void* f)) Cyc_Tcenv_get_visible)(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp329=
f; void* _temp337; void* _temp339; void* _temp341; void* _temp343; void*
_temp345; _LL331: if(*(( int*) _temp329) == Cyc_Tcenv_Outermost){ _LL338:
_temp337=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp329)->f1; goto
_LL332;} else{ goto _LL333;} _LL333: if(*(( int*) _temp329) == Cyc_Tcenv_Frame){
_LL342: _temp341=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp329)->f1; goto
_LL340; _LL340: _temp339=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp329)->f2;
goto _LL334;} else{ goto _LL335;} _LL335: if(*(( int*) _temp329) == Cyc_Tcenv_Hidden){
_LL346: _temp345=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp329)->f1; goto
_LL344; _LL344: _temp343=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp329)->f2;
goto _LL336;} else{ goto _LL330;} _LL332: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp347=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp347[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp348; _temp348.tag= Cyc_Tcenv_Outermost;
_temp348.f1=( void*) g( _temp337); _temp348;}); _temp347;}); _LL334: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp349=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp349[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp350; _temp350.tag= Cyc_Tcenv_Frame; _temp350.f1=( void*) g( _temp341);
_temp350.f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp339); _temp350;}); _temp349;}); _LL336: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp351=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp351[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp352; _temp352.tag= Cyc_Tcenv_Hidden;
_temp352.f1=( void*) g( _temp345); _temp352.f2=( void*)(( void*(*)( void*(* g)(
void*), void* f)) Cyc_Tcenv_map_frames)( g, _temp343); _temp352;}); _temp351;});
_LL330:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( int) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( int) _throw((
void*) Cyc_Tcenv_Env_error);} return({ struct Cyc_Tcenv_Tenv* _temp353=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp353->ns= te->ns;
_temp353->ae= te->ae; _temp353->le=({ struct Cyc_Core_Opt* _temp354=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp354->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v, fe); _temp354;}); _temp353;});} static int Cyc_Tcenv_region_ancestor(
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
struct _tagged_arr)({ struct _tagged_arr _temp357= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Absyn_VarType_struct* _temp355=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp355[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp356; _temp356.tag= Cyc_Absyn_VarType; _temp356.f1= r; _temp356;}); _temp355;}));
xprintf("can't find region variable %.*s in po", _get_arr_size( _temp357, 1u),
_temp357.curr);}));} static int Cyc_Tcenv_check_region_outlives( struct Cyc_List_List*
po, struct Cyc_Absyn_Tvar* r1, struct Cyc_Absyn_Tvar* r2){ struct Cyc_List_List*
r2_ancestors; while( po != 0? Cyc_Absyn_tvar_cmp((( struct Cyc_Tcenv_RgnPO*)((
struct Cyc_List_List*) _check_null( po))->hd)->tvar, r2) != 0: 0) { po=(( struct
Cyc_List_List*) _check_null( po))->tl;} if( po == 0){ return 0;} return Cyc_Tcenv_region_ancestor(((
struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null( po))->hd)->ancestors,
r1);} struct _tuple5{ void* f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tcenv_initial_region_po(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_List_List*
po){ struct Cyc_List_List* result= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; if( Cyc_Tcutil_tvar_kind( tv) ==(
void*) Cyc_Absyn_RgnKind){ result=({ struct Cyc_List_List* _temp358=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp358->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp359=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp359->tvar= tv; _temp359->ancestors= 0; _temp359;}); _temp358->tl= result;
_temp358;});}} for( 0; po != 0; po=(( struct Cyc_List_List*) _check_null( po))->tl){
struct _tuple5 _temp360=*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( po))->hd); void* _temp368; void* _temp370; void* _temp372; struct
Cyc_Absyn_Tvar* _temp374; void* _temp376; struct Cyc_Absyn_Tvar* _temp378; void*
_temp380; void* _temp382; _LL362: _LL371: _temp370= _temp360.f1; if( _temp370 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL369;} else{ goto _LL364;} _LL369: _temp368=
_temp360.f2; goto _LL363; _LL364: _LL377: _temp376= _temp360.f1; if((
unsigned int) _temp376 > 4u?*(( int*) _temp376) == Cyc_Absyn_VarType: 0){ _LL379:
_temp378=(( struct Cyc_Absyn_VarType_struct*) _temp376)->f1; goto _LL373;} else{
goto _LL366;} _LL373: _temp372= _temp360.f2; if(( unsigned int) _temp372 > 4u?*((
int*) _temp372) == Cyc_Absyn_VarType: 0){ _LL375: _temp374=(( struct Cyc_Absyn_VarType_struct*)
_temp372)->f1; goto _LL365;} else{ goto _LL366;} _LL366: _LL383: _temp382=
_temp360.f1; goto _LL381; _LL381: _temp380= _temp360.f2; goto _LL367; _LL363:
goto _LL361; _LL365: if( Cyc_Absyn_tvar_cmp( _temp378, _temp374) == 0? 1: Cyc_Tcenv_check_region_outlives(
result, _temp374, _temp378)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp388= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp384=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp384[ 0]=({ struct Cyc_Absyn_VarType_struct _temp385; _temp385.tag= Cyc_Absyn_VarType;
_temp385.f1= _temp378; _temp385;}); _temp384;})); struct _tagged_arr _temp389=
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct* _temp386=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp386[ 0]=({ struct Cyc_Absyn_VarType_struct _temp387; _temp387.tag= Cyc_Absyn_VarType;
_temp387.f1= _temp374; _temp387;}); _temp386;})); xprintf("bad region lifetime assertion (circularity):%.*s < %.*s",
_get_arr_size( _temp388, 1u), _temp388.curr, _get_arr_size( _temp389, 1u),
_temp389.curr);}));} else{ struct Cyc_Tcenv_RgnPO* _temp390= Cyc_Tcenv_find_region_in_po(
result, _temp378); struct Cyc_Tcenv_RgnPO* _temp391= Cyc_Tcenv_find_region_in_po(
result, _temp374); _temp391->ancestors=({ struct Cyc_List_List* _temp392=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp392->hd=(
void*) _temp390; _temp392->tl= _temp391->ancestors; _temp392;});} goto _LL361;
_LL367: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp393=
Cyc_Absynpp_typ2string( _temp382); struct _tagged_arr _temp394= Cyc_Absynpp_typ2string(
_temp380); xprintf("bad region lifetime assertion:: %.*s < %.*s", _get_arr_size(
_temp393, 1u), _temp393.curr, _get_arr_size( _temp394, 1u), _temp394.curr);}));
goto _LL361; _LL361:;} return result;} static struct Cyc_List_List* Cyc_Tcenv_add_region_po(
struct Cyc_List_List* po, struct Cyc_Absyn_Tvar* tv){ return({ struct Cyc_List_List*
_temp395=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp395->hd=( void*)({ struct Cyc_Tcenv_RgnPO* _temp396=( struct Cyc_Tcenv_RgnPO*)
GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO)); _temp396->tvar= tv; _temp396->ancestors=
po; _temp396;}); _temp395->tl= po; _temp395;});} static struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_copy_fenv_old_ctrl( struct Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv*
_temp397=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp397->shared= f->shared; _temp397->type_vars= f->type_vars; _temp397->region_order=
f->region_order; _temp397->locals= f->locals; _temp397->encloser= f->encloser;
_temp397->ctrl_env= f->ctrl_env; _temp397->capability=( void*)(( void*) f->capability);
_temp397->curr_rgn=( void*)(( void*) f->curr_rgn); _temp397;});} static struct
Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl( struct Cyc_Tcenv_Fenv* f){ struct
Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({ struct Cyc_Tcenv_Fenv* _temp398=(
struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp398->shared=
f->shared; _temp398->type_vars= f->type_vars; _temp398->region_order= f->region_order;
_temp398->locals= f->locals; _temp398->encloser= f->encloser; _temp398->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp399=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp399->continue_stmt=( void*)(( void*) c->continue_stmt);
_temp399->break_stmt=( void*)(( void*) c->break_stmt); _temp399->fallthru_clause=
c->fallthru_clause; _temp399->next_stmt=( void*)(( void*) c->next_stmt);
_temp399->try_depth= c->try_depth; _temp399;}); _temp398->capability=( void*)((
void*) f->capability); _temp398->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp398;});} void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected return_typ",
sizeof( unsigned char), 29u));} return( void*)(( Cyc_Tcenv_get_fenv( te))->shared)->return_typ;}
struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv( te))->type_vars;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_type_vars",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp400= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_List_List*
_temp401=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, _temp400->type_vars); Cyc_Tcutil_check_unique_tvars(
loc, _temp401); _temp400->type_vars= _temp401; return Cyc_Tcenv_put_fenv( te,
_temp400);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_local_var",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp402= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp403=(* vd->name).f2;{ void*
_temp404=(* vd->name).f1; _LL406: if( _temp404 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL407;} else{ goto _LL408;} _LL408: goto _LL409; _LL407: goto _LL405; _LL409:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp410=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp410[ 0]=({ struct
Cyc_Core_Impossible_struct _temp411; _temp411.tag= Cyc_Core_Impossible; _temp411.f1=
_tag_arr("add_local_var: called with Rel_n", sizeof( unsigned char), 33u);
_temp411;}); _temp410;})); goto _LL405; _LL405:;} _temp402->locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(
_temp402->locals, _temp403,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp412=(
struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp412[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp413; _temp413.tag= Cyc_Absyn_Local_b;
_temp413.f1= vd; _temp413;}); _temp412;})); return Cyc_Tcenv_put_fenv( te,
_temp402);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_pat_var",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* _temp414= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp415=(* vd->name).f2; _temp414->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void*
data)) Cyc_Dict_insert)( _temp414->locals, _temp415,( void*)({ struct Cyc_Absyn_Pat_b_struct*
_temp416=( struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp416[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp417; _temp417.tag= Cyc_Absyn_Pat_b;
_temp417.f1= vd; _temp417;}); _temp416;})); return Cyc_Tcenv_put_fenv( te,
_temp414);}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp420; struct
_tagged_arr* _temp421; void* _temp423; struct _tuple0* _temp418= q; _temp420=*
_temp418; _LL424: _temp423= _temp420.f1; goto _LL422; _LL422: _temp421= _temp420.f2;
goto _LL419; _LL419: { void* _temp425= _temp423; struct Cyc_List_List* _temp434;
_LL427: if( _temp425 ==( void*) Cyc_Absyn_Loc_n){ goto _LL428;} else{ goto
_LL429;} _LL429: if(( unsigned int) _temp425 > 1u?*(( int*) _temp425) == Cyc_Absyn_Rel_n:
0){ _LL435: _temp434=(( struct Cyc_Absyn_Rel_n_struct*) _temp425)->f1; if(
_temp434 == 0){ goto _LL433;} else{ goto _LL431;}} else{ goto _LL431;} _LL433:
if( te->le != 0){ goto _LL430;} else{ goto _LL431;} _LL431: goto _LL432; _LL428:
if( te->le == 0){( int) _throw(( void*) Cyc_Dict_Absent);} else{ goto _LL430;}
_LL430: { struct Cyc_Tcenv_Fenv* _temp436= Cyc_Tcenv_get_fenv( te); struct
_handler_cons _temp437; _push_handler(& _temp437);{ int _temp439= 0; if( setjmp(
_temp437.handler)){ _temp439= 1;} if( ! _temp439){{ void* _temp442=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp440=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp440[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp441; _temp441.tag= Cyc_Tcenv_VarRes; _temp441.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp436->locals,
_temp421); _temp441;}); _temp440;}); _npop_handler( 0u); return _temp442;};
_pop_handler();} else{ void* _temp438=( void*) _exn_thrown; void* _temp444=
_temp438; _LL446: if( _temp444 == Cyc_Dict_Absent){ goto _LL447;} else{ goto
_LL448;} _LL448: goto _LL449; _LL447: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL449:( void) _throw( _temp444); _LL445:;}}} _LL432: { struct
_handler_cons _temp450; _push_handler(& _temp450);{ int _temp452= 0; if( setjmp(
_temp450.handler)){ _temp452= 1;} if( ! _temp452){{ void* _temp453= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp453;}; _pop_handler();} else{ void*
_temp451=( void*) _exn_thrown; void* _temp455= _temp451; _LL457: if( _temp455 ==
Cyc_Dict_Absent){ goto _LL458;} else{ goto _LL459;} _LL459: goto _LL460; _LL458:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp461= Cyc_Absynpp_qvar2string(
q); xprintf("unbound identifier: %.*s", _get_arr_size( _temp461, 1u), _temp461.curr);}));
return( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp462=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp462[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp463; _temp463.tag= Cyc_Tcenv_VarRes; _temp463.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b);
_temp463;}); _temp462;}); _LL460:( void) _throw( _temp455); _LL456:;}}} _LL426:;}}
void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( int(*)( struct _tagged_arr
msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_continue", sizeof(
unsigned char), 35u));}{ void* _temp464=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp474; _LL466: if(( unsigned int) _temp464 > 3u?*((
int*) _temp464) == Cyc_Tcenv_Stmt_j: 0){ _LL475: _temp474=(( struct Cyc_Tcenv_Stmt_j_struct*)
_temp464)->f1; goto _LL467;} else{ goto _LL468;} _LL468: if( _temp464 ==( void*)
Cyc_Tcenv_NotLoop_j){ goto _LL469;} else{ goto _LL470;} _LL470: if( _temp464 ==(
void*) Cyc_Tcenv_CaseEnd_j){ goto _LL471;} else{ goto _LL472;} _LL472: if(
_temp464 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL473;} else{ goto _LL465;} _LL467:
_temp474->non_local_preds=({ struct Cyc_List_List* _temp476=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp476->hd=( void*) s; _temp476->tl=
_temp474->non_local_preds; _temp476;});* sopt=( struct Cyc_Absyn_Stmt*) _temp474;
return; _LL469: Cyc_Tcutil_terr( s->loc, _tag_arr("continue not in a loop",
sizeof( unsigned char), 23u)); return; _LL471:(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd", sizeof(
unsigned char), 32u)); return; _LL473:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: continue dest is CaseEnd", sizeof( unsigned char), 32u));
return; _LL465:;}} void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_break",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv(
te); void* _temp477=( void*)( fe->ctrl_env)->break_stmt; struct Cyc_Absyn_Stmt*
_temp487; _LL479: if(( unsigned int) _temp477 > 3u?*(( int*) _temp477) == Cyc_Tcenv_Stmt_j:
0){ _LL488: _temp487=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp477)->f1; goto
_LL480;} else{ goto _LL481;} _LL481: if( _temp477 ==( void*) Cyc_Tcenv_NotLoop_j){
goto _LL482;} else{ goto _LL483;} _LL483: if( _temp477 ==( void*) Cyc_Tcenv_FnEnd_j){
goto _LL484;} else{ goto _LL485;} _LL485: if( _temp477 ==( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL486;} else{ goto _LL478;} _LL480: _temp487->non_local_preds=({ struct
Cyc_List_List* _temp489=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp489->hd=( void*) s; _temp489->tl= _temp487->non_local_preds; _temp489;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp487; return; _LL482: Cyc_Tcutil_terr( s->loc,
_tag_arr("break not in a loop or switch", sizeof( unsigned char), 30u)); return;
_LL484: if(( void*)( fe->shared)->return_typ !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(
s->loc, _tag_arr("break causes function not to return a value", sizeof(
unsigned char), 44u));} return; _LL486: Cyc_Tcutil_terr( s->loc, _tag_arr("break causes outer switch clause to implicitly fallthru",
sizeof( unsigned char), 56u)); return; _LL478:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_arr* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( int(*)( struct _tagged_arr
msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_goto", sizeof(
unsigned char), 31u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* _temp490=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
if( _temp490 == 0){ struct Cyc_SlowDict_Dict* _temp491=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* _temp492=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp491, l); if(
_temp492 == 0){ _temp492=({ struct Cyc_Core_Opt* _temp493=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp493->v=( void*) 0; _temp493;});}(
fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)(
_temp491, l,({ struct Cyc_List_List* _temp494=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp494->hd=( void*) s; _temp494->tl=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp492))->v; _temp494;}));}
else{(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*) _check_null( _temp490))->v)->non_local_preds=({
struct Cyc_List_List* _temp495=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp495->hd=( void*) s; _temp495->tl=(( struct Cyc_Absyn_Stmt*)((
struct Cyc_Core_Opt*) _check_null( _temp490))->v)->non_local_preds; _temp495;});*
sopt=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*)
_check_null( _temp490))->v);}}} struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause***
clauseopt){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_fallthru", sizeof( unsigned char), 35u));}{
struct Cyc_Tcenv_Fenv* _temp496= Cyc_Tcenv_get_fenv( te); struct _tuple3*
_temp497=( _temp496->ctrl_env)->fallthru_clause; if( _temp497 != 0){(((*((
struct _tuple3*) _check_null( _temp497))).f1)->body)->non_local_preds=({ struct
Cyc_List_List* _temp498=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp498->hd=( void*) s; _temp498->tl=(((*(( struct _tuple3*) _check_null(
_temp497))).f1)->body)->non_local_preds; _temp498;});* clauseopt=({ struct Cyc_Absyn_Switch_clause**
_temp499=( struct Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*)
* 1); _temp499[ 0]=(*(( struct _tuple3*) _check_null( _temp497))).f1; _temp499;});}
return _temp497;}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause*
clause){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_fallthru", sizeof( unsigned char), 31u));}{
struct Cyc_List_List* _temp500= 0; for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ _temp500=({ struct Cyc_List_List* _temp501=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp501->hd=( void*)(( void*)((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->type);
_temp501->tl= _temp500; _temp501;});} _temp500=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp500);{ struct Cyc_Tcenv_Fenv*
fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=({
struct _tuple3* _temp502=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp502->f1= clause; _temp502->f2= new_tvs; _temp502->f3= _temp500; _temp502;});
return Cyc_Tcenv_put_fenv( te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected clear_fallthru", sizeof(
unsigned char), 33u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause= 0; return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le == 0){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_loop",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp503=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp503[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp504; _temp504.tag= Cyc_Tcenv_Stmt_j; _temp504.f1= continue_dest; _temp504;});
_temp503;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp505=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp505[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp506; _temp506.tag= Cyc_Tcenv_Stmt_j;
_temp506.f1= continue_dest; _temp506;}); _temp505;}))); return Cyc_Tcenv_put_fenv(
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
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* _temp507= Cyc_Tcenv_get_fenv(
te); struct Cyc_SlowDict_Dict* _temp508=( _temp507->shared)->needed_labels;
struct Cyc_Core_Opt* _temp509=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp508, v); if(
_temp509 != 0){( _temp507->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_arr* key)) Cyc_SlowDict_delete)(
_temp508, v);{ struct Cyc_List_List* _temp510=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp509))->v; s->non_local_preds= _temp510; for( 0;
_temp510 != 0; _temp510=(( struct Cyc_List_List*) _check_null( _temp510))->tl){
void* _temp511=( void*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*)
_check_null( _temp510))->hd)->r; struct Cyc_Absyn_Stmt* _temp517; struct Cyc_Absyn_Stmt**
_temp519; struct _tagged_arr* _temp520; _LL513: if(( unsigned int) _temp511 > 1u?*((
int*) _temp511) == Cyc_Absyn_Goto_s: 0){ _LL521: _temp520=(( struct Cyc_Absyn_Goto_s_struct*)
_temp511)->f1; goto _LL518; _LL518: _temp517=(( struct Cyc_Absyn_Goto_s_struct*)
_temp511)->f2; _temp519=&(( struct Cyc_Absyn_Goto_s_struct*) _temp511)->f2; goto
_LL514;} else{ goto _LL515;} _LL515: goto _LL516; _LL514:* _temp519=( struct Cyc_Absyn_Stmt*)
s; goto _LL512; _LL516:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: add_label backpatching of non-goto", sizeof( unsigned char), 42u));
goto _LL512; _LL512:;}}} if((( int(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_member)(( _temp507->shared)->seen_labels, v)){ Cyc_Tcutil_terr(
s->loc,( struct _tagged_arr)({ struct _tagged_arr _temp522=* v; xprintf("Repeated label: %.*s",
_get_arr_size( _temp522, 1u), _temp522.curr);}));}( _temp507->shared)->seen_labels=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Stmt* data)) Cyc_Dict_insert)(( _temp507->shared)->seen_labels,
v, s); return te;}} int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv* te){
return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)((( Cyc_Tcenv_get_fenv(
te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
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
Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar* tv;{ void* _temp523= Cyc_Tcutil_compress(
r); struct Cyc_Absyn_Tvar* _temp529; _LL525: if(( unsigned int) _temp523 > 4u?*((
int*) _temp523) == Cyc_Absyn_VarType: 0){ _LL530: _temp529=(( struct Cyc_Absyn_VarType_struct*)
_temp523)->f1; goto _LL526;} else{ goto _LL527;} _LL527: goto _LL528; _LL526: tv=
_temp529; goto _LL524; _LL528: tv=(( struct Cyc_Absyn_Tvar*(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("bad add region", sizeof(
unsigned char), 15u)); goto _LL524; _LL524:;} fe->region_order= Cyc_Tcenv_add_region_po(
fe->region_order, tv);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp531=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp531[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp532; _temp532.tag= Cyc_Absyn_JoinEff;
_temp532.f1=({ struct Cyc_List_List* _temp533=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp533->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp535=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp535[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp536; _temp536.tag= Cyc_Absyn_AccessEff;
_temp536.f1=( void*) r; _temp536;}); _temp535;})); _temp533->tl=({ struct Cyc_List_List*
_temp534=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp534->hd=( void*)(( void*) fe->capability); _temp534->tl= 0; _temp534;});
_temp533;}); _temp532;}); _temp531;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected new_block",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp544=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp544[ 0]=({ struct Cyc_Absyn_VarType_struct _temp545; _temp545.tag= Cyc_Absyn_VarType;
_temp545.f1= block_rgn; _temp545;}); _temp544;}); fe->type_vars=({ struct Cyc_List_List*
_temp537=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp537->hd=( void*) block_rgn; _temp537->tl= fe->type_vars; _temp537;}); Cyc_Tcutil_check_unique_tvars(
loc, fe->type_vars); fe->region_order= Cyc_Tcenv_add_region_po( fe->region_order,
block_rgn);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp538=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp538[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp539; _temp539.tag= Cyc_Absyn_JoinEff;
_temp539.f1=({ struct Cyc_List_List* _temp540=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp540->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp542=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp542[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp543; _temp543.tag= Cyc_Absyn_AccessEff;
_temp543.f1=( void*) block_typ; _temp543;}); _temp542;})); _temp540->tl=({
struct Cyc_List_List* _temp541=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp541->hd=( void*)(( void*) fe->capability); _temp541->tl=
0; _temp541;}); _temp540;}); _temp539;}); _temp538;})));( void*)( fe->curr_rgn=(
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
_temp546= Cyc_Absynpp_typ2string( rgn); xprintf("Expression accesses unavailable region %.*s",
_get_arr_size( _temp546, 1u), _temp546.curr);}));} int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv* te, void* rt_a, void* rt_b){ if( rt_a ==( void*) Cyc_Absyn_HeapRgn){
return 1;} if( te->le != 0){ struct _tuple5 _temp548=({ struct _tuple5 _temp547;
_temp547.f1= Cyc_Tcutil_compress( rt_a); _temp547.f2= Cyc_Tcutil_compress( rt_b);
_temp547;}); void* _temp554; struct Cyc_Absyn_Tvar* _temp556; void* _temp558;
struct Cyc_Absyn_Tvar* _temp560; _LL550: _LL559: _temp558= _temp548.f1; if((
unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_VarType: 0){ _LL561:
_temp560=(( struct Cyc_Absyn_VarType_struct*) _temp558)->f1; goto _LL555;} else{
goto _LL552;} _LL555: _temp554= _temp548.f2; if(( unsigned int) _temp554 > 4u?*((
int*) _temp554) == Cyc_Absyn_VarType: 0){ _LL557: _temp556=(( struct Cyc_Absyn_VarType_struct*)
_temp554)->f1; goto _LL551;} else{ goto _LL552;} _LL552: goto _LL553; _LL551:
return Cyc_Tcenv_check_region_outlives(( Cyc_Tcenv_get_fenv( te))->region_order,
_temp560, _temp556); _LL553: return 0; _LL549:;} return 0;} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp562=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp562[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp563; _temp563.tag= Cyc_Absyn_JoinEff;
_temp563.f1= 0; _temp563;}); _temp562;});} else{ struct Cyc_Tcenv_Fenv* _temp564=
Cyc_Tcenv_get_fenv( te); capability=( void*) _temp564->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp565= Cyc_Absynpp_typ2string( capability); struct _tagged_arr
_temp566= Cyc_Absynpp_typ2string( eff); xprintf("Capability %.*s does not cover function's effect %.*s",
_get_arr_size( _temp565, 1u), _temp565.curr, _get_arr_size( _temp566, 1u),
_temp566.curr);}));}} void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po){ for( 0; po != 0;
po=(( struct Cyc_List_List*) _check_null( po))->tl){ struct _tuple5 _temp569;
void* _temp570; void* _temp572; struct _tuple5* _temp567=( struct _tuple5*)((
struct Cyc_List_List*) _check_null( po))->hd; _temp569=* _temp567; _LL573:
_temp572= _temp569.f1; goto _LL571; _LL571: _temp570= _temp569.f2; goto _LL568;
_LL568: if( ! Cyc_Tcenv_region_outlives( te, _temp572, _temp570)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp574= Cyc_Absynpp_typ2string(
_temp572); struct _tagged_arr _temp575= Cyc_Absynpp_typ2string( _temp570);
xprintf("function requires that region %.*s outlive %.*s", _get_arr_size(
_temp574, 1u), _temp574.curr, _get_arr_size( _temp575, 1u), _temp575.curr);}));}}}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar* _temp628=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp628->name=({ struct _tagged_arr*
_temp629=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp629[ 0]=( struct _tagged_arr)({ struct _tagged_arr _temp630=*(* fd->name).f2;
xprintf("`%.*s", _get_arr_size( _temp630, 1u), _temp630.curr);}); _temp629;});
_temp628->identity= Cyc_Tcutil_new_tvar_id(); _temp628->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp628;}); struct
Cyc_List_List* _temp576=({ struct Cyc_List_List* _temp627=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp627->hd=( void*) rgn0; _temp627->tl=
fd->tvs; _temp627;}); Cyc_Tcutil_check_unique_tvars( loc, _temp576);{ struct Cyc_List_List*
_temp577= Cyc_Tcenv_add_region_po( Cyc_Tcenv_initial_region_po( loc, fd->tvs, fd->rgn_po),
rgn0); void* param_rgn=( void*)({ struct Cyc_Absyn_VarType_struct* _temp625=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp625[ 0]=({ struct Cyc_Absyn_VarType_struct _temp626; _temp626.tag= Cyc_Absyn_VarType;
_temp626.f1= rgn0; _temp626;}); _temp625;}); struct Cyc_List_List* _temp578= 0;{
struct Cyc_List_List* _temp579= fd->args; for( 0; _temp579 != 0; _temp579=((
struct Cyc_List_List*) _check_null( _temp579))->tl){ struct Cyc_Absyn_Vardecl*
_temp580=({ struct Cyc_Absyn_Vardecl* _temp585=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp585[ 0]=({ struct Cyc_Absyn_Vardecl
_temp586; _temp586.sc=( void*)(( void*) Cyc_Absyn_Public); _temp586.name=({
struct _tuple0* _temp589=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp589[ 0]=({ struct _tuple0 _temp590; _temp590.f1=( void*) Cyc_Absyn_Loc_n;
_temp590.f2=(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp579))->hd)).f1;
_temp590;}); _temp589;}); _temp586.tq=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( _temp579))->hd)).f2; _temp586.type=( void*)(*(( struct _tuple6*)((
struct Cyc_List_List*) _check_null( _temp579))->hd)).f3; _temp586.initializer= 0;
_temp586.rgn=({ struct Cyc_Core_Opt* _temp587=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp587[ 0]=({ struct Cyc_Core_Opt _temp588;
_temp588.v=( void*) param_rgn; _temp588;}); _temp587;}); _temp586.attributes= 0;
_temp586;}); _temp585;}); _temp578=({ struct Cyc_List_List* _temp581=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp581[ 0]=({
struct Cyc_List_List _temp582; _temp582.hd=( void*) _temp580; _temp582.tl=
_temp578; _temp582;}); _temp581;}); locals=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,(*((
struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp579))->hd)).f1,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp583=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp583[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp584; _temp584.tag= Cyc_Absyn_Param_b; _temp584.f1= _temp580; _temp584;});
_temp583;}));}} if( fd->cyc_varargs != 0){ int _temp593; void* _temp595; void*
_temp597; struct Cyc_Absyn_Tqual _temp599; struct Cyc_Core_Opt* _temp601; struct
Cyc_Absyn_VarargInfo _temp591=*(( struct Cyc_Absyn_VarargInfo*) _check_null( fd->cyc_varargs));
_LL602: _temp601= _temp591.name; goto _LL600; _LL600: _temp599= _temp591.tq;
goto _LL598; _LL598: _temp597=( void*) _temp591.type; goto _LL596; _LL596:
_temp595=( void*) _temp591.rgn; goto _LL594; _LL594: _temp593= _temp591.inject;
goto _LL592; _LL592: if( _temp601 != 0){ void* _temp603= Cyc_Absyn_tagged_typ(
_temp597, _temp595, _temp599); struct Cyc_Absyn_Vardecl* _temp604=({ struct Cyc_Absyn_Vardecl*
_temp608=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)
* 1); _temp608[ 0]=({ struct Cyc_Absyn_Vardecl _temp609; _temp609.sc=( void*)((
void*) Cyc_Absyn_Public); _temp609.name=({ struct _tuple0* _temp612=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0) * 1); _temp612[ 0]=({ struct
_tuple0 _temp613; _temp613.f1=( void*) Cyc_Absyn_Loc_n; _temp613.f2=( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp601))->v; _temp613;});
_temp612;}); _temp609.tq= Cyc_Absyn_empty_tqual(); _temp609.type=( void*)
_temp603; _temp609.initializer= 0; _temp609.rgn=({ struct Cyc_Core_Opt* _temp610=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp610[ 0]=({
struct Cyc_Core_Opt _temp611; _temp611.v=( void*) param_rgn; _temp611;});
_temp610;}); _temp609.attributes= 0; _temp609;}); _temp608;}); _temp578=({
struct Cyc_List_List* _temp605=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp605->hd=( void*) _temp604; _temp605->tl= _temp578;
_temp605;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp601))->v,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp606=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp606[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp607; _temp607.tag= Cyc_Absyn_Param_b;
_temp607.f1= _temp604; _temp607;}); _temp606;}));} else{ Cyc_Tcutil_terr( loc,
_tag_arr("missing name for varargs", sizeof( unsigned char), 25u));}} fd->param_vardecls=({
struct Cyc_Core_Opt* _temp614=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) * 1); _temp614[ 0]=({ struct Cyc_Core_Opt _temp615; _temp615.v=(
void*) _temp578; _temp615;}); _temp614;}); return({ struct Cyc_Tcenv_Fenv*
_temp616=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp616->shared=({ struct Cyc_Tcenv_SharedFenv* _temp624=( struct Cyc_Tcenv_SharedFenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv)); _temp624->return_typ=( void*)((
void*) fd->ret_type); _temp624->seen_labels=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp624->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp);
_temp624;}); _temp616->type_vars= _temp576; _temp616->region_order= _temp577;
_temp616->locals= locals; _temp616->encloser= fd->body; _temp616->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp623=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp623->continue_stmt=( void*)(( void*)
Cyc_Tcenv_NotLoop_j); _temp623->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp623->fallthru_clause= 0; _temp623->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp623->try_depth= 0; _temp623;}); _temp616->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp617=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp617[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp618; _temp618.tag= Cyc_Absyn_JoinEff; _temp618.f1=({ struct Cyc_List_List*
_temp619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp619->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp621=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp621[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp622; _temp622.tag= Cyc_Absyn_AccessEff;
_temp622.f1=( void*) param_rgn; _temp622;}); _temp621;})); _temp619->tl=({
struct Cyc_List_List* _temp620=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp620->hd=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( fd->effect))->v); _temp620->tl= 0; _temp620;}); _temp619;});
_temp618;}); _temp617;})); _temp616->curr_rgn=( void*) param_rgn; _temp616;});}}