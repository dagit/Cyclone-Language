 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern int
Cyc_Core_intcmp( int, int); extern int Cyc_Core_ptrcmp( void**, void**); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
extern int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List* l,
void* x); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern struct Cyc_Set_Set* Cyc_Set_delete(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_is_empty( struct Cyc_Set_Set*
s); extern unsigned char Cyc_Set_Absent[ 11u]; extern void* Cyc_Set_choose(
struct Cyc_Set_Set* s); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter_c( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern int Cyc_String_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct
_tagged_arr*); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp( void*, void*);
extern int Cyc_CfFlowInfo_field_cmp( void*, void*); extern int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*,
void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_noneil={
0u,( void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_unes_thisil={ 0u,( void*)(( void*) 1u),( void*)(( void*)
1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_allil={
0u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_noneil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_thisil={
0u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_allil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
2u)}; static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_none_v={
0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_noneil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_this_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_thisil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_all_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_allil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_none_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_noneil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_this_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_thisil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_all_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_allil)};
void* Cyc_CfFlowInfo_leaf_unesc_none=( void*)& Cyc_CfFlowInfo_leaf_unesc_none_v;
void* Cyc_CfFlowInfo_leaf_unesc_this=( void*)& Cyc_CfFlowInfo_leaf_unesc_this_v;
void* Cyc_CfFlowInfo_leaf_unesc_all=( void*)& Cyc_CfFlowInfo_leaf_unesc_all_v;
void* Cyc_CfFlowInfo_leaf_esc_none=( void*)& Cyc_CfFlowInfo_leaf_esc_none_v;
void* Cyc_CfFlowInfo_leaf_esc_this=( void*)& Cyc_CfFlowInfo_leaf_esc_this_v;
void* Cyc_CfFlowInfo_leaf_esc_all=( void*)& Cyc_CfFlowInfo_leaf_esc_all_v;
struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfFlowInfo_mkLeafPI( void* esc,
void* il){ struct _tuple3 _temp13=({ struct _tuple3 _temp12; _temp12.f1= esc;
_temp12.f2= il; _temp12;}); void* _temp27; void* _temp29; void* _temp31; void*
_temp33; void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; _LL15: _LL30: _temp29=
_temp13.f1; if( _temp29 ==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL28;} else{
goto _LL17;} _LL28: _temp27= _temp13.f2; if( _temp27 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL16;} else{ goto _LL17;} _LL17: _LL34: _temp33= _temp13.f1; if( _temp33
==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL32;} else{ goto _LL19;} _LL32: _temp31=
_temp13.f2; if( _temp31 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL18;} else{
goto _LL19;} _LL19: _LL38: _temp37= _temp13.f1; if( _temp37 ==( void*) Cyc_CfFlowInfo_Unesc){
goto _LL36;} else{ goto _LL21;} _LL36: _temp35= _temp13.f2; if( _temp35 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL20;} else{ goto _LL21;} _LL21: _LL42: _temp41=
_temp13.f1; if( _temp41 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL40;} else{ goto
_LL23;} _LL40: _temp39= _temp13.f2; if( _temp39 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL22;} else{ goto _LL23;} _LL23: _LL46: _temp45= _temp13.f1; if( _temp45
==( void*) Cyc_CfFlowInfo_Esc){ goto _LL44;} else{ goto _LL25;} _LL44: _temp43=
_temp13.f2; if( _temp43 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL24;} else{
goto _LL25;} _LL25: _LL50: _temp49= _temp13.f1; if( _temp49 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL48;} else{ goto _LL14;} _LL48: _temp47= _temp13.f2; if( _temp47 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL26;} else{ goto _LL14;} _LL16: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL18: return Cyc_CfFlowInfo_leaf_unesc_this; _LL20: return Cyc_CfFlowInfo_leaf_unesc_all;
_LL22: return Cyc_CfFlowInfo_leaf_esc_none; _LL24: return Cyc_CfFlowInfo_leaf_esc_this;
_LL26: return Cyc_CfFlowInfo_leaf_esc_all; _LL14:;} int Cyc_CfFlowInfo_local_root_cmp(
void* r1, void* r2){ if( r1 == r2){ return 0;}{ struct _tuple3 _temp52=({ struct
_tuple3 _temp51; _temp51.f1= r1; _temp51.f2= r2; _temp51;}); void* _temp62;
struct Cyc_Absyn_Exp* _temp64; void* _temp66; struct Cyc_Absyn_Vardecl* _temp68;
void* _temp70; struct Cyc_Absyn_Vardecl* _temp72; void* _temp74; struct Cyc_Absyn_Exp*
_temp76; void* _temp78; struct Cyc_Absyn_Vardecl* _temp80; void* _temp82; struct
Cyc_Absyn_Vardecl* _temp84; void* _temp86; struct Cyc_Absyn_Exp* _temp88; void*
_temp90; struct Cyc_Absyn_Exp* _temp92; _LL54: _LL67: _temp66= _temp52.f1; if(*((
int*) _temp66) == Cyc_CfFlowInfo_VarRoot){ _LL69: _temp68=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp66)->f1; goto _LL63;} else{ goto _LL56;} _LL63: _temp62= _temp52.f2; if(*((
int*) _temp62) == Cyc_CfFlowInfo_MallocPt){ _LL65: _temp64=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp62)->f1; goto _LL55;} else{ goto _LL56;} _LL56: _LL75: _temp74= _temp52.f1;
if(*(( int*) _temp74) == Cyc_CfFlowInfo_MallocPt){ _LL77: _temp76=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp74)->f1; goto _LL71;} else{ goto _LL58;} _LL71: _temp70= _temp52.f2; if(*((
int*) _temp70) == Cyc_CfFlowInfo_VarRoot){ _LL73: _temp72=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp70)->f1; goto _LL57;} else{ goto _LL58;} _LL58: _LL83: _temp82= _temp52.f1;
if(*(( int*) _temp82) == Cyc_CfFlowInfo_VarRoot){ _LL85: _temp84=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL60;} _LL79: _temp78= _temp52.f2; if(*((
int*) _temp78) == Cyc_CfFlowInfo_VarRoot){ _LL81: _temp80=(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp78)->f1; goto _LL59;} else{ goto _LL60;} _LL60: _LL91: _temp90= _temp52.f1;
if(*(( int*) _temp90) == Cyc_CfFlowInfo_MallocPt){ _LL93: _temp92=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp90)->f1; goto _LL87;} else{ goto _LL53;} _LL87: _temp86= _temp52.f2; if(*((
int*) _temp86) == Cyc_CfFlowInfo_MallocPt){ _LL89: _temp88=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp86)->f1; goto _LL61;} else{ goto _LL53;} _LL55: return 1; _LL57: return - 1;
_LL59: return(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp)( _temp84, _temp80); _LL61: return(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp)( _temp92, _temp88); _LL53:;}} int Cyc_CfFlowInfo_field_cmp(
void* f1, void* f2){ if( f1 == f2){ return 0;}{ struct _tuple3 _temp95=({ struct
_tuple3 _temp94; _temp94.f1= f1; _temp94.f2= f2; _temp94;}); void* _temp105; int
_temp107; void* _temp109; struct _tagged_arr* _temp111; void* _temp113; struct
_tagged_arr* _temp115; void* _temp117; int _temp119; void* _temp121; struct
_tagged_arr* _temp123; void* _temp125; struct _tagged_arr* _temp127; void*
_temp129; int _temp131; void* _temp133; int _temp135; _LL97: _LL110: _temp109=
_temp95.f1; if(*(( int*) _temp109) == Cyc_CfFlowInfo_StructF){ _LL112: _temp111=((
struct Cyc_CfFlowInfo_StructF_struct*) _temp109)->f1; goto _LL106;} else{ goto
_LL99;} _LL106: _temp105= _temp95.f2; if(*(( int*) _temp105) == Cyc_CfFlowInfo_TupleF){
_LL108: _temp107=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp105)->f1; goto
_LL98;} else{ goto _LL99;} _LL99: _LL118: _temp117= _temp95.f1; if(*(( int*)
_temp117) == Cyc_CfFlowInfo_TupleF){ _LL120: _temp119=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp117)->f1; goto _LL114;} else{ goto _LL101;} _LL114: _temp113= _temp95.f2;
if(*(( int*) _temp113) == Cyc_CfFlowInfo_StructF){ _LL116: _temp115=(( struct
Cyc_CfFlowInfo_StructF_struct*) _temp113)->f1; goto _LL100;} else{ goto _LL101;}
_LL101: _LL126: _temp125= _temp95.f1; if(*(( int*) _temp125) == Cyc_CfFlowInfo_StructF){
_LL128: _temp127=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp125)->f1; goto
_LL122;} else{ goto _LL103;} _LL122: _temp121= _temp95.f2; if(*(( int*) _temp121)
== Cyc_CfFlowInfo_StructF){ _LL124: _temp123=(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp121)->f1; goto _LL102;} else{ goto _LL103;} _LL103: _LL134: _temp133=
_temp95.f1; if(*(( int*) _temp133) == Cyc_CfFlowInfo_TupleF){ _LL136: _temp135=((
struct Cyc_CfFlowInfo_TupleF_struct*) _temp133)->f1; goto _LL130;} else{ goto
_LL96;} _LL130: _temp129= _temp95.f2; if(*(( int*) _temp129) == Cyc_CfFlowInfo_TupleF){
_LL132: _temp131=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp129)->f1; goto
_LL104;} else{ goto _LL96;} _LL98: return 1; _LL100: return - 1; _LL102: return((
int(*)( struct _tagged_arr*, struct _tagged_arr*)) Cyc_Core_ptrcmp)( _temp127,
_temp123); _LL104: return _temp135 - _temp131; _LL96:;}} int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place* p1, struct Cyc_CfFlowInfo_Place* p2){ if( p1 == p2){
return 0;}{ int i= Cyc_CfFlowInfo_local_root_cmp(( void*) p1->root,( void*) p2->root);
if( i != 0){ return i;} return(( int(*)( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_CfFlowInfo_field_cmp, p1->fields,
p2->fields);}} struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; void*
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t){ void* _temp137= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_TunionFieldInfo _temp149; struct Cyc_List_List* _temp151;
void* _temp153; struct Cyc_Absyn_Tunionfield* _temp155; struct Cyc_Absyn_Tuniondecl*
_temp157; struct Cyc_List_List* _temp159; struct Cyc_List_List* _temp161; struct
Cyc_Absyn_Structdecl** _temp163; struct Cyc_Absyn_Structdecl* _temp165; struct
Cyc_Absyn_Structdecl _temp166; struct Cyc_List_List* _temp167; struct Cyc_Core_Opt*
_temp169; struct Cyc_Core_Opt _temp171; struct Cyc_List_List* _temp172; struct
Cyc_List_List* _temp174; struct Cyc_Core_Opt* _temp176; void* _temp178; struct
Cyc_List_List* _temp180; struct _tuple0* _temp182; _LL139: if(( unsigned int)
_temp137 > 4u?*(( int*) _temp137) == Cyc_Absyn_TunionFieldType: 0){ _LL150:
_temp149=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp137)->f1; _LL154:
_temp153=( void*) _temp149.field_info; if(*(( int*) _temp153) == Cyc_Absyn_KnownTunionfield){
_LL158: _temp157=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp153)->f1;
goto _LL156; _LL156: _temp155=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp153)->f2; goto _LL152;} else{ goto _LL141;} _LL152: _temp151= _temp149.targs;
goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 4u?*((
int*) _temp137) == Cyc_Absyn_TupleType: 0){ _LL160: _temp159=(( struct Cyc_Absyn_TupleType_struct*)
_temp137)->f1; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp137 > 4u?*(( int*) _temp137) == Cyc_Absyn_AnonStructType: 0){ _LL162:
_temp161=(( struct Cyc_Absyn_AnonStructType_struct*) _temp137)->f1; goto _LL144;}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp137 > 4u?*(( int*) _temp137)
== Cyc_Absyn_StructType: 0){ _LL183: _temp182=(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f1; goto _LL181; _LL181: _temp180=(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f2; goto _LL164; _LL164: _temp163=(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f3; if( _temp163 == 0){ goto _LL147;} else{ _temp165=* _temp163;
_temp166=* _temp165; _LL179: _temp178=( void*) _temp166.sc; goto _LL177; _LL177:
_temp176= _temp166.name; goto _LL175; _LL175: _temp174= _temp166.tvs; goto
_LL170; _LL170: _temp169= _temp166.fields; if( _temp169 == 0){ goto _LL147;}
else{ _temp171=* _temp169; _LL173: _temp172=( struct Cyc_List_List*) _temp171.v;
goto _LL168;} _LL168: _temp167= _temp166.attributes; goto _LL146;}} else{ goto
_LL147;} _LL147: goto _LL148; _LL140: if( _temp155->typs == 0){ return Cyc_CfFlowInfo_leaf_unesc_none;}
_temp159= _temp155->typs; goto _LL142; _LL142: { struct Cyc_Dict_Dict* _temp184=((
struct Cyc_Dict_Dict*(*)( int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);
goto _LL185; _LL185:{ int i= 0; for( 0; _temp159 != 0;( _temp159=(( struct Cyc_List_List*)
_check_null( _temp159))->tl, ++ i)){ struct _tuple4 _temp188; void* _temp189;
struct Cyc_Absyn_Tqual _temp191; struct _tuple4* _temp186=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp159))->hd; _temp188=* _temp186; _LL192:
_temp191= _temp188.f1; goto _LL190; _LL190: _temp189= _temp188.f2; goto _LL187;
_LL187: _temp184=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key,
void* data)) Cyc_Dict_insert)( _temp184, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp189));}} return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp193=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp193[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp194; _temp194.tag= Cyc_CfFlowInfo_TuplePI;
_temp194.f1= _temp184; _temp194;}); _temp193;});} _LL144: _temp172= _temp161;
goto _LL146; _LL146: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
for( 0; _temp172 != 0; _temp172=(( struct Cyc_List_List*) _check_null( _temp172))->tl){
struct Cyc_Absyn_Structfield _temp197; struct Cyc_List_List* _temp198; struct
Cyc_Core_Opt* _temp200; void* _temp202; struct Cyc_Absyn_Tqual _temp204; struct
_tagged_arr* _temp206; struct Cyc_Absyn_Structfield* _temp195=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp172))->hd; _temp197=* _temp195; _LL207:
_temp206= _temp197.name; goto _LL205; _LL205: _temp204= _temp197.tq; goto _LL203;
_LL203: _temp202=( void*) _temp197.type; goto _LL201; _LL201: _temp200= _temp197.width;
goto _LL199; _LL199: _temp198= _temp197.attributes; goto _LL196; _LL196: if( Cyc_String_strcmp(*
_temp206, _tag_arr("", sizeof( unsigned char), 1u)) != 0){ d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(
d, _temp206, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp202));}} return( void*)({
struct Cyc_CfFlowInfo_StructPI_struct* _temp208=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp208[ 0]=({
struct Cyc_CfFlowInfo_StructPI_struct _temp209; _temp209.tag= Cyc_CfFlowInfo_StructPI;
_temp209.f1= d; _temp209;}); _temp208;});} _LL148: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL138:;} void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place){ struct Cyc_CfFlowInfo_Place _temp212; struct Cyc_List_List* _temp213;
void* _temp215; struct Cyc_CfFlowInfo_Place* _temp210= place; _temp212=*
_temp210; _LL216: _temp215=( void*) _temp212.root; goto _LL214; _LL214: _temp213=
_temp212.fields; goto _LL211; _LL211: { void* _temp217=(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( d, _temp215); goto _LL218; _LL218: for( 0;
_temp213 != 0; _temp213=(( struct Cyc_List_List*) _check_null( _temp213))->tl){
struct _tuple3 _temp220=({ struct _tuple3 _temp219; _temp219.f1= _temp217;
_temp219.f2=( void*)(( struct Cyc_List_List*) _check_null( _temp213))->hd;
_temp219;}); void* _temp228; int _temp230; void* _temp232; struct Cyc_Dict_Dict*
_temp234; void* _temp236; struct _tagged_arr* _temp238; void* _temp240; struct
Cyc_Dict_Dict* _temp242; _LL222: _LL233: _temp232= _temp220.f1; if(*(( int*)
_temp232) == Cyc_CfFlowInfo_TuplePI){ _LL235: _temp234=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp232)->f1; goto _LL229;} else{ goto _LL224;} _LL229: _temp228= _temp220.f2;
if(*(( int*) _temp228) == Cyc_CfFlowInfo_TupleF){ _LL231: _temp230=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp228)->f1; goto _LL223;} else{ goto _LL224;} _LL224: _LL241: _temp240=
_temp220.f1; if(*(( int*) _temp240) == Cyc_CfFlowInfo_StructPI){ _LL243:
_temp242=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp240)->f1; goto _LL237;}
else{ goto _LL226;} _LL237: _temp236= _temp220.f2; if(*(( int*) _temp236) == Cyc_CfFlowInfo_StructF){
_LL239: _temp238=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp236)->f1; goto
_LL225;} else{ goto _LL226;} _LL226: goto _LL227; _LL223: _temp217=(( void*(*)(
struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp234, _temp230); goto
_LL221; _LL225: _temp217=(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp242, _temp238); goto _LL221; _LL227:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp244=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp244[ 0]=({ struct
Cyc_Core_Impossible_struct _temp245; _temp245.tag= Cyc_Core_Impossible; _temp245.f1=
_tag_arr("bad lookup_place", sizeof( unsigned char), 17u); _temp245;}); _temp244;}));
_LL221:;} return _temp217;}} struct _tuple5{ void* f1; struct Cyc_List_List* f2;
} ; void* Cyc_CfFlowInfo_insert_place_rec( void* insert_path, struct Cyc_List_List*
fields, void* pinfo){ if( fields == 0){ return pinfo;}{ struct _tuple5 _temp247=({
struct _tuple5 _temp246; _temp246.f1= insert_path; _temp246.f2= fields; _temp246;});
struct Cyc_List_List* _temp255; struct Cyc_List_List _temp257; struct Cyc_List_List*
_temp258; void* _temp260; int _temp262; void* _temp264; struct Cyc_Dict_Dict*
_temp266; struct Cyc_List_List* _temp268; struct Cyc_List_List _temp270; struct
Cyc_List_List* _temp271; void* _temp273; struct _tagged_arr* _temp275; void*
_temp277; struct Cyc_Dict_Dict* _temp279; _LL249: _LL265: _temp264= _temp247.f1;
if(*(( int*) _temp264) == Cyc_CfFlowInfo_TuplePI){ _LL267: _temp266=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp264)->f1; goto _LL256;} else{ goto _LL251;}
_LL256: _temp255= _temp247.f2; if( _temp255 == 0){ goto _LL251;} else{ _temp257=*
_temp255; _LL261: _temp260=( void*) _temp257.hd; if(*(( int*) _temp260) == Cyc_CfFlowInfo_TupleF){
_LL263: _temp262=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp260)->f1; goto
_LL259;} else{ goto _LL251;} _LL259: _temp258= _temp257.tl; goto _LL250;} _LL251:
_LL278: _temp277= _temp247.f1; if(*(( int*) _temp277) == Cyc_CfFlowInfo_StructPI){
_LL280: _temp279=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp277)->f1; goto
_LL269;} else{ goto _LL253;} _LL269: _temp268= _temp247.f2; if( _temp268 == 0){
goto _LL253;} else{ _temp270=* _temp268; _LL274: _temp273=( void*) _temp270.hd;
if(*(( int*) _temp273) == Cyc_CfFlowInfo_StructF){ _LL276: _temp275=(( struct
Cyc_CfFlowInfo_StructF_struct*) _temp273)->f1; goto _LL272;} else{ goto _LL253;}
_LL272: _temp271= _temp270.tl; goto _LL252;} _LL253: goto _LL254; _LL250: { void*
_temp281=(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp266, _temp262); goto _LL282; _LL282: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp283=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp283[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp284; _temp284.tag= Cyc_CfFlowInfo_TuplePI;
_temp284.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp266, _temp262, Cyc_CfFlowInfo_insert_place_rec(
_temp281, _temp258, pinfo)); _temp284;}); _temp283;});} _LL252: { void* _temp285=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp279, _temp275); goto _LL286; _LL286: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp287=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp287[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp288; _temp288.tag=
Cyc_CfFlowInfo_StructPI; _temp288.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( _temp279, _temp275,
Cyc_CfFlowInfo_insert_place_rec( _temp285, _temp271, pinfo)); _temp288;});
_temp287;});} _LL254:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp289=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp289[ 0]=({ struct Cyc_Core_Impossible_struct _temp290; _temp290.tag= Cyc_Core_Impossible;
_temp290.f1= _tag_arr("bad insert_place", sizeof( unsigned char), 17u); _temp290;});
_temp289;})); _LL248:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){
struct Cyc_CfFlowInfo_Place _temp293; struct Cyc_List_List* _temp294; void*
_temp296; struct Cyc_CfFlowInfo_Place* _temp291= place; _temp293=* _temp291;
_LL297: _temp296=( void*) _temp293.root; goto _LL295; _LL295: _temp294= _temp293.fields;
goto _LL292; _LL292: return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( d, _temp296, Cyc_CfFlowInfo_insert_place_rec(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp296),
_temp294, pinfo));} struct _tuple6{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List*
f2; } ; int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path);
int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env, void* state){ void*
_temp298= state; void* _temp306; void* _temp308; void* _temp310; void* _temp312;
struct Cyc_CfFlowInfo_Place* _temp314; _LL300: if(*(( int*) _temp298) == Cyc_CfFlowInfo_UnknownIS){
_LL309: _temp308=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp298)->f1;
goto _LL307; _LL307: _temp306=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp298)->f2; if( _temp306 ==( void*) Cyc_CfFlowInfo_AllIL){ goto _LL301;}
else{ goto _LL302;}} else{ goto _LL302;} _LL302: if(*(( int*) _temp298) == Cyc_CfFlowInfo_UnknownIS){
_LL313: _temp312=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp298)->f1;
goto _LL311; _LL311: _temp310=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp298)->f2; goto _LL303;} else{ goto _LL304;} _LL304: if(*(( int*) _temp298)
== Cyc_CfFlowInfo_MustPointTo){ _LL315: _temp314=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp298)->f1; goto _LL305;} else{ goto _LL299;} _LL301: return 1; _LL303:
return 0; _LL305: { struct _tuple6 _temp318; struct Cyc_List_List* _temp319;
struct Cyc_List_List** _temp321; struct Cyc_Dict_Dict* _temp322; struct _tuple6*
_temp316= env; _temp318=* _temp316; _LL323: _temp322= _temp318.f1; goto _LL320;
_LL320: _temp319= _temp318.f2; _temp321=&(* _temp316).f2; goto _LL317; _LL317:
if((( int(*)( int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp321, _temp314)){ return 1;}* _temp321=({ struct Cyc_List_List* _temp324=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp324[
0]=({ struct Cyc_List_List _temp325; _temp325.hd=( void*) _temp314; _temp325.tl=*
_temp321; _temp325;}); _temp324;}); return Cyc_CfFlowInfo_isAllInit_pinfo( env,
Cyc_CfFlowInfo_lookup_place( _temp322, _temp314));} _LL299:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp326= path; void* _temp334; struct Cyc_Dict_Dict* _temp336; struct Cyc_Dict_Dict*
_temp338; _LL328: if(*(( int*) _temp326) == Cyc_CfFlowInfo_LeafPI){ _LL335:
_temp334=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp326)->f1; goto
_LL329;} else{ goto _LL330;} _LL330: if(*(( int*) _temp326) == Cyc_CfFlowInfo_TuplePI){
_LL337: _temp336=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp326)->f1; goto
_LL331;} else{ goto _LL332;} _LL332: if(*(( int*) _temp326) == Cyc_CfFlowInfo_StructPI){
_LL339: _temp338=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp326)->f1; goto
_LL333;} else{ goto _LL327;} _LL329: return Cyc_CfFlowInfo_isAllInit_is( env,
_temp334); _LL331: return(( int(*)( int(* f)( struct _tuple6*, int, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f, env,
_temp336); _LL333: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_arr*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_arr* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp338); _LL327:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 _temp341=({ struct _tuple6 _temp340;
_temp340.f1= pinfo_dict; _temp340.f2= 0; _temp340;}); goto _LL342; _LL342:
return Cyc_CfFlowInfo_isAllInit_pinfo(& _temp341, pinfo);} struct _tuple7{
struct Cyc_Dict_Dict* f1; struct Cyc_Set_Set** f2; } ; void* Cyc_CfFlowInfo_pinfo_getplaces_esc(
struct _tuple7* env, void* pinfo){ struct _tuple7 _temp345; struct Cyc_Set_Set**
_temp346; struct Cyc_Dict_Dict* _temp348; struct _tuple7* _temp343= env;
_temp345=* _temp343; _LL349: _temp348= _temp345.f1; goto _LL347; _LL347:
_temp346= _temp345.f2; goto _LL344; _LL344: { void* _temp350= pinfo; void*
_temp360; struct Cyc_CfFlowInfo_Place* _temp362; void* _temp364; void* _temp366;
void* _temp368; struct Cyc_Dict_Dict* _temp370; struct Cyc_Dict_Dict* _temp372;
_LL352: if(*(( int*) _temp350) == Cyc_CfFlowInfo_LeafPI){ _LL361: _temp360=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp350)->f1; if(*(( int*)
_temp360) == Cyc_CfFlowInfo_MustPointTo){ _LL363: _temp362=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp360)->f1; goto _LL353;} else{ goto _LL354;}} else{ goto _LL354;} _LL354:
if(*(( int*) _temp350) == Cyc_CfFlowInfo_LeafPI){ _LL365: _temp364=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp350)->f1; if(*(( int*) _temp364) ==
Cyc_CfFlowInfo_UnknownIS){ _LL369: _temp368=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp364)->f1; goto _LL367; _LL367: _temp366=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp364)->f2; goto _LL355;} else{ goto _LL356;}} else{ goto _LL356;} _LL356:
if(*(( int*) _temp350) == Cyc_CfFlowInfo_TuplePI){ _LL371: _temp370=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp350)->f1; goto _LL357;} else{ goto _LL358;}
_LL358: if(*(( int*) _temp350) == Cyc_CfFlowInfo_StructPI){ _LL373: _temp372=((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp350)->f1; goto _LL359;} else{ goto
_LL351;} _LL353:* _temp346=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* _temp346, _temp362); if(
Cyc_CfFlowInfo_isAllInit( _temp348, Cyc_CfFlowInfo_lookup_place( _temp348,
_temp362))){ return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL355: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, _temp366); _LL357: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp374=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp374[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp375; _temp375.tag= Cyc_CfFlowInfo_TuplePI;
_temp375.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp370); _temp375;}); _temp374;}); _LL359: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp376=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp376[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp377; _temp377.tag=
Cyc_CfFlowInfo_StructPI; _temp377.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp372); _temp377;}); _temp376;});
_LL351:;}} void Cyc_CfFlowInfo_pinfo_getplaces( struct Cyc_Set_Set** places_ptr,
void* a, void* pinfo){ void* _temp378= pinfo; void* _temp388; void* _temp390;
void* _temp392; void* _temp394; struct Cyc_CfFlowInfo_Place* _temp396; struct
Cyc_Dict_Dict* _temp398; struct Cyc_Dict_Dict* _temp400; _LL380: if(*(( int*)
_temp378) == Cyc_CfFlowInfo_LeafPI){ _LL389: _temp388=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp378)->f1; if(*(( int*) _temp388) == Cyc_CfFlowInfo_UnknownIS){ _LL393:
_temp392=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp388)->f1; goto
_LL391; _LL391: _temp390=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp388)->f2; goto _LL381;} else{ goto _LL382;}} else{ goto _LL382;} _LL382:
if(*(( int*) _temp378) == Cyc_CfFlowInfo_LeafPI){ _LL395: _temp394=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp378)->f1; if(*(( int*) _temp394) ==
Cyc_CfFlowInfo_MustPointTo){ _LL397: _temp396=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp394)->f1; goto _LL383;} else{ goto _LL384;}} else{ goto _LL384;} _LL384:
if(*(( int*) _temp378) == Cyc_CfFlowInfo_TuplePI){ _LL399: _temp398=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp378)->f1; goto _LL385;} else{ goto _LL386;}
_LL386: if(*(( int*) _temp378) == Cyc_CfFlowInfo_StructPI){ _LL401: _temp400=((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp378)->f1; goto _LL387;} else{ goto
_LL379;} _LL381: return; _LL383:* places_ptr=(( struct Cyc_Set_Set*(*)( struct
Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* places_ptr,
_temp396); return; _LL385:(( void(*)( void(* f)( struct Cyc_Set_Set**, int, void*),
struct Cyc_Set_Set** env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
places_ptr, _temp398); return; _LL387:(( void(*)( void(* f)( struct Cyc_Set_Set**,
struct _tagged_arr*, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)(( void(*)( struct Cyc_Set_Set** places_ptr, struct _tagged_arr*
a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, places_ptr, _temp400); return;
_LL379:;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these( struct Cyc_Set_Set*
places, struct Cyc_Dict_Dict* d){ while( !(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_is_empty)(
places)) { struct Cyc_CfFlowInfo_Place* _temp402=(( struct Cyc_CfFlowInfo_Place*(*)(
struct Cyc_Set_Set* s)) Cyc_Set_choose)( places); goto _LL403; _LL403: places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_delete)( places, _temp402);{ void* place_old_pinfo;{ struct
_handler_cons _temp404; _push_handler(& _temp404);{ int _temp406= 0; if( setjmp(
_temp404.handler)){ _temp406= 1;} if( ! _temp406){ place_old_pinfo= Cyc_CfFlowInfo_lookup_place(
d, _temp402);; _pop_handler();} else{ void* _temp405=( void*) _exn_thrown; void*
_temp408= _temp405; _LL410: if( _temp408 == Cyc_Dict_Absent){ goto _LL411;}
else{ goto _LL412;} _LL412: goto _LL413; _LL411: continue; _LL413:( void) _throw(
_temp408); _LL409:;}}}{ struct _tuple7 _temp415=({ struct _tuple7 _temp414;
_temp414.f1= d; _temp414.f2=& places; _temp414;}); goto _LL416; _LL416: { void*
_temp417= Cyc_CfFlowInfo_pinfo_getplaces_esc(& _temp415, place_old_pinfo); goto
_LL418; _LL418: if( place_old_pinfo != _temp417){ d= Cyc_CfFlowInfo_insert_place(
d, _temp402, _temp417);}}}}} return d;} static struct Cyc_Set_Set** Cyc_CfFlowInfo_empty_place_set_ptr=
0; struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo, struct Cyc_Dict_Dict*
d){ if( Cyc_CfFlowInfo_empty_place_set_ptr == 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({
struct Cyc_Set_Set** _temp419=( struct Cyc_Set_Set**) GC_malloc( sizeof( struct
Cyc_Set_Set*) * 1); _temp419[ 0]=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp419;});}{ struct Cyc_Set_Set* _temp420=*(( struct Cyc_Set_Set**)
_check_null( Cyc_CfFlowInfo_empty_place_set_ptr)); goto _LL421; _LL421:(( void(*)(
struct Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(&
_temp420, 0, pinfo); return Cyc_CfFlowInfo_escape_these( _temp420, d);}} void*
Cyc_CfFlowInfo_assign_unknown_dict( void* level, void* pinfo){ void* _temp422=
pinfo; void* _temp432; void* _temp434; void* _temp436; void* _temp438; struct
Cyc_Dict_Dict* _temp440; struct Cyc_Dict_Dict* _temp442; _LL424: if(*(( int*)
_temp422) == Cyc_CfFlowInfo_LeafPI){ _LL433: _temp432=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp422)->f1; if(*(( int*) _temp432) == Cyc_CfFlowInfo_UnknownIS){ _LL437:
_temp436=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp432)->f1; if(
_temp436 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL435;} else{ goto _LL426;}
_LL435: _temp434=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp432)->f2;
goto _LL425;} else{ goto _LL426;}} else{ goto _LL426;} _LL426: if(*(( int*)
_temp422) == Cyc_CfFlowInfo_LeafPI){ _LL439: _temp438=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp422)->f1; goto _LL427;} else{ goto _LL428;} _LL428: if(*(( int*) _temp422)
== Cyc_CfFlowInfo_TuplePI){ _LL441: _temp440=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp422)->f1; goto _LL429;} else{ goto _LL430;} _LL430: if(*(( int*) _temp422)
== Cyc_CfFlowInfo_StructPI){ _LL443: _temp442=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp422)->f1; goto _LL431;} else{ goto _LL423;} _LL425: return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc, level); _LL427: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Unesc, level); _LL429: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp444=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp444[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp445; _temp445.tag= Cyc_CfFlowInfo_TuplePI;
_temp445.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp440); _temp445;}); _temp444;}); _LL431: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp446=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp446[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp447; _temp447.tag=
Cyc_CfFlowInfo_StructPI; _temp447.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp442); _temp447;}); _temp446;}); _LL423:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_Set_Set** places;
} ; void* Cyc_CfFlowInfo_join_escape( void* esc1, void* esc2){ struct _tuple3
_temp449=({ struct _tuple3 _temp448; _temp448.f1= esc1; _temp448.f2= esc2;
_temp448;}); void* _temp457; void* _temp459; void* _temp461; void* _temp463;
_LL451: _LL460: _temp459= _temp449.f1; if( _temp459 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL458;} else{ goto _LL453;} _LL458: _temp457= _temp449.f2; goto _LL452;
_LL453: _LL464: _temp463= _temp449.f1; goto _LL462; _LL462: _temp461= _temp449.f2;
if( _temp461 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL454;} else{ goto _LL455;}
_LL455: goto _LL456; _LL452: goto _LL454; _LL454: return( void*) Cyc_CfFlowInfo_Esc;
_LL456: return( void*) Cyc_CfFlowInfo_Unesc; _LL450:;} void* Cyc_CfFlowInfo_join_initlevel(
void* il1, void* il2){ struct _tuple3 _temp466=({ struct _tuple3 _temp465;
_temp465.f1= il1; _temp465.f2= il2; _temp465;}); void* _temp478; void* _temp480;
void* _temp482; void* _temp484; void* _temp486; void* _temp488; void* _temp490;
void* _temp492; _LL468: _LL481: _temp480= _temp466.f1; if( _temp480 ==( void*)
Cyc_CfFlowInfo_NoneIL){ goto _LL479;} else{ goto _LL470;} _LL479: _temp478=
_temp466.f2; goto _LL469; _LL470: _LL485: _temp484= _temp466.f1; goto _LL483;
_LL483: _temp482= _temp466.f2; if( _temp482 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL471;} else{ goto _LL472;} _LL472: _LL489: _temp488= _temp466.f1; if(
_temp488 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL487;} else{ goto _LL474;}
_LL487: _temp486= _temp466.f2; goto _LL473; _LL474: _LL493: _temp492= _temp466.f1;
goto _LL491; _LL491: _temp490= _temp466.f2; if( _temp490 ==( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL475;} else{ goto _LL476;} _LL476: goto _LL477; _LL469: goto _LL471;
_LL471: return( void*) Cyc_CfFlowInfo_NoneIL; _LL473: goto _LL475; _LL475:
return( void*) Cyc_CfFlowInfo_ThisIL; _LL477: return( void*) Cyc_CfFlowInfo_AllIL;
_LL467:;} void* Cyc_CfFlowInfo_join_initstate( struct Cyc_CfFlowInfo_JoinEnv*
env, void* state1, void* state2){ struct _tuple3 _temp495=({ struct _tuple3
_temp494; _temp494.f1= state1; _temp494.f2= state2; _temp494;}); void* _temp505;
void* _temp507; void* _temp509; void* _temp511; void* _temp513; void* _temp515;
void* _temp517; struct Cyc_CfFlowInfo_Place* _temp519; void* _temp521; void*
_temp523; void* _temp525; void* _temp527; void* _temp529; void* _temp531; void*
_temp533; struct Cyc_CfFlowInfo_Place* _temp535; void* _temp537; struct Cyc_CfFlowInfo_Place*
_temp539; void* _temp541; struct Cyc_CfFlowInfo_Place* _temp543; _LL497: _LL512:
_temp511= _temp495.f1; if(*(( int*) _temp511) == Cyc_CfFlowInfo_UnknownIS){
_LL516: _temp515=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp511)->f1;
goto _LL514; _LL514: _temp513=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp511)->f2; goto _LL506;} else{ goto _LL499;} _LL506: _temp505= _temp495.f2;
if(*(( int*) _temp505) == Cyc_CfFlowInfo_UnknownIS){ _LL510: _temp509=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp505)->f1; goto _LL508; _LL508:
_temp507=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp505)->f2; goto
_LL498;} else{ goto _LL499;} _LL499: _LL522: _temp521= _temp495.f1; if(*(( int*)
_temp521) == Cyc_CfFlowInfo_UnknownIS){ _LL526: _temp525=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp521)->f1; goto _LL524; _LL524: _temp523=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp521)->f2; goto _LL518;} else{ goto _LL501;} _LL518: _temp517= _temp495.f2;
if(*(( int*) _temp517) == Cyc_CfFlowInfo_MustPointTo){ _LL520: _temp519=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp517)->f1; goto _LL500;} else{
goto _LL501;} _LL501: _LL534: _temp533= _temp495.f1; if(*(( int*) _temp533) ==
Cyc_CfFlowInfo_MustPointTo){ _LL536: _temp535=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp533)->f1; goto _LL528;} else{ goto _LL503;} _LL528: _temp527= _temp495.f2;
if(*(( int*) _temp527) == Cyc_CfFlowInfo_UnknownIS){ _LL532: _temp531=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp527)->f1; goto _LL530; _LL530:
_temp529=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp527)->f2; goto
_LL502;} else{ goto _LL503;} _LL503: _LL542: _temp541= _temp495.f1; if(*(( int*)
_temp541) == Cyc_CfFlowInfo_MustPointTo){ _LL544: _temp543=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp541)->f1; goto _LL538;} else{ goto _LL496;} _LL538: _temp537= _temp495.f2;
if(*(( int*) _temp537) == Cyc_CfFlowInfo_MustPointTo){ _LL540: _temp539=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp537)->f1; goto _LL504;} else{
goto _LL496;} _LL498: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_join_escape(
_temp515, _temp509), Cyc_CfFlowInfo_join_initlevel( _temp513, _temp507)); _LL500: {
struct _tuple6 _temp546=({ struct _tuple6 _temp545; _temp545.f1= env->d2;
_temp545.f2= 0; _temp545;}); goto _LL547; _LL547: if( _temp523 ==( void*) Cyc_CfFlowInfo_AllIL?
Cyc_CfFlowInfo_isAllInit_pinfo(& _temp546, Cyc_CfFlowInfo_lookup_place( env->d2,
_temp519)): 0){* env->places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places, _temp519);
return Cyc_CfFlowInfo_mkLeafPI( _temp525, _temp523);} return Cyc_CfFlowInfo_mkLeafPI(
_temp525, Cyc_CfFlowInfo_join_initlevel( _temp523,( void*) Cyc_CfFlowInfo_ThisIL));}
_LL502: { struct _tuple6 _temp549=({ struct _tuple6 _temp548; _temp548.f1= env->d1;
_temp548.f2= 0; _temp548;}); goto _LL550; _LL550: if( _temp529 ==( void*) Cyc_CfFlowInfo_AllIL?
Cyc_CfFlowInfo_isAllInit_pinfo(& _temp549, Cyc_CfFlowInfo_lookup_place( env->d1,
_temp535)): 0){* env->places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places, _temp535);
return Cyc_CfFlowInfo_mkLeafPI( _temp531, _temp529);} return Cyc_CfFlowInfo_mkLeafPI(
_temp531, Cyc_CfFlowInfo_join_initlevel(( void*) Cyc_CfFlowInfo_ThisIL, _temp529));}
_LL504: if( Cyc_CfFlowInfo_place_cmp( _temp543, _temp539) == 0){ return( void*)({
struct Cyc_CfFlowInfo_LeafPI_struct* _temp551=( struct Cyc_CfFlowInfo_LeafPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct)); _temp551[ 0]=({ struct
Cyc_CfFlowInfo_LeafPI_struct _temp552; _temp552.tag= Cyc_CfFlowInfo_LeafPI;
_temp552.f1=( void*) state1; _temp552;}); _temp551;});}{ struct _tuple6 _temp554=({
struct _tuple6 _temp553; _temp553.f1= env->d1; _temp553.f2= 0; _temp553;}); goto
_LL555; _LL555: { struct _tuple6 _temp557=({ struct _tuple6 _temp556; _temp556.f1=
env->d2; _temp556.f2= 0; _temp556;}); goto _LL558; _LL558: if( Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp554, Cyc_CfFlowInfo_lookup_place( env->d1, _temp543))? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp557, Cyc_CfFlowInfo_lookup_place( env->d2, _temp539)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp543);* env->places=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places,
_temp539); return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,(
void*) Cyc_CfFlowInfo_ThisIL);}} _LL496:;} void* Cyc_CfFlowInfo_join_pathinfo(
struct Cyc_CfFlowInfo_JoinEnv* env, void* pinfo1, void* pinfo2){ if( pinfo1 ==
pinfo2){ return pinfo1;}{ struct _tuple3 _temp560=({ struct _tuple3 _temp559;
_temp559.f1= pinfo1; _temp559.f2= pinfo2; _temp559;}); void* _temp570; void*
_temp572; void* _temp574; void* _temp576; void* _temp578; struct Cyc_Dict_Dict*
_temp580; void* _temp582; struct Cyc_Dict_Dict* _temp584; void* _temp586; struct
Cyc_Dict_Dict* _temp588; void* _temp590; struct Cyc_Dict_Dict* _temp592; _LL562:
_LL575: _temp574= _temp560.f1; if(*(( int*) _temp574) == Cyc_CfFlowInfo_LeafPI){
_LL577: _temp576=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp574)->f1;
goto _LL571;} else{ goto _LL564;} _LL571: _temp570= _temp560.f2; if(*(( int*)
_temp570) == Cyc_CfFlowInfo_LeafPI){ _LL573: _temp572=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp570)->f1; goto _LL563;} else{ goto _LL564;} _LL564: _LL583: _temp582=
_temp560.f1; if(*(( int*) _temp582) == Cyc_CfFlowInfo_TuplePI){ _LL585: _temp584=((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp582)->f1; goto _LL579;} else{ goto
_LL566;} _LL579: _temp578= _temp560.f2; if(*(( int*) _temp578) == Cyc_CfFlowInfo_TuplePI){
_LL581: _temp580=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp578)->f1; goto
_LL565;} else{ goto _LL566;} _LL566: _LL591: _temp590= _temp560.f1; if(*(( int*)
_temp590) == Cyc_CfFlowInfo_StructPI){ _LL593: _temp592=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp590)->f1; goto _LL587;} else{ goto _LL568;} _LL587: _temp586= _temp560.f2;
if(*(( int*) _temp586) == Cyc_CfFlowInfo_StructPI){ _LL589: _temp588=(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp586)->f1; goto _LL567;} else{ goto _LL568;}
_LL568: goto _LL569; _LL563: if( _temp576 == _temp572){ return pinfo1;} return
Cyc_CfFlowInfo_join_initstate( env, _temp576, _temp572); _LL565: if( _temp584 ==
_temp580){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp594=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp594[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp595; _temp595.tag= Cyc_CfFlowInfo_TuplePI;
_temp595.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp584, _temp580); _temp595;}); _temp594;}); _LL567: if( _temp592 ==
_temp588){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp596=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp596[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp597; _temp597.tag=
Cyc_CfFlowInfo_StructPI; _temp597.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp592, _temp588); _temp597;}); _temp596;});
_LL569:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp598=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp598[ 0]=({ struct Cyc_Core_Impossible_struct _temp599; _temp599.tag= Cyc_Core_Impossible;
_temp599.f1= _tag_arr("join_pathinfo", sizeof( unsigned char), 14u); _temp599;});
_temp598;})); _LL561:;}} void* Cyc_CfFlowInfo_join_flow( void* f1, void* f2){
if( Cyc_CfFlowInfo_empty_place_set_ptr == 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({
struct Cyc_Set_Set** _temp600=( struct Cyc_Set_Set**) GC_malloc( sizeof( struct
Cyc_Set_Set*) * 1); _temp600[ 0]=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp600;});} if( f1 == f2){ return f1;}{ struct _tuple3 _temp602=({ struct
_tuple3 _temp601; _temp601.f1= f1; _temp601.f2= f2; _temp601;}); void* _temp610;
void* _temp612; void* _temp614; void* _temp616; void* _temp618; struct Cyc_Dict_Dict*
_temp620; void* _temp622; struct Cyc_Dict_Dict* _temp624; _LL604: _LL613:
_temp612= _temp602.f1; if( _temp612 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL611;} else{ goto _LL606;} _LL611: _temp610= _temp602.f2; goto _LL605; _LL606:
_LL617: _temp616= _temp602.f1; goto _LL615; _LL615: _temp614= _temp602.f2; if(
_temp614 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL607;} else{ goto _LL608;}
_LL608: _LL623: _temp622= _temp602.f1; if(( unsigned int) _temp622 > 1u?*(( int*)
_temp622) == Cyc_CfFlowInfo_InitsFL: 0){ _LL625: _temp624=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp622)->f1; goto _LL619;} else{ goto _LL603;} _LL619: _temp618= _temp602.f2;
if(( unsigned int) _temp618 > 1u?*(( int*) _temp618) == Cyc_CfFlowInfo_InitsFL:
0){ _LL621: _temp620=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp618)->f1;
goto _LL609;} else{ goto _LL603;} _LL605: return f2; _LL607: return f1; _LL609:
if( _temp624 == _temp620){ return f1;}{ struct Cyc_Set_Set* _temp626=*(( struct
Cyc_Set_Set**) _check_null( Cyc_CfFlowInfo_empty_place_set_ptr)); goto _LL627;
_LL627: { struct Cyc_CfFlowInfo_JoinEnv _temp629=({ struct Cyc_CfFlowInfo_JoinEnv
_temp628; _temp628.d1= _temp624; _temp628.d2= _temp620; _temp628.places=&
_temp626; _temp628;}); goto _LL630; _LL630: { struct Cyc_Dict_Dict* _temp631=((
struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*, void*,
void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,& _temp629, _temp624,
_temp620); goto _LL632; _LL632: return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp633=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp633[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp634; _temp634.tag= Cyc_CfFlowInfo_InitsFL;
_temp634.f1= Cyc_CfFlowInfo_escape_these( _temp626, _temp631); _temp634;});
_temp633;});}}} _LL603:;}} int Cyc_CfFlowInfo_initstate_lessthan_approx( void*
state1, void* state2){ struct _tuple3 _temp636=({ struct _tuple3 _temp635;
_temp635.f1= state1; _temp635.f2= state2; _temp635;}); void* _temp644; void*
_temp646; void* _temp648; void* _temp650; void* _temp652; void* _temp654; void*
_temp656; struct Cyc_CfFlowInfo_Place* _temp658; void* _temp660; struct Cyc_CfFlowInfo_Place*
_temp662; _LL638: _LL651: _temp650= _temp636.f1; if(*(( int*) _temp650) == Cyc_CfFlowInfo_UnknownIS){
_LL655: _temp654=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp650)->f1;
goto _LL653; _LL653: _temp652=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp650)->f2; goto _LL645;} else{ goto _LL640;} _LL645: _temp644= _temp636.f2;
if(*(( int*) _temp644) == Cyc_CfFlowInfo_UnknownIS){ _LL649: _temp648=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp644)->f1; goto _LL647; _LL647:
_temp646=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp644)->f2; goto
_LL639;} else{ goto _LL640;} _LL640: _LL661: _temp660= _temp636.f1; if(*(( int*)
_temp660) == Cyc_CfFlowInfo_MustPointTo){ _LL663: _temp662=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp660)->f1; goto _LL657;} else{ goto _LL642;} _LL657: _temp656= _temp636.f2;
if(*(( int*) _temp656) == Cyc_CfFlowInfo_MustPointTo){ _LL659: _temp658=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp656)->f1; goto _LL641;} else{
goto _LL642;} _LL642: goto _LL643; _LL639: return Cyc_CfFlowInfo_join_escape(
_temp654, _temp648) == _temp648? Cyc_CfFlowInfo_join_initlevel( _temp652,
_temp646) == _temp646: 0; _LL641: return Cyc_CfFlowInfo_place_cmp( _temp662,
_temp658) == 0; _LL643: return 0; _LL637:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp665=({ struct _tuple3 _temp664; _temp664.f1= pinfo1;
_temp664.f2= pinfo2; _temp664;}); void* _temp675; void* _temp677; void* _temp679;
void* _temp681; void* _temp683; struct Cyc_Dict_Dict* _temp685; void* _temp687;
struct Cyc_Dict_Dict* _temp689; void* _temp691; struct Cyc_Dict_Dict* _temp693;
void* _temp695; struct Cyc_Dict_Dict* _temp697; _LL667: _LL680: _temp679=
_temp665.f1; if(*(( int*) _temp679) == Cyc_CfFlowInfo_LeafPI){ _LL682: _temp681=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp679)->f1; goto _LL676;}
else{ goto _LL669;} _LL676: _temp675= _temp665.f2; if(*(( int*) _temp675) == Cyc_CfFlowInfo_LeafPI){
_LL678: _temp677=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp675)->f1;
goto _LL668;} else{ goto _LL669;} _LL669: _LL688: _temp687= _temp665.f1; if(*((
int*) _temp687) == Cyc_CfFlowInfo_TuplePI){ _LL690: _temp689=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp687)->f1; goto _LL684;} else{ goto _LL671;} _LL684: _temp683= _temp665.f2;
if(*(( int*) _temp683) == Cyc_CfFlowInfo_TuplePI){ _LL686: _temp685=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp683)->f1; goto _LL670;} else{ goto _LL671;}
_LL671: _LL696: _temp695= _temp665.f1; if(*(( int*) _temp695) == Cyc_CfFlowInfo_StructPI){
_LL698: _temp697=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp695)->f1; goto
_LL692;} else{ goto _LL673;} _LL692: _temp691= _temp665.f2; if(*(( int*)
_temp691) == Cyc_CfFlowInfo_StructPI){ _LL694: _temp693=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp691)->f1; goto _LL672;} else{ goto _LL673;} _LL673: goto _LL674; _LL668:
return _temp681 == _temp677? 1: Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp681, _temp677); _LL670: return _temp689 == _temp685? 1:(( int(*)( int(* f)(
int, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp689, _temp685); _LL672: return _temp697 == _temp693? 1:(( int(*)( int(* f)(
struct _tagged_arr*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_arr* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp697, _temp693);
_LL674:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp699=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp699[ 0]=({ struct Cyc_Core_Impossible_struct _temp700; _temp700.tag= Cyc_Core_Impossible;
_temp700.f1= _tag_arr("pinfo_lessthan", sizeof( unsigned char), 15u); _temp700;});
_temp699;})); _LL666:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2){ if( f1 == f2){ return 1;}{ struct _tuple3 _temp702=({ struct _tuple3
_temp701; _temp701.f1= f1; _temp701.f2= f2; _temp701;}); void* _temp710; void*
_temp712; void* _temp714; void* _temp716; void* _temp718; struct Cyc_Dict_Dict*
_temp720; void* _temp722; struct Cyc_Dict_Dict* _temp724; _LL704: _LL713:
_temp712= _temp702.f1; if( _temp712 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL711;} else{ goto _LL706;} _LL711: _temp710= _temp702.f2; goto _LL705; _LL706:
_LL717: _temp716= _temp702.f1; goto _LL715; _LL715: _temp714= _temp702.f2; if(
_temp714 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL707;} else{ goto _LL708;}
_LL708: _LL723: _temp722= _temp702.f1; if(( unsigned int) _temp722 > 1u?*(( int*)
_temp722) == Cyc_CfFlowInfo_InitsFL: 0){ _LL725: _temp724=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp722)->f1; goto _LL719;} else{ goto _LL703;} _LL719: _temp718= _temp702.f2;
if(( unsigned int) _temp718 > 1u?*(( int*) _temp718) == Cyc_CfFlowInfo_InitsFL:
0){ _LL721: _temp720=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp718)->f1;
goto _LL709;} else{ goto _LL703;} _LL705: return 1; _LL707: return 0; _LL709:
if( _temp724 == _temp720){ return 1;} return(( int(*)( int(* f)( void*, void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( void* ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp724, _temp720); _LL703:;}}