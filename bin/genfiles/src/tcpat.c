 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; struct _tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd(
struct _tuple0*); extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length(
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*, int), void*
env); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set*
s); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern
unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern int Cyc_String_strcmp( struct _tagged_string s1,
struct _tagged_string s2); extern int Cyc_String_zstrcmp( struct _tagged_string,
struct _tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static const
int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const
int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_string
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_string desc;
} ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple1{ void* f1;
struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
_tagged_string* name; struct Cyc_Absyn_Conref* kind; } ; static const int Cyc_Absyn_Unknown_b=
0; static const int Cyc_Absyn_Upper_b= 0; struct Cyc_Absyn_Upper_b_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{ void* elt_typ; void*
rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual tq; struct
Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1* tunion_name; struct _tuple1* field_name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{
int tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
1; struct Cyc_Absyn_KnownTunionfield_struct{ int tag; struct Cyc_Absyn_Tuniondecl*
f1; struct Cyc_Absyn_Tunionfield* f2; } ; struct Cyc_Absyn_TunionFieldInfo{ void*
field_info; struct Cyc_List_List* targs; } ; static const int Cyc_Absyn_VoidType=
0; static const int Cyc_Absyn_Evar= 0; struct Cyc_Absyn_Evar_struct{ int tag;
void* f1; struct Cyc_Core_Opt* f2; int f3; } ; static const int Cyc_Absyn_VarType=
1; struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ;
static const int Cyc_Absyn_TunionType= 2; struct Cyc_Absyn_TunionType_struct{
int tag; struct Cyc_Absyn_TunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType=
3; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_PointerType= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; static const int Cyc_Absyn_IntType= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Absyn_FloatType= 1; static const int Cyc_Absyn_DoubleType= 2; static
const int Cyc_Absyn_ArrayType= 6; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ; static const
int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; static const int Cyc_Absyn_TupleType= 8; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType= 9;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
16; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
_tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_float_t; extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); static const
int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern
void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_unify( void*, void*);
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct
_tuple4* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle*, struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_string err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); struct _tuple5{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; } ; extern struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static void Cyc_Tcpat_resolve_pat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp0=( void*) p->r; struct _tuple1*
_temp10; struct Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; struct
_tuple1* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20;
struct _tuple1* _temp22; _LL2: if(( unsigned int) _temp0 > 2u?*(( int*) _temp0)
== Cyc_Absyn_UnknownId_p: 0){ _LL11: _temp10=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownCall_p: 0){ _LL17: _temp16=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f1; goto _LL15; _LL15: _temp14=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f2; goto _LL13; _LL13: _temp12=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f3; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownFields_p: 0){ _LL23: _temp22=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f1; goto _LL21; _LL21: _temp20=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f2; goto _LL19; _LL19: _temp18=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f3; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3:{ struct
_handler_cons _temp24; _push_handler(& _temp24);{ int _temp26= 0; if( setjmp(
_temp24.handler)){ _temp26= 1;} if( ! _temp26){{ void* _temp27= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp10); struct Cyc_Absyn_Structdecl* _temp37; struct Cyc_Absyn_Tunionfield*
_temp39; struct Cyc_Absyn_Tuniondecl* _temp41; struct Cyc_Absyn_Enumfield*
_temp43; struct Cyc_Absyn_Enumdecl* _temp45; void* _temp47; _LL29: if(*(( int*)
_temp27) == Cyc_Tcenv_StructRes){ _LL38: _temp37=(( struct Cyc_Tcenv_StructRes_struct*)
_temp27)->f1; goto _LL30;} else{ goto _LL31;} _LL31: if(*(( int*) _temp27) ==
Cyc_Tcenv_TunionRes){ _LL42: _temp41=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f1; goto _LL40; _LL40: _temp39=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(*(( int*) _temp27) ==
Cyc_Tcenv_EnumRes){ _LL46: _temp45=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f1;
goto _LL44; _LL44: _temp43=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f2;
goto _LL34;} else{ goto _LL35;} _LL35: if(*(( int*) _temp27) == Cyc_Tcenv_VarRes){
_LL48: _temp47=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp27)->f1; goto
_LL36;} else{ goto _LL28;} _LL30: Cyc_Tcutil_terr( p->loc,({ unsigned char*
_temp49="struct tag used without arguments in pattern"; struct _tagged_string
_temp50; _temp50.curr= _temp49; _temp50.base= _temp49; _temp50.last_plus_one=
_temp49 + 45; _temp50;})); _npop_handler( 0u); return; _LL32:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp51=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp51[ 0]=({ struct Cyc_Absyn_Tunion_p_struct
_temp52; _temp52.tag= Cyc_Absyn_Tunion_p; _temp52.f1= _temp41; _temp52.f2=
_temp39; _temp52.f3= 0; _temp52.f4= 0; _temp52;}); _temp51;}))); _npop_handler(
0u); return; _LL34:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp53=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp53[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp54; _temp54.tag= Cyc_Absyn_Enum_p;
_temp54.f1= _temp45; _temp54.f2= _temp43; _temp54;}); _temp53;})));
_npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;}; _pop_handler();} else{
void* _temp25=( void*) _exn_thrown; void* _temp56= _temp25; _LL58: if( _temp56
== Cyc_Dict_Absent){ goto _LL59;} else{ goto _LL60;} _LL60: goto _LL61; _LL59:
goto _LL57; _LL61:( void) _throw( _temp56); _LL57:;}}}{ void* _temp62=(* _temp10).f1;
struct Cyc_List_List* _temp70; _LL64: if( _temp62 ==( void*) Cyc_Absyn_Loc_n){
goto _LL65;} else{ goto _LL66;} _LL66: if(( unsigned int) _temp62 > 1u?*(( int*)
_temp62) == Cyc_Absyn_Rel_n: 0){ _LL71: _temp70=(( struct Cyc_Absyn_Rel_n_struct*)
_temp62)->f1; if( _temp70 == 0){ goto _LL67;} else{ goto _LL68;}} else{ goto
_LL68;} _LL68: goto _LL69; _LL65: goto _LL67; _LL67:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp72=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp72[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp73; _temp73.tag= Cyc_Absyn_Var_p;
_temp73.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp73;}); _temp72;}))); return; _LL69: Cyc_Tcutil_terr( p->loc,({
unsigned char* _temp74="qualified variable in pattern"; struct _tagged_string
_temp75; _temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one=
_temp74 + 30; _temp75;})); return; _LL63:;} _LL5:{ struct _handler_cons _temp76;
_push_handler(& _temp76);{ int _temp78= 0; if( setjmp( _temp76.handler)){
_temp78= 1;} if( ! _temp78){{ void* _temp79= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp16); struct Cyc_Absyn_Structdecl* _temp89; struct Cyc_Absyn_Tunionfield*
_temp91; struct Cyc_Absyn_Tuniondecl* _temp93; struct Cyc_Absyn_Enumfield*
_temp95; struct Cyc_Absyn_Enumdecl* _temp97; void* _temp99; _LL81: if(*(( int*)
_temp79) == Cyc_Tcenv_StructRes){ _LL90: _temp89=(( struct Cyc_Tcenv_StructRes_struct*)
_temp79)->f1; goto _LL82;} else{ goto _LL83;} _LL83: if(*(( int*) _temp79) ==
Cyc_Tcenv_TunionRes){ _LL94: _temp93=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp79)->f1; goto _LL92; _LL92: _temp91=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp79)->f2; goto _LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp79) ==
Cyc_Tcenv_EnumRes){ _LL98: _temp97=(( struct Cyc_Tcenv_EnumRes_struct*) _temp79)->f1;
goto _LL96; _LL96: _temp95=(( struct Cyc_Tcenv_EnumRes_struct*) _temp79)->f2;
goto _LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp79) == Cyc_Tcenv_VarRes){
_LL100: _temp99=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp79)->f1; goto
_LL88;} else{ goto _LL80;} _LL82: { struct Cyc_List_List* _temp101= 0; goto
_LL102; _LL102: for( 0; _temp12 != 0; _temp12=(( struct Cyc_List_List*)
_check_null( _temp12))->tl){ _temp101=({ struct Cyc_List_List* _temp103=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp103->hd=( void*)({
struct _tuple6* _temp104=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp104->f1= 0; _temp104->f2=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp12))->hd; _temp104;}); _temp103->tl= _temp101; _temp103;});}(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp105=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp105[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp106; _temp106.tag= Cyc_Absyn_Struct_p;
_temp106.f1= _temp89; _temp106.f2= 0; _temp106.f3= _temp14; _temp106.f4=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp101);
_temp106;}); _temp105;}))); _npop_handler( 0u); return;} _LL84:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp107=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp107[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp108; _temp108.tag= Cyc_Absyn_Tunion_p; _temp108.f1=
_temp93; _temp108.f2= _temp91; _temp108.f3= _temp14; _temp108.f4= _temp12;
_temp108;}); _temp107;}))); _npop_handler( 0u); return; _LL86: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp109= Cyc_Absynpp_qvar2string( _temp16);
xprintf("bad enum constructor %.*s in pattern", _temp109.last_plus_one -
_temp109.curr, _temp109.curr);})); _npop_handler( 0u); return; _LL88: goto _LL80;
_LL80:;}; _pop_handler();} else{ void* _temp77=( void*) _exn_thrown; void*
_temp111= _temp77; _LL113: if( _temp111 == Cyc_Dict_Absent){ goto _LL114;} else{
goto _LL115;} _LL115: goto _LL116; _LL114: goto _LL112; _LL116:( void) _throw(
_temp111); _LL112:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp117=
Cyc_Absynpp_qvar2string( _temp16); xprintf("%.*s is not a constructor in pattern",
_temp117.last_plus_one - _temp117.curr, _temp117.curr);})); return; _LL7:{
struct _handler_cons _temp118; _push_handler(& _temp118);{ int _temp120= 0; if(
setjmp( _temp118.handler)){ _temp120= 1;} if( ! _temp120){{ void* _temp121= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp22); struct Cyc_Absyn_Structdecl* _temp127; _LL123: if(*(( int*)
_temp121) == Cyc_Tcenv_StructRes){ _LL128: _temp127=(( struct Cyc_Tcenv_StructRes_struct*)
_temp121)->f1; goto _LL124;} else{ goto _LL125;} _LL125: goto _LL126; _LL124:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp129=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp129[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp130; _temp130.tag= Cyc_Absyn_Struct_p;
_temp130.f1= _temp127; _temp130.f2= 0; _temp130.f3= _temp20; _temp130.f4=
_temp18; _temp130;}); _temp129;}))); _npop_handler( 0u); return; _LL126: goto
_LL122; _LL122:;}; _pop_handler();} else{ void* _temp119=( void*) _exn_thrown;
void* _temp132= _temp119; _LL134: if( _temp132 == Cyc_Dict_Absent){ goto _LL135;}
else{ goto _LL136;} _LL136: goto _LL137; _LL135: goto _LL133; _LL137:( void)
_throw( _temp132); _LL133:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string
_temp138= Cyc_Absynpp_qvar2string( _temp22); xprintf("%.*s is not a struct name, but pattern has designators",
_temp138.last_plus_one - _temp138.curr, _temp138.curr);})); return; _LL9:
return; _LL1:;} static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl*
vd){ return(* vd->name).f2;} static void* Cyc_Tcpat_any_type( void** topt){ if(
topt != 0){ return*(( void**) _check_null( topt));} return Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind);} static void* Cyc_Tcpat_num_type( void** topt, void*
numt){ if( topt != 0? Cyc_Tcutil_coerceable(*(( void**) _check_null( topt))): 0){
return*(( void**) _check_null( topt));}{ void* _temp139= Cyc_Tcutil_compress(
numt); struct Cyc_Absyn_Enumdecl* _temp145; struct _tuple1* _temp147; _LL141:
if(( unsigned int) _temp139 > 4u?*(( int*) _temp139) == Cyc_Absyn_EnumType: 0){
_LL148: _temp147=(( struct Cyc_Absyn_EnumType_struct*) _temp139)->f1; goto
_LL146; _LL146: _temp145=(( struct Cyc_Absyn_EnumType_struct*) _temp139)->f2;
goto _LL142;} else{ goto _LL143;} _LL143: goto _LL144; _LL142: if( topt != 0){
return*(( void**) _check_null( topt));} goto _LL140; _LL144: goto _LL140; _LL140:;}
return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl* vd, struct
Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t); vd->tq=
Cyc_Absyn_empty_tqual();* v_result_ptr=({ struct Cyc_List_List* _temp149=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp149->hd=(
void*) vd; _temp149->tl=* v_result_ptr; _temp149;});} struct _tuple7{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple5 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp150=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp184; struct Cyc_Absyn_Vardecl*
_temp186; int _temp188; void* _temp190; int _temp192; void* _temp194;
unsigned char _temp196; struct _tagged_string _temp198; struct Cyc_Absyn_Enumfield*
_temp200; struct Cyc_Absyn_Enumdecl* _temp202; struct Cyc_Absyn_Pat* _temp204;
struct Cyc_List_List* _temp206; struct Cyc_List_List* _temp208; struct Cyc_List_List*
_temp210; struct Cyc_Core_Opt* _temp212; struct Cyc_Core_Opt** _temp214; struct
Cyc_Absyn_Structdecl* _temp215; struct Cyc_List_List* _temp217; struct Cyc_List_List*
_temp219; struct Cyc_Absyn_Tunionfield* _temp221; struct Cyc_Absyn_Tuniondecl*
_temp223; struct _tuple1* _temp225; struct Cyc_List_List* _temp227; struct Cyc_List_List*
_temp229; struct _tuple1* _temp231; struct Cyc_List_List* _temp233; struct Cyc_List_List*
_temp235; struct _tuple1* _temp237; _LL152: if( _temp150 ==( void*) Cyc_Absyn_Wild_p){
goto _LL153;} else{ goto _LL154;} _LL154: if(( unsigned int) _temp150 > 2u?*((
int*) _temp150) == Cyc_Absyn_Var_p: 0){ _LL185: _temp184=(( struct Cyc_Absyn_Var_p_struct*)
_temp150)->f1; goto _LL155;} else{ goto _LL156;} _LL156: if(( unsigned int)
_temp150 > 2u?*(( int*) _temp150) == Cyc_Absyn_Reference_p: 0){ _LL187: _temp186=((
struct Cyc_Absyn_Reference_p_struct*) _temp150)->f1; goto _LL157;} else{ goto
_LL158;} _LL158: if(( unsigned int) _temp150 > 2u?*(( int*) _temp150) == Cyc_Absyn_Int_p:
0){ _LL191: _temp190=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp150)->f1;
if( _temp190 ==( void*) Cyc_Absyn_Unsigned){ goto _LL189;} else{ goto _LL160;}
_LL189: _temp188=(( struct Cyc_Absyn_Int_p_struct*) _temp150)->f2; goto _LL159;}
else{ goto _LL160;} _LL160: if(( unsigned int) _temp150 > 2u?*(( int*) _temp150)
== Cyc_Absyn_Int_p: 0){ _LL195: _temp194=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp150)->f1; if( _temp194 ==( void*) Cyc_Absyn_Signed){ goto _LL193;} else{
goto _LL162;} _LL193: _temp192=(( struct Cyc_Absyn_Int_p_struct*) _temp150)->f2;
goto _LL161;} else{ goto _LL162;} _LL162: if(( unsigned int) _temp150 > 2u?*((
int*) _temp150) == Cyc_Absyn_Char_p: 0){ _LL197: _temp196=(( struct Cyc_Absyn_Char_p_struct*)
_temp150)->f1; goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int)
_temp150 > 2u?*(( int*) _temp150) == Cyc_Absyn_Float_p: 0){ _LL199: _temp198=((
struct Cyc_Absyn_Float_p_struct*) _temp150)->f1; goto _LL165;} else{ goto _LL166;}
_LL166: if(( unsigned int) _temp150 > 2u?*(( int*) _temp150) == Cyc_Absyn_Enum_p:
0){ _LL203: _temp202=(( struct Cyc_Absyn_Enum_p_struct*) _temp150)->f1; goto
_LL201; _LL201: _temp200=(( struct Cyc_Absyn_Enum_p_struct*) _temp150)->f2; goto
_LL167;} else{ goto _LL168;} _LL168: if( _temp150 ==( void*) Cyc_Absyn_Null_p){
goto _LL169;} else{ goto _LL170;} _LL170: if(( unsigned int) _temp150 > 2u?*((
int*) _temp150) == Cyc_Absyn_Pointer_p: 0){ _LL205: _temp204=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp150)->f1; goto _LL171;} else{ goto _LL172;} _LL172: if(( unsigned int)
_temp150 > 2u?*(( int*) _temp150) == Cyc_Absyn_Tuple_p: 0){ _LL207: _temp206=((
struct Cyc_Absyn_Tuple_p_struct*) _temp150)->f1; goto _LL173;} else{ goto _LL174;}
_LL174: if(( unsigned int) _temp150 > 2u?*(( int*) _temp150) == Cyc_Absyn_Struct_p:
0){ _LL216: _temp215=(( struct Cyc_Absyn_Struct_p_struct*) _temp150)->f1; goto
_LL213; _LL213: _temp212=(( struct Cyc_Absyn_Struct_p_struct*) _temp150)->f2;
_temp214=&(( struct Cyc_Absyn_Struct_p_struct*) _temp150)->f2; goto _LL211;
_LL211: _temp210=(( struct Cyc_Absyn_Struct_p_struct*) _temp150)->f3; goto
_LL209; _LL209: _temp208=(( struct Cyc_Absyn_Struct_p_struct*) _temp150)->f4;
goto _LL175;} else{ goto _LL176;} _LL176: if(( unsigned int) _temp150 > 2u?*((
int*) _temp150) == Cyc_Absyn_Tunion_p: 0){ _LL224: _temp223=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp150)->f1; goto _LL222; _LL222: _temp221=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp150)->f2; goto _LL220; _LL220: _temp219=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp150)->f3; goto _LL218; _LL218: _temp217=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp150)->f4; goto _LL177;} else{ goto _LL178;} _LL178: if(( unsigned int)
_temp150 > 2u?*(( int*) _temp150) == Cyc_Absyn_UnknownId_p: 0){ _LL226: _temp225=((
struct Cyc_Absyn_UnknownId_p_struct*) _temp150)->f1; goto _LL179;} else{ goto
_LL180;} _LL180: if(( unsigned int) _temp150 > 2u?*(( int*) _temp150) == Cyc_Absyn_UnknownCall_p:
0){ _LL232: _temp231=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp150)->f1;
goto _LL230; _LL230: _temp229=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp150)->f2; goto _LL228; _LL228: _temp227=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp150)->f3; goto _LL181;} else{ goto _LL182;} _LL182: if(( unsigned int)
_temp150 > 2u?*(( int*) _temp150) == Cyc_Absyn_UnknownFields_p: 0){ _LL238:
_temp237=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp150)->f1; goto _LL236;
_LL236: _temp235=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp150)->f2;
goto _LL234; _LL234: _temp233=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp150)->f3; goto _LL183;} else{ goto _LL151;} _LL153: t= Cyc_Tcpat_any_type(
topt); goto _LL151; _LL155: t= Cyc_Tcpat_any_type( topt); Cyc_Tcpat_set_vd(
_temp184,& v_result, t); goto _LL151; _LL157: t= Cyc_Tcpat_any_type( topt); if(
region_opt == 0){ Cyc_Tcutil_terr( p->loc,({ unsigned char* _temp239="* pattern would point into an unknown region";
struct _tagged_string _temp240; _temp240.curr= _temp239; _temp240.base= _temp239;
_temp240.last_plus_one= _temp239 + 45; _temp240;})); goto _LL151;} Cyc_Tcpat_set_vd(
_temp186,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp241=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp241[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp242; _temp242.tag= Cyc_Absyn_PointerType;
_temp242.f1=({ struct Cyc_Absyn_PtrInfo _temp243; _temp243.elt_typ=( void*) t;
_temp243.rgn_typ=( void*)*(( void**) _check_null( region_opt)); _temp243.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp243.tq= Cyc_Absyn_empty_tqual();
_temp243.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp243;}); _temp242;}); _temp241;})); goto _LL151; _LL159: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_uint_t); goto _LL151; _LL161: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL151; _LL163: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto
_LL151; _LL165: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL151;
_LL167: t= Cyc_Tcpat_num_type( topt,( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp244=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp244[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp245; _temp245.tag= Cyc_Absyn_EnumType;
_temp245.f1= _temp202->name; _temp245.f2=( struct Cyc_Absyn_Enumdecl*) _temp202;
_temp245;}); _temp244;})); goto _LL151; _LL169: if( topt != 0){ void* _temp246=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp252; _LL248: if(( unsigned int) _temp246 > 4u?*(( int*) _temp246) == Cyc_Absyn_PointerType:
0){ _LL253: _temp252=(( struct Cyc_Absyn_PointerType_struct*) _temp246)->f1;
goto _LL249;} else{ goto _LL250;} _LL250: goto _LL251; _LL249: t=*(( void**)
_check_null( topt)); goto tcpat_end; _LL251: goto _LL247; _LL247:;} t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp254=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp254[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp255; _temp255.tag= Cyc_Absyn_PointerType;
_temp255.f1=({ struct Cyc_Absyn_PtrInfo _temp256; _temp256.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp256.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp256.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp256.tq= Cyc_Absyn_empty_tqual(); _temp256.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp256;}); _temp255;});
_temp254;}); goto _LL151; _LL171: { void* inner_typ=( void*) Cyc_Absyn_VoidType;
void** _temp257= 0; goto _LL258; _LL258: if( topt != 0){ void* _temp259= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp265; struct Cyc_Absyn_Conref*
_temp267; struct Cyc_Absyn_Tqual _temp269; struct Cyc_Absyn_Conref* _temp271;
void* _temp273; void* _temp275; _LL261: if(( unsigned int) _temp259 > 4u?*(( int*)
_temp259) == Cyc_Absyn_PointerType: 0){ _LL266: _temp265=(( struct Cyc_Absyn_PointerType_struct*)
_temp259)->f1; _LL276: _temp275=( void*) _temp265.elt_typ; goto _LL274; _LL274:
_temp273=( void*) _temp265.rgn_typ; goto _LL272; _LL272: _temp271= _temp265.nullable;
goto _LL270; _LL270: _temp269= _temp265.tq; goto _LL268; _LL268: _temp267=
_temp265.bounds; goto _LL262;} else{ goto _LL263;} _LL263: goto _LL264; _LL262:
inner_typ= _temp275; _temp257=( void**)& inner_typ; goto _LL260; _LL264: goto
_LL260; _LL260:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
struct Cyc_List_List* _temp279; struct Cyc_List_List* _temp281; struct _tuple5
_temp277= Cyc_Tcpat_tcPatRec( te, _temp204, _temp257,( void**)& ptr_rgn); _LL282:
_temp281= _temp277.f1; goto _LL280; _LL280: _temp279= _temp277.f2; goto _LL278;
_LL278: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp281); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp279); if( _temp257 == 0){ void* _temp283= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp204->topt))->v); struct Cyc_Absyn_TunionFieldInfo
_temp289; struct Cyc_List_List* _temp291; void* _temp293; struct Cyc_Absyn_Tunionfield*
_temp295; struct Cyc_Absyn_Tuniondecl* _temp297; _LL285: if(( unsigned int)
_temp283 > 4u?*(( int*) _temp283) == Cyc_Absyn_TunionFieldType: 0){ _LL290:
_temp289=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp283)->f1; _LL294:
_temp293=( void*) _temp289.field_info; if(*(( int*) _temp293) == Cyc_Absyn_KnownTunionfield){
_LL298: _temp297=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp293)->f1;
goto _LL296; _LL296: _temp295=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp293)->f2; goto _LL292;} else{ goto _LL287;} _LL292: _temp291= _temp289.targs;
goto _LL286;} else{ goto _LL287;} _LL287: goto _LL288; _LL286: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp299=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp299[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp300; _temp300.tag= Cyc_Absyn_TunionType;
_temp300.f1=({ struct Cyc_Absyn_TunionInfo _temp301; _temp301.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp302=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp302[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp303; _temp303.tag= Cyc_Absyn_KnownTunion;
_temp303.f1= _temp297; _temp303;}); _temp302;})); _temp301.targs= _temp291;
_temp301.rgn=( void*) ptr_rgn; _temp301;}); _temp300;}); _temp299;}); goto
_LL284; _LL288: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp304=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp304[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp305; _temp305.tag= Cyc_Absyn_PointerType;
_temp305.f1=({ struct Cyc_Absyn_PtrInfo _temp306; _temp306.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp204->topt))->v); _temp306.rgn_typ=(
void*) ptr_rgn; _temp306.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp306.tq= Cyc_Absyn_empty_tqual(); _temp306.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp306;}); _temp305;}); _temp304;}); goto _LL284;
_LL284:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp307=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp307[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp308; _temp308.tag= Cyc_Absyn_PointerType;
_temp308.f1=({ struct Cyc_Absyn_PtrInfo _temp309; _temp309.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp204->topt))->v); _temp309.rgn_typ=(
void*) ptr_rgn; _temp309.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp309.tq= Cyc_Absyn_empty_tqual(); _temp309.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp309;}); _temp308;}); _temp307;});} goto _LL151;}}
_LL173: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp310= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_List_List* _temp316; _LL312: if(( unsigned int) _temp310 > 4u?*(( int*)
_temp310) == Cyc_Absyn_TupleType: 0){ _LL317: _temp316=(( struct Cyc_Absyn_TupleType_struct*)
_temp310)->f1; goto _LL313;} else{ goto _LL314;} _LL314: goto _LL315; _LL313:
topt_ts= _temp316; goto _LL311; _LL315: goto _LL311; _LL311:;} for( 0; _temp206
!= 0; _temp206=(( struct Cyc_List_List*) _check_null( _temp206))->tl){ void**
_temp318= 0; goto _LL319; _LL319: if( topt_ts != 0){ _temp318=( void**)&(*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( topt_ts))->hd)).f2;
topt_ts=(( struct Cyc_List_List*) _check_null( topt_ts))->tl;}{ struct Cyc_List_List*
_temp322; struct Cyc_List_List* _temp324; struct _tuple5 _temp320= Cyc_Tcpat_tcPatRec(
te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp206))->hd,
_temp318, region_opt); _LL325: _temp324= _temp320.f1; goto _LL323; _LL323:
_temp322= _temp320.f2; goto _LL321; _LL321: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp324); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp322); pat_ts=({ struct Cyc_List_List*
_temp326=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp326->hd=( void*)({ struct _tuple7* _temp327=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp327->f1= Cyc_Absyn_empty_tqual(); _temp327->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Pat*)(( struct
Cyc_List_List*) _check_null( _temp206))->hd)->topt))->v; _temp327;}); _temp326->tl=
pat_ts; _temp326;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp328=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp328[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp329; _temp329.tag= Cyc_Absyn_TupleType;
_temp329.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp329;}); _temp328;}); goto _LL151;} _LL175: if( _temp210 != 0){(
void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp330="tcPat: struct<...> not implemented"; struct
_tagged_string _temp331; _temp331.curr= _temp330; _temp331.base= _temp330;
_temp331.last_plus_one= _temp330 + 35; _temp331;})));}{ struct _RegionHandle
_temp332= _new_region(); struct _RegionHandle* rgn=& _temp332; _push_region( rgn);{
struct Cyc_List_List* _temp333=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp215->tvs); goto _LL334; _LL334: { struct Cyc_List_List* _temp335=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp333); goto _LL336;
_LL336: t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp337=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp337[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp338; _temp338.tag= Cyc_Absyn_StructType;
_temp338.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp215->name))->v); _temp338.f2= _temp335; _temp338.f3=({ struct
Cyc_Absyn_Structdecl** _temp339=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp339[ 0]= _temp215; _temp339;});
_temp338;}); _temp337;}); if( topt != 0){ Cyc_Tcutil_unify( t,*(( void**)
_check_null( topt)));}* _temp214=({ struct Cyc_Core_Opt* _temp340=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp340->v=( void*) _temp335;
_temp340;}); if( _temp215->fields == 0){ Cyc_Tcutil_terr( p->loc,({
unsigned char* _temp341="can't destructure an abstract struct"; struct
_tagged_string _temp342; _temp342.curr= _temp341; _temp342.base= _temp341;
_temp342.last_plus_one= _temp341 + 37; _temp342;})); _npop_handler( 0u); goto
_LL151;}{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, p->loc,
_temp208,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp215->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple8 _temp345; struct Cyc_Absyn_Pat* _temp346; struct Cyc_Absyn_Structfield*
_temp348; struct _tuple8* _temp343=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp345=* _temp343; _LL349: _temp348= _temp345.f1;
goto _LL347; _LL347: _temp346= _temp345.f2; goto _LL344; _LL344: { void*
_temp350= Cyc_Tcutil_rsubstitute( rgn, _temp333,( void*) _temp348->type); goto
_LL351; _LL351: { struct Cyc_List_List* _temp354; struct Cyc_List_List* _temp356;
struct _tuple5 _temp352= Cyc_Tcpat_tcPatRec( te, _temp346,( void**)& _temp350,
region_opt); _LL357: _temp356= _temp352.f1; goto _LL355; _LL355: _temp354=
_temp352.f2; goto _LL353; _LL353: tv_result=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp356); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp354); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp346->topt))->v, _temp350)){ Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp358=* _temp348->name; struct _tagged_string
_temp359= Cyc_Absynpp_typ2string( _temp350); struct _tagged_string _temp360= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp346->topt))->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp358.last_plus_one - _temp358.curr, _temp358.curr, _temp359.last_plus_one -
_temp359.curr, _temp359.curr, _temp360.last_plus_one - _temp360.curr, _temp360.curr);}));}}}}}}};
_pop_region(& _temp332);} goto _LL151; _LL177: if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp219) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp221->tvs)){ Cyc_Tcutil_terr( p->loc,({ unsigned char* _temp361="wrong number of existential type variables in pattern";
struct _tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 54; _temp362;}));}{ struct _RegionHandle
_temp363= _new_region(); struct _RegionHandle* rgn=& _temp363; _push_region( rgn);{
struct Cyc_List_List* _temp364= 0; goto _LL365; _LL365:{ struct Cyc_List_List* t=
_temp219; for( 0; t != 0; t=(( struct Cyc_List_List*) _check_null( t))->tl){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( t))->hd;{ void* _temp366=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void*
_temp376; void* _temp378; struct Cyc_Absyn_Conref* _temp380; _LL368: if((
unsigned int) _temp366 > 1u?*(( int*) _temp366) == Cyc_Absyn_Eq_constr: 0){
_LL377: _temp376=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp366)->f1;
if( _temp376 ==( void*) Cyc_Absyn_BoxKind){ goto _LL369;} else{ goto _LL370;}}
else{ goto _LL370;} _LL370: if(( unsigned int) _temp366 > 1u?*(( int*) _temp366)
== Cyc_Absyn_Eq_constr: 0){ _LL379: _temp378=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp366)->f1; goto _LL371;} else{ goto _LL372;} _LL372: if( _temp366 ==( void*)
Cyc_Absyn_No_constr){ goto _LL373;} else{ goto _LL374;} _LL374: if((
unsigned int) _temp366 > 1u?*(( int*) _temp366) == Cyc_Absyn_Forward_constr: 0){
_LL381: _temp380=(( struct Cyc_Absyn_Forward_constr_struct*) _temp366)->f1; goto
_LL375;} else{ goto _LL367;} _LL369: goto _LL367; _LL371: Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp382= Cyc_Absynpp_qvar2string( _temp223->name); struct
_tagged_string _temp383=* tv->name; struct _tagged_string _temp384= Cyc_Absynpp_kind2string(
_temp378); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp382.last_plus_one - _temp382.curr, _temp382.curr, _temp383.last_plus_one -
_temp383.curr, _temp383.curr, _temp384.last_plus_one - _temp384.curr, _temp384.curr);}));
goto _LL367; _LL373:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp385=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp385[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp386; _temp386.tag= Cyc_Absyn_Eq_constr;
_temp386.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp386;}); _temp385;})));
goto _LL367; _LL375:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp387="forward after compress in conref"; struct
_tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 33; _temp388;}))); _LL367:;} _temp364=({
struct Cyc_List_List* _temp389=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp389->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp390=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp390[ 0]=({ struct Cyc_Absyn_VarType_struct _temp391; _temp391.tag= Cyc_Absyn_VarType;
_temp391.f1= tv; _temp391;}); _temp390;})); _temp389->tl= _temp364; _temp389;});}}
_temp364=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp364); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp219);{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp219); struct Cyc_List_List* tqts=
_temp221->typs; struct Cyc_List_List* _temp392=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp223->tvs); goto _LL393; _LL393: { struct Cyc_List_List* _temp394=((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp221->tvs, _temp364); goto _LL395; _LL395: { struct Cyc_List_List* _temp396=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp392); goto _LL397;
_LL397: { struct Cyc_List_List* _temp398=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, _temp394); goto _LL399; _LL399: { struct Cyc_List_List*
_temp400=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp392, _temp394); goto
_LL401; _LL401: if( tqts == 0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp402=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp402[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp403; _temp403.tag= Cyc_Absyn_TunionType;
_temp403.f1=({ struct Cyc_Absyn_TunionInfo _temp404; _temp404.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp405=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp405[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp406; _temp406.tag= Cyc_Absyn_KnownTunion;
_temp406.f1= _temp223; _temp406;}); _temp405;})); _temp404.targs= _temp396;
_temp404.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp404;});
_temp403;}); _temp402;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp407=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp407[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp408; _temp408.tag= Cyc_Absyn_TunionFieldType; _temp408.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp409; _temp409.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp410=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp410[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp411; _temp411.tag= Cyc_Absyn_KnownTunionfield; _temp411.f1= _temp223;
_temp411.f2= _temp221; _temp411;}); _temp410;})); _temp409.targs= _temp396;
_temp409;}); _temp408;}); _temp407;});} if( topt != 0? tqts == 0: 0){ void*
_temp412= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo
_temp420; struct Cyc_Absyn_TunionInfo _temp422; void* _temp424; struct Cyc_List_List*
_temp426; void* _temp428; _LL414: if(( unsigned int) _temp412 > 4u?*(( int*)
_temp412) == Cyc_Absyn_TunionFieldType: 0){ _LL421: _temp420=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp412)->f1; goto _LL415;} else{ goto _LL416;} _LL416: if(( unsigned int)
_temp412 > 4u?*(( int*) _temp412) == Cyc_Absyn_TunionType: 0){ _LL423: _temp422=((
struct Cyc_Absyn_TunionType_struct*) _temp412)->f1; _LL429: _temp428=( void*)
_temp422.tunion_info; goto _LL427; _LL427: _temp426= _temp422.targs; goto _LL425;
_LL425: _temp424=( void*) _temp422.rgn; goto _LL417;} else{ goto _LL418;} _LL418:
goto _LL419; _LL415: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp430=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp430[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp431; _temp431.tag= Cyc_Absyn_TunionFieldType; _temp431.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp432; _temp432.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp433=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp433[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp434; _temp434.tag= Cyc_Absyn_KnownTunionfield; _temp434.f1= _temp223;
_temp434.f2= _temp221; _temp434;}); _temp433;})); _temp432.targs= _temp396;
_temp432;}); _temp431;}); _temp430;}); goto _LL413; _LL417: { struct Cyc_List_List*
_temp435= _temp396; goto _LL436; _LL436: for( 0; _temp435 != 0? _temp426 != 0: 0;(
_temp435=(( struct Cyc_List_List*) _check_null( _temp435))->tl, _temp426=((
struct Cyc_List_List*) _check_null( _temp426))->tl)){ Cyc_Tcutil_unify(( void*)((
struct Cyc_List_List*) _check_null( _temp435))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp426))->hd);} goto _LL413;} _LL419: goto _LL413; _LL413:;} for(
0; _temp217 != 0? tqts != 0: 0;( _temp217=(( struct Cyc_List_List*) _check_null(
_temp217))->tl, tqts=(( struct Cyc_List_List*) _check_null( tqts))->tl)){ struct
Cyc_Absyn_Pat* _temp437=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp217))->hd; goto _LL438; _LL438: { void* _temp439= Cyc_Tcutil_rsubstitute(
rgn, _temp400,(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( tqts))->hd)).f2);
goto _LL440; _LL440: { struct Cyc_List_List* _temp443; struct Cyc_List_List*
_temp445; struct _tuple5 _temp441= Cyc_Tcpat_tcPatRec( te2, _temp437,( void**)&
_temp439, region_opt); _LL446: _temp445= _temp441.f1; goto _LL444; _LL444:
_temp443= _temp441.f2; goto _LL442; _LL442: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp445); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp443); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp437->topt))->v, _temp439)){ Cyc_Tcutil_terr(
_temp437->loc,({ struct _tagged_string _temp447= Cyc_Absynpp_qvar2string(
_temp223->name); struct _tagged_string _temp448= Cyc_Absynpp_typ2string(
_temp439); struct _tagged_string _temp449= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp437->topt))->v); xprintf("%.*s expects argument type %.*s, not %.*s",
_temp447.last_plus_one - _temp447.curr, _temp447.curr, _temp448.last_plus_one -
_temp448.curr, _temp448.curr, _temp449.last_plus_one - _temp449.curr, _temp449.curr);}));}}}}
if( _temp217 != 0){ Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp450=
Cyc_Absynpp_qvar2string( _temp223->name); xprintf("too many arguments for tunion constructor %.*s",
_temp450.last_plus_one - _temp450.curr, _temp450.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp451= Cyc_Absynpp_qvar2string( _temp223->name);
xprintf("too few arguments for tunion constructor %.*s", _temp451.last_plus_one
- _temp451.curr, _temp451.curr);}));}}}}}}}; _pop_region(& _temp363);} goto
_LL151; _LL179: goto _LL181; _LL181: goto _LL183; _LL183: t= Cyc_Absyn_wildtyp();
goto _LL151; _LL151:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp452=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp452->v=(
void*) t; _temp452;}); return({ struct _tuple5 _temp453; _temp453.f1= tv_result;
_temp453.f2= v_result; _temp453;});}} struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple5
_temp454= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt); goto _LL455; _LL455:{
struct _RegionHandle _temp456= _new_region(); struct _RegionHandle* r=& _temp456;
_push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name, _temp454.f2), p->loc,({
unsigned char* _temp457="pattern contains a repeated variable"; struct
_tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 37; _temp458;}));; _pop_region(& _temp456);}
return _temp454;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Pat* p){ void* _temp459=( void*) p->r; struct Cyc_Absyn_Pat*
_temp471; struct Cyc_List_List* _temp473; struct Cyc_List_List* _temp475; struct
Cyc_Absyn_Tunionfield* _temp477; struct Cyc_Absyn_Tuniondecl* _temp479; struct
Cyc_List_List* _temp481; struct Cyc_List_List* _temp483; struct Cyc_Core_Opt*
_temp485; struct Cyc_Absyn_Structdecl* _temp487; struct Cyc_List_List* _temp489;
_LL461: if(( unsigned int) _temp459 > 2u?*(( int*) _temp459) == Cyc_Absyn_Pointer_p:
0){ _LL472: _temp471=(( struct Cyc_Absyn_Pointer_p_struct*) _temp459)->f1; goto
_LL462;} else{ goto _LL463;} _LL463: if(( unsigned int) _temp459 > 2u?*(( int*)
_temp459) == Cyc_Absyn_Tunion_p: 0){ _LL480: _temp479=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp459)->f1; goto _LL478; _LL478: _temp477=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp459)->f2; goto _LL476; _LL476: _temp475=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp459)->f3; goto _LL474; _LL474: _temp473=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp459)->f4; goto _LL464;} else{ goto _LL465;} _LL465: if(( unsigned int)
_temp459 > 2u?*(( int*) _temp459) == Cyc_Absyn_Struct_p: 0){ _LL488: _temp487=((
struct Cyc_Absyn_Struct_p_struct*) _temp459)->f1; goto _LL486; _LL486: _temp485=((
struct Cyc_Absyn_Struct_p_struct*) _temp459)->f2; goto _LL484; _LL484: _temp483=((
struct Cyc_Absyn_Struct_p_struct*) _temp459)->f3; goto _LL482; _LL482: _temp481=((
struct Cyc_Absyn_Struct_p_struct*) _temp459)->f4; goto _LL466;} else{ goto
_LL467;} _LL467: if(( unsigned int) _temp459 > 2u?*(( int*) _temp459) == Cyc_Absyn_Tuple_p:
0){ _LL490: _temp489=(( struct Cyc_Absyn_Tuple_p_struct*) _temp459)->f1; goto
_LL468;} else{ goto _LL469;} _LL469: goto _LL470; _LL462: Cyc_Tcpat_check_pat_regions(
te, _temp471);{ void* _temp491=( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v;
struct Cyc_Absyn_PtrInfo _temp499; struct Cyc_Absyn_Conref* _temp501; struct Cyc_Absyn_Tqual
_temp503; struct Cyc_Absyn_Conref* _temp505; void* _temp507; void* _temp509;
struct Cyc_Absyn_TunionInfo _temp511; void* _temp513; struct Cyc_List_List*
_temp515; void* _temp517; _LL493: if(( unsigned int) _temp491 > 4u?*(( int*)
_temp491) == Cyc_Absyn_PointerType: 0){ _LL500: _temp499=(( struct Cyc_Absyn_PointerType_struct*)
_temp491)->f1; _LL510: _temp509=( void*) _temp499.elt_typ; goto _LL508; _LL508:
_temp507=( void*) _temp499.rgn_typ; goto _LL506; _LL506: _temp505= _temp499.nullable;
goto _LL504; _LL504: _temp503= _temp499.tq; goto _LL502; _LL502: _temp501=
_temp499.bounds; goto _LL494;} else{ goto _LL495;} _LL495: if(( unsigned int)
_temp491 > 4u?*(( int*) _temp491) == Cyc_Absyn_TunionType: 0){ _LL512: _temp511=((
struct Cyc_Absyn_TunionType_struct*) _temp491)->f1; _LL518: _temp517=( void*)
_temp511.tunion_info; goto _LL516; _LL516: _temp515= _temp511.targs; goto _LL514;
_LL514: _temp513=( void*) _temp511.rgn; goto _LL496;} else{ goto _LL497;} _LL497:
goto _LL498; _LL494: _temp513= _temp507; goto _LL496; _LL496: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp513); return; _LL498:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp519=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp519[ 0]=({ struct Cyc_Core_Impossible_struct _temp520; _temp520.tag= Cyc_Core_Impossible;
_temp520.f1=({ unsigned char* _temp521="check_pat_regions: bad pointer type";
struct _tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 36; _temp522;}); _temp520;}); _temp519;}));
_LL492:;} _LL464: for( 0; _temp473 != 0; _temp473=(( struct Cyc_List_List*)
_check_null( _temp473))->tl){ Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp473))->hd);}{ void* _temp523=( void*)((
struct Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo
_temp531; void* _temp533; struct Cyc_List_List* _temp535; void* _temp537; struct
Cyc_Absyn_TunionFieldInfo _temp539; _LL525: if(( unsigned int) _temp523 > 4u?*((
int*) _temp523) == Cyc_Absyn_TunionType: 0){ _LL532: _temp531=(( struct Cyc_Absyn_TunionType_struct*)
_temp523)->f1; _LL538: _temp537=( void*) _temp531.tunion_info; goto _LL536;
_LL536: _temp535= _temp531.targs; goto _LL534; _LL534: _temp533=( void*)
_temp531.rgn; goto _LL526;} else{ goto _LL527;} _LL527: if(( unsigned int)
_temp523 > 4u?*(( int*) _temp523) == Cyc_Absyn_TunionFieldType: 0){ _LL540:
_temp539=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp523)->f1; goto _LL528;}
else{ goto _LL529;} _LL529: goto _LL530; _LL526: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp533); return; _LL528: return; _LL530:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp541=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp541[ 0]=({ struct
Cyc_Core_Impossible_struct _temp542; _temp542.tag= Cyc_Core_Impossible; _temp542.f1=({
unsigned char* _temp543="check_pat_regions: bad tunion type"; struct
_tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 35; _temp544;}); _temp542;}); _temp541;}));
_LL524:;} _LL466: _temp489=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp481); goto _LL468; _LL468: for( 0; _temp489
!= 0; _temp489=(( struct Cyc_List_List*) _check_null( _temp489))->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp489))->hd);}
return; _LL470: return; _LL460:;} static const int Cyc_Tcpat_Name_v= 0; struct
Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_string f1; } ; static const int
Cyc_Tcpat_Int_v= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; struct
Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ; static
const int Cyc_Tcpat_Any= 0; static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static int
Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){
void* _temp545=( void*) c1->name; struct _tagged_string _temp551; int _temp553;
_LL547: if(*(( int*) _temp545) == Cyc_Tcpat_Name_v){ _LL552: _temp551=(( struct
Cyc_Tcpat_Name_v_struct*) _temp545)->f1; goto _LL548;} else{ goto _LL549;}
_LL549: if(*(( int*) _temp545) == Cyc_Tcpat_Int_v){ _LL554: _temp553=(( struct
Cyc_Tcpat_Int_v_struct*) _temp545)->f1; goto _LL550;} else{ goto _LL546;} _LL548: {
void* _temp555=( void*) c2->name; struct _tagged_string _temp561; int _temp563;
_LL557: if(*(( int*) _temp555) == Cyc_Tcpat_Name_v){ _LL562: _temp561=(( struct
Cyc_Tcpat_Name_v_struct*) _temp555)->f1; goto _LL558;} else{ goto _LL559;}
_LL559: if(*(( int*) _temp555) == Cyc_Tcpat_Int_v){ _LL564: _temp563=(( struct
Cyc_Tcpat_Int_v_struct*) _temp555)->f1; goto _LL560;} else{ goto _LL556;} _LL558:
return Cyc_String_zstrcmp( _temp551, _temp561); _LL560: return - 1; _LL556:;}
_LL550: { void* _temp565=( void*) c2->name; struct _tagged_string _temp571; int
_temp573; _LL567: if(*(( int*) _temp565) == Cyc_Tcpat_Name_v){ _LL572: _temp571=((
struct Cyc_Tcpat_Name_v_struct*) _temp565)->f1; goto _LL568;} else{ goto _LL569;}
_LL569: if(*(( int*) _temp565) == Cyc_Tcpat_Int_v){ _LL574: _temp573=(( struct
Cyc_Tcpat_Int_v_struct*) _temp565)->f1; goto _LL570;} else{ goto _LL566;} _LL568:
return 1; _LL570: return _temp553 - _temp573; _LL566:;} _LL546:;} static struct
Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)( int(*
comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)( Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={( void*) 1}; static struct Cyc_Core_Opt
Cyc_Tcpat_two_opt={( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static unsigned char _temp578[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,{ _temp578,
_temp578, _temp578 + 5u}}; static unsigned char _temp582[ 4u]="ptr"; static
struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,{ _temp582,
_temp582, _temp582 + 4u}}; static unsigned char _temp586[ 6u]="tuple"; static
struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp586,
_temp586, _temp586 + 6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp587=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp587->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp588=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp588[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp589; _temp589.tag= Cyc_Tcpat_Int_v;
_temp589.f1= i; _temp589;}); _temp588;})); _temp587->arity= 0; _temp587->span= 0;
_temp587;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp590=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp590->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp591=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp591[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp592; _temp592.tag= Cyc_Tcpat_Name_v; _temp592.f1= f; _temp592;}); _temp591;}));
_temp590->arity= 0; _temp590->span= 0; _temp590;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp593=(
struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp593->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp595=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp595[ 0]=({ struct Cyc_Tcpat_Int_v_struct
_temp596; _temp596.tag= Cyc_Tcpat_Int_v; _temp596.f1=( int) c; _temp596;});
_temp595;})); _temp593->arity= 0; _temp593->span=({ struct Cyc_Core_Opt*
_temp594=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp594->v=( void*) 256; _temp594;}); _temp593;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s* _temp597=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp597->name=(
void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp597->arity= i; _temp597->span=
Cyc_Tcpat_one_opt_ptr; _temp597;});} static void* Cyc_Tcpat_null_pat(){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp598=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp598[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp599; _temp599.tag= Cyc_Tcpat_Con; _temp599.f1= Cyc_Tcpat_null_con; _temp599.f2=
0; _temp599;}); _temp598;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp600=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp600[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp601; _temp601.tag= Cyc_Tcpat_Con; _temp601.f1= Cyc_Tcpat_int_con( i);
_temp601.f2= 0; _temp601;}); _temp600;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp602=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp602[ 0]=({ struct Cyc_Tcpat_Con_struct _temp603; _temp603.tag= Cyc_Tcpat_Con;
_temp603.f1= Cyc_Tcpat_char_con( c); _temp603.f2= 0; _temp603;}); _temp602;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_string f){ return( void*)({
struct Cyc_Tcpat_Con_struct* _temp604=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp604[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp605; _temp605.tag= Cyc_Tcpat_Con; _temp605.f1= Cyc_Tcpat_float_con( f);
_temp605.f2= 0; _temp605;}); _temp604;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp606=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp606[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp607; _temp607.tag= Cyc_Tcpat_Con; _temp607.f1= Cyc_Tcpat_null_ptr_con;
_temp607.f2=({ struct Cyc_List_List* _temp608=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp608->hd=( void*) p; _temp608->tl= 0;
_temp608;}); _temp607;}); _temp606;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp609=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp609[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp610; _temp610.tag= Cyc_Tcpat_Con; _temp610.f1= Cyc_Tcpat_ptr_con; _temp610.f2=({
struct Cyc_List_List* _temp611=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp611->hd=( void*) p; _temp611->tl= 0; _temp611;});
_temp610;}); _temp609;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp612=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp612[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp613; _temp613.tag= Cyc_Tcpat_Con; _temp613.f1= Cyc_Tcpat_tuple_con((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ss)); _temp613.f2= ss; _temp613;});
_temp612;});} static void* Cyc_Tcpat_con_pat( struct _tagged_string con_name,
struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){ struct Cyc_Tcpat_Con_s* c=({
struct Cyc_Tcpat_Con_s* _temp616=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp616->name=( void*)(( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp617=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp617[ 0]=({ struct Cyc_Tcpat_Name_v_struct _temp618; _temp618.tag= Cyc_Tcpat_Name_v;
_temp618.f1= con_name; _temp618;}); _temp617;})); _temp616->arity=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ps); _temp616->span= span; _temp616;});
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp614=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp614[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp615; _temp615.tag= Cyc_Tcpat_Con; _temp615.f1= c; _temp615.f2= ps; _temp615;});
_temp614;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp619=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp653; int
_temp655; void* _temp657; unsigned char _temp659; struct _tagged_string _temp661;
struct Cyc_Absyn_Vardecl* _temp663; struct Cyc_Absyn_Pat* _temp665; struct Cyc_List_List*
_temp667; struct Cyc_List_List* _temp669; struct Cyc_Absyn_Tunionfield* _temp671;
struct Cyc_Absyn_Tuniondecl* _temp673; struct Cyc_List_List* _temp675; struct
Cyc_List_List* _temp677; struct Cyc_Absyn_Tunionfield* _temp679; struct Cyc_Absyn_Tuniondecl*
_temp681; struct Cyc_List_List* _temp683; struct Cyc_List_List* _temp685; struct
Cyc_List_List* _temp687; struct Cyc_Core_Opt* _temp689; struct Cyc_Absyn_Structdecl*
_temp691; struct Cyc_Absyn_Enumfield* _temp693; struct Cyc_Absyn_Enumdecl*
_temp695; struct _tuple1* _temp697; struct Cyc_List_List* _temp699; struct Cyc_List_List*
_temp701; struct _tuple1* _temp703; struct Cyc_List_List* _temp705; struct Cyc_List_List*
_temp707; struct _tuple1* _temp709; _LL621: if( _temp619 ==( void*) Cyc_Absyn_Wild_p){
goto _LL622;} else{ goto _LL623;} _LL623: if(( unsigned int) _temp619 > 2u?*((
int*) _temp619) == Cyc_Absyn_Var_p: 0){ _LL654: _temp653=(( struct Cyc_Absyn_Var_p_struct*)
_temp619)->f1; goto _LL624;} else{ goto _LL625;} _LL625: if( _temp619 ==( void*)
Cyc_Absyn_Null_p){ goto _LL626;} else{ goto _LL627;} _LL627: if(( unsigned int)
_temp619 > 2u?*(( int*) _temp619) == Cyc_Absyn_Int_p: 0){ _LL658: _temp657=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp619)->f1; goto _LL656; _LL656:
_temp655=(( struct Cyc_Absyn_Int_p_struct*) _temp619)->f2; goto _LL628;} else{
goto _LL629;} _LL629: if(( unsigned int) _temp619 > 2u?*(( int*) _temp619) ==
Cyc_Absyn_Char_p: 0){ _LL660: _temp659=(( struct Cyc_Absyn_Char_p_struct*)
_temp619)->f1; goto _LL630;} else{ goto _LL631;} _LL631: if(( unsigned int)
_temp619 > 2u?*(( int*) _temp619) == Cyc_Absyn_Float_p: 0){ _LL662: _temp661=((
struct Cyc_Absyn_Float_p_struct*) _temp619)->f1; goto _LL632;} else{ goto _LL633;}
_LL633: if(( unsigned int) _temp619 > 2u?*(( int*) _temp619) == Cyc_Absyn_Reference_p:
0){ _LL664: _temp663=(( struct Cyc_Absyn_Reference_p_struct*) _temp619)->f1;
goto _LL634;} else{ goto _LL635;} _LL635: if(( unsigned int) _temp619 > 2u?*((
int*) _temp619) == Cyc_Absyn_Pointer_p: 0){ _LL666: _temp665=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp619)->f1; goto _LL636;} else{ goto _LL637;} _LL637: if(( unsigned int)
_temp619 > 2u?*(( int*) _temp619) == Cyc_Absyn_Tunion_p: 0){ _LL674: _temp673=((
struct Cyc_Absyn_Tunion_p_struct*) _temp619)->f1; goto _LL672; _LL672: _temp671=((
struct Cyc_Absyn_Tunion_p_struct*) _temp619)->f2; goto _LL670; _LL670: _temp669=((
struct Cyc_Absyn_Tunion_p_struct*) _temp619)->f3; goto _LL668; _LL668: _temp667=((
struct Cyc_Absyn_Tunion_p_struct*) _temp619)->f4; if( _temp667 == 0){ goto
_LL638;} else{ goto _LL639;}} else{ goto _LL639;} _LL639: if(( unsigned int)
_temp619 > 2u?*(( int*) _temp619) == Cyc_Absyn_Tunion_p: 0){ _LL682: _temp681=((
struct Cyc_Absyn_Tunion_p_struct*) _temp619)->f1; goto _LL680; _LL680: _temp679=((
struct Cyc_Absyn_Tunion_p_struct*) _temp619)->f2; goto _LL678; _LL678: _temp677=((
struct Cyc_Absyn_Tunion_p_struct*) _temp619)->f3; goto _LL676; _LL676: _temp675=((
struct Cyc_Absyn_Tunion_p_struct*) _temp619)->f4; goto _LL640;} else{ goto
_LL641;} _LL641: if(( unsigned int) _temp619 > 2u?*(( int*) _temp619) == Cyc_Absyn_Tuple_p:
0){ _LL684: _temp683=(( struct Cyc_Absyn_Tuple_p_struct*) _temp619)->f1; goto
_LL642;} else{ goto _LL643;} _LL643: if(( unsigned int) _temp619 > 2u?*(( int*)
_temp619) == Cyc_Absyn_Struct_p: 0){ _LL692: _temp691=(( struct Cyc_Absyn_Struct_p_struct*)
_temp619)->f1; goto _LL690; _LL690: _temp689=(( struct Cyc_Absyn_Struct_p_struct*)
_temp619)->f2; goto _LL688; _LL688: _temp687=(( struct Cyc_Absyn_Struct_p_struct*)
_temp619)->f3; goto _LL686; _LL686: _temp685=(( struct Cyc_Absyn_Struct_p_struct*)
_temp619)->f4; goto _LL644;} else{ goto _LL645;} _LL645: if(( unsigned int)
_temp619 > 2u?*(( int*) _temp619) == Cyc_Absyn_Enum_p: 0){ _LL696: _temp695=((
struct Cyc_Absyn_Enum_p_struct*) _temp619)->f1; goto _LL694; _LL694: _temp693=((
struct Cyc_Absyn_Enum_p_struct*) _temp619)->f2; goto _LL646;} else{ goto _LL647;}
_LL647: if(( unsigned int) _temp619 > 2u?*(( int*) _temp619) == Cyc_Absyn_UnknownId_p:
0){ _LL698: _temp697=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp619)->f1;
goto _LL648;} else{ goto _LL649;} _LL649: if(( unsigned int) _temp619 > 2u?*((
int*) _temp619) == Cyc_Absyn_UnknownCall_p: 0){ _LL704: _temp703=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp619)->f1; goto _LL702; _LL702: _temp701=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp619)->f2; goto _LL700; _LL700: _temp699=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp619)->f3; goto _LL650;} else{ goto _LL651;} _LL651: if(( unsigned int)
_temp619 > 2u?*(( int*) _temp619) == Cyc_Absyn_UnknownFields_p: 0){ _LL710:
_temp709=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp619)->f1; goto _LL708;
_LL708: _temp707=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp619)->f2;
goto _LL706; _LL706: _temp705=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp619)->f3; goto _LL652;} else{ goto _LL620;} _LL622: s=( void*) Cyc_Tcpat_Any;
goto _LL620; _LL624: s=( void*) Cyc_Tcpat_Any; goto _LL620; _LL626: s= Cyc_Tcpat_null_pat();
goto _LL620; _LL628: s= Cyc_Tcpat_int_pat( _temp655); goto _LL620; _LL630: s=
Cyc_Tcpat_char_pat( _temp659); goto _LL620; _LL632: s= Cyc_Tcpat_float_pat(
_temp661); goto _LL620; _LL634: s=( void*) Cyc_Tcpat_Any; goto _LL620; _LL636:{
void* _temp711= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
p->topt))->v); struct Cyc_Absyn_PtrInfo _temp719; struct Cyc_Absyn_Conref*
_temp721; struct Cyc_Absyn_Tqual _temp723; struct Cyc_Absyn_Conref* _temp725;
void* _temp727; void* _temp729; struct Cyc_Absyn_TunionInfo _temp731; _LL713:
if(( unsigned int) _temp711 > 4u?*(( int*) _temp711) == Cyc_Absyn_PointerType: 0){
_LL720: _temp719=(( struct Cyc_Absyn_PointerType_struct*) _temp711)->f1; _LL730:
_temp729=( void*) _temp719.elt_typ; goto _LL728; _LL728: _temp727=( void*)
_temp719.rgn_typ; goto _LL726; _LL726: _temp725= _temp719.nullable; goto _LL724;
_LL724: _temp723= _temp719.tq; goto _LL722; _LL722: _temp721= _temp719.bounds;
goto _LL714;} else{ goto _LL715;} _LL715: if(( unsigned int) _temp711 > 4u?*((
int*) _temp711) == Cyc_Absyn_TunionType: 0){ _LL732: _temp731=(( struct Cyc_Absyn_TunionType_struct*)
_temp711)->f1; goto _LL716;} else{ goto _LL717;} _LL717: goto _LL718; _LL714: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp733=(
void*) _temp725->v; struct Cyc_Absyn_Conref* _temp741; int _temp743; _LL735: if((
unsigned int) _temp733 > 1u?*(( int*) _temp733) == Cyc_Absyn_Forward_constr: 0){
_LL742: _temp741=(( struct Cyc_Absyn_Forward_constr_struct*) _temp733)->f1; goto
_LL736;} else{ goto _LL737;} _LL737: if( _temp733 ==( void*) Cyc_Absyn_No_constr){
goto _LL738;} else{ goto _LL739;} _LL739: if(( unsigned int) _temp733 > 1u?*((
int*) _temp733) == Cyc_Absyn_Eq_constr: 0){ _LL744: _temp743=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp733)->f1; goto _LL740;} else{ goto _LL734;} _LL736:( void*)( _temp725->v=(
void*)(( void*) _temp741->v)); continue; _LL738:( void*)( _temp725->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp745=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp745[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp746; _temp746.tag= Cyc_Absyn_Eq_constr; _temp746.f1=(
void*) 0; _temp746;}); _temp745;}))); is_nullable= 0; still_working= 0; goto
_LL734; _LL740: is_nullable=( int) _temp743; still_working= 0; goto _LL734;
_LL734:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp665); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL712;}} _LL716:{ void* _temp747=(
void*) _temp665->r; struct Cyc_List_List* _temp753; struct Cyc_List_List*
_temp755; struct Cyc_Absyn_Tunionfield* _temp757; struct Cyc_Absyn_Tuniondecl*
_temp759; _LL749: if(( unsigned int) _temp747 > 2u?*(( int*) _temp747) == Cyc_Absyn_Tunion_p:
0){ _LL760: _temp759=(( struct Cyc_Absyn_Tunion_p_struct*) _temp747)->f1; goto
_LL758; _LL758: _temp757=(( struct Cyc_Absyn_Tunion_p_struct*) _temp747)->f2;
goto _LL756; _LL756: _temp755=(( struct Cyc_Absyn_Tunion_p_struct*) _temp747)->f3;
goto _LL754; _LL754: _temp753=(( struct Cyc_Absyn_Tunion_p_struct*) _temp747)->f4;
goto _LL750;} else{ goto _LL751;} _LL751: goto _LL752; _LL750: { struct Cyc_Core_Opt*
span; if( _temp759->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp761=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp761->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp759->fields))->v);
_temp761;});} s= Cyc_Tcpat_con_pat(*(* _temp757->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp753)); goto _LL748;} _LL752: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp762="non-[x]tunion pattern has tunion type"; struct
_tagged_string _temp763; _temp763.curr= _temp762; _temp763.base= _temp762;
_temp763.last_plus_one= _temp762 + 38; _temp763;})); goto _LL748; _LL748:;} goto
_LL712; _LL718: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp764="pointer pattern does not have pointer type!"; struct
_tagged_string _temp765; _temp765.curr= _temp764; _temp765.base= _temp764;
_temp765.last_plus_one= _temp764 + 44; _temp765;})); goto _LL712; _LL712:;} goto
_LL620; _LL638: { struct Cyc_Core_Opt* span;{ void* _temp766= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v); struct Cyc_Absyn_TunionInfo
_temp774; struct Cyc_Absyn_TunionFieldInfo _temp776; _LL768: if(( unsigned int)
_temp766 > 4u?*(( int*) _temp766) == Cyc_Absyn_TunionType: 0){ _LL775: _temp774=((
struct Cyc_Absyn_TunionType_struct*) _temp766)->f1; goto _LL769;} else{ goto
_LL770;} _LL770: if(( unsigned int) _temp766 > 4u?*(( int*) _temp766) == Cyc_Absyn_TunionFieldType:
0){ _LL777: _temp776=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp766)->f1;
goto _LL771;} else{ goto _LL772;} _LL772: goto _LL773; _LL769: if( _temp673->is_xtunion){
span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp778=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp778->v=( void*)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp673->fields))->v); _temp778;});} goto _LL767; _LL771: span=({
struct Cyc_Core_Opt* _temp779=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp779->v=( void*) 1; _temp779;}); goto _LL767; _LL773:
span=(( struct Cyc_Core_Opt*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp780="void tunion pattern has bad type"; struct
_tagged_string _temp781; _temp781.curr= _temp780; _temp781.base= _temp780;
_temp781.last_plus_one= _temp780 + 33; _temp781;})); goto _LL767; _LL767:;} s=
Cyc_Tcpat_con_pat(*(* _temp671->name).f2, span, 0); goto _LL620;} _LL640:
_temp683= _temp675; goto _LL642; _LL642: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp683)); goto _LL620; _LL644: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp691->fields))->v;
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fields))->hd)->name,({ unsigned char* _temp807=""; struct
_tagged_string _temp808; _temp808.curr= _temp807; _temp808.base= _temp807;
_temp808.last_plus_one= _temp807 + 1; _temp808;})) == 0;{ struct Cyc_List_List*
dlps0= _temp685; for( 0; ! found? dlps0 != 0: 0; dlps0=(( struct Cyc_List_List*)
_check_null( dlps0))->tl){ struct _tuple6 _temp784; struct Cyc_Absyn_Pat*
_temp785; struct Cyc_List_List* _temp787; struct _tuple6* _temp782=( struct
_tuple6*)(( struct Cyc_List_List*) _check_null( dlps0))->hd; _temp784=* _temp782;
_LL788: _temp787= _temp784.f1; goto _LL786; _LL786: _temp785= _temp784.f2; goto
_LL783; _LL783: { struct Cyc_List_List* _temp789= _temp787; struct Cyc_List_List
_temp795; struct Cyc_List_List* _temp796; void* _temp798; struct _tagged_string*
_temp800; _LL791: if( _temp789 == 0){ goto _LL793;} else{ _temp795=* _temp789;
_LL799: _temp798=( void*) _temp795.hd; if(*(( int*) _temp798) == Cyc_Absyn_FieldName){
_LL801: _temp800=(( struct Cyc_Absyn_FieldName_struct*) _temp798)->f1; goto
_LL797;} else{ goto _LL793;} _LL797: _temp796= _temp795.tl; if( _temp796 == 0){
goto _LL792;} else{ goto _LL793;}} _LL793: goto _LL794; _LL792: if( Cyc_String_zstrptrcmp(
_temp800,(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd)->name) == 0){ ps=({ struct Cyc_List_List* _temp802=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp802->hd=( void*) Cyc_Tcpat_compile_pat(
_temp785); _temp802->tl= ps; _temp802;}); found= 1;} goto _LL790; _LL794:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp803="bad designator(s)";
struct _tagged_string _temp804; _temp804.curr= _temp803; _temp804.base= _temp803;
_temp804.last_plus_one= _temp803 + 18; _temp804;})); goto _LL790; _LL790:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp805="bad designator"; struct _tagged_string _temp806;
_temp806.curr= _temp805; _temp806.base= _temp805; _temp806.last_plus_one=
_temp805 + 15; _temp806;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL620;}
_LL646: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp695->fields))->v);
s= Cyc_Tcpat_con_pat(*(* _temp693->name).f2,({ struct Cyc_Core_Opt* _temp809=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp809->v=(
void*) span; _temp809;}), 0); goto _LL620;} _LL648: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp810="compile_pat: unknownid";
struct _tagged_string _temp811; _temp811.curr= _temp810; _temp811.base= _temp810;
_temp811.last_plus_one= _temp810 + 23; _temp811;})); goto _LL620; _LL650: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp812="compile_pat: unknowncall";
struct _tagged_string _temp813; _temp813.curr= _temp812; _temp813.base= _temp812;
_temp813.last_plus_one= _temp812 + 25; _temp813;})); goto _LL620; _LL652: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp814="compile_pat: unknownfields";
struct _tagged_string _temp815; _temp815.curr= _temp814; _temp815.base= _temp814;
_temp815.last_plus_one= _temp814 + 27; _temp815;})); goto _LL620; _LL620:;}
return s;} static const int Cyc_Tcpat_Pos= 0; struct Cyc_Tcpat_Pos_struct{ int
tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag; struct Cyc_Set_Set* f1;
} ; static const int Cyc_Tcpat_Failure= 0; static const int Cyc_Tcpat_Success= 0;
struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s*
f2; void* f3; void* f4; } ; struct _tuple9{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static const int Cyc_Tcpat_Yes= 0; static
const int Cyc_Tcpat_No= 1; static const int Cyc_Tcpat_Maybe= 2; static void* Cyc_Tcpat_add_neg(
void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp816= td; struct Cyc_Set_Set*
_temp822; struct Cyc_List_List* _temp824; struct Cyc_Tcpat_Con_s* _temp826;
_LL818: if(*(( int*) _temp816) == Cyc_Tcpat_Neg){ _LL823: _temp822=(( struct Cyc_Tcpat_Neg_struct*)
_temp816)->f1; goto _LL819;} else{ goto _LL820;} _LL820: if(*(( int*) _temp816)
== Cyc_Tcpat_Pos){ _LL827: _temp826=(( struct Cyc_Tcpat_Pos_struct*) _temp816)->f1;
goto _LL825; _LL825: _temp824=(( struct Cyc_Tcpat_Pos_struct*) _temp816)->f2;
goto _LL821;} else{ goto _LL817;} _LL819: if((( int(*)( struct Cyc_Set_Set* s,
struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp822, c)){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp828="add_neg called when constructor already in set";
struct _tagged_string _temp829; _temp829.curr= _temp828; _temp829.base= _temp828;
_temp829.last_plus_one= _temp828 + 47; _temp829;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp822) + 1 >=( int)(( struct
Cyc_Core_Opt*) _check_null( c->span))->v: 0){(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp830="add_neg called when |cs U {c}| >= span(c)";
struct _tagged_string _temp831; _temp831.curr= _temp830; _temp831.base= _temp830;
_temp831.last_plus_one= _temp830 + 42; _temp831;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp832=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp832[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp833; _temp833.tag= Cyc_Tcpat_Neg; _temp833.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp822,
c); _temp833;}); _temp832;}); _LL821: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp834="add_neg called when td is Positive";
struct _tagged_string _temp835; _temp835.curr= _temp834; _temp835.base= _temp834;
_temp835.last_plus_one= _temp834 + 35; _temp835;})); _LL817:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp836= td; struct Cyc_List_List*
_temp842; struct Cyc_Tcpat_Con_s* _temp844; struct Cyc_Set_Set* _temp846; _LL838:
if(*(( int*) _temp836) == Cyc_Tcpat_Pos){ _LL845: _temp844=(( struct Cyc_Tcpat_Pos_struct*)
_temp836)->f1; goto _LL843; _LL843: _temp842=(( struct Cyc_Tcpat_Pos_struct*)
_temp836)->f2; goto _LL839;} else{ goto _LL840;} _LL840: if(*(( int*) _temp836)
== Cyc_Tcpat_Neg){ _LL847: _temp846=(( struct Cyc_Tcpat_Neg_struct*) _temp836)->f1;
goto _LL841;} else{ goto _LL837;} _LL839: if( Cyc_Tcpat_compare_con( c, _temp844)
== 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL841:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp846, c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)((
struct Cyc_Core_Opt*) _check_null( c->span))->v ==(( int(*)( struct Cyc_Set_Set*
s)) Cyc_Set_cardinality)( _temp846) + 1: 0){ return( void*) Cyc_Tcpat_Yes;}
else{ return( void*) Cyc_Tcpat_Maybe;}} _LL837:;} struct _tuple10{ struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; static struct Cyc_List_List* Cyc_Tcpat_augment(
struct Cyc_List_List* ctxt, void* dsc){ struct Cyc_List_List* _temp848= ctxt;
struct Cyc_List_List _temp854; struct Cyc_List_List* _temp855; struct _tuple10*
_temp857; struct _tuple10 _temp859; struct Cyc_List_List* _temp860; struct Cyc_Tcpat_Con_s*
_temp862; _LL850: if( _temp848 == 0){ goto _LL851;} else{ goto _LL852;} _LL852:
if( _temp848 == 0){ goto _LL849;} else{ _temp854=* _temp848; _LL858: _temp857=(
struct _tuple10*) _temp854.hd; _temp859=* _temp857; _LL863: _temp862= _temp859.f1;
goto _LL861; _LL861: _temp860= _temp859.f2; goto _LL856; _LL856: _temp855=
_temp854.tl; goto _LL853;} _LL851: return 0; _LL853: return({ struct Cyc_List_List*
_temp864=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp864->hd=( void*)({ struct _tuple10* _temp865=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp865->f1= _temp862; _temp865->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp866=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp866->hd=( void*) dsc; _temp866->tl= _temp860;
_temp866;}); _temp865;}); _temp864->tl= _temp855; _temp864;}); _LL849:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp867= ctxt; struct Cyc_List_List _temp873; struct Cyc_List_List*
_temp874; struct _tuple10* _temp876; struct _tuple10 _temp878; struct Cyc_List_List*
_temp879; struct Cyc_Tcpat_Con_s* _temp881; _LL869: if( _temp867 == 0){ goto
_LL870;} else{ goto _LL871;} _LL871: if( _temp867 == 0){ goto _LL868;} else{
_temp873=* _temp867; _LL877: _temp876=( struct _tuple10*) _temp873.hd; _temp878=*
_temp876; _LL882: _temp881= _temp878.f1; goto _LL880; _LL880: _temp879= _temp878.f2;
goto _LL875; _LL875: _temp874= _temp873.tl; goto _LL872;} _LL870: return((
struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp883="norm_context: empty context"; struct _tagged_string
_temp884; _temp884.curr= _temp883; _temp884.base= _temp883; _temp884.last_plus_one=
_temp883 + 28; _temp884;})); _LL872: return Cyc_Tcpat_augment( _temp874,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp885=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp885[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp886; _temp886.tag= Cyc_Tcpat_Pos; _temp886.f1= _temp881; _temp886.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp879);
_temp886;}); _temp885;})); _LL868:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp888=({ struct _tuple5 _temp887; _temp887.f1= ctxt; _temp887.f2= work;
_temp887;}); struct Cyc_List_List* _temp898; struct Cyc_List_List* _temp900;
struct Cyc_List_List* _temp902; struct Cyc_List_List* _temp904; struct Cyc_List_List*
_temp906; struct Cyc_List_List* _temp908; struct Cyc_List_List* _temp910; struct
Cyc_List_List _temp912; struct Cyc_List_List* _temp913; struct _tuple9* _temp915;
struct _tuple9 _temp917; struct Cyc_List_List* _temp918; struct Cyc_List_List*
_temp920; struct Cyc_List_List* _temp922; struct Cyc_List_List* _temp924; struct
Cyc_List_List _temp926; struct Cyc_List_List* _temp927; struct _tuple10*
_temp929; struct _tuple10 _temp931; struct Cyc_List_List* _temp932; struct Cyc_Tcpat_Con_s*
_temp934; _LL890: _LL901: _temp900= _temp888.f1; if( _temp900 == 0){ goto _LL899;}
else{ goto _LL892;} _LL899: _temp898= _temp888.f2; if( _temp898 == 0){ goto
_LL891;} else{ goto _LL892;} _LL892: _LL905: _temp904= _temp888.f1; if( _temp904
== 0){ goto _LL903;} else{ goto _LL894;} _LL903: _temp902= _temp888.f2; goto
_LL893; _LL894: _LL909: _temp908= _temp888.f1; goto _LL907; _LL907: _temp906=
_temp888.f2; if( _temp906 == 0){ goto _LL895;} else{ goto _LL896;} _LL896:
_LL925: _temp924= _temp888.f1; if( _temp924 == 0){ goto _LL889;} else{ _temp926=*
_temp924; _LL930: _temp929=( struct _tuple10*) _temp926.hd; _temp931=* _temp929;
_LL935: _temp934= _temp931.f1; goto _LL933; _LL933: _temp932= _temp931.f2; goto
_LL928; _LL928: _temp927= _temp926.tl; goto _LL911;} _LL911: _temp910= _temp888.f2;
if( _temp910 == 0){ goto _LL889;} else{ _temp912=* _temp910; _LL916: _temp915=(
struct _tuple9*) _temp912.hd; _temp917=* _temp915; _LL923: _temp922= _temp917.f1;
goto _LL921; _LL921: _temp920= _temp917.f2; goto _LL919; _LL919: _temp918=
_temp917.f3; goto _LL914; _LL914: _temp913= _temp912.tl; goto _LL897;} _LL891:
return dsc; _LL893: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp936="build_desc: ctxt and work don't match"; struct
_tagged_string _temp937; _temp937.curr= _temp936; _temp937.base= _temp936;
_temp937.last_plus_one= _temp936 + 38; _temp937;})); _LL895: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp938="build_desc: ctxt and work don't match";
struct _tagged_string _temp939; _temp939.curr= _temp938; _temp939.base= _temp938;
_temp939.last_plus_one= _temp938 + 38; _temp939;})); _LL897: { struct Cyc_Tcpat_Pos_struct*
_temp943=({ struct Cyc_Tcpat_Pos_struct* _temp940=( struct Cyc_Tcpat_Pos_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Pos_struct)); _temp940[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp941; _temp941.tag= Cyc_Tcpat_Pos; _temp941.f1= _temp934; _temp941.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp932),({
struct Cyc_List_List* _temp942=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp942->hd=( void*) dsc; _temp942->tl= _temp918;
_temp942;})); _temp941;}); _temp940;}); goto _LL944; _LL944: return Cyc_Tcpat_build_desc(
_temp927,( void*) _temp943, _temp913);} _LL889:;} static void* Cyc_Tcpat_match(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules); static
void* Cyc_Tcpat_or_match( void* dsc, struct Cyc_List_List* allmrules){ struct
Cyc_List_List* _temp945= allmrules; struct Cyc_List_List _temp951; struct Cyc_List_List*
_temp952; struct _tuple0* _temp954; struct _tuple0 _temp956; void* _temp957;
void* _temp959; _LL947: if( _temp945 == 0){ goto _LL948;} else{ goto _LL949;}
_LL949: if( _temp945 == 0){ goto _LL946;} else{ _temp951=* _temp945; _LL955:
_temp954=( struct _tuple0*) _temp951.hd; _temp956=* _temp954; _LL960: _temp959=
_temp956.f1; goto _LL958; _LL958: _temp957= _temp956.f2; goto _LL953; _LL953:
_temp952= _temp951.tl; goto _LL950;} _LL948: return( void*) Cyc_Tcpat_Failure;
_LL950: return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp959, 0, dsc, 0, 0, _temp957,
_temp952); _LL946:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List*
allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp961=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp961[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp962; _temp962.tag= Cyc_Tcpat_Neg; _temp962.f1= Cyc_Tcpat_empty_con_set();
_temp962;}); _temp961;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp963= work; struct Cyc_List_List
_temp971; struct Cyc_List_List* _temp972; struct _tuple9* _temp974; struct
_tuple9 _temp976; struct Cyc_List_List* _temp977; struct Cyc_List_List* _temp979;
struct Cyc_List_List* _temp981; struct Cyc_List_List _temp983; struct Cyc_List_List*
_temp984; struct _tuple9* _temp986; struct _tuple9 _temp988; struct Cyc_List_List*
_temp989; struct Cyc_List_List* _temp991; struct Cyc_List_List* _temp993; _LL965:
if( _temp963 == 0){ goto _LL966;} else{ goto _LL967;} _LL967: if( _temp963 == 0){
goto _LL969;} else{ _temp971=* _temp963; _LL975: _temp974=( struct _tuple9*)
_temp971.hd; _temp976=* _temp974; _LL982: _temp981= _temp976.f1; if( _temp981 ==
0){ goto _LL980;} else{ goto _LL969;} _LL980: _temp979= _temp976.f2; if(
_temp979 == 0){ goto _LL978;} else{ goto _LL969;} _LL978: _temp977= _temp976.f3;
if( _temp977 == 0){ goto _LL973;} else{ goto _LL969;} _LL973: _temp972= _temp971.tl;
goto _LL968;} _LL969: if( _temp963 == 0){ goto _LL964;} else{ _temp983=*
_temp963; _LL987: _temp986=( struct _tuple9*) _temp983.hd; _temp988=* _temp986;
_LL994: _temp993= _temp988.f1; goto _LL992; _LL992: _temp991= _temp988.f2; goto
_LL990; _LL990: _temp989= _temp988.f3; goto _LL985; _LL985: _temp984= _temp983.tl;
goto _LL970;} _LL966: return( void*)({ struct Cyc_Tcpat_Success_struct* _temp995=(
struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp995[ 0]=({ struct Cyc_Tcpat_Success_struct _temp996; _temp996.tag= Cyc_Tcpat_Success;
_temp996.f1=( void*) right_hand_side; _temp996;}); _temp995;}); _LL968: return((
void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp972, right_hand_side, rules); _LL970: if(( _temp993 == 0? 1: _temp991
== 0)? 1: _temp989 == 0){ return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp997="tcpat:and_match: malformed work frame"; struct
_tagged_string _temp998; _temp998.curr= _temp997; _temp998.base= _temp997;
_temp998.last_plus_one= _temp997 + 38; _temp998;}));}{ struct Cyc_List_List
_temp1001; struct Cyc_List_List* _temp1002; void* _temp1004; struct Cyc_List_List*
_temp999=( struct Cyc_List_List*) _check_null( _temp993); _temp1001=* _temp999;
_LL1005: _temp1004=( void*) _temp1001.hd; goto _LL1003; _LL1003: _temp1002=
_temp1001.tl; goto _LL1000; _LL1000: { struct Cyc_List_List _temp1008; struct
Cyc_List_List* _temp1009; struct Cyc_List_List* _temp1011; struct Cyc_List_List*
_temp1006=( struct Cyc_List_List*) _check_null( _temp991); _temp1008=* _temp1006;
_LL1012: _temp1011=( struct Cyc_List_List*) _temp1008.hd; goto _LL1010; _LL1010:
_temp1009= _temp1008.tl; goto _LL1007; _LL1007: { struct Cyc_List_List _temp1015;
struct Cyc_List_List* _temp1016; void* _temp1018; struct Cyc_List_List*
_temp1013=( struct Cyc_List_List*) _check_null( _temp989); _temp1015=* _temp1013;
_LL1019: _temp1018=( void*) _temp1015.hd; goto _LL1017; _LL1017: _temp1016=
_temp1015.tl; goto _LL1014; _LL1014: { struct _tuple9* _temp1021=({ struct
_tuple9* _temp1020=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1020->f1= _temp1002; _temp1020->f2= _temp1009; _temp1020->f3= _temp1016;
_temp1020;}); goto _LL1022; _LL1022: return(( void*(*)( void* p, struct Cyc_List_List*
obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp1004,
_temp1011, _temp1018, ctx,({ struct Cyc_List_List* _temp1023=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1023->hd=( void*) _temp1021;
_temp1023->tl= _temp984; _temp1023;}), right_hand_side, rules);}}}} _LL964:;}
static struct Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon,
void* dsc){ void* _temp1024= dsc; struct Cyc_Set_Set* _temp1030; struct Cyc_List_List*
_temp1032; struct Cyc_Tcpat_Con_s* _temp1034; _LL1026: if(*(( int*) _temp1024)
== Cyc_Tcpat_Neg){ _LL1031: _temp1030=(( struct Cyc_Tcpat_Neg_struct*) _temp1024)->f1;
goto _LL1027;} else{ goto _LL1028;} _LL1028: if(*(( int*) _temp1024) == Cyc_Tcpat_Pos){
_LL1035: _temp1034=(( struct Cyc_Tcpat_Pos_struct*) _temp1024)->f1; goto _LL1033;
_LL1033: _temp1032=(( struct Cyc_Tcpat_Pos_struct*) _temp1024)->f2; goto _LL1029;}
else{ goto _LL1025;} _LL1027: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp1039=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp1039[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp1040; _temp1040.tag= Cyc_Tcpat_Neg;
_temp1040.f1= Cyc_Tcpat_empty_con_set(); _temp1040;}); _temp1039;}); struct Cyc_List_List*
_temp1036= 0; goto _LL1037; _LL1037:{ int i= 0; for( 0; i < pcon->arity; ++ i){
_temp1036=({ struct Cyc_List_List* _temp1038=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1038->hd=( void*) any; _temp1038->tl=
_temp1036; _temp1038;});}} return _temp1036;} _LL1029: return _temp1032; _LL1025:;}
static struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp1041=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1041->hd=( void*)( i + 1); _temp1041->tl=
obj; _temp1041;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1042= p; struct Cyc_List_List* _temp1048; struct Cyc_Tcpat_Con_s*
_temp1050; _LL1044: if( _temp1042 ==( void*) Cyc_Tcpat_Any){ goto _LL1045;}
else{ goto _LL1046;} _LL1046: if(( unsigned int) _temp1042 > 1u?*(( int*)
_temp1042) == Cyc_Tcpat_Con: 0){ _LL1051: _temp1050=(( struct Cyc_Tcpat_Con_struct*)
_temp1042)->f1; goto _LL1049; _LL1049: _temp1048=(( struct Cyc_Tcpat_Con_struct*)
_temp1042)->f2; goto _LL1047;} else{ goto _LL1043;} _LL1045: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1047: { void* _temp1052= Cyc_Tcpat_static_match(
_temp1050, dsc); _LL1054: if( _temp1052 ==( void*) Cyc_Tcpat_Yes){ goto _LL1055;}
else{ goto _LL1056;} _LL1056: if( _temp1052 ==( void*) Cyc_Tcpat_No){ goto
_LL1057;} else{ goto _LL1058;} _LL1058: if( _temp1052 ==( void*) Cyc_Tcpat_Maybe){
goto _LL1059;} else{ goto _LL1053;} _LL1055: { struct Cyc_List_List* _temp1062=({
struct Cyc_List_List* _temp1060=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1060->hd=( void*)({ struct _tuple10* _temp1061=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1061->f1= _temp1050;
_temp1061->f2= 0; _temp1061;}); _temp1060->tl= ctx; _temp1060;}); goto _LL1063;
_LL1063: { struct _tuple9* _temp1065=({ struct _tuple9* _temp1064=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp1064->f1= _temp1048;
_temp1064->f2= Cyc_Tcpat_getoargs( _temp1050, obj); _temp1064->f3= Cyc_Tcpat_getdargs(
_temp1050, dsc); _temp1064;}); goto _LL1066; _LL1066: { struct Cyc_List_List*
_temp1068=({ struct Cyc_List_List* _temp1067=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1067->hd=( void*) _temp1065; _temp1067->tl=
work; _temp1067;}); goto _LL1069; _LL1069: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( _temp1062, _temp1068, right_hand_side, rules);}}}
_LL1057: return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)(
Cyc_Tcpat_build_desc( ctx, dsc, work), rules); _LL1059: { struct Cyc_List_List*
_temp1072=({ struct Cyc_List_List* _temp1070=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1070->hd=( void*)({ struct _tuple10*
_temp1071=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1071->f1=
_temp1050; _temp1071->f2= 0; _temp1071;}); _temp1070->tl= ctx; _temp1070;});
goto _LL1073; _LL1073: { struct _tuple9* _temp1075=({ struct _tuple9* _temp1074=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp1074->f1= _temp1048;
_temp1074->f2= Cyc_Tcpat_getoargs( _temp1050, obj); _temp1074->f3= Cyc_Tcpat_getdargs(
_temp1050, dsc); _temp1074;}); goto _LL1076; _LL1076: { struct Cyc_List_List*
_temp1078=({ struct Cyc_List_List* _temp1077=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1077->hd=( void*) _temp1075; _temp1077->tl=
work; _temp1077;}); goto _LL1079; _LL1079: { void* _temp1080=(( void*(*)( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_and_match)( _temp1072, _temp1078,
right_hand_side, rules); goto _LL1081; _LL1081: { void* _temp1082=(( void*(*)(
void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, Cyc_Tcpat_add_neg( dsc, _temp1050), work), rules); goto _LL1083; _LL1083:
return( void*)({ struct Cyc_Tcpat_IfEq_struct* _temp1084=( struct Cyc_Tcpat_IfEq_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct)); _temp1084[ 0]=({ struct Cyc_Tcpat_IfEq_struct
_temp1085; _temp1085.tag= Cyc_Tcpat_IfEq; _temp1085.f1= obj; _temp1085.f2=
_temp1050; _temp1085.f3=( void*) _temp1080; _temp1085.f4=( void*) _temp1082;
_temp1085;}); _temp1084;});}}}}} _LL1053:;} _LL1043:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp1086= d; void* _temp1094; void* _temp1096; void*
_temp1098; struct Cyc_Tcpat_Con_s* _temp1100; struct Cyc_List_List* _temp1102;
_LL1088: if( _temp1086 ==( void*) Cyc_Tcpat_Failure){ goto _LL1089;} else{ goto
_LL1090;} _LL1090: if(( unsigned int) _temp1086 > 1u?*(( int*) _temp1086) == Cyc_Tcpat_Success:
0){ _LL1095: _temp1094=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1086)->f1;
goto _LL1091;} else{ goto _LL1092;} _LL1092: if(( unsigned int) _temp1086 > 1u?*((
int*) _temp1086) == Cyc_Tcpat_IfEq: 0){ _LL1103: _temp1102=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1086)->f1; goto _LL1101; _LL1101: _temp1100=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1086)->f2; goto _LL1099; _LL1099: _temp1098=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1086)->f3; goto _LL1097; _LL1097: _temp1096=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1086)->f4; goto _LL1093;} else{ goto _LL1087;} _LL1089: not_exhaust( env1);
goto _LL1087; _LL1091: rhs_appears( env2, _temp1094); goto _LL1087; _LL1093:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1098,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1096, not_exhaust, env1, rhs_appears, env2); goto _LL1087; _LL1087:;}
struct _tuple11{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple12{
void* f1; struct _tuple11* f2; } ; static struct _tuple12* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple11* rhs=({ struct _tuple11* _temp1109=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1109->f1= 0; _temp1109->f2=(
swc->pattern)->loc; _temp1109;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1104=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1104->hd=( void*) sp0; _temp1104->tl=({ struct Cyc_List_List*
_temp1105=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1105->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1105->tl= 0; _temp1105;});
_temp1104;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1106=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1106->hd=( void*) sp0; _temp1106->tl=({ struct Cyc_List_List* _temp1107=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1107->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1107->tl= 0; _temp1107;}); _temp1106;}));}
return({ struct _tuple12* _temp1108=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1108->f1= sp; _temp1108->f2= rhs; _temp1108;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp1110="patterns may not be exhaustive"; struct
_tagged_string _temp1111; _temp1111.curr= _temp1110; _temp1111.base= _temp1110;
_temp1111.last_plus_one= _temp1110 + 31; _temp1111;}));} static void Cyc_Tcpat_rule_occurs(
int dummy, struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp1112= 0;
goto _LL1113; _LL1113: { struct Cyc_List_List* _temp1114=(( struct Cyc_List_List*(*)(
struct _tuple12*(* f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,& _temp1112, swcs); goto
_LL1115; _LL1115: { void* _temp1116=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1114); goto _LL1117; _LL1117:(( void(*)( void* d,
void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1116, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1114 != 0; _temp1114=(( struct Cyc_List_List*) _check_null( _temp1114))->tl){
struct _tuple12 _temp1120; struct _tuple11* _temp1121; struct _tuple11 _temp1123;
struct Cyc_Position_Segment* _temp1124; int _temp1126; void* _temp1128; struct
_tuple12* _temp1118=( struct _tuple12*)(( struct Cyc_List_List*) _check_null(
_temp1114))->hd; _temp1120=* _temp1118; _LL1129: _temp1128= _temp1120.f1; goto
_LL1122; _LL1122: _temp1121= _temp1120.f2; _temp1123=* _temp1121; _LL1127:
_temp1126= _temp1123.f1; goto _LL1125; _LL1125: _temp1124= _temp1123.f2; goto
_LL1119; _LL1119: if( ! _temp1126){ Cyc_Tcutil_terr( _temp1124,({ unsigned char*
_temp1130="redundant pattern"; struct _tagged_string _temp1131; _temp1131.curr=
_temp1130; _temp1131.base= _temp1130; _temp1131.last_plus_one= _temp1130 + 18;
_temp1131;}));}}}}} struct _tuple13{ struct Cyc_Position_Segment* f1; int f2; }
; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple13* pr){(* pr).f2= 0; Cyc_Tcutil_warn((*
pr).f1,({ unsigned char* _temp1132="pattern not exhaustive"; struct
_tagged_string _temp1133; _temp1133.curr= _temp1132; _temp1133.base= _temp1132;
_temp1133.last_plus_one= _temp1132 + 23; _temp1133;}));} static void Cyc_Tcpat_dummy_fn(
int i, int j){ return;} struct _tuple14{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
_temp1136=({ struct Cyc_List_List* _temp1134=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1134->hd=( void*)({ struct _tuple14*
_temp1135=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1135->f1=
Cyc_Tcpat_compile_pat( p); _temp1135->f2= 0; _temp1135;}); _temp1134->tl= 0;
_temp1134;}); goto _LL1137; _LL1137: { void* _temp1138=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( _temp1136); goto _LL1139; _LL1139: {
struct _tuple13* _temp1141=({ struct _tuple13* _temp1140=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp1140->f1= loc; _temp1140->f2= 1;
_temp1140;}); goto _LL1142; _LL1142:(( void(*)( void* d, void(* not_exhaust)(
struct _tuple13*), struct _tuple13* env1, void(* rhs_appears)( int, int), int
env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1138, Cyc_Tcpat_not_exhaust_warn,
_temp1141, Cyc_Tcpat_dummy_fn, 0); return(* _temp1141).f2;}}} static struct
_tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({ struct _tuple11*
_temp1144=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1144->f1=
0; _temp1144->f2=( swc->pattern)->loc; _temp1144;}); return({ struct _tuple12*
_temp1143=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1143->f1=
sp0; _temp1143->f2= rhs; _temp1143;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp1145=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); goto
_LL1146; _LL1146: { void* _temp1147=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1145); goto _LL1148; _LL1148:(( void(*)( void* d,
void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1147, Cyc_Tcpat_not_exhaust_err2, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1145 != 0; _temp1145=(( struct Cyc_List_List*) _check_null( _temp1145))->tl){
struct _tuple12 _temp1151; struct _tuple11* _temp1152; struct _tuple11 _temp1154;
struct Cyc_Position_Segment* _temp1155; int _temp1157; void* _temp1159; struct
_tuple12* _temp1149=( struct _tuple12*)(( struct Cyc_List_List*) _check_null(
_temp1145))->hd; _temp1151=* _temp1149; _LL1160: _temp1159= _temp1151.f1; goto
_LL1153; _LL1153: _temp1152= _temp1151.f2; _temp1154=* _temp1152; _LL1158:
_temp1157= _temp1154.f1; goto _LL1156; _LL1156: _temp1155= _temp1154.f2; goto
_LL1150; _LL1150: if( ! _temp1157){ Cyc_Tcutil_terr( _temp1155,({ unsigned char*
_temp1161="redundant pattern"; struct _tagged_string _temp1162; _temp1162.curr=
_temp1161; _temp1162.base= _temp1161; _temp1162.last_plus_one= _temp1161 + 18;
_temp1162;}));}}}}