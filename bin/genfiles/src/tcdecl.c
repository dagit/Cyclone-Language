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
_throw(( void*) Cyc_Tcdecl_Incompatible);} else{({ struct Cyc_Stdio_String_pa_struct
_temp2; _temp2.tag= Cyc_Stdio_String_pa; _temp2.f1=( struct _tagged_arr) msg2;{
struct Cyc_Stdio_String_pa_struct _temp1; _temp1.tag= Cyc_Stdio_String_pa;
_temp1.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( msg1));{
void* _temp0[ 2u]={& _temp1,& _temp2}; Cyc_Tcutil_terr( loc, _tag_arr("%s %s",
sizeof( unsigned char), 6u), _tag_arr( _temp0, sizeof( void*), 2u));}}});}}
static void Cyc_Tcdecl_merge_scope_err( void* s0, void* s1, struct _tagged_arr t,
struct _tagged_arr v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp7; _temp7.tag= Cyc_Stdio_String_pa; _temp7.f1=( struct _tagged_arr) Cyc_Absynpp_scope2string(
s0);{ struct Cyc_Stdio_String_pa_struct _temp6; _temp6.tag= Cyc_Stdio_String_pa;
_temp6.f1=( struct _tagged_arr) Cyc_Absynpp_scope2string( s1);{ struct Cyc_Stdio_String_pa_struct
_temp5; _temp5.tag= Cyc_Stdio_String_pa; _temp5.f1=( struct _tagged_arr) v;{
struct Cyc_Stdio_String_pa_struct _temp4; _temp4.tag= Cyc_Stdio_String_pa;
_temp4.f1=( struct _tagged_arr) t;{ void* _temp3[ 4u]={& _temp4,& _temp5,&
_temp6,& _temp7}; Cyc_Stdio_aprintf( _tag_arr("%s %s is %s whereas expected scope is %s",
sizeof( unsigned char), 41u), _tag_arr( _temp3, sizeof( void*), 4u));}}}}}));}
struct _tuple4{ void* f1; void* f2; } ; struct _tuple3 Cyc_Tcdecl_merge_scope(
void* s0, void* s1, struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){{ struct _tuple4 _temp9=({ struct _tuple4 _temp8;
_temp8.f1= s0; _temp8.f2= s1; _temp8;}); void* _temp29; void* _temp31; void*
_temp33; void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; void* _temp51; _LL11:
_LL32: _temp31= _temp9.f1; if( _temp31 ==( void*) Cyc_Absyn_ExternC){ goto _LL30;}
else{ goto _LL13;} _LL30: _temp29= _temp9.f2; if( _temp29 ==( void*) Cyc_Absyn_ExternC){
goto _LL12;} else{ goto _LL13;} _LL13: _LL34: _temp33= _temp9.f1; if( _temp33 ==(
void*) Cyc_Absyn_ExternC){ goto _LL14;} else{ goto _LL15;} _LL15: _LL36: _temp35=
_temp9.f2; if( _temp35 ==( void*) Cyc_Absyn_ExternC){ goto _LL16;} else{ goto
_LL17;} _LL17: _LL38: _temp37= _temp9.f2; if( _temp37 ==( void*) Cyc_Absyn_Extern){
goto _LL18;} else{ goto _LL19;} _LL19: _LL40: _temp39= _temp9.f1; if( _temp39 ==(
void*) Cyc_Absyn_Extern){ goto _LL20;} else{ goto _LL21;} _LL21: _LL44: _temp43=
_temp9.f1; if( _temp43 ==( void*) Cyc_Absyn_Static){ goto _LL42;} else{ goto
_LL23;} _LL42: _temp41= _temp9.f2; if( _temp41 ==( void*) Cyc_Absyn_Static){
goto _LL22;} else{ goto _LL23;} _LL23: _LL48: _temp47= _temp9.f1; if( _temp47 ==(
void*) Cyc_Absyn_Public){ goto _LL46;} else{ goto _LL25;} _LL46: _temp45= _temp9.f2;
if( _temp45 ==( void*) Cyc_Absyn_Public){ goto _LL24;} else{ goto _LL25;} _LL25:
_LL52: _temp51= _temp9.f1; if( _temp51 ==( void*) Cyc_Absyn_Abstract){ goto
_LL50;} else{ goto _LL27;} _LL50: _temp49= _temp9.f2; if( _temp49 ==( void*) Cyc_Absyn_Abstract){
goto _LL26;} else{ goto _LL27;} _LL27: goto _LL28; _LL12: goto _LL10; _LL14:
goto _LL16; _LL16: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp53; _temp53.f1= s1; _temp53.f2= 0; _temp53;}); _LL18: s1= s0;
goto _LL10; _LL20: goto _LL10; _LL22: goto _LL10; _LL24: goto _LL10; _LL26: goto
_LL10; _LL28: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp54; _temp54.f1= s1; _temp54.f2= 0; _temp54;}); _LL10:;}
return({ struct _tuple3 _temp55; _temp55.f1= s1; _temp55.f2= 1; _temp55;});}
static int Cyc_Tcdecl_check_type( void* t0, void* t1){ return Cyc_Tcutil_unify(
t0, t1);} static unsigned int Cyc_Tcdecl_get_uint_const_value( struct Cyc_Absyn_Exp*
e){ void* _temp56=( void*) e->r; void* _temp62; int _temp64; _LL58: if(*(( int*)
_temp56) == Cyc_Absyn_Const_e){ _LL63: _temp62=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp56)->f1; if(( unsigned int) _temp62 > 1u?*(( int*) _temp62) == Cyc_Absyn_Int_c:
0){ _LL65: _temp64=(( struct Cyc_Absyn_Int_c_struct*) _temp62)->f2; goto _LL59;}
else{ goto _LL60;}} else{ goto _LL60;} _LL60: goto _LL61; _LL59: return(
unsigned int) _temp64; _LL61:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp66=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp66[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp67; _temp67.tag= Cyc_Core_InvalidArg;
_temp67.f1= _tag_arr("Tcdecl::get_uint_const_value", sizeof( unsigned char), 29u);
_temp67;}); _temp66;})); _LL57:;} inline static int Cyc_Tcdecl_check_tvs( struct
Cyc_List_List* tvs0, struct Cyc_List_List* tvs1, struct _tagged_arr t, struct
_tagged_arr v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ if(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs0) !=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( tvs1)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp70; _temp70.tag= Cyc_Stdio_String_pa;
_temp70.f1=( struct _tagged_arr) v;{ struct Cyc_Stdio_String_pa_struct _temp69;
_temp69.tag= Cyc_Stdio_String_pa; _temp69.f1=( struct _tagged_arr) t;{ void*
_temp68[ 2u]={& _temp69,& _temp70}; Cyc_Stdio_aprintf( _tag_arr("%s %s has a different number of type parameters",
sizeof( unsigned char), 48u), _tag_arr( _temp68, sizeof( void*), 2u));}}}));
return 0;}{ struct Cyc_List_List* _temp71= tvs0; struct Cyc_List_List* _temp72=
tvs1; for( 0; _temp71 != 0;( _temp71=(( struct Cyc_List_List*) _check_null(
_temp71))->tl, _temp72=(( struct Cyc_List_List*) _check_null( _temp72))->tl)){
struct Cyc_Absyn_Conref* _temp73= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp71))->hd)->kind); struct Cyc_Absyn_Conref*
_temp74= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp72))->hd)->kind); if( Cyc_Absyn_conref_val( _temp73) != Cyc_Absyn_conref_val(
_temp74)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp80; _temp80.tag= Cyc_Stdio_String_pa; _temp80.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(
_temp74);{ struct Cyc_Stdio_String_pa_struct _temp79; _temp79.tag= Cyc_Stdio_String_pa;
_temp79.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp71))->hd)->name;{ struct Cyc_Stdio_String_pa_struct _temp78;
_temp78.tag= Cyc_Stdio_String_pa; _temp78.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(
_temp73);{ struct Cyc_Stdio_String_pa_struct _temp77; _temp77.tag= Cyc_Stdio_String_pa;
_temp77.f1=( struct _tagged_arr) v;{ struct Cyc_Stdio_String_pa_struct _temp76;
_temp76.tag= Cyc_Stdio_String_pa; _temp76.f1=( struct _tagged_arr) t;{ void*
_temp75[ 5u]={& _temp76,& _temp77,& _temp78,& _temp79,& _temp80}; Cyc_Stdio_aprintf(
_tag_arr("%s %s has a different kind (%s) for type parameter %s (%s)", sizeof(
unsigned char), 59u), _tag_arr( _temp75, sizeof( void*), 5u));}}}}}})); return 0;}}
return 1;}} inline static int Cyc_Tcdecl_check_atts( struct Cyc_List_List* atts0,
struct Cyc_List_List* atts1, struct _tagged_arr t, struct _tagged_arr v, struct
Cyc_Position_Segment* loc, struct _tagged_arr* msg){ if( ! Cyc_Tcutil_same_atts(
atts0, atts1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp83; _temp83.tag= Cyc_Stdio_String_pa; _temp83.f1=( struct _tagged_arr) v;{
struct Cyc_Stdio_String_pa_struct _temp82; _temp82.tag= Cyc_Stdio_String_pa;
_temp82.f1=( struct _tagged_arr) t;{ void* _temp81[ 2u]={& _temp82,& _temp83};
Cyc_Stdio_aprintf( _tag_arr("%s %s has different attributes", sizeof(
unsigned char), 31u), _tag_arr( _temp81, sizeof( void*), 2u));}}})); return 0;}
else{ return 1;}} struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
inline static struct Cyc_List_List* Cyc_Tcdecl_build_tvs_map( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1){ struct Cyc_List_List* _temp84= 0; for( 0;
tvs0 != 0;( tvs0=(( struct Cyc_List_List*) _check_null( tvs0))->tl, tvs1=((
struct Cyc_List_List*) _check_null( tvs1))->tl)){ _temp84=({ struct Cyc_List_List*
_temp85=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp85->hd=( void*)({ struct _tuple5* _temp86=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp86->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs1))->hd; _temp86->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp87=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp87[ 0]=({ struct Cyc_Absyn_VarType_struct _temp88; _temp88.tag= Cyc_Absyn_VarType;
_temp88.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs0))->hd;
_temp88;}); _temp87;}); _temp86;}); _temp85->tl= _temp84; _temp85;});} return
_temp84;} struct _tuple6{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
static struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_struct_or_union_decl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct
_tagged_arr t, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
struct _tagged_arr _temp89= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( d0->name))->v); int _temp90= 1; if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, _temp89, loc, msg)){ return 0;}{ int _temp93; void* _temp95;
struct _tuple3 _temp91= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc,
t, _temp89, loc, msg); _LL96: _temp95= _temp91.f1; goto _LL94; _LL94: _temp93=
_temp91.f2; goto _LL92; _LL92: if( ! _temp93){ _temp90= 0;} if( ! Cyc_Tcdecl_check_atts(
d0->attributes, d1->attributes, t, _temp89, loc, msg)){ _temp90= 0;}{ struct Cyc_Absyn_Structdecl*
d2;{ struct _tuple6 _temp98=({ struct _tuple6 _temp97; _temp97.f1= d0->fields;
_temp97.f2= d1->fields; _temp97;}); struct Cyc_Core_Opt* _temp106; struct Cyc_Core_Opt*
_temp108; struct Cyc_Core_Opt* _temp110; struct Cyc_Core_Opt _temp112; struct
Cyc_List_List* _temp113; struct Cyc_Core_Opt* _temp115; struct Cyc_Core_Opt
_temp117; struct Cyc_List_List* _temp118; _LL100: _LL107: _temp106= _temp98.f2;
if( _temp106 == 0){ goto _LL101;} else{ goto _LL102;} _LL102: _LL109: _temp108=
_temp98.f1; if( _temp108 == 0){ goto _LL103;} else{ goto _LL104;} _LL104: _LL116:
_temp115= _temp98.f1; if( _temp115 == 0){ goto _LL99;} else{ _temp117=* _temp115;
_LL119: _temp118=( struct Cyc_List_List*) _temp117.v; goto _LL111;} _LL111:
_temp110= _temp98.f2; if( _temp110 == 0){ goto _LL99;} else{ _temp112=* _temp110;
_LL114: _temp113=( struct Cyc_List_List*) _temp112.v; goto _LL105;} _LL101: d2=
d0; goto _LL99; _LL103: d2= d1; goto _LL99; _LL105: { struct Cyc_List_List*
_temp120= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); for( 0; _temp118 != 0?
_temp113 != 0: 0;( _temp118=(( struct Cyc_List_List*) _check_null( _temp118))->tl,
_temp113=(( struct Cyc_List_List*) _check_null( _temp113))->tl)){ struct Cyc_Absyn_Structfield
_temp123; struct Cyc_List_List* _temp124; struct Cyc_Core_Opt* _temp126; void*
_temp128; struct Cyc_Absyn_Tqual _temp130; struct _tagged_arr* _temp132; struct
Cyc_Absyn_Structfield* _temp121=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp118))->hd; _temp123=* _temp121; _LL133: _temp132= _temp123.name;
goto _LL131; _LL131: _temp130= _temp123.tq; goto _LL129; _LL129: _temp128=( void*)
_temp123.type; goto _LL127; _LL127: _temp126= _temp123.width; goto _LL125;
_LL125: _temp124= _temp123.attributes; goto _LL122; _LL122: { struct Cyc_Absyn_Structfield
_temp136; struct Cyc_List_List* _temp137; struct Cyc_Core_Opt* _temp139; void*
_temp141; struct Cyc_Absyn_Tqual _temp143; struct _tagged_arr* _temp145; struct
Cyc_Absyn_Structfield* _temp134=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp113))->hd; _temp136=* _temp134; _LL146: _temp145= _temp136.name;
goto _LL144; _LL144: _temp143= _temp136.tq; goto _LL142; _LL142: _temp141=( void*)
_temp136.type; goto _LL140; _LL140: _temp139= _temp136.width; goto _LL138;
_LL138: _temp137= _temp136.attributes; goto _LL135; _LL135: if( Cyc_String_zstrptrcmp(
_temp132, _temp145) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp151; _temp151.tag= Cyc_Stdio_String_pa;
_temp151.f1=( struct _tagged_arr)* _temp145;{ struct Cyc_Stdio_String_pa_struct
_temp150; _temp150.tag= Cyc_Stdio_String_pa; _temp150.f1=( struct _tagged_arr)*
_temp132;{ struct Cyc_Stdio_String_pa_struct _temp149; _temp149.tag= Cyc_Stdio_String_pa;
_temp149.f1=( struct _tagged_arr) _temp89;{ struct Cyc_Stdio_String_pa_struct
_temp148; _temp148.tag= Cyc_Stdio_String_pa; _temp148.f1=( struct _tagged_arr) t;{
void* _temp147[ 4u]={& _temp148,& _temp149,& _temp150,& _temp151}; Cyc_Stdio_aprintf(
_tag_arr("%s %s : field name mismatch %s != %s", sizeof( unsigned char), 37u),
_tag_arr( _temp147, sizeof( void*), 4u));}}}}})); return 0;} if( ! Cyc_Tcutil_same_atts(
_temp124, _temp137)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
Cyc_Stdio_String_pa_struct _temp155; _temp155.tag= Cyc_Stdio_String_pa; _temp155.f1=(
struct _tagged_arr)* _temp132;{ struct Cyc_Stdio_String_pa_struct _temp154;
_temp154.tag= Cyc_Stdio_String_pa; _temp154.f1=( struct _tagged_arr) _temp89;{
struct Cyc_Stdio_String_pa_struct _temp153; _temp153.tag= Cyc_Stdio_String_pa;
_temp153.f1=( struct _tagged_arr) t;{ void* _temp152[ 3u]={& _temp153,& _temp154,&
_temp155}; Cyc_Stdio_aprintf( _tag_arr("%s %s : attribute mismatch on field %s",
sizeof( unsigned char), 39u), _tag_arr( _temp152, sizeof( void*), 3u));}}}}));
_temp90= 0;} if( ! Cyc_Tcutil_equal_tqual( _temp130, _temp143)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp159;
_temp159.tag= Cyc_Stdio_String_pa; _temp159.f1=( struct _tagged_arr)* _temp132;{
struct Cyc_Stdio_String_pa_struct _temp158; _temp158.tag= Cyc_Stdio_String_pa;
_temp158.f1=( struct _tagged_arr) _temp89;{ struct Cyc_Stdio_String_pa_struct
_temp157; _temp157.tag= Cyc_Stdio_String_pa; _temp157.f1=( struct _tagged_arr) t;{
void* _temp156[ 3u]={& _temp157,& _temp158,& _temp159}; Cyc_Stdio_aprintf(
_tag_arr("%s %s : qualifier mismatch on field %s", sizeof( unsigned char), 39u),
_tag_arr( _temp156, sizeof( void*), 3u));}}}})); _temp90= 0;} if(((( _temp126 !=
0? _temp139 != 0: 0)? Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp126))->v) != Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp139))->v): 0)?
1:( _temp126 == 0? _temp139 != 0: 0))? 1:( _temp126 != 0? _temp139 == 0: 0)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp163; _temp163.tag= Cyc_Stdio_String_pa; _temp163.f1=( struct _tagged_arr)*
_temp132;{ struct Cyc_Stdio_String_pa_struct _temp162; _temp162.tag= Cyc_Stdio_String_pa;
_temp162.f1=( struct _tagged_arr) _temp89;{ struct Cyc_Stdio_String_pa_struct
_temp161; _temp161.tag= Cyc_Stdio_String_pa; _temp161.f1=( struct _tagged_arr) t;{
void* _temp160[ 3u]={& _temp161,& _temp162,& _temp163}; Cyc_Stdio_aprintf(
_tag_arr("%s %s : bitfield mismatch on field %s", sizeof( unsigned char), 38u),
_tag_arr( _temp160, sizeof( void*), 3u));}}}})); _temp90= 0;}{ void* subst_t1=
Cyc_Tcutil_substitute( _temp120, _temp141); if( ! Cyc_Tcdecl_check_type(
_temp128, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
Cyc_Stdio_String_pa_struct _temp169; _temp169.tag= Cyc_Stdio_String_pa; _temp169.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( subst_t1);{ struct Cyc_Stdio_String_pa_struct
_temp168; _temp168.tag= Cyc_Stdio_String_pa; _temp168.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp128);{ struct Cyc_Stdio_String_pa_struct _temp167;
_temp167.tag= Cyc_Stdio_String_pa; _temp167.f1=( struct _tagged_arr)* _temp132;{
struct Cyc_Stdio_String_pa_struct _temp166; _temp166.tag= Cyc_Stdio_String_pa;
_temp166.f1=( struct _tagged_arr) _temp89;{ struct Cyc_Stdio_String_pa_struct
_temp165; _temp165.tag= Cyc_Stdio_String_pa; _temp165.f1=( struct _tagged_arr) t;{
void* _temp164[ 5u]={& _temp165,& _temp166,& _temp167,& _temp168,& _temp169};
Cyc_Stdio_aprintf( _tag_arr("%s %s : type mismatch on field %s: %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp164, sizeof( void*), 5u));}}}}}}));
_temp90= 0;}}}} if( _temp118 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp173; _temp173.tag= Cyc_Stdio_String_pa;
_temp173.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp118))->hd)->name;{ struct Cyc_Stdio_String_pa_struct _temp172;
_temp172.tag= Cyc_Stdio_String_pa; _temp172.f1=( struct _tagged_arr) _temp89;{
struct Cyc_Stdio_String_pa_struct _temp171; _temp171.tag= Cyc_Stdio_String_pa;
_temp171.f1=( struct _tagged_arr) t;{ void* _temp170[ 3u]={& _temp171,& _temp172,&
_temp173}; Cyc_Stdio_aprintf( _tag_arr("%s %s is missing field %s", sizeof(
unsigned char), 26u), _tag_arr( _temp170, sizeof( void*), 3u));}}}})); _temp90=
0;} if( _temp113 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
Cyc_Stdio_String_pa_struct _temp177; _temp177.tag= Cyc_Stdio_String_pa; _temp177.f1=(
struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp113))->hd)->name;{ struct Cyc_Stdio_String_pa_struct _temp176;
_temp176.tag= Cyc_Stdio_String_pa; _temp176.f1=( struct _tagged_arr) _temp89;{
struct Cyc_Stdio_String_pa_struct _temp175; _temp175.tag= Cyc_Stdio_String_pa;
_temp175.f1=( struct _tagged_arr) t;{ void* _temp174[ 3u]={& _temp175,& _temp176,&
_temp177}; Cyc_Stdio_aprintf( _tag_arr("%s %s has extra field %s", sizeof(
unsigned char), 25u), _tag_arr( _temp174, sizeof( void*), 3u));}}}})); _temp90=
0;} d2= d0; goto _LL99;} _LL99:;} if( ! _temp90){ return 0;} if( _temp95 ==(
void*) d2->sc){ return( struct Cyc_Absyn_Structdecl*) d2;} else{ d2=({ struct
Cyc_Absyn_Structdecl* _temp178=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp178[ 0]=* d2; _temp178;});( void*)(
d2->sc=( void*) _temp95); return( struct Cyc_Absyn_Structdecl*) d2;}}}} struct
Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl*
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
Cyc_Stdio_String_pa_struct _temp183; _temp183.tag= Cyc_Stdio_String_pa; _temp183.f1=(
struct _tagged_arr)*(* f0->name).f2;{ struct Cyc_Stdio_String_pa_struct _temp182;
_temp182.tag= Cyc_Stdio_String_pa; _temp182.f1=( struct _tagged_arr)*(* f1->name).f2;{
struct Cyc_Stdio_String_pa_struct _temp181; _temp181.tag= Cyc_Stdio_String_pa;
_temp181.f1=( struct _tagged_arr) v;{ struct Cyc_Stdio_String_pa_struct _temp180;
_temp180.tag= Cyc_Stdio_String_pa; _temp180.f1=( struct _tagged_arr) t;{ void*
_temp179[ 4u]={& _temp180,& _temp181,& _temp182,& _temp183}; Cyc_Stdio_aprintf(
_tag_arr("%s %s: field name mismatch %s != %s", sizeof( unsigned char), 36u),
_tag_arr( _temp179, sizeof( void*), 4u));}}}}})); return 0;}{ struct _tagged_arr
_temp184=*(* f0->name).f2; int _temp190; void* _temp192; struct _tuple3 _temp188=
Cyc_Tcdecl_merge_scope(( void*) f0->sc,( void*) f1->sc,( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp187; _temp187.tag= Cyc_Stdio_String_pa;
_temp187.f1=( struct _tagged_arr) v;{ struct Cyc_Stdio_String_pa_struct _temp186;
_temp186.tag= Cyc_Stdio_String_pa; _temp186.f1=( struct _tagged_arr) t;{ void*
_temp185[ 2u]={& _temp186,& _temp187}; Cyc_Stdio_aprintf( _tag_arr("in %s %s, field",
sizeof( unsigned char), 16u), _tag_arr( _temp185, sizeof( void*), 2u));}}}),
_temp184, loc, msg); _LL193: _temp192= _temp188.f1; goto _LL191; _LL191:
_temp190= _temp188.f2; goto _LL189; _LL189: { struct Cyc_List_List* _temp194= f0->tvs;
struct Cyc_List_List* _temp195= f1->tvs; if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp194) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp195)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp199; _temp199.tag= Cyc_Stdio_String_pa; _temp199.f1=( struct _tagged_arr)
_temp184;{ struct Cyc_Stdio_String_pa_struct _temp198; _temp198.tag= Cyc_Stdio_String_pa;
_temp198.f1=( struct _tagged_arr) v;{ struct Cyc_Stdio_String_pa_struct _temp197;
_temp197.tag= Cyc_Stdio_String_pa; _temp197.f1=( struct _tagged_arr) t;{ void*
_temp196[ 3u]={& _temp197,& _temp198,& _temp199}; Cyc_Stdio_aprintf( _tag_arr("%s %s, field %s: type parameter number mismatch",
sizeof( unsigned char), 48u), _tag_arr( _temp196, sizeof( void*), 3u));}}}}));
return 0;}{ struct Cyc_List_List* _temp200= Cyc_Tcdecl_build_tvs_map( _temp194,
_temp195); struct Cyc_List_List* _temp201= f0->typs; struct Cyc_List_List*
_temp202= f1->typs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp201) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp202)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp206; _temp206.tag= Cyc_Stdio_String_pa; _temp206.f1=( struct _tagged_arr)
_temp184;{ struct Cyc_Stdio_String_pa_struct _temp205; _temp205.tag= Cyc_Stdio_String_pa;
_temp205.f1=( struct _tagged_arr) v;{ struct Cyc_Stdio_String_pa_struct _temp204;
_temp204.tag= Cyc_Stdio_String_pa; _temp204.f1=( struct _tagged_arr) t;{ void*
_temp203[ 3u]={& _temp204,& _temp205,& _temp206}; Cyc_Stdio_aprintf( _tag_arr("%s %s, field %s: parameter number mismatch",
sizeof( unsigned char), 43u), _tag_arr( _temp203, sizeof( void*), 3u));}}}}));
_temp190= 0;} for( 0; _temp201 != 0;( _temp201=(( struct Cyc_List_List*)
_check_null( _temp201))->tl, _temp202=(( struct Cyc_List_List*) _check_null(
_temp202))->tl)){ if( ! Cyc_Tcutil_equal_tqual((*(( struct _tuple7*)(( struct
Cyc_List_List*) _check_null( _temp201))->hd)).f1,(*(( struct _tuple7*)(( struct
Cyc_List_List*) _check_null( _temp202))->hd)).f1)){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp210; _temp210.tag=
Cyc_Stdio_String_pa; _temp210.f1=( struct _tagged_arr) _temp184;{ struct Cyc_Stdio_String_pa_struct
_temp209; _temp209.tag= Cyc_Stdio_String_pa; _temp209.f1=( struct _tagged_arr) v;{
struct Cyc_Stdio_String_pa_struct _temp208; _temp208.tag= Cyc_Stdio_String_pa;
_temp208.f1=( struct _tagged_arr) t;{ void* _temp207[ 3u]={& _temp208,& _temp209,&
_temp210}; Cyc_Stdio_aprintf( _tag_arr("%s %s, field %s: parameter qualifier",
sizeof( unsigned char), 37u), _tag_arr( _temp207, sizeof( void*), 3u));}}}}));
_temp190= 0;}{ void* subst_t1= Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp200,
inst),(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp202))->hd)).f2);
if( ! Cyc_Tcdecl_check_type((*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp201))->hd)).f2, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp216; _temp216.tag= Cyc_Stdio_String_pa;
_temp216.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( subst_t1);{ struct Cyc_Stdio_String_pa_struct
_temp215; _temp215.tag= Cyc_Stdio_String_pa; _temp215.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string((*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp201))->hd)).f2);{ struct Cyc_Stdio_String_pa_struct _temp214;
_temp214.tag= Cyc_Stdio_String_pa; _temp214.f1=( struct _tagged_arr) _temp184;{
struct Cyc_Stdio_String_pa_struct _temp213; _temp213.tag= Cyc_Stdio_String_pa;
_temp213.f1=( struct _tagged_arr) v;{ struct Cyc_Stdio_String_pa_struct _temp212;
_temp212.tag= Cyc_Stdio_String_pa; _temp212.f1=( struct _tagged_arr) t;{ void*
_temp211[ 5u]={& _temp212,& _temp213,& _temp214,& _temp215,& _temp216}; Cyc_Stdio_aprintf(
_tag_arr("%s %s, field %s: parameter type mismatch %s != %s", sizeof(
unsigned char), 50u), _tag_arr( _temp211, sizeof( void*), 5u));}}}}}}));
_temp190= 0;}}} if( ! _temp190){ return 0;} if(( void*) f0->sc != _temp192){
struct Cyc_Absyn_Tunionfield* _temp217=({ struct Cyc_Absyn_Tunionfield* _temp218=(
struct Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp218[ 0]=* f0; _temp218;});( void*)((( struct Cyc_Absyn_Tunionfield*)
_check_null( _temp217))->sc=( void*) _temp192); return _temp217;} else{ return(
struct Cyc_Absyn_Tunionfield*) f0;}}}}} static int Cyc_Tcdecl_substitute_tunionfield_f1(
struct _tagged_arr* name, struct Cyc_Absyn_Tvar* x){ return Cyc_String_strptrcmp(
x->name, name) == 0;} static struct _tuple7* Cyc_Tcdecl_substitute_tunionfield_f2(
struct Cyc_List_List* inst, struct _tuple7* x){ struct _tuple7 _temp221; void*
_temp222; struct Cyc_Absyn_Tqual _temp224; struct _tuple7* _temp219= x; _temp221=*
_temp219; _LL225: _temp224= _temp221.f1; goto _LL223; _LL223: _temp222= _temp221.f2;
goto _LL220; _LL220: return({ struct _tuple7* _temp226=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7)); _temp226->f1= _temp224; _temp226->f2= Cyc_Tcutil_substitute(
inst, _temp222); _temp226;});} static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List* d0tvs, struct Cyc_List_List* d1tvs, struct Cyc_List_List*
inst1, struct Cyc_Absyn_Tunionfield* f1){ struct Cyc_Absyn_Tunionfield* _temp227=({
struct Cyc_Absyn_Tunionfield* _temp235=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp235[ 0]=* f1; _temp235;});
struct Cyc_List_List* _temp228= 0;{ struct Cyc_List_List* _temp229= f1->tvs;
for( 0; _temp229 != 0; _temp229=(( struct Cyc_List_List*) _check_null( _temp229))->tl){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp229))->hd; if((( int(*)( int(* pred)( struct _tagged_arr*,
struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcdecl_substitute_tunionfield_f1, tv->name, d0tvs)? 1:(( int(*)( int(* pred)(
struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1, tv->name,
d1tvs)){ int i= 0; struct _tagged_arr base=* tv->name; struct _tagged_arr*
new_name; do { new_name=({ struct _tagged_arr* _temp230=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp230[ 0]=( struct _tagged_arr) Cyc_String_strconcat(
base,( struct _tagged_arr) Cyc_Core_string_of_int( i)); _temp230;}); i ++;} while (((
int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct
_tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
new_name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)); tv=({ struct Cyc_Absyn_Tvar* _temp231=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp231[ 0]=* tv; _temp231;}); tv->name=
new_name;} _temp228=({ struct Cyc_List_List* _temp232=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp232->hd=( void*) tv; _temp232->tl=
_temp228; _temp232;});}} _temp228=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp228); _temp227->tvs= _temp228;{ struct Cyc_List_List*
_temp233= Cyc_Tcdecl_build_tvs_map( _temp228, f1->tvs); struct Cyc_List_List*
_temp234=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp233, inst1); _temp227->typs=(( struct Cyc_List_List*(*)(
struct _tuple7*(* f)( struct Cyc_List_List*, struct _tuple7*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcdecl_substitute_tunionfield_f2,
_temp234, f1->typs); return _temp227;}} static struct Cyc_List_List* Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1, int* res, int* incl,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp244=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp244[ 0]= 0; _temp244;}); struct Cyc_List_List** _temp236= f2sp; int cmp= -
1; for( 0; f0s != 0? f1s != 0: 0; f1s=(( struct Cyc_List_List*) _check_null( f1s))->tl){
while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)->name,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd)->name)) < 0: 0) { struct Cyc_List_List*
_temp237=({ struct Cyc_List_List* _temp238=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp238->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd); _temp238->tl= 0; _temp238;});*
_temp236= _temp237; _temp236=&(( struct Cyc_List_List*) _check_null( _temp237))->tl;
f0s=(( struct Cyc_List_List*) _check_null( f0s))->tl;} if( f0s == 0? 1: cmp > 0){*
incl= 0;{ struct Cyc_List_List* _temp239=({ struct Cyc_List_List* _temp240=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp240->hd=(
void*) Cyc_Tcdecl_substitute_tunionfield( tvs0, tvs1, inst,( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd); _temp240->tl= 0; _temp240;});*
_temp236= _temp239; _temp236=&(( struct Cyc_List_List*) _check_null( _temp239))->tl;}}
else{ struct Cyc_Absyn_Tunionfield* _temp241= Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f0s))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f1s))->hd,
inst, t, v, msg); if( _temp241 != 0){ if( _temp241 !=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f0s))->hd)){*
incl= 0;}{ struct Cyc_List_List* _temp242=({ struct Cyc_List_List* _temp243=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp243->hd=(
void*)(( struct Cyc_Absyn_Tunionfield*) _check_null( _temp241)); _temp243->tl= 0;
_temp243;});* _temp236= _temp242; _temp236=&(( struct Cyc_List_List*)
_check_null( _temp242))->tl;}} else{* res= 0;} f0s=(( struct Cyc_List_List*)
_check_null( f0s))->tl;}} if( f1s != 0){* incl= 0;* _temp236= f1s;} else{*
_temp236= f0s;} return* f2sp;} struct _tuple8{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; } ; static struct _tuple8 Cyc_Tcdecl_split( struct Cyc_List_List*
f){ if( f == 0){ return({ struct _tuple8 _temp245; _temp245.f1= 0; _temp245.f2=
0; _temp245;});} if((( struct Cyc_List_List*) _check_null( f))->tl == 0){ return({
struct _tuple8 _temp246; _temp246.f1= f; _temp246.f2= 0; _temp246;});}{ struct
Cyc_List_List* _temp249; struct Cyc_List_List* _temp251; struct _tuple8 _temp247=
Cyc_Tcdecl_split((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( f))->tl))->tl); _LL252: _temp251= _temp247.f1; goto _LL250; _LL250:
_temp249= _temp247.f2; goto _LL248; _LL248: return({ struct _tuple8 _temp253;
_temp253.f1=({ struct Cyc_List_List* _temp255=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp255->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( f))->hd); _temp255->tl= _temp251; _temp255;}); _temp253.f2=({
struct Cyc_List_List* _temp254=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp254->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( f))->tl))->hd); _temp254->tl=
_temp249; _temp254;}); _temp253;});}} struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List* f, int* res, struct _tagged_arr* v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct Cyc_List_List* _temp258; struct Cyc_List_List*
_temp260; struct _tuple8 _temp256=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL261: _temp260= _temp256.f1; goto _LL259; _LL259:
_temp258= _temp256.f2; goto _LL257; _LL257: if( _temp260 != 0?(( struct Cyc_List_List*)
_check_null( _temp260))->tl != 0: 0){ _temp260= Cyc_Tcdecl_sort_xtunion_fields(
_temp260, res, v, loc, msg);} if( _temp258 != 0?(( struct Cyc_List_List*)
_check_null( _temp258))->tl != 0: 0){ _temp258= Cyc_Tcdecl_sort_xtunion_fields(
_temp258, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp260,
_temp258, 0, 0, 0, res,({ int* _temp262=( int*) GC_malloc_atomic( sizeof( int));
_temp262[ 0]= 1; _temp262;}), _tag_arr("xtunion", sizeof( unsigned char), 8u),*
v, loc, msg);} struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl( struct
Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp263= Cyc_Absynpp_qvar2string(
d0->name); struct _tagged_arr t= _tag_arr("[x]tunion", sizeof( unsigned char),
10u); int _temp264= 1; if( d0->is_xtunion != d1->is_xtunion){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp268;
_temp268.tag= Cyc_Stdio_String_pa; _temp268.f1=( struct _tagged_arr) Cyc_Tcdecl_is_x2string(
d1->is_xtunion);{ struct Cyc_Stdio_String_pa_struct _temp267; _temp267.tag= Cyc_Stdio_String_pa;
_temp267.f1=( struct _tagged_arr) Cyc_Tcdecl_is_x2string( d0->is_xtunion);{
struct Cyc_Stdio_String_pa_struct _temp266; _temp266.tag= Cyc_Stdio_String_pa;
_temp266.f1=( struct _tagged_arr) _temp263;{ void* _temp265[ 3u]={& _temp266,&
_temp267,& _temp268}; Cyc_Stdio_aprintf( _tag_arr("expected %s to be a %s instead of a %s",
sizeof( unsigned char), 39u), _tag_arr( _temp265, sizeof( void*), 3u));}}}}));
_temp264= 0;} else{ t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, _temp263, loc, msg)){ return 0;}{ int _temp271; void*
_temp273; struct _tuple3 _temp269= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*)
d1->sc, t, _temp263, loc, msg); _LL274: _temp273= _temp269.f1; goto _LL272;
_LL272: _temp271= _temp269.f2; goto _LL270; _LL270: if( ! _temp271){ _temp264= 0;}{
struct Cyc_Absyn_Tuniondecl* d2;{ struct _tuple6 _temp276=({ struct _tuple6
_temp275; _temp275.f1= d0->fields; _temp275.f2= d1->fields; _temp275;}); struct
Cyc_Core_Opt* _temp284; struct Cyc_Core_Opt* _temp286; struct Cyc_Core_Opt*
_temp288; struct Cyc_Core_Opt _temp290; struct Cyc_List_List* _temp291; struct
Cyc_Core_Opt* _temp293; struct Cyc_Core_Opt _temp295; struct Cyc_List_List*
_temp296; _LL278: _LL285: _temp284= _temp276.f2; if( _temp284 == 0){ goto _LL279;}
else{ goto _LL280;} _LL280: _LL287: _temp286= _temp276.f1; if( _temp286 == 0){
goto _LL281;} else{ goto _LL282;} _LL282: _LL294: _temp293= _temp276.f1; if(
_temp293 == 0){ goto _LL277;} else{ _temp295=* _temp293; _LL297: _temp296=(
struct Cyc_List_List*) _temp295.v; goto _LL289;} _LL289: _temp288= _temp276.f2;
if( _temp288 == 0){ goto _LL277;} else{ _temp290=* _temp288; _LL292: _temp291=(
struct Cyc_List_List*) _temp290.v; goto _LL283;} _LL279: d2= d0; goto _LL277;
_LL281: d2= d1; goto _LL277; _LL283: { struct Cyc_List_List* _temp298= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); if( d0->is_xtunion){ int _temp299= 1; struct Cyc_List_List*
_temp300= Cyc_Tcdecl_merge_xtunion_fields( _temp296, _temp291, _temp298, d0->tvs,
d1->tvs,& _temp264,& _temp299, t, _temp263, loc, msg); if( _temp299){ d2= d0;}
else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp301=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp301[ 0]=* d0; _temp301;});(
void*)( d2->sc=( void*) _temp273); d2->fields=({ struct Cyc_Core_Opt* _temp302=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp302->v=(
void*) _temp300; _temp302;});}} else{ for( 0; _temp296 != 0? _temp291 != 0: 0;(
_temp296=(( struct Cyc_List_List*) _check_null( _temp296))->tl, _temp291=((
struct Cyc_List_List*) _check_null( _temp291))->tl)){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp296))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp291))->hd,
_temp298, t, _temp263, msg);} if( _temp296 != 0){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp306; _temp306.tag=
Cyc_Stdio_String_pa; _temp306.f1=( struct _tagged_arr)*(*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp291))->hd)->name).f2;{ struct Cyc_Stdio_String_pa_struct
_temp305; _temp305.tag= Cyc_Stdio_String_pa; _temp305.f1=( struct _tagged_arr)
_temp263;{ struct Cyc_Stdio_String_pa_struct _temp304; _temp304.tag= Cyc_Stdio_String_pa;
_temp304.f1=( struct _tagged_arr) t;{ void* _temp303[ 3u]={& _temp304,& _temp305,&
_temp306}; Cyc_Stdio_aprintf( _tag_arr("%s %s has extra field %s", sizeof(
unsigned char), 25u), _tag_arr( _temp303, sizeof( void*), 3u));}}}})); _temp264=
0;} if( _temp291 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
Cyc_Stdio_String_pa_struct _temp310; _temp310.tag= Cyc_Stdio_String_pa; _temp310.f1=(
struct _tagged_arr)*(*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp291))->hd)->name).f2;{ struct Cyc_Stdio_String_pa_struct
_temp309; _temp309.tag= Cyc_Stdio_String_pa; _temp309.f1=( struct _tagged_arr)
_temp263;{ struct Cyc_Stdio_String_pa_struct _temp308; _temp308.tag= Cyc_Stdio_String_pa;
_temp308.f1=( struct _tagged_arr) t;{ void* _temp307[ 3u]={& _temp308,& _temp309,&
_temp310}; Cyc_Stdio_aprintf( _tag_arr("%s %s is missing field %s", sizeof(
unsigned char), 26u), _tag_arr( _temp307, sizeof( void*), 3u));}}}})); _temp264=
0;} d2= d0;} goto _LL277;} _LL277:;} if( ! _temp264){ return 0;} if( _temp273 ==(
void*) d2->sc){ return( struct Cyc_Absyn_Tuniondecl*) d2;} else{ d2=({ struct
Cyc_Absyn_Tuniondecl* _temp311=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp311[ 0]=* d2; _temp311;});( void*)(
d2->sc=( void*) _temp273); return( struct Cyc_Absyn_Tuniondecl*) d2;}}}} struct
Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl( struct Cyc_Absyn_Enumdecl* d0,
struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg){ struct _tagged_arr _temp312= Cyc_Absynpp_qvar2string( d0->name);
int _temp313= 1; int _temp316; void* _temp318; struct _tuple3 _temp314= Cyc_Tcdecl_merge_scope((
void*) d0->sc,( void*) d1->sc, _tag_arr("enum", sizeof( unsigned char), 5u),
_temp312, loc, msg); _LL319: _temp318= _temp314.f1; goto _LL317; _LL317:
_temp316= _temp314.f2; goto _LL315; _LL315: if( ! _temp316){ _temp313= 0;}{
struct Cyc_Absyn_Enumdecl* d2;{ struct _tuple6 _temp321=({ struct _tuple6
_temp320; _temp320.f1= d0->fields; _temp320.f2= d1->fields; _temp320;}); struct
Cyc_Core_Opt* _temp329; struct Cyc_Core_Opt* _temp331; struct Cyc_Core_Opt*
_temp333; struct Cyc_Core_Opt _temp335; struct Cyc_List_List* _temp336; struct
Cyc_Core_Opt* _temp338; struct Cyc_Core_Opt _temp340; struct Cyc_List_List*
_temp341; _LL323: _LL330: _temp329= _temp321.f2; if( _temp329 == 0){ goto _LL324;}
else{ goto _LL325;} _LL325: _LL332: _temp331= _temp321.f1; if( _temp331 == 0){
goto _LL326;} else{ goto _LL327;} _LL327: _LL339: _temp338= _temp321.f1; if(
_temp338 == 0){ goto _LL322;} else{ _temp340=* _temp338; _LL342: _temp341=(
struct Cyc_List_List*) _temp340.v; goto _LL334;} _LL334: _temp333= _temp321.f2;
if( _temp333 == 0){ goto _LL322;} else{ _temp335=* _temp333; _LL337: _temp336=(
struct Cyc_List_List*) _temp335.v; goto _LL328;} _LL324: d2= d0; goto _LL322;
_LL326: d2= d1; goto _LL322; _LL328: for( 0; _temp341 != 0? _temp336 != 0: 0;(
_temp341=(( struct Cyc_List_List*) _check_null( _temp341))->tl, _temp336=((
struct Cyc_List_List*) _check_null( _temp336))->tl)){ struct Cyc_Absyn_Enumfield
_temp345; struct Cyc_Position_Segment* _temp346; struct Cyc_Absyn_Exp* _temp348;
struct _tuple0* _temp350; struct Cyc_Absyn_Enumfield* _temp343=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp341))->hd; _temp345=* _temp343; _LL351:
_temp350= _temp345.name; goto _LL349; _LL349: _temp348= _temp345.tag; goto
_LL347; _LL347: _temp346= _temp345.loc; goto _LL344; _LL344: { struct Cyc_Absyn_Enumfield
_temp354; struct Cyc_Position_Segment* _temp355; struct Cyc_Absyn_Exp* _temp357;
struct _tuple0* _temp359; struct Cyc_Absyn_Enumfield* _temp352=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp336))->hd; _temp354=* _temp352; _LL360:
_temp359= _temp354.name; goto _LL358; _LL358: _temp357= _temp354.tag; goto
_LL356; _LL356: _temp355= _temp354.loc; goto _LL353; _LL353: if( Cyc_String_zstrptrcmp((*
_temp359).f2,(* _temp350).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp364; _temp364.tag= Cyc_Stdio_String_pa;
_temp364.f1=( struct _tagged_arr)*(* _temp350).f2;{ struct Cyc_Stdio_String_pa_struct
_temp363; _temp363.tag= Cyc_Stdio_String_pa; _temp363.f1=( struct _tagged_arr)*(*
_temp359).f2;{ struct Cyc_Stdio_String_pa_struct _temp362; _temp362.tag= Cyc_Stdio_String_pa;
_temp362.f1=( struct _tagged_arr) _temp312;{ void* _temp361[ 3u]={& _temp362,&
_temp363,& _temp364}; Cyc_Stdio_aprintf( _tag_arr("enum %s: field name mismatch %s != %s",
sizeof( unsigned char), 38u), _tag_arr( _temp361, sizeof( void*), 3u));}}}}));
_temp313= 0;} if( Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)
_check_null( _temp348)) != Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)
_check_null( _temp357))){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp367; _temp367.tag= Cyc_Stdio_String_pa;
_temp367.f1=( struct _tagged_arr)*(* _temp359).f2;{ struct Cyc_Stdio_String_pa_struct
_temp366; _temp366.tag= Cyc_Stdio_String_pa; _temp366.f1=( struct _tagged_arr)
_temp312;{ void* _temp365[ 2u]={& _temp366,& _temp367}; Cyc_Stdio_aprintf(
_tag_arr("enum %s, field %s, value mismatch", sizeof( unsigned char), 34u),
_tag_arr( _temp365, sizeof( void*), 2u));}}})); _temp313= 0;}}} d2= d0; goto
_LL322; _LL322:;} if( ! _temp313){ return 0;} if(( void*) d2->sc == _temp318){
return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({ struct Cyc_Absyn_Enumdecl*
_temp368=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp368[ 0]=* d2; _temp368;});( void*)( d2->sc=( void*) _temp318); return(
struct Cyc_Absyn_Enumdecl*) d2;}}} inline static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void* sc0, void* t0, struct Cyc_Absyn_Tqual tq0, struct Cyc_List_List* atts0,
void* sc1, void* t1, struct Cyc_Absyn_Tqual tq1, struct Cyc_List_List* atts1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ int _temp369= 1; int _temp372; void* _temp374; struct
_tuple3 _temp370= Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg); _LL375:
_temp374= _temp370.f1; goto _LL373; _LL373: _temp372= _temp370.f2; goto _LL371;
_LL371: if( ! _temp372){ _temp369= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp380; _temp380.tag= Cyc_Stdio_String_pa; _temp380.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t0);{ struct Cyc_Stdio_String_pa_struct _temp379;
_temp379.tag= Cyc_Stdio_String_pa; _temp379.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ struct Cyc_Stdio_String_pa_struct _temp378; _temp378.tag= Cyc_Stdio_String_pa;
_temp378.f1=( struct _tagged_arr) v;{ struct Cyc_Stdio_String_pa_struct _temp377;
_temp377.tag= Cyc_Stdio_String_pa; _temp377.f1=( struct _tagged_arr) t;{ void*
_temp376[ 4u]={& _temp377,& _temp378,& _temp379,& _temp380}; Cyc_Stdio_aprintf(
_tag_arr("%s %s has type %s instead of %s", sizeof( unsigned char), 32u),
_tag_arr( _temp376, sizeof( void*), 4u));}}}}})); _temp369= 0;} if( ! Cyc_Tcutil_equal_tqual(
tq0, tq1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp383; _temp383.tag= Cyc_Stdio_String_pa; _temp383.f1=( struct _tagged_arr) v;{
struct Cyc_Stdio_String_pa_struct _temp382; _temp382.tag= Cyc_Stdio_String_pa;
_temp382.f1=( struct _tagged_arr) t;{ void* _temp381[ 2u]={& _temp382,& _temp383};
Cyc_Stdio_aprintf( _tag_arr("%s %s has different type qualifiers", sizeof(
unsigned char), 36u), _tag_arr( _temp381, sizeof( void*), 2u));}}})); _temp369=
0;} if( ! Cyc_Tcutil_same_atts( atts0, atts1)){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp386; _temp386.tag=
Cyc_Stdio_String_pa; _temp386.f1=( struct _tagged_arr) v;{ struct Cyc_Stdio_String_pa_struct
_temp385; _temp385.tag= Cyc_Stdio_String_pa; _temp385.f1=( struct _tagged_arr) t;{
void* _temp384[ 2u]={& _temp385,& _temp386}; Cyc_Stdio_aprintf( _tag_arr("%s %s has different attributes",
sizeof( unsigned char), 31u), _tag_arr( _temp384, sizeof( void*), 2u));}}}));({
void* _temp387[ 0u]={}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("previous attributes: ",
sizeof( unsigned char), 22u), _tag_arr( _temp387, sizeof( void*), 0u));}); for(
0; atts0 != 0; atts0=(( struct Cyc_List_List*) _check_null( atts0))->tl){({
struct Cyc_Stdio_String_pa_struct _temp389; _temp389.tag= Cyc_Stdio_String_pa;
_temp389.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts0))->hd);{ void* _temp388[ 1u]={& _temp389};
Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s ", sizeof( unsigned char), 4u),
_tag_arr( _temp388, sizeof( void*), 1u));}});}({ void* _temp390[ 0u]={}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("\ncurrent attributes: ", sizeof( unsigned char), 22u),
_tag_arr( _temp390, sizeof( void*), 0u));}); for( 0; atts1 != 0; atts1=(( struct
Cyc_List_List*) _check_null( atts1))->tl){({ struct Cyc_Stdio_String_pa_struct
_temp392; _temp392.tag= Cyc_Stdio_String_pa; _temp392.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts1))->hd);{
void* _temp391[ 1u]={& _temp392}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s ",
sizeof( unsigned char), 4u), _tag_arr( _temp391, sizeof( void*), 1u));}});}({
void* _temp393[ 0u]={}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("\n",
sizeof( unsigned char), 2u), _tag_arr( _temp393, sizeof( void*), 0u));});
_temp369= 0;} return({ struct _tuple3 _temp394; _temp394.f1= _temp374; _temp394.f2=
_temp369; _temp394;});} struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp395= Cyc_Absynpp_qvar2string(
d0->name); int _temp398; void* _temp400; struct _tuple3 _temp396= Cyc_Tcdecl_check_var_or_fn_decl((
void*) d0->sc,( void*) d0->type, d0->tq, d0->attributes,( void*) d1->sc,( void*)
d1->type, d1->tq, d1->attributes, _tag_arr("variable", sizeof( unsigned char), 9u),
_temp395, loc, msg); _LL401: _temp400= _temp396.f1; goto _LL399; _LL399:
_temp398= _temp396.f2; goto _LL397; _LL397: if( ! _temp398){ return 0;} if((
void*) d0->sc == _temp400){ return( struct Cyc_Absyn_Vardecl*) d0;} else{ struct
Cyc_Absyn_Vardecl* _temp402=({ struct Cyc_Absyn_Vardecl* _temp403=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp403[ 0]=* d0; _temp403;});(
void*)((( struct Cyc_Absyn_Vardecl*) _check_null( _temp402))->sc=( void*)
_temp400); return _temp402;}} struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp404= Cyc_Absynpp_qvar2string(
d0->name); if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs, _tag_arr("typedef",
sizeof( unsigned char), 8u), _temp404, loc, msg)){ return 0;}{ struct Cyc_List_List*
_temp405= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); void* subst_defn1= Cyc_Tcutil_substitute(
_temp405,( void*) d1->defn); if( ! Cyc_Tcdecl_check_type(( void*) d0->defn,
subst_defn1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp409; _temp409.tag= Cyc_Stdio_String_pa; _temp409.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*) d0->defn);{ struct Cyc_Stdio_String_pa_struct
_temp408; _temp408.tag= Cyc_Stdio_String_pa; _temp408.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( subst_defn1);{ struct Cyc_Stdio_String_pa_struct
_temp407; _temp407.tag= Cyc_Stdio_String_pa; _temp407.f1=( struct _tagged_arr)
_temp404;{ void* _temp406[ 3u]={& _temp407,& _temp408,& _temp409}; Cyc_Stdio_aprintf(
_tag_arr("typedef %s does not refer to the same type: %s != %s", sizeof(
unsigned char), 53u), _tag_arr( _temp406, sizeof( void*), 3u));}}}})); return 0;}
return( struct Cyc_Absyn_Typedefdecl*) d0;}} void* Cyc_Tcdecl_merge_binding(
void* b0, void* b1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
struct _tuple4 _temp411=({ struct _tuple4 _temp410; _temp410.f1= b0; _temp410.f2=
b1; _temp410;}); void* _temp425; void* _temp427; void* _temp429; struct Cyc_Absyn_Vardecl*
_temp431; void* _temp433; struct Cyc_Absyn_Vardecl* _temp435; void* _temp437;
struct Cyc_Absyn_Fndecl* _temp439; void* _temp441; struct Cyc_Absyn_Vardecl*
_temp443; void* _temp445; struct Cyc_Absyn_Fndecl* _temp447; void* _temp449;
void* _temp451; struct Cyc_Absyn_Vardecl* _temp453; void* _temp455; struct Cyc_Absyn_Fndecl*
_temp457; _LL413: _LL428: _temp427= _temp411.f1; if( _temp427 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL426;} else{ goto _LL415;} _LL426: _temp425= _temp411.f2; if( _temp425 ==(
void*) Cyc_Absyn_Unresolved_b){ goto _LL414;} else{ goto _LL415;} _LL415: _LL434:
_temp433= _temp411.f1; if(( unsigned int) _temp433 > 1u?*(( int*) _temp433) ==
Cyc_Absyn_Global_b: 0){ _LL436: _temp435=(( struct Cyc_Absyn_Global_b_struct*)
_temp433)->f1; goto _LL430;} else{ goto _LL417;} _LL430: _temp429= _temp411.f2;
if(( unsigned int) _temp429 > 1u?*(( int*) _temp429) == Cyc_Absyn_Global_b: 0){
_LL432: _temp431=(( struct Cyc_Absyn_Global_b_struct*) _temp429)->f1; goto
_LL416;} else{ goto _LL417;} _LL417: _LL442: _temp441= _temp411.f1; if((
unsigned int) _temp441 > 1u?*(( int*) _temp441) == Cyc_Absyn_Global_b: 0){
_LL444: _temp443=(( struct Cyc_Absyn_Global_b_struct*) _temp441)->f1; goto
_LL438;} else{ goto _LL419;} _LL438: _temp437= _temp411.f2; if(( unsigned int)
_temp437 > 1u?*(( int*) _temp437) == Cyc_Absyn_Funname_b: 0){ _LL440: _temp439=((
struct Cyc_Absyn_Funname_b_struct*) _temp437)->f1; goto _LL418;} else{ goto
_LL419;} _LL419: _LL450: _temp449= _temp411.f1; if(( unsigned int) _temp449 > 1u?*((
int*) _temp449) == Cyc_Absyn_Funname_b: 0){ goto _LL446;} else{ goto _LL421;}
_LL446: _temp445= _temp411.f2; if(( unsigned int) _temp445 > 1u?*(( int*)
_temp445) == Cyc_Absyn_Funname_b: 0){ _LL448: _temp447=(( struct Cyc_Absyn_Funname_b_struct*)
_temp445)->f1; goto _LL420;} else{ goto _LL421;} _LL421: _LL456: _temp455=
_temp411.f1; if(( unsigned int) _temp455 > 1u?*(( int*) _temp455) == Cyc_Absyn_Funname_b:
0){ _LL458: _temp457=(( struct Cyc_Absyn_Funname_b_struct*) _temp455)->f1; goto
_LL452;} else{ goto _LL423;} _LL452: _temp451= _temp411.f2; if(( unsigned int)
_temp451 > 1u?*(( int*) _temp451) == Cyc_Absyn_Global_b: 0){ _LL454: _temp453=((
struct Cyc_Absyn_Global_b_struct*) _temp451)->f1; goto _LL422;} else{ goto
_LL423;} _LL423: goto _LL424; _LL414: return( void*) Cyc_Absyn_Unresolved_b;
_LL416: { struct Cyc_Absyn_Vardecl* _temp459= Cyc_Tcdecl_merge_vardecl( _temp435,
_temp431, loc, msg); if( _temp459 == 0){ return( void*) Cyc_Absyn_Unresolved_b;}
if(( struct Cyc_Absyn_Vardecl*) _temp435 == _temp459){ return b0;} else{ if((
struct Cyc_Absyn_Vardecl*) _temp431 == _temp459){ return b1;} else{ return( void*)({
struct Cyc_Absyn_Global_b_struct* _temp460=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp460[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp461; _temp461.tag= Cyc_Absyn_Global_b; _temp461.f1=(
struct Cyc_Absyn_Vardecl*) _check_null( _temp459); _temp461;}); _temp460;});}}}
_LL418: { int _temp464; struct _tuple3 _temp462= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp443->sc,( void*) _temp443->type, _temp443->tq, _temp443->attributes,(
void*) _temp439->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp439->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp439->attributes, _tag_arr("function", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp443->name), loc, msg); _LL465:
_temp464= _temp462.f2; goto _LL463; _LL463: if( ! _temp464){ return( void*) Cyc_Absyn_Unresolved_b;}
return b1;} _LL420: Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp467; _temp467.tag= Cyc_Stdio_String_pa; _temp467.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp447->name);{ void* _temp466[ 1u]={& _temp467}; Cyc_Stdio_aprintf(
_tag_arr("redefinition of function %s", sizeof( unsigned char), 28u), _tag_arr(
_temp466, sizeof( void*), 1u));}})); return( void*) Cyc_Absyn_Unresolved_b;
_LL422: { int _temp470; struct _tuple3 _temp468= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp457->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp457->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp457->attributes,( void*) _temp453->sc,( void*)
_temp453->type, _temp453->tq, _temp453->attributes, _tag_arr("variable", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp457->name), loc, msg); _LL471:
_temp470= _temp468.f2; goto _LL469; _LL469: if( ! _temp470){ return( void*) Cyc_Absyn_Unresolved_b;}
return b0;} _LL424:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp472=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp472[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp473; _temp473.tag= Cyc_Core_InvalidArg;
_temp473.f1= _tag_arr("Tcdecl::merge_binding", sizeof( unsigned char), 22u);
_temp473;}); _temp472;})); _LL412:;} struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl
_temp476; struct Cyc_Absyn_Tunionfield* _temp477; struct Cyc_Absyn_Tuniondecl*
_temp479; struct Cyc_Tcdecl_Xtunionfielddecl* _temp474= d0; _temp476=* _temp474;
_LL480: _temp479= _temp476.base; goto _LL478; _LL478: _temp477= _temp476.field;
goto _LL475; _LL475: { struct Cyc_Tcdecl_Xtunionfielddecl _temp483; struct Cyc_Absyn_Tunionfield*
_temp484; struct Cyc_Absyn_Tuniondecl* _temp486; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp481= d1; _temp483=* _temp481; _LL487: _temp486= _temp483.base; goto _LL485;
_LL485: _temp484= _temp483.field; goto _LL482; _LL482: { struct _tagged_arr
_temp488= Cyc_Absynpp_qvar2string( _temp477->name); if( Cyc_Absyn_qvar_cmp(
_temp479->name, _temp486->name) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp492; _temp492.tag= Cyc_Stdio_String_pa;
_temp492.f1=( struct _tagged_arr) _temp488;{ struct Cyc_Stdio_String_pa_struct
_temp491; _temp491.tag= Cyc_Stdio_String_pa; _temp491.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp486->name);{ struct Cyc_Stdio_String_pa_struct
_temp490; _temp490.tag= Cyc_Stdio_String_pa; _temp490.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp479->name);{ void* _temp489[ 3u]={& _temp490,&
_temp491,& _temp492}; Cyc_Stdio_aprintf( _tag_arr("xtunions %s and %s have both a field named %s",
sizeof( unsigned char), 46u), _tag_arr( _temp489, sizeof( void*), 3u));}}}}));
return 0;} if( ! Cyc_Tcdecl_check_tvs( _temp479->tvs, _temp486->tvs, _tag_arr("xtunion",
sizeof( unsigned char), 8u), Cyc_Absynpp_qvar2string( _temp479->name), loc, msg)){
return 0;}{ struct Cyc_List_List* _temp493= Cyc_Tcdecl_build_tvs_map( _temp479->tvs,
_temp486->tvs); struct Cyc_Absyn_Tunionfield* _temp494= Cyc_Tcdecl_merge_tunionfield(
_temp477, _temp484, _temp493, _tag_arr("xtunionfield", sizeof( unsigned char),
13u), _temp488, msg); if( _temp494 == 0){ return 0;} if( _temp494 ==( struct Cyc_Absyn_Tunionfield*)
_temp477){ return( struct Cyc_Tcdecl_Xtunionfielddecl*) d0;} else{ return({
struct Cyc_Tcdecl_Xtunionfielddecl* _temp495=( struct Cyc_Tcdecl_Xtunionfielddecl*)
GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp495->base=
_temp479; _temp495->field=( struct Cyc_Absyn_Tunionfield*) _check_null( _temp494);
_temp495;});}}}}}
