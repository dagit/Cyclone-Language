 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
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
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); struct Cyc_Stdio___sFILE;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct
Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char
Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
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
struct _tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern int
Cyc_Evexp_okay_szofarg( void* t); void Cyc_Evexp_exp_err( struct Cyc_Position_Segment*
loc, struct _tagged_arr msg){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp0= msg; xprintf("Exp_err: %.*s",
_get_arr_size( _temp0, 1u), _temp0.curr);})));} void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e); void* Cyc_Evexp_promote_const( void* cn){ void* _temp1=
cn; unsigned char _temp9; void* _temp11; short _temp13; void* _temp15; _LL3: if((
unsigned int) _temp1 > 1u?*(( int*) _temp1) == Cyc_Absyn_Char_c: 0){ _LL12:
_temp11=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1)->f1; goto _LL10;
_LL10: _temp9=(( struct Cyc_Absyn_Char_c_struct*) _temp1)->f2; goto _LL4;} else{
goto _LL5;} _LL5: if(( unsigned int) _temp1 > 1u?*(( int*) _temp1) == Cyc_Absyn_Short_c:
0){ _LL16: _temp15=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1)->f1;
goto _LL14; _LL14: _temp13=(( struct Cyc_Absyn_Short_c_struct*) _temp1)->f2;
goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL4: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp17=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp17[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp18; _temp18.tag= Cyc_Absyn_Int_c;
_temp18.f1=( void*) _temp11; _temp18.f2=( int) _temp9; _temp18;}); _temp17;});
_LL6: return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp19=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp19[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp20; _temp20.tag= Cyc_Absyn_Int_c; _temp20.f1=( void*) _temp15; _temp20.f2=(
int) _temp13; _temp20;}); _temp19;}); _LL8: return cn; _LL2:;} unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e){ void* _temp21= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp27; _LL23: if(( unsigned int) _temp21 > 1u?*(( int*) _temp21) ==
Cyc_Absyn_Int_c: 0){ _LL28: _temp27=(( struct Cyc_Absyn_Int_c_struct*) _temp21)->f2;
goto _LL24;} else{ goto _LL25;} _LL25: goto _LL26; _LL24: return( unsigned int)
_temp27; _LL26: Cyc_Evexp_exp_err( e->loc, _tag_arr("expecting unsigned int",
sizeof( unsigned char), 23u)); return 0u; _LL22:;} int Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp* e){ void* _temp29= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp39; long long _temp41; _LL31: if(( unsigned int) _temp29 > 1u?*((
int*) _temp29) == Cyc_Absyn_Int_c: 0){ _LL40: _temp39=(( struct Cyc_Absyn_Int_c_struct*)
_temp29)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp29
> 1u?*(( int*) _temp29) == Cyc_Absyn_LongLong_c: 0){ _LL42: _temp41=(( struct
Cyc_Absyn_LongLong_c_struct*) _temp29)->f2; goto _LL34;} else{ goto _LL35;}
_LL35: if( _temp29 ==( void*) Cyc_Absyn_Null_c){ goto _LL36;} else{ goto _LL37;}
_LL37: goto _LL38; _LL32: return _temp39 != 0; _LL34: return _temp41 != 0; _LL36:
return 0; _LL38: Cyc_Evexp_exp_err( e->loc, _tag_arr("expecting bool", sizeof(
unsigned char), 15u)); return 0; _LL30:;} struct _tuple3{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; int Cyc_Evexp_okay_szofarg( void* t){ void* _temp43= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp93; struct Cyc_Core_Opt* _temp95; struct Cyc_Core_Opt
_temp97; void* _temp98; struct Cyc_Absyn_TunionFieldInfo _temp100; void*
_temp102; struct Cyc_Absyn_Tunionfield* _temp104; struct Cyc_List_List* _temp106;
struct Cyc_Absyn_PtrInfo _temp108; struct Cyc_Absyn_Conref* _temp110; struct Cyc_Absyn_Exp*
_temp112; void* _temp114; struct Cyc_Absyn_Structdecl** _temp116; struct Cyc_List_List*
_temp118; struct Cyc_Absyn_Uniondecl** _temp120; struct Cyc_List_List* _temp122;
struct Cyc_List_List* _temp124; _LL45: if( _temp43 ==( void*) Cyc_Absyn_VoidType){
goto _LL46;} else{ goto _LL47;} _LL47: if(( unsigned int) _temp43 > 4u?*(( int*)
_temp43) == Cyc_Absyn_VarType: 0){ _LL94: _temp93=(( struct Cyc_Absyn_VarType_struct*)
_temp43)->f1; goto _LL48;} else{ goto _LL49;} _LL49: if(( unsigned int) _temp43
> 4u?*(( int*) _temp43) == Cyc_Absyn_Evar: 0){ _LL96: _temp95=(( struct Cyc_Absyn_Evar_struct*)
_temp43)->f1; if( _temp95 == 0){ goto _LL51;} else{ _temp97=* _temp95; _LL99:
_temp98=( void*) _temp97.v; if( _temp98 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL50;} else{ goto _LL51;}}} else{ goto _LL51;} _LL51: if(( unsigned int)
_temp43 > 4u?*(( int*) _temp43) == Cyc_Absyn_Evar: 0){ goto _LL52;} else{ goto
_LL53;} _LL53: if(( unsigned int) _temp43 > 4u?*(( int*) _temp43) == Cyc_Absyn_TunionType:
0){ goto _LL54;} else{ goto _LL55;} _LL55: if(( unsigned int) _temp43 > 4u?*((
int*) _temp43) == Cyc_Absyn_TunionFieldType: 0){ _LL101: _temp100=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp43)->f1; _LL103: _temp102=( void*) _temp100.field_info; if(*(( int*)
_temp102) == Cyc_Absyn_KnownTunionfield){ _LL105: _temp104=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp102)->f2; goto _LL56;} else{ goto _LL57;}} else{ goto _LL57;} _LL57: if((
unsigned int) _temp43 > 4u?*(( int*) _temp43) == Cyc_Absyn_TupleType: 0){ _LL107:
_temp106=(( struct Cyc_Absyn_TupleType_struct*) _temp43)->f1; goto _LL58;} else{
goto _LL59;} _LL59: if(( unsigned int) _temp43 > 4u?*(( int*) _temp43) == Cyc_Absyn_TunionFieldType:
0){ goto _LL60;} else{ goto _LL61;} _LL61: if(( unsigned int) _temp43 > 4u?*((
int*) _temp43) == Cyc_Absyn_PointerType: 0){ _LL109: _temp108=(( struct Cyc_Absyn_PointerType_struct*)
_temp43)->f1; _LL111: _temp110= _temp108.bounds; goto _LL62;} else{ goto _LL63;}
_LL63: if(( unsigned int) _temp43 > 4u?*(( int*) _temp43) == Cyc_Absyn_IntType:
0){ goto _LL64;} else{ goto _LL65;} _LL65: if( _temp43 ==( void*) Cyc_Absyn_FloatType){
goto _LL66;} else{ goto _LL67;} _LL67: if( _temp43 ==( void*) Cyc_Absyn_DoubleType){
goto _LL68;} else{ goto _LL69;} _LL69: if(( unsigned int) _temp43 > 4u?*(( int*)
_temp43) == Cyc_Absyn_ArrayType: 0){ _LL115: _temp114=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp43)->f1; goto _LL113; _LL113: _temp112=(( struct Cyc_Absyn_ArrayType_struct*)
_temp43)->f3; goto _LL70;} else{ goto _LL71;} _LL71: if(( unsigned int) _temp43
> 4u?*(( int*) _temp43) == Cyc_Absyn_FnType: 0){ goto _LL72;} else{ goto _LL73;}
_LL73: if(( unsigned int) _temp43 > 4u?*(( int*) _temp43) == Cyc_Absyn_RgnHandleType:
0){ goto _LL74;} else{ goto _LL75;} _LL75: if(( unsigned int) _temp43 > 4u?*((
int*) _temp43) == Cyc_Absyn_StructType: 0){ _LL117: _temp116=(( struct Cyc_Absyn_StructType_struct*)
_temp43)->f3; goto _LL76;} else{ goto _LL77;} _LL77: if(( unsigned int) _temp43
> 4u?*(( int*) _temp43) == Cyc_Absyn_AnonStructType: 0){ _LL119: _temp118=((
struct Cyc_Absyn_AnonStructType_struct*) _temp43)->f1; goto _LL78;} else{ goto
_LL79;} _LL79: if(( unsigned int) _temp43 > 4u?*(( int*) _temp43) == Cyc_Absyn_UnionType:
0){ _LL123: _temp122=(( struct Cyc_Absyn_UnionType_struct*) _temp43)->f2; goto
_LL121; _LL121: _temp120=(( struct Cyc_Absyn_UnionType_struct*) _temp43)->f3;
goto _LL80;} else{ goto _LL81;} _LL81: if(( unsigned int) _temp43 > 4u?*(( int*)
_temp43) == Cyc_Absyn_AnonUnionType: 0){ _LL125: _temp124=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp43)->f1; goto _LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp43
> 4u?*(( int*) _temp43) == Cyc_Absyn_EnumType: 0){ goto _LL84;} else{ goto _LL85;}
_LL85: if( _temp43 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL86;} else{ goto _LL87;}
_LL87: if(( unsigned int) _temp43 > 4u?*(( int*) _temp43) == Cyc_Absyn_JoinEff:
0){ goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp43 > 4u?*((
int*) _temp43) == Cyc_Absyn_AccessEff: 0){ goto _LL90;} else{ goto _LL91;} _LL91:
if(( unsigned int) _temp43 > 4u?*(( int*) _temp43) == Cyc_Absyn_TypedefType: 0){
goto _LL92;} else{ goto _LL44;} _LL46: return 0; _LL48: { void* _temp126= Cyc_Absyn_conref_val(
_temp93->kind); _LL128: if( _temp126 ==( void*) Cyc_Absyn_BoxKind){ goto _LL129;}
else{ goto _LL130;} _LL130: goto _LL131; _LL129: return 1; _LL131: return 0;
_LL127:;} _LL50: return 1; _LL52: return 0; _LL54: return 1; _LL56: _temp106=
_temp104->typs; goto _LL58; _LL58: for( 0; _temp106 != 0; _temp106=(( struct Cyc_List_List*)
_check_null( _temp106))->tl){ if( ! Cyc_Evexp_okay_szofarg((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp106))->hd)).f2)){ return 0;}} return 1;
_LL60: return 0; _LL62: { void* _temp132=( void*)( Cyc_Absyn_compress_conref(
_temp110))->v; void* _temp140; _LL134: if(( unsigned int) _temp132 > 1u?*(( int*)
_temp132) == Cyc_Absyn_Eq_constr: 0){ _LL141: _temp140=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp132)->f1; if( _temp140 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL135;} else{
goto _LL136;}} else{ goto _LL136;} _LL136: if(( unsigned int) _temp132 > 1u?*((
int*) _temp132) == Cyc_Absyn_Eq_constr: 0){ goto _LL137;} else{ goto _LL138;}
_LL138: goto _LL139; _LL135: return 1; _LL137: return 1; _LL139: return 0;
_LL133:;} _LL64: return 1; _LL66: return 1; _LL68: return 1; _LL70: return
_temp112 != 0; _LL72: return 0; _LL74: return 1; _LL76: if( _temp116 == 0){
return(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("szof on unchecked StructType",
sizeof( unsigned char), 29u));}{ struct Cyc_Absyn_Structdecl* _temp142=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp116)); if( _temp142->fields ==
0){ return 0;} _temp118=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp142->fields))->v; goto _LL78;} _LL78: for( 0; _temp118 != 0;
_temp118=(( struct Cyc_List_List*) _check_null( _temp118))->tl){ if( ! Cyc_Evexp_okay_szofarg((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp118))->hd)->type)){ return 0;}} return 1; _LL80: if( _temp120 == 0){ return((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("szof on unchecked UnionType",
sizeof( unsigned char), 28u));}{ struct Cyc_Absyn_Uniondecl* _temp143=*(( struct
Cyc_Absyn_Uniondecl**) _check_null( _temp120)); if( _temp143->fields == 0){
return 0;} _temp124=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp143->fields))->v; goto _LL82;} _LL82: for( 0; _temp124 != 0; _temp124=((
struct Cyc_List_List*) _check_null( _temp124))->tl){ if( ! Cyc_Evexp_okay_szofarg((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp124))->hd)->type)){ return 0;}} return 1; _LL84: return 1; _LL86: return 0;
_LL88: return 0; _LL90: return 0; _LL92: return(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)(( struct _tagged_arr)({ struct _tagged_arr _temp144= Cyc_Absynpp_typ2string(
t); xprintf("szof typedeftype %.*s", _get_arr_size( _temp144, 1u), _temp144.curr);}));
_LL44:;} struct _tuple4{ void* f1; void* f2; } ; void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple4 _temp146=({ struct _tuple4 _temp145; _temp145.f1= p;
_temp145.f2= cn; _temp145;}); void* _temp160; void* _temp162; int _temp164; void*
_temp166; void* _temp168; void* _temp170; int _temp172; void* _temp174; void*
_temp176; void* _temp178; int _temp180; void* _temp182; void* _temp184; void*
_temp186; _LL148: _LL161: _temp160= _temp146.f1; if( _temp160 ==( void*) Cyc_Absyn_Plus){
goto _LL149;} else{ goto _LL150;} _LL150: _LL169: _temp168= _temp146.f1; if(
_temp168 ==( void*) Cyc_Absyn_Minus){ goto _LL163;} else{ goto _LL152;} _LL163:
_temp162= _temp146.f2; if(( unsigned int) _temp162 > 1u?*(( int*) _temp162) ==
Cyc_Absyn_Int_c: 0){ _LL167: _temp166=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp162)->f1; goto _LL165; _LL165: _temp164=(( struct Cyc_Absyn_Int_c_struct*)
_temp162)->f2; goto _LL151;} else{ goto _LL152;} _LL152: _LL177: _temp176=
_temp146.f1; if( _temp176 ==( void*) Cyc_Absyn_Bitnot){ goto _LL171;} else{ goto
_LL154;} _LL171: _temp170= _temp146.f2; if(( unsigned int) _temp170 > 1u?*(( int*)
_temp170) == Cyc_Absyn_Int_c: 0){ _LL175: _temp174=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp170)->f1; goto _LL173; _LL173: _temp172=(( struct Cyc_Absyn_Int_c_struct*)
_temp170)->f2; goto _LL153;} else{ goto _LL154;} _LL154: _LL183: _temp182=
_temp146.f1; if( _temp182 ==( void*) Cyc_Absyn_Not){ goto _LL179;} else{ goto
_LL156;} _LL179: _temp178= _temp146.f2; if(( unsigned int) _temp178 > 1u?*(( int*)
_temp178) == Cyc_Absyn_Int_c: 0){ _LL181: _temp180=(( struct Cyc_Absyn_Int_c_struct*)
_temp178)->f2; goto _LL155;} else{ goto _LL156;} _LL156: _LL187: _temp186=
_temp146.f1; if( _temp186 ==( void*) Cyc_Absyn_Not){ goto _LL185;} else{ goto
_LL158;} _LL185: _temp184= _temp146.f2; if( _temp184 ==( void*) Cyc_Absyn_Null_c){
goto _LL157;} else{ goto _LL158;} _LL158: goto _LL159; _LL149: return cn; _LL151:
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp188=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp188[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp189; _temp189.tag= Cyc_Absyn_Int_c; _temp189.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp189.f2= - _temp164; _temp189;}); _temp188;}); _LL153: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp190=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp190[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp191; _temp191.tag= Cyc_Absyn_Int_c; _temp191.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp191.f2= ~ _temp172; _temp191;}); _temp190;}); _LL155: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp192=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp192[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp193; _temp193.tag= Cyc_Absyn_Int_c; _temp193.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp193.f2= _temp180 == 0? 1: 0; _temp193;}); _temp192;}); _LL157: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp194=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp194[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp195; _temp195.tag= Cyc_Absyn_Int_c; _temp195.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp195.f2= 1; _temp195;}); _temp194;}); _LL159: Cyc_Evexp_exp_err( e->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u)); return cn;
_LL147:;} struct _tuple5{ void* f1; int f2; } ; void* Cyc_Evexp_eval_const_binprimop(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* _temp196=
Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e1)); void* _temp197= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e2)); void* s1; void* s2; int i1; int i2;{ void*
_temp198= _temp196; int _temp204; void* _temp206; _LL200: if(( unsigned int)
_temp198 > 1u?*(( int*) _temp198) == Cyc_Absyn_Int_c: 0){ _LL207: _temp206=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp198)->f1; goto _LL205; _LL205:
_temp204=(( struct Cyc_Absyn_Int_c_struct*) _temp198)->f2; goto _LL201;} else{
goto _LL202;} _LL202: goto _LL203; _LL201: s1= _temp206; i1= _temp204; goto
_LL199; _LL203: Cyc_Evexp_exp_err( e1->loc, _tag_arr("bad constant expression",
sizeof( unsigned char), 24u)); return _temp196; _LL199:;}{ void* _temp208=
_temp197; int _temp214; void* _temp216; _LL210: if(( unsigned int) _temp208 > 1u?*((
int*) _temp208) == Cyc_Absyn_Int_c: 0){ _LL217: _temp216=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp208)->f1; goto _LL215; _LL215: _temp214=(( struct Cyc_Absyn_Int_c_struct*)
_temp208)->f2; goto _LL211;} else{ goto _LL212;} _LL212: goto _LL213; _LL211: s2=
_temp216; i2= _temp214; goto _LL209; _LL213: Cyc_Evexp_exp_err( e2->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u)); return
_temp196; _LL209:;}{ void* _temp218= p; _LL220: if( _temp218 ==( void*) Cyc_Absyn_Div){
goto _LL221;} else{ goto _LL222;} _LL222: if( _temp218 ==( void*) Cyc_Absyn_Mod){
goto _LL223;} else{ goto _LL224;} _LL224: goto _LL225; _LL221: goto _LL223;
_LL223: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc, _tag_arr("division by zero in constant expression",
sizeof( unsigned char), 40u)); return _temp196;} goto _LL219; _LL225: goto
_LL219; _LL219:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned? 1: s2 ==(
void*) Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1; unsigned int u2=(
unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1; int use_i3= 0; int
use_u3= 0; int use_b3= 0;{ struct _tuple5 _temp227=({ struct _tuple5 _temp226;
_temp226.f1= p; _temp226.f2= has_u_arg; _temp226;}); int _temp283; void*
_temp285; int _temp287; void* _temp289; int _temp291; void* _temp293; int
_temp295; void* _temp297; int _temp299; void* _temp301; int _temp303; void*
_temp305; int _temp307; void* _temp309; int _temp311; void* _temp313; int
_temp315; void* _temp317; int _temp319; void* _temp321; void* _temp323; void*
_temp325; int _temp327; void* _temp329; int _temp331; void* _temp333; int
_temp335; void* _temp337; int _temp339; void* _temp341; int _temp343; void*
_temp345; int _temp347; void* _temp349; int _temp351; void* _temp353; int
_temp355; void* _temp357; void* _temp359; void* _temp361; void* _temp363; void*
_temp365; void* _temp367; void* _temp369; _LL229: _LL286: _temp285= _temp227.f1;
if( _temp285 ==( void*) Cyc_Absyn_Plus){ goto _LL284;} else{ goto _LL231;}
_LL284: _temp283= _temp227.f2; if( _temp283 == 0){ goto _LL230;} else{ goto
_LL231;} _LL231: _LL290: _temp289= _temp227.f1; if( _temp289 ==( void*) Cyc_Absyn_Times){
goto _LL288;} else{ goto _LL233;} _LL288: _temp287= _temp227.f2; if( _temp287 ==
0){ goto _LL232;} else{ goto _LL233;} _LL233: _LL294: _temp293= _temp227.f1; if(
_temp293 ==( void*) Cyc_Absyn_Minus){ goto _LL292;} else{ goto _LL235;} _LL292:
_temp291= _temp227.f2; if( _temp291 == 0){ goto _LL234;} else{ goto _LL235;}
_LL235: _LL298: _temp297= _temp227.f1; if( _temp297 ==( void*) Cyc_Absyn_Div){
goto _LL296;} else{ goto _LL237;} _LL296: _temp295= _temp227.f2; if( _temp295 ==
0){ goto _LL236;} else{ goto _LL237;} _LL237: _LL302: _temp301= _temp227.f1; if(
_temp301 ==( void*) Cyc_Absyn_Mod){ goto _LL300;} else{ goto _LL239;} _LL300:
_temp299= _temp227.f2; if( _temp299 == 0){ goto _LL238;} else{ goto _LL239;}
_LL239: _LL306: _temp305= _temp227.f1; if( _temp305 ==( void*) Cyc_Absyn_Plus){
goto _LL304;} else{ goto _LL241;} _LL304: _temp303= _temp227.f2; if( _temp303 ==
1){ goto _LL240;} else{ goto _LL241;} _LL241: _LL310: _temp309= _temp227.f1; if(
_temp309 ==( void*) Cyc_Absyn_Times){ goto _LL308;} else{ goto _LL243;} _LL308:
_temp307= _temp227.f2; if( _temp307 == 1){ goto _LL242;} else{ goto _LL243;}
_LL243: _LL314: _temp313= _temp227.f1; if( _temp313 ==( void*) Cyc_Absyn_Minus){
goto _LL312;} else{ goto _LL245;} _LL312: _temp311= _temp227.f2; if( _temp311 ==
1){ goto _LL244;} else{ goto _LL245;} _LL245: _LL318: _temp317= _temp227.f1; if(
_temp317 ==( void*) Cyc_Absyn_Div){ goto _LL316;} else{ goto _LL247;} _LL316:
_temp315= _temp227.f2; if( _temp315 == 1){ goto _LL246;} else{ goto _LL247;}
_LL247: _LL322: _temp321= _temp227.f1; if( _temp321 ==( void*) Cyc_Absyn_Mod){
goto _LL320;} else{ goto _LL249;} _LL320: _temp319= _temp227.f2; if( _temp319 ==
1){ goto _LL248;} else{ goto _LL249;} _LL249: _LL324: _temp323= _temp227.f1; if(
_temp323 ==( void*) Cyc_Absyn_Eq){ goto _LL250;} else{ goto _LL251;} _LL251:
_LL326: _temp325= _temp227.f1; if( _temp325 ==( void*) Cyc_Absyn_Neq){ goto
_LL252;} else{ goto _LL253;} _LL253: _LL330: _temp329= _temp227.f1; if( _temp329
==( void*) Cyc_Absyn_Gt){ goto _LL328;} else{ goto _LL255;} _LL328: _temp327=
_temp227.f2; if( _temp327 == 0){ goto _LL254;} else{ goto _LL255;} _LL255:
_LL334: _temp333= _temp227.f1; if( _temp333 ==( void*) Cyc_Absyn_Lt){ goto
_LL332;} else{ goto _LL257;} _LL332: _temp331= _temp227.f2; if( _temp331 == 0){
goto _LL256;} else{ goto _LL257;} _LL257: _LL338: _temp337= _temp227.f1; if(
_temp337 ==( void*) Cyc_Absyn_Gte){ goto _LL336;} else{ goto _LL259;} _LL336:
_temp335= _temp227.f2; if( _temp335 == 0){ goto _LL258;} else{ goto _LL259;}
_LL259: _LL342: _temp341= _temp227.f1; if( _temp341 ==( void*) Cyc_Absyn_Lte){
goto _LL340;} else{ goto _LL261;} _LL340: _temp339= _temp227.f2; if( _temp339 ==
0){ goto _LL260;} else{ goto _LL261;} _LL261: _LL346: _temp345= _temp227.f1; if(
_temp345 ==( void*) Cyc_Absyn_Gt){ goto _LL344;} else{ goto _LL263;} _LL344:
_temp343= _temp227.f2; if( _temp343 == 1){ goto _LL262;} else{ goto _LL263;}
_LL263: _LL350: _temp349= _temp227.f1; if( _temp349 ==( void*) Cyc_Absyn_Lt){
goto _LL348;} else{ goto _LL265;} _LL348: _temp347= _temp227.f2; if( _temp347 ==
1){ goto _LL264;} else{ goto _LL265;} _LL265: _LL354: _temp353= _temp227.f1; if(
_temp353 ==( void*) Cyc_Absyn_Gte){ goto _LL352;} else{ goto _LL267;} _LL352:
_temp351= _temp227.f2; if( _temp351 == 1){ goto _LL266;} else{ goto _LL267;}
_LL267: _LL358: _temp357= _temp227.f1; if( _temp357 ==( void*) Cyc_Absyn_Lte){
goto _LL356;} else{ goto _LL269;} _LL356: _temp355= _temp227.f2; if( _temp355 ==
1){ goto _LL268;} else{ goto _LL269;} _LL269: _LL360: _temp359= _temp227.f1; if(
_temp359 ==( void*) Cyc_Absyn_Bitand){ goto _LL270;} else{ goto _LL271;} _LL271:
_LL362: _temp361= _temp227.f1; if( _temp361 ==( void*) Cyc_Absyn_Bitor){ goto
_LL272;} else{ goto _LL273;} _LL273: _LL364: _temp363= _temp227.f1; if( _temp363
==( void*) Cyc_Absyn_Bitxor){ goto _LL274;} else{ goto _LL275;} _LL275: _LL366:
_temp365= _temp227.f1; if( _temp365 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL276;}
else{ goto _LL277;} _LL277: _LL368: _temp367= _temp227.f1; if( _temp367 ==( void*)
Cyc_Absyn_Bitlrshift){ goto _LL278;} else{ goto _LL279;} _LL279: _LL370:
_temp369= _temp227.f1; if( _temp369 ==( void*) Cyc_Absyn_Bitarshift){ goto
_LL280;} else{ goto _LL281;} _LL281: goto _LL282; _LL230: i3= i1 + i2; use_i3= 1;
goto _LL228; _LL232: i3= i1 * i2; use_i3= 1; goto _LL228; _LL234: i3= i1 - i2;
use_i3= 1; goto _LL228; _LL236: i3= i1 / i2; use_i3= 1; goto _LL228; _LL238: i3=
i1 % i2; use_i3= 1; goto _LL228; _LL240: u3= u1 + u2; use_u3= 1; goto _LL228;
_LL242: u3= u1 * u2; use_u3= 1; goto _LL228; _LL244: u3= u1 - u2; use_u3= 1;
goto _LL228; _LL246: u3= u1 / u2; use_u3= 1; goto _LL228; _LL248: u3= u1 % u2;
use_u3= 1; goto _LL228; _LL250: b3= i1 == i2; use_b3= 1; goto _LL228; _LL252: b3=
i1 != i2; use_b3= 1; goto _LL228; _LL254: b3= i1 > i2; use_b3= 1; goto _LL228;
_LL256: b3= i1 < i2; use_b3= 1; goto _LL228; _LL258: b3= i1 >= i2; use_b3= 1;
goto _LL228; _LL260: b3= i1 <= i2; use_b3= 1; goto _LL228; _LL262: b3= u1 > u2;
use_b3= 1; goto _LL228; _LL264: b3= u1 < u2; use_b3= 1; goto _LL228; _LL266: b3=
u1 >= u2; use_b3= 1; goto _LL228; _LL268: b3= u1 <= u2; use_b3= 1; goto _LL228;
_LL270: u3= u1 & u2; use_u3= 1; goto _LL228; _LL272: u3= u1 | u2; use_u3= 1;
goto _LL228; _LL274: u3= u1 ^ u2; use_u3= 1; goto _LL228; _LL276: u3= u1 << u2;
use_u3= 1; goto _LL228; _LL278: u3= u1 >> u2; use_u3= 1; goto _LL228; _LL280:
Cyc_Evexp_exp_err( e1->loc, _tag_arr(">>> NOT IMPLEMENTED", sizeof(
unsigned char), 20u)); return _temp196; _LL282: Cyc_Evexp_exp_err( e1->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u)); return
_temp196; _LL228:;} if( use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp371=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp371[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp372; _temp372.tag= Cyc_Absyn_Int_c;
_temp372.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp372.f2= i3; _temp372;});
_temp371;});} if( use_u3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp373=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp373[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp374; _temp374.tag= Cyc_Absyn_Int_c;
_temp374.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp374.f2=( int) u3;
_temp374;}); _temp373;});} if( use_b3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp375=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp375[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp376; _temp376.tag= Cyc_Absyn_Int_c;
_temp376.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp376.f2= b3? 1: 0; _temp376;});
_temp375;});}( int) _throw(( void*)({ struct Cyc_Core_Unreachable_struct*
_temp377=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));
_temp377[ 0]=({ struct Cyc_Core_Unreachable_struct _temp378; _temp378.tag= Cyc_Core_Unreachable;
_temp378.f1= _tag_arr("Evexp::eval_const_binop", sizeof( unsigned char), 24u);
_temp378;}); _temp377;}));}} void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp*
e){ void* cn;{ void* _temp379=( void*) e->r; void* _temp399; struct Cyc_Absyn_Exp*
_temp401; struct Cyc_Absyn_Exp* _temp403; struct Cyc_Absyn_Exp* _temp405; struct
Cyc_List_List* _temp407; void* _temp409; struct Cyc_Absyn_Exp* _temp411; void*
_temp413; struct Cyc_Absyn_Enumfield* _temp415; _LL381: if(*(( int*) _temp379)
== Cyc_Absyn_Const_e){ _LL400: _temp399=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp379)->f1; goto _LL382;} else{ goto _LL383;} _LL383: if(*(( int*) _temp379)
== Cyc_Absyn_Conditional_e){ _LL406: _temp405=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp379)->f1; goto _LL404; _LL404: _temp403=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp379)->f2; goto _LL402; _LL402: _temp401=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp379)->f3; goto _LL384;} else{ goto _LL385;} _LL385: if(*(( int*) _temp379)
== Cyc_Absyn_Primop_e){ _LL410: _temp409=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp379)->f1; goto _LL408; _LL408: _temp407=(( struct Cyc_Absyn_Primop_e_struct*)
_temp379)->f2; goto _LL386;} else{ goto _LL387;} _LL387: if(*(( int*) _temp379)
== Cyc_Absyn_Sizeoftyp_e){ goto _LL388;} else{ goto _LL389;} _LL389: if(*(( int*)
_temp379) == Cyc_Absyn_Sizeofexp_e){ goto _LL390;} else{ goto _LL391;} _LL391:
if(*(( int*) _temp379) == Cyc_Absyn_Offsetof_e){ goto _LL392;} else{ goto _LL393;}
_LL393: if(*(( int*) _temp379) == Cyc_Absyn_Cast_e){ _LL414: _temp413=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp379)->f1; goto _LL412; _LL412: _temp411=((
struct Cyc_Absyn_Cast_e_struct*) _temp379)->f2; goto _LL394;} else{ goto _LL395;}
_LL395: if(*(( int*) _temp379) == Cyc_Absyn_Enum_e){ _LL416: _temp415=(( struct
Cyc_Absyn_Enum_e_struct*) _temp379)->f3; goto _LL396;} else{ goto _LL397;}
_LL397: goto _LL398; _LL382: return _temp399; _LL384: cn= Cyc_Evexp_eval_const_bool_exp(
_temp405)? Cyc_Evexp_eval_const_exp( _temp403): Cyc_Evexp_eval_const_exp(
_temp401); goto _LL380; _LL386: if( _temp407 == 0){ Cyc_Evexp_exp_err( e->loc,
_tag_arr("bad static expression (no args to primop)", sizeof( unsigned char), 42u));
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp417=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp417[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp418; _temp418.tag= Cyc_Absyn_Int_c; _temp418.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp418.f2= 0; _temp418;}); _temp417;});} if((( struct Cyc_List_List*)
_check_null( _temp407))->tl == 0){ cn= Cyc_Evexp_eval_const_unprimop( _temp409,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp407))->hd);}
else{ if((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp407))->tl))->tl != 0){ Cyc_Evexp_exp_err( e->loc, _tag_arr("bad static expression (too many args to primop)",
sizeof( unsigned char), 48u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp419=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp419[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp420; _temp420.tag= Cyc_Absyn_Int_c;
_temp420.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp420.f2= 0; _temp420;});
_temp419;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp409,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp407))->hd,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp407))->tl))->hd);}} goto _LL380; _LL388: goto _LL390; _LL390: Cyc_Evexp_exp_err(
e->loc, _tag_arr("sizeof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 62u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp421=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp421[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp422; _temp422.tag= Cyc_Absyn_Int_c;
_temp422.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp422.f2= 0; _temp422;});
_temp421;}); _LL392: Cyc_Evexp_exp_err( e->loc, _tag_arr("offsetof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 64u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp423=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp423[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp424; _temp424.tag= Cyc_Absyn_Int_c;
_temp424.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp424.f2= 0; _temp424;});
_temp423;}); _LL394: cn= Cyc_Evexp_eval_const_exp( _temp411);{ struct _tuple4
_temp426=({ struct _tuple4 _temp425; _temp425.f1= Cyc_Tcutil_compress( _temp413);
_temp425.f2= cn; _temp425;}); void* _temp436; unsigned char _temp438; void*
_temp440; void* _temp442; void* _temp444; void* _temp446; void* _temp448; short
_temp450; void* _temp452; void* _temp454; void* _temp456; void* _temp458; void*
_temp460; int _temp462; void* _temp464; void* _temp466; void* _temp468; void*
_temp470; _LL428: _LL443: _temp442= _temp426.f1; if(( unsigned int) _temp442 > 4u?*((
int*) _temp442) == Cyc_Absyn_IntType: 0){ _LL447: _temp446=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp442)->f1; goto _LL445; _LL445: _temp444=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp442)->f2; goto _LL437;} else{ goto _LL430;} _LL437: _temp436= _temp426.f2;
if(( unsigned int) _temp436 > 1u?*(( int*) _temp436) == Cyc_Absyn_Char_c: 0){
_LL441: _temp440=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp436)->f1; goto
_LL439; _LL439: _temp438=(( struct Cyc_Absyn_Char_c_struct*) _temp436)->f2; goto
_LL429;} else{ goto _LL430;} _LL430: _LL455: _temp454= _temp426.f1; if((
unsigned int) _temp454 > 4u?*(( int*) _temp454) == Cyc_Absyn_IntType: 0){ _LL459:
_temp458=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp454)->f1; goto _LL457;
_LL457: _temp456=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp454)->f2;
goto _LL449;} else{ goto _LL432;} _LL449: _temp448= _temp426.f2; if((
unsigned int) _temp448 > 1u?*(( int*) _temp448) == Cyc_Absyn_Short_c: 0){ _LL453:
_temp452=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp448)->f1; goto _LL451;
_LL451: _temp450=(( struct Cyc_Absyn_Short_c_struct*) _temp448)->f2; goto _LL431;}
else{ goto _LL432;} _LL432: _LL467: _temp466= _temp426.f1; if(( unsigned int)
_temp466 > 4u?*(( int*) _temp466) == Cyc_Absyn_IntType: 0){ _LL471: _temp470=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp466)->f1; goto _LL469; _LL469:
_temp468=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp466)->f2; goto _LL461;}
else{ goto _LL434;} _LL461: _temp460= _temp426.f2; if(( unsigned int) _temp460 >
1u?*(( int*) _temp460) == Cyc_Absyn_Int_c: 0){ _LL465: _temp464=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp460)->f1; goto _LL463; _LL463: _temp462=((
struct Cyc_Absyn_Int_c_struct*) _temp460)->f2; goto _LL433;} else{ goto _LL434;}
_LL434: goto _LL435; _LL429: _temp458= _temp446; _temp456= _temp444; _temp452=
_temp440; _temp450=( short) _temp438; goto _LL431; _LL431: _temp470= _temp458;
_temp468= _temp456; _temp464= _temp452; _temp462=( int) _temp450; goto _LL433;
_LL433: if( _temp470 != _temp464){ cn=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp472=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp472[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp473; _temp473.tag= Cyc_Absyn_Int_c;
_temp473.f1=( void*) _temp470; _temp473.f2= _temp462; _temp473;}); _temp472;});}
goto _LL427; _LL435: Cyc_Evexp_exp_err( e->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp474= Cyc_Absynpp_typ2string( _temp413); xprintf("eval_const: cannot cast to %.*s",
_get_arr_size( _temp474, 1u), _temp474.curr);})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp475=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp475[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp476; _temp476.tag= Cyc_Absyn_Int_c;
_temp476.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp476.f2= 0; _temp476;});
_temp475;}); _LL427:;} goto _LL380; _LL396: return Cyc_Evexp_eval_const_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*) _check_null(
_temp415))->tag)); _LL398: Cyc_Evexp_exp_err( e->loc, _tag_arr("bad static expression",
sizeof( unsigned char), 22u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp477=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp477[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp478; _temp478.tag= Cyc_Absyn_Int_c;
_temp478.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp478.f2= 0; _temp478;});
_temp477;}); _LL380:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp479=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp479[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp480; _temp480.tag= Cyc_Absyn_Const_e;
_temp480.f1=( void*) cn; _temp480;}); _temp479;}))); return cn;}