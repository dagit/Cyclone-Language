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
Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct
Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern
void* Cyc_Absyn_exn_typ; extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment* s); struct Cyc_Stdio___sFILE;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Stdio_xprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
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
extern void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
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
struct Cyc_Tcenv_Tenv*, struct _tagged_arr*, struct Cyc_Absyn_Stmt*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar*
name); extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple4*
Cyc_Tcutil_make_inst_var( struct Cyc_List_List*, struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern int* Cyc_Tcutil_new_tvar_id();
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_arr msg_part); struct _tuple5{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple5 Cyc_Tcpat_tcPat( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt);
extern void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p); extern void Cyc_Tcpat_check_switch_exhaustive( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct Cyc_CfFlowInfo_Place;
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_arr* f1; } ; static const int Cyc_CfFlowInfo_TupleF= 1;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_Esc=
0; static const int Cyc_CfFlowInfo_Unesc= 1; static const int Cyc_CfFlowInfo_NoneIL=
0; static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_UnknownIS= 0; struct Cyc_CfFlowInfo_UnknownIS_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfFlowInfo_MustPointTo= 1;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI=
2; struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; } ; extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); static void Cyc_Tcstmt_decorate_stmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser(
te)); s->try_depth= Cyc_Tcenv_get_try_depth( te);} static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
tvs, struct Cyc_List_List* vs, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp*
where_opt, int new_block){ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars(
loc, te, tvs); if( new_block){ te2= Cyc_Tcenv_new_block( loc, te2);}{ struct Cyc_Core_Opt*
_temp0=({ struct Cyc_Core_Opt* _temp2=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp2->v=( void*) Cyc_Tcenv_curr_rgn( te2); _temp2;});{
struct Cyc_List_List* _temp1= vs; for( 0; _temp1 != 0; _temp1=(( struct Cyc_List_List*)
_check_null( _temp1))->tl){ te2= Cyc_Tcenv_add_pat_var( loc, te2,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1))->hd);(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1))->hd)->rgn= _temp0;}} if( where_opt
!= 0){ Cyc_Tcexp_tcTest( te2,( struct Cyc_Absyn_Exp*) _check_null( where_opt),
_tag_arr("switch clause guard", sizeof( unsigned char), 20u));} if( vs != 0){
te2= Cyc_Tcenv_set_encloser( te2, s);} Cyc_Tcstmt_tcStmt( te2, s, 0); if( vs !=
0){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));}}} void
Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s0, int
new_block){ Cyc_Tcstmt_decorate_stmt( te, s0);{ void* _temp3=( void*) s0->r;
struct Cyc_Absyn_Exp* _temp45; struct Cyc_Absyn_Stmt* _temp47; struct Cyc_Absyn_Stmt*
_temp49; struct Cyc_Absyn_Exp* _temp51; struct Cyc_Absyn_Stmt* _temp53; struct
Cyc_Absyn_Stmt* _temp55; struct Cyc_Absyn_Exp* _temp57; struct Cyc_Absyn_Stmt*
_temp59; struct _tuple2 _temp61; struct Cyc_Absyn_Stmt* _temp63; struct Cyc_Absyn_Exp*
_temp65; struct Cyc_Absyn_Stmt* _temp67; struct _tuple2 _temp69; struct Cyc_Absyn_Stmt*
_temp71; struct Cyc_Absyn_Exp* _temp73; struct _tuple2 _temp75; struct Cyc_Absyn_Stmt*
_temp77; struct Cyc_Absyn_Exp* _temp79; struct Cyc_Absyn_Exp* _temp81; struct
_tuple2 _temp83; struct Cyc_Absyn_Stmt* _temp85; struct Cyc_Absyn_Exp* _temp87;
struct Cyc_Absyn_Stmt* _temp89; struct Cyc_Absyn_Stmt* _temp91; struct Cyc_Absyn_Stmt**
_temp93; struct Cyc_Absyn_Stmt* _temp94; struct Cyc_Absyn_Stmt** _temp96; struct
Cyc_Absyn_Stmt* _temp97; struct Cyc_Absyn_Stmt** _temp99; struct _tagged_arr*
_temp100; struct Cyc_Absyn_Switch_clause** _temp102; struct Cyc_Absyn_Switch_clause***
_temp104; struct Cyc_List_List* _temp105; struct Cyc_Absyn_Stmt* _temp107;
struct _tagged_arr* _temp109; struct Cyc_List_List* _temp111; struct Cyc_Absyn_Exp*
_temp113; struct Cyc_List_List* _temp115; struct Cyc_Absyn_Exp* _temp117; struct
Cyc_List_List* _temp119; struct Cyc_Absyn_Stmt* _temp121; struct Cyc_Absyn_Stmt*
_temp123; struct Cyc_Absyn_Decl* _temp125; struct Cyc_Absyn_Stmt* _temp127;
struct Cyc_Absyn_Vardecl* _temp129; struct Cyc_Absyn_Tvar* _temp131; struct Cyc_Absyn_Stmt*
_temp133; struct Cyc_Absyn_Stmt* _temp135; _LL5: if( _temp3 ==( void*) Cyc_Absyn_Skip_s){
goto _LL6;} else{ goto _LL7;} _LL7: if(( unsigned int) _temp3 > 1u?*(( int*)
_temp3) == Cyc_Absyn_Exp_s: 0){ _LL46: _temp45=(( struct Cyc_Absyn_Exp_s_struct*)
_temp3)->f1; goto _LL8;} else{ goto _LL9;} _LL9: if(( unsigned int) _temp3 > 1u?*((
int*) _temp3) == Cyc_Absyn_Seq_s: 0){ _LL50: _temp49=(( struct Cyc_Absyn_Seq_s_struct*)
_temp3)->f1; goto _LL48; _LL48: _temp47=(( struct Cyc_Absyn_Seq_s_struct*)
_temp3)->f2; goto _LL10;} else{ goto _LL11;} _LL11: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_Return_s: 0){ _LL52: _temp51=(( struct Cyc_Absyn_Return_s_struct*)
_temp3)->f1; goto _LL12;} else{ goto _LL13;} _LL13: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_IfThenElse_s: 0){ _LL58: _temp57=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp3)->f1; goto _LL56; _LL56: _temp55=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp3)->f2; goto _LL54; _LL54: _temp53=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp3)->f3; goto _LL14;} else{ goto _LL15;} _LL15: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_While_s: 0){ _LL62: _temp61=(( struct Cyc_Absyn_While_s_struct*)
_temp3)->f1; _LL66: _temp65= _temp61.f1; goto _LL64; _LL64: _temp63= _temp61.f2;
goto _LL60; _LL60: _temp59=(( struct Cyc_Absyn_While_s_struct*) _temp3)->f2;
goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp3 > 1u?*(( int*)
_temp3) == Cyc_Absyn_For_s: 0){ _LL82: _temp81=(( struct Cyc_Absyn_For_s_struct*)
_temp3)->f1; goto _LL76; _LL76: _temp75=(( struct Cyc_Absyn_For_s_struct*)
_temp3)->f2; _LL80: _temp79= _temp75.f1; goto _LL78; _LL78: _temp77= _temp75.f2;
goto _LL70; _LL70: _temp69=(( struct Cyc_Absyn_For_s_struct*) _temp3)->f3; _LL74:
_temp73= _temp69.f1; goto _LL72; _LL72: _temp71= _temp69.f2; goto _LL68; _LL68:
_temp67=(( struct Cyc_Absyn_For_s_struct*) _temp3)->f4; goto _LL18;} else{ goto
_LL19;} _LL19: if(( unsigned int) _temp3 > 1u?*(( int*) _temp3) == Cyc_Absyn_Do_s:
0){ _LL90: _temp89=(( struct Cyc_Absyn_Do_s_struct*) _temp3)->f1; goto _LL84;
_LL84: _temp83=(( struct Cyc_Absyn_Do_s_struct*) _temp3)->f2; _LL88: _temp87=
_temp83.f1; goto _LL86; _LL86: _temp85= _temp83.f2; goto _LL20;} else{ goto
_LL21;} _LL21: if(( unsigned int) _temp3 > 1u?*(( int*) _temp3) == Cyc_Absyn_Break_s:
0){ _LL92: _temp91=(( struct Cyc_Absyn_Break_s_struct*) _temp3)->f1; _temp93=&((
struct Cyc_Absyn_Break_s_struct*) _temp3)->f1; goto _LL22;} else{ goto _LL23;}
_LL23: if(( unsigned int) _temp3 > 1u?*(( int*) _temp3) == Cyc_Absyn_Continue_s:
0){ _LL95: _temp94=(( struct Cyc_Absyn_Continue_s_struct*) _temp3)->f1; _temp96=&((
struct Cyc_Absyn_Continue_s_struct*) _temp3)->f1; goto _LL24;} else{ goto _LL25;}
_LL25: if(( unsigned int) _temp3 > 1u?*(( int*) _temp3) == Cyc_Absyn_Goto_s: 0){
_LL101: _temp100=(( struct Cyc_Absyn_Goto_s_struct*) _temp3)->f1; goto _LL98;
_LL98: _temp97=(( struct Cyc_Absyn_Goto_s_struct*) _temp3)->f2; _temp99=&((
struct Cyc_Absyn_Goto_s_struct*) _temp3)->f2; goto _LL26;} else{ goto _LL27;}
_LL27: if(( unsigned int) _temp3 > 1u?*(( int*) _temp3) == Cyc_Absyn_Fallthru_s:
0){ _LL106: _temp105=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp3)->f1; goto
_LL103; _LL103: _temp102=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp3)->f2;
_temp104=&(( struct Cyc_Absyn_Fallthru_s_struct*) _temp3)->f2; goto _LL28;}
else{ goto _LL29;} _LL29: if(( unsigned int) _temp3 > 1u?*(( int*) _temp3) ==
Cyc_Absyn_Label_s: 0){ _LL110: _temp109=(( struct Cyc_Absyn_Label_s_struct*)
_temp3)->f1; goto _LL108; _LL108: _temp107=(( struct Cyc_Absyn_Label_s_struct*)
_temp3)->f2; goto _LL30;} else{ goto _LL31;} _LL31: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_Switch_s: 0){ _LL114: _temp113=(( struct Cyc_Absyn_Switch_s_struct*)
_temp3)->f1; goto _LL112; _LL112: _temp111=(( struct Cyc_Absyn_Switch_s_struct*)
_temp3)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_SwitchC_s: 0){ _LL118: _temp117=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp3)->f1; goto _LL116; _LL116: _temp115=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp3)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_TryCatch_s: 0){ _LL122: _temp121=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp3)->f1; goto _LL120; _LL120: _temp119=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp3)->f2; goto _LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_Decl_s: 0){ _LL126: _temp125=(( struct Cyc_Absyn_Decl_s_struct*)
_temp3)->f1; goto _LL124; _LL124: _temp123=(( struct Cyc_Absyn_Decl_s_struct*)
_temp3)->f2; goto _LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_Region_s: 0){ _LL132: _temp131=(( struct Cyc_Absyn_Region_s_struct*)
_temp3)->f1; goto _LL130; _LL130: _temp129=(( struct Cyc_Absyn_Region_s_struct*)
_temp3)->f2; goto _LL128; _LL128: _temp127=(( struct Cyc_Absyn_Region_s_struct*)
_temp3)->f3; goto _LL40;} else{ goto _LL41;} _LL41: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_Cut_s: 0){ _LL134: _temp133=(( struct Cyc_Absyn_Cut_s_struct*)
_temp3)->f1; goto _LL42;} else{ goto _LL43;} _LL43: if(( unsigned int) _temp3 >
1u?*(( int*) _temp3) == Cyc_Absyn_Splice_s: 0){ _LL136: _temp135=(( struct Cyc_Absyn_Splice_s_struct*)
_temp3)->f1; goto _LL44;} else{ goto _LL4;} _LL6: return; _LL8: Cyc_Tcexp_tcExp(
te, 0, _temp45); return; _LL10: { struct Cyc_Tcenv_Tenv* _temp137= Cyc_Tcenv_set_next(
te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp150=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp150[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp151; _temp151.tag= Cyc_Tcenv_Stmt_j; _temp151.f1= _temp47; _temp151;});
_temp150;})); Cyc_Tcstmt_tcStmt( _temp137, _temp49, 1); while( 1) { void*
_temp138=( void*) _temp47->r; struct Cyc_Absyn_Stmt* _temp144; struct Cyc_Absyn_Stmt*
_temp146; _LL140: if(( unsigned int) _temp138 > 1u?*(( int*) _temp138) == Cyc_Absyn_Seq_s:
0){ _LL147: _temp146=(( struct Cyc_Absyn_Seq_s_struct*) _temp138)->f1; goto
_LL145; _LL145: _temp144=(( struct Cyc_Absyn_Seq_s_struct*) _temp138)->f2; goto
_LL141;} else{ goto _LL142;} _LL142: goto _LL143; _LL141: Cyc_Tcstmt_decorate_stmt(
te, _temp47); _temp137= Cyc_Tcenv_set_next( te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp148=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp148[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp149; _temp149.tag= Cyc_Tcenv_Stmt_j;
_temp149.f1= _temp144; _temp149;}); _temp148;})); Cyc_Tcstmt_tcStmt( _temp137,
_temp146, 1); _temp47= _temp144; continue; _LL143: goto seq_end; _LL139:;}
seq_end: Cyc_Tcstmt_tcStmt( te, _temp47, 1); return;} _LL12: { void* t= Cyc_Tcenv_return_typ(
te); if( _temp51 == 0){ void* _temp152= Cyc_Tcutil_compress( t); _LL154: if(
_temp152 ==( void*) Cyc_Absyn_VoidType){ goto _LL155;} else{ goto _LL156;}
_LL156: goto _LL157; _LL155: goto _LL153; _LL157: Cyc_Tcutil_terr( s0->loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp158)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp162= _tag_arr("must return a value of type %s",
sizeof( unsigned char), 31u); struct Cyc_Stdio_String_pa_struct _temp163;
_temp163.tag= Cyc_Stdio_String_pa; _temp163.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp161=( void*)& _temp163; void* _temp159[ 1u]={ _temp161}; struct
_tagged_arr _temp160={( void*) _temp159,( void*) _temp159,( void*)( _temp159 + 1u)};
_temp158( _temp162, _temp160);}})); goto _LL153; _LL153:;} else{ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*) _check_null( _temp51); Cyc_Tcexp_tcExp( te,( void**)&
t, e); if( ! Cyc_Tcutil_coerce_use( te, e, t)){ Cyc_Tcutil_terr( s0->loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp164)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp169= _tag_arr("returns value of type %s but requires %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp171;
_temp171.tag= Cyc_Stdio_String_pa; _temp171.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp167=(
void*)& _temp171; struct Cyc_Stdio_String_pa_struct _temp170; _temp170.tag= Cyc_Stdio_String_pa;
_temp170.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp168=(
void*)& _temp170; void* _temp165[ 2u]={ _temp167, _temp168}; struct _tagged_arr
_temp166={( void*) _temp165,( void*) _temp165,( void*)( _temp165 + 2u)};
_temp164( _temp169, _temp166);}}}));}{ void* _temp172= Cyc_Tcutil_compress( t);
_LL174: if( _temp172 ==( void*) Cyc_Absyn_VoidType){ goto _LL175;} else{ goto
_LL176;} _LL176: goto _LL177; _LL175: Cyc_Tcutil_terr( s0->loc, _tag_arr("function returns void expression",
sizeof( unsigned char), 33u)); goto _LL173; _LL177: goto _LL173; _LL173:;}}
return;} _LL14: Cyc_Tcexp_tcTest( te, _temp57, _tag_arr("if statement", sizeof(
unsigned char), 13u)); Cyc_Tcstmt_tcStmt( te, _temp55, 1); Cyc_Tcstmt_tcStmt( te,
_temp53, 1); return; _LL16: Cyc_Tcstmt_decorate_stmt( te, _temp63); Cyc_Tcexp_tcTest(
te, _temp65, _tag_arr("while loop", sizeof( unsigned char), 11u)); Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop( te, _temp63), _temp59, 1); return; _LL18: Cyc_Tcstmt_decorate_stmt(
te, _temp77); Cyc_Tcstmt_decorate_stmt( te, _temp71); Cyc_Tcexp_tcExp( te, 0,
_temp81); Cyc_Tcexp_tcTest( te, _temp79, _tag_arr("for loop", sizeof(
unsigned char), 9u)); te= Cyc_Tcenv_set_in_loop( te, _temp71); Cyc_Tcstmt_tcStmt(
te, _temp67, 1); Cyc_Tcexp_tcExp( te, 0, _temp73); return; _LL20: Cyc_Tcstmt_decorate_stmt(
te, _temp85); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop( te, _temp85), _temp89, 1);
Cyc_Tcexp_tcTest( te, _temp87, _tag_arr("do loop", sizeof( unsigned char), 8u));
return; _LL22: Cyc_Tcenv_process_break( te, s0, _temp93); return; _LL24: Cyc_Tcenv_process_continue(
te, s0, _temp96); return; _LL26: Cyc_Tcenv_process_goto( te, s0, _temp100,
_temp99); return; _LL28: { struct _tuple3* _temp178= Cyc_Tcenv_process_fallthru(
te, s0, _temp104); if( _temp178 == 0){ Cyc_Tcutil_terr( s0->loc, _tag_arr("fallthru not in a non-last case",
sizeof( unsigned char), 32u)); return;}{ struct _tuple3 _temp181; struct Cyc_List_List*
_temp182; struct Cyc_List_List* _temp184; struct _tuple3* _temp179=( struct
_tuple3*) _check_null( _temp178); _temp181=* _temp179; _LL185: _temp184=
_temp181.f2; goto _LL183; _LL183: _temp182= _temp181.f3; goto _LL180; _LL180: {
struct Cyc_List_List* instantiation=(( struct Cyc_List_List*(*)( struct _tuple4*(*
f)( struct Cyc_List_List*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_make_inst_var, Cyc_Tcenv_lookup_type_vars(
te), _temp184); _temp182=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_List_List*,
void*), struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcutil_substitute, instantiation, _temp182); for( 0; _temp182 != 0? _temp105
!= 0: 0;( _temp182=(( struct Cyc_List_List*) _check_null( _temp182))->tl,
_temp105=(( struct Cyc_List_List*) _check_null( _temp105))->tl)){ Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp105))->hd);
if( ! Cyc_Tcutil_coerce_arg( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp105))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp182))->hd)){ Cyc_Tcutil_terr( s0->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp186)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp191= _tag_arr("fallthru argument has type %s but pattern variable has type %s",
sizeof( unsigned char), 63u); struct Cyc_Stdio_String_pa_struct _temp193;
_temp193.tag= Cyc_Stdio_String_pa; _temp193.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp105))->hd)->topt))->v);{ void* _temp189=( void*)&
_temp193; struct Cyc_Stdio_String_pa_struct _temp192; _temp192.tag= Cyc_Stdio_String_pa;
_temp192.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp182))->hd);{ void* _temp190=( void*)& _temp192; void* _temp187[
2u]={ _temp189, _temp190}; struct _tagged_arr _temp188={( void*) _temp187,( void*)
_temp187,( void*)( _temp187 + 2u)}; _temp186( _temp191, _temp188);}}}));}} if(
_temp105 != 0){ Cyc_Tcutil_terr( s0->loc, _tag_arr("too many arguments to explicit fallthru",
sizeof( unsigned char), 40u));} if( _temp182 != 0){ Cyc_Tcutil_terr( s0->loc,
_tag_arr("too few arguments to explicit fallthru", sizeof( unsigned char), 39u));}
return;}}} _LL30: Cyc_Tcstmt_tcStmt( Cyc_Tcenv_new_named_block( s0->loc, Cyc_Tcenv_add_label(
te, _temp109, _temp107),({ struct Cyc_Absyn_Tvar* _temp194=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp194->name=({ struct _tagged_arr*
_temp195=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp195[ 0]=( struct _tagged_arr)({ struct _tagged_arr(* _temp196)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr
_temp200= _tag_arr("`%s", sizeof( unsigned char), 4u); struct Cyc_Stdio_String_pa_struct
_temp201; _temp201.tag= Cyc_Stdio_String_pa; _temp201.f1=( struct _tagged_arr)*
_temp109;{ void* _temp199=( void*)& _temp201; void* _temp197[ 1u]={ _temp199};
struct _tagged_arr _temp198={( void*) _temp197,( void*) _temp197,( void*)(
_temp197 + 1u)}; _temp196( _temp200, _temp198);}}); _temp195;}); _temp194->identity=
Cyc_Tcutil_new_tvar_id(); _temp194->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp194;})), _temp107, 0); return; _LL32: Cyc_Tcexp_tcExp( te, 0, _temp113);{
void* _temp202=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp113->topt))->v;
te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List*
scs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp111); for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){
struct Cyc_Absyn_Pat* _temp203=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd)->pattern; struct Cyc_List_List* _temp206; struct Cyc_List_List*
_temp208; struct _tuple5 _temp204= Cyc_Tcpat_tcPat( te, _temp203,( void**)&
_temp202, 0); _LL209: _temp208= _temp204.f1; goto _LL207; _LL207: _temp206=
_temp204.f2; goto _LL205; _LL205: if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp203->topt))->v, _temp202)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd)->loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp210)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp215= _tag_arr("switch on type %s, but case expects type %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp217;
_temp217.tag= Cyc_Stdio_String_pa; _temp217.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp202);{ void* _temp213=( void*)& _temp217; struct Cyc_Stdio_String_pa_struct
_temp216; _temp216.tag= Cyc_Stdio_String_pa; _temp216.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp203->topt))->v);{
void* _temp214=( void*)& _temp216; void* _temp211[ 2u]={ _temp213, _temp214};
struct _tagged_arr _temp212={( void*) _temp211,( void*) _temp211,( void*)(
_temp211 + 2u)}; _temp210( _temp215, _temp212);}}}));} else{ Cyc_Tcpat_check_pat_regions(
te, _temp203);}(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp218=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp218->v=( void*) _temp206;
_temp218;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd)->loc, te, _temp208, _temp206,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->body,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp208, _temp206,( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd);}} Cyc_Tcpat_check_switch_exhaustive(
s0->loc, _temp111); return;} _LL34: { struct Cyc_List_List* _temp219= 0; for( 0;
_temp115 != 0; _temp115=(( struct Cyc_List_List*) _check_null( _temp115))->tl){
struct Cyc_Absyn_SwitchC_clause _temp222; struct Cyc_Position_Segment* _temp223;
struct Cyc_Absyn_Stmt* _temp225; struct Cyc_Absyn_Exp* _temp227; struct Cyc_Absyn_SwitchC_clause*
_temp220=( struct Cyc_Absyn_SwitchC_clause*)(( struct Cyc_List_List*)
_check_null( _temp115))->hd; _temp222=* _temp220; _LL228: _temp227= _temp222.cnst_exp;
goto _LL226; _LL226: _temp225= _temp222.body; goto _LL224; _LL224: _temp223=
_temp222.loc; goto _LL221; _LL221: { struct Cyc_Absyn_Pat* pat; if( _temp227 !=
0){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*) _check_null( _temp227)); pat=
Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct* _temp229=( struct
Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp229[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp230; _temp230.tag= Cyc_Absyn_Int_p;
_temp230.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp230.f2=( int) Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp227)); _temp230;}); _temp229;}),((
struct Cyc_Absyn_Exp*) _check_null( _temp227))->loc);} else{ pat= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, _temp223);} _temp219=({ struct Cyc_List_List* _temp231=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp231->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp232=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp232->pattern= pat;
_temp232->pat_vars= 0; _temp232->where_clause= 0; _temp232->body= _temp225;
_temp232->loc= _temp223; _temp232;}); _temp231->tl= _temp219; _temp231;});}}(
void*)( s0->r=( void*)(( void*)( Cyc_Absyn_switch_stmt( _temp117,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp219), 0))->r)); Cyc_Tcstmt_tcStmt(
te, s0, new_block); return;} _LL36:( void*)( _temp121->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
Cyc_Absyn_new_stmt(( void*) _temp121->r, _temp121->loc), Cyc_Absyn_skip_stmt(
_temp121->loc), _temp121->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_enter_try( te), s0), _temp121, 1); te= Cyc_Tcenv_set_in_switch( te);
te= Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List* _temp233=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp119); for( 0; _temp233 != 0;
_temp233=(( struct Cyc_List_List*) _check_null( _temp233))->tl){ struct Cyc_Absyn_Pat*
_temp234=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp233))->hd)->pattern; struct Cyc_List_List* _temp237; struct
Cyc_List_List* _temp239; struct _tuple5 _temp235= Cyc_Tcpat_tcPat( te, _temp234,(
void**)& Cyc_Absyn_exn_typ, 0); _LL240: _temp239= _temp235.f1; goto _LL238;
_LL238: _temp237= _temp235.f2; goto _LL236; _LL236: if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp234->topt))->v, Cyc_Absyn_exn_typ)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp233))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp241)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp245= _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u); struct Cyc_Stdio_String_pa_struct _temp246;
_temp246.tag= Cyc_Stdio_String_pa; _temp246.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp234->topt))->v);{ void*
_temp244=( void*)& _temp246; void* _temp242[ 1u]={ _temp244}; struct _tagged_arr
_temp243={( void*) _temp242,( void*) _temp242,( void*)( _temp242 + 1u)};
_temp241( _temp245, _temp243);}}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp234);}(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp233))->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp247=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp247->v=( void*)
_temp237; _temp247;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp233))->hd)->loc, te, _temp239, _temp237,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp233))->hd)->body,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp233))->hd)->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp239, _temp237,( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp233))->hd);}} Cyc_Tcpat_check_catch_overlap(
s0->loc, _temp119); return; _LL38: { struct _tagged_arr unimp_msg_part; if(
new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ void* _temp248=( void*)
_temp125->r; struct Cyc_Absyn_Vardecl* _temp274; int _temp276; int* _temp278;
struct Cyc_Absyn_Exp* _temp279; struct Cyc_Core_Opt* _temp281; struct Cyc_Core_Opt**
_temp283; struct Cyc_Core_Opt* _temp284; struct Cyc_Core_Opt** _temp286; struct
Cyc_Absyn_Pat* _temp287; struct Cyc_List_List* _temp289; struct Cyc_List_List*
_temp291; struct _tagged_arr* _temp293; struct Cyc_List_List* _temp295; struct
_tuple0* _temp297; _LL250: if(*(( int*) _temp248) == Cyc_Absyn_Var_d){ _LL275:
_temp274=(( struct Cyc_Absyn_Var_d_struct*) _temp248)->f1; goto _LL251;} else{
goto _LL252;} _LL252: if(*(( int*) _temp248) == Cyc_Absyn_Let_d){ _LL288:
_temp287=(( struct Cyc_Absyn_Let_d_struct*) _temp248)->f1; goto _LL285; _LL285:
_temp284=(( struct Cyc_Absyn_Let_d_struct*) _temp248)->f2; _temp286=&(( struct
Cyc_Absyn_Let_d_struct*) _temp248)->f2; goto _LL282; _LL282: _temp281=(( struct
Cyc_Absyn_Let_d_struct*) _temp248)->f3; _temp283=&(( struct Cyc_Absyn_Let_d_struct*)
_temp248)->f3; goto _LL280; _LL280: _temp279=(( struct Cyc_Absyn_Let_d_struct*)
_temp248)->f4; goto _LL277; _LL277: _temp276=(( struct Cyc_Absyn_Let_d_struct*)
_temp248)->f5; _temp278=&(( struct Cyc_Absyn_Let_d_struct*) _temp248)->f5; goto
_LL253;} else{ goto _LL254;} _LL254: if(*(( int*) _temp248) == Cyc_Absyn_Letv_d){
_LL290: _temp289=(( struct Cyc_Absyn_Letv_d_struct*) _temp248)->f1; goto _LL255;}
else{ goto _LL256;} _LL256: if(*(( int*) _temp248) == Cyc_Absyn_Namespace_d){
_LL294: _temp293=(( struct Cyc_Absyn_Namespace_d_struct*) _temp248)->f1; goto
_LL292; _LL292: _temp291=(( struct Cyc_Absyn_Namespace_d_struct*) _temp248)->f2;
goto _LL257;} else{ goto _LL258;} _LL258: if(*(( int*) _temp248) == Cyc_Absyn_Using_d){
_LL298: _temp297=(( struct Cyc_Absyn_Using_d_struct*) _temp248)->f1; goto _LL296;
_LL296: _temp295=(( struct Cyc_Absyn_Using_d_struct*) _temp248)->f2; goto _LL259;}
else{ goto _LL260;} _LL260: if(*(( int*) _temp248) == Cyc_Absyn_Fn_d){ goto
_LL261;} else{ goto _LL262;} _LL262: if(*(( int*) _temp248) == Cyc_Absyn_Struct_d){
goto _LL263;} else{ goto _LL264;} _LL264: if(*(( int*) _temp248) == Cyc_Absyn_Union_d){
goto _LL265;} else{ goto _LL266;} _LL266: if(*(( int*) _temp248) == Cyc_Absyn_Tunion_d){
goto _LL267;} else{ goto _LL268;} _LL268: if(*(( int*) _temp248) == Cyc_Absyn_Enum_d){
goto _LL269;} else{ goto _LL270;} _LL270: if(*(( int*) _temp248) == Cyc_Absyn_Typedef_d){
goto _LL271;} else{ goto _LL272;} _LL272: if(*(( int*) _temp248) == Cyc_Absyn_ExternC_d){
goto _LL273;} else{ goto _LL249;} _LL251: { struct Cyc_Absyn_Vardecl _temp301;
struct Cyc_List_List* _temp302; struct Cyc_Core_Opt* _temp304; struct Cyc_Core_Opt**
_temp306; struct Cyc_Absyn_Exp* _temp307; void* _temp309; struct Cyc_Absyn_Tqual
_temp311; struct _tuple0* _temp313; struct _tuple0 _temp315; struct _tagged_arr*
_temp316; void* _temp318; void* _temp320; struct Cyc_Absyn_Vardecl* _temp299=
_temp274; _temp301=* _temp299; _LL321: _temp320=( void*) _temp301.sc; goto
_LL314; _LL314: _temp313= _temp301.name; _temp315=* _temp313; _LL319: _temp318=
_temp315.f1; goto _LL317; _LL317: _temp316= _temp315.f2; goto _LL312; _LL312:
_temp311= _temp301.tq; goto _LL310; _LL310: _temp309=( void*) _temp301.type;
goto _LL308; _LL308: _temp307= _temp301.initializer; goto _LL305; _LL305:
_temp304= _temp301.rgn; _temp306=&(* _temp299).rgn; goto _LL303; _LL303:
_temp302= _temp301.attributes; goto _LL300; _LL300: { void* _temp322= Cyc_Tcenv_curr_rgn(
te); int is_local;{ void* _temp323= _temp320; _LL325: if( _temp323 ==( void*)
Cyc_Absyn_Static){ goto _LL326;} else{ goto _LL327;} _LL327: if( _temp323 ==(
void*) Cyc_Absyn_Extern){ goto _LL328;} else{ goto _LL329;} _LL329: if( _temp323
==( void*) Cyc_Absyn_ExternC){ goto _LL330;} else{ goto _LL331;} _LL331: if(
_temp323 ==( void*) Cyc_Absyn_Abstract){ goto _LL332;} else{ goto _LL333;}
_LL333: if( _temp323 ==( void*) Cyc_Absyn_Public){ goto _LL334;} else{ goto
_LL324;} _LL326: goto _LL328; _LL328: goto _LL330; _LL330: is_local= 0; goto
_LL324; _LL332: Cyc_Tcutil_terr( _temp125->loc, _tag_arr("bad abstract scope for local variable",
sizeof( unsigned char), 38u)); goto _LL334; _LL334: is_local= 1; goto _LL324;
_LL324:;}* _temp306= is_local?({ struct Cyc_Core_Opt* _temp335=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp335->v=( void*) _temp322;
_temp335;}):({ struct Cyc_Core_Opt* _temp336=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp336->v=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp336;});{ void* _temp337= _temp318; struct Cyc_List_List* _temp347; struct
Cyc_List_List* _temp349; _LL339: if( _temp337 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL340;} else{ goto _LL341;} _LL341: if(( unsigned int) _temp337 > 1u?*(( int*)
_temp337) == Cyc_Absyn_Rel_n: 0){ _LL348: _temp347=(( struct Cyc_Absyn_Rel_n_struct*)
_temp337)->f1; if( _temp347 == 0){ goto _LL342;} else{ goto _LL343;}} else{ goto
_LL343;} _LL343: if(( unsigned int) _temp337 > 1u?*(( int*) _temp337) == Cyc_Absyn_Abs_n:
0){ _LL350: _temp349=(( struct Cyc_Absyn_Abs_n_struct*) _temp337)->f1; goto
_LL344;} else{ goto _LL345;} _LL345: goto _LL346; _LL340: goto _LL338; _LL342:(*
_temp274->name).f1=( void*) Cyc_Absyn_Loc_n; goto _LL338; _LL344:( int) _throw(
Cyc_Tcutil_impos( _tag_arr("tcstmt: Abs_n declaration", sizeof( unsigned char),
26u))); _LL346: Cyc_Tcutil_terr( _temp125->loc, _tag_arr("cannot declare a qualified local variable",
sizeof( unsigned char), 42u)); goto _LL338; _LL338:;}{ void* _temp351= Cyc_Tcutil_compress(
_temp309); struct Cyc_Absyn_Exp* _temp358; struct Cyc_Absyn_Tqual _temp360; void*
_temp362; _LL353: if(( unsigned int) _temp351 > 4u?*(( int*) _temp351) == Cyc_Absyn_ArrayType:
0){ _LL363: _temp362=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp351)->f1;
goto _LL361; _LL361: _temp360=(( struct Cyc_Absyn_ArrayType_struct*) _temp351)->f2;
goto _LL359; _LL359: _temp358=(( struct Cyc_Absyn_ArrayType_struct*) _temp351)->f3;
if( _temp358 == 0){ goto _LL357;} else{ goto _LL355;}} else{ goto _LL355;}
_LL357: if( _temp274->initializer != 0){ goto _LL354;} else{ goto _LL355;}
_LL355: goto _LL356; _LL354:{ void* _temp364=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp274->initializer))->r; void* _temp374; struct _tagged_arr
_temp376; struct Cyc_List_List* _temp378; struct Cyc_List_List* _temp380; _LL366:
if(*(( int*) _temp364) == Cyc_Absyn_Const_e){ _LL375: _temp374=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp364)->f1; if(( unsigned int) _temp374 > 1u?*((
int*) _temp374) == Cyc_Absyn_String_c: 0){ _LL377: _temp376=(( struct Cyc_Absyn_String_c_struct*)
_temp374)->f1; goto _LL367;} else{ goto _LL368;}} else{ goto _LL368;} _LL368:
if(*(( int*) _temp364) == Cyc_Absyn_UnresolvedMem_e){ _LL379: _temp378=(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp364)->f2; goto _LL369;} else{ goto
_LL370;} _LL370: if(*(( int*) _temp364) == Cyc_Absyn_Array_e){ _LL381: _temp380=((
struct Cyc_Absyn_Array_e_struct*) _temp364)->f1; goto _LL371;} else{ goto _LL372;}
_LL372: goto _LL373; _LL367: _temp309=( void*)( _temp274->type=( void*)(( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp382=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp382[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp383; _temp383.tag= Cyc_Absyn_ArrayType; _temp383.f1=(
void*) _temp362; _temp383.f2= _temp360; _temp383.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp376, sizeof( unsigned char)), 0); _temp383;}); _temp382;})));
goto _LL365; _LL369: _temp380= _temp378; goto _LL371; _LL371: _temp309=( void*)(
_temp274->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp384=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp384[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp385; _temp385.tag= Cyc_Absyn_ArrayType;
_temp385.f1=( void*) _temp362; _temp385.f2= _temp360; _temp385.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp380), 0); _temp385;}); _temp384;}))); goto _LL365; _LL373: goto _LL365;
_LL365:;} goto _LL352; _LL356: goto _LL352; _LL352:;}{ struct Cyc_List_List*
_temp386= ! is_local? 0: Cyc_Tcenv_lookup_type_vars( te); Cyc_Tcutil_check_type(
s0->loc, te, _temp386,( void*) Cyc_Absyn_MemKind, _temp309);{ struct Cyc_Tcenv_Tenv*
_temp387= Cyc_Tcenv_add_local_var( _temp123->loc, te, _temp274); if( _temp320 ==(
void*) Cyc_Absyn_Extern? 1: _temp320 ==( void*) Cyc_Absyn_ExternC){ Cyc_Tcutil_terr(
_temp125->loc, _tag_arr("local extern declarations not yet supported", sizeof(
unsigned char), 44u));} if( _temp307 != 0){ Cyc_Tcexp_tcExpInitializer( _temp387,(
void**)& _temp309,( struct Cyc_Absyn_Exp*) _check_null( _temp307)); if( ! Cyc_Tcutil_coerce_assign(
_temp387,( struct Cyc_Absyn_Exp*) _check_null( _temp307), _temp309)){ Cyc_Tcutil_terr(
_temp125->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp388)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr
_temp394= _tag_arr("%s declared with type %s, initialized with type %s", sizeof(
unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp397; _temp397.tag=
Cyc_Stdio_String_pa; _temp397.f1=( struct _tagged_arr)* _temp316;{ void*
_temp391=( void*)& _temp397; struct Cyc_Stdio_String_pa_struct _temp396;
_temp396.tag= Cyc_Stdio_String_pa; _temp396.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp309);{ void* _temp392=( void*)& _temp396; struct Cyc_Stdio_String_pa_struct
_temp395; _temp395.tag= Cyc_Stdio_String_pa; _temp395.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*) _check_null( _temp307))->topt))->v);{ void* _temp393=( void*)&
_temp395; void* _temp389[ 3u]={ _temp391, _temp392, _temp393}; struct
_tagged_arr _temp390={( void*) _temp389,( void*) _temp389,( void*)( _temp389 + 3u)};
_temp388( _temp394, _temp390);}}}}));}} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
_temp387, s0), _temp123, 0); return;}}}} _LL253: { struct Cyc_Core_Opt* _temp398=*
_temp283; Cyc_Tcexp_tcExpInitializer( te, 0, _temp279);* _temp283= _temp279->topt;{
struct Cyc_List_List* _temp401; struct Cyc_List_List* _temp403; struct _tuple5
_temp399= Cyc_Tcpat_tcPat( te, _temp287,( void**)(( void**)(( void*)&(( struct
Cyc_Core_Opt*) _check_null( _temp279->topt))->v)), 0); _LL404: _temp403=
_temp399.f1; goto _LL402; _LL402: _temp401= _temp399.f2; goto _LL400; _LL400:*
_temp286=({ struct Cyc_Core_Opt* _temp405=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp405->v=( void*) _temp401; _temp405;}); if(
_temp398 != 0? ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp287->topt))->v,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp398))->v):
0){ Cyc_Tcutil_terr( _temp125->loc, _tag_arr("type of pattern does not match declared type",
sizeof( unsigned char), 45u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp287->topt))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp279->topt))->v)){ Cyc_Tcutil_terr( _temp125->loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp406)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp411= _tag_arr("pattern type %s does not match definition type %s",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp413;
_temp413.tag= Cyc_Stdio_String_pa; _temp413.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp287->topt))->v);{ void*
_temp409=( void*)& _temp413; struct Cyc_Stdio_String_pa_struct _temp412;
_temp412.tag= Cyc_Stdio_String_pa; _temp412.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp279->topt))->v);{ void*
_temp410=( void*)& _temp412; void* _temp407[ 2u]={ _temp409, _temp410}; struct
_tagged_arr _temp408={( void*) _temp407,( void*) _temp407,( void*)( _temp407 + 2u)};
_temp406( _temp411, _temp408);}}}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp287);}* _temp278= Cyc_Tcpat_check_let_pat_exhaustive( _temp287->loc,
_temp287); Cyc_Tcstmt_pattern_synth( s0->loc, te, _temp403, _temp401, _temp123,
0, 0); return;}} _LL255: { void* _temp414= Cyc_Tcenv_curr_rgn( te); struct Cyc_Tcenv_Tenv*
_temp415= te; for( 0; _temp289 != 0; _temp289=(( struct Cyc_List_List*)
_check_null( _temp289))->tl){ struct Cyc_Absyn_Vardecl* _temp416=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp289))->hd; struct Cyc_Absyn_Vardecl
_temp419; struct Cyc_Core_Opt* _temp420; struct Cyc_Core_Opt** _temp422; void*
_temp423; struct _tuple0* _temp425; struct _tuple0 _temp427; void* _temp428;
struct Cyc_Absyn_Vardecl* _temp417= _temp416; _temp419=* _temp417; _LL426:
_temp425= _temp419.name; _temp427=* _temp425; _LL429: _temp428= _temp427.f1;
goto _LL424; _LL424: _temp423=( void*) _temp419.type; goto _LL421; _LL421:
_temp420= _temp419.rgn; _temp422=&(* _temp417).rgn; goto _LL418; _LL418:*
_temp422=({ struct Cyc_Core_Opt* _temp430=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp430->v=( void*) _temp414; _temp430;});{ void*
_temp431= _temp428; struct Cyc_List_List* _temp441; struct Cyc_List_List*
_temp443; _LL433: if( _temp431 ==( void*) Cyc_Absyn_Loc_n){ goto _LL434;} else{
goto _LL435;} _LL435: if(( unsigned int) _temp431 > 1u?*(( int*) _temp431) ==
Cyc_Absyn_Rel_n: 0){ _LL442: _temp441=(( struct Cyc_Absyn_Rel_n_struct*)
_temp431)->f1; if( _temp441 == 0){ goto _LL436;} else{ goto _LL437;}} else{ goto
_LL437;} _LL437: if(( unsigned int) _temp431 > 1u?*(( int*) _temp431) == Cyc_Absyn_Abs_n:
0){ _LL444: _temp443=(( struct Cyc_Absyn_Abs_n_struct*) _temp431)->f1; goto
_LL438;} else{ goto _LL439;} _LL439: goto _LL440; _LL434: goto _LL432; _LL436:(*
_temp416->name).f1=( void*) Cyc_Absyn_Loc_n; goto _LL432; _LL438:( int) _throw(
Cyc_Tcutil_impos( _tag_arr("tcstmt: Abs_n declaration", sizeof( unsigned char),
26u))); _LL440: Cyc_Tcutil_terr( _temp125->loc, _tag_arr("cannot declare a qualified local variable",
sizeof( unsigned char), 42u)); goto _LL432; _LL432:;} Cyc_Tcutil_check_type( s0->loc,
_temp415, Cyc_Tcenv_lookup_type_vars( _temp415),( void*) Cyc_Absyn_MemKind,
_temp423); _temp415= Cyc_Tcenv_add_local_var( _temp123->loc, _temp415, _temp416);}
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( _temp415, s0), _temp123, 0); return;}
_LL257: unimp_msg_part= _tag_arr("namespace", sizeof( unsigned char), 10u); goto
_LL249; _LL259: unimp_msg_part= _tag_arr("using", sizeof( unsigned char), 6u);
goto _LL249; _LL261: unimp_msg_part= _tag_arr("function", sizeof( unsigned char),
9u); goto _LL249; _LL263: unimp_msg_part= _tag_arr("struct", sizeof(
unsigned char), 7u); goto _LL249; _LL265: unimp_msg_part= _tag_arr("union",
sizeof( unsigned char), 6u); goto _LL249; _LL267: unimp_msg_part= _tag_arr("[x]tunion",
sizeof( unsigned char), 10u); goto _LL249; _LL269: unimp_msg_part= _tag_arr("enum",
sizeof( unsigned char), 5u); goto _LL249; _LL271: unimp_msg_part= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL249; _LL273: unimp_msg_part= _tag_arr("extern \"C\"",
sizeof( unsigned char), 11u); goto _LL249; _LL249:;}( int) _throw( Cyc_Tcutil_impos((
struct _tagged_arr)({ struct _tagged_arr(* _temp445)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp449= _tag_arr("tcStmt: nested %s declarations unimplemented",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp450;
_temp450.tag= Cyc_Stdio_String_pa; _temp450.f1=( struct _tagged_arr)
unimp_msg_part;{ void* _temp448=( void*)& _temp450; void* _temp446[ 1u]={
_temp448}; struct _tagged_arr _temp447={( void*) _temp446,( void*) _temp446,(
void*)( _temp446 + 1u)}; _temp445( _temp449, _temp447);}})));} _LL40:( void*)(
_temp127->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt(( void*)
_temp127->r, _temp127->loc), Cyc_Absyn_skip_stmt( _temp127->loc), _temp127->loc))->r));
if( new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ struct Cyc_Absyn_Vardecl
_temp453; struct Cyc_Core_Opt* _temp454; struct Cyc_Core_Opt** _temp456; void*
_temp457; struct Cyc_Absyn_Vardecl* _temp451= _temp129; _temp453=* _temp451;
_LL458: _temp457=( void*) _temp453.type; goto _LL455; _LL455: _temp454= _temp453.rgn;
_temp456=&(* _temp451).rgn; goto _LL452; _LL452: { void* _temp459= Cyc_Tcenv_curr_rgn(
te); void* rgn_typ=( void*)({ struct Cyc_Absyn_VarType_struct* _temp464=( struct
Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp464[ 0]=({ struct Cyc_Absyn_VarType_struct _temp465; _temp465.tag= Cyc_Absyn_VarType;
_temp465.f1= _temp131; _temp465;}); _temp464;});* _temp456=({ struct Cyc_Core_Opt*
_temp460=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp460->v=( void*) _temp459; _temp460;}); te= Cyc_Tcenv_add_type_vars( s0->loc,
te,({ struct Cyc_List_List* _temp461=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp461->hd=( void*) _temp131; _temp461->tl= 0;
_temp461;})); Cyc_Tcutil_check_type( s0->loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_BoxKind, _temp457); te= Cyc_Tcenv_add_region( te, rgn_typ); if(
! Cyc_Tcutil_unify( _temp457,( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp462=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp462[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp463; _temp463.tag=
Cyc_Absyn_RgnHandleType; _temp463.f1=( void*) rgn_typ; _temp463;}); _temp462;}))){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("region stmt: type of region handle is wrong!",
sizeof( unsigned char), 45u));} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( Cyc_Tcenv_add_local_var(
_temp127->loc, Cyc_Tcenv_enter_try( te), _temp129), s0), _temp127, 0); return;}}
_LL42:( int) _throw( Cyc_Tcutil_impos( _tag_arr("tcStmt: cut is not implemented",
sizeof( unsigned char), 31u))); _LL44:( int) _throw( Cyc_Tcutil_impos( _tag_arr("tcStmt: splice is not implemented",
sizeof( unsigned char), 34u))); _LL4:;}}