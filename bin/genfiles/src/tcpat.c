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
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_rzip(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_tabulate_c( int
n, void*(* f)( void*, int), void* env); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void*
elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u];
struct _tuple1{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar;
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
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
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
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 16;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static const int
Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; } ; static
const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x, void*
t, struct Cyc_Absyn_Exp* init); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct
Cyc_PP_Doc; extern struct _tagged_string Cyc_Absynpp_typ2string( void*); extern
struct _tagged_string Cyc_Absynpp_kind2string( void*); extern struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); static const int Cyc_Tcenv_VarRes=
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
ae; struct Cyc_Core_Opt* le; } ; extern void* Cyc_Tcenv_lookup_ordinary( struct
Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern struct
Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_unify( void*, void*);
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); struct _tuple4{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5*
Cyc_Tcutil_r_make_inst_var( struct _tuple4*, struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_string err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); struct _tuple6{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; } ; extern struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple7{ struct Cyc_List_List*
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
_temp49=( unsigned char*)"struct tag used without arguments in pattern"; struct
_tagged_string _temp50; _temp50.curr= _temp49; _temp50.base= _temp49; _temp50.last_plus_one=
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
unsigned char* _temp74=( unsigned char*)"qualified variable in pattern"; struct
_tagged_string _temp75; _temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one=
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
struct _tuple7* _temp104=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
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
p->loc,( struct _tagged_string)({ struct _tagged_string _temp109= Cyc_Absynpp_qvar2string(
_temp16); xprintf("bad enum constructor %.*s in pattern", _temp109.last_plus_one
- _temp109.curr, _temp109.curr);})); _npop_handler( 0u); return; _LL88: goto
_LL80; _LL80:;}; _pop_handler();} else{ void* _temp77=( void*) _exn_thrown; void*
_temp111= _temp77; _LL113: if( _temp111 == Cyc_Dict_Absent){ goto _LL114;} else{
goto _LL115;} _LL115: goto _LL116; _LL114: goto _LL112; _LL116:( void) _throw(
_temp111); _LL112:;}}} Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ struct
_tagged_string _temp117= Cyc_Absynpp_qvar2string( _temp16); xprintf("%.*s is not a constructor in pattern",
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
_throw( _temp132); _LL133:;}}} Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({
struct _tagged_string _temp138= Cyc_Absynpp_qvar2string( _temp22); xprintf("%.*s is not a struct name, but pattern has designators",
_temp138.last_plus_one - _temp138.curr, _temp138.curr);})); return; _LL9:
return; _LL1:;} static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl*
vd){ return(* vd->name).f2;} static struct Cyc_Core_Opt Cyc_Tcpat_mk={( void*)((
void*) 1u)}; static struct Cyc_Core_Opt Cyc_Tcpat_rk={( void*)(( void*) 3u)};
static struct Cyc_Core_Opt Cyc_Tcpat_ak={( void*)(( void*) 0u)}; static void*
Cyc_Tcpat_any_type( struct Cyc_List_List* s, void** topt){ if( topt != 0){
return*(( void**) _check_null( topt));} return Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcpat_mk,({ struct Cyc_Core_Opt* _temp139=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp139->v=( void*) s; _temp139;}));} static
void* Cyc_Tcpat_num_type( void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(*((
void**) _check_null( topt))): 0){ return*(( void**) _check_null( topt));}{ void*
_temp140= Cyc_Tcutil_compress( numt); struct Cyc_Absyn_Enumdecl* _temp146;
struct _tuple1* _temp148; _LL142: if(( unsigned int) _temp140 > 4u?*(( int*)
_temp140) == Cyc_Absyn_EnumType: 0){ _LL149: _temp148=(( struct Cyc_Absyn_EnumType_struct*)
_temp140)->f1; goto _LL147; _LL147: _temp146=(( struct Cyc_Absyn_EnumType_struct*)
_temp140)->f2; goto _LL143;} else{ goto _LL144;} _LL144: goto _LL145; _LL143:
if( topt != 0){ return*(( void**) _check_null( topt));} goto _LL141; _LL145:
goto _LL141; _LL141:;} return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl*
vd, struct Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t);
vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({ struct Cyc_List_List* _temp150=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp150->hd=(
void*) vd; _temp150->tl=* v_result_ptr; _temp150;});} struct _tuple8{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; struct _tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple6 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp151=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp185; struct Cyc_Absyn_Vardecl*
_temp187; int _temp189; void* _temp191; int _temp193; void* _temp195;
unsigned char _temp197; struct _tagged_string _temp199; struct Cyc_Absyn_Enumfield*
_temp201; struct Cyc_Absyn_Enumdecl* _temp203; struct Cyc_Absyn_Pat* _temp205;
struct Cyc_List_List* _temp207; struct Cyc_List_List* _temp209; struct Cyc_List_List*
_temp211; struct Cyc_Core_Opt* _temp213; struct Cyc_Core_Opt** _temp215; struct
Cyc_Absyn_Structdecl* _temp216; struct Cyc_List_List* _temp218; struct Cyc_List_List*
_temp220; struct Cyc_Absyn_Tunionfield* _temp222; struct Cyc_Absyn_Tuniondecl*
_temp224; struct _tuple1* _temp226; struct Cyc_List_List* _temp228; struct Cyc_List_List*
_temp230; struct _tuple1* _temp232; struct Cyc_List_List* _temp234; struct Cyc_List_List*
_temp236; struct _tuple1* _temp238; _LL153: if( _temp151 ==( void*) Cyc_Absyn_Wild_p){
goto _LL154;} else{ goto _LL155;} _LL155: if(( unsigned int) _temp151 > 2u?*((
int*) _temp151) == Cyc_Absyn_Var_p: 0){ _LL186: _temp185=(( struct Cyc_Absyn_Var_p_struct*)
_temp151)->f1; goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int)
_temp151 > 2u?*(( int*) _temp151) == Cyc_Absyn_Reference_p: 0){ _LL188: _temp187=((
struct Cyc_Absyn_Reference_p_struct*) _temp151)->f1; goto _LL158;} else{ goto
_LL159;} _LL159: if(( unsigned int) _temp151 > 2u?*(( int*) _temp151) == Cyc_Absyn_Int_p:
0){ _LL192: _temp191=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp151)->f1;
if( _temp191 ==( void*) Cyc_Absyn_Unsigned){ goto _LL190;} else{ goto _LL161;}
_LL190: _temp189=(( struct Cyc_Absyn_Int_p_struct*) _temp151)->f2; goto _LL160;}
else{ goto _LL161;} _LL161: if(( unsigned int) _temp151 > 2u?*(( int*) _temp151)
== Cyc_Absyn_Int_p: 0){ _LL196: _temp195=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp151)->f1; if( _temp195 ==( void*) Cyc_Absyn_Signed){ goto _LL194;} else{
goto _LL163;} _LL194: _temp193=(( struct Cyc_Absyn_Int_p_struct*) _temp151)->f2;
goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int) _temp151 > 2u?*((
int*) _temp151) == Cyc_Absyn_Char_p: 0){ _LL198: _temp197=(( struct Cyc_Absyn_Char_p_struct*)
_temp151)->f1; goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int)
_temp151 > 2u?*(( int*) _temp151) == Cyc_Absyn_Float_p: 0){ _LL200: _temp199=((
struct Cyc_Absyn_Float_p_struct*) _temp151)->f1; goto _LL166;} else{ goto _LL167;}
_LL167: if(( unsigned int) _temp151 > 2u?*(( int*) _temp151) == Cyc_Absyn_Enum_p:
0){ _LL204: _temp203=(( struct Cyc_Absyn_Enum_p_struct*) _temp151)->f1; goto
_LL202; _LL202: _temp201=(( struct Cyc_Absyn_Enum_p_struct*) _temp151)->f2; goto
_LL168;} else{ goto _LL169;} _LL169: if( _temp151 ==( void*) Cyc_Absyn_Null_p){
goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp151 > 2u?*((
int*) _temp151) == Cyc_Absyn_Pointer_p: 0){ _LL206: _temp205=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp151)->f1; goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int)
_temp151 > 2u?*(( int*) _temp151) == Cyc_Absyn_Tuple_p: 0){ _LL208: _temp207=((
struct Cyc_Absyn_Tuple_p_struct*) _temp151)->f1; goto _LL174;} else{ goto _LL175;}
_LL175: if(( unsigned int) _temp151 > 2u?*(( int*) _temp151) == Cyc_Absyn_Struct_p:
0){ _LL217: _temp216=(( struct Cyc_Absyn_Struct_p_struct*) _temp151)->f1; goto
_LL214; _LL214: _temp213=(( struct Cyc_Absyn_Struct_p_struct*) _temp151)->f2;
_temp215=&(( struct Cyc_Absyn_Struct_p_struct*) _temp151)->f2; goto _LL212;
_LL212: _temp211=(( struct Cyc_Absyn_Struct_p_struct*) _temp151)->f3; goto
_LL210; _LL210: _temp209=(( struct Cyc_Absyn_Struct_p_struct*) _temp151)->f4;
goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int) _temp151 > 2u?*((
int*) _temp151) == Cyc_Absyn_Tunion_p: 0){ _LL225: _temp224=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp151)->f1; goto _LL223; _LL223: _temp222=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp151)->f2; goto _LL221; _LL221: _temp220=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp151)->f3; goto _LL219; _LL219: _temp218=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp151)->f4; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp151 > 2u?*(( int*) _temp151) == Cyc_Absyn_UnknownId_p: 0){ _LL227: _temp226=((
struct Cyc_Absyn_UnknownId_p_struct*) _temp151)->f1; goto _LL180;} else{ goto
_LL181;} _LL181: if(( unsigned int) _temp151 > 2u?*(( int*) _temp151) == Cyc_Absyn_UnknownCall_p:
0){ _LL233: _temp232=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp151)->f1;
goto _LL231; _LL231: _temp230=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp151)->f2; goto _LL229; _LL229: _temp228=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp151)->f3; goto _LL182;} else{ goto _LL183;} _LL183: if(( unsigned int)
_temp151 > 2u?*(( int*) _temp151) == Cyc_Absyn_UnknownFields_p: 0){ _LL239:
_temp238=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp151)->f1; goto _LL237;
_LL237: _temp236=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp151)->f2;
goto _LL235; _LL235: _temp234=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp151)->f3; goto _LL184;} else{ goto _LL152;} _LL154: t= Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars( te), topt); goto _LL152; _LL156: t= Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars( te), topt); Cyc_Tcpat_set_vd( _temp185,& v_result, t);
goto _LL152; _LL158: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars( te),
topt); if( region_opt == 0){ Cyc_Tcutil_terr( p->loc,({ unsigned char* _temp240=(
unsigned char*)"* pattern would point into an unknown region"; struct
_tagged_string _temp241; _temp241.curr= _temp240; _temp241.base= _temp240;
_temp241.last_plus_one= _temp240 + 45; _temp241;})); goto _LL152;} Cyc_Tcpat_set_vd(
_temp187,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp242=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp242[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp243; _temp243.tag= Cyc_Absyn_PointerType;
_temp243.f1=({ struct Cyc_Absyn_PtrInfo _temp244; _temp244.elt_typ=( void*) t;
_temp244.rgn_typ=( void*)*(( void**) _check_null( region_opt)); _temp244.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp244.tq= Cyc_Absyn_empty_tqual();
_temp244.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp244;}); _temp243;}); _temp242;})); goto _LL152; _LL160: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_uint_t); goto _LL152; _LL162: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL152; _LL164: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto
_LL152; _LL166: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL152;
_LL168: t= Cyc_Tcpat_num_type( topt,( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp245=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp245[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp246; _temp246.tag= Cyc_Absyn_EnumType;
_temp246.f1= _temp203->name; _temp246.f2=( struct Cyc_Absyn_Enumdecl*) _temp203;
_temp246;}); _temp245;})); goto _LL152; _LL170: if( topt != 0){ void* _temp247=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp253; _LL249: if(( unsigned int) _temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_PointerType:
0){ _LL254: _temp253=(( struct Cyc_Absyn_PointerType_struct*) _temp247)->f1;
goto _LL250;} else{ goto _LL251;} _LL251: goto _LL252; _LL250: t=*(( void**)
_check_null( topt)); goto tcpat_end; _LL252: goto _LL248; _LL248:;}{ struct Cyc_List_List*
_temp255= Cyc_Tcenv_lookup_type_vars( te); goto _LL256; _LL256: t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp257=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp257[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp258; _temp258.tag= Cyc_Absyn_PointerType;
_temp258.f1=({ struct Cyc_Absyn_PtrInfo _temp259; _temp259.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_ak,({ struct Cyc_Core_Opt* _temp261=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp261->v=( void*)
_temp255; _temp261;})); _temp259.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp260=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp260->v=( void*) _temp255;
_temp260;})); _temp259.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp259.tq= Cyc_Absyn_empty_tqual(); _temp259.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp259;}); _temp258;}); _temp257;}); goto _LL152;}
_LL172: { void* inner_typ=( void*) Cyc_Absyn_VoidType; void** _temp262= 0; goto
_LL263; _LL263: if( topt != 0){ void* _temp264= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp270; struct Cyc_Absyn_Conref*
_temp272; struct Cyc_Absyn_Tqual _temp274; struct Cyc_Absyn_Conref* _temp276;
void* _temp278; void* _temp280; _LL266: if(( unsigned int) _temp264 > 4u?*(( int*)
_temp264) == Cyc_Absyn_PointerType: 0){ _LL271: _temp270=(( struct Cyc_Absyn_PointerType_struct*)
_temp264)->f1; _LL281: _temp280=( void*) _temp270.elt_typ; goto _LL279; _LL279:
_temp278=( void*) _temp270.rgn_typ; goto _LL277; _LL277: _temp276= _temp270.nullable;
goto _LL275; _LL275: _temp274= _temp270.tq; goto _LL273; _LL273: _temp272=
_temp270.bounds; goto _LL267;} else{ goto _LL268;} _LL268: goto _LL269; _LL267:
inner_typ= _temp280; _temp262=( void**)& inner_typ; goto _LL265; _LL269: goto
_LL265; _LL265:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp315=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp315->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp315;})); struct Cyc_List_List* _temp284; struct Cyc_List_List*
_temp286; struct _tuple6 _temp282= Cyc_Tcpat_tcPatRec( te, _temp205, _temp262,(
void**)& ptr_rgn); _LL287: _temp286= _temp282.f1; goto _LL285; _LL285: _temp284=
_temp282.f2; goto _LL283; _LL283: tv_result=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp286); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp284); if( _temp262 == 0){
void* _temp288= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp205->topt))->v); struct Cyc_Absyn_TunionFieldInfo _temp294; struct Cyc_List_List*
_temp296; void* _temp298; struct Cyc_Absyn_Tunionfield* _temp300; struct Cyc_Absyn_Tuniondecl*
_temp302; _LL290: if(( unsigned int) _temp288 > 4u?*(( int*) _temp288) == Cyc_Absyn_TunionFieldType:
0){ _LL295: _temp294=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp288)->f1;
_LL299: _temp298=( void*) _temp294.field_info; if(*(( int*) _temp298) == Cyc_Absyn_KnownTunionfield){
_LL303: _temp302=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp298)->f1;
goto _LL301; _LL301: _temp300=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp298)->f2; goto _LL297;} else{ goto _LL292;} _LL297: _temp296= _temp294.targs;
goto _LL291;} else{ goto _LL292;} _LL292: goto _LL293; _LL291: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp304=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp304[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp305; _temp305.tag= Cyc_Absyn_TunionType;
_temp305.f1=({ struct Cyc_Absyn_TunionInfo _temp306; _temp306.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp307=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp307[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp308; _temp308.tag= Cyc_Absyn_KnownTunion;
_temp308.f1= _temp302; _temp308;}); _temp307;})); _temp306.targs= _temp296;
_temp306.rgn=( void*) ptr_rgn; _temp306;}); _temp305;}); _temp304;}); goto
_LL289; _LL293: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp309=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp309[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp310; _temp310.tag= Cyc_Absyn_PointerType;
_temp310.f1=({ struct Cyc_Absyn_PtrInfo _temp311; _temp311.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp205->topt))->v); _temp311.rgn_typ=(
void*) ptr_rgn; _temp311.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp311.tq= Cyc_Absyn_empty_tqual(); _temp311.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp311;}); _temp310;}); _temp309;}); goto _LL289;
_LL289:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp312=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp312[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp313; _temp313.tag= Cyc_Absyn_PointerType;
_temp313.f1=({ struct Cyc_Absyn_PtrInfo _temp314; _temp314.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp205->topt))->v); _temp314.rgn_typ=(
void*) ptr_rgn; _temp314.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp314.tq= Cyc_Absyn_empty_tqual(); _temp314.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp314;}); _temp313;}); _temp312;});} goto _LL152;}}
_LL174: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp316= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_List_List* _temp322; _LL318: if(( unsigned int) _temp316 > 4u?*(( int*)
_temp316) == Cyc_Absyn_TupleType: 0){ _LL323: _temp322=(( struct Cyc_Absyn_TupleType_struct*)
_temp316)->f1; goto _LL319;} else{ goto _LL320;} _LL320: goto _LL321; _LL319:
topt_ts= _temp322; goto _LL317; _LL321: goto _LL317; _LL317:;} for( 0; _temp207
!= 0; _temp207=(( struct Cyc_List_List*) _check_null( _temp207))->tl){ void**
_temp324= 0; goto _LL325; _LL325: if( topt_ts != 0){ _temp324=( void**)&(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( topt_ts))->hd)).f2;
topt_ts=(( struct Cyc_List_List*) _check_null( topt_ts))->tl;}{ struct Cyc_List_List*
_temp328; struct Cyc_List_List* _temp330; struct _tuple6 _temp326= Cyc_Tcpat_tcPatRec(
te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp207))->hd,
_temp324, region_opt); _LL331: _temp330= _temp326.f1; goto _LL329; _LL329:
_temp328= _temp326.f2; goto _LL327; _LL327: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp330); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp328); pat_ts=({ struct Cyc_List_List*
_temp332=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp332->hd=( void*)({ struct _tuple8* _temp333=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp333->f1= Cyc_Absyn_empty_tqual(); _temp333->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Pat*)(( struct
Cyc_List_List*) _check_null( _temp207))->hd)->topt))->v; _temp333;}); _temp332->tl=
pat_ts; _temp332;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp334=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp334[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp335; _temp335.tag= Cyc_Absyn_TupleType;
_temp335.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp335;}); _temp334;}); goto _LL152;} _LL176: if( _temp211 != 0){(
void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp336=( unsigned char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp337; _temp337.curr= _temp336; _temp337.base= _temp336;
_temp337.last_plus_one= _temp336 + 35; _temp337;})));}{ struct _RegionHandle
_temp338= _new_region(); struct _RegionHandle* rgn=& _temp338; _push_region( rgn);{
struct _tuple4 _temp340=({ struct _tuple4 _temp339; _temp339.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp339.f2= rgn; _temp339;}); goto _LL341; _LL341: { struct Cyc_List_List*
_temp342=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)(
struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp340, _temp216->tvs);
goto _LL343; _LL343: { struct Cyc_List_List* _temp344=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp342); goto _LL345; _LL345: t=( void*)({
struct Cyc_Absyn_StructType_struct* _temp346=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp346[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp347; _temp347.tag= Cyc_Absyn_StructType;
_temp347.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp216->name))->v); _temp347.f2= _temp344; _temp347.f3=({ struct
Cyc_Absyn_Structdecl** _temp348=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp348[ 0]= _temp216; _temp348;});
_temp347;}); _temp346;}); if( topt != 0){ Cyc_Tcutil_unify( t,*(( void**)
_check_null( topt)));}* _temp215=({ struct Cyc_Core_Opt* _temp349=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp349->v=( void*) _temp344;
_temp349;}); if( _temp216->fields == 0){ Cyc_Tcutil_terr( p->loc,({
unsigned char* _temp350=( unsigned char*)"can't destructure an abstract struct";
struct _tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 37; _temp351;})); _npop_handler( 0u); goto
_LL152;}{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, p->loc,
_temp209,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp216->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp354; struct Cyc_Absyn_Pat* _temp355; struct Cyc_Absyn_Structfield*
_temp357; struct _tuple9* _temp352=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp354=* _temp352; _LL358: _temp357= _temp354.f1;
goto _LL356; _LL356: _temp355= _temp354.f2; goto _LL353; _LL353: { void*
_temp359= Cyc_Tcutil_rsubstitute( rgn, _temp342,( void*) _temp357->type); goto
_LL360; _LL360: { struct Cyc_List_List* _temp363; struct Cyc_List_List* _temp365;
struct _tuple6 _temp361= Cyc_Tcpat_tcPatRec( te, _temp355,( void**)& _temp359,
region_opt); _LL366: _temp365= _temp361.f1; goto _LL364; _LL364: _temp363=
_temp361.f2; goto _LL362; _LL362: tv_result=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp365); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp363); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp355->topt))->v, _temp359)){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_string)({ struct _tagged_string _temp367=* _temp357->name;
struct _tagged_string _temp368= Cyc_Absynpp_typ2string( _temp359); struct
_tagged_string _temp369= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp355->topt))->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp367.last_plus_one - _temp367.curr, _temp367.curr, _temp368.last_plus_one -
_temp368.curr, _temp368.curr, _temp369.last_plus_one - _temp369.curr, _temp369.curr);}));}}}}}}}};
_pop_region(& _temp338);} goto _LL152; _LL178: if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp220) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp222->tvs)){ Cyc_Tcutil_terr( p->loc,({ unsigned char* _temp370=(
unsigned char*)"wrong number of existential type variables in pattern"; struct
_tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 54; _temp371;}));}{ struct _RegionHandle
_temp372= _new_region(); struct _RegionHandle* rgn=& _temp372; _push_region( rgn);{
struct Cyc_List_List* _temp373= 0; goto _LL374; _LL374:{ struct Cyc_List_List* t=
_temp220; for( 0; t != 0; t=(( struct Cyc_List_List*) _check_null( t))->tl){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( t))->hd;{ void* _temp375=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void*
_temp385; void* _temp387; struct Cyc_Absyn_Conref* _temp389; _LL377: if((
unsigned int) _temp375 > 1u?*(( int*) _temp375) == Cyc_Absyn_Eq_constr: 0){
_LL386: _temp385=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp375)->f1;
if( _temp385 ==( void*) Cyc_Absyn_BoxKind){ goto _LL378;} else{ goto _LL379;}}
else{ goto _LL379;} _LL379: if(( unsigned int) _temp375 > 1u?*(( int*) _temp375)
== Cyc_Absyn_Eq_constr: 0){ _LL388: _temp387=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp375)->f1; goto _LL380;} else{ goto _LL381;} _LL381: if( _temp375 ==( void*)
Cyc_Absyn_No_constr){ goto _LL382;} else{ goto _LL383;} _LL383: if((
unsigned int) _temp375 > 1u?*(( int*) _temp375) == Cyc_Absyn_Forward_constr: 0){
_LL390: _temp389=(( struct Cyc_Absyn_Forward_constr_struct*) _temp375)->f1; goto
_LL384;} else{ goto _LL376;} _LL378: goto _LL376; _LL380: Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ struct _tagged_string _temp391= Cyc_Absynpp_qvar2string(
_temp224->name); struct _tagged_string _temp392=* tv->name; struct
_tagged_string _temp393= Cyc_Absynpp_kind2string( _temp387); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp391.last_plus_one - _temp391.curr, _temp391.curr, _temp392.last_plus_one -
_temp392.curr, _temp392.curr, _temp393.last_plus_one - _temp393.curr, _temp393.curr);}));
goto _LL376; _LL382:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp394=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp394[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp395; _temp395.tag= Cyc_Absyn_Eq_constr;
_temp395.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp395;}); _temp394;})));
goto _LL376; _LL384:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp396=( unsigned char*)"forward after compress in conref";
struct _tagged_string _temp397; _temp397.curr= _temp396; _temp397.base= _temp396;
_temp397.last_plus_one= _temp396 + 33; _temp397;}))); _LL376:;} _temp373=({
struct Cyc_List_List* _temp398=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp398->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp399=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp399[ 0]=({ struct Cyc_Absyn_VarType_struct _temp400; _temp400.tag= Cyc_Absyn_VarType;
_temp400.f1= tv; _temp400;}); _temp399;})); _temp398->tl= _temp373; _temp398;});}}
_temp373=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp373); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp220);{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp220); struct Cyc_List_List* tqts=
_temp222->typs; struct Cyc_List_List* _temp401= Cyc_Tcenv_lookup_type_vars( te);
goto _LL402; _LL402: { struct _tuple4 _temp404=({ struct _tuple4 _temp403;
_temp403.f1= _temp401; _temp403.f2= rgn; _temp403;}); goto _LL405; _LL405: {
struct Cyc_List_List* _temp406=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp404, _temp224->tvs); goto _LL407; _LL407: { struct Cyc_List_List* _temp408=((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp222->tvs, _temp373); goto _LL409; _LL409: { struct Cyc_List_List* _temp410=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp406); goto _LL411;
_LL411: { struct Cyc_List_List* _temp412=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple5*)) Cyc_Core_snd, _temp408); goto _LL413; _LL413: { struct Cyc_List_List*
_temp414=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp406, _temp408); goto
_LL415; _LL415: if( tqts == 0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp416=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp416[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp417; _temp417.tag= Cyc_Absyn_TunionType;
_temp417.f1=({ struct Cyc_Absyn_TunionInfo _temp418; _temp418.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp420=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp420[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp421; _temp421.tag= Cyc_Absyn_KnownTunion;
_temp421.f1= _temp224; _temp421;}); _temp420;})); _temp418.targs= _temp410;
_temp418.rgn=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({
struct Cyc_Core_Opt* _temp419=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp419->v=( void*) _temp401; _temp419;})); _temp418;});
_temp417;}); _temp416;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp422=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp422[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp423; _temp423.tag= Cyc_Absyn_TunionFieldType; _temp423.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp424; _temp424.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp425=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp425[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp426; _temp426.tag= Cyc_Absyn_KnownTunionfield; _temp426.f1= _temp224;
_temp426.f2= _temp222; _temp426;}); _temp425;})); _temp424.targs= _temp410;
_temp424;}); _temp423;}); _temp422;});} if( topt != 0? tqts == 0: 0){ void*
_temp427= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo
_temp435; struct Cyc_Absyn_TunionInfo _temp437; void* _temp439; struct Cyc_List_List*
_temp441; void* _temp443; _LL429: if(( unsigned int) _temp427 > 4u?*(( int*)
_temp427) == Cyc_Absyn_TunionFieldType: 0){ _LL436: _temp435=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp427)->f1; goto _LL430;} else{ goto _LL431;} _LL431: if(( unsigned int)
_temp427 > 4u?*(( int*) _temp427) == Cyc_Absyn_TunionType: 0){ _LL438: _temp437=((
struct Cyc_Absyn_TunionType_struct*) _temp427)->f1; _LL444: _temp443=( void*)
_temp437.tunion_info; goto _LL442; _LL442: _temp441= _temp437.targs; goto _LL440;
_LL440: _temp439=( void*) _temp437.rgn; goto _LL432;} else{ goto _LL433;} _LL433:
goto _LL434; _LL430: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp445=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp445[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp446; _temp446.tag= Cyc_Absyn_TunionFieldType; _temp446.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp447; _temp447.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp448=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp448[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp449; _temp449.tag= Cyc_Absyn_KnownTunionfield; _temp449.f1= _temp224;
_temp449.f2= _temp222; _temp449;}); _temp448;})); _temp447.targs= _temp410;
_temp447;}); _temp446;}); _temp445;}); goto _LL428; _LL432: { struct Cyc_List_List*
_temp450= _temp410; goto _LL451; _LL451: for( 0; _temp450 != 0? _temp441 != 0: 0;(
_temp450=(( struct Cyc_List_List*) _check_null( _temp450))->tl, _temp441=((
struct Cyc_List_List*) _check_null( _temp441))->tl)){ Cyc_Tcutil_unify(( void*)((
struct Cyc_List_List*) _check_null( _temp450))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp441))->hd);} goto _LL428;} _LL434: goto _LL428; _LL428:;} for(
0; _temp218 != 0? tqts != 0: 0;( _temp218=(( struct Cyc_List_List*) _check_null(
_temp218))->tl, tqts=(( struct Cyc_List_List*) _check_null( tqts))->tl)){ struct
Cyc_Absyn_Pat* _temp452=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp218))->hd; goto _LL453; _LL453: { void* _temp454= Cyc_Tcutil_rsubstitute(
rgn, _temp414,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( tqts))->hd)).f2);
goto _LL455; _LL455: { struct Cyc_List_List* _temp458; struct Cyc_List_List*
_temp460; struct _tuple6 _temp456= Cyc_Tcpat_tcPatRec( te2, _temp452,( void**)&
_temp454, region_opt); _LL461: _temp460= _temp456.f1; goto _LL459; _LL459:
_temp458= _temp456.f2; goto _LL457; _LL457: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp460); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp458); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp452->topt))->v, _temp454)){ Cyc_Tcutil_terr(
_temp452->loc,( struct _tagged_string)({ struct _tagged_string _temp462= Cyc_Absynpp_qvar2string(
_temp224->name); struct _tagged_string _temp463= Cyc_Absynpp_typ2string(
_temp454); struct _tagged_string _temp464= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp452->topt))->v); xprintf("%.*s expects argument type %.*s, not %.*s",
_temp462.last_plus_one - _temp462.curr, _temp462.curr, _temp463.last_plus_one -
_temp463.curr, _temp463.curr, _temp464.last_plus_one - _temp464.curr, _temp464.curr);}));}}}}
if( _temp218 != 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ struct
_tagged_string _temp465= Cyc_Absynpp_qvar2string( _temp224->name); xprintf("too many arguments for tunion constructor %.*s",
_temp465.last_plus_one - _temp465.curr, _temp465.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_string)({ struct _tagged_string _temp466= Cyc_Absynpp_qvar2string(
_temp224->name); xprintf("too few arguments for tunion constructor %.*s",
_temp466.last_plus_one - _temp466.curr, _temp466.curr);}));}}}}}}}}};
_pop_region(& _temp372);} goto _LL152; _LL180: goto _LL182; _LL182: goto _LL184;
_LL184: t= Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp467=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp467->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp467;})); goto _LL152; _LL152:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt*
_temp468=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp468->v=( void*) t; _temp468;}); return({ struct _tuple6 _temp469; _temp469.f1=
tv_result; _temp469.f2= v_result; _temp469;});}} struct _tuple6 Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** topt, void**
region_opt){ struct _tuple6 _temp470= Cyc_Tcpat_tcPatRec( te, p, topt,
region_opt); goto _LL471; _LL471:{ struct _RegionHandle _temp472= _new_region();
struct _RegionHandle* r=& _temp472; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_string*(* f)(
struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name,
_temp470.f2), p->loc,({ unsigned char* _temp473=( unsigned char*)"pattern contains a repeated variable";
struct _tagged_string _temp474; _temp474.curr= _temp473; _temp474.base= _temp473;
_temp474.last_plus_one= _temp473 + 37; _temp474;}));; _pop_region(& _temp472);}
return _temp470;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Pat* p){ void* _temp475=( void*) p->r; struct Cyc_Absyn_Pat*
_temp487; struct Cyc_List_List* _temp489; struct Cyc_List_List* _temp491; struct
Cyc_Absyn_Tunionfield* _temp493; struct Cyc_Absyn_Tuniondecl* _temp495; struct
Cyc_List_List* _temp497; struct Cyc_List_List* _temp499; struct Cyc_Core_Opt*
_temp501; struct Cyc_Absyn_Structdecl* _temp503; struct Cyc_List_List* _temp505;
_LL477: if(( unsigned int) _temp475 > 2u?*(( int*) _temp475) == Cyc_Absyn_Pointer_p:
0){ _LL488: _temp487=(( struct Cyc_Absyn_Pointer_p_struct*) _temp475)->f1; goto
_LL478;} else{ goto _LL479;} _LL479: if(( unsigned int) _temp475 > 2u?*(( int*)
_temp475) == Cyc_Absyn_Tunion_p: 0){ _LL496: _temp495=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp475)->f1; goto _LL494; _LL494: _temp493=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp475)->f2; goto _LL492; _LL492: _temp491=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp475)->f3; goto _LL490; _LL490: _temp489=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp475)->f4; goto _LL480;} else{ goto _LL481;} _LL481: if(( unsigned int)
_temp475 > 2u?*(( int*) _temp475) == Cyc_Absyn_Struct_p: 0){ _LL504: _temp503=((
struct Cyc_Absyn_Struct_p_struct*) _temp475)->f1; goto _LL502; _LL502: _temp501=((
struct Cyc_Absyn_Struct_p_struct*) _temp475)->f2; goto _LL500; _LL500: _temp499=((
struct Cyc_Absyn_Struct_p_struct*) _temp475)->f3; goto _LL498; _LL498: _temp497=((
struct Cyc_Absyn_Struct_p_struct*) _temp475)->f4; goto _LL482;} else{ goto
_LL483;} _LL483: if(( unsigned int) _temp475 > 2u?*(( int*) _temp475) == Cyc_Absyn_Tuple_p:
0){ _LL506: _temp505=(( struct Cyc_Absyn_Tuple_p_struct*) _temp475)->f1; goto
_LL484;} else{ goto _LL485;} _LL485: goto _LL486; _LL478: Cyc_Tcpat_check_pat_regions(
te, _temp487);{ void* _temp507=( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v;
struct Cyc_Absyn_PtrInfo _temp515; struct Cyc_Absyn_Conref* _temp517; struct Cyc_Absyn_Tqual
_temp519; struct Cyc_Absyn_Conref* _temp521; void* _temp523; void* _temp525;
struct Cyc_Absyn_TunionInfo _temp527; void* _temp529; struct Cyc_List_List*
_temp531; void* _temp533; _LL509: if(( unsigned int) _temp507 > 4u?*(( int*)
_temp507) == Cyc_Absyn_PointerType: 0){ _LL516: _temp515=(( struct Cyc_Absyn_PointerType_struct*)
_temp507)->f1; _LL526: _temp525=( void*) _temp515.elt_typ; goto _LL524; _LL524:
_temp523=( void*) _temp515.rgn_typ; goto _LL522; _LL522: _temp521= _temp515.nullable;
goto _LL520; _LL520: _temp519= _temp515.tq; goto _LL518; _LL518: _temp517=
_temp515.bounds; goto _LL510;} else{ goto _LL511;} _LL511: if(( unsigned int)
_temp507 > 4u?*(( int*) _temp507) == Cyc_Absyn_TunionType: 0){ _LL528: _temp527=((
struct Cyc_Absyn_TunionType_struct*) _temp507)->f1; _LL534: _temp533=( void*)
_temp527.tunion_info; goto _LL532; _LL532: _temp531= _temp527.targs; goto _LL530;
_LL530: _temp529=( void*) _temp527.rgn; goto _LL512;} else{ goto _LL513;} _LL513:
goto _LL514; _LL510: _temp529= _temp523; goto _LL512; _LL512: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp529); return; _LL514:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp535=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp535[ 0]=({ struct Cyc_Core_Impossible_struct _temp536; _temp536.tag= Cyc_Core_Impossible;
_temp536.f1=({ unsigned char* _temp537=( unsigned char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp538; _temp538.curr= _temp537; _temp538.base= _temp537;
_temp538.last_plus_one= _temp537 + 36; _temp538;}); _temp536;}); _temp535;}));
_LL508:;} _LL480: for( 0; _temp489 != 0; _temp489=(( struct Cyc_List_List*)
_check_null( _temp489))->tl){ Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp489))->hd);}{ void* _temp539=( void*)((
struct Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo
_temp547; void* _temp549; struct Cyc_List_List* _temp551; void* _temp553; struct
Cyc_Absyn_TunionFieldInfo _temp555; _LL541: if(( unsigned int) _temp539 > 4u?*((
int*) _temp539) == Cyc_Absyn_TunionType: 0){ _LL548: _temp547=(( struct Cyc_Absyn_TunionType_struct*)
_temp539)->f1; _LL554: _temp553=( void*) _temp547.tunion_info; goto _LL552;
_LL552: _temp551= _temp547.targs; goto _LL550; _LL550: _temp549=( void*)
_temp547.rgn; goto _LL542;} else{ goto _LL543;} _LL543: if(( unsigned int)
_temp539 > 4u?*(( int*) _temp539) == Cyc_Absyn_TunionFieldType: 0){ _LL556:
_temp555=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp539)->f1; goto _LL544;}
else{ goto _LL545;} _LL545: goto _LL546; _LL542: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp549); return; _LL544: return; _LL546:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp557=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp557[ 0]=({ struct
Cyc_Core_Impossible_struct _temp558; _temp558.tag= Cyc_Core_Impossible; _temp558.f1=({
unsigned char* _temp559=( unsigned char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp560; _temp560.curr= _temp559; _temp560.base= _temp559;
_temp560.last_plus_one= _temp559 + 35; _temp560;}); _temp558;}); _temp557;}));
_LL540:;} _LL482: _temp505=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple7*)) Cyc_Core_snd, _temp497); goto _LL484; _LL484: for( 0; _temp505
!= 0; _temp505=(( struct Cyc_List_List*) _check_null( _temp505))->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp505))->hd);}
return; _LL486: return; _LL476:;} static const int Cyc_Tcpat_Name_v= 0; struct
Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_string f1; } ; static const int
Cyc_Tcpat_Int_v= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; struct
Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ; static
const int Cyc_Tcpat_Any= 0; static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static int
Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){
void* _temp561=( void*) c1->name; struct _tagged_string _temp567; int _temp569;
_LL563: if(*(( int*) _temp561) == Cyc_Tcpat_Name_v){ _LL568: _temp567=(( struct
Cyc_Tcpat_Name_v_struct*) _temp561)->f1; goto _LL564;} else{ goto _LL565;}
_LL565: if(*(( int*) _temp561) == Cyc_Tcpat_Int_v){ _LL570: _temp569=(( struct
Cyc_Tcpat_Int_v_struct*) _temp561)->f1; goto _LL566;} else{ goto _LL562;} _LL564: {
void* _temp571=( void*) c2->name; struct _tagged_string _temp577; int _temp579;
_LL573: if(*(( int*) _temp571) == Cyc_Tcpat_Name_v){ _LL578: _temp577=(( struct
Cyc_Tcpat_Name_v_struct*) _temp571)->f1; goto _LL574;} else{ goto _LL575;}
_LL575: if(*(( int*) _temp571) == Cyc_Tcpat_Int_v){ _LL580: _temp579=(( struct
Cyc_Tcpat_Int_v_struct*) _temp571)->f1; goto _LL576;} else{ goto _LL572;} _LL574:
return Cyc_String_zstrcmp( _temp567, _temp577); _LL576: return - 1; _LL572:;}
_LL566: { void* _temp581=( void*) c2->name; struct _tagged_string _temp587; int
_temp589; _LL583: if(*(( int*) _temp581) == Cyc_Tcpat_Name_v){ _LL588: _temp587=((
struct Cyc_Tcpat_Name_v_struct*) _temp581)->f1; goto _LL584;} else{ goto _LL585;}
_LL585: if(*(( int*) _temp581) == Cyc_Tcpat_Int_v){ _LL590: _temp589=(( struct
Cyc_Tcpat_Int_v_struct*) _temp581)->f1; goto _LL586;} else{ goto _LL582;} _LL584:
return 1; _LL586: return _temp569 - _temp589; _LL582:;} _LL562:;} static struct
Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)( int(*
comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)( Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={( void*) 1}; static struct Cyc_Core_Opt
Cyc_Tcpat_two_opt={( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static unsigned char _temp594[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,{ _temp594,
_temp594, _temp594 + 5u}}; static unsigned char _temp598[ 4u]="ptr"; static
struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,{ _temp598,
_temp598, _temp598 + 4u}}; static unsigned char _temp602[ 6u]="tuple"; static
struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp602,
_temp602, _temp602 + 6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp603=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp603->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp604=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp604[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp605; _temp605.tag= Cyc_Tcpat_Int_v;
_temp605.f1= i; _temp605;}); _temp604;})); _temp603->arity= 0; _temp603->span= 0;
_temp603;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp606=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp606->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp607=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp607[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp608; _temp608.tag= Cyc_Tcpat_Name_v; _temp608.f1= f; _temp608;}); _temp607;}));
_temp606->arity= 0; _temp606->span= 0; _temp606;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp609=(
struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp609->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp611=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp611[ 0]=({ struct Cyc_Tcpat_Int_v_struct
_temp612; _temp612.tag= Cyc_Tcpat_Int_v; _temp612.f1=( int) c; _temp612;});
_temp611;})); _temp609->arity= 0; _temp609->span=({ struct Cyc_Core_Opt*
_temp610=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp610->v=( void*) 256; _temp610;}); _temp609;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s* _temp613=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp613->name=(
void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp613->arity= i; _temp613->span=
Cyc_Tcpat_one_opt_ptr; _temp613;});} static void* Cyc_Tcpat_null_pat(){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp614=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp614[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp615; _temp615.tag= Cyc_Tcpat_Con; _temp615.f1= Cyc_Tcpat_null_con; _temp615.f2=
0; _temp615;}); _temp614;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp616=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp616[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp617; _temp617.tag= Cyc_Tcpat_Con; _temp617.f1= Cyc_Tcpat_int_con( i);
_temp617.f2= 0; _temp617;}); _temp616;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp618=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp618[ 0]=({ struct Cyc_Tcpat_Con_struct _temp619; _temp619.tag= Cyc_Tcpat_Con;
_temp619.f1= Cyc_Tcpat_char_con( c); _temp619.f2= 0; _temp619;}); _temp618;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_string f){ return( void*)({
struct Cyc_Tcpat_Con_struct* _temp620=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp620[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp621; _temp621.tag= Cyc_Tcpat_Con; _temp621.f1= Cyc_Tcpat_float_con( f);
_temp621.f2= 0; _temp621;}); _temp620;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp622=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp622[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp623; _temp623.tag= Cyc_Tcpat_Con; _temp623.f1= Cyc_Tcpat_null_ptr_con;
_temp623.f2=({ struct Cyc_List_List* _temp624=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp624->hd=( void*) p; _temp624->tl= 0;
_temp624;}); _temp623;}); _temp622;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp625=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp625[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp626; _temp626.tag= Cyc_Tcpat_Con; _temp626.f1= Cyc_Tcpat_ptr_con; _temp626.f2=({
struct Cyc_List_List* _temp627=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp627->hd=( void*) p; _temp627->tl= 0; _temp627;});
_temp626;}); _temp625;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp628=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp628[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp629; _temp629.tag= Cyc_Tcpat_Con; _temp629.f1= Cyc_Tcpat_tuple_con((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ss)); _temp629.f2= ss; _temp629;});
_temp628;});} static void* Cyc_Tcpat_con_pat( struct _tagged_string con_name,
struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){ struct Cyc_Tcpat_Con_s* c=({
struct Cyc_Tcpat_Con_s* _temp632=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp632->name=( void*)(( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp633=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp633[ 0]=({ struct Cyc_Tcpat_Name_v_struct _temp634; _temp634.tag= Cyc_Tcpat_Name_v;
_temp634.f1= con_name; _temp634;}); _temp633;})); _temp632->arity=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ps); _temp632->span= span; _temp632;});
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp630=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp630[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp631; _temp631.tag= Cyc_Tcpat_Con; _temp631.f1= c; _temp631.f2= ps; _temp631;});
_temp630;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp635=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp669; int
_temp671; void* _temp673; unsigned char _temp675; struct _tagged_string _temp677;
struct Cyc_Absyn_Vardecl* _temp679; struct Cyc_Absyn_Pat* _temp681; struct Cyc_List_List*
_temp683; struct Cyc_List_List* _temp685; struct Cyc_Absyn_Tunionfield* _temp687;
struct Cyc_Absyn_Tuniondecl* _temp689; struct Cyc_List_List* _temp691; struct
Cyc_List_List* _temp693; struct Cyc_Absyn_Tunionfield* _temp695; struct Cyc_Absyn_Tuniondecl*
_temp697; struct Cyc_List_List* _temp699; struct Cyc_List_List* _temp701; struct
Cyc_List_List* _temp703; struct Cyc_Core_Opt* _temp705; struct Cyc_Absyn_Structdecl*
_temp707; struct Cyc_Absyn_Enumfield* _temp709; struct Cyc_Absyn_Enumdecl*
_temp711; struct _tuple1* _temp713; struct Cyc_List_List* _temp715; struct Cyc_List_List*
_temp717; struct _tuple1* _temp719; struct Cyc_List_List* _temp721; struct Cyc_List_List*
_temp723; struct _tuple1* _temp725; _LL637: if( _temp635 ==( void*) Cyc_Absyn_Wild_p){
goto _LL638;} else{ goto _LL639;} _LL639: if(( unsigned int) _temp635 > 2u?*((
int*) _temp635) == Cyc_Absyn_Var_p: 0){ _LL670: _temp669=(( struct Cyc_Absyn_Var_p_struct*)
_temp635)->f1; goto _LL640;} else{ goto _LL641;} _LL641: if( _temp635 ==( void*)
Cyc_Absyn_Null_p){ goto _LL642;} else{ goto _LL643;} _LL643: if(( unsigned int)
_temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_Int_p: 0){ _LL674: _temp673=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp635)->f1; goto _LL672; _LL672:
_temp671=(( struct Cyc_Absyn_Int_p_struct*) _temp635)->f2; goto _LL644;} else{
goto _LL645;} _LL645: if(( unsigned int) _temp635 > 2u?*(( int*) _temp635) ==
Cyc_Absyn_Char_p: 0){ _LL676: _temp675=(( struct Cyc_Absyn_Char_p_struct*)
_temp635)->f1; goto _LL646;} else{ goto _LL647;} _LL647: if(( unsigned int)
_temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_Float_p: 0){ _LL678: _temp677=((
struct Cyc_Absyn_Float_p_struct*) _temp635)->f1; goto _LL648;} else{ goto _LL649;}
_LL649: if(( unsigned int) _temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_Reference_p:
0){ _LL680: _temp679=(( struct Cyc_Absyn_Reference_p_struct*) _temp635)->f1;
goto _LL650;} else{ goto _LL651;} _LL651: if(( unsigned int) _temp635 > 2u?*((
int*) _temp635) == Cyc_Absyn_Pointer_p: 0){ _LL682: _temp681=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp635)->f1; goto _LL652;} else{ goto _LL653;} _LL653: if(( unsigned int)
_temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_Tunion_p: 0){ _LL690: _temp689=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f1; goto _LL688; _LL688: _temp687=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f2; goto _LL686; _LL686: _temp685=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f3; goto _LL684; _LL684: _temp683=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f4; if( _temp683 == 0){ goto
_LL654;} else{ goto _LL655;}} else{ goto _LL655;} _LL655: if(( unsigned int)
_temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_Tunion_p: 0){ _LL698: _temp697=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f1; goto _LL696; _LL696: _temp695=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f2; goto _LL694; _LL694: _temp693=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f3; goto _LL692; _LL692: _temp691=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f4; goto _LL656;} else{ goto
_LL657;} _LL657: if(( unsigned int) _temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_Tuple_p:
0){ _LL700: _temp699=(( struct Cyc_Absyn_Tuple_p_struct*) _temp635)->f1; goto
_LL658;} else{ goto _LL659;} _LL659: if(( unsigned int) _temp635 > 2u?*(( int*)
_temp635) == Cyc_Absyn_Struct_p: 0){ _LL708: _temp707=(( struct Cyc_Absyn_Struct_p_struct*)
_temp635)->f1; goto _LL706; _LL706: _temp705=(( struct Cyc_Absyn_Struct_p_struct*)
_temp635)->f2; goto _LL704; _LL704: _temp703=(( struct Cyc_Absyn_Struct_p_struct*)
_temp635)->f3; goto _LL702; _LL702: _temp701=(( struct Cyc_Absyn_Struct_p_struct*)
_temp635)->f4; goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int)
_temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_Enum_p: 0){ _LL712: _temp711=((
struct Cyc_Absyn_Enum_p_struct*) _temp635)->f1; goto _LL710; _LL710: _temp709=((
struct Cyc_Absyn_Enum_p_struct*) _temp635)->f2; goto _LL662;} else{ goto _LL663;}
_LL663: if(( unsigned int) _temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_UnknownId_p:
0){ _LL714: _temp713=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp635)->f1;
goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int) _temp635 > 2u?*((
int*) _temp635) == Cyc_Absyn_UnknownCall_p: 0){ _LL720: _temp719=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp635)->f1; goto _LL718; _LL718: _temp717=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp635)->f2; goto _LL716; _LL716: _temp715=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp635)->f3; goto _LL666;} else{ goto _LL667;} _LL667: if(( unsigned int)
_temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_UnknownFields_p: 0){ _LL726:
_temp725=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp635)->f1; goto _LL724;
_LL724: _temp723=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp635)->f2;
goto _LL722; _LL722: _temp721=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp635)->f3; goto _LL668;} else{ goto _LL636;} _LL638: s=( void*) Cyc_Tcpat_Any;
goto _LL636; _LL640: s=( void*) Cyc_Tcpat_Any; goto _LL636; _LL642: s= Cyc_Tcpat_null_pat();
goto _LL636; _LL644: s= Cyc_Tcpat_int_pat( _temp671); goto _LL636; _LL646: s=
Cyc_Tcpat_char_pat( _temp675); goto _LL636; _LL648: s= Cyc_Tcpat_float_pat(
_temp677); goto _LL636; _LL650: s=( void*) Cyc_Tcpat_Any; goto _LL636; _LL652:{
void* _temp727= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
p->topt))->v); struct Cyc_Absyn_PtrInfo _temp735; struct Cyc_Absyn_Conref*
_temp737; struct Cyc_Absyn_Tqual _temp739; struct Cyc_Absyn_Conref* _temp741;
void* _temp743; void* _temp745; struct Cyc_Absyn_TunionInfo _temp747; _LL729:
if(( unsigned int) _temp727 > 4u?*(( int*) _temp727) == Cyc_Absyn_PointerType: 0){
_LL736: _temp735=(( struct Cyc_Absyn_PointerType_struct*) _temp727)->f1; _LL746:
_temp745=( void*) _temp735.elt_typ; goto _LL744; _LL744: _temp743=( void*)
_temp735.rgn_typ; goto _LL742; _LL742: _temp741= _temp735.nullable; goto _LL740;
_LL740: _temp739= _temp735.tq; goto _LL738; _LL738: _temp737= _temp735.bounds;
goto _LL730;} else{ goto _LL731;} _LL731: if(( unsigned int) _temp727 > 4u?*((
int*) _temp727) == Cyc_Absyn_TunionType: 0){ _LL748: _temp747=(( struct Cyc_Absyn_TunionType_struct*)
_temp727)->f1; goto _LL732;} else{ goto _LL733;} _LL733: goto _LL734; _LL730: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp749=(
void*) _temp741->v; struct Cyc_Absyn_Conref* _temp757; int _temp759; _LL751: if((
unsigned int) _temp749 > 1u?*(( int*) _temp749) == Cyc_Absyn_Forward_constr: 0){
_LL758: _temp757=(( struct Cyc_Absyn_Forward_constr_struct*) _temp749)->f1; goto
_LL752;} else{ goto _LL753;} _LL753: if( _temp749 ==( void*) Cyc_Absyn_No_constr){
goto _LL754;} else{ goto _LL755;} _LL755: if(( unsigned int) _temp749 > 1u?*((
int*) _temp749) == Cyc_Absyn_Eq_constr: 0){ _LL760: _temp759=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp749)->f1; goto _LL756;} else{ goto _LL750;} _LL752:( void*)( _temp741->v=(
void*)(( void*) _temp757->v)); continue; _LL754:( void*)( _temp741->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp761=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp761[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp762; _temp762.tag= Cyc_Absyn_Eq_constr; _temp762.f1=(
void*) 0; _temp762;}); _temp761;}))); is_nullable= 0; still_working= 0; goto
_LL750; _LL756: is_nullable=( int) _temp759; still_working= 0; goto _LL750;
_LL750:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp681); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL728;}} _LL732:{ void* _temp763=(
void*) _temp681->r; struct Cyc_List_List* _temp769; struct Cyc_List_List*
_temp771; struct Cyc_Absyn_Tunionfield* _temp773; struct Cyc_Absyn_Tuniondecl*
_temp775; _LL765: if(( unsigned int) _temp763 > 2u?*(( int*) _temp763) == Cyc_Absyn_Tunion_p:
0){ _LL776: _temp775=(( struct Cyc_Absyn_Tunion_p_struct*) _temp763)->f1; goto
_LL774; _LL774: _temp773=(( struct Cyc_Absyn_Tunion_p_struct*) _temp763)->f2;
goto _LL772; _LL772: _temp771=(( struct Cyc_Absyn_Tunion_p_struct*) _temp763)->f3;
goto _LL770; _LL770: _temp769=(( struct Cyc_Absyn_Tunion_p_struct*) _temp763)->f4;
goto _LL766;} else{ goto _LL767;} _LL767: goto _LL768; _LL766: { struct Cyc_Core_Opt*
span; if( _temp775->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp777=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp777->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp775->fields))->v);
_temp777;});} s= Cyc_Tcpat_con_pat(*(* _temp773->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp769)); goto _LL764;} _LL768: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp778=( unsigned char*)"non-[x]tunion pattern has tunion type";
struct _tagged_string _temp779; _temp779.curr= _temp778; _temp779.base= _temp778;
_temp779.last_plus_one= _temp778 + 38; _temp779;})); goto _LL764; _LL764:;} goto
_LL728; _LL734: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp780=( unsigned char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp781; _temp781.curr= _temp780; _temp781.base= _temp780;
_temp781.last_plus_one= _temp780 + 44; _temp781;})); goto _LL728; _LL728:;} goto
_LL636; _LL654: { struct Cyc_Core_Opt* span;{ void* _temp782= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v); struct Cyc_Absyn_TunionInfo
_temp790; struct Cyc_Absyn_TunionFieldInfo _temp792; _LL784: if(( unsigned int)
_temp782 > 4u?*(( int*) _temp782) == Cyc_Absyn_TunionType: 0){ _LL791: _temp790=((
struct Cyc_Absyn_TunionType_struct*) _temp782)->f1; goto _LL785;} else{ goto
_LL786;} _LL786: if(( unsigned int) _temp782 > 4u?*(( int*) _temp782) == Cyc_Absyn_TunionFieldType:
0){ _LL793: _temp792=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp782)->f1;
goto _LL787;} else{ goto _LL788;} _LL788: goto _LL789; _LL785: if( _temp689->is_xtunion){
span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp794=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp794->v=( void*)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp689->fields))->v); _temp794;});} goto _LL783; _LL787: span=({
struct Cyc_Core_Opt* _temp795=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp795->v=( void*) 1; _temp795;}); goto _LL783; _LL789:
span=(( struct Cyc_Core_Opt*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp796=( unsigned char*)"void tunion pattern has bad type";
struct _tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 33; _temp797;})); goto _LL783; _LL783:;} s=
Cyc_Tcpat_con_pat(*(* _temp687->name).f2, span, 0); goto _LL636;} _LL656:
_temp699= _temp691; goto _LL658; _LL658: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp699)); goto _LL636; _LL660: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp707->fields))->v;
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fields))->hd)->name,({ unsigned char* _temp823=( unsigned char*)"";
struct _tagged_string _temp824; _temp824.curr= _temp823; _temp824.base= _temp823;
_temp824.last_plus_one= _temp823 + 1; _temp824;})) == 0;{ struct Cyc_List_List*
dlps0= _temp701; for( 0; ! found? dlps0 != 0: 0; dlps0=(( struct Cyc_List_List*)
_check_null( dlps0))->tl){ struct _tuple7 _temp800; struct Cyc_Absyn_Pat*
_temp801; struct Cyc_List_List* _temp803; struct _tuple7* _temp798=( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( dlps0))->hd; _temp800=* _temp798;
_LL804: _temp803= _temp800.f1; goto _LL802; _LL802: _temp801= _temp800.f2; goto
_LL799; _LL799: { struct Cyc_List_List* _temp805= _temp803; struct Cyc_List_List
_temp811; struct Cyc_List_List* _temp812; void* _temp814; struct _tagged_string*
_temp816; _LL807: if( _temp805 == 0){ goto _LL809;} else{ _temp811=* _temp805;
_LL815: _temp814=( void*) _temp811.hd; if(*(( int*) _temp814) == Cyc_Absyn_FieldName){
_LL817: _temp816=(( struct Cyc_Absyn_FieldName_struct*) _temp814)->f1; goto
_LL813;} else{ goto _LL809;} _LL813: _temp812= _temp811.tl; if( _temp812 == 0){
goto _LL808;} else{ goto _LL809;}} _LL809: goto _LL810; _LL808: if( Cyc_String_zstrptrcmp(
_temp816,(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd)->name) == 0){ ps=({ struct Cyc_List_List* _temp818=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp818->hd=( void*) Cyc_Tcpat_compile_pat(
_temp801); _temp818->tl= ps; _temp818;}); found= 1;} goto _LL806; _LL810:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp819=(
unsigned char*)"bad designator(s)"; struct _tagged_string _temp820; _temp820.curr=
_temp819; _temp820.base= _temp819; _temp820.last_plus_one= _temp819 + 18;
_temp820;})); goto _LL806; _LL806:;}}} if( ! found){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp821=( unsigned char*)"bad designator";
struct _tagged_string _temp822; _temp822.curr= _temp821; _temp822.base= _temp821;
_temp822.last_plus_one= _temp821 + 15; _temp822;}));}}} s= Cyc_Tcpat_tuple_pat(
ps); goto _LL636;} _LL662: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp711->fields))->v);
s= Cyc_Tcpat_con_pat(*(* _temp709->name).f2,({ struct Cyc_Core_Opt* _temp825=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp825->v=(
void*) span; _temp825;}), 0); goto _LL636;} _LL664: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp826=( unsigned char*)"compile_pat: unknownid";
struct _tagged_string _temp827; _temp827.curr= _temp826; _temp827.base= _temp826;
_temp827.last_plus_one= _temp826 + 23; _temp827;})); goto _LL636; _LL666: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp828=(
unsigned char*)"compile_pat: unknowncall"; struct _tagged_string _temp829;
_temp829.curr= _temp828; _temp829.base= _temp828; _temp829.last_plus_one=
_temp828 + 25; _temp829;})); goto _LL636; _LL668: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp830=( unsigned char*)"compile_pat: unknownfields";
struct _tagged_string _temp831; _temp831.curr= _temp830; _temp831.base= _temp830;
_temp831.last_plus_one= _temp830 + 27; _temp831;})); goto _LL636; _LL636:;}
return s;} static const int Cyc_Tcpat_Pos= 0; struct Cyc_Tcpat_Pos_struct{ int
tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag; struct Cyc_Set_Set* f1;
} ; static const int Cyc_Tcpat_Failure= 0; static const int Cyc_Tcpat_Success= 0;
struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s*
f2; void* f3; void* f4; } ; struct _tuple10{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static const int Cyc_Tcpat_Yes=
0; static const int Cyc_Tcpat_No= 1; static const int Cyc_Tcpat_Maybe= 2; static
void* Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp832=
td; struct Cyc_Set_Set* _temp838; struct Cyc_List_List* _temp840; struct Cyc_Tcpat_Con_s*
_temp842; _LL834: if(*(( int*) _temp832) == Cyc_Tcpat_Neg){ _LL839: _temp838=((
struct Cyc_Tcpat_Neg_struct*) _temp832)->f1; goto _LL835;} else{ goto _LL836;}
_LL836: if(*(( int*) _temp832) == Cyc_Tcpat_Pos){ _LL843: _temp842=(( struct Cyc_Tcpat_Pos_struct*)
_temp832)->f1; goto _LL841; _LL841: _temp840=(( struct Cyc_Tcpat_Pos_struct*)
_temp832)->f2; goto _LL837;} else{ goto _LL833;} _LL835: if((( int(*)( struct
Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp838, c)){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp844=(
unsigned char*)"add_neg called when constructor already in set"; struct
_tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 47; _temp845;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp838) + 1 >=( int)(( struct
Cyc_Core_Opt*) _check_null( c->span))->v: 0){(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp846=( unsigned char*)"add_neg called when |cs U {c}| >= span(c)";
struct _tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 42; _temp847;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp848=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp848[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp849; _temp849.tag= Cyc_Tcpat_Neg; _temp849.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp838,
c); _temp849;}); _temp848;}); _LL837: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp850=( unsigned char*)"add_neg called when td is Positive";
struct _tagged_string _temp851; _temp851.curr= _temp850; _temp851.base= _temp850;
_temp851.last_plus_one= _temp850 + 35; _temp851;})); _LL833:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp852= td; struct Cyc_List_List*
_temp858; struct Cyc_Tcpat_Con_s* _temp860; struct Cyc_Set_Set* _temp862; _LL854:
if(*(( int*) _temp852) == Cyc_Tcpat_Pos){ _LL861: _temp860=(( struct Cyc_Tcpat_Pos_struct*)
_temp852)->f1; goto _LL859; _LL859: _temp858=(( struct Cyc_Tcpat_Pos_struct*)
_temp852)->f2; goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp852)
== Cyc_Tcpat_Neg){ _LL863: _temp862=(( struct Cyc_Tcpat_Neg_struct*) _temp852)->f1;
goto _LL857;} else{ goto _LL853;} _LL855: if( Cyc_Tcpat_compare_con( c, _temp860)
== 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL857:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp862, c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)((
struct Cyc_Core_Opt*) _check_null( c->span))->v ==(( int(*)( struct Cyc_Set_Set*
s)) Cyc_Set_cardinality)( _temp862) + 1: 0){ return( void*) Cyc_Tcpat_Yes;}
else{ return( void*) Cyc_Tcpat_Maybe;}} _LL853:;} struct _tuple11{ struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; static struct Cyc_List_List* Cyc_Tcpat_augment(
struct Cyc_List_List* ctxt, void* dsc){ struct Cyc_List_List* _temp864= ctxt;
struct Cyc_List_List _temp870; struct Cyc_List_List* _temp871; struct _tuple11*
_temp873; struct _tuple11 _temp875; struct Cyc_List_List* _temp876; struct Cyc_Tcpat_Con_s*
_temp878; _LL866: if( _temp864 == 0){ goto _LL867;} else{ goto _LL868;} _LL868:
if( _temp864 == 0){ goto _LL865;} else{ _temp870=* _temp864; _LL874: _temp873=(
struct _tuple11*) _temp870.hd; _temp875=* _temp873; _LL879: _temp878= _temp875.f1;
goto _LL877; _LL877: _temp876= _temp875.f2; goto _LL872; _LL872: _temp871=
_temp870.tl; goto _LL869;} _LL867: return 0; _LL869: return({ struct Cyc_List_List*
_temp880=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp880->hd=( void*)({ struct _tuple11* _temp881=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp881->f1= _temp878; _temp881->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp882=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp882->hd=( void*) dsc; _temp882->tl= _temp876;
_temp882;}); _temp881;}); _temp880->tl= _temp871; _temp880;}); _LL865:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp883= ctxt; struct Cyc_List_List _temp889; struct Cyc_List_List*
_temp890; struct _tuple11* _temp892; struct _tuple11 _temp894; struct Cyc_List_List*
_temp895; struct Cyc_Tcpat_Con_s* _temp897; _LL885: if( _temp883 == 0){ goto
_LL886;} else{ goto _LL887;} _LL887: if( _temp883 == 0){ goto _LL884;} else{
_temp889=* _temp883; _LL893: _temp892=( struct _tuple11*) _temp889.hd; _temp894=*
_temp892; _LL898: _temp897= _temp894.f1; goto _LL896; _LL896: _temp895= _temp894.f2;
goto _LL891; _LL891: _temp890= _temp889.tl; goto _LL888;} _LL886: return((
struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp899=( unsigned char*)"norm_context: empty context"; struct
_tagged_string _temp900; _temp900.curr= _temp899; _temp900.base= _temp899;
_temp900.last_plus_one= _temp899 + 28; _temp900;})); _LL888: return Cyc_Tcpat_augment(
_temp890,( void*)({ struct Cyc_Tcpat_Pos_struct* _temp901=( struct Cyc_Tcpat_Pos_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Pos_struct)); _temp901[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp902; _temp902.tag= Cyc_Tcpat_Pos; _temp902.f1= _temp897; _temp902.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp895);
_temp902;}); _temp901;})); _LL884:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple6
_temp904=({ struct _tuple6 _temp903; _temp903.f1= ctxt; _temp903.f2= work;
_temp903;}); struct Cyc_List_List* _temp914; struct Cyc_List_List* _temp916;
struct Cyc_List_List* _temp918; struct Cyc_List_List* _temp920; struct Cyc_List_List*
_temp922; struct Cyc_List_List* _temp924; struct Cyc_List_List* _temp926; struct
Cyc_List_List _temp928; struct Cyc_List_List* _temp929; struct _tuple10*
_temp931; struct _tuple10 _temp933; struct Cyc_List_List* _temp934; struct Cyc_List_List*
_temp936; struct Cyc_List_List* _temp938; struct Cyc_List_List* _temp940; struct
Cyc_List_List _temp942; struct Cyc_List_List* _temp943; struct _tuple11*
_temp945; struct _tuple11 _temp947; struct Cyc_List_List* _temp948; struct Cyc_Tcpat_Con_s*
_temp950; _LL906: _LL917: _temp916= _temp904.f1; if( _temp916 == 0){ goto _LL915;}
else{ goto _LL908;} _LL915: _temp914= _temp904.f2; if( _temp914 == 0){ goto
_LL907;} else{ goto _LL908;} _LL908: _LL921: _temp920= _temp904.f1; if( _temp920
== 0){ goto _LL919;} else{ goto _LL910;} _LL919: _temp918= _temp904.f2; goto
_LL909; _LL910: _LL925: _temp924= _temp904.f1; goto _LL923; _LL923: _temp922=
_temp904.f2; if( _temp922 == 0){ goto _LL911;} else{ goto _LL912;} _LL912:
_LL941: _temp940= _temp904.f1; if( _temp940 == 0){ goto _LL905;} else{ _temp942=*
_temp940; _LL946: _temp945=( struct _tuple11*) _temp942.hd; _temp947=* _temp945;
_LL951: _temp950= _temp947.f1; goto _LL949; _LL949: _temp948= _temp947.f2; goto
_LL944; _LL944: _temp943= _temp942.tl; goto _LL927;} _LL927: _temp926= _temp904.f2;
if( _temp926 == 0){ goto _LL905;} else{ _temp928=* _temp926; _LL932: _temp931=(
struct _tuple10*) _temp928.hd; _temp933=* _temp931; _LL939: _temp938= _temp933.f1;
goto _LL937; _LL937: _temp936= _temp933.f2; goto _LL935; _LL935: _temp934=
_temp933.f3; goto _LL930; _LL930: _temp929= _temp928.tl; goto _LL913;} _LL907:
return dsc; _LL909: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp952=( unsigned char*)"build_desc: ctxt and work don't match";
struct _tagged_string _temp953; _temp953.curr= _temp952; _temp953.base= _temp952;
_temp953.last_plus_one= _temp952 + 38; _temp953;})); _LL911: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp954=(
unsigned char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp955; _temp955.curr= _temp954; _temp955.base= _temp954; _temp955.last_plus_one=
_temp954 + 38; _temp955;})); _LL913: { struct Cyc_Tcpat_Pos_struct* _temp959=({
struct Cyc_Tcpat_Pos_struct* _temp956=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp956[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp957; _temp957.tag= Cyc_Tcpat_Pos; _temp957.f1= _temp950; _temp957.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp948),({
struct Cyc_List_List* _temp958=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp958->hd=( void*) dsc; _temp958->tl= _temp934;
_temp958;})); _temp957;}); _temp956;}); goto _LL960; _LL960: return Cyc_Tcpat_build_desc(
_temp943,( void*) _temp959, _temp929);} _LL905:;} static void* Cyc_Tcpat_match(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules); static
void* Cyc_Tcpat_or_match( void* dsc, struct Cyc_List_List* allmrules){ struct
Cyc_List_List* _temp961= allmrules; struct Cyc_List_List _temp967; struct Cyc_List_List*
_temp968; struct _tuple0* _temp970; struct _tuple0 _temp972; void* _temp973;
void* _temp975; _LL963: if( _temp961 == 0){ goto _LL964;} else{ goto _LL965;}
_LL965: if( _temp961 == 0){ goto _LL962;} else{ _temp967=* _temp961; _LL971:
_temp970=( struct _tuple0*) _temp967.hd; _temp972=* _temp970; _LL976: _temp975=
_temp972.f1; goto _LL974; _LL974: _temp973= _temp972.f2; goto _LL969; _LL969:
_temp968= _temp967.tl; goto _LL966;} _LL964: return( void*) Cyc_Tcpat_Failure;
_LL966: return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp975, 0, dsc, 0, 0, _temp973,
_temp968); _LL962:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List*
allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp977=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp977[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp978; _temp978.tag= Cyc_Tcpat_Neg; _temp978.f1= Cyc_Tcpat_empty_con_set();
_temp978;}); _temp977;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp979= work; struct Cyc_List_List
_temp987; struct Cyc_List_List* _temp988; struct _tuple10* _temp990; struct
_tuple10 _temp992; struct Cyc_List_List* _temp993; struct Cyc_List_List*
_temp995; struct Cyc_List_List* _temp997; struct Cyc_List_List _temp999; struct
Cyc_List_List* _temp1000; struct _tuple10* _temp1002; struct _tuple10 _temp1004;
struct Cyc_List_List* _temp1005; struct Cyc_List_List* _temp1007; struct Cyc_List_List*
_temp1009; _LL981: if( _temp979 == 0){ goto _LL982;} else{ goto _LL983;} _LL983:
if( _temp979 == 0){ goto _LL985;} else{ _temp987=* _temp979; _LL991: _temp990=(
struct _tuple10*) _temp987.hd; _temp992=* _temp990; _LL998: _temp997= _temp992.f1;
if( _temp997 == 0){ goto _LL996;} else{ goto _LL985;} _LL996: _temp995= _temp992.f2;
if( _temp995 == 0){ goto _LL994;} else{ goto _LL985;} _LL994: _temp993= _temp992.f3;
if( _temp993 == 0){ goto _LL989;} else{ goto _LL985;} _LL989: _temp988= _temp987.tl;
goto _LL984;} _LL985: if( _temp979 == 0){ goto _LL980;} else{ _temp999=*
_temp979; _LL1003: _temp1002=( struct _tuple10*) _temp999.hd; _temp1004=*
_temp1002; _LL1010: _temp1009= _temp1004.f1; goto _LL1008; _LL1008: _temp1007=
_temp1004.f2; goto _LL1006; _LL1006: _temp1005= _temp1004.f3; goto _LL1001;
_LL1001: _temp1000= _temp999.tl; goto _LL986;} _LL982: return( void*)({ struct
Cyc_Tcpat_Success_struct* _temp1011=( struct Cyc_Tcpat_Success_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct)); _temp1011[ 0]=({ struct
Cyc_Tcpat_Success_struct _temp1012; _temp1012.tag= Cyc_Tcpat_Success; _temp1012.f1=(
void*) right_hand_side; _temp1012;}); _temp1011;}); _LL984: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context( ctx),
_temp988, right_hand_side, rules); _LL986: if(( _temp1009 == 0? 1: _temp1007 ==
0)? 1: _temp1005 == 0){ return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp1013=( unsigned char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1014; _temp1014.curr= _temp1013; _temp1014.base=
_temp1013; _temp1014.last_plus_one= _temp1013 + 38; _temp1014;}));}{ struct Cyc_List_List
_temp1017; struct Cyc_List_List* _temp1018; void* _temp1020; struct Cyc_List_List*
_temp1015=( struct Cyc_List_List*) _check_null( _temp1009); _temp1017=*
_temp1015; _LL1021: _temp1020=( void*) _temp1017.hd; goto _LL1019; _LL1019:
_temp1018= _temp1017.tl; goto _LL1016; _LL1016: { struct Cyc_List_List _temp1024;
struct Cyc_List_List* _temp1025; struct Cyc_List_List* _temp1027; struct Cyc_List_List*
_temp1022=( struct Cyc_List_List*) _check_null( _temp1007); _temp1024=*
_temp1022; _LL1028: _temp1027=( struct Cyc_List_List*) _temp1024.hd; goto
_LL1026; _LL1026: _temp1025= _temp1024.tl; goto _LL1023; _LL1023: { struct Cyc_List_List
_temp1031; struct Cyc_List_List* _temp1032; void* _temp1034; struct Cyc_List_List*
_temp1029=( struct Cyc_List_List*) _check_null( _temp1005); _temp1031=*
_temp1029; _LL1035: _temp1034=( void*) _temp1031.hd; goto _LL1033; _LL1033:
_temp1032= _temp1031.tl; goto _LL1030; _LL1030: { struct _tuple10* _temp1037=({
struct _tuple10* _temp1036=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp1036->f1= _temp1018; _temp1036->f2= _temp1025; _temp1036->f3=
_temp1032; _temp1036;}); goto _LL1038; _LL1038: return(( void*(*)( void* p,
struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)(
_temp1020, _temp1027, _temp1034, ctx,({ struct Cyc_List_List* _temp1039=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1039->hd=( void*)
_temp1037; _temp1039->tl= _temp1000; _temp1039;}), right_hand_side, rules);}}}}
_LL980:;} static struct Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s*
pcon, void* dsc){ void* _temp1040= dsc; struct Cyc_Set_Set* _temp1046; struct
Cyc_List_List* _temp1048; struct Cyc_Tcpat_Con_s* _temp1050; _LL1042: if(*(( int*)
_temp1040) == Cyc_Tcpat_Neg){ _LL1047: _temp1046=(( struct Cyc_Tcpat_Neg_struct*)
_temp1040)->f1; goto _LL1043;} else{ goto _LL1044;} _LL1044: if(*(( int*)
_temp1040) == Cyc_Tcpat_Pos){ _LL1051: _temp1050=(( struct Cyc_Tcpat_Pos_struct*)
_temp1040)->f1; goto _LL1049; _LL1049: _temp1048=(( struct Cyc_Tcpat_Pos_struct*)
_temp1040)->f2; goto _LL1045;} else{ goto _LL1041;} _LL1043: { void* any=( void*)({
struct Cyc_Tcpat_Neg_struct* _temp1055=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1055[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1056; _temp1056.tag= Cyc_Tcpat_Neg; _temp1056.f1= Cyc_Tcpat_empty_con_set();
_temp1056;}); _temp1055;}); struct Cyc_List_List* _temp1052= 0; goto _LL1053;
_LL1053:{ int i= 0; for( 0; i < pcon->arity; ++ i){ _temp1052=({ struct Cyc_List_List*
_temp1054=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1054->hd=( void*) any; _temp1054->tl= _temp1052; _temp1054;});}} return
_temp1052;} _LL1045: return _temp1048; _LL1041:;} static struct Cyc_List_List*
Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){ return({ struct Cyc_List_List*
_temp1057=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1057->hd=( void*)( i + 1); _temp1057->tl= obj; _temp1057;});} static struct
Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s* pcon, struct Cyc_List_List*
obj){ return(( struct Cyc_List_List*(*)( int n, struct Cyc_List_List*(* f)(
struct Cyc_List_List*, int), struct Cyc_List_List* env)) Cyc_List_tabulate_c)(
pcon->arity, Cyc_Tcpat_getoarg, obj);} static void* Cyc_Tcpat_match( void* p,
struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules){ void* _temp1058= p;
struct Cyc_List_List* _temp1064; struct Cyc_Tcpat_Con_s* _temp1066; _LL1060: if(
_temp1058 ==( void*) Cyc_Tcpat_Any){ goto _LL1061;} else{ goto _LL1062;} _LL1062:
if(( unsigned int) _temp1058 > 1u?*(( int*) _temp1058) == Cyc_Tcpat_Con: 0){
_LL1067: _temp1066=(( struct Cyc_Tcpat_Con_struct*) _temp1058)->f1; goto _LL1065;
_LL1065: _temp1064=(( struct Cyc_Tcpat_Con_struct*) _temp1058)->f2; goto _LL1063;}
else{ goto _LL1059;} _LL1061: return(( void*(*)( struct Cyc_List_List* ctx,
struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules))
Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc), work, right_hand_side, rules);
_LL1063: { void* _temp1068= Cyc_Tcpat_static_match( _temp1066, dsc); _LL1070:
if( _temp1068 ==( void*) Cyc_Tcpat_Yes){ goto _LL1071;} else{ goto _LL1072;}
_LL1072: if( _temp1068 ==( void*) Cyc_Tcpat_No){ goto _LL1073;} else{ goto
_LL1074;} _LL1074: if( _temp1068 ==( void*) Cyc_Tcpat_Maybe){ goto _LL1075;}
else{ goto _LL1069;} _LL1071: { struct Cyc_List_List* _temp1078=({ struct Cyc_List_List*
_temp1076=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1076->hd=( void*)({ struct _tuple11* _temp1077=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp1077->f1= _temp1066; _temp1077->f2= 0;
_temp1077;}); _temp1076->tl= ctx; _temp1076;}); goto _LL1079; _LL1079: { struct
_tuple10* _temp1081=({ struct _tuple10* _temp1080=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp1080->f1= _temp1064; _temp1080->f2= Cyc_Tcpat_getoargs(
_temp1066, obj); _temp1080->f3= Cyc_Tcpat_getdargs( _temp1066, dsc); _temp1080;});
goto _LL1082; _LL1082: { struct Cyc_List_List* _temp1084=({ struct Cyc_List_List*
_temp1083=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1083->hd=( void*) _temp1081; _temp1083->tl= work; _temp1083;}); goto
_LL1085; _LL1085: return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
_temp1078, _temp1084, right_hand_side, rules);}}} _LL1073: return(( void*(*)(
void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, dsc, work), rules); _LL1075: { struct Cyc_List_List* _temp1088=({ struct
Cyc_List_List* _temp1086=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1086->hd=( void*)({ struct _tuple11* _temp1087=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp1087->f1= _temp1066; _temp1087->f2= 0;
_temp1087;}); _temp1086->tl= ctx; _temp1086;}); goto _LL1089; _LL1089: { struct
_tuple10* _temp1091=({ struct _tuple10* _temp1090=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp1090->f1= _temp1064; _temp1090->f2= Cyc_Tcpat_getoargs(
_temp1066, obj); _temp1090->f3= Cyc_Tcpat_getdargs( _temp1066, dsc); _temp1090;});
goto _LL1092; _LL1092: { struct Cyc_List_List* _temp1094=({ struct Cyc_List_List*
_temp1093=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1093->hd=( void*) _temp1091; _temp1093->tl= work; _temp1093;}); goto
_LL1095; _LL1095: { void* _temp1096=(( void*(*)( struct Cyc_List_List* ctx,
struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules))
Cyc_Tcpat_and_match)( _temp1088, _temp1094, right_hand_side, rules); goto
_LL1097; _LL1097: { void* _temp1098=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1066), work), rules); goto _LL1099; _LL1099: return( void*)({ struct
Cyc_Tcpat_IfEq_struct* _temp1100=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_IfEq_struct)); _temp1100[ 0]=({ struct Cyc_Tcpat_IfEq_struct
_temp1101; _temp1101.tag= Cyc_Tcpat_IfEq; _temp1101.f1= obj; _temp1101.f2=
_temp1066; _temp1101.f3=( void*) _temp1096; _temp1101.f4=( void*) _temp1098;
_temp1101;}); _temp1100;});}}}}} _LL1069:;} _LL1059:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp1102= d; void* _temp1110; void* _temp1112; void*
_temp1114; struct Cyc_Tcpat_Con_s* _temp1116; struct Cyc_List_List* _temp1118;
_LL1104: if( _temp1102 ==( void*) Cyc_Tcpat_Failure){ goto _LL1105;} else{ goto
_LL1106;} _LL1106: if(( unsigned int) _temp1102 > 1u?*(( int*) _temp1102) == Cyc_Tcpat_Success:
0){ _LL1111: _temp1110=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1102)->f1;
goto _LL1107;} else{ goto _LL1108;} _LL1108: if(( unsigned int) _temp1102 > 1u?*((
int*) _temp1102) == Cyc_Tcpat_IfEq: 0){ _LL1119: _temp1118=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1102)->f1; goto _LL1117; _LL1117: _temp1116=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1102)->f2; goto _LL1115; _LL1115: _temp1114=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1102)->f3; goto _LL1113; _LL1113: _temp1112=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1102)->f4; goto _LL1109;} else{ goto _LL1103;} _LL1105: not_exhaust( env1);
goto _LL1103; _LL1107: rhs_appears( env2, _temp1110); goto _LL1103; _LL1109:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1114,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1112, not_exhaust, env1, rhs_appears, env2); goto _LL1103; _LL1103:;}
struct _tuple12{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple13{
void* f1; struct _tuple12* f2; } ; static struct _tuple13* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple12* rhs=({ struct _tuple12* _temp1125=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1125->f1= 0; _temp1125->f2=(
swc->pattern)->loc; _temp1125;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1120=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1120->hd=( void*) sp0; _temp1120->tl=({ struct Cyc_List_List*
_temp1121=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1121->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1121->tl= 0; _temp1121;});
_temp1120;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1122=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1122->hd=( void*) sp0; _temp1122->tl=({ struct Cyc_List_List* _temp1123=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1123->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1123->tl= 0; _temp1123;}); _temp1122;}));}
return({ struct _tuple13* _temp1124=( struct _tuple13*) GC_malloc( sizeof(
struct _tuple13)); _temp1124->f1= sp; _temp1124->f2= rhs; _temp1124;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp1126=( unsigned char*)"patterns may not be exhaustive";
struct _tagged_string _temp1127; _temp1127.curr= _temp1126; _temp1127.base=
_temp1126; _temp1127.last_plus_one= _temp1126 + 31; _temp1127;}));} static void
Cyc_Tcpat_rule_occurs( int dummy, struct _tuple12* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp1128= 0;
goto _LL1129; _LL1129: { struct Cyc_List_List* _temp1130=(( struct Cyc_List_List*(*)(
struct _tuple13*(* f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,& _temp1128, swcs); goto
_LL1131; _LL1131: { void* _temp1132=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1130); goto _LL1133; _LL1133:(( void(*)( void* d,
void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1132, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1130 != 0; _temp1130=(( struct Cyc_List_List*) _check_null( _temp1130))->tl){
struct _tuple13 _temp1136; struct _tuple12* _temp1137; struct _tuple12 _temp1139;
struct Cyc_Position_Segment* _temp1140; int _temp1142; void* _temp1144; struct
_tuple13* _temp1134=( struct _tuple13*)(( struct Cyc_List_List*) _check_null(
_temp1130))->hd; _temp1136=* _temp1134; _LL1145: _temp1144= _temp1136.f1; goto
_LL1138; _LL1138: _temp1137= _temp1136.f2; _temp1139=* _temp1137; _LL1143:
_temp1142= _temp1139.f1; goto _LL1141; _LL1141: _temp1140= _temp1139.f2; goto
_LL1135; _LL1135: if( ! _temp1142){ Cyc_Tcutil_terr( _temp1140,({ unsigned char*
_temp1146=( unsigned char*)"redundant pattern"; struct _tagged_string _temp1147;
_temp1147.curr= _temp1146; _temp1147.base= _temp1146; _temp1147.last_plus_one=
_temp1146 + 18; _temp1147;}));}}}}} struct _tuple14{ struct Cyc_Position_Segment*
f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple14* pr){(*
pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,({ unsigned char* _temp1148=( unsigned char*)"pattern not exhaustive";
struct _tagged_string _temp1149; _temp1149.curr= _temp1148; _temp1149.base=
_temp1148; _temp1149.last_plus_one= _temp1148 + 23; _temp1149;}));} static void
Cyc_Tcpat_dummy_fn( int i, int j){ return;} struct _tuple15{ void* f1; int f2; }
; int Cyc_Tcpat_check_let_pat_exhaustive( struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Pat* p){ struct Cyc_List_List* _temp1152=({ struct Cyc_List_List*
_temp1150=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1150->hd=( void*)({ struct _tuple15* _temp1151=( struct _tuple15*)
GC_malloc( sizeof( struct _tuple15)); _temp1151->f1= Cyc_Tcpat_compile_pat( p);
_temp1151->f2= 0; _temp1151;}); _temp1150->tl= 0; _temp1150;}); goto _LL1153;
_LL1153: { void* _temp1154=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp1152); goto _LL1155; _LL1155: { struct _tuple14* _temp1157=({ struct
_tuple14* _temp1156=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp1156->f1= loc; _temp1156->f2= 1; _temp1156;}); goto _LL1158; _LL1158:((
void(*)( void* d, void(* not_exhaust)( struct _tuple14*), struct _tuple14* env1,
void(* rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1154, Cyc_Tcpat_not_exhaust_warn, _temp1157, Cyc_Tcpat_dummy_fn, 0); return(*
_temp1157).f2;}}} static struct _tuple13* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple12* rhs=({
struct _tuple12* _temp1160=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp1160->f1= 0; _temp1160->f2=( swc->pattern)->loc; _temp1160;});
return({ struct _tuple13* _temp1159=( struct _tuple13*) GC_malloc( sizeof(
struct _tuple13)); _temp1159->f1= sp0; _temp1159->f2= rhs; _temp1159;});} static
void Cyc_Tcpat_not_exhaust_err2( struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp1161=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); goto
_LL1162; _LL1162: { void* _temp1163=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1161); goto _LL1164; _LL1164:(( void(*)( void* d,
void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1163, Cyc_Tcpat_not_exhaust_err2, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1161 != 0; _temp1161=(( struct Cyc_List_List*) _check_null( _temp1161))->tl){
struct _tuple13 _temp1167; struct _tuple12* _temp1168; struct _tuple12 _temp1170;
struct Cyc_Position_Segment* _temp1171; int _temp1173; void* _temp1175; struct
_tuple13* _temp1165=( struct _tuple13*)(( struct Cyc_List_List*) _check_null(
_temp1161))->hd; _temp1167=* _temp1165; _LL1176: _temp1175= _temp1167.f1; goto
_LL1169; _LL1169: _temp1168= _temp1167.f2; _temp1170=* _temp1168; _LL1174:
_temp1173= _temp1170.f1; goto _LL1172; _LL1172: _temp1171= _temp1170.f2; goto
_LL1166; _LL1166: if( ! _temp1173){ Cyc_Tcutil_terr( _temp1171,({ unsigned char*
_temp1177=( unsigned char*)"redundant pattern"; struct _tagged_string _temp1178;
_temp1178.curr= _temp1177; _temp1178.base= _temp1177; _temp1178.last_plus_one=
_temp1177 + 18; _temp1178;}));}}}}