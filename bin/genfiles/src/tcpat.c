 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; struct
_tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd( struct _tuple0*);
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_String_zstrcmp(
struct _tagged_arr, struct _tagged_arr); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static
const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static
const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; }
; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple1{ void* f1;
struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
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
24; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x, void*
t, struct Cyc_Absyn_Exp* init); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct
Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids:
1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*);
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*);
extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle*, struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_List_List*
f1; struct _RegionHandle* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1;
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple4*,
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List*
vs, struct Cyc_Position_Segment* loc, struct _tagged_arr err_msg); extern struct
Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields); struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ;
extern struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
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
_LL36;} else{ goto _LL28;} _LL30: Cyc_Tcutil_terr( p->loc, _tag_arr("struct tag used without arguments in pattern",
sizeof( unsigned char), 45u)); _npop_handler( 0u); return; _LL32:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp49=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp49[ 0]=({ struct Cyc_Absyn_Tunion_p_struct
_temp50; _temp50.tag= Cyc_Absyn_Tunion_p; _temp50.f1= _temp41; _temp50.f2=
_temp39; _temp50.f3= 0; _temp50.f4= 0; _temp50;}); _temp49;}))); _npop_handler(
0u); return; _LL34:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp51=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp51[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp52; _temp52.tag= Cyc_Absyn_Enum_p;
_temp52.f1= _temp45; _temp52.f2= _temp43; _temp52;}); _temp51;})));
_npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;}; _pop_handler();} else{
void* _temp25=( void*) _exn_thrown; void* _temp54= _temp25; _LL56: if( _temp54
== Cyc_Dict_Absent){ goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL57:
goto _LL55; _LL59:( void) _throw( _temp54); _LL55:;}}}{ void* _temp60=(* _temp10).f1;
struct Cyc_List_List* _temp68; _LL62: if( _temp60 ==( void*) Cyc_Absyn_Loc_n){
goto _LL63;} else{ goto _LL64;} _LL64: if(( unsigned int) _temp60 > 1u?*(( int*)
_temp60) == Cyc_Absyn_Rel_n: 0){ _LL69: _temp68=(( struct Cyc_Absyn_Rel_n_struct*)
_temp60)->f1; if( _temp68 == 0){ goto _LL65;} else{ goto _LL66;}} else{ goto
_LL66;} _LL66: goto _LL67; _LL63: goto _LL65; _LL65:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp70=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp70[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp71; _temp71.tag= Cyc_Absyn_Var_p;
_temp71.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp71;}); _temp70;}))); return; _LL67: Cyc_Tcutil_terr( p->loc, _tag_arr("qualified variable in pattern",
sizeof( unsigned char), 30u)); return; _LL61:;} _LL5:{ struct _handler_cons
_temp72; _push_handler(& _temp72);{ int _temp74= 0; if( setjmp( _temp72.handler)){
_temp74= 1;} if( ! _temp74){{ void* _temp75= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp16); struct Cyc_Absyn_Structdecl* _temp85; struct Cyc_Absyn_Tunionfield*
_temp87; struct Cyc_Absyn_Tuniondecl* _temp89; struct Cyc_Absyn_Enumfield*
_temp91; struct Cyc_Absyn_Enumdecl* _temp93; void* _temp95; _LL77: if(*(( int*)
_temp75) == Cyc_Tcenv_StructRes){ _LL86: _temp85=(( struct Cyc_Tcenv_StructRes_struct*)
_temp75)->f1; goto _LL78;} else{ goto _LL79;} _LL79: if(*(( int*) _temp75) ==
Cyc_Tcenv_TunionRes){ _LL90: _temp89=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp75)->f1; goto _LL88; _LL88: _temp87=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp75)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if(*(( int*) _temp75) ==
Cyc_Tcenv_EnumRes){ _LL94: _temp93=(( struct Cyc_Tcenv_EnumRes_struct*) _temp75)->f1;
goto _LL92; _LL92: _temp91=(( struct Cyc_Tcenv_EnumRes_struct*) _temp75)->f2;
goto _LL82;} else{ goto _LL83;} _LL83: if(*(( int*) _temp75) == Cyc_Tcenv_VarRes){
_LL96: _temp95=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp75)->f1; goto
_LL84;} else{ goto _LL76;} _LL78: { struct Cyc_List_List* _temp97= 0; goto _LL98;
_LL98: for( 0; _temp12 != 0; _temp12=(( struct Cyc_List_List*) _check_null(
_temp12))->tl){ _temp97=({ struct Cyc_List_List* _temp99=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp99->hd=( void*)({ struct _tuple7*
_temp100=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp100->f1= 0;
_temp100->f2=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp12))->hd; _temp100;}); _temp99->tl= _temp97; _temp99;});}( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp101=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp101[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp102; _temp102.tag= Cyc_Absyn_Struct_p; _temp102.f1=
_temp85; _temp102.f2= 0; _temp102.f3= _temp14; _temp102.f4=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp97); _temp102;}); _temp101;})));
_npop_handler( 0u); return;} _LL80:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp103=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp103[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp104; _temp104.tag= Cyc_Absyn_Tunion_p;
_temp104.f1= _temp89; _temp104.f2= _temp87; _temp104.f3= _temp14; _temp104.f4=
_temp12; _temp104;}); _temp103;}))); _npop_handler( 0u); return; _LL82: Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp105= Cyc_Absynpp_qvar2string(
_temp16); xprintf("bad enum constructor %.*s in pattern", _temp105.last_plus_one
- _temp105.curr, _temp105.curr);})); _npop_handler( 0u); return; _LL84: goto
_LL76; _LL76:;}; _pop_handler();} else{ void* _temp73=( void*) _exn_thrown; void*
_temp107= _temp73; _LL109: if( _temp107 == Cyc_Dict_Absent){ goto _LL110;} else{
goto _LL111;} _LL111: goto _LL112; _LL110: goto _LL108; _LL112:( void) _throw(
_temp107); _LL108:;}}} Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp113= Cyc_Absynpp_qvar2string( _temp16); xprintf("%.*s is not a constructor in pattern",
_temp113.last_plus_one - _temp113.curr, _temp113.curr);})); return; _LL7:{
struct _handler_cons _temp114; _push_handler(& _temp114);{ int _temp116= 0; if(
setjmp( _temp114.handler)){ _temp116= 1;} if( ! _temp116){{ void* _temp117= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp22); struct Cyc_Absyn_Structdecl* _temp123; _LL119: if(*(( int*)
_temp117) == Cyc_Tcenv_StructRes){ _LL124: _temp123=(( struct Cyc_Tcenv_StructRes_struct*)
_temp117)->f1; goto _LL120;} else{ goto _LL121;} _LL121: goto _LL122; _LL120:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp125=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp125[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp126; _temp126.tag= Cyc_Absyn_Struct_p;
_temp126.f1= _temp123; _temp126.f2= 0; _temp126.f3= _temp20; _temp126.f4=
_temp18; _temp126;}); _temp125;}))); _npop_handler( 0u); return; _LL122: goto
_LL118; _LL118:;}; _pop_handler();} else{ void* _temp115=( void*) _exn_thrown;
void* _temp128= _temp115; _LL130: if( _temp128 == Cyc_Dict_Absent){ goto _LL131;}
else{ goto _LL132;} _LL132: goto _LL133; _LL131: goto _LL129; _LL133:( void)
_throw( _temp128); _LL129:;}}} Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({
struct _tagged_arr _temp134= Cyc_Absynpp_qvar2string( _temp22); xprintf("%.*s is not a struct name, but pattern has designators",
_temp134.last_plus_one - _temp134.curr, _temp134.curr);})); return; _LL9:
return; _LL1:;} static struct _tagged_arr* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl*
vd){ return(* vd->name).f2;} static struct Cyc_Core_Opt Cyc_Tcpat_mk={( void*)((
void*) 1u)}; static struct Cyc_Core_Opt Cyc_Tcpat_rk={( void*)(( void*) 3u)};
static struct Cyc_Core_Opt Cyc_Tcpat_ak={( void*)(( void*) 0u)}; static void*
Cyc_Tcpat_any_type( struct Cyc_List_List* s, void** topt){ if( topt != 0){
return*(( void**) _check_null( topt));} return Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcpat_mk,({ struct Cyc_Core_Opt* _temp135=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp135->v=( void*) s; _temp135;}));} static
void* Cyc_Tcpat_num_type( void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(*((
void**) _check_null( topt))): 0){ return*(( void**) _check_null( topt));}{ void*
_temp136= Cyc_Tcutil_compress( numt); struct Cyc_Absyn_Enumdecl* _temp142;
struct _tuple1* _temp144; _LL138: if(( unsigned int) _temp136 > 4u?*(( int*)
_temp136) == Cyc_Absyn_EnumType: 0){ _LL145: _temp144=(( struct Cyc_Absyn_EnumType_struct*)
_temp136)->f1; goto _LL143; _LL143: _temp142=(( struct Cyc_Absyn_EnumType_struct*)
_temp136)->f2; goto _LL139;} else{ goto _LL140;} _LL140: goto _LL141; _LL139:
if( topt != 0){ return*(( void**) _check_null( topt));} goto _LL137; _LL141:
goto _LL137; _LL137:;} return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl*
vd, struct Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t);
vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({ struct Cyc_List_List* _temp146=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp146->hd=(
void*) vd; _temp146->tl=* v_result_ptr; _temp146;});} struct _tuple8{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; struct _tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple6 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp147=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp181; struct Cyc_Absyn_Vardecl*
_temp183; int _temp185; void* _temp187; int _temp189; void* _temp191;
unsigned char _temp193; struct _tagged_arr _temp195; struct Cyc_Absyn_Enumfield*
_temp197; struct Cyc_Absyn_Enumdecl* _temp199; struct Cyc_Absyn_Pat* _temp201;
struct Cyc_List_List* _temp203; struct Cyc_List_List* _temp205; struct Cyc_List_List*
_temp207; struct Cyc_Core_Opt* _temp209; struct Cyc_Core_Opt** _temp211; struct
Cyc_Absyn_Structdecl* _temp212; struct Cyc_List_List* _temp214; struct Cyc_List_List*
_temp216; struct Cyc_Absyn_Tunionfield* _temp218; struct Cyc_Absyn_Tuniondecl*
_temp220; struct _tuple1* _temp222; struct Cyc_List_List* _temp224; struct Cyc_List_List*
_temp226; struct _tuple1* _temp228; struct Cyc_List_List* _temp230; struct Cyc_List_List*
_temp232; struct _tuple1* _temp234; _LL149: if( _temp147 ==( void*) Cyc_Absyn_Wild_p){
goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int) _temp147 > 2u?*((
int*) _temp147) == Cyc_Absyn_Var_p: 0){ _LL182: _temp181=(( struct Cyc_Absyn_Var_p_struct*)
_temp147)->f1; goto _LL152;} else{ goto _LL153;} _LL153: if(( unsigned int)
_temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Reference_p: 0){ _LL184: _temp183=((
struct Cyc_Absyn_Reference_p_struct*) _temp147)->f1; goto _LL154;} else{ goto
_LL155;} _LL155: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Int_p:
0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp147)->f1;
if( _temp187 ==( void*) Cyc_Absyn_Unsigned){ goto _LL186;} else{ goto _LL157;}
_LL186: _temp185=(( struct Cyc_Absyn_Int_p_struct*) _temp147)->f2; goto _LL156;}
else{ goto _LL157;} _LL157: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147)
== Cyc_Absyn_Int_p: 0){ _LL192: _temp191=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp147)->f1; if( _temp191 ==( void*) Cyc_Absyn_Signed){ goto _LL190;} else{
goto _LL159;} _LL190: _temp189=(( struct Cyc_Absyn_Int_p_struct*) _temp147)->f2;
goto _LL158;} else{ goto _LL159;} _LL159: if(( unsigned int) _temp147 > 2u?*((
int*) _temp147) == Cyc_Absyn_Char_p: 0){ _LL194: _temp193=(( struct Cyc_Absyn_Char_p_struct*)
_temp147)->f1; goto _LL160;} else{ goto _LL161;} _LL161: if(( unsigned int)
_temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Float_p: 0){ _LL196: _temp195=((
struct Cyc_Absyn_Float_p_struct*) _temp147)->f1; goto _LL162;} else{ goto _LL163;}
_LL163: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Enum_p:
0){ _LL200: _temp199=(( struct Cyc_Absyn_Enum_p_struct*) _temp147)->f1; goto
_LL198; _LL198: _temp197=(( struct Cyc_Absyn_Enum_p_struct*) _temp147)->f2; goto
_LL164;} else{ goto _LL165;} _LL165: if( _temp147 ==( void*) Cyc_Absyn_Null_p){
goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int) _temp147 > 2u?*((
int*) _temp147) == Cyc_Absyn_Pointer_p: 0){ _LL202: _temp201=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp147)->f1; goto _LL168;} else{ goto _LL169;} _LL169: if(( unsigned int)
_temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Tuple_p: 0){ _LL204: _temp203=((
struct Cyc_Absyn_Tuple_p_struct*) _temp147)->f1; goto _LL170;} else{ goto _LL171;}
_LL171: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Struct_p:
0){ _LL213: _temp212=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f1; goto
_LL210; _LL210: _temp209=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f2;
_temp211=&(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f2; goto _LL208;
_LL208: _temp207=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f3; goto
_LL206; _LL206: _temp205=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f4;
goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int) _temp147 > 2u?*((
int*) _temp147) == Cyc_Absyn_Tunion_p: 0){ _LL221: _temp220=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f1; goto _LL219; _LL219: _temp218=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f2; goto _LL217; _LL217: _temp216=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f3; goto _LL215; _LL215: _temp214=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f4; goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int)
_temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_UnknownId_p: 0){ _LL223: _temp222=((
struct Cyc_Absyn_UnknownId_p_struct*) _temp147)->f1; goto _LL176;} else{ goto
_LL177;} _LL177: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_UnknownCall_p:
0){ _LL229: _temp228=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp147)->f1;
goto _LL227; _LL227: _temp226=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp147)->f2; goto _LL225; _LL225: _temp224=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp147)->f3; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_UnknownFields_p: 0){ _LL235:
_temp234=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp147)->f1; goto _LL233;
_LL233: _temp232=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp147)->f2;
goto _LL231; _LL231: _temp230=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp147)->f3; goto _LL180;} else{ goto _LL148;} _LL150: t= Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars( te), topt); goto _LL148; _LL152: t= Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars( te), topt); Cyc_Tcpat_set_vd( _temp181,& v_result, t);
goto _LL148; _LL154: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars( te),
topt); if( region_opt == 0){ Cyc_Tcutil_terr( p->loc, _tag_arr("* pattern would point into an unknown region",
sizeof( unsigned char), 45u)); goto _LL148;} Cyc_Tcpat_set_vd( _temp183,&
v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp236=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp236[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp237; _temp237.tag= Cyc_Absyn_PointerType;
_temp237.f1=({ struct Cyc_Absyn_PtrInfo _temp238; _temp238.elt_typ=( void*) t;
_temp238.rgn_typ=( void*)*(( void**) _check_null( region_opt)); _temp238.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp238.tq= Cyc_Absyn_empty_tqual();
_temp238.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp238;}); _temp237;}); _temp236;})); goto _LL148; _LL156: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_uint_t); goto _LL148; _LL158: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL148; _LL160: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto
_LL148; _LL162: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL148;
_LL164: t= Cyc_Tcpat_num_type( topt,( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp239=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp239[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp240; _temp240.tag= Cyc_Absyn_EnumType;
_temp240.f1= _temp199->name; _temp240.f2=( struct Cyc_Absyn_Enumdecl*) _temp199;
_temp240;}); _temp239;})); goto _LL148; _LL166: if( topt != 0){ void* _temp241=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp247; _LL243: if(( unsigned int) _temp241 > 4u?*(( int*) _temp241) == Cyc_Absyn_PointerType:
0){ _LL248: _temp247=(( struct Cyc_Absyn_PointerType_struct*) _temp241)->f1;
goto _LL244;} else{ goto _LL245;} _LL245: goto _LL246; _LL244: t=*(( void**)
_check_null( topt)); goto tcpat_end; _LL246: goto _LL242; _LL242:;}{ struct Cyc_List_List*
_temp249= Cyc_Tcenv_lookup_type_vars( te); goto _LL250; _LL250: t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp251=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp251[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp252; _temp252.tag= Cyc_Absyn_PointerType;
_temp252.f1=({ struct Cyc_Absyn_PtrInfo _temp253; _temp253.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_ak,({ struct Cyc_Core_Opt* _temp255=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp255->v=( void*)
_temp249; _temp255;})); _temp253.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp254=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp254->v=( void*) _temp249;
_temp254;})); _temp253.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp253.tq= Cyc_Absyn_empty_tqual(); _temp253.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp253;}); _temp252;}); _temp251;}); goto _LL148;}
_LL168: { void* inner_typ=( void*) Cyc_Absyn_VoidType; void** _temp256= 0; goto
_LL257; _LL257: if( topt != 0){ void* _temp258= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp264; struct Cyc_Absyn_Conref*
_temp266; struct Cyc_Absyn_Tqual _temp268; struct Cyc_Absyn_Conref* _temp270;
void* _temp272; void* _temp274; _LL260: if(( unsigned int) _temp258 > 4u?*(( int*)
_temp258) == Cyc_Absyn_PointerType: 0){ _LL265: _temp264=(( struct Cyc_Absyn_PointerType_struct*)
_temp258)->f1; _LL275: _temp274=( void*) _temp264.elt_typ; goto _LL273; _LL273:
_temp272=( void*) _temp264.rgn_typ; goto _LL271; _LL271: _temp270= _temp264.nullable;
goto _LL269; _LL269: _temp268= _temp264.tq; goto _LL267; _LL267: _temp266=
_temp264.bounds; goto _LL261;} else{ goto _LL262;} _LL262: goto _LL263; _LL261:
inner_typ= _temp274; _temp256=( void**)& inner_typ; goto _LL259; _LL263: goto
_LL259; _LL259:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp309=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp309->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp309;})); struct Cyc_List_List* _temp278; struct Cyc_List_List*
_temp280; struct _tuple6 _temp276= Cyc_Tcpat_tcPatRec( te, _temp201, _temp256,(
void**)& ptr_rgn); _LL281: _temp280= _temp276.f1; goto _LL279; _LL279: _temp278=
_temp276.f2; goto _LL277; _LL277: tv_result=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp280); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp278); if( _temp256 == 0){
void* _temp282= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp201->topt))->v); struct Cyc_Absyn_TunionFieldInfo _temp288; struct Cyc_List_List*
_temp290; void* _temp292; struct Cyc_Absyn_Tunionfield* _temp294; struct Cyc_Absyn_Tuniondecl*
_temp296; _LL284: if(( unsigned int) _temp282 > 4u?*(( int*) _temp282) == Cyc_Absyn_TunionFieldType:
0){ _LL289: _temp288=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp282)->f1;
_LL293: _temp292=( void*) _temp288.field_info; if(*(( int*) _temp292) == Cyc_Absyn_KnownTunionfield){
_LL297: _temp296=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp292)->f1;
goto _LL295; _LL295: _temp294=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp292)->f2; goto _LL291;} else{ goto _LL286;} _LL291: _temp290= _temp288.targs;
goto _LL285;} else{ goto _LL286;} _LL286: goto _LL287; _LL285: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp298=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp298[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp299; _temp299.tag= Cyc_Absyn_TunionType;
_temp299.f1=({ struct Cyc_Absyn_TunionInfo _temp300; _temp300.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp301=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp301[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp302; _temp302.tag= Cyc_Absyn_KnownTunion;
_temp302.f1= _temp296; _temp302;}); _temp301;})); _temp300.targs= _temp290;
_temp300.rgn=( void*) ptr_rgn; _temp300;}); _temp299;}); _temp298;}); goto
_LL283; _LL287: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp303=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp303[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp304; _temp304.tag= Cyc_Absyn_PointerType;
_temp304.f1=({ struct Cyc_Absyn_PtrInfo _temp305; _temp305.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp201->topt))->v); _temp305.rgn_typ=(
void*) ptr_rgn; _temp305.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp305.tq= Cyc_Absyn_empty_tqual(); _temp305.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp305;}); _temp304;}); _temp303;}); goto _LL283;
_LL283:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp306=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp306[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp307; _temp307.tag= Cyc_Absyn_PointerType;
_temp307.f1=({ struct Cyc_Absyn_PtrInfo _temp308; _temp308.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp201->topt))->v); _temp308.rgn_typ=(
void*) ptr_rgn; _temp308.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp308.tq= Cyc_Absyn_empty_tqual(); _temp308.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp308;}); _temp307;}); _temp306;});} goto _LL148;}}
_LL170: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp310= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_List_List* _temp316; _LL312: if(( unsigned int) _temp310 > 4u?*(( int*)
_temp310) == Cyc_Absyn_TupleType: 0){ _LL317: _temp316=(( struct Cyc_Absyn_TupleType_struct*)
_temp310)->f1; goto _LL313;} else{ goto _LL314;} _LL314: goto _LL315; _LL313:
topt_ts= _temp316; goto _LL311; _LL315: goto _LL311; _LL311:;} for( 0; _temp203
!= 0; _temp203=(( struct Cyc_List_List*) _check_null( _temp203))->tl){ void**
_temp318= 0; goto _LL319; _LL319: if( topt_ts != 0){ _temp318=( void**)&(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( topt_ts))->hd)).f2;
topt_ts=(( struct Cyc_List_List*) _check_null( topt_ts))->tl;}{ struct Cyc_List_List*
_temp322; struct Cyc_List_List* _temp324; struct _tuple6 _temp320= Cyc_Tcpat_tcPatRec(
te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp203))->hd,
_temp318, region_opt); _LL325: _temp324= _temp320.f1; goto _LL323; _LL323:
_temp322= _temp320.f2; goto _LL321; _LL321: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp324); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp322); pat_ts=({ struct Cyc_List_List*
_temp326=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp326->hd=( void*)({ struct _tuple8* _temp327=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp327->f1= Cyc_Absyn_empty_tqual(); _temp327->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Pat*)(( struct
Cyc_List_List*) _check_null( _temp203))->hd)->topt))->v; _temp327;}); _temp326->tl=
pat_ts; _temp326;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp328=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp328[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp329; _temp329.tag= Cyc_Absyn_TupleType;
_temp329.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp329;}); _temp328;}); goto _LL148;} _LL172: if( _temp207 != 0){(
int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcPat: struct<...> not implemented",
sizeof( unsigned char), 35u)));}{ struct _RegionHandle _temp330= _new_region();
struct _RegionHandle* rgn=& _temp330; _push_region( rgn);{ struct _tuple4
_temp332=({ struct _tuple4 _temp331; _temp331.f1= Cyc_Tcenv_lookup_type_vars( te);
_temp331.f2= rgn; _temp331;}); goto _LL333; _LL333: { struct Cyc_List_List*
_temp334=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)(
struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp332, _temp212->tvs);
goto _LL335; _LL335: { struct Cyc_List_List* _temp336=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp334); goto _LL337; _LL337: t=( void*)({
struct Cyc_Absyn_StructType_struct* _temp338=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp338[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp339; _temp339.tag= Cyc_Absyn_StructType;
_temp339.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp212->name))->v); _temp339.f2= _temp336; _temp339.f3=({ struct
Cyc_Absyn_Structdecl** _temp340=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp340[ 0]= _temp212; _temp340;});
_temp339;}); _temp338;}); if( topt != 0){ Cyc_Tcutil_unify( t,*(( void**)
_check_null( topt)));}* _temp211=({ struct Cyc_Core_Opt* _temp341=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp341->v=( void*) _temp336;
_temp341;}); if( _temp212->fields == 0){ Cyc_Tcutil_terr( p->loc, _tag_arr("can't destructure an abstract struct",
sizeof( unsigned char), 37u)); _npop_handler( 0u); goto _LL148;}{ struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, p->loc, _temp205,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp212->fields))->v); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp344; struct Cyc_Absyn_Pat*
_temp345; struct Cyc_Absyn_Structfield* _temp347; struct _tuple9* _temp342=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp344=*
_temp342; _LL348: _temp347= _temp344.f1; goto _LL346; _LL346: _temp345= _temp344.f2;
goto _LL343; _LL343: { void* _temp349= Cyc_Tcutil_rsubstitute( rgn, _temp334,(
void*) _temp347->type); goto _LL350; _LL350: { struct Cyc_List_List* _temp353;
struct Cyc_List_List* _temp355; struct _tuple6 _temp351= Cyc_Tcpat_tcPatRec( te,
_temp345,( void**)& _temp349, region_opt); _LL356: _temp355= _temp351.f1; goto
_LL354; _LL354: _temp353= _temp351.f2; goto _LL352; _LL352: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp355); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp353); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp345->topt))->v, _temp349)){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp357=* _temp347->name;
struct _tagged_arr _temp358= Cyc_Absynpp_typ2string( _temp349); struct
_tagged_arr _temp359= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp345->topt))->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp357.last_plus_one - _temp357.curr, _temp357.curr, _temp358.last_plus_one -
_temp358.curr, _temp358.curr, _temp359.last_plus_one - _temp359.curr, _temp359.curr);}));}}}}}}}};
_pop_region(& _temp330);} goto _LL148; _LL174: if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp216) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp218->tvs)){ Cyc_Tcutil_terr( p->loc, _tag_arr("wrong number of existential type variables in pattern",
sizeof( unsigned char), 54u));}{ struct _RegionHandle _temp360= _new_region();
struct _RegionHandle* rgn=& _temp360; _push_region( rgn);{ struct Cyc_List_List*
_temp361= 0; goto _LL362; _LL362:{ struct Cyc_List_List* t= _temp216; for( 0; t
!= 0; t=(( struct Cyc_List_List*) _check_null( t))->tl){ struct Cyc_Absyn_Tvar*
tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( t))->hd;{
void* _temp363=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp373; void* _temp375;
struct Cyc_Absyn_Conref* _temp377; _LL365: if(( unsigned int) _temp363 > 1u?*((
int*) _temp363) == Cyc_Absyn_Eq_constr: 0){ _LL374: _temp373=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp363)->f1; if( _temp373 ==( void*) Cyc_Absyn_BoxKind){
goto _LL366;} else{ goto _LL367;}} else{ goto _LL367;} _LL367: if(( unsigned int)
_temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Eq_constr: 0){ _LL376: _temp375=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp363)->f1; goto _LL368;} else{
goto _LL369;} _LL369: if( _temp363 ==( void*) Cyc_Absyn_No_constr){ goto _LL370;}
else{ goto _LL371;} _LL371: if(( unsigned int) _temp363 > 1u?*(( int*) _temp363)
== Cyc_Absyn_Forward_constr: 0){ _LL378: _temp377=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp363)->f1; goto _LL372;} else{ goto _LL364;} _LL366: goto _LL364; _LL368:
Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp379= Cyc_Absynpp_qvar2string(
_temp220->name); struct _tagged_arr _temp380=* tv->name; struct _tagged_arr
_temp381= Cyc_Absynpp_kind2string( _temp375); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp379.last_plus_one - _temp379.curr, _temp379.curr, _temp380.last_plus_one -
_temp380.curr, _temp380.curr, _temp381.last_plus_one - _temp381.curr, _temp381.curr);}));
goto _LL364; _LL370:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp382=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp382[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp383; _temp383.tag= Cyc_Absyn_Eq_constr;
_temp383.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp383;}); _temp382;})));
goto _LL364; _LL372:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("forward after compress in conref", sizeof( unsigned char), 33u)));
_LL364:;} _temp361=({ struct Cyc_List_List* _temp384=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp384->hd=( void*)((
void*)({ struct Cyc_Absyn_VarType_struct* _temp385=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp385[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp386; _temp386.tag= Cyc_Absyn_VarType; _temp386.f1= tv; _temp386;});
_temp385;})); _temp384->tl= _temp361; _temp384;});}} _temp361=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp361); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp216);{ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te,
_temp216); struct Cyc_List_List* tqts= _temp218->typs; struct Cyc_List_List*
_temp387= Cyc_Tcenv_lookup_type_vars( te); goto _LL388; _LL388: { struct _tuple4
_temp390=({ struct _tuple4 _temp389; _temp389.f1= _temp387; _temp389.f2= rgn;
_temp389;}); goto _LL391; _LL391: { struct Cyc_List_List* _temp392=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp390, _temp220->tvs); goto _LL393; _LL393: { struct Cyc_List_List* _temp394=((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp218->tvs, _temp361); goto _LL395; _LL395: { struct Cyc_List_List* _temp396=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp392); goto _LL397;
_LL397: { struct Cyc_List_List* _temp398=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple5*)) Cyc_Core_snd, _temp394); goto _LL399; _LL399: { struct Cyc_List_List*
_temp400=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp392, _temp394); goto
_LL401; _LL401: if( tqts == 0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp402=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp402[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp403; _temp403.tag= Cyc_Absyn_TunionType;
_temp403.f1=({ struct Cyc_Absyn_TunionInfo _temp404; _temp404.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp406=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp406[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp407; _temp407.tag= Cyc_Absyn_KnownTunion;
_temp407.f1= _temp220; _temp407;}); _temp406;})); _temp404.targs= _temp396;
_temp404.rgn=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({
struct Cyc_Core_Opt* _temp405=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp405->v=( void*) _temp387; _temp405;})); _temp404;});
_temp403;}); _temp402;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp408=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp408[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp409; _temp409.tag= Cyc_Absyn_TunionFieldType; _temp409.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp410; _temp410.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp411=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp411[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp412; _temp412.tag= Cyc_Absyn_KnownTunionfield; _temp412.f1= _temp220;
_temp412.f2= _temp218; _temp412;}); _temp411;})); _temp410.targs= _temp396;
_temp410;}); _temp409;}); _temp408;});} if( topt != 0? tqts == 0: 0){ void*
_temp413= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo
_temp421; struct Cyc_Absyn_TunionInfo _temp423; void* _temp425; struct Cyc_List_List*
_temp427; void* _temp429; _LL415: if(( unsigned int) _temp413 > 4u?*(( int*)
_temp413) == Cyc_Absyn_TunionFieldType: 0){ _LL422: _temp421=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp413)->f1; goto _LL416;} else{ goto _LL417;} _LL417: if(( unsigned int)
_temp413 > 4u?*(( int*) _temp413) == Cyc_Absyn_TunionType: 0){ _LL424: _temp423=((
struct Cyc_Absyn_TunionType_struct*) _temp413)->f1; _LL430: _temp429=( void*)
_temp423.tunion_info; goto _LL428; _LL428: _temp427= _temp423.targs; goto _LL426;
_LL426: _temp425=( void*) _temp423.rgn; goto _LL418;} else{ goto _LL419;} _LL419:
goto _LL420; _LL416: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp431=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp431[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp432; _temp432.tag= Cyc_Absyn_TunionFieldType; _temp432.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp433; _temp433.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp434=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp434[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp435; _temp435.tag= Cyc_Absyn_KnownTunionfield; _temp435.f1= _temp220;
_temp435.f2= _temp218; _temp435;}); _temp434;})); _temp433.targs= _temp396;
_temp433;}); _temp432;}); _temp431;}); goto _LL414; _LL418: { struct Cyc_List_List*
_temp436= _temp396; goto _LL437; _LL437: for( 0; _temp436 != 0? _temp427 != 0: 0;(
_temp436=(( struct Cyc_List_List*) _check_null( _temp436))->tl, _temp427=((
struct Cyc_List_List*) _check_null( _temp427))->tl)){ Cyc_Tcutil_unify(( void*)((
struct Cyc_List_List*) _check_null( _temp436))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp427))->hd);} goto _LL414;} _LL420: goto _LL414; _LL414:;} for(
0; _temp214 != 0? tqts != 0: 0;( _temp214=(( struct Cyc_List_List*) _check_null(
_temp214))->tl, tqts=(( struct Cyc_List_List*) _check_null( tqts))->tl)){ struct
Cyc_Absyn_Pat* _temp438=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp214))->hd; goto _LL439; _LL439: { void* _temp440= Cyc_Tcutil_rsubstitute(
rgn, _temp400,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( tqts))->hd)).f2);
goto _LL441; _LL441: { struct Cyc_List_List* _temp444; struct Cyc_List_List*
_temp446; struct _tuple6 _temp442= Cyc_Tcpat_tcPatRec( te2, _temp438,( void**)&
_temp440, region_opt); _LL447: _temp446= _temp442.f1; goto _LL445; _LL445:
_temp444= _temp442.f2; goto _LL443; _LL443: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp446); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp444); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp438->topt))->v, _temp440)){ Cyc_Tcutil_terr(
_temp438->loc,( struct _tagged_arr)({ struct _tagged_arr _temp448= Cyc_Absynpp_qvar2string(
_temp220->name); struct _tagged_arr _temp449= Cyc_Absynpp_typ2string( _temp440);
struct _tagged_arr _temp450= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp438->topt))->v); xprintf("%.*s expects argument type %.*s, not %.*s",
_temp448.last_plus_one - _temp448.curr, _temp448.curr, _temp449.last_plus_one -
_temp449.curr, _temp449.curr, _temp450.last_plus_one - _temp450.curr, _temp450.curr);}));}}}}
if( _temp214 != 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp451= Cyc_Absynpp_qvar2string( _temp220->name); xprintf("too many arguments for tunion constructor %.*s",
_temp451.last_plus_one - _temp451.curr, _temp451.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp452= Cyc_Absynpp_qvar2string(
_temp220->name); xprintf("too few arguments for tunion constructor %.*s",
_temp452.last_plus_one - _temp452.curr, _temp452.curr);}));}}}}}}}}};
_pop_region(& _temp360);} goto _LL148; _LL176: goto _LL178; _LL178: goto _LL180;
_LL180: t= Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp453=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp453->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp453;})); goto _LL148; _LL148:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt*
_temp454=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp454->v=( void*) t; _temp454;}); return({ struct _tuple6 _temp455; _temp455.f1=
tv_result; _temp455.f2= v_result; _temp455;});}} struct _tuple6 Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** topt, void**
region_opt){ struct _tuple6 _temp456= Cyc_Tcpat_tcPatRec( te, p, topt,
region_opt); goto _LL457; _LL457:{ struct _RegionHandle _temp458= _new_region();
struct _RegionHandle* r=& _temp458; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name,
_temp456.f2), p->loc, _tag_arr("pattern contains a repeated variable", sizeof(
unsigned char), 37u));; _pop_region(& _temp458);} return _temp456;} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp459=( void*) p->r;
struct Cyc_Absyn_Pat* _temp471; struct Cyc_List_List* _temp473; struct Cyc_List_List*
_temp475; struct Cyc_Absyn_Tunionfield* _temp477; struct Cyc_Absyn_Tuniondecl*
_temp479; struct Cyc_List_List* _temp481; struct Cyc_List_List* _temp483; struct
Cyc_Core_Opt* _temp485; struct Cyc_Absyn_Structdecl* _temp487; struct Cyc_List_List*
_temp489; _LL461: if(( unsigned int) _temp459 > 2u?*(( int*) _temp459) == Cyc_Absyn_Pointer_p:
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
te, p->loc, _temp513); return; _LL498:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp519=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp519[ 0]=({ struct Cyc_Core_Impossible_struct _temp520; _temp520.tag= Cyc_Core_Impossible;
_temp520.f1= _tag_arr("check_pat_regions: bad pointer type", sizeof(
unsigned char), 36u); _temp520;}); _temp519;})); _LL492:;} _LL464: for( 0;
_temp473 != 0; _temp473=(( struct Cyc_List_List*) _check_null( _temp473))->tl){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp473))->hd);}{ void* _temp521=( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo _temp529; void* _temp531;
struct Cyc_List_List* _temp533; void* _temp535; struct Cyc_Absyn_TunionFieldInfo
_temp537; _LL523: if(( unsigned int) _temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_TunionType:
0){ _LL530: _temp529=(( struct Cyc_Absyn_TunionType_struct*) _temp521)->f1;
_LL536: _temp535=( void*) _temp529.tunion_info; goto _LL534; _LL534: _temp533=
_temp529.targs; goto _LL532; _LL532: _temp531=( void*) _temp529.rgn; goto _LL524;}
else{ goto _LL525;} _LL525: if(( unsigned int) _temp521 > 4u?*(( int*) _temp521)
== Cyc_Absyn_TunionFieldType: 0){ _LL538: _temp537=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp521)->f1; goto _LL526;} else{ goto _LL527;} _LL527: goto _LL528; _LL524:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp531); return; _LL526: return;
_LL528:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp539=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp539[ 0]=({ struct Cyc_Core_Impossible_struct _temp540; _temp540.tag= Cyc_Core_Impossible;
_temp540.f1= _tag_arr("check_pat_regions: bad tunion type", sizeof(
unsigned char), 35u); _temp540;}); _temp539;})); _LL522:;} _LL466: _temp489=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(* f)( struct _tuple7*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)( struct _tuple7*))
Cyc_Core_snd, _temp481); goto _LL468; _LL468: for( 0; _temp489 != 0; _temp489=((
struct Cyc_List_List*) _check_null( _temp489))->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp489))->hd);}
return; _LL470: return; _LL460:;} static const int Cyc_Tcpat_Name_v= 0; struct
Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_arr f1; } ; static const int
Cyc_Tcpat_Int_v= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; struct
Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ; static
const int Cyc_Tcpat_Any= 0; static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static int
Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){
void* _temp541=( void*) c1->name; struct _tagged_arr _temp547; int _temp549;
_LL543: if(*(( int*) _temp541) == Cyc_Tcpat_Name_v){ _LL548: _temp547=(( struct
Cyc_Tcpat_Name_v_struct*) _temp541)->f1; goto _LL544;} else{ goto _LL545;}
_LL545: if(*(( int*) _temp541) == Cyc_Tcpat_Int_v){ _LL550: _temp549=(( struct
Cyc_Tcpat_Int_v_struct*) _temp541)->f1; goto _LL546;} else{ goto _LL542;} _LL544: {
void* _temp551=( void*) c2->name; struct _tagged_arr _temp557; int _temp559;
_LL553: if(*(( int*) _temp551) == Cyc_Tcpat_Name_v){ _LL558: _temp557=(( struct
Cyc_Tcpat_Name_v_struct*) _temp551)->f1; goto _LL554;} else{ goto _LL555;}
_LL555: if(*(( int*) _temp551) == Cyc_Tcpat_Int_v){ _LL560: _temp559=(( struct
Cyc_Tcpat_Int_v_struct*) _temp551)->f1; goto _LL556;} else{ goto _LL552;} _LL554:
return Cyc_String_zstrcmp( _temp547, _temp557); _LL556: return - 1; _LL552:;}
_LL546: { void* _temp561=( void*) c2->name; struct _tagged_arr _temp567; int
_temp569; _LL563: if(*(( int*) _temp561) == Cyc_Tcpat_Name_v){ _LL568: _temp567=((
struct Cyc_Tcpat_Name_v_struct*) _temp561)->f1; goto _LL564;} else{ goto _LL565;}
_LL565: if(*(( int*) _temp561) == Cyc_Tcpat_Int_v){ _LL570: _temp569=(( struct
Cyc_Tcpat_Int_v_struct*) _temp561)->f1; goto _LL566;} else{ goto _LL562;} _LL564:
return 1; _LL566: return _temp549 - _temp569; _LL562:;} _LL542:;} static struct
Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)( int(*
comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)( Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={( void*) 1}; static struct Cyc_Core_Opt
Cyc_Tcpat_two_opt={( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static unsigned char _temp572[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,{ _temp572,
_temp572, _temp572 + 5u}}; static unsigned char _temp574[ 4u]="ptr"; static
struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,{ _temp574,
_temp574, _temp574 + 4u}}; static unsigned char _temp576[ 6u]="tuple"; static
struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp576,
_temp576, _temp576 + 6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp577=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp577->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp578=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp578[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp579; _temp579.tag= Cyc_Tcpat_Int_v;
_temp579.f1= i; _temp579;}); _temp578;})); _temp577->arity= 0; _temp577->span= 0;
_temp577;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_arr f){ return({ struct Cyc_Tcpat_Con_s* _temp580=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp580->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp581=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp581[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp582; _temp582.tag= Cyc_Tcpat_Name_v; _temp582.f1= f; _temp582;}); _temp581;}));
_temp580->arity= 0; _temp580->span= 0; _temp580;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp583=(
struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp583->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp585=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp585[ 0]=({ struct Cyc_Tcpat_Int_v_struct
_temp586; _temp586.tag= Cyc_Tcpat_Int_v; _temp586.f1=( int) c; _temp586;});
_temp585;})); _temp583->arity= 0; _temp583->span=({ struct Cyc_Core_Opt*
_temp584=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp584->v=( void*) 256; _temp584;}); _temp583;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s* _temp587=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp587->name=(
void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp587->arity= i; _temp587->span=
Cyc_Tcpat_one_opt_ptr; _temp587;});} static void* Cyc_Tcpat_null_pat(){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp588=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp588[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp589; _temp589.tag= Cyc_Tcpat_Con; _temp589.f1= Cyc_Tcpat_null_con; _temp589.f2=
0; _temp589;}); _temp588;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp590=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp590[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp591; _temp591.tag= Cyc_Tcpat_Con; _temp591.f1= Cyc_Tcpat_int_con( i);
_temp591.f2= 0; _temp591;}); _temp590;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp592=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp592[ 0]=({ struct Cyc_Tcpat_Con_struct _temp593; _temp593.tag= Cyc_Tcpat_Con;
_temp593.f1= Cyc_Tcpat_char_con( c); _temp593.f2= 0; _temp593;}); _temp592;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_arr f){ return( void*)({ struct
Cyc_Tcpat_Con_struct* _temp594=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp594[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp595; _temp595.tag= Cyc_Tcpat_Con; _temp595.f1= Cyc_Tcpat_float_con( f);
_temp595.f2= 0; _temp595;}); _temp594;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp596=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp596[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp597; _temp597.tag= Cyc_Tcpat_Con; _temp597.f1= Cyc_Tcpat_null_ptr_con;
_temp597.f2=({ struct Cyc_List_List* _temp598=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp598->hd=( void*) p; _temp598->tl= 0;
_temp598;}); _temp597;}); _temp596;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp599=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp599[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp600; _temp600.tag= Cyc_Tcpat_Con; _temp600.f1= Cyc_Tcpat_ptr_con; _temp600.f2=({
struct Cyc_List_List* _temp601=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp601->hd=( void*) p; _temp601->tl= 0; _temp601;});
_temp600;}); _temp599;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp602=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp602[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp603; _temp603.tag= Cyc_Tcpat_Con; _temp603.f1= Cyc_Tcpat_tuple_con((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ss)); _temp603.f2= ss; _temp603;});
_temp602;});} static void* Cyc_Tcpat_con_pat( struct _tagged_arr con_name,
struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){ struct Cyc_Tcpat_Con_s* c=({
struct Cyc_Tcpat_Con_s* _temp606=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp606->name=( void*)(( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp607=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp607[ 0]=({ struct Cyc_Tcpat_Name_v_struct _temp608; _temp608.tag= Cyc_Tcpat_Name_v;
_temp608.f1= con_name; _temp608;}); _temp607;})); _temp606->arity=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ps); _temp606->span= span; _temp606;});
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp604=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp604[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp605; _temp605.tag= Cyc_Tcpat_Con; _temp605.f1= c; _temp605.f2= ps; _temp605;});
_temp604;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp609=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp643; int
_temp645; void* _temp647; unsigned char _temp649; struct _tagged_arr _temp651;
struct Cyc_Absyn_Vardecl* _temp653; struct Cyc_Absyn_Pat* _temp655; struct Cyc_List_List*
_temp657; struct Cyc_List_List* _temp659; struct Cyc_Absyn_Tunionfield* _temp661;
struct Cyc_Absyn_Tuniondecl* _temp663; struct Cyc_List_List* _temp665; struct
Cyc_List_List* _temp667; struct Cyc_Absyn_Tunionfield* _temp669; struct Cyc_Absyn_Tuniondecl*
_temp671; struct Cyc_List_List* _temp673; struct Cyc_List_List* _temp675; struct
Cyc_List_List* _temp677; struct Cyc_Core_Opt* _temp679; struct Cyc_Absyn_Structdecl*
_temp681; struct Cyc_Absyn_Enumfield* _temp683; struct Cyc_Absyn_Enumdecl*
_temp685; struct _tuple1* _temp687; struct Cyc_List_List* _temp689; struct Cyc_List_List*
_temp691; struct _tuple1* _temp693; struct Cyc_List_List* _temp695; struct Cyc_List_List*
_temp697; struct _tuple1* _temp699; _LL611: if( _temp609 ==( void*) Cyc_Absyn_Wild_p){
goto _LL612;} else{ goto _LL613;} _LL613: if(( unsigned int) _temp609 > 2u?*((
int*) _temp609) == Cyc_Absyn_Var_p: 0){ _LL644: _temp643=(( struct Cyc_Absyn_Var_p_struct*)
_temp609)->f1; goto _LL614;} else{ goto _LL615;} _LL615: if( _temp609 ==( void*)
Cyc_Absyn_Null_p){ goto _LL616;} else{ goto _LL617;} _LL617: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Int_p: 0){ _LL648: _temp647=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp609)->f1; goto _LL646; _LL646:
_temp645=(( struct Cyc_Absyn_Int_p_struct*) _temp609)->f2; goto _LL618;} else{
goto _LL619;} _LL619: if(( unsigned int) _temp609 > 2u?*(( int*) _temp609) ==
Cyc_Absyn_Char_p: 0){ _LL650: _temp649=(( struct Cyc_Absyn_Char_p_struct*)
_temp609)->f1; goto _LL620;} else{ goto _LL621;} _LL621: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Float_p: 0){ _LL652: _temp651=((
struct Cyc_Absyn_Float_p_struct*) _temp609)->f1; goto _LL622;} else{ goto _LL623;}
_LL623: if(( unsigned int) _temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Reference_p:
0){ _LL654: _temp653=(( struct Cyc_Absyn_Reference_p_struct*) _temp609)->f1;
goto _LL624;} else{ goto _LL625;} _LL625: if(( unsigned int) _temp609 > 2u?*((
int*) _temp609) == Cyc_Absyn_Pointer_p: 0){ _LL656: _temp655=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp609)->f1; goto _LL626;} else{ goto _LL627;} _LL627: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Tunion_p: 0){ _LL664: _temp663=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f1; goto _LL662; _LL662: _temp661=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f2; goto _LL660; _LL660: _temp659=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f3; goto _LL658; _LL658: _temp657=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f4; if( _temp657 == 0){ goto
_LL628;} else{ goto _LL629;}} else{ goto _LL629;} _LL629: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Tunion_p: 0){ _LL672: _temp671=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f1; goto _LL670; _LL670: _temp669=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f2; goto _LL668; _LL668: _temp667=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f3; goto _LL666; _LL666: _temp665=((
struct Cyc_Absyn_Tunion_p_struct*) _temp609)->f4; goto _LL630;} else{ goto
_LL631;} _LL631: if(( unsigned int) _temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Tuple_p:
0){ _LL674: _temp673=(( struct Cyc_Absyn_Tuple_p_struct*) _temp609)->f1; goto
_LL632;} else{ goto _LL633;} _LL633: if(( unsigned int) _temp609 > 2u?*(( int*)
_temp609) == Cyc_Absyn_Struct_p: 0){ _LL682: _temp681=(( struct Cyc_Absyn_Struct_p_struct*)
_temp609)->f1; goto _LL680; _LL680: _temp679=(( struct Cyc_Absyn_Struct_p_struct*)
_temp609)->f2; goto _LL678; _LL678: _temp677=(( struct Cyc_Absyn_Struct_p_struct*)
_temp609)->f3; goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_Struct_p_struct*)
_temp609)->f4; goto _LL634;} else{ goto _LL635;} _LL635: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_Enum_p: 0){ _LL686: _temp685=((
struct Cyc_Absyn_Enum_p_struct*) _temp609)->f1; goto _LL684; _LL684: _temp683=((
struct Cyc_Absyn_Enum_p_struct*) _temp609)->f2; goto _LL636;} else{ goto _LL637;}
_LL637: if(( unsigned int) _temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_UnknownId_p:
0){ _LL688: _temp687=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp609)->f1;
goto _LL638;} else{ goto _LL639;} _LL639: if(( unsigned int) _temp609 > 2u?*((
int*) _temp609) == Cyc_Absyn_UnknownCall_p: 0){ _LL694: _temp693=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp609)->f1; goto _LL692; _LL692: _temp691=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp609)->f2; goto _LL690; _LL690: _temp689=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp609)->f3; goto _LL640;} else{ goto _LL641;} _LL641: if(( unsigned int)
_temp609 > 2u?*(( int*) _temp609) == Cyc_Absyn_UnknownFields_p: 0){ _LL700:
_temp699=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp609)->f1; goto _LL698;
_LL698: _temp697=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp609)->f2;
goto _LL696; _LL696: _temp695=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp609)->f3; goto _LL642;} else{ goto _LL610;} _LL612: s=( void*) Cyc_Tcpat_Any;
goto _LL610; _LL614: s=( void*) Cyc_Tcpat_Any; goto _LL610; _LL616: s= Cyc_Tcpat_null_pat();
goto _LL610; _LL618: s= Cyc_Tcpat_int_pat( _temp645); goto _LL610; _LL620: s=
Cyc_Tcpat_char_pat( _temp649); goto _LL610; _LL622: s= Cyc_Tcpat_float_pat(
_temp651); goto _LL610; _LL624: s=( void*) Cyc_Tcpat_Any; goto _LL610; _LL626:{
void* _temp701= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
p->topt))->v); struct Cyc_Absyn_PtrInfo _temp709; struct Cyc_Absyn_Conref*
_temp711; struct Cyc_Absyn_Tqual _temp713; struct Cyc_Absyn_Conref* _temp715;
void* _temp717; void* _temp719; struct Cyc_Absyn_TunionInfo _temp721; _LL703:
if(( unsigned int) _temp701 > 4u?*(( int*) _temp701) == Cyc_Absyn_PointerType: 0){
_LL710: _temp709=(( struct Cyc_Absyn_PointerType_struct*) _temp701)->f1; _LL720:
_temp719=( void*) _temp709.elt_typ; goto _LL718; _LL718: _temp717=( void*)
_temp709.rgn_typ; goto _LL716; _LL716: _temp715= _temp709.nullable; goto _LL714;
_LL714: _temp713= _temp709.tq; goto _LL712; _LL712: _temp711= _temp709.bounds;
goto _LL704;} else{ goto _LL705;} _LL705: if(( unsigned int) _temp701 > 4u?*((
int*) _temp701) == Cyc_Absyn_TunionType: 0){ _LL722: _temp721=(( struct Cyc_Absyn_TunionType_struct*)
_temp701)->f1; goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL704: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp723=(
void*) _temp715->v; struct Cyc_Absyn_Conref* _temp731; int _temp733; _LL725: if((
unsigned int) _temp723 > 1u?*(( int*) _temp723) == Cyc_Absyn_Forward_constr: 0){
_LL732: _temp731=(( struct Cyc_Absyn_Forward_constr_struct*) _temp723)->f1; goto
_LL726;} else{ goto _LL727;} _LL727: if( _temp723 ==( void*) Cyc_Absyn_No_constr){
goto _LL728;} else{ goto _LL729;} _LL729: if(( unsigned int) _temp723 > 1u?*((
int*) _temp723) == Cyc_Absyn_Eq_constr: 0){ _LL734: _temp733=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp723)->f1; goto _LL730;} else{ goto _LL724;} _LL726:( void*)( _temp715->v=(
void*)(( void*) _temp731->v)); continue; _LL728:( void*)( _temp715->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp735=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp735[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp736; _temp736.tag= Cyc_Absyn_Eq_constr; _temp736.f1=(
void*) 0; _temp736;}); _temp735;}))); is_nullable= 0; still_working= 0; goto
_LL724; _LL730: is_nullable=( int) _temp733; still_working= 0; goto _LL724;
_LL724:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp655); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL702;}} _LL706:{ void* _temp737=(
void*) _temp655->r; struct Cyc_List_List* _temp743; struct Cyc_List_List*
_temp745; struct Cyc_Absyn_Tunionfield* _temp747; struct Cyc_Absyn_Tuniondecl*
_temp749; _LL739: if(( unsigned int) _temp737 > 2u?*(( int*) _temp737) == Cyc_Absyn_Tunion_p:
0){ _LL750: _temp749=(( struct Cyc_Absyn_Tunion_p_struct*) _temp737)->f1; goto
_LL748; _LL748: _temp747=(( struct Cyc_Absyn_Tunion_p_struct*) _temp737)->f2;
goto _LL746; _LL746: _temp745=(( struct Cyc_Absyn_Tunion_p_struct*) _temp737)->f3;
goto _LL744; _LL744: _temp743=(( struct Cyc_Absyn_Tunion_p_struct*) _temp737)->f4;
goto _LL740;} else{ goto _LL741;} _LL741: goto _LL742; _LL740: { struct Cyc_Core_Opt*
span; if( _temp749->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp751=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp751->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp749->fields))->v);
_temp751;});} s= Cyc_Tcpat_con_pat(*(* _temp747->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp743)); goto _LL738;} _LL742: s=(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("non-[x]tunion pattern has tunion type", sizeof( unsigned char), 38u));
goto _LL738; _LL738:;} goto _LL702; _LL708: s=(( void*(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("pointer pattern does not have pointer type!",
sizeof( unsigned char), 44u)); goto _LL702; _LL702:;} goto _LL610; _LL628: {
struct Cyc_Core_Opt* span;{ void* _temp752= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( p->topt))->v); struct Cyc_Absyn_TunionInfo
_temp760; struct Cyc_Absyn_TunionFieldInfo _temp762; _LL754: if(( unsigned int)
_temp752 > 4u?*(( int*) _temp752) == Cyc_Absyn_TunionType: 0){ _LL761: _temp760=((
struct Cyc_Absyn_TunionType_struct*) _temp752)->f1; goto _LL755;} else{ goto
_LL756;} _LL756: if(( unsigned int) _temp752 > 4u?*(( int*) _temp752) == Cyc_Absyn_TunionFieldType:
0){ _LL763: _temp762=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp752)->f1;
goto _LL757;} else{ goto _LL758;} _LL758: goto _LL759; _LL755: if( _temp663->is_xtunion){
span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp764=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp764->v=( void*)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp663->fields))->v); _temp764;});} goto _LL753; _LL757: span=({
struct Cyc_Core_Opt* _temp765=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp765->v=( void*) 1; _temp765;}); goto _LL753; _LL759:
span=(( struct Cyc_Core_Opt*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("void tunion pattern has bad type", sizeof( unsigned char), 33u)); goto
_LL753; _LL753:;} s= Cyc_Tcpat_con_pat(*(* _temp661->name).f2, span, 0); goto
_LL610;} _LL630: _temp673= _temp665; goto _LL632; _LL632: s= Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp673)); goto _LL610; _LL634: {
struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp681->fields))->v; for( 0; fields != 0;
fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ int found= Cyc_String_strcmp(*((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fields))->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) == 0;{ struct Cyc_List_List* dlps0=
_temp675; for( 0; ! found? dlps0 != 0: 0; dlps0=(( struct Cyc_List_List*)
_check_null( dlps0))->tl){ struct _tuple7 _temp768; struct Cyc_Absyn_Pat*
_temp769; struct Cyc_List_List* _temp771; struct _tuple7* _temp766=( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( dlps0))->hd; _temp768=* _temp766;
_LL772: _temp771= _temp768.f1; goto _LL770; _LL770: _temp769= _temp768.f2; goto
_LL767; _LL767: { struct Cyc_List_List* _temp773= _temp771; struct Cyc_List_List
_temp779; struct Cyc_List_List* _temp780; void* _temp782; struct _tagged_arr*
_temp784; _LL775: if( _temp773 == 0){ goto _LL777;} else{ _temp779=* _temp773;
_LL783: _temp782=( void*) _temp779.hd; if(*(( int*) _temp782) == Cyc_Absyn_FieldName){
_LL785: _temp784=(( struct Cyc_Absyn_FieldName_struct*) _temp782)->f1; goto
_LL781;} else{ goto _LL777;} _LL781: _temp780= _temp779.tl; if( _temp780 == 0){
goto _LL776;} else{ goto _LL777;}} _LL777: goto _LL778; _LL776: if( Cyc_String_zstrptrcmp(
_temp784,(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd)->name) == 0){ ps=({ struct Cyc_List_List* _temp786=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp786->hd=( void*) Cyc_Tcpat_compile_pat(
_temp769); _temp786->tl= ps; _temp786;}); found= 1;} goto _LL774; _LL778:(( int(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad designator(s)", sizeof(
unsigned char), 18u)); goto _LL774; _LL774:;}}} if( ! found){(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad designator", sizeof(
unsigned char), 15u));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL610;} _LL636: {
int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp685->fields))->v); s= Cyc_Tcpat_con_pat(*(*
_temp683->name).f2,({ struct Cyc_Core_Opt* _temp787=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp787->v=( void*) span;
_temp787;}), 0); goto _LL610;} _LL638: s=(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("compile_pat: unknownid", sizeof( unsigned char), 23u)); goto _LL610;
_LL640: s=(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("compile_pat: unknowncall",
sizeof( unsigned char), 25u)); goto _LL610; _LL642: s=(( void*(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("compile_pat: unknownfields", sizeof(
unsigned char), 27u)); goto _LL610; _LL610:;} return s;} static const int Cyc_Tcpat_Pos=
0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag;
struct Cyc_Set_Set* f1; } ; static const int Cyc_Tcpat_Failure= 0; static const
int Cyc_Tcpat_Success= 0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1; }
; static const int Cyc_Tcpat_IfEq= 1; struct Cyc_Tcpat_IfEq_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s* f2; void* f3; void* f4; } ;
struct _tuple10{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; } ; static const int Cyc_Tcpat_Yes= 0; static const int Cyc_Tcpat_No= 1;
static const int Cyc_Tcpat_Maybe= 2; static void* Cyc_Tcpat_add_neg( void* td,
struct Cyc_Tcpat_Con_s* c){ void* _temp788= td; struct Cyc_Set_Set* _temp794;
struct Cyc_List_List* _temp796; struct Cyc_Tcpat_Con_s* _temp798; _LL790: if(*((
int*) _temp788) == Cyc_Tcpat_Neg){ _LL795: _temp794=(( struct Cyc_Tcpat_Neg_struct*)
_temp788)->f1; goto _LL791;} else{ goto _LL792;} _LL792: if(*(( int*) _temp788)
== Cyc_Tcpat_Pos){ _LL799: _temp798=(( struct Cyc_Tcpat_Pos_struct*) _temp788)->f1;
goto _LL797; _LL797: _temp796=(( struct Cyc_Tcpat_Pos_struct*) _temp788)->f2;
goto _LL793;} else{ goto _LL789;} _LL791: if((( int(*)( struct Cyc_Set_Set* s,
struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp794, c)){(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("add_neg called when constructor already in set",
sizeof( unsigned char), 47u));} if( c->span != 0?(( int(*)( struct Cyc_Set_Set*
s)) Cyc_Set_cardinality)( _temp794) + 1 >=( int)(( struct Cyc_Core_Opt*)
_check_null( c->span))->v: 0){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("add_neg called when |cs U {c}| >= span(c)", sizeof( unsigned char), 42u));}
return( void*)({ struct Cyc_Tcpat_Neg_struct* _temp800=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp800[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp801; _temp801.tag= Cyc_Tcpat_Neg; _temp801.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp794,
c); _temp801;}); _temp800;}); _LL793: return(( void*(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("add_neg called when td is Positive", sizeof(
unsigned char), 35u)); _LL789:;} static void* Cyc_Tcpat_static_match( struct Cyc_Tcpat_Con_s*
c, void* td){ void* _temp802= td; struct Cyc_List_List* _temp808; struct Cyc_Tcpat_Con_s*
_temp810; struct Cyc_Set_Set* _temp812; _LL804: if(*(( int*) _temp802) == Cyc_Tcpat_Pos){
_LL811: _temp810=(( struct Cyc_Tcpat_Pos_struct*) _temp802)->f1; goto _LL809;
_LL809: _temp808=(( struct Cyc_Tcpat_Pos_struct*) _temp802)->f2; goto _LL805;}
else{ goto _LL806;} _LL806: if(*(( int*) _temp802) == Cyc_Tcpat_Neg){ _LL813:
_temp812=(( struct Cyc_Tcpat_Neg_struct*) _temp802)->f1; goto _LL807;} else{
goto _LL803;} _LL805: if( Cyc_Tcpat_compare_con( c, _temp810) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL807: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp812,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)(( struct Cyc_Core_Opt*)
_check_null( c->span))->v ==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp812) + 1: 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}}
_LL803:;} struct _tuple11{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;
} ; static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt,
void* dsc){ struct Cyc_List_List* _temp814= ctxt; struct Cyc_List_List _temp820;
struct Cyc_List_List* _temp821; struct _tuple11* _temp823; struct _tuple11
_temp825; struct Cyc_List_List* _temp826; struct Cyc_Tcpat_Con_s* _temp828;
_LL816: if( _temp814 == 0){ goto _LL817;} else{ goto _LL818;} _LL818: if(
_temp814 == 0){ goto _LL815;} else{ _temp820=* _temp814; _LL824: _temp823=(
struct _tuple11*) _temp820.hd; _temp825=* _temp823; _LL829: _temp828= _temp825.f1;
goto _LL827; _LL827: _temp826= _temp825.f2; goto _LL822; _LL822: _temp821=
_temp820.tl; goto _LL819;} _LL817: return 0; _LL819: return({ struct Cyc_List_List*
_temp830=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp830->hd=( void*)({ struct _tuple11* _temp831=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp831->f1= _temp828; _temp831->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp832=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp832->hd=( void*) dsc; _temp832->tl= _temp826;
_temp832;}); _temp831;}); _temp830->tl= _temp821; _temp830;}); _LL815:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp833= ctxt; struct Cyc_List_List _temp839; struct Cyc_List_List*
_temp840; struct _tuple11* _temp842; struct _tuple11 _temp844; struct Cyc_List_List*
_temp845; struct Cyc_Tcpat_Con_s* _temp847; _LL835: if( _temp833 == 0){ goto
_LL836;} else{ goto _LL837;} _LL837: if( _temp833 == 0){ goto _LL834;} else{
_temp839=* _temp833; _LL843: _temp842=( struct _tuple11*) _temp839.hd; _temp844=*
_temp842; _LL848: _temp847= _temp844.f1; goto _LL846; _LL846: _temp845= _temp844.f2;
goto _LL841; _LL841: _temp840= _temp839.tl; goto _LL838;} _LL836: return((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("norm_context: empty context",
sizeof( unsigned char), 28u)); _LL838: return Cyc_Tcpat_augment( _temp840,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp849=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp849[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp850; _temp850.tag= Cyc_Tcpat_Pos; _temp850.f1= _temp847; _temp850.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp845);
_temp850;}); _temp849;})); _LL834:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple6
_temp852=({ struct _tuple6 _temp851; _temp851.f1= ctxt; _temp851.f2= work;
_temp851;}); struct Cyc_List_List* _temp862; struct Cyc_List_List* _temp864;
struct Cyc_List_List* _temp866; struct Cyc_List_List* _temp868; struct Cyc_List_List*
_temp870; struct Cyc_List_List* _temp872; struct Cyc_List_List* _temp874; struct
Cyc_List_List _temp876; struct Cyc_List_List* _temp877; struct _tuple10*
_temp879; struct _tuple10 _temp881; struct Cyc_List_List* _temp882; struct Cyc_List_List*
_temp884; struct Cyc_List_List* _temp886; struct Cyc_List_List* _temp888; struct
Cyc_List_List _temp890; struct Cyc_List_List* _temp891; struct _tuple11*
_temp893; struct _tuple11 _temp895; struct Cyc_List_List* _temp896; struct Cyc_Tcpat_Con_s*
_temp898; _LL854: _LL865: _temp864= _temp852.f1; if( _temp864 == 0){ goto _LL863;}
else{ goto _LL856;} _LL863: _temp862= _temp852.f2; if( _temp862 == 0){ goto
_LL855;} else{ goto _LL856;} _LL856: _LL869: _temp868= _temp852.f1; if( _temp868
== 0){ goto _LL867;} else{ goto _LL858;} _LL867: _temp866= _temp852.f2; goto
_LL857; _LL858: _LL873: _temp872= _temp852.f1; goto _LL871; _LL871: _temp870=
_temp852.f2; if( _temp870 == 0){ goto _LL859;} else{ goto _LL860;} _LL860:
_LL889: _temp888= _temp852.f1; if( _temp888 == 0){ goto _LL853;} else{ _temp890=*
_temp888; _LL894: _temp893=( struct _tuple11*) _temp890.hd; _temp895=* _temp893;
_LL899: _temp898= _temp895.f1; goto _LL897; _LL897: _temp896= _temp895.f2; goto
_LL892; _LL892: _temp891= _temp890.tl; goto _LL875;} _LL875: _temp874= _temp852.f2;
if( _temp874 == 0){ goto _LL853;} else{ _temp876=* _temp874; _LL880: _temp879=(
struct _tuple10*) _temp876.hd; _temp881=* _temp879; _LL887: _temp886= _temp881.f1;
goto _LL885; _LL885: _temp884= _temp881.f2; goto _LL883; _LL883: _temp882=
_temp881.f3; goto _LL878; _LL878: _temp877= _temp876.tl; goto _LL861;} _LL855:
return dsc; _LL857: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("build_desc: ctxt and work don't match", sizeof( unsigned char), 38u));
_LL859: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("build_desc: ctxt and work don't match",
sizeof( unsigned char), 38u)); _LL861: { struct Cyc_Tcpat_Pos_struct* _temp903=({
struct Cyc_Tcpat_Pos_struct* _temp900=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp900[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp901; _temp901.tag= Cyc_Tcpat_Pos; _temp901.f1= _temp898; _temp901.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp896),({
struct Cyc_List_List* _temp902=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp902->hd=( void*) dsc; _temp902->tl= _temp882;
_temp902;})); _temp901;}); _temp900;}); goto _LL904; _LL904: return Cyc_Tcpat_build_desc(
_temp891,( void*) _temp903, _temp877);} _LL853:;} static void* Cyc_Tcpat_match(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules); static
void* Cyc_Tcpat_or_match( void* dsc, struct Cyc_List_List* allmrules){ struct
Cyc_List_List* _temp905= allmrules; struct Cyc_List_List _temp911; struct Cyc_List_List*
_temp912; struct _tuple0* _temp914; struct _tuple0 _temp916; void* _temp917;
void* _temp919; _LL907: if( _temp905 == 0){ goto _LL908;} else{ goto _LL909;}
_LL909: if( _temp905 == 0){ goto _LL906;} else{ _temp911=* _temp905; _LL915:
_temp914=( struct _tuple0*) _temp911.hd; _temp916=* _temp914; _LL920: _temp919=
_temp916.f1; goto _LL918; _LL918: _temp917= _temp916.f2; goto _LL913; _LL913:
_temp912= _temp911.tl; goto _LL910;} _LL908: return( void*) Cyc_Tcpat_Failure;
_LL910: return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp919, 0, dsc, 0, 0, _temp917,
_temp912); _LL906:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List*
allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp921=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp921[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp922; _temp922.tag= Cyc_Tcpat_Neg; _temp922.f1= Cyc_Tcpat_empty_con_set();
_temp922;}); _temp921;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp923= work; struct Cyc_List_List
_temp931; struct Cyc_List_List* _temp932; struct _tuple10* _temp934; struct
_tuple10 _temp936; struct Cyc_List_List* _temp937; struct Cyc_List_List*
_temp939; struct Cyc_List_List* _temp941; struct Cyc_List_List _temp943; struct
Cyc_List_List* _temp944; struct _tuple10* _temp946; struct _tuple10 _temp948;
struct Cyc_List_List* _temp949; struct Cyc_List_List* _temp951; struct Cyc_List_List*
_temp953; _LL925: if( _temp923 == 0){ goto _LL926;} else{ goto _LL927;} _LL927:
if( _temp923 == 0){ goto _LL929;} else{ _temp931=* _temp923; _LL935: _temp934=(
struct _tuple10*) _temp931.hd; _temp936=* _temp934; _LL942: _temp941= _temp936.f1;
if( _temp941 == 0){ goto _LL940;} else{ goto _LL929;} _LL940: _temp939= _temp936.f2;
if( _temp939 == 0){ goto _LL938;} else{ goto _LL929;} _LL938: _temp937= _temp936.f3;
if( _temp937 == 0){ goto _LL933;} else{ goto _LL929;} _LL933: _temp932= _temp931.tl;
goto _LL928;} _LL929: if( _temp923 == 0){ goto _LL924;} else{ _temp943=*
_temp923; _LL947: _temp946=( struct _tuple10*) _temp943.hd; _temp948=* _temp946;
_LL954: _temp953= _temp948.f1; goto _LL952; _LL952: _temp951= _temp948.f2; goto
_LL950; _LL950: _temp949= _temp948.f3; goto _LL945; _LL945: _temp944= _temp943.tl;
goto _LL930;} _LL926: return( void*)({ struct Cyc_Tcpat_Success_struct* _temp955=(
struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp955[ 0]=({ struct Cyc_Tcpat_Success_struct _temp956; _temp956.tag= Cyc_Tcpat_Success;
_temp956.f1=( void*) right_hand_side; _temp956;}); _temp955;}); _LL928: return((
void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp932, right_hand_side, rules); _LL930: if(( _temp953 == 0? 1: _temp951
== 0)? 1: _temp949 == 0){ return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("tcpat:and_match: malformed work frame", sizeof( unsigned char), 38u));}{
struct Cyc_List_List _temp959; struct Cyc_List_List* _temp960; void* _temp962;
struct Cyc_List_List* _temp957=( struct Cyc_List_List*) _check_null( _temp953);
_temp959=* _temp957; _LL963: _temp962=( void*) _temp959.hd; goto _LL961; _LL961:
_temp960= _temp959.tl; goto _LL958; _LL958: { struct Cyc_List_List _temp966;
struct Cyc_List_List* _temp967; struct Cyc_List_List* _temp969; struct Cyc_List_List*
_temp964=( struct Cyc_List_List*) _check_null( _temp951); _temp966=* _temp964;
_LL970: _temp969=( struct Cyc_List_List*) _temp966.hd; goto _LL968; _LL968:
_temp967= _temp966.tl; goto _LL965; _LL965: { struct Cyc_List_List _temp973;
struct Cyc_List_List* _temp974; void* _temp976; struct Cyc_List_List* _temp971=(
struct Cyc_List_List*) _check_null( _temp949); _temp973=* _temp971; _LL977:
_temp976=( void*) _temp973.hd; goto _LL975; _LL975: _temp974= _temp973.tl; goto
_LL972; _LL972: { struct _tuple10* _temp979=({ struct _tuple10* _temp978=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp978->f1= _temp960;
_temp978->f2= _temp967; _temp978->f3= _temp974; _temp978;}); goto _LL980; _LL980:
return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_match)( _temp962, _temp969, _temp976, ctx,({ struct Cyc_List_List*
_temp981=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp981->hd=( void*) _temp979; _temp981->tl= _temp944; _temp981;}),
right_hand_side, rules);}}}} _LL924:;} static struct Cyc_List_List* Cyc_Tcpat_getdargs(
struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp982= dsc; struct Cyc_Set_Set*
_temp988; struct Cyc_List_List* _temp990; struct Cyc_Tcpat_Con_s* _temp992;
_LL984: if(*(( int*) _temp982) == Cyc_Tcpat_Neg){ _LL989: _temp988=(( struct Cyc_Tcpat_Neg_struct*)
_temp982)->f1; goto _LL985;} else{ goto _LL986;} _LL986: if(*(( int*) _temp982)
== Cyc_Tcpat_Pos){ _LL993: _temp992=(( struct Cyc_Tcpat_Pos_struct*) _temp982)->f1;
goto _LL991; _LL991: _temp990=(( struct Cyc_Tcpat_Pos_struct*) _temp982)->f2;
goto _LL987;} else{ goto _LL983;} _LL985: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp997=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp997[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp998; _temp998.tag= Cyc_Tcpat_Neg;
_temp998.f1= Cyc_Tcpat_empty_con_set(); _temp998;}); _temp997;}); struct Cyc_List_List*
_temp994= 0; goto _LL995; _LL995:{ int i= 0; for( 0; i < pcon->arity; ++ i){
_temp994=({ struct Cyc_List_List* _temp996=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp996->hd=( void*) any; _temp996->tl=
_temp994; _temp996;});}} return _temp994;} _LL987: return _temp990; _LL983:;}
static struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp999=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp999->hd=( void*)( i + 1); _temp999->tl= obj;
_temp999;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1000= p; struct Cyc_List_List* _temp1006; struct Cyc_Tcpat_Con_s*
_temp1008; _LL1002: if( _temp1000 ==( void*) Cyc_Tcpat_Any){ goto _LL1003;}
else{ goto _LL1004;} _LL1004: if(( unsigned int) _temp1000 > 1u?*(( int*)
_temp1000) == Cyc_Tcpat_Con: 0){ _LL1009: _temp1008=(( struct Cyc_Tcpat_Con_struct*)
_temp1000)->f1; goto _LL1007; _LL1007: _temp1006=(( struct Cyc_Tcpat_Con_struct*)
_temp1000)->f2; goto _LL1005;} else{ goto _LL1001;} _LL1003: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL1005: { void* _temp1010= Cyc_Tcpat_static_match(
_temp1008, dsc); _LL1012: if( _temp1010 ==( void*) Cyc_Tcpat_Yes){ goto _LL1013;}
else{ goto _LL1014;} _LL1014: if( _temp1010 ==( void*) Cyc_Tcpat_No){ goto
_LL1015;} else{ goto _LL1016;} _LL1016: if( _temp1010 ==( void*) Cyc_Tcpat_Maybe){
goto _LL1017;} else{ goto _LL1011;} _LL1013: { struct Cyc_List_List* _temp1020=({
struct Cyc_List_List* _temp1018=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1018->hd=( void*)({ struct _tuple11* _temp1019=(
struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1019->f1= _temp1008;
_temp1019->f2= 0; _temp1019;}); _temp1018->tl= ctx; _temp1018;}); goto _LL1021;
_LL1021: { struct _tuple10* _temp1023=({ struct _tuple10* _temp1022=( struct
_tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1022->f1= _temp1006;
_temp1022->f2= Cyc_Tcpat_getoargs( _temp1008, obj); _temp1022->f3= Cyc_Tcpat_getdargs(
_temp1008, dsc); _temp1022;}); goto _LL1024; _LL1024: { struct Cyc_List_List*
_temp1026=({ struct Cyc_List_List* _temp1025=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1025->hd=( void*) _temp1023; _temp1025->tl=
work; _temp1025;}); goto _LL1027; _LL1027: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( _temp1020, _temp1026, right_hand_side, rules);}}}
_LL1015: return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)(
Cyc_Tcpat_build_desc( ctx, dsc, work), rules); _LL1017: { struct Cyc_List_List*
_temp1030=({ struct Cyc_List_List* _temp1028=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1028->hd=( void*)({ struct _tuple11*
_temp1029=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1029->f1=
_temp1008; _temp1029->f2= 0; _temp1029;}); _temp1028->tl= ctx; _temp1028;});
goto _LL1031; _LL1031: { struct _tuple10* _temp1033=({ struct _tuple10*
_temp1032=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1032->f1=
_temp1006; _temp1032->f2= Cyc_Tcpat_getoargs( _temp1008, obj); _temp1032->f3=
Cyc_Tcpat_getdargs( _temp1008, dsc); _temp1032;}); goto _LL1034; _LL1034: {
struct Cyc_List_List* _temp1036=({ struct Cyc_List_List* _temp1035=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1035->hd=( void*) _temp1033;
_temp1035->tl= work; _temp1035;}); goto _LL1037; _LL1037: { void* _temp1038=((
void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( _temp1030,
_temp1036, right_hand_side, rules); goto _LL1039; _LL1039: { void* _temp1040=((
void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, Cyc_Tcpat_add_neg( dsc, _temp1008), work), rules); goto _LL1041; _LL1041:
return( void*)({ struct Cyc_Tcpat_IfEq_struct* _temp1042=( struct Cyc_Tcpat_IfEq_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct)); _temp1042[ 0]=({ struct Cyc_Tcpat_IfEq_struct
_temp1043; _temp1043.tag= Cyc_Tcpat_IfEq; _temp1043.f1= obj; _temp1043.f2=
_temp1008; _temp1043.f3=( void*) _temp1038; _temp1043.f4=( void*) _temp1040;
_temp1043;}); _temp1042;});}}}}} _LL1011:;} _LL1001:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp1044= d; void* _temp1052; void* _temp1054; void*
_temp1056; struct Cyc_Tcpat_Con_s* _temp1058; struct Cyc_List_List* _temp1060;
_LL1046: if( _temp1044 ==( void*) Cyc_Tcpat_Failure){ goto _LL1047;} else{ goto
_LL1048;} _LL1048: if(( unsigned int) _temp1044 > 1u?*(( int*) _temp1044) == Cyc_Tcpat_Success:
0){ _LL1053: _temp1052=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1044)->f1;
goto _LL1049;} else{ goto _LL1050;} _LL1050: if(( unsigned int) _temp1044 > 1u?*((
int*) _temp1044) == Cyc_Tcpat_IfEq: 0){ _LL1061: _temp1060=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1044)->f1; goto _LL1059; _LL1059: _temp1058=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1044)->f2; goto _LL1057; _LL1057: _temp1056=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1044)->f3; goto _LL1055; _LL1055: _temp1054=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1044)->f4; goto _LL1051;} else{ goto _LL1045;} _LL1047: not_exhaust( env1);
goto _LL1045; _LL1049: rhs_appears( env2, _temp1052); goto _LL1045; _LL1051:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1056,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1054, not_exhaust, env1, rhs_appears, env2); goto _LL1045; _LL1045:;}
struct _tuple12{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple13{
void* f1; struct _tuple12* f2; } ; static struct _tuple13* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple12* rhs=({ struct _tuple12* _temp1067=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1067->f1= 0; _temp1067->f2=(
swc->pattern)->loc; _temp1067;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1062=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1062->hd=( void*) sp0; _temp1062->tl=({ struct Cyc_List_List*
_temp1063=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1063->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1063->tl= 0; _temp1063;});
_temp1062;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1064=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1064->hd=( void*) sp0; _temp1064->tl=({ struct Cyc_List_List* _temp1065=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1065->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1065->tl= 0; _temp1065;}); _temp1064;}));}
return({ struct _tuple13* _temp1066=( struct _tuple13*) GC_malloc( sizeof(
struct _tuple13)); _temp1066->f1= sp; _temp1066->f2= rhs; _temp1066;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr(
loc, _tag_arr("patterns may not be exhaustive", sizeof( unsigned char), 31u));}
static void Cyc_Tcpat_rule_occurs( int dummy, struct _tuple12* rhs){(* rhs).f1=
1;} void Cyc_Tcpat_check_switch_exhaustive( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* swcs){ int _temp1068= 0; goto _LL1069; _LL1069: { struct
Cyc_List_List* _temp1070=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( int*,
struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcpat_get_match,& _temp1068, swcs); goto _LL1071; _LL1071: { void* _temp1072=((
void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( _temp1070);
goto _LL1073; _LL1073:(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple12*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1072, Cyc_Tcpat_not_exhaust_err,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; _temp1070 != 0; _temp1070=(( struct Cyc_List_List*)
_check_null( _temp1070))->tl){ struct _tuple13 _temp1076; struct _tuple12*
_temp1077; struct _tuple12 _temp1079; struct Cyc_Position_Segment* _temp1080;
int _temp1082; void* _temp1084; struct _tuple13* _temp1074=( struct _tuple13*)((
struct Cyc_List_List*) _check_null( _temp1070))->hd; _temp1076=* _temp1074;
_LL1085: _temp1084= _temp1076.f1; goto _LL1078; _LL1078: _temp1077= _temp1076.f2;
_temp1079=* _temp1077; _LL1083: _temp1082= _temp1079.f1; goto _LL1081; _LL1081:
_temp1080= _temp1079.f2; goto _LL1075; _LL1075: if( ! _temp1082){ Cyc_Tcutil_terr(
_temp1080, _tag_arr("redundant pattern", sizeof( unsigned char), 18u));}}}}}
struct _tuple14{ struct Cyc_Position_Segment* f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn(
struct _tuple14* pr){(* pr).f2= 0; Cyc_Tcutil_warn((* pr).f1, _tag_arr("pattern not exhaustive",
sizeof( unsigned char), 23u));} static void Cyc_Tcpat_dummy_fn( int i, int j){
return;} struct _tuple15{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
_temp1088=({ struct Cyc_List_List* _temp1086=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1086->hd=( void*)({ struct _tuple15*
_temp1087=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1087->f1=
Cyc_Tcpat_compile_pat( p); _temp1087->f2= 0; _temp1087;}); _temp1086->tl= 0;
_temp1086;}); goto _LL1089; _LL1089: { void* _temp1090=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( _temp1088); goto _LL1091; _LL1091: {
struct _tuple14* _temp1093=({ struct _tuple14* _temp1092=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1092->f1= loc; _temp1092->f2= 1;
_temp1092;}); goto _LL1094; _LL1094:(( void(*)( void* d, void(* not_exhaust)(
struct _tuple14*), struct _tuple14* env1, void(* rhs_appears)( int, int), int
env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1090, Cyc_Tcpat_not_exhaust_warn,
_temp1093, Cyc_Tcpat_dummy_fn, 0); return(* _temp1093).f2;}}} static struct
_tuple13* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple12* rhs=({ struct _tuple12*
_temp1096=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1096->f1=
0; _temp1096->f2=( swc->pattern)->loc; _temp1096;}); return({ struct _tuple13*
_temp1095=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp1095->f1=
sp0; _temp1095->f2= rhs; _temp1095;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp1097=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); goto
_LL1098; _LL1098: { void* _temp1099=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1097); goto _LL1100; _LL1100:(( void(*)( void* d,
void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1099, Cyc_Tcpat_not_exhaust_err2, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1097 != 0; _temp1097=(( struct Cyc_List_List*) _check_null( _temp1097))->tl){
struct _tuple13 _temp1103; struct _tuple12* _temp1104; struct _tuple12 _temp1106;
struct Cyc_Position_Segment* _temp1107; int _temp1109; void* _temp1111; struct
_tuple13* _temp1101=( struct _tuple13*)(( struct Cyc_List_List*) _check_null(
_temp1097))->hd; _temp1103=* _temp1101; _LL1112: _temp1111= _temp1103.f1; goto
_LL1105; _LL1105: _temp1104= _temp1103.f2; _temp1106=* _temp1104; _LL1110:
_temp1109= _temp1106.f1; goto _LL1108; _LL1108: _temp1107= _temp1106.f2; goto
_LL1102; _LL1102: if( ! _temp1109){ Cyc_Tcutil_terr( _temp1107, _tag_arr("redundant pattern",
sizeof( unsigned char), 18u));}}}}