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
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _tagged_arr Cyc_Core_string_of_int(
int); extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); struct Cyc_Lineno_Pos{
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
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Printf_ft=
0; static const int Cyc_Absyn_Scanf_ft= 1; static const int Cyc_Absyn_Regparm_att=
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
Cyc_Absyn_Format_att= 3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1;
int f2; int f3; } ; static const int Cyc_Absyn_Carray_mod= 0; static const int
Cyc_Absyn_ConstArray_mod= 0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Pointer_mod= 1; struct
Cyc_Absyn_Pointer_mod_struct{ int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual
f3; } ; static const int Cyc_Absyn_Function_mod= 2; struct Cyc_Absyn_Function_mod_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
static const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
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
19; static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_String_strptrcmp( struct _tagged_arr* s1,
struct _tagged_arr* s2); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat( struct
_tagged_arr, struct _tagged_arr); extern unsigned char Cyc_Tcdecl_Incompatible[
17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base;
struct Cyc_Absyn_Tunionfield* field; } ; extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg0, struct _tagged_arr msg1); struct _tuple3{ void*
f1; int f2; } ; extern struct _tuple3 Cyc_Tcdecl_merge_scope( void* s0, void* s1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern void* Cyc_Tcdecl_merge_binding( void* d0,
void* d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern
struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl( struct
Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct
Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List* f, int* res, struct
_tagged_arr* v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg);
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; struct Cyc_Set_Set; extern unsigned char
Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2); extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*,
struct Cyc_List_List*); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); extern
struct _tagged_arr Cyc_Absynpp_scope2string( void* sc); unsigned char Cyc_Tcdecl_Incompatible[
17u]="\000\000\000\000Incompatible"; void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg1, struct _tagged_arr msg2){ if( msg1 == 0){( int)
_throw(( void*) Cyc_Tcdecl_Incompatible);} else{({ void(* _temp0)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp5= loc; struct _tagged_arr _temp6= _tag_arr("%s %s", sizeof( unsigned char),
6u); struct Cyc_Stdio_String_pa_struct _temp8; _temp8.tag= Cyc_Stdio_String_pa;
_temp8.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( msg1));{
void* _temp3=( void*)& _temp8; struct Cyc_Stdio_String_pa_struct _temp7; _temp7.tag=
Cyc_Stdio_String_pa; _temp7.f1=( struct _tagged_arr) msg2;{ void* _temp4=( void*)&
_temp7; void* _temp1[ 2u]={ _temp3, _temp4}; struct _tagged_arr _temp2={( void*)
_temp1,( void*) _temp1,( void*)( _temp1 + 2u)}; _temp0( _temp5, _temp6, _temp2);}}});}}
static void Cyc_Tcdecl_merge_scope_err( void* s0, void* s1, struct _tagged_arr t,
struct _tagged_arr v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp9)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp16= _tag_arr("%s %s is %s whereas expected scope is %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp20; _temp20.tag=
Cyc_Stdio_String_pa; _temp20.f1=( struct _tagged_arr) t;{ void* _temp12=( void*)&
_temp20; struct Cyc_Stdio_String_pa_struct _temp19; _temp19.tag= Cyc_Stdio_String_pa;
_temp19.f1=( struct _tagged_arr) v;{ void* _temp13=( void*)& _temp19; struct Cyc_Stdio_String_pa_struct
_temp18; _temp18.tag= Cyc_Stdio_String_pa; _temp18.f1=( struct _tagged_arr) Cyc_Absynpp_scope2string(
s1);{ void* _temp14=( void*)& _temp18; struct Cyc_Stdio_String_pa_struct _temp17;
_temp17.tag= Cyc_Stdio_String_pa; _temp17.f1=( struct _tagged_arr) Cyc_Absynpp_scope2string(
s0);{ void* _temp15=( void*)& _temp17; void* _temp10[ 4u]={ _temp12, _temp13,
_temp14, _temp15}; struct _tagged_arr _temp11={( void*) _temp10,( void*) _temp10,(
void*)( _temp10 + 4u)}; _temp9( _temp16, _temp11);}}}}}));} struct _tuple4{ void*
f1; void* f2; } ; struct _tuple3 Cyc_Tcdecl_merge_scope( void* s0, void* s1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){{ struct _tuple4 _temp22=({ struct _tuple4 _temp21;
_temp21.f1= s0; _temp21.f2= s1; _temp21;}); void* _temp42; void* _temp44; void*
_temp46; void* _temp48; void* _temp50; void* _temp52; void* _temp54; void*
_temp56; void* _temp58; void* _temp60; void* _temp62; void* _temp64; _LL24:
_LL45: _temp44= _temp22.f1; if( _temp44 ==( void*) Cyc_Absyn_ExternC){ goto
_LL43;} else{ goto _LL26;} _LL43: _temp42= _temp22.f2; if( _temp42 ==( void*)
Cyc_Absyn_ExternC){ goto _LL25;} else{ goto _LL26;} _LL26: _LL47: _temp46=
_temp22.f1; if( _temp46 ==( void*) Cyc_Absyn_ExternC){ goto _LL27;} else{ goto
_LL28;} _LL28: _LL49: _temp48= _temp22.f2; if( _temp48 ==( void*) Cyc_Absyn_ExternC){
goto _LL29;} else{ goto _LL30;} _LL30: _LL51: _temp50= _temp22.f2; if( _temp50
==( void*) Cyc_Absyn_Extern){ goto _LL31;} else{ goto _LL32;} _LL32: _LL53:
_temp52= _temp22.f1; if( _temp52 ==( void*) Cyc_Absyn_Extern){ goto _LL33;}
else{ goto _LL34;} _LL34: _LL57: _temp56= _temp22.f1; if( _temp56 ==( void*) Cyc_Absyn_Static){
goto _LL55;} else{ goto _LL36;} _LL55: _temp54= _temp22.f2; if( _temp54 ==( void*)
Cyc_Absyn_Static){ goto _LL35;} else{ goto _LL36;} _LL36: _LL61: _temp60=
_temp22.f1; if( _temp60 ==( void*) Cyc_Absyn_Public){ goto _LL59;} else{ goto
_LL38;} _LL59: _temp58= _temp22.f2; if( _temp58 ==( void*) Cyc_Absyn_Public){
goto _LL37;} else{ goto _LL38;} _LL38: _LL65: _temp64= _temp22.f1; if( _temp64
==( void*) Cyc_Absyn_Abstract){ goto _LL63;} else{ goto _LL40;} _LL63: _temp62=
_temp22.f2; if( _temp62 ==( void*) Cyc_Absyn_Abstract){ goto _LL39;} else{ goto
_LL40;} _LL40: goto _LL41; _LL25: goto _LL23; _LL27: goto _LL29; _LL29: Cyc_Tcdecl_merge_scope_err(
s0, s1, t, v, loc, msg); return({ struct _tuple3 _temp66; _temp66.f1= s1;
_temp66.f2= 0; _temp66;}); _LL31: s1= s0; goto _LL23; _LL33: goto _LL23; _LL35:
goto _LL23; _LL37: goto _LL23; _LL39: goto _LL23; _LL41: Cyc_Tcdecl_merge_scope_err(
s0, s1, t, v, loc, msg); return({ struct _tuple3 _temp67; _temp67.f1= s1;
_temp67.f2= 0; _temp67;}); _LL23:;} return({ struct _tuple3 _temp68; _temp68.f1=
s1; _temp68.f2= 1; _temp68;});} static int Cyc_Tcdecl_check_type( void* t0, void*
t1){ return Cyc_Tcutil_unify( t0, t1);} static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp* e){ void* _temp69=( void*) e->r; void* _temp75; int
_temp77; _LL71: if(*(( int*) _temp69) == Cyc_Absyn_Const_e){ _LL76: _temp75=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp69)->f1; if(( unsigned int)
_temp75 > 1u?*(( int*) _temp75) == Cyc_Absyn_Int_c: 0){ _LL78: _temp77=(( struct
Cyc_Absyn_Int_c_struct*) _temp75)->f2; goto _LL72;} else{ goto _LL73;}} else{
goto _LL73;} _LL73: goto _LL74; _LL72: return( unsigned int) _temp77; _LL74:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp79=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp79[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp80; _temp80.tag= Cyc_Core_InvalidArg; _temp80.f1=
_tag_arr("Tcdecl::get_uint_const_value", sizeof( unsigned char), 29u); _temp80;});
_temp79;})); _LL70:;} inline static int Cyc_Tcdecl_check_tvs( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1, struct _tagged_arr t, struct _tagged_arr v,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( tvs0) !=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr(* _temp81)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp86= _tag_arr("%s %s has a different number of type parameters",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp88; _temp88.tag=
Cyc_Stdio_String_pa; _temp88.f1=( struct _tagged_arr) t;{ void* _temp84=( void*)&
_temp88; struct Cyc_Stdio_String_pa_struct _temp87; _temp87.tag= Cyc_Stdio_String_pa;
_temp87.f1=( struct _tagged_arr) v;{ void* _temp85=( void*)& _temp87; void*
_temp82[ 2u]={ _temp84, _temp85}; struct _tagged_arr _temp83={( void*) _temp82,(
void*) _temp82,( void*)( _temp82 + 2u)}; _temp81( _temp86, _temp83);}}}));
return 0;}{ struct Cyc_List_List* _temp89= tvs0; struct Cyc_List_List* _temp90=
tvs1; for( 0; _temp89 != 0;( _temp89=(( struct Cyc_List_List*) _check_null(
_temp89))->tl, _temp90=(( struct Cyc_List_List*) _check_null( _temp90))->tl)){
struct Cyc_Absyn_Conref* _temp91= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp89))->hd)->kind); struct Cyc_Absyn_Conref*
_temp92= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp90))->hd)->kind); if( Cyc_Absyn_conref_val( _temp91) != Cyc_Absyn_conref_val(
_temp92)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(*
_temp93)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp101= _tag_arr("%s %s has a different kind (%s) for type parameter %s (%s)",
sizeof( unsigned char), 59u); struct Cyc_Stdio_String_pa_struct _temp106;
_temp106.tag= Cyc_Stdio_String_pa; _temp106.f1=( struct _tagged_arr) t;{ void*
_temp96=( void*)& _temp106; struct Cyc_Stdio_String_pa_struct _temp105; _temp105.tag=
Cyc_Stdio_String_pa; _temp105.f1=( struct _tagged_arr) v;{ void* _temp97=( void*)&
_temp105; struct Cyc_Stdio_String_pa_struct _temp104; _temp104.tag= Cyc_Stdio_String_pa;
_temp104.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( _temp91);{ void*
_temp98=( void*)& _temp104; struct Cyc_Stdio_String_pa_struct _temp103; _temp103.tag=
Cyc_Stdio_String_pa; _temp103.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp89))->hd)->name;{ void* _temp99=( void*)&
_temp103; struct Cyc_Stdio_String_pa_struct _temp102; _temp102.tag= Cyc_Stdio_String_pa;
_temp102.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( _temp92);{ void*
_temp100=( void*)& _temp102; void* _temp94[ 5u]={ _temp96, _temp97, _temp98,
_temp99, _temp100}; struct _tagged_arr _temp95={( void*) _temp94,( void*)
_temp94,( void*)( _temp94 + 5u)}; _temp93( _temp101, _temp95);}}}}}})); return 0;}}
return 1;}} inline static int Cyc_Tcdecl_check_atts( struct Cyc_List_List* atts0,
struct Cyc_List_List* atts1, struct _tagged_arr t, struct _tagged_arr v, struct
Cyc_Position_Segment* loc, struct _tagged_arr* msg){ if( ! Cyc_Tcutil_same_atts(
atts0, atts1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp107)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp112= _tag_arr("%s %s has different attributes", sizeof(
unsigned char), 31u); struct Cyc_Stdio_String_pa_struct _temp114; _temp114.tag=
Cyc_Stdio_String_pa; _temp114.f1=( struct _tagged_arr) t;{ void* _temp110=( void*)&
_temp114; struct Cyc_Stdio_String_pa_struct _temp113; _temp113.tag= Cyc_Stdio_String_pa;
_temp113.f1=( struct _tagged_arr) v;{ void* _temp111=( void*)& _temp113; void*
_temp108[ 2u]={ _temp110, _temp111}; struct _tagged_arr _temp109={( void*)
_temp108,( void*) _temp108,( void*)( _temp108 + 2u)}; _temp107( _temp112,
_temp109);}}})); return 0;} else{ return 1;}} struct _tuple5{ struct Cyc_Absyn_Tvar*
f1; void* f2; } ; inline static struct Cyc_List_List* Cyc_Tcdecl_build_tvs_map(
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1){ struct Cyc_List_List*
_temp115= 0; for( 0; tvs0 != 0;( tvs0=(( struct Cyc_List_List*) _check_null(
tvs0))->tl, tvs1=(( struct Cyc_List_List*) _check_null( tvs1))->tl)){ _temp115=({
struct Cyc_List_List* _temp116=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp116->hd=( void*)({ struct _tuple5* _temp117=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp117->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs1))->hd; _temp117->f2=( void*)({ struct
Cyc_Absyn_VarType_struct* _temp118=( struct Cyc_Absyn_VarType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_VarType_struct)); _temp118[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp119; _temp119.tag= Cyc_Absyn_VarType; _temp119.f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs0))->hd; _temp119;}); _temp118;});
_temp117;}); _temp116->tl= _temp115; _temp116;});} return _temp115;} struct
_tuple6{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_struct_or_union_decl( struct Cyc_Absyn_Structdecl* d0, struct
Cyc_Absyn_Structdecl* d1, struct _tagged_arr t, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct _tagged_arr _temp120= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d0->name))->v); int
_temp121= 1; if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs, t, _temp120, loc, msg)){
return 0;}{ int _temp124; void* _temp126; struct _tuple3 _temp122= Cyc_Tcdecl_merge_scope((
void*) d0->sc,( void*) d1->sc, t, _temp120, loc, msg); _LL127: _temp126=
_temp122.f1; goto _LL125; _LL125: _temp124= _temp122.f2; goto _LL123; _LL123:
if( ! _temp124){ _temp121= 0;} if( ! Cyc_Tcdecl_check_atts( d0->attributes, d1->attributes,
t, _temp120, loc, msg)){ _temp121= 0;}{ struct Cyc_Absyn_Structdecl* d2;{ struct
_tuple6 _temp129=({ struct _tuple6 _temp128; _temp128.f1= d0->fields; _temp128.f2=
d1->fields; _temp128;}); struct Cyc_Core_Opt* _temp137; struct Cyc_Core_Opt*
_temp139; struct Cyc_Core_Opt* _temp141; struct Cyc_Core_Opt _temp143; struct
Cyc_List_List* _temp144; struct Cyc_Core_Opt* _temp146; struct Cyc_Core_Opt
_temp148; struct Cyc_List_List* _temp149; _LL131: _LL138: _temp137= _temp129.f2;
if( _temp137 == 0){ goto _LL132;} else{ goto _LL133;} _LL133: _LL140: _temp139=
_temp129.f1; if( _temp139 == 0){ goto _LL134;} else{ goto _LL135;} _LL135:
_LL147: _temp146= _temp129.f1; if( _temp146 == 0){ goto _LL130;} else{ _temp148=*
_temp146; _LL150: _temp149=( struct Cyc_List_List*) _temp148.v; goto _LL142;}
_LL142: _temp141= _temp129.f2; if( _temp141 == 0){ goto _LL130;} else{ _temp143=*
_temp141; _LL145: _temp144=( struct Cyc_List_List*) _temp143.v; goto _LL136;}
_LL132: d2= d0; goto _LL130; _LL134: d2= d1; goto _LL130; _LL136: { struct Cyc_List_List*
_temp151= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); for( 0; _temp149 != 0?
_temp144 != 0: 0;( _temp149=(( struct Cyc_List_List*) _check_null( _temp149))->tl,
_temp144=(( struct Cyc_List_List*) _check_null( _temp144))->tl)){ struct Cyc_Absyn_Structfield
_temp154; struct Cyc_List_List* _temp155; struct Cyc_Core_Opt* _temp157; void*
_temp159; struct Cyc_Absyn_Tqual _temp161; struct _tagged_arr* _temp163; struct
Cyc_Absyn_Structfield* _temp152=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp149))->hd; _temp154=* _temp152; _LL164: _temp163= _temp154.name;
goto _LL162; _LL162: _temp161= _temp154.tq; goto _LL160; _LL160: _temp159=( void*)
_temp154.type; goto _LL158; _LL158: _temp157= _temp154.width; goto _LL156;
_LL156: _temp155= _temp154.attributes; goto _LL153; _LL153: { struct Cyc_Absyn_Structfield
_temp167; struct Cyc_List_List* _temp168; struct Cyc_Core_Opt* _temp170; void*
_temp172; struct Cyc_Absyn_Tqual _temp174; struct _tagged_arr* _temp176; struct
Cyc_Absyn_Structfield* _temp165=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp144))->hd; _temp167=* _temp165; _LL177: _temp176= _temp167.name;
goto _LL175; _LL175: _temp174= _temp167.tq; goto _LL173; _LL173: _temp172=( void*)
_temp167.type; goto _LL171; _LL171: _temp170= _temp167.width; goto _LL169;
_LL169: _temp168= _temp167.attributes; goto _LL166; _LL166: if( Cyc_String_zstrptrcmp(
_temp163, _temp176) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr(* _temp178)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp185= _tag_arr("%s %s : field name mismatch %s != %s",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp189;
_temp189.tag= Cyc_Stdio_String_pa; _temp189.f1=( struct _tagged_arr) t;{ void*
_temp181=( void*)& _temp189; struct Cyc_Stdio_String_pa_struct _temp188;
_temp188.tag= Cyc_Stdio_String_pa; _temp188.f1=( struct _tagged_arr) _temp120;{
void* _temp182=( void*)& _temp188; struct Cyc_Stdio_String_pa_struct _temp187;
_temp187.tag= Cyc_Stdio_String_pa; _temp187.f1=( struct _tagged_arr)* _temp163;{
void* _temp183=( void*)& _temp187; struct Cyc_Stdio_String_pa_struct _temp186;
_temp186.tag= Cyc_Stdio_String_pa; _temp186.f1=( struct _tagged_arr)* _temp176;{
void* _temp184=( void*)& _temp186; void* _temp179[ 4u]={ _temp181, _temp182,
_temp183, _temp184}; struct _tagged_arr _temp180={( void*) _temp179,( void*)
_temp179,( void*)( _temp179 + 4u)}; _temp178( _temp185, _temp180);}}}}}));
return 0;} if( ! Cyc_Tcutil_same_atts( _temp155, _temp168)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp190)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp196= _tag_arr("%s %s : attribute mismatch on field %s", sizeof(
unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp199; _temp199.tag=
Cyc_Stdio_String_pa; _temp199.f1=( struct _tagged_arr) t;{ void* _temp193=( void*)&
_temp199; struct Cyc_Stdio_String_pa_struct _temp198; _temp198.tag= Cyc_Stdio_String_pa;
_temp198.f1=( struct _tagged_arr) _temp120;{ void* _temp194=( void*)& _temp198;
struct Cyc_Stdio_String_pa_struct _temp197; _temp197.tag= Cyc_Stdio_String_pa;
_temp197.f1=( struct _tagged_arr)* _temp163;{ void* _temp195=( void*)& _temp197;
void* _temp191[ 3u]={ _temp193, _temp194, _temp195}; struct _tagged_arr _temp192={(
void*) _temp191,( void*) _temp191,( void*)( _temp191 + 3u)}; _temp190( _temp196,
_temp192);}}}})); _temp121= 0;} if( ! Cyc_Tcutil_equal_tqual( _temp161, _temp174)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp200)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp206= _tag_arr("%s %s : qualifier mismatch on field %s", sizeof(
unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp209; _temp209.tag=
Cyc_Stdio_String_pa; _temp209.f1=( struct _tagged_arr) t;{ void* _temp203=( void*)&
_temp209; struct Cyc_Stdio_String_pa_struct _temp208; _temp208.tag= Cyc_Stdio_String_pa;
_temp208.f1=( struct _tagged_arr) _temp120;{ void* _temp204=( void*)& _temp208;
struct Cyc_Stdio_String_pa_struct _temp207; _temp207.tag= Cyc_Stdio_String_pa;
_temp207.f1=( struct _tagged_arr)* _temp163;{ void* _temp205=( void*)& _temp207;
void* _temp201[ 3u]={ _temp203, _temp204, _temp205}; struct _tagged_arr _temp202={(
void*) _temp201,( void*) _temp201,( void*)( _temp201 + 3u)}; _temp200( _temp206,
_temp202);}}}})); _temp121= 0;} if(((( _temp157 != 0? _temp170 != 0: 0)? Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp157))->v) !=
Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp170))->v): 0)? 1:( _temp157 == 0? _temp170 != 0: 0))? 1:(
_temp157 != 0? _temp170 == 0: 0)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr(* _temp210)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp216= _tag_arr("%s %s : bitfield mismatch on field %s",
sizeof( unsigned char), 38u); struct Cyc_Stdio_String_pa_struct _temp219;
_temp219.tag= Cyc_Stdio_String_pa; _temp219.f1=( struct _tagged_arr) t;{ void*
_temp213=( void*)& _temp219; struct Cyc_Stdio_String_pa_struct _temp218;
_temp218.tag= Cyc_Stdio_String_pa; _temp218.f1=( struct _tagged_arr) _temp120;{
void* _temp214=( void*)& _temp218; struct Cyc_Stdio_String_pa_struct _temp217;
_temp217.tag= Cyc_Stdio_String_pa; _temp217.f1=( struct _tagged_arr)* _temp163;{
void* _temp215=( void*)& _temp217; void* _temp211[ 3u]={ _temp213, _temp214,
_temp215}; struct _tagged_arr _temp212={( void*) _temp211,( void*) _temp211,(
void*)( _temp211 + 3u)}; _temp210( _temp216, _temp212);}}}})); _temp121= 0;}{
void* subst_t1= Cyc_Tcutil_substitute( _temp151, _temp172); if( ! Cyc_Tcdecl_check_type(
_temp159, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp220)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp228= _tag_arr("%s %s : type mismatch on field %s: %s != %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp233;
_temp233.tag= Cyc_Stdio_String_pa; _temp233.f1=( struct _tagged_arr) t;{ void*
_temp223=( void*)& _temp233; struct Cyc_Stdio_String_pa_struct _temp232;
_temp232.tag= Cyc_Stdio_String_pa; _temp232.f1=( struct _tagged_arr) _temp120;{
void* _temp224=( void*)& _temp232; struct Cyc_Stdio_String_pa_struct _temp231;
_temp231.tag= Cyc_Stdio_String_pa; _temp231.f1=( struct _tagged_arr)* _temp163;{
void* _temp225=( void*)& _temp231; struct Cyc_Stdio_String_pa_struct _temp230;
_temp230.tag= Cyc_Stdio_String_pa; _temp230.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp159);{ void* _temp226=( void*)& _temp230; struct Cyc_Stdio_String_pa_struct
_temp229; _temp229.tag= Cyc_Stdio_String_pa; _temp229.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( subst_t1);{ void* _temp227=( void*)& _temp229; void*
_temp221[ 5u]={ _temp223, _temp224, _temp225, _temp226, _temp227}; struct
_tagged_arr _temp222={( void*) _temp221,( void*) _temp221,( void*)( _temp221 + 5u)};
_temp220( _temp228, _temp222);}}}}}})); _temp121= 0;}}}} if( _temp149 != 0){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp234)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp240= _tag_arr("%s %s is missing field %s", sizeof( unsigned char), 26u);
struct Cyc_Stdio_String_pa_struct _temp243; _temp243.tag= Cyc_Stdio_String_pa;
_temp243.f1=( struct _tagged_arr) t;{ void* _temp237=( void*)& _temp243; struct
Cyc_Stdio_String_pa_struct _temp242; _temp242.tag= Cyc_Stdio_String_pa; _temp242.f1=(
struct _tagged_arr) _temp120;{ void* _temp238=( void*)& _temp242; struct Cyc_Stdio_String_pa_struct
_temp241; _temp241.tag= Cyc_Stdio_String_pa; _temp241.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp149))->hd)->name;{
void* _temp239=( void*)& _temp241; void* _temp235[ 3u]={ _temp237, _temp238,
_temp239}; struct _tagged_arr _temp236={( void*) _temp235,( void*) _temp235,(
void*)( _temp235 + 3u)}; _temp234( _temp240, _temp236);}}}})); _temp121= 0;} if(
_temp144 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp244)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp250= _tag_arr("%s %s has extra field %s", sizeof(
unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp253; _temp253.tag=
Cyc_Stdio_String_pa; _temp253.f1=( struct _tagged_arr) t;{ void* _temp247=( void*)&
_temp253; struct Cyc_Stdio_String_pa_struct _temp252; _temp252.tag= Cyc_Stdio_String_pa;
_temp252.f1=( struct _tagged_arr) _temp120;{ void* _temp248=( void*)& _temp252;
struct Cyc_Stdio_String_pa_struct _temp251; _temp251.tag= Cyc_Stdio_String_pa;
_temp251.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp144))->hd)->name;{ void* _temp249=( void*)& _temp251; void*
_temp245[ 3u]={ _temp247, _temp248, _temp249}; struct _tagged_arr _temp246={(
void*) _temp245,( void*) _temp245,( void*)( _temp245 + 3u)}; _temp244( _temp250,
_temp246);}}}})); _temp121= 0;} d2= d0; goto _LL130;} _LL130:;} if( ! _temp121){
return 0;} if( _temp126 ==( void*) d2->sc){ return( struct Cyc_Absyn_Structdecl*)
d2;} else{ d2=({ struct Cyc_Absyn_Structdecl* _temp254=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp254[ 0]=* d2; _temp254;});(
void*)( d2->sc=( void*) _temp126); return( struct Cyc_Absyn_Structdecl*) d2;}}}}
struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg){ return Cyc_Tcdecl_merge_struct_or_union_decl( d0, d1,
_tag_arr("struct", sizeof( unsigned char), 7u), loc, msg);} struct Cyc_Absyn_Uniondecl*
Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ return( struct
Cyc_Absyn_Uniondecl*) Cyc_Tcdecl_merge_struct_or_union_decl(( struct Cyc_Absyn_Structdecl*)
d0,( struct Cyc_Absyn_Structdecl*) d1, _tag_arr("union", sizeof( unsigned char),
6u), loc, msg);} inline static struct _tagged_arr Cyc_Tcdecl_is_x2string( int
is_x){ if( is_x){ return _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{
return _tag_arr("tunion", sizeof( unsigned char), 7u);}} struct _tuple7{ struct
Cyc_Absyn_Tqual f1; void* f2; } ; static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_merge_tunionfield(
struct Cyc_Absyn_Tunionfield* f0, struct Cyc_Absyn_Tunionfield* f1, struct Cyc_List_List*
inst, struct _tagged_arr t, struct _tagged_arr v, struct _tagged_arr* msg){
struct Cyc_Position_Segment* loc= f1->loc; if( Cyc_String_zstrptrcmp((* f0->name).f2,(*
f1->name).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp255)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp262= _tag_arr("%s %s: field name mismatch %s != %s",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp266;
_temp266.tag= Cyc_Stdio_String_pa; _temp266.f1=( struct _tagged_arr) t;{ void*
_temp258=( void*)& _temp266; struct Cyc_Stdio_String_pa_struct _temp265;
_temp265.tag= Cyc_Stdio_String_pa; _temp265.f1=( struct _tagged_arr) v;{ void*
_temp259=( void*)& _temp265; struct Cyc_Stdio_String_pa_struct _temp264;
_temp264.tag= Cyc_Stdio_String_pa; _temp264.f1=( struct _tagged_arr)*(* f1->name).f2;{
void* _temp260=( void*)& _temp264; struct Cyc_Stdio_String_pa_struct _temp263;
_temp263.tag= Cyc_Stdio_String_pa; _temp263.f1=( struct _tagged_arr)*(* f0->name).f2;{
void* _temp261=( void*)& _temp263; void* _temp256[ 4u]={ _temp258, _temp259,
_temp260, _temp261}; struct _tagged_arr _temp257={( void*) _temp256,( void*)
_temp256,( void*)( _temp256 + 4u)}; _temp255( _temp262, _temp257);}}}}}));
return 0;}{ struct _tagged_arr _temp267=*(* f0->name).f2; int _temp278; void*
_temp280; struct _tuple3 _temp276= Cyc_Tcdecl_merge_scope(( void*) f0->sc,( void*)
f1->sc,( struct _tagged_arr)({ struct _tagged_arr(* _temp268)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp273= _tag_arr("in %s %s, field", sizeof( unsigned char), 16u); struct Cyc_Stdio_String_pa_struct
_temp275; _temp275.tag= Cyc_Stdio_String_pa; _temp275.f1=( struct _tagged_arr) t;{
void* _temp271=( void*)& _temp275; struct Cyc_Stdio_String_pa_struct _temp274;
_temp274.tag= Cyc_Stdio_String_pa; _temp274.f1=( struct _tagged_arr) v;{ void*
_temp272=( void*)& _temp274; void* _temp269[ 2u]={ _temp271, _temp272}; struct
_tagged_arr _temp270={( void*) _temp269,( void*) _temp269,( void*)( _temp269 + 2u)};
_temp268( _temp273, _temp270);}}}), _temp267, loc, msg); _LL281: _temp280=
_temp276.f1; goto _LL279; _LL279: _temp278= _temp276.f2; goto _LL277; _LL277: {
struct Cyc_List_List* _temp282= f0->tvs; struct Cyc_List_List* _temp283= f1->tvs;
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp282) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp283)){ Cyc_Tcdecl_merr( loc,
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp284)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp290=
_tag_arr("%s %s, field %s: type parameter number mismatch", sizeof(
unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp293; _temp293.tag=
Cyc_Stdio_String_pa; _temp293.f1=( struct _tagged_arr) t;{ void* _temp287=( void*)&
_temp293; struct Cyc_Stdio_String_pa_struct _temp292; _temp292.tag= Cyc_Stdio_String_pa;
_temp292.f1=( struct _tagged_arr) v;{ void* _temp288=( void*)& _temp292; struct
Cyc_Stdio_String_pa_struct _temp291; _temp291.tag= Cyc_Stdio_String_pa; _temp291.f1=(
struct _tagged_arr) _temp267;{ void* _temp289=( void*)& _temp291; void* _temp285[
3u]={ _temp287, _temp288, _temp289}; struct _tagged_arr _temp286={( void*)
_temp285,( void*) _temp285,( void*)( _temp285 + 3u)}; _temp284( _temp290,
_temp286);}}}})); return 0;}{ struct Cyc_List_List* _temp294= Cyc_Tcdecl_build_tvs_map(
_temp282, _temp283); struct Cyc_List_List* _temp295= f0->typs; struct Cyc_List_List*
_temp296= f1->typs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp295) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp296)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp297)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp303= _tag_arr("%s %s, field %s: parameter number mismatch",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp306;
_temp306.tag= Cyc_Stdio_String_pa; _temp306.f1=( struct _tagged_arr) t;{ void*
_temp300=( void*)& _temp306; struct Cyc_Stdio_String_pa_struct _temp305;
_temp305.tag= Cyc_Stdio_String_pa; _temp305.f1=( struct _tagged_arr) v;{ void*
_temp301=( void*)& _temp305; struct Cyc_Stdio_String_pa_struct _temp304;
_temp304.tag= Cyc_Stdio_String_pa; _temp304.f1=( struct _tagged_arr) _temp267;{
void* _temp302=( void*)& _temp304; void* _temp298[ 3u]={ _temp300, _temp301,
_temp302}; struct _tagged_arr _temp299={( void*) _temp298,( void*) _temp298,(
void*)( _temp298 + 3u)}; _temp297( _temp303, _temp299);}}}})); _temp278= 0;}
for( 0; _temp295 != 0;( _temp295=(( struct Cyc_List_List*) _check_null( _temp295))->tl,
_temp296=(( struct Cyc_List_List*) _check_null( _temp296))->tl)){ if( ! Cyc_Tcutil_equal_tqual((*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp295))->hd)).f1,(*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp296))->hd)).f1)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp307)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp313= _tag_arr("%s %s, field %s: parameter qualifier", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp316; _temp316.tag=
Cyc_Stdio_String_pa; _temp316.f1=( struct _tagged_arr) t;{ void* _temp310=( void*)&
_temp316; struct Cyc_Stdio_String_pa_struct _temp315; _temp315.tag= Cyc_Stdio_String_pa;
_temp315.f1=( struct _tagged_arr) v;{ void* _temp311=( void*)& _temp315; struct
Cyc_Stdio_String_pa_struct _temp314; _temp314.tag= Cyc_Stdio_String_pa; _temp314.f1=(
struct _tagged_arr) _temp267;{ void* _temp312=( void*)& _temp314; void* _temp308[
3u]={ _temp310, _temp311, _temp312}; struct _tagged_arr _temp309={( void*)
_temp308,( void*) _temp308,( void*)( _temp308 + 3u)}; _temp307( _temp313,
_temp309);}}}})); _temp278= 0;}{ void* subst_t1= Cyc_Tcutil_substitute((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp294, inst),(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null(
_temp296))->hd)).f2); if( ! Cyc_Tcdecl_check_type((*(( struct _tuple7*)(( struct
Cyc_List_List*) _check_null( _temp295))->hd)).f2, subst_t1)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp317)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp325= _tag_arr("%s %s, field %s: parameter type mismatch %s != %s", sizeof(
unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp330; _temp330.tag=
Cyc_Stdio_String_pa; _temp330.f1=( struct _tagged_arr) t;{ void* _temp320=( void*)&
_temp330; struct Cyc_Stdio_String_pa_struct _temp329; _temp329.tag= Cyc_Stdio_String_pa;
_temp329.f1=( struct _tagged_arr) v;{ void* _temp321=( void*)& _temp329; struct
Cyc_Stdio_String_pa_struct _temp328; _temp328.tag= Cyc_Stdio_String_pa; _temp328.f1=(
struct _tagged_arr) _temp267;{ void* _temp322=( void*)& _temp328; struct Cyc_Stdio_String_pa_struct
_temp327; _temp327.tag= Cyc_Stdio_String_pa; _temp327.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string((*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp295))->hd)).f2);{ void* _temp323=( void*)& _temp327; struct
Cyc_Stdio_String_pa_struct _temp326; _temp326.tag= Cyc_Stdio_String_pa; _temp326.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( subst_t1);{ void* _temp324=( void*)&
_temp326; void* _temp318[ 5u]={ _temp320, _temp321, _temp322, _temp323, _temp324};
struct _tagged_arr _temp319={( void*) _temp318,( void*) _temp318,( void*)(
_temp318 + 5u)}; _temp317( _temp325, _temp319);}}}}}})); _temp278= 0;}}} if( !
_temp278){ return 0;} if(( void*) f0->sc != _temp280){ struct Cyc_Absyn_Tunionfield*
_temp331=({ struct Cyc_Absyn_Tunionfield* _temp332=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp332[ 0]=* f0; _temp332;});(
void*)((( struct Cyc_Absyn_Tunionfield*) _check_null( _temp331))->sc=( void*)
_temp280); return _temp331;} else{ return( struct Cyc_Absyn_Tunionfield*) f0;}}}}}
static int Cyc_Tcdecl_substitute_tunionfield_f1( struct _tagged_arr* name,
struct Cyc_Absyn_Tvar* x){ return Cyc_String_strptrcmp( x->name, name) == 0;}
static struct _tuple7* Cyc_Tcdecl_substitute_tunionfield_f2( struct Cyc_List_List*
inst, struct _tuple7* x){ struct _tuple7 _temp335; void* _temp336; struct Cyc_Absyn_Tqual
_temp338; struct _tuple7* _temp333= x; _temp335=* _temp333; _LL339: _temp338=
_temp335.f1; goto _LL337; _LL337: _temp336= _temp335.f2; goto _LL334; _LL334:
return({ struct _tuple7* _temp340=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7)); _temp340->f1= _temp338; _temp340->f2= Cyc_Tcutil_substitute( inst,
_temp336); _temp340;});} static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List* d0tvs, struct Cyc_List_List* d1tvs, struct Cyc_List_List*
inst1, struct Cyc_Absyn_Tunionfield* f1){ struct Cyc_Absyn_Tunionfield* _temp341=({
struct Cyc_Absyn_Tunionfield* _temp349=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp349[ 0]=* f1; _temp349;});
struct Cyc_List_List* _temp342= 0;{ struct Cyc_List_List* _temp343= f1->tvs;
for( 0; _temp343 != 0; _temp343=(( struct Cyc_List_List*) _check_null( _temp343))->tl){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp343))->hd; if((( int(*)( int(* pred)( struct _tagged_arr*,
struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcdecl_substitute_tunionfield_f1, tv->name, d0tvs)? 1:(( int(*)( int(* pred)(
struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1, tv->name,
d1tvs)){ int i= 0; struct _tagged_arr base=* tv->name; struct _tagged_arr*
new_name; do { new_name=({ struct _tagged_arr* _temp344=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp344[ 0]=( struct _tagged_arr) Cyc_String_strconcat(
base,( struct _tagged_arr) Cyc_Core_string_of_int( i)); _temp344;}); i ++;} while (((
int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct
_tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
new_name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)); tv=({ struct Cyc_Absyn_Tvar* _temp345=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp345[ 0]=* tv; _temp345;}); tv->name=
new_name;} _temp342=({ struct Cyc_List_List* _temp346=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp346->hd=( void*) tv; _temp346->tl=
_temp342; _temp346;});}} _temp342=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp342); _temp341->tvs= _temp342;{ struct Cyc_List_List*
_temp347= Cyc_Tcdecl_build_tvs_map( _temp342, f1->tvs); struct Cyc_List_List*
_temp348=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp347, inst1); _temp341->typs=(( struct Cyc_List_List*(*)(
struct _tuple7*(* f)( struct Cyc_List_List*, struct _tuple7*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcdecl_substitute_tunionfield_f2,
_temp348, f1->typs); return _temp341;}} static struct Cyc_List_List* Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1, int* res, int* incl,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp358=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp358[ 0]= 0; _temp358;}); struct Cyc_List_List** _temp350= f2sp; int cmp= -
1; for( 0; f0s != 0? f1s != 0: 0; f1s=(( struct Cyc_List_List*) _check_null( f1s))->tl){
while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)->name,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd)->name)) < 0: 0) { struct Cyc_List_List*
_temp351=({ struct Cyc_List_List* _temp352=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp352->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd); _temp352->tl= 0; _temp352;});*
_temp350= _temp351; _temp350=&(( struct Cyc_List_List*) _check_null( _temp351))->tl;
f0s=(( struct Cyc_List_List*) _check_null( f0s))->tl;} if( f0s == 0? 1: cmp > 0){*
incl= 0;{ struct Cyc_List_List* _temp353=({ struct Cyc_List_List* _temp354=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp354->hd=(
void*) Cyc_Tcdecl_substitute_tunionfield( tvs0, tvs1, inst,( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd); _temp354->tl= 0; _temp354;});*
_temp350= _temp353; _temp350=&(( struct Cyc_List_List*) _check_null( _temp353))->tl;}}
else{ struct Cyc_Absyn_Tunionfield* _temp355= Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f0s))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f1s))->hd,
inst, t, v, msg); if( _temp355 != 0){ if( _temp355 !=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f0s))->hd)){*
incl= 0;}{ struct Cyc_List_List* _temp356=({ struct Cyc_List_List* _temp357=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp357->hd=(
void*)(( struct Cyc_Absyn_Tunionfield*) _check_null( _temp355)); _temp357->tl= 0;
_temp357;});* _temp350= _temp356; _temp350=&(( struct Cyc_List_List*)
_check_null( _temp356))->tl;}} else{* res= 0;} f0s=(( struct Cyc_List_List*)
_check_null( f0s))->tl;}} if( f1s != 0){* incl= 0;* _temp350= f1s;} else{*
_temp350= f0s;} return* f2sp;} struct _tuple8{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; } ; static struct _tuple8 Cyc_Tcdecl_split( struct Cyc_List_List*
f){ if( f == 0){ return({ struct _tuple8 _temp359; _temp359.f1= 0; _temp359.f2=
0; _temp359;});} if((( struct Cyc_List_List*) _check_null( f))->tl == 0){ return({
struct _tuple8 _temp360; _temp360.f1= f; _temp360.f2= 0; _temp360;});}{ struct
Cyc_List_List* _temp363; struct Cyc_List_List* _temp365; struct _tuple8 _temp361=
Cyc_Tcdecl_split((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( f))->tl))->tl); _LL366: _temp365= _temp361.f1; goto _LL364; _LL364:
_temp363= _temp361.f2; goto _LL362; _LL362: return({ struct _tuple8 _temp367;
_temp367.f1=({ struct Cyc_List_List* _temp369=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp369->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( f))->hd); _temp369->tl= _temp365; _temp369;}); _temp367.f2=({
struct Cyc_List_List* _temp368=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp368->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( f))->tl))->hd); _temp368->tl=
_temp363; _temp368;}); _temp367;});}} struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List* f, int* res, struct _tagged_arr* v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct Cyc_List_List* _temp372; struct Cyc_List_List*
_temp374; struct _tuple8 _temp370=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL375: _temp374= _temp370.f1; goto _LL373; _LL373:
_temp372= _temp370.f2; goto _LL371; _LL371: if( _temp374 != 0?(( struct Cyc_List_List*)
_check_null( _temp374))->tl != 0: 0){ _temp374= Cyc_Tcdecl_sort_xtunion_fields(
_temp374, res, v, loc, msg);} if( _temp372 != 0?(( struct Cyc_List_List*)
_check_null( _temp372))->tl != 0: 0){ _temp372= Cyc_Tcdecl_sort_xtunion_fields(
_temp372, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp374,
_temp372, 0, 0, 0, res,({ int* _temp376=( int*) GC_malloc_atomic( sizeof( int));
_temp376[ 0]= 1; _temp376;}), _tag_arr("xtunion", sizeof( unsigned char), 8u),*
v, loc, msg);} struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl( struct
Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp377= Cyc_Absynpp_qvar2string(
d0->name); struct _tagged_arr t= _tag_arr("[x]tunion", sizeof( unsigned char),
10u); int _temp378= 1; if( d0->is_xtunion != d1->is_xtunion){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp379)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp385= _tag_arr("expected %s to be a %s instead of a %s", sizeof(
unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp388; _temp388.tag=
Cyc_Stdio_String_pa; _temp388.f1=( struct _tagged_arr) _temp377;{ void* _temp382=(
void*)& _temp388; struct Cyc_Stdio_String_pa_struct _temp387; _temp387.tag= Cyc_Stdio_String_pa;
_temp387.f1=( struct _tagged_arr) Cyc_Tcdecl_is_x2string( d0->is_xtunion);{ void*
_temp383=( void*)& _temp387; struct Cyc_Stdio_String_pa_struct _temp386;
_temp386.tag= Cyc_Stdio_String_pa; _temp386.f1=( struct _tagged_arr) Cyc_Tcdecl_is_x2string(
d1->is_xtunion);{ void* _temp384=( void*)& _temp386; void* _temp380[ 3u]={
_temp382, _temp383, _temp384}; struct _tagged_arr _temp381={( void*) _temp380,(
void*) _temp380,( void*)( _temp380 + 3u)}; _temp379( _temp385, _temp381);}}}}));
_temp378= 0;} else{ t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, _temp377, loc, msg)){ return 0;}{ int _temp391; void*
_temp393; struct _tuple3 _temp389= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*)
d1->sc, t, _temp377, loc, msg); _LL394: _temp393= _temp389.f1; goto _LL392;
_LL392: _temp391= _temp389.f2; goto _LL390; _LL390: if( ! _temp391){ _temp378= 0;}{
struct Cyc_Absyn_Tuniondecl* d2;{ struct _tuple6 _temp396=({ struct _tuple6
_temp395; _temp395.f1= d0->fields; _temp395.f2= d1->fields; _temp395;}); struct
Cyc_Core_Opt* _temp404; struct Cyc_Core_Opt* _temp406; struct Cyc_Core_Opt*
_temp408; struct Cyc_Core_Opt _temp410; struct Cyc_List_List* _temp411; struct
Cyc_Core_Opt* _temp413; struct Cyc_Core_Opt _temp415; struct Cyc_List_List*
_temp416; _LL398: _LL405: _temp404= _temp396.f2; if( _temp404 == 0){ goto _LL399;}
else{ goto _LL400;} _LL400: _LL407: _temp406= _temp396.f1; if( _temp406 == 0){
goto _LL401;} else{ goto _LL402;} _LL402: _LL414: _temp413= _temp396.f1; if(
_temp413 == 0){ goto _LL397;} else{ _temp415=* _temp413; _LL417: _temp416=(
struct Cyc_List_List*) _temp415.v; goto _LL409;} _LL409: _temp408= _temp396.f2;
if( _temp408 == 0){ goto _LL397;} else{ _temp410=* _temp408; _LL412: _temp411=(
struct Cyc_List_List*) _temp410.v; goto _LL403;} _LL399: d2= d0; goto _LL397;
_LL401: d2= d1; goto _LL397; _LL403: { struct Cyc_List_List* _temp418= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); if( d0->is_xtunion){ int _temp419= 1; struct Cyc_List_List*
_temp420= Cyc_Tcdecl_merge_xtunion_fields( _temp416, _temp411, _temp418, d0->tvs,
d1->tvs,& _temp378,& _temp419, t, _temp377, loc, msg); if( _temp419){ d2= d0;}
else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp421=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp421[ 0]=* d0; _temp421;});(
void*)( d2->sc=( void*) _temp393); d2->fields=({ struct Cyc_Core_Opt* _temp422=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp422->v=(
void*) _temp420; _temp422;});}} else{ for( 0; _temp416 != 0? _temp411 != 0: 0;(
_temp416=(( struct Cyc_List_List*) _check_null( _temp416))->tl, _temp411=((
struct Cyc_List_List*) _check_null( _temp411))->tl)){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp416))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp411))->hd,
_temp418, t, _temp377, msg);} if( _temp416 != 0){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct _tagged_arr(* _temp423)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp429= _tag_arr("%s %s has extra field %s",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp432;
_temp432.tag= Cyc_Stdio_String_pa; _temp432.f1=( struct _tagged_arr) t;{ void*
_temp426=( void*)& _temp432; struct Cyc_Stdio_String_pa_struct _temp431;
_temp431.tag= Cyc_Stdio_String_pa; _temp431.f1=( struct _tagged_arr) _temp377;{
void* _temp427=( void*)& _temp431; struct Cyc_Stdio_String_pa_struct _temp430;
_temp430.tag= Cyc_Stdio_String_pa; _temp430.f1=( struct _tagged_arr)*(*(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp411))->hd)->name).f2;{
void* _temp428=( void*)& _temp430; void* _temp424[ 3u]={ _temp426, _temp427,
_temp428}; struct _tagged_arr _temp425={( void*) _temp424,( void*) _temp424,(
void*)( _temp424 + 3u)}; _temp423( _temp429, _temp425);}}}})); _temp378= 0;} if(
_temp411 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp433)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp439= _tag_arr("%s %s is missing field %s", sizeof(
unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp442; _temp442.tag=
Cyc_Stdio_String_pa; _temp442.f1=( struct _tagged_arr) t;{ void* _temp436=( void*)&
_temp442; struct Cyc_Stdio_String_pa_struct _temp441; _temp441.tag= Cyc_Stdio_String_pa;
_temp441.f1=( struct _tagged_arr) _temp377;{ void* _temp437=( void*)& _temp441;
struct Cyc_Stdio_String_pa_struct _temp440; _temp440.tag= Cyc_Stdio_String_pa;
_temp440.f1=( struct _tagged_arr)*(*(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp411))->hd)->name).f2;{ void* _temp438=( void*)&
_temp440; void* _temp434[ 3u]={ _temp436, _temp437, _temp438}; struct
_tagged_arr _temp435={( void*) _temp434,( void*) _temp434,( void*)( _temp434 + 3u)};
_temp433( _temp439, _temp435);}}}})); _temp378= 0;} d2= d0;} goto _LL397;}
_LL397:;} if( ! _temp378){ return 0;} if( _temp393 ==( void*) d2->sc){ return(
struct Cyc_Absyn_Tuniondecl*) d2;} else{ d2=({ struct Cyc_Absyn_Tuniondecl*
_temp443=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp443[ 0]=* d2; _temp443;});( void*)( d2->sc=( void*) _temp393); return(
struct Cyc_Absyn_Tuniondecl*) d2;}}}} struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp444= Cyc_Absynpp_qvar2string(
d0->name); int _temp445= 1; int _temp448; void* _temp450; struct _tuple3
_temp446= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc, _tag_arr("enum",
sizeof( unsigned char), 5u), _temp444, loc, msg); _LL451: _temp450= _temp446.f1;
goto _LL449; _LL449: _temp448= _temp446.f2; goto _LL447; _LL447: if( ! _temp448){
_temp445= 0;}{ struct Cyc_Absyn_Enumdecl* d2;{ struct _tuple6 _temp453=({ struct
_tuple6 _temp452; _temp452.f1= d0->fields; _temp452.f2= d1->fields; _temp452;});
struct Cyc_Core_Opt* _temp461; struct Cyc_Core_Opt* _temp463; struct Cyc_Core_Opt*
_temp465; struct Cyc_Core_Opt _temp467; struct Cyc_List_List* _temp468; struct
Cyc_Core_Opt* _temp470; struct Cyc_Core_Opt _temp472; struct Cyc_List_List*
_temp473; _LL455: _LL462: _temp461= _temp453.f2; if( _temp461 == 0){ goto _LL456;}
else{ goto _LL457;} _LL457: _LL464: _temp463= _temp453.f1; if( _temp463 == 0){
goto _LL458;} else{ goto _LL459;} _LL459: _LL471: _temp470= _temp453.f1; if(
_temp470 == 0){ goto _LL454;} else{ _temp472=* _temp470; _LL474: _temp473=(
struct Cyc_List_List*) _temp472.v; goto _LL466;} _LL466: _temp465= _temp453.f2;
if( _temp465 == 0){ goto _LL454;} else{ _temp467=* _temp465; _LL469: _temp468=(
struct Cyc_List_List*) _temp467.v; goto _LL460;} _LL456: d2= d0; goto _LL454;
_LL458: d2= d1; goto _LL454; _LL460: for( 0; _temp473 != 0? _temp468 != 0: 0;(
_temp473=(( struct Cyc_List_List*) _check_null( _temp473))->tl, _temp468=((
struct Cyc_List_List*) _check_null( _temp468))->tl)){ struct Cyc_Absyn_Enumfield
_temp477; struct Cyc_Position_Segment* _temp478; struct Cyc_Absyn_Exp* _temp480;
struct _tuple0* _temp482; struct Cyc_Absyn_Enumfield* _temp475=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp473))->hd; _temp477=* _temp475; _LL483:
_temp482= _temp477.name; goto _LL481; _LL481: _temp480= _temp477.tag; goto
_LL479; _LL479: _temp478= _temp477.loc; goto _LL476; _LL476: { struct Cyc_Absyn_Enumfield
_temp486; struct Cyc_Position_Segment* _temp487; struct Cyc_Absyn_Exp* _temp489;
struct _tuple0* _temp491; struct Cyc_Absyn_Enumfield* _temp484=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp468))->hd; _temp486=* _temp484; _LL492:
_temp491= _temp486.name; goto _LL490; _LL490: _temp489= _temp486.tag; goto
_LL488; _LL488: _temp487= _temp486.loc; goto _LL485; _LL485: if( Cyc_String_zstrptrcmp((*
_temp491).f2,(* _temp482).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr(* _temp493)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp499= _tag_arr("enum %s: field name mismatch %s != %s",
sizeof( unsigned char), 38u); struct Cyc_Stdio_String_pa_struct _temp502;
_temp502.tag= Cyc_Stdio_String_pa; _temp502.f1=( struct _tagged_arr) _temp444;{
void* _temp496=( void*)& _temp502; struct Cyc_Stdio_String_pa_struct _temp501;
_temp501.tag= Cyc_Stdio_String_pa; _temp501.f1=( struct _tagged_arr)*(* _temp491).f2;{
void* _temp497=( void*)& _temp501; struct Cyc_Stdio_String_pa_struct _temp500;
_temp500.tag= Cyc_Stdio_String_pa; _temp500.f1=( struct _tagged_arr)*(* _temp482).f2;{
void* _temp498=( void*)& _temp500; void* _temp494[ 3u]={ _temp496, _temp497,
_temp498}; struct _tagged_arr _temp495={( void*) _temp494,( void*) _temp494,(
void*)( _temp494 + 3u)}; _temp493( _temp499, _temp495);}}}})); _temp445= 0;} if(
Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*) _check_null( _temp480))
!= Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*) _check_null(
_temp489))){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp503)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp508= _tag_arr("enum %s, field %s, value mismatch",
sizeof( unsigned char), 34u); struct Cyc_Stdio_String_pa_struct _temp510;
_temp510.tag= Cyc_Stdio_String_pa; _temp510.f1=( struct _tagged_arr) _temp444;{
void* _temp506=( void*)& _temp510; struct Cyc_Stdio_String_pa_struct _temp509;
_temp509.tag= Cyc_Stdio_String_pa; _temp509.f1=( struct _tagged_arr)*(* _temp491).f2;{
void* _temp507=( void*)& _temp509; void* _temp504[ 2u]={ _temp506, _temp507};
struct _tagged_arr _temp505={( void*) _temp504,( void*) _temp504,( void*)(
_temp504 + 2u)}; _temp503( _temp508, _temp505);}}})); _temp445= 0;}}} d2= d0;
goto _LL454; _LL454:;} if( ! _temp445){ return 0;} if(( void*) d2->sc ==
_temp450){ return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({ struct Cyc_Absyn_Enumdecl*
_temp511=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp511[ 0]=* d2; _temp511;});( void*)( d2->sc=( void*) _temp450); return(
struct Cyc_Absyn_Enumdecl*) d2;}}} inline static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void* sc0, void* t0, struct Cyc_Absyn_Tqual tq0, struct Cyc_List_List* atts0,
void* sc1, void* t1, struct Cyc_Absyn_Tqual tq1, struct Cyc_List_List* atts1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ int _temp512= 1; int _temp515; void* _temp517; struct
_tuple3 _temp513= Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg); _LL518:
_temp517= _temp513.f1; goto _LL516; _LL516: _temp515= _temp513.f2; goto _LL514;
_LL514: if( ! _temp515){ _temp512= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp519)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp526= _tag_arr("%s %s has type %s instead of %s", sizeof(
unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp530; _temp530.tag=
Cyc_Stdio_String_pa; _temp530.f1=( struct _tagged_arr) t;{ void* _temp522=( void*)&
_temp530; struct Cyc_Stdio_String_pa_struct _temp529; _temp529.tag= Cyc_Stdio_String_pa;
_temp529.f1=( struct _tagged_arr) v;{ void* _temp523=( void*)& _temp529; struct
Cyc_Stdio_String_pa_struct _temp528; _temp528.tag= Cyc_Stdio_String_pa; _temp528.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp524=( void*)&
_temp528; struct Cyc_Stdio_String_pa_struct _temp527; _temp527.tag= Cyc_Stdio_String_pa;
_temp527.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t0);{ void* _temp525=(
void*)& _temp527; void* _temp520[ 4u]={ _temp522, _temp523, _temp524, _temp525};
struct _tagged_arr _temp521={( void*) _temp520,( void*) _temp520,( void*)(
_temp520 + 4u)}; _temp519( _temp526, _temp521);}}}}})); _temp512= 0;} if( ! Cyc_Tcutil_equal_tqual(
tq0, tq1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(*
_temp531)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp536= _tag_arr("%s %s has different type qualifiers",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp538;
_temp538.tag= Cyc_Stdio_String_pa; _temp538.f1=( struct _tagged_arr) t;{ void*
_temp534=( void*)& _temp538; struct Cyc_Stdio_String_pa_struct _temp537;
_temp537.tag= Cyc_Stdio_String_pa; _temp537.f1=( struct _tagged_arr) v;{ void*
_temp535=( void*)& _temp537; void* _temp532[ 2u]={ _temp534, _temp535}; struct
_tagged_arr _temp533={( void*) _temp532,( void*) _temp532,( void*)( _temp532 + 2u)};
_temp531( _temp536, _temp533);}}})); _temp512= 0;} if( ! Cyc_Tcutil_same_atts(
atts0, atts1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp539)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp544= _tag_arr("%s %s has different attributes", sizeof(
unsigned char), 31u); struct Cyc_Stdio_String_pa_struct _temp546; _temp546.tag=
Cyc_Stdio_String_pa; _temp546.f1=( struct _tagged_arr) t;{ void* _temp542=( void*)&
_temp546; struct Cyc_Stdio_String_pa_struct _temp545; _temp545.tag= Cyc_Stdio_String_pa;
_temp545.f1=( struct _tagged_arr) v;{ void* _temp543=( void*)& _temp545; void*
_temp540[ 2u]={ _temp542, _temp543}; struct _tagged_arr _temp541={( void*)
_temp540,( void*) _temp540,( void*)( _temp540 + 2u)}; _temp539( _temp544,
_temp541);}}}));({ int(* _temp547)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp550= Cyc_Stdio_stderr; struct _tagged_arr _temp551= _tag_arr("previous attributes: ",
sizeof( unsigned char), 22u); void* _temp548[ 0u]={}; struct _tagged_arr
_temp549={( void*) _temp548,( void*) _temp548,( void*)( _temp548 + 0u)};
_temp547( _temp550, _temp551, _temp549);}); for( 0; atts0 != 0; atts0=(( struct
Cyc_List_List*) _check_null( atts0))->tl){({ int(* _temp552)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp556= Cyc_Stdio_stderr; struct _tagged_arr _temp557= _tag_arr("%s ", sizeof(
unsigned char), 4u); struct Cyc_Stdio_String_pa_struct _temp558; _temp558.tag=
Cyc_Stdio_String_pa; _temp558.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts0))->hd);{ void* _temp555=(
void*)& _temp558; void* _temp553[ 1u]={ _temp555}; struct _tagged_arr _temp554={(
void*) _temp553,( void*) _temp553,( void*)( _temp553 + 1u)}; _temp552( _temp556,
_temp557, _temp554);}});}({ int(* _temp559)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp562= Cyc_Stdio_stderr; struct _tagged_arr _temp563= _tag_arr("\ncurrent attributes: ",
sizeof( unsigned char), 22u); void* _temp560[ 0u]={}; struct _tagged_arr
_temp561={( void*) _temp560,( void*) _temp560,( void*)( _temp560 + 0u)};
_temp559( _temp562, _temp563, _temp561);}); for( 0; atts1 != 0; atts1=(( struct
Cyc_List_List*) _check_null( atts1))->tl){({ int(* _temp564)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp568= Cyc_Stdio_stderr; struct _tagged_arr _temp569= _tag_arr("%s ", sizeof(
unsigned char), 4u); struct Cyc_Stdio_String_pa_struct _temp570; _temp570.tag=
Cyc_Stdio_String_pa; _temp570.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts1))->hd);{ void* _temp567=(
void*)& _temp570; void* _temp565[ 1u]={ _temp567}; struct _tagged_arr _temp566={(
void*) _temp565,( void*) _temp565,( void*)( _temp565 + 1u)}; _temp564( _temp568,
_temp569, _temp566);}});}({ int(* _temp571)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp574= Cyc_Stdio_stderr; struct _tagged_arr _temp575= _tag_arr("\n", sizeof(
unsigned char), 2u); void* _temp572[ 0u]={}; struct _tagged_arr _temp573={( void*)
_temp572,( void*) _temp572,( void*)( _temp572 + 0u)}; _temp571( _temp574,
_temp575, _temp573);}); _temp512= 0;} return({ struct _tuple3 _temp576; _temp576.f1=
_temp517; _temp576.f2= _temp512; _temp576;});} struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp577= Cyc_Absynpp_qvar2string(
d0->name); int _temp580; void* _temp582; struct _tuple3 _temp578= Cyc_Tcdecl_check_var_or_fn_decl((
void*) d0->sc,( void*) d0->type, d0->tq, d0->attributes,( void*) d1->sc,( void*)
d1->type, d1->tq, d1->attributes, _tag_arr("variable", sizeof( unsigned char), 9u),
_temp577, loc, msg); _LL583: _temp582= _temp578.f1; goto _LL581; _LL581:
_temp580= _temp578.f2; goto _LL579; _LL579: if( ! _temp580){ return 0;} if((
void*) d0->sc == _temp582){ return( struct Cyc_Absyn_Vardecl*) d0;} else{ struct
Cyc_Absyn_Vardecl* _temp584=({ struct Cyc_Absyn_Vardecl* _temp585=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp585[ 0]=* d0; _temp585;});(
void*)((( struct Cyc_Absyn_Vardecl*) _check_null( _temp584))->sc=( void*)
_temp582); return _temp584;}} struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp586= Cyc_Absynpp_qvar2string(
d0->name); if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs, _tag_arr("typedef",
sizeof( unsigned char), 8u), _temp586, loc, msg)){ return 0;}{ struct Cyc_List_List*
_temp587= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); void* subst_defn1= Cyc_Tcutil_substitute(
_temp587,( void*) d1->defn); if( ! Cyc_Tcdecl_check_type(( void*) d0->defn,
subst_defn1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp588)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp594= _tag_arr("typedef %s does not refer to the same type: %s != %s",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp597;
_temp597.tag= Cyc_Stdio_String_pa; _temp597.f1=( struct _tagged_arr) _temp586;{
void* _temp591=( void*)& _temp597; struct Cyc_Stdio_String_pa_struct _temp596;
_temp596.tag= Cyc_Stdio_String_pa; _temp596.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
subst_defn1);{ void* _temp592=( void*)& _temp596; struct Cyc_Stdio_String_pa_struct
_temp595; _temp595.tag= Cyc_Stdio_String_pa; _temp595.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*) d0->defn);{ void* _temp593=( void*)& _temp595;
void* _temp589[ 3u]={ _temp591, _temp592, _temp593}; struct _tagged_arr _temp590={(
void*) _temp589,( void*) _temp589,( void*)( _temp589 + 3u)}; _temp588( _temp594,
_temp590);}}}})); return 0;} return( struct Cyc_Absyn_Typedefdecl*) d0;}} void*
Cyc_Tcdecl_merge_binding( void* b0, void* b1, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct _tuple4 _temp599=({ struct _tuple4 _temp598;
_temp598.f1= b0; _temp598.f2= b1; _temp598;}); void* _temp613; void* _temp615;
void* _temp617; struct Cyc_Absyn_Vardecl* _temp619; void* _temp621; struct Cyc_Absyn_Vardecl*
_temp623; void* _temp625; struct Cyc_Absyn_Fndecl* _temp627; void* _temp629;
struct Cyc_Absyn_Vardecl* _temp631; void* _temp633; struct Cyc_Absyn_Fndecl*
_temp635; void* _temp637; void* _temp639; struct Cyc_Absyn_Vardecl* _temp641;
void* _temp643; struct Cyc_Absyn_Fndecl* _temp645; _LL601: _LL616: _temp615=
_temp599.f1; if( _temp615 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL614;}
else{ goto _LL603;} _LL614: _temp613= _temp599.f2; if( _temp613 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL602;} else{ goto _LL603;} _LL603: _LL622: _temp621= _temp599.f1; if((
unsigned int) _temp621 > 1u?*(( int*) _temp621) == Cyc_Absyn_Global_b: 0){
_LL624: _temp623=(( struct Cyc_Absyn_Global_b_struct*) _temp621)->f1; goto
_LL618;} else{ goto _LL605;} _LL618: _temp617= _temp599.f2; if(( unsigned int)
_temp617 > 1u?*(( int*) _temp617) == Cyc_Absyn_Global_b: 0){ _LL620: _temp619=((
struct Cyc_Absyn_Global_b_struct*) _temp617)->f1; goto _LL604;} else{ goto
_LL605;} _LL605: _LL630: _temp629= _temp599.f1; if(( unsigned int) _temp629 > 1u?*((
int*) _temp629) == Cyc_Absyn_Global_b: 0){ _LL632: _temp631=(( struct Cyc_Absyn_Global_b_struct*)
_temp629)->f1; goto _LL626;} else{ goto _LL607;} _LL626: _temp625= _temp599.f2;
if(( unsigned int) _temp625 > 1u?*(( int*) _temp625) == Cyc_Absyn_Funname_b: 0){
_LL628: _temp627=(( struct Cyc_Absyn_Funname_b_struct*) _temp625)->f1; goto
_LL606;} else{ goto _LL607;} _LL607: _LL638: _temp637= _temp599.f1; if((
unsigned int) _temp637 > 1u?*(( int*) _temp637) == Cyc_Absyn_Funname_b: 0){ goto
_LL634;} else{ goto _LL609;} _LL634: _temp633= _temp599.f2; if(( unsigned int)
_temp633 > 1u?*(( int*) _temp633) == Cyc_Absyn_Funname_b: 0){ _LL636: _temp635=((
struct Cyc_Absyn_Funname_b_struct*) _temp633)->f1; goto _LL608;} else{ goto
_LL609;} _LL609: _LL644: _temp643= _temp599.f1; if(( unsigned int) _temp643 > 1u?*((
int*) _temp643) == Cyc_Absyn_Funname_b: 0){ _LL646: _temp645=(( struct Cyc_Absyn_Funname_b_struct*)
_temp643)->f1; goto _LL640;} else{ goto _LL611;} _LL640: _temp639= _temp599.f2;
if(( unsigned int) _temp639 > 1u?*(( int*) _temp639) == Cyc_Absyn_Global_b: 0){
_LL642: _temp641=(( struct Cyc_Absyn_Global_b_struct*) _temp639)->f1; goto
_LL610;} else{ goto _LL611;} _LL611: goto _LL612; _LL602: return( void*) Cyc_Absyn_Unresolved_b;
_LL604: { struct Cyc_Absyn_Vardecl* _temp647= Cyc_Tcdecl_merge_vardecl( _temp623,
_temp619, loc, msg); if( _temp647 == 0){ return( void*) Cyc_Absyn_Unresolved_b;}
if(( struct Cyc_Absyn_Vardecl*) _temp623 == _temp647){ return b0;} else{ if((
struct Cyc_Absyn_Vardecl*) _temp619 == _temp647){ return b1;} else{ return( void*)({
struct Cyc_Absyn_Global_b_struct* _temp648=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp648[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp649; _temp649.tag= Cyc_Absyn_Global_b; _temp649.f1=(
struct Cyc_Absyn_Vardecl*) _check_null( _temp647); _temp649;}); _temp648;});}}}
_LL606: { int _temp652; struct _tuple3 _temp650= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp631->sc,( void*) _temp631->type, _temp631->tq, _temp631->attributes,(
void*) _temp627->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp627->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp627->attributes, _tag_arr("function", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp631->name), loc, msg); _LL653:
_temp652= _temp650.f2; goto _LL651; _LL651: if( ! _temp652){ return( void*) Cyc_Absyn_Unresolved_b;}
return b1;} _LL608: Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp654)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp658= _tag_arr("redefinition of function %s", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp659; _temp659.tag=
Cyc_Stdio_String_pa; _temp659.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp635->name);{ void* _temp657=( void*)& _temp659; void* _temp655[ 1u]={
_temp657}; struct _tagged_arr _temp656={( void*) _temp655,( void*) _temp655,(
void*)( _temp655 + 1u)}; _temp654( _temp658, _temp656);}})); return( void*) Cyc_Absyn_Unresolved_b;
_LL610: { int _temp662; struct _tuple3 _temp660= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp645->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp645->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp645->attributes,( void*) _temp641->sc,( void*)
_temp641->type, _temp641->tq, _temp641->attributes, _tag_arr("variable", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp645->name), loc, msg); _LL663:
_temp662= _temp660.f2; goto _LL661; _LL661: if( ! _temp662){ return( void*) Cyc_Absyn_Unresolved_b;}
return b0;} _LL612:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp664=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp664[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp665; _temp665.tag= Cyc_Core_InvalidArg;
_temp665.f1= _tag_arr("Tcdecl::merge_binding", sizeof( unsigned char), 22u);
_temp665;}); _temp664;})); _LL600:;} struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl
_temp668; struct Cyc_Absyn_Tunionfield* _temp669; struct Cyc_Absyn_Tuniondecl*
_temp671; struct Cyc_Tcdecl_Xtunionfielddecl* _temp666= d0; _temp668=* _temp666;
_LL672: _temp671= _temp668.base; goto _LL670; _LL670: _temp669= _temp668.field;
goto _LL667; _LL667: { struct Cyc_Tcdecl_Xtunionfielddecl _temp675; struct Cyc_Absyn_Tunionfield*
_temp676; struct Cyc_Absyn_Tuniondecl* _temp678; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp673= d1; _temp675=* _temp673; _LL679: _temp678= _temp675.base; goto _LL677;
_LL677: _temp676= _temp675.field; goto _LL674; _LL674: { struct _tagged_arr
_temp680= Cyc_Absynpp_qvar2string( _temp669->name); if( Cyc_Absyn_qvar_cmp(
_temp671->name, _temp678->name) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr(* _temp681)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp687= _tag_arr("xtunions %s and %s have both a field named %s",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp690;
_temp690.tag= Cyc_Stdio_String_pa; _temp690.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp671->name);{ void* _temp684=( void*)& _temp690; struct Cyc_Stdio_String_pa_struct
_temp689; _temp689.tag= Cyc_Stdio_String_pa; _temp689.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp678->name);{ void* _temp685=( void*)& _temp689;
struct Cyc_Stdio_String_pa_struct _temp688; _temp688.tag= Cyc_Stdio_String_pa;
_temp688.f1=( struct _tagged_arr) _temp680;{ void* _temp686=( void*)& _temp688;
void* _temp682[ 3u]={ _temp684, _temp685, _temp686}; struct _tagged_arr _temp683={(
void*) _temp682,( void*) _temp682,( void*)( _temp682 + 3u)}; _temp681( _temp687,
_temp683);}}}})); return 0;} if( ! Cyc_Tcdecl_check_tvs( _temp671->tvs, _temp678->tvs,
_tag_arr("xtunion", sizeof( unsigned char), 8u), Cyc_Absynpp_qvar2string(
_temp671->name), loc, msg)){ return 0;}{ struct Cyc_List_List* _temp691= Cyc_Tcdecl_build_tvs_map(
_temp671->tvs, _temp678->tvs); struct Cyc_Absyn_Tunionfield* _temp692= Cyc_Tcdecl_merge_tunionfield(
_temp669, _temp676, _temp691, _tag_arr("xtunionfield", sizeof( unsigned char),
13u), _temp680, msg); if( _temp692 == 0){ return 0;} if( _temp692 ==( struct Cyc_Absyn_Tunionfield*)
_temp669){ return( struct Cyc_Tcdecl_Xtunionfielddecl*) d0;} else{ return({
struct Cyc_Tcdecl_Xtunionfielddecl* _temp693=( struct Cyc_Tcdecl_Xtunionfielddecl*)
GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp693->base=
_temp671; _temp693->field=( struct Cyc_Absyn_Tunionfield*) _check_null( _temp692);
_temp693;});}}}}}
