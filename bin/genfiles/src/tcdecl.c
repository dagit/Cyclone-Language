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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
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
struct Cyc_Position_Segment*, struct _tagged_arr); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2); extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); extern
struct _tagged_arr Cyc_Absynpp_scope2string( void* sc); unsigned char Cyc_Tcdecl_Incompatible[
17u]="\000\000\000\000Incompatible"; void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg1, struct _tagged_arr msg2){ if( msg1 == 0){( int)
_throw(( void*) Cyc_Tcdecl_Incompatible);} else{ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp0)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp5= _tag_arr("%s %s",
sizeof( unsigned char), 6u); struct Cyc_Stdio_String_pa_struct _temp7; _temp7.tag=
Cyc_Stdio_String_pa; _temp7.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_null( msg1));{ void* _temp3=( void*)& _temp7; struct Cyc_Stdio_String_pa_struct
_temp6; _temp6.tag= Cyc_Stdio_String_pa; _temp6.f1=( struct _tagged_arr) msg2;{
void* _temp4=( void*)& _temp6; void* _temp1[ 2u]={ _temp3, _temp4}; struct
_tagged_arr _temp2={( void*) _temp1,( void*) _temp1,( void*)( _temp1 + 2u)};
_temp0( _temp5, _temp2);}}}));}} static void Cyc_Tcdecl_merge_scope_err( void*
s0, void* s1, struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr(* _temp8)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp15= _tag_arr("%s %s is %s whereas expected scope is %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp19; _temp19.tag=
Cyc_Stdio_String_pa; _temp19.f1=( struct _tagged_arr) t;{ void* _temp11=( void*)&
_temp19; struct Cyc_Stdio_String_pa_struct _temp18; _temp18.tag= Cyc_Stdio_String_pa;
_temp18.f1=( struct _tagged_arr) v;{ void* _temp12=( void*)& _temp18; struct Cyc_Stdio_String_pa_struct
_temp17; _temp17.tag= Cyc_Stdio_String_pa; _temp17.f1=( struct _tagged_arr) Cyc_Absynpp_scope2string(
s1);{ void* _temp13=( void*)& _temp17; struct Cyc_Stdio_String_pa_struct _temp16;
_temp16.tag= Cyc_Stdio_String_pa; _temp16.f1=( struct _tagged_arr) Cyc_Absynpp_scope2string(
s0);{ void* _temp14=( void*)& _temp16; void* _temp9[ 4u]={ _temp11, _temp12,
_temp13, _temp14}; struct _tagged_arr _temp10={( void*) _temp9,( void*) _temp9,(
void*)( _temp9 + 4u)}; _temp8( _temp15, _temp10);}}}}}));} struct _tuple4{ void*
f1; void* f2; } ; struct _tuple3 Cyc_Tcdecl_merge_scope( void* s0, void* s1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){{ struct _tuple4 _temp21=({ struct _tuple4 _temp20;
_temp20.f1= s0; _temp20.f2= s1; _temp20;}); void* _temp41; void* _temp43; void*
_temp45; void* _temp47; void* _temp49; void* _temp51; void* _temp53; void*
_temp55; void* _temp57; void* _temp59; void* _temp61; void* _temp63; _LL23:
_LL44: _temp43= _temp21.f1; if( _temp43 ==( void*) Cyc_Absyn_ExternC){ goto
_LL42;} else{ goto _LL25;} _LL42: _temp41= _temp21.f2; if( _temp41 ==( void*)
Cyc_Absyn_ExternC){ goto _LL24;} else{ goto _LL25;} _LL25: _LL46: _temp45=
_temp21.f1; if( _temp45 ==( void*) Cyc_Absyn_ExternC){ goto _LL26;} else{ goto
_LL27;} _LL27: _LL48: _temp47= _temp21.f2; if( _temp47 ==( void*) Cyc_Absyn_ExternC){
goto _LL28;} else{ goto _LL29;} _LL29: _LL50: _temp49= _temp21.f2; if( _temp49
==( void*) Cyc_Absyn_Extern){ goto _LL30;} else{ goto _LL31;} _LL31: _LL52:
_temp51= _temp21.f1; if( _temp51 ==( void*) Cyc_Absyn_Extern){ goto _LL32;}
else{ goto _LL33;} _LL33: _LL56: _temp55= _temp21.f1; if( _temp55 ==( void*) Cyc_Absyn_Static){
goto _LL54;} else{ goto _LL35;} _LL54: _temp53= _temp21.f2; if( _temp53 ==( void*)
Cyc_Absyn_Static){ goto _LL34;} else{ goto _LL35;} _LL35: _LL60: _temp59=
_temp21.f1; if( _temp59 ==( void*) Cyc_Absyn_Public){ goto _LL58;} else{ goto
_LL37;} _LL58: _temp57= _temp21.f2; if( _temp57 ==( void*) Cyc_Absyn_Public){
goto _LL36;} else{ goto _LL37;} _LL37: _LL64: _temp63= _temp21.f1; if( _temp63
==( void*) Cyc_Absyn_Abstract){ goto _LL62;} else{ goto _LL39;} _LL62: _temp61=
_temp21.f2; if( _temp61 ==( void*) Cyc_Absyn_Abstract){ goto _LL38;} else{ goto
_LL39;} _LL39: goto _LL40; _LL24: goto _LL22; _LL26: goto _LL28; _LL28: Cyc_Tcdecl_merge_scope_err(
s0, s1, t, v, loc, msg); return({ struct _tuple3 _temp65; _temp65.f1= s1;
_temp65.f2= 0; _temp65;}); _LL30: s1= s0; goto _LL22; _LL32: goto _LL22; _LL34:
goto _LL22; _LL36: goto _LL22; _LL38: goto _LL22; _LL40: Cyc_Tcdecl_merge_scope_err(
s0, s1, t, v, loc, msg); return({ struct _tuple3 _temp66; _temp66.f1= s1;
_temp66.f2= 0; _temp66;}); _LL22:;} return({ struct _tuple3 _temp67; _temp67.f1=
s1; _temp67.f2= 1; _temp67;});} static int Cyc_Tcdecl_check_type( void* t0, void*
t1){ return Cyc_Tcutil_unify( t0, t1);} static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp* e){ void* _temp68=( void*) e->r; void* _temp74; int
_temp76; _LL70: if(*(( int*) _temp68) == Cyc_Absyn_Const_e){ _LL75: _temp74=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp68)->f1; if(( unsigned int)
_temp74 > 1u?*(( int*) _temp74) == Cyc_Absyn_Int_c: 0){ _LL77: _temp76=(( struct
Cyc_Absyn_Int_c_struct*) _temp74)->f2; goto _LL71;} else{ goto _LL72;}} else{
goto _LL72;} _LL72: goto _LL73; _LL71: return( unsigned int) _temp76; _LL73:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp78=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp78[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp79; _temp79.tag= Cyc_Core_InvalidArg; _temp79.f1=
_tag_arr("Tcdecl::get_uint_const_value", sizeof( unsigned char), 29u); _temp79;});
_temp78;})); _LL69:;} inline static int Cyc_Tcdecl_check_tvs( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1, struct _tagged_arr t, struct _tagged_arr v,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( tvs0) !=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr(* _temp80)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp85= _tag_arr("%s %s has a different number of type parameters",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp87; _temp87.tag=
Cyc_Stdio_String_pa; _temp87.f1=( struct _tagged_arr) t;{ void* _temp83=( void*)&
_temp87; struct Cyc_Stdio_String_pa_struct _temp86; _temp86.tag= Cyc_Stdio_String_pa;
_temp86.f1=( struct _tagged_arr) v;{ void* _temp84=( void*)& _temp86; void*
_temp81[ 2u]={ _temp83, _temp84}; struct _tagged_arr _temp82={( void*) _temp81,(
void*) _temp81,( void*)( _temp81 + 2u)}; _temp80( _temp85, _temp82);}}}));
return 0;}{ struct Cyc_List_List* _temp88= tvs0; struct Cyc_List_List* _temp89=
tvs1; for( 0; _temp88 != 0;( _temp88=(( struct Cyc_List_List*) _check_null(
_temp88))->tl, _temp89=(( struct Cyc_List_List*) _check_null( _temp89))->tl)){
struct Cyc_Absyn_Conref* _temp90= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp88))->hd)->kind); struct Cyc_Absyn_Conref*
_temp91= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp89))->hd)->kind); if( Cyc_Absyn_conref_val( _temp90) != Cyc_Absyn_conref_val(
_temp91)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(*
_temp92)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct
_tagged_arr _temp100= _tag_arr("%s %s has a different kind (%s) for type parameter %s (%s)",
sizeof( unsigned char), 59u); struct Cyc_Stdio_String_pa_struct _temp105;
_temp105.tag= Cyc_Stdio_String_pa; _temp105.f1=( struct _tagged_arr) t;{ void*
_temp95=( void*)& _temp105; struct Cyc_Stdio_String_pa_struct _temp104; _temp104.tag=
Cyc_Stdio_String_pa; _temp104.f1=( struct _tagged_arr) v;{ void* _temp96=( void*)&
_temp104; struct Cyc_Stdio_String_pa_struct _temp103; _temp103.tag= Cyc_Stdio_String_pa;
_temp103.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( _temp90);{ void*
_temp97=( void*)& _temp103; struct Cyc_Stdio_String_pa_struct _temp102; _temp102.tag=
Cyc_Stdio_String_pa; _temp102.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp88))->hd)->name;{ void* _temp98=( void*)&
_temp102; struct Cyc_Stdio_String_pa_struct _temp101; _temp101.tag= Cyc_Stdio_String_pa;
_temp101.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( _temp91);{ void*
_temp99=( void*)& _temp101; void* _temp93[ 5u]={ _temp95, _temp96, _temp97,
_temp98, _temp99}; struct _tagged_arr _temp94={( void*) _temp93,( void*) _temp93,(
void*)( _temp93 + 5u)}; _temp92( _temp100, _temp94);}}}}}})); return 0;}} return
1;}} inline static int Cyc_Tcdecl_check_atts( struct Cyc_List_List* atts0,
struct Cyc_List_List* atts1, struct _tagged_arr t, struct _tagged_arr v, struct
Cyc_Position_Segment* loc, struct _tagged_arr* msg){ if( ! Cyc_Tcutil_same_atts(
atts0, atts1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp106)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp111= _tag_arr("%s %s has different attributes", sizeof(
unsigned char), 31u); struct Cyc_Stdio_String_pa_struct _temp113; _temp113.tag=
Cyc_Stdio_String_pa; _temp113.f1=( struct _tagged_arr) t;{ void* _temp109=( void*)&
_temp113; struct Cyc_Stdio_String_pa_struct _temp112; _temp112.tag= Cyc_Stdio_String_pa;
_temp112.f1=( struct _tagged_arr) v;{ void* _temp110=( void*)& _temp112; void*
_temp107[ 2u]={ _temp109, _temp110}; struct _tagged_arr _temp108={( void*)
_temp107,( void*) _temp107,( void*)( _temp107 + 2u)}; _temp106( _temp111,
_temp108);}}})); return 0;} else{ return 1;}} struct _tuple5{ struct Cyc_Absyn_Tvar*
f1; void* f2; } ; inline static struct Cyc_List_List* Cyc_Tcdecl_build_tvs_map(
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1){ struct Cyc_List_List*
_temp114= 0; for( 0; tvs0 != 0;( tvs0=(( struct Cyc_List_List*) _check_null(
tvs0))->tl, tvs1=(( struct Cyc_List_List*) _check_null( tvs1))->tl)){ _temp114=({
struct Cyc_List_List* _temp115=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp115->hd=( void*)({ struct _tuple5* _temp116=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp116->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs1))->hd; _temp116->f2=( void*)({ struct
Cyc_Absyn_VarType_struct* _temp117=( struct Cyc_Absyn_VarType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_VarType_struct)); _temp117[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp118; _temp118.tag= Cyc_Absyn_VarType; _temp118.f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs0))->hd; _temp118;}); _temp117;});
_temp116;}); _temp115->tl= _temp114; _temp115;});} return _temp114;} struct
_tuple6{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_struct_or_union_decl( struct Cyc_Absyn_Structdecl* d0, struct
Cyc_Absyn_Structdecl* d1, struct _tagged_arr t, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct _tagged_arr _temp119= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d0->name))->v); int
_temp120= 1; if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs, t, _temp119, loc, msg)){
return 0;}{ int _temp123; void* _temp125; struct _tuple3 _temp121= Cyc_Tcdecl_merge_scope((
void*) d0->sc,( void*) d1->sc, t, _temp119, loc, msg); _LL126: _temp125=
_temp121.f1; goto _LL124; _LL124: _temp123= _temp121.f2; goto _LL122; _LL122:
if( ! _temp123){ _temp120= 0;} if( ! Cyc_Tcdecl_check_atts( d0->attributes, d1->attributes,
t, _temp119, loc, msg)){ _temp120= 0;}{ struct Cyc_Absyn_Structdecl* d2;{ struct
_tuple6 _temp128=({ struct _tuple6 _temp127; _temp127.f1= d0->fields; _temp127.f2=
d1->fields; _temp127;}); struct Cyc_Core_Opt* _temp136; struct Cyc_Core_Opt*
_temp138; struct Cyc_Core_Opt* _temp140; struct Cyc_Core_Opt _temp142; struct
Cyc_List_List* _temp143; struct Cyc_Core_Opt* _temp145; struct Cyc_Core_Opt
_temp147; struct Cyc_List_List* _temp148; _LL130: _LL137: _temp136= _temp128.f2;
if( _temp136 == 0){ goto _LL131;} else{ goto _LL132;} _LL132: _LL139: _temp138=
_temp128.f1; if( _temp138 == 0){ goto _LL133;} else{ goto _LL134;} _LL134:
_LL146: _temp145= _temp128.f1; if( _temp145 == 0){ goto _LL129;} else{ _temp147=*
_temp145; _LL149: _temp148=( struct Cyc_List_List*) _temp147.v; goto _LL141;}
_LL141: _temp140= _temp128.f2; if( _temp140 == 0){ goto _LL129;} else{ _temp142=*
_temp140; _LL144: _temp143=( struct Cyc_List_List*) _temp142.v; goto _LL135;}
_LL131: d2= d0; goto _LL129; _LL133: d2= d1; goto _LL129; _LL135: { struct Cyc_List_List*
_temp150= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); for( 0; _temp148 != 0?
_temp143 != 0: 0;( _temp148=(( struct Cyc_List_List*) _check_null( _temp148))->tl,
_temp143=(( struct Cyc_List_List*) _check_null( _temp143))->tl)){ struct Cyc_Absyn_Structfield
_temp153; struct Cyc_List_List* _temp154; struct Cyc_Core_Opt* _temp156; void*
_temp158; struct Cyc_Absyn_Tqual _temp160; struct _tagged_arr* _temp162; struct
Cyc_Absyn_Structfield* _temp151=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp148))->hd; _temp153=* _temp151; _LL163: _temp162= _temp153.name;
goto _LL161; _LL161: _temp160= _temp153.tq; goto _LL159; _LL159: _temp158=( void*)
_temp153.type; goto _LL157; _LL157: _temp156= _temp153.width; goto _LL155;
_LL155: _temp154= _temp153.attributes; goto _LL152; _LL152: { struct Cyc_Absyn_Structfield
_temp166; struct Cyc_List_List* _temp167; struct Cyc_Core_Opt* _temp169; void*
_temp171; struct Cyc_Absyn_Tqual _temp173; struct _tagged_arr* _temp175; struct
Cyc_Absyn_Structfield* _temp164=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp143))->hd; _temp166=* _temp164; _LL176: _temp175= _temp166.name;
goto _LL174; _LL174: _temp173= _temp166.tq; goto _LL172; _LL172: _temp171=( void*)
_temp166.type; goto _LL170; _LL170: _temp169= _temp166.width; goto _LL168;
_LL168: _temp167= _temp166.attributes; goto _LL165; _LL165: if( Cyc_String_zstrptrcmp(
_temp162, _temp175) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr(* _temp177)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp184= _tag_arr("%s %s : field name mismatch %s != %s",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp188;
_temp188.tag= Cyc_Stdio_String_pa; _temp188.f1=( struct _tagged_arr) t;{ void*
_temp180=( void*)& _temp188; struct Cyc_Stdio_String_pa_struct _temp187;
_temp187.tag= Cyc_Stdio_String_pa; _temp187.f1=( struct _tagged_arr) _temp119;{
void* _temp181=( void*)& _temp187; struct Cyc_Stdio_String_pa_struct _temp186;
_temp186.tag= Cyc_Stdio_String_pa; _temp186.f1=( struct _tagged_arr)* _temp162;{
void* _temp182=( void*)& _temp186; struct Cyc_Stdio_String_pa_struct _temp185;
_temp185.tag= Cyc_Stdio_String_pa; _temp185.f1=( struct _tagged_arr)* _temp175;{
void* _temp183=( void*)& _temp185; void* _temp178[ 4u]={ _temp180, _temp181,
_temp182, _temp183}; struct _tagged_arr _temp179={( void*) _temp178,( void*)
_temp178,( void*)( _temp178 + 4u)}; _temp177( _temp184, _temp179);}}}}}));
return 0;} if( ! Cyc_Tcutil_same_atts( _temp154, _temp167)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp189)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr
_temp195= _tag_arr("%s %s : attribute mismatch on field %s", sizeof(
unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp198; _temp198.tag=
Cyc_Stdio_String_pa; _temp198.f1=( struct _tagged_arr) t;{ void* _temp192=( void*)&
_temp198; struct Cyc_Stdio_String_pa_struct _temp197; _temp197.tag= Cyc_Stdio_String_pa;
_temp197.f1=( struct _tagged_arr) _temp119;{ void* _temp193=( void*)& _temp197;
struct Cyc_Stdio_String_pa_struct _temp196; _temp196.tag= Cyc_Stdio_String_pa;
_temp196.f1=( struct _tagged_arr)* _temp162;{ void* _temp194=( void*)& _temp196;
void* _temp190[ 3u]={ _temp192, _temp193, _temp194}; struct _tagged_arr _temp191={(
void*) _temp190,( void*) _temp190,( void*)( _temp190 + 3u)}; _temp189( _temp195,
_temp191);}}}})); _temp120= 0;} if( ! Cyc_Tcutil_equal_tqual( _temp160, _temp173)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp199)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct
_tagged_arr _temp205= _tag_arr("%s %s : qualifier mismatch on field %s", sizeof(
unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp208; _temp208.tag=
Cyc_Stdio_String_pa; _temp208.f1=( struct _tagged_arr) t;{ void* _temp202=( void*)&
_temp208; struct Cyc_Stdio_String_pa_struct _temp207; _temp207.tag= Cyc_Stdio_String_pa;
_temp207.f1=( struct _tagged_arr) _temp119;{ void* _temp203=( void*)& _temp207;
struct Cyc_Stdio_String_pa_struct _temp206; _temp206.tag= Cyc_Stdio_String_pa;
_temp206.f1=( struct _tagged_arr)* _temp162;{ void* _temp204=( void*)& _temp206;
void* _temp200[ 3u]={ _temp202, _temp203, _temp204}; struct _tagged_arr _temp201={(
void*) _temp200,( void*) _temp200,( void*)( _temp200 + 3u)}; _temp199( _temp205,
_temp201);}}}})); _temp120= 0;} if(((( _temp156 != 0? _temp169 != 0: 0)? Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp156))->v) !=
Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp169))->v): 0)? 1:( _temp156 == 0? _temp169 != 0: 0))? 1:(
_temp156 != 0? _temp169 == 0: 0)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr(* _temp209)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp215= _tag_arr("%s %s : bitfield mismatch on field %s",
sizeof( unsigned char), 38u); struct Cyc_Stdio_String_pa_struct _temp218;
_temp218.tag= Cyc_Stdio_String_pa; _temp218.f1=( struct _tagged_arr) t;{ void*
_temp212=( void*)& _temp218; struct Cyc_Stdio_String_pa_struct _temp217;
_temp217.tag= Cyc_Stdio_String_pa; _temp217.f1=( struct _tagged_arr) _temp119;{
void* _temp213=( void*)& _temp217; struct Cyc_Stdio_String_pa_struct _temp216;
_temp216.tag= Cyc_Stdio_String_pa; _temp216.f1=( struct _tagged_arr)* _temp162;{
void* _temp214=( void*)& _temp216; void* _temp210[ 3u]={ _temp212, _temp213,
_temp214}; struct _tagged_arr _temp211={( void*) _temp210,( void*) _temp210,(
void*)( _temp210 + 3u)}; _temp209( _temp215, _temp211);}}}})); _temp120= 0;}{
void* subst_t1= Cyc_Tcutil_substitute( _temp150, _temp171); if( ! Cyc_Tcdecl_check_type(
_temp158, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp219)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp227= _tag_arr("%s %s : type mismatch on field %s: %s != %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp232;
_temp232.tag= Cyc_Stdio_String_pa; _temp232.f1=( struct _tagged_arr) t;{ void*
_temp222=( void*)& _temp232; struct Cyc_Stdio_String_pa_struct _temp231;
_temp231.tag= Cyc_Stdio_String_pa; _temp231.f1=( struct _tagged_arr) _temp119;{
void* _temp223=( void*)& _temp231; struct Cyc_Stdio_String_pa_struct _temp230;
_temp230.tag= Cyc_Stdio_String_pa; _temp230.f1=( struct _tagged_arr)* _temp162;{
void* _temp224=( void*)& _temp230; struct Cyc_Stdio_String_pa_struct _temp229;
_temp229.tag= Cyc_Stdio_String_pa; _temp229.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp158);{ void* _temp225=( void*)& _temp229; struct Cyc_Stdio_String_pa_struct
_temp228; _temp228.tag= Cyc_Stdio_String_pa; _temp228.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( subst_t1);{ void* _temp226=( void*)& _temp228; void*
_temp220[ 5u]={ _temp222, _temp223, _temp224, _temp225, _temp226}; struct
_tagged_arr _temp221={( void*) _temp220,( void*) _temp220,( void*)( _temp220 + 5u)};
_temp219( _temp227, _temp221);}}}}}})); _temp120= 0;}}}} if( _temp148 != 0){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp233)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr
_temp239= _tag_arr("%s %s is missing field %s", sizeof( unsigned char), 26u);
struct Cyc_Stdio_String_pa_struct _temp242; _temp242.tag= Cyc_Stdio_String_pa;
_temp242.f1=( struct _tagged_arr) t;{ void* _temp236=( void*)& _temp242; struct
Cyc_Stdio_String_pa_struct _temp241; _temp241.tag= Cyc_Stdio_String_pa; _temp241.f1=(
struct _tagged_arr) _temp119;{ void* _temp237=( void*)& _temp241; struct Cyc_Stdio_String_pa_struct
_temp240; _temp240.tag= Cyc_Stdio_String_pa; _temp240.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp148))->hd)->name;{
void* _temp238=( void*)& _temp240; void* _temp234[ 3u]={ _temp236, _temp237,
_temp238}; struct _tagged_arr _temp235={( void*) _temp234,( void*) _temp234,(
void*)( _temp234 + 3u)}; _temp233( _temp239, _temp235);}}}})); _temp120= 0;} if(
_temp143 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp243)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp249= _tag_arr("%s %s has extra field %s", sizeof(
unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp252; _temp252.tag=
Cyc_Stdio_String_pa; _temp252.f1=( struct _tagged_arr) t;{ void* _temp246=( void*)&
_temp252; struct Cyc_Stdio_String_pa_struct _temp251; _temp251.tag= Cyc_Stdio_String_pa;
_temp251.f1=( struct _tagged_arr) _temp119;{ void* _temp247=( void*)& _temp251;
struct Cyc_Stdio_String_pa_struct _temp250; _temp250.tag= Cyc_Stdio_String_pa;
_temp250.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp143))->hd)->name;{ void* _temp248=( void*)& _temp250; void*
_temp244[ 3u]={ _temp246, _temp247, _temp248}; struct _tagged_arr _temp245={(
void*) _temp244,( void*) _temp244,( void*)( _temp244 + 3u)}; _temp243( _temp249,
_temp245);}}}})); _temp120= 0;} d2= d0; goto _LL129;} _LL129:;} if( ! _temp120){
return 0;} if( _temp125 ==( void*) d2->sc){ return( struct Cyc_Absyn_Structdecl*)
d2;} else{ d2=({ struct Cyc_Absyn_Structdecl* _temp253=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp253[ 0]=* d2; _temp253;});(
void*)( d2->sc=( void*) _temp125); return( struct Cyc_Absyn_Structdecl*) d2;}}}}
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
_tagged_arr(* _temp254)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp261= _tag_arr("%s %s: field name mismatch %s != %s",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp265;
_temp265.tag= Cyc_Stdio_String_pa; _temp265.f1=( struct _tagged_arr) t;{ void*
_temp257=( void*)& _temp265; struct Cyc_Stdio_String_pa_struct _temp264;
_temp264.tag= Cyc_Stdio_String_pa; _temp264.f1=( struct _tagged_arr) v;{ void*
_temp258=( void*)& _temp264; struct Cyc_Stdio_String_pa_struct _temp263;
_temp263.tag= Cyc_Stdio_String_pa; _temp263.f1=( struct _tagged_arr)*(* f1->name).f2;{
void* _temp259=( void*)& _temp263; struct Cyc_Stdio_String_pa_struct _temp262;
_temp262.tag= Cyc_Stdio_String_pa; _temp262.f1=( struct _tagged_arr)*(* f0->name).f2;{
void* _temp260=( void*)& _temp262; void* _temp255[ 4u]={ _temp257, _temp258,
_temp259, _temp260}; struct _tagged_arr _temp256={( void*) _temp255,( void*)
_temp255,( void*)( _temp255 + 4u)}; _temp254( _temp261, _temp256);}}}}}));
return 0;}{ struct _tagged_arr _temp266=*(* f0->name).f2; int _temp277; void*
_temp279; struct _tuple3 _temp275= Cyc_Tcdecl_merge_scope(( void*) f0->sc,( void*)
f1->sc,( struct _tagged_arr)({ struct _tagged_arr(* _temp267)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr
_temp272= _tag_arr("in %s %s, field", sizeof( unsigned char), 16u); struct Cyc_Stdio_String_pa_struct
_temp274; _temp274.tag= Cyc_Stdio_String_pa; _temp274.f1=( struct _tagged_arr) t;{
void* _temp270=( void*)& _temp274; struct Cyc_Stdio_String_pa_struct _temp273;
_temp273.tag= Cyc_Stdio_String_pa; _temp273.f1=( struct _tagged_arr) v;{ void*
_temp271=( void*)& _temp273; void* _temp268[ 2u]={ _temp270, _temp271}; struct
_tagged_arr _temp269={( void*) _temp268,( void*) _temp268,( void*)( _temp268 + 2u)};
_temp267( _temp272, _temp269);}}}), _temp266, loc, msg); _LL280: _temp279=
_temp275.f1; goto _LL278; _LL278: _temp277= _temp275.f2; goto _LL276; _LL276: {
struct Cyc_List_List* _temp281= f0->tvs; struct Cyc_List_List* _temp282= f1->tvs;
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp281) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp282)){ Cyc_Tcdecl_merr( loc,
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp283)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp289=
_tag_arr("%s %s, field %s: type parameter number mismatch", sizeof(
unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp292; _temp292.tag=
Cyc_Stdio_String_pa; _temp292.f1=( struct _tagged_arr) t;{ void* _temp286=( void*)&
_temp292; struct Cyc_Stdio_String_pa_struct _temp291; _temp291.tag= Cyc_Stdio_String_pa;
_temp291.f1=( struct _tagged_arr) v;{ void* _temp287=( void*)& _temp291; struct
Cyc_Stdio_String_pa_struct _temp290; _temp290.tag= Cyc_Stdio_String_pa; _temp290.f1=(
struct _tagged_arr) _temp266;{ void* _temp288=( void*)& _temp290; void* _temp284[
3u]={ _temp286, _temp287, _temp288}; struct _tagged_arr _temp285={( void*)
_temp284,( void*) _temp284,( void*)( _temp284 + 3u)}; _temp283( _temp289,
_temp285);}}}})); return 0;}{ struct Cyc_List_List* _temp293= Cyc_Tcdecl_build_tvs_map(
_temp281, _temp282); struct Cyc_List_List* _temp294= f0->typs; struct Cyc_List_List*
_temp295= f1->typs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp294) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp295)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp296)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct
_tagged_arr _temp302= _tag_arr("%s %s, field %s: parameter number mismatch",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp305;
_temp305.tag= Cyc_Stdio_String_pa; _temp305.f1=( struct _tagged_arr) t;{ void*
_temp299=( void*)& _temp305; struct Cyc_Stdio_String_pa_struct _temp304;
_temp304.tag= Cyc_Stdio_String_pa; _temp304.f1=( struct _tagged_arr) v;{ void*
_temp300=( void*)& _temp304; struct Cyc_Stdio_String_pa_struct _temp303;
_temp303.tag= Cyc_Stdio_String_pa; _temp303.f1=( struct _tagged_arr) _temp266;{
void* _temp301=( void*)& _temp303; void* _temp297[ 3u]={ _temp299, _temp300,
_temp301}; struct _tagged_arr _temp298={( void*) _temp297,( void*) _temp297,(
void*)( _temp297 + 3u)}; _temp296( _temp302, _temp298);}}}})); _temp277= 0;}
for( 0; _temp294 != 0;( _temp294=(( struct Cyc_List_List*) _check_null( _temp294))->tl,
_temp295=(( struct Cyc_List_List*) _check_null( _temp295))->tl)){ if( ! Cyc_Tcutil_equal_tqual((*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp294))->hd)).f1,(*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp295))->hd)).f1)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp306)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct
_tagged_arr _temp312= _tag_arr("%s %s, field %s: parameter qualifier", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp315; _temp315.tag=
Cyc_Stdio_String_pa; _temp315.f1=( struct _tagged_arr) t;{ void* _temp309=( void*)&
_temp315; struct Cyc_Stdio_String_pa_struct _temp314; _temp314.tag= Cyc_Stdio_String_pa;
_temp314.f1=( struct _tagged_arr) v;{ void* _temp310=( void*)& _temp314; struct
Cyc_Stdio_String_pa_struct _temp313; _temp313.tag= Cyc_Stdio_String_pa; _temp313.f1=(
struct _tagged_arr) _temp266;{ void* _temp311=( void*)& _temp313; void* _temp307[
3u]={ _temp309, _temp310, _temp311}; struct _tagged_arr _temp308={( void*)
_temp307,( void*) _temp307,( void*)( _temp307 + 3u)}; _temp306( _temp312,
_temp308);}}}})); _temp277= 0;}{ void* subst_t1= Cyc_Tcutil_substitute((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp293, inst),(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null(
_temp295))->hd)).f2); if( ! Cyc_Tcdecl_check_type((*(( struct _tuple7*)(( struct
Cyc_List_List*) _check_null( _temp294))->hd)).f2, subst_t1)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp316)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr
_temp324= _tag_arr("%s %s, field %s: parameter type mismatch %s != %s", sizeof(
unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp329; _temp329.tag=
Cyc_Stdio_String_pa; _temp329.f1=( struct _tagged_arr) t;{ void* _temp319=( void*)&
_temp329; struct Cyc_Stdio_String_pa_struct _temp328; _temp328.tag= Cyc_Stdio_String_pa;
_temp328.f1=( struct _tagged_arr) v;{ void* _temp320=( void*)& _temp328; struct
Cyc_Stdio_String_pa_struct _temp327; _temp327.tag= Cyc_Stdio_String_pa; _temp327.f1=(
struct _tagged_arr) _temp266;{ void* _temp321=( void*)& _temp327; struct Cyc_Stdio_String_pa_struct
_temp326; _temp326.tag= Cyc_Stdio_String_pa; _temp326.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string((*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp294))->hd)).f2);{ void* _temp322=( void*)& _temp326; struct
Cyc_Stdio_String_pa_struct _temp325; _temp325.tag= Cyc_Stdio_String_pa; _temp325.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( subst_t1);{ void* _temp323=( void*)&
_temp325; void* _temp317[ 5u]={ _temp319, _temp320, _temp321, _temp322, _temp323};
struct _tagged_arr _temp318={( void*) _temp317,( void*) _temp317,( void*)(
_temp317 + 5u)}; _temp316( _temp324, _temp318);}}}}}})); _temp277= 0;}}} if( !
_temp277){ return 0;} if(( void*) f0->sc != _temp279){ struct Cyc_Absyn_Tunionfield*
_temp330=({ struct Cyc_Absyn_Tunionfield* _temp331=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp331[ 0]=* f0; _temp331;});(
void*)((( struct Cyc_Absyn_Tunionfield*) _check_null( _temp330))->sc=( void*)
_temp279); return _temp330;} else{ return( struct Cyc_Absyn_Tunionfield*) f0;}}}}}
static int Cyc_Tcdecl_substitute_tunionfield_f1( struct _tagged_arr* name,
struct Cyc_Absyn_Tvar* x){ return Cyc_String_strptrcmp( x->name, name) == 0;}
static struct _tuple7* Cyc_Tcdecl_substitute_tunionfield_f2( struct Cyc_List_List*
inst, struct _tuple7* x){ struct _tuple7 _temp334; void* _temp335; struct Cyc_Absyn_Tqual
_temp337; struct _tuple7* _temp332= x; _temp334=* _temp332; _LL338: _temp337=
_temp334.f1; goto _LL336; _LL336: _temp335= _temp334.f2; goto _LL333; _LL333:
return({ struct _tuple7* _temp339=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7)); _temp339->f1= _temp337; _temp339->f2= Cyc_Tcutil_substitute( inst,
_temp335); _temp339;});} static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List* d0tvs, struct Cyc_List_List* d1tvs, struct Cyc_List_List*
inst1, struct Cyc_Absyn_Tunionfield* f1){ struct Cyc_Absyn_Tunionfield* _temp340=({
struct Cyc_Absyn_Tunionfield* _temp348=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp348[ 0]=* f1; _temp348;});
struct Cyc_List_List* _temp341= 0;{ struct Cyc_List_List* _temp342= f1->tvs;
for( 0; _temp342 != 0; _temp342=(( struct Cyc_List_List*) _check_null( _temp342))->tl){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp342))->hd; if((( int(*)( int(* pred)( struct _tagged_arr*,
struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcdecl_substitute_tunionfield_f1, tv->name, d0tvs)? 1:(( int(*)( int(* pred)(
struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1, tv->name,
d1tvs)){ int i= 0; struct _tagged_arr base=* tv->name; struct _tagged_arr*
new_name; do { new_name=({ struct _tagged_arr* _temp343=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp343[ 0]=( struct _tagged_arr) Cyc_String_strconcat(
base,( struct _tagged_arr) Cyc_Core_string_of_int( i)); _temp343;}); i ++;} while (((
int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct
_tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
new_name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)); tv=({ struct Cyc_Absyn_Tvar* _temp344=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp344[ 0]=* tv; _temp344;}); tv->name=
new_name;} _temp341=({ struct Cyc_List_List* _temp345=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp345->hd=( void*) tv; _temp345->tl=
_temp341; _temp345;});}} _temp341=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp341); _temp340->tvs= _temp341;{ struct Cyc_List_List*
_temp346= Cyc_Tcdecl_build_tvs_map( _temp341, f1->tvs); struct Cyc_List_List*
_temp347=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp346, inst1); _temp340->typs=(( struct Cyc_List_List*(*)(
struct _tuple7*(* f)( struct Cyc_List_List*, struct _tuple7*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcdecl_substitute_tunionfield_f2,
_temp347, f1->typs); return _temp340;}} static struct Cyc_List_List* Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1, int* res, int* incl,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp357=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp357[ 0]= 0; _temp357;}); struct Cyc_List_List** _temp349= f2sp; int cmp= -
1; for( 0; f0s != 0? f1s != 0: 0; f1s=(( struct Cyc_List_List*) _check_null( f1s))->tl){
while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)->name,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd)->name)) < 0: 0) { struct Cyc_List_List*
_temp350=({ struct Cyc_List_List* _temp351=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp351->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd); _temp351->tl= 0; _temp351;});*
_temp349= _temp350; _temp349=&(( struct Cyc_List_List*) _check_null( _temp350))->tl;
f0s=(( struct Cyc_List_List*) _check_null( f0s))->tl;} if( f0s == 0? 1: cmp > 0){*
incl= 0;{ struct Cyc_List_List* _temp352=({ struct Cyc_List_List* _temp353=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp353->hd=(
void*) Cyc_Tcdecl_substitute_tunionfield( tvs0, tvs1, inst,( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd); _temp353->tl= 0; _temp353;});*
_temp349= _temp352; _temp349=&(( struct Cyc_List_List*) _check_null( _temp352))->tl;}}
else{ struct Cyc_Absyn_Tunionfield* _temp354= Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f0s))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f1s))->hd,
inst, t, v, msg); if( _temp354 != 0){ if( _temp354 !=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f0s))->hd)){*
incl= 0;}{ struct Cyc_List_List* _temp355=({ struct Cyc_List_List* _temp356=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp356->hd=(
void*)(( struct Cyc_Absyn_Tunionfield*) _check_null( _temp354)); _temp356->tl= 0;
_temp356;});* _temp349= _temp355; _temp349=&(( struct Cyc_List_List*)
_check_null( _temp355))->tl;}} else{* res= 0;} f0s=(( struct Cyc_List_List*)
_check_null( f0s))->tl;}} if( f1s != 0){* incl= 0;* _temp349= f1s;} else{*
_temp349= f0s;} return* f2sp;} struct _tuple8{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; } ; static struct _tuple8 Cyc_Tcdecl_split( struct Cyc_List_List*
f){ if( f == 0){ return({ struct _tuple8 _temp358; _temp358.f1= 0; _temp358.f2=
0; _temp358;});} if((( struct Cyc_List_List*) _check_null( f))->tl == 0){ return({
struct _tuple8 _temp359; _temp359.f1= f; _temp359.f2= 0; _temp359;});}{ struct
Cyc_List_List* _temp362; struct Cyc_List_List* _temp364; struct _tuple8 _temp360=
Cyc_Tcdecl_split((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( f))->tl))->tl); _LL365: _temp364= _temp360.f1; goto _LL363; _LL363:
_temp362= _temp360.f2; goto _LL361; _LL361: return({ struct _tuple8 _temp366;
_temp366.f1=({ struct Cyc_List_List* _temp368=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp368->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( f))->hd); _temp368->tl= _temp364; _temp368;}); _temp366.f2=({
struct Cyc_List_List* _temp367=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp367->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( f))->tl))->hd); _temp367->tl=
_temp362; _temp367;}); _temp366;});}} struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List* f, int* res, struct _tagged_arr* v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct Cyc_List_List* _temp371; struct Cyc_List_List*
_temp373; struct _tuple8 _temp369=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL374: _temp373= _temp369.f1; goto _LL372; _LL372:
_temp371= _temp369.f2; goto _LL370; _LL370: if( _temp373 != 0?(( struct Cyc_List_List*)
_check_null( _temp373))->tl != 0: 0){ _temp373= Cyc_Tcdecl_sort_xtunion_fields(
_temp373, res, v, loc, msg);} if( _temp371 != 0?(( struct Cyc_List_List*)
_check_null( _temp371))->tl != 0: 0){ _temp371= Cyc_Tcdecl_sort_xtunion_fields(
_temp371, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp373,
_temp371, 0, 0, 0, res,({ int* _temp375=( int*) GC_malloc_atomic( sizeof( int));
_temp375[ 0]= 1; _temp375;}), _tag_arr("xtunion", sizeof( unsigned char), 8u),*
v, loc, msg);} struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl( struct
Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp376= Cyc_Absynpp_qvar2string(
d0->name); struct _tagged_arr t= _tag_arr("[x]tunion", sizeof( unsigned char),
10u); int _temp377= 1; if( d0->is_xtunion != d1->is_xtunion){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp378)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr
_temp384= _tag_arr("expected %s to be a %s instead of a %s", sizeof(
unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp387; _temp387.tag=
Cyc_Stdio_String_pa; _temp387.f1=( struct _tagged_arr) _temp376;{ void* _temp381=(
void*)& _temp387; struct Cyc_Stdio_String_pa_struct _temp386; _temp386.tag= Cyc_Stdio_String_pa;
_temp386.f1=( struct _tagged_arr) Cyc_Tcdecl_is_x2string( d0->is_xtunion);{ void*
_temp382=( void*)& _temp386; struct Cyc_Stdio_String_pa_struct _temp385;
_temp385.tag= Cyc_Stdio_String_pa; _temp385.f1=( struct _tagged_arr) Cyc_Tcdecl_is_x2string(
d1->is_xtunion);{ void* _temp383=( void*)& _temp385; void* _temp379[ 3u]={
_temp381, _temp382, _temp383}; struct _tagged_arr _temp380={( void*) _temp379,(
void*) _temp379,( void*)( _temp379 + 3u)}; _temp378( _temp384, _temp380);}}}}));
_temp377= 0;} else{ t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, _temp376, loc, msg)){ return 0;}{ int _temp390; void*
_temp392; struct _tuple3 _temp388= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*)
d1->sc, t, _temp376, loc, msg); _LL393: _temp392= _temp388.f1; goto _LL391;
_LL391: _temp390= _temp388.f2; goto _LL389; _LL389: if( ! _temp390){ _temp377= 0;}{
struct Cyc_Absyn_Tuniondecl* d2;{ struct _tuple6 _temp395=({ struct _tuple6
_temp394; _temp394.f1= d0->fields; _temp394.f2= d1->fields; _temp394;}); struct
Cyc_Core_Opt* _temp403; struct Cyc_Core_Opt* _temp405; struct Cyc_Core_Opt*
_temp407; struct Cyc_Core_Opt _temp409; struct Cyc_List_List* _temp410; struct
Cyc_Core_Opt* _temp412; struct Cyc_Core_Opt _temp414; struct Cyc_List_List*
_temp415; _LL397: _LL404: _temp403= _temp395.f2; if( _temp403 == 0){ goto _LL398;}
else{ goto _LL399;} _LL399: _LL406: _temp405= _temp395.f1; if( _temp405 == 0){
goto _LL400;} else{ goto _LL401;} _LL401: _LL413: _temp412= _temp395.f1; if(
_temp412 == 0){ goto _LL396;} else{ _temp414=* _temp412; _LL416: _temp415=(
struct Cyc_List_List*) _temp414.v; goto _LL408;} _LL408: _temp407= _temp395.f2;
if( _temp407 == 0){ goto _LL396;} else{ _temp409=* _temp407; _LL411: _temp410=(
struct Cyc_List_List*) _temp409.v; goto _LL402;} _LL398: d2= d0; goto _LL396;
_LL400: d2= d1; goto _LL396; _LL402: { struct Cyc_List_List* _temp417= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); if( d0->is_xtunion){ int _temp418= 1; struct Cyc_List_List*
_temp419= Cyc_Tcdecl_merge_xtunion_fields( _temp415, _temp410, _temp417, d0->tvs,
d1->tvs,& _temp377,& _temp418, t, _temp376, loc, msg); if( _temp418){ d2= d0;}
else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp420=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp420[ 0]=* d0; _temp420;});(
void*)( d2->sc=( void*) _temp392); d2->fields=({ struct Cyc_Core_Opt* _temp421=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp421->v=(
void*) _temp419; _temp421;});}} else{ for( 0; _temp415 != 0? _temp410 != 0: 0;(
_temp415=(( struct Cyc_List_List*) _check_null( _temp415))->tl, _temp410=((
struct Cyc_List_List*) _check_null( _temp410))->tl)){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp415))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp410))->hd,
_temp417, t, _temp376, msg);} if( _temp415 != 0){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct _tagged_arr(* _temp422)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp428= _tag_arr("%s %s has extra field %s",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp431;
_temp431.tag= Cyc_Stdio_String_pa; _temp431.f1=( struct _tagged_arr) t;{ void*
_temp425=( void*)& _temp431; struct Cyc_Stdio_String_pa_struct _temp430;
_temp430.tag= Cyc_Stdio_String_pa; _temp430.f1=( struct _tagged_arr) _temp376;{
void* _temp426=( void*)& _temp430; struct Cyc_Stdio_String_pa_struct _temp429;
_temp429.tag= Cyc_Stdio_String_pa; _temp429.f1=( struct _tagged_arr)*(*(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp410))->hd)->name).f2;{
void* _temp427=( void*)& _temp429; void* _temp423[ 3u]={ _temp425, _temp426,
_temp427}; struct _tagged_arr _temp424={( void*) _temp423,( void*) _temp423,(
void*)( _temp423 + 3u)}; _temp422( _temp428, _temp424);}}}})); _temp377= 0;} if(
_temp410 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp432)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp438= _tag_arr("%s %s is missing field %s", sizeof(
unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp441; _temp441.tag=
Cyc_Stdio_String_pa; _temp441.f1=( struct _tagged_arr) t;{ void* _temp435=( void*)&
_temp441; struct Cyc_Stdio_String_pa_struct _temp440; _temp440.tag= Cyc_Stdio_String_pa;
_temp440.f1=( struct _tagged_arr) _temp376;{ void* _temp436=( void*)& _temp440;
struct Cyc_Stdio_String_pa_struct _temp439; _temp439.tag= Cyc_Stdio_String_pa;
_temp439.f1=( struct _tagged_arr)*(*(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp410))->hd)->name).f2;{ void* _temp437=( void*)&
_temp439; void* _temp433[ 3u]={ _temp435, _temp436, _temp437}; struct
_tagged_arr _temp434={( void*) _temp433,( void*) _temp433,( void*)( _temp433 + 3u)};
_temp432( _temp438, _temp434);}}}})); _temp377= 0;} d2= d0;} goto _LL396;}
_LL396:;} if( ! _temp377){ return 0;} if( _temp392 ==( void*) d2->sc){ return(
struct Cyc_Absyn_Tuniondecl*) d2;} else{ d2=({ struct Cyc_Absyn_Tuniondecl*
_temp442=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp442[ 0]=* d2; _temp442;});( void*)( d2->sc=( void*) _temp392); return(
struct Cyc_Absyn_Tuniondecl*) d2;}}}} struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp443= Cyc_Absynpp_qvar2string(
d0->name); int _temp444= 1; int _temp447; void* _temp449; struct _tuple3
_temp445= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc, _tag_arr("enum",
sizeof( unsigned char), 5u), _temp443, loc, msg); _LL450: _temp449= _temp445.f1;
goto _LL448; _LL448: _temp447= _temp445.f2; goto _LL446; _LL446: if( ! _temp447){
_temp444= 0;}{ struct Cyc_Absyn_Enumdecl* d2;{ struct _tuple6 _temp452=({ struct
_tuple6 _temp451; _temp451.f1= d0->fields; _temp451.f2= d1->fields; _temp451;});
struct Cyc_Core_Opt* _temp460; struct Cyc_Core_Opt* _temp462; struct Cyc_Core_Opt*
_temp464; struct Cyc_Core_Opt _temp466; struct Cyc_List_List* _temp467; struct
Cyc_Core_Opt* _temp469; struct Cyc_Core_Opt _temp471; struct Cyc_List_List*
_temp472; _LL454: _LL461: _temp460= _temp452.f2; if( _temp460 == 0){ goto _LL455;}
else{ goto _LL456;} _LL456: _LL463: _temp462= _temp452.f1; if( _temp462 == 0){
goto _LL457;} else{ goto _LL458;} _LL458: _LL470: _temp469= _temp452.f1; if(
_temp469 == 0){ goto _LL453;} else{ _temp471=* _temp469; _LL473: _temp472=(
struct Cyc_List_List*) _temp471.v; goto _LL465;} _LL465: _temp464= _temp452.f2;
if( _temp464 == 0){ goto _LL453;} else{ _temp466=* _temp464; _LL468: _temp467=(
struct Cyc_List_List*) _temp466.v; goto _LL459;} _LL455: d2= d0; goto _LL453;
_LL457: d2= d1; goto _LL453; _LL459: for( 0; _temp472 != 0? _temp467 != 0: 0;(
_temp472=(( struct Cyc_List_List*) _check_null( _temp472))->tl, _temp467=((
struct Cyc_List_List*) _check_null( _temp467))->tl)){ struct Cyc_Absyn_Enumfield
_temp476; struct Cyc_Position_Segment* _temp477; struct Cyc_Absyn_Exp* _temp479;
struct _tuple0* _temp481; struct Cyc_Absyn_Enumfield* _temp474=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp472))->hd; _temp476=* _temp474; _LL482:
_temp481= _temp476.name; goto _LL480; _LL480: _temp479= _temp476.tag; goto
_LL478; _LL478: _temp477= _temp476.loc; goto _LL475; _LL475: { struct Cyc_Absyn_Enumfield
_temp485; struct Cyc_Position_Segment* _temp486; struct Cyc_Absyn_Exp* _temp488;
struct _tuple0* _temp490; struct Cyc_Absyn_Enumfield* _temp483=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp467))->hd; _temp485=* _temp483; _LL491:
_temp490= _temp485.name; goto _LL489; _LL489: _temp488= _temp485.tag; goto
_LL487; _LL487: _temp486= _temp485.loc; goto _LL484; _LL484: if( Cyc_String_zstrptrcmp((*
_temp490).f2,(* _temp481).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr(* _temp492)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp498= _tag_arr("enum %s: field name mismatch %s != %s",
sizeof( unsigned char), 38u); struct Cyc_Stdio_String_pa_struct _temp501;
_temp501.tag= Cyc_Stdio_String_pa; _temp501.f1=( struct _tagged_arr) _temp443;{
void* _temp495=( void*)& _temp501; struct Cyc_Stdio_String_pa_struct _temp500;
_temp500.tag= Cyc_Stdio_String_pa; _temp500.f1=( struct _tagged_arr)*(* _temp490).f2;{
void* _temp496=( void*)& _temp500; struct Cyc_Stdio_String_pa_struct _temp499;
_temp499.tag= Cyc_Stdio_String_pa; _temp499.f1=( struct _tagged_arr)*(* _temp481).f2;{
void* _temp497=( void*)& _temp499; void* _temp493[ 3u]={ _temp495, _temp496,
_temp497}; struct _tagged_arr _temp494={( void*) _temp493,( void*) _temp493,(
void*)( _temp493 + 3u)}; _temp492( _temp498, _temp494);}}}})); _temp444= 0;} if(
Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*) _check_null( _temp479))
!= Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*) _check_null(
_temp488))){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp502)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp507= _tag_arr("enum %s, field %s, value mismatch",
sizeof( unsigned char), 34u); struct Cyc_Stdio_String_pa_struct _temp509;
_temp509.tag= Cyc_Stdio_String_pa; _temp509.f1=( struct _tagged_arr) _temp443;{
void* _temp505=( void*)& _temp509; struct Cyc_Stdio_String_pa_struct _temp508;
_temp508.tag= Cyc_Stdio_String_pa; _temp508.f1=( struct _tagged_arr)*(* _temp490).f2;{
void* _temp506=( void*)& _temp508; void* _temp503[ 2u]={ _temp505, _temp506};
struct _tagged_arr _temp504={( void*) _temp503,( void*) _temp503,( void*)(
_temp503 + 2u)}; _temp502( _temp507, _temp504);}}})); _temp444= 0;}}} d2= d0;
goto _LL453; _LL453:;} if( ! _temp444){ return 0;} if(( void*) d2->sc ==
_temp449){ return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({ struct Cyc_Absyn_Enumdecl*
_temp510=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp510[ 0]=* d2; _temp510;});( void*)( d2->sc=( void*) _temp449); return(
struct Cyc_Absyn_Enumdecl*) d2;}}} inline static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void* sc0, void* t0, struct Cyc_Absyn_Tqual tq0, struct Cyc_List_List* atts0,
void* sc1, void* t1, struct Cyc_Absyn_Tqual tq1, struct Cyc_List_List* atts1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ int _temp511= 1; int _temp514; void* _temp516; struct
_tuple3 _temp512= Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg); _LL517:
_temp516= _temp512.f1; goto _LL515; _LL515: _temp514= _temp512.f2; goto _LL513;
_LL513: if( ! _temp514){ _temp511= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp518)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct
_tagged_arr _temp525= _tag_arr("%s %s has type %s instead of %s", sizeof(
unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp529; _temp529.tag=
Cyc_Stdio_String_pa; _temp529.f1=( struct _tagged_arr) t;{ void* _temp521=( void*)&
_temp529; struct Cyc_Stdio_String_pa_struct _temp528; _temp528.tag= Cyc_Stdio_String_pa;
_temp528.f1=( struct _tagged_arr) v;{ void* _temp522=( void*)& _temp528; struct
Cyc_Stdio_String_pa_struct _temp527; _temp527.tag= Cyc_Stdio_String_pa; _temp527.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp523=( void*)&
_temp527; struct Cyc_Stdio_String_pa_struct _temp526; _temp526.tag= Cyc_Stdio_String_pa;
_temp526.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t0);{ void* _temp524=(
void*)& _temp526; void* _temp519[ 4u]={ _temp521, _temp522, _temp523, _temp524};
struct _tagged_arr _temp520={( void*) _temp519,( void*) _temp519,( void*)(
_temp519 + 4u)}; _temp518( _temp525, _temp520);}}}}})); _temp511= 0;} if( ! Cyc_Tcutil_equal_tqual(
tq0, tq1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr(*
_temp530)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp535= _tag_arr("%s %s has different type qualifiers",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp537;
_temp537.tag= Cyc_Stdio_String_pa; _temp537.f1=( struct _tagged_arr) t;{ void*
_temp533=( void*)& _temp537; struct Cyc_Stdio_String_pa_struct _temp536;
_temp536.tag= Cyc_Stdio_String_pa; _temp536.f1=( struct _tagged_arr) v;{ void*
_temp534=( void*)& _temp536; void* _temp531[ 2u]={ _temp533, _temp534}; struct
_tagged_arr _temp532={( void*) _temp531,( void*) _temp531,( void*)( _temp531 + 2u)};
_temp530( _temp535, _temp532);}}})); _temp511= 0;} if( ! Cyc_Tcutil_same_atts(
atts0, atts1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp538)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp543= _tag_arr("%s %s has different attributes", sizeof(
unsigned char), 31u); struct Cyc_Stdio_String_pa_struct _temp545; _temp545.tag=
Cyc_Stdio_String_pa; _temp545.f1=( struct _tagged_arr) t;{ void* _temp541=( void*)&
_temp545; struct Cyc_Stdio_String_pa_struct _temp544; _temp544.tag= Cyc_Stdio_String_pa;
_temp544.f1=( struct _tagged_arr) v;{ void* _temp542=( void*)& _temp544; void*
_temp539[ 2u]={ _temp541, _temp542}; struct _tagged_arr _temp540={( void*)
_temp539,( void*) _temp539,( void*)( _temp539 + 2u)}; _temp538( _temp543,
_temp540);}}})); _temp511= 0;} return({ struct _tuple3 _temp546; _temp546.f1=
_temp516; _temp546.f2= _temp511; _temp546;});} struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp547= Cyc_Absynpp_qvar2string(
d0->name); int _temp550; void* _temp552; struct _tuple3 _temp548= Cyc_Tcdecl_check_var_or_fn_decl((
void*) d0->sc,( void*) d0->type, d0->tq, d0->attributes,( void*) d1->sc,( void*)
d1->type, d1->tq, d1->attributes, _tag_arr("variable", sizeof( unsigned char), 9u),
_temp547, loc, msg); _LL553: _temp552= _temp548.f1; goto _LL551; _LL551:
_temp550= _temp548.f2; goto _LL549; _LL549: if( ! _temp550){ return 0;} if((
void*) d0->sc == _temp552){ return( struct Cyc_Absyn_Vardecl*) d0;} else{ struct
Cyc_Absyn_Vardecl* _temp554=({ struct Cyc_Absyn_Vardecl* _temp555=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp555[ 0]=* d0; _temp555;});(
void*)((( struct Cyc_Absyn_Vardecl*) _check_null( _temp554))->sc=( void*)
_temp552); return _temp554;}} struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp556= Cyc_Absynpp_qvar2string(
d0->name); if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs, _tag_arr("typedef",
sizeof( unsigned char), 8u), _temp556, loc, msg)){ return 0;}{ struct Cyc_List_List*
_temp557= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); void* subst_defn1= Cyc_Tcutil_substitute(
_temp557,( void*) d1->defn); if( ! Cyc_Tcdecl_check_type(( void*) d0->defn,
subst_defn1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp558)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp564= _tag_arr("typedef %s does not refer to the same type: %s != %s",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp567;
_temp567.tag= Cyc_Stdio_String_pa; _temp567.f1=( struct _tagged_arr) _temp556;{
void* _temp561=( void*)& _temp567; struct Cyc_Stdio_String_pa_struct _temp566;
_temp566.tag= Cyc_Stdio_String_pa; _temp566.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
subst_defn1);{ void* _temp562=( void*)& _temp566; struct Cyc_Stdio_String_pa_struct
_temp565; _temp565.tag= Cyc_Stdio_String_pa; _temp565.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*) d0->defn);{ void* _temp563=( void*)& _temp565;
void* _temp559[ 3u]={ _temp561, _temp562, _temp563}; struct _tagged_arr _temp560={(
void*) _temp559,( void*) _temp559,( void*)( _temp559 + 3u)}; _temp558( _temp564,
_temp560);}}}})); return 0;} return( struct Cyc_Absyn_Typedefdecl*) d0;}} void*
Cyc_Tcdecl_merge_binding( void* b0, void* b1, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct _tuple4 _temp569=({ struct _tuple4 _temp568;
_temp568.f1= b0; _temp568.f2= b1; _temp568;}); void* _temp583; void* _temp585;
void* _temp587; struct Cyc_Absyn_Vardecl* _temp589; void* _temp591; struct Cyc_Absyn_Vardecl*
_temp593; void* _temp595; struct Cyc_Absyn_Fndecl* _temp597; void* _temp599;
struct Cyc_Absyn_Vardecl* _temp601; void* _temp603; struct Cyc_Absyn_Fndecl*
_temp605; void* _temp607; void* _temp609; struct Cyc_Absyn_Vardecl* _temp611;
void* _temp613; struct Cyc_Absyn_Fndecl* _temp615; _LL571: _LL586: _temp585=
_temp569.f1; if( _temp585 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL584;}
else{ goto _LL573;} _LL584: _temp583= _temp569.f2; if( _temp583 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL572;} else{ goto _LL573;} _LL573: _LL592: _temp591= _temp569.f1; if((
unsigned int) _temp591 > 1u?*(( int*) _temp591) == Cyc_Absyn_Global_b: 0){
_LL594: _temp593=(( struct Cyc_Absyn_Global_b_struct*) _temp591)->f1; goto
_LL588;} else{ goto _LL575;} _LL588: _temp587= _temp569.f2; if(( unsigned int)
_temp587 > 1u?*(( int*) _temp587) == Cyc_Absyn_Global_b: 0){ _LL590: _temp589=((
struct Cyc_Absyn_Global_b_struct*) _temp587)->f1; goto _LL574;} else{ goto
_LL575;} _LL575: _LL600: _temp599= _temp569.f1; if(( unsigned int) _temp599 > 1u?*((
int*) _temp599) == Cyc_Absyn_Global_b: 0){ _LL602: _temp601=(( struct Cyc_Absyn_Global_b_struct*)
_temp599)->f1; goto _LL596;} else{ goto _LL577;} _LL596: _temp595= _temp569.f2;
if(( unsigned int) _temp595 > 1u?*(( int*) _temp595) == Cyc_Absyn_Funname_b: 0){
_LL598: _temp597=(( struct Cyc_Absyn_Funname_b_struct*) _temp595)->f1; goto
_LL576;} else{ goto _LL577;} _LL577: _LL608: _temp607= _temp569.f1; if((
unsigned int) _temp607 > 1u?*(( int*) _temp607) == Cyc_Absyn_Funname_b: 0){ goto
_LL604;} else{ goto _LL579;} _LL604: _temp603= _temp569.f2; if(( unsigned int)
_temp603 > 1u?*(( int*) _temp603) == Cyc_Absyn_Funname_b: 0){ _LL606: _temp605=((
struct Cyc_Absyn_Funname_b_struct*) _temp603)->f1; goto _LL578;} else{ goto
_LL579;} _LL579: _LL614: _temp613= _temp569.f1; if(( unsigned int) _temp613 > 1u?*((
int*) _temp613) == Cyc_Absyn_Funname_b: 0){ _LL616: _temp615=(( struct Cyc_Absyn_Funname_b_struct*)
_temp613)->f1; goto _LL610;} else{ goto _LL581;} _LL610: _temp609= _temp569.f2;
if(( unsigned int) _temp609 > 1u?*(( int*) _temp609) == Cyc_Absyn_Global_b: 0){
_LL612: _temp611=(( struct Cyc_Absyn_Global_b_struct*) _temp609)->f1; goto
_LL580;} else{ goto _LL581;} _LL581: goto _LL582; _LL572: return( void*) Cyc_Absyn_Unresolved_b;
_LL574: { struct Cyc_Absyn_Vardecl* _temp617= Cyc_Tcdecl_merge_vardecl( _temp593,
_temp589, loc, msg); if( _temp617 == 0){ return( void*) Cyc_Absyn_Unresolved_b;}
if(( struct Cyc_Absyn_Vardecl*) _temp593 == _temp617){ return b0;} else{ if((
struct Cyc_Absyn_Vardecl*) _temp589 == _temp617){ return b1;} else{ return( void*)({
struct Cyc_Absyn_Global_b_struct* _temp618=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp618[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp619; _temp619.tag= Cyc_Absyn_Global_b; _temp619.f1=(
struct Cyc_Absyn_Vardecl*) _check_null( _temp617); _temp619;}); _temp618;});}}}
_LL576: { int _temp622; struct _tuple3 _temp620= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp601->sc,( void*) _temp601->type, _temp601->tq, _temp601->attributes,(
void*) _temp597->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp597->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp597->attributes, _tag_arr("function", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp601->name), loc, msg); _LL623:
_temp622= _temp620.f2; goto _LL621; _LL621: if( ! _temp622){ return( void*) Cyc_Absyn_Unresolved_b;}
return b1;} _LL578: Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp624)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp628= _tag_arr("redefinition of function %s", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp629; _temp629.tag=
Cyc_Stdio_String_pa; _temp629.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp605->name);{ void* _temp627=( void*)& _temp629; void* _temp625[ 1u]={
_temp627}; struct _tagged_arr _temp626={( void*) _temp625,( void*) _temp625,(
void*)( _temp625 + 1u)}; _temp624( _temp628, _temp626);}})); return( void*) Cyc_Absyn_Unresolved_b;
_LL580: { int _temp632; struct _tuple3 _temp630= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp615->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp615->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp615->attributes,( void*) _temp611->sc,( void*)
_temp611->type, _temp611->tq, _temp611->attributes, _tag_arr("variable", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp615->name), loc, msg); _LL633:
_temp632= _temp630.f2; goto _LL631; _LL631: if( ! _temp632){ return( void*) Cyc_Absyn_Unresolved_b;}
return b0;} _LL582:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp634=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp634[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp635; _temp635.tag= Cyc_Core_InvalidArg;
_temp635.f1= _tag_arr("Tcdecl::merge_binding", sizeof( unsigned char), 22u);
_temp635;}); _temp634;})); _LL570:;} struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl
_temp638; struct Cyc_Absyn_Tunionfield* _temp639; struct Cyc_Absyn_Tuniondecl*
_temp641; struct Cyc_Tcdecl_Xtunionfielddecl* _temp636= d0; _temp638=* _temp636;
_LL642: _temp641= _temp638.base; goto _LL640; _LL640: _temp639= _temp638.field;
goto _LL637; _LL637: { struct Cyc_Tcdecl_Xtunionfielddecl _temp645; struct Cyc_Absyn_Tunionfield*
_temp646; struct Cyc_Absyn_Tuniondecl* _temp648; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp643= d1; _temp645=* _temp643; _LL649: _temp648= _temp645.base; goto _LL647;
_LL647: _temp646= _temp645.field; goto _LL644; _LL644: { struct _tagged_arr
_temp650= Cyc_Absynpp_qvar2string( _temp639->name); if( Cyc_Absyn_qvar_cmp(
_temp641->name, _temp648->name) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr(* _temp651)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp657= _tag_arr("xtunions %s and %s have both a field named %s",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp660;
_temp660.tag= Cyc_Stdio_String_pa; _temp660.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp641->name);{ void* _temp654=( void*)& _temp660; struct Cyc_Stdio_String_pa_struct
_temp659; _temp659.tag= Cyc_Stdio_String_pa; _temp659.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp648->name);{ void* _temp655=( void*)& _temp659;
struct Cyc_Stdio_String_pa_struct _temp658; _temp658.tag= Cyc_Stdio_String_pa;
_temp658.f1=( struct _tagged_arr) _temp650;{ void* _temp656=( void*)& _temp658;
void* _temp652[ 3u]={ _temp654, _temp655, _temp656}; struct _tagged_arr _temp653={(
void*) _temp652,( void*) _temp652,( void*)( _temp652 + 3u)}; _temp651( _temp657,
_temp653);}}}})); return 0;} if( ! Cyc_Tcdecl_check_tvs( _temp641->tvs, _temp648->tvs,
_tag_arr("xtunion", sizeof( unsigned char), 8u), Cyc_Absynpp_qvar2string(
_temp641->name), loc, msg)){ return 0;}{ struct Cyc_List_List* _temp661= Cyc_Tcdecl_build_tvs_map(
_temp641->tvs, _temp648->tvs); struct Cyc_Absyn_Tunionfield* _temp662= Cyc_Tcdecl_merge_tunionfield(
_temp639, _temp646, _temp661, _tag_arr("xtunionfield", sizeof( unsigned char),
13u), _temp650, msg); if( _temp662 == 0){ return 0;} if( _temp662 ==( struct Cyc_Absyn_Tunionfield*)
_temp639){ return( struct Cyc_Tcdecl_Xtunionfielddecl*) d0;} else{ return({
struct Cyc_Tcdecl_Xtunionfielddecl* _temp663=( struct Cyc_Tcdecl_Xtunionfielddecl*)
GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp663->base=
_temp641; _temp663->field=( struct Cyc_Absyn_Tunionfield*) _check_null( _temp662);
_temp663;});}}}}}