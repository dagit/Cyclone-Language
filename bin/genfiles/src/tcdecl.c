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
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_strptrcmp(
struct _tagged_arr* s1, struct _tagged_arr* s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat(
struct _tagged_arr, struct _tagged_arr); extern unsigned char Cyc_Tcdecl_Incompatible[
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
_tagged_arr)({ struct _tagged_arr _temp0=*(( struct _tagged_arr*) _check_null(
msg1)); struct _tagged_arr _temp1= msg2; xprintf("%.*s %.*s", _get_arr_size(
_temp0, 1u), _temp0.curr, _get_arr_size( _temp1, 1u), _temp1.curr);}));}} static
void Cyc_Tcdecl_merge_scope_err( void* s0, void* s1, struct _tagged_arr t,
struct _tagged_arr v, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp2= t;
struct _tagged_arr _temp3= v; struct _tagged_arr _temp4= Cyc_Absynpp_scope2string(
s1); struct _tagged_arr _temp5= Cyc_Absynpp_scope2string( s0); xprintf("%.*s %.*s is %.*s whereas expected scope is %.*s",
_get_arr_size( _temp2, 1u), _temp2.curr, _get_arr_size( _temp3, 1u), _temp3.curr,
_get_arr_size( _temp4, 1u), _temp4.curr, _get_arr_size( _temp5, 1u), _temp5.curr);}));}
struct _tuple4{ void* f1; void* f2; } ; struct _tuple3 Cyc_Tcdecl_merge_scope(
void* s0, void* s1, struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){{ struct _tuple4 _temp7=({ struct _tuple4 _temp6;
_temp6.f1= s0; _temp6.f2= s1; _temp6;}); void* _temp27; void* _temp29; void*
_temp31; void* _temp33; void* _temp35; void* _temp37; void* _temp39; void*
_temp41; void* _temp43; void* _temp45; void* _temp47; void* _temp49; void*
_temp51; void* _temp53; void* _temp55; void* _temp57; _LL9: _LL30: _temp29=
_temp7.f1; if( _temp29 ==( void*) Cyc_Absyn_ExternC){ goto _LL28;} else{ goto
_LL11;} _LL28: _temp27= _temp7.f2; if( _temp27 ==( void*) Cyc_Absyn_ExternC){
goto _LL10;} else{ goto _LL11;} _LL11: _LL34: _temp33= _temp7.f1; if( _temp33 ==(
void*) Cyc_Absyn_ExternC){ goto _LL32;} else{ goto _LL13;} _LL32: _temp31=
_temp7.f2; goto _LL12; _LL13: _LL38: _temp37= _temp7.f1; goto _LL36; _LL36:
_temp35= _temp7.f2; if( _temp35 ==( void*) Cyc_Absyn_ExternC){ goto _LL14;}
else{ goto _LL15;} _LL15: _LL42: _temp41= _temp7.f1; goto _LL40; _LL40: _temp39=
_temp7.f2; if( _temp39 ==( void*) Cyc_Absyn_Extern){ goto _LL16;} else{ goto
_LL17;} _LL17: _LL46: _temp45= _temp7.f1; if( _temp45 ==( void*) Cyc_Absyn_Extern){
goto _LL44;} else{ goto _LL19;} _LL44: _temp43= _temp7.f2; goto _LL18; _LL19:
_LL50: _temp49= _temp7.f1; if( _temp49 ==( void*) Cyc_Absyn_Static){ goto _LL48;}
else{ goto _LL21;} _LL48: _temp47= _temp7.f2; if( _temp47 ==( void*) Cyc_Absyn_Static){
goto _LL20;} else{ goto _LL21;} _LL21: _LL54: _temp53= _temp7.f1; if( _temp53 ==(
void*) Cyc_Absyn_Public){ goto _LL52;} else{ goto _LL23;} _LL52: _temp51= _temp7.f2;
if( _temp51 ==( void*) Cyc_Absyn_Public){ goto _LL22;} else{ goto _LL23;} _LL23:
_LL58: _temp57= _temp7.f1; if( _temp57 ==( void*) Cyc_Absyn_Abstract){ goto
_LL56;} else{ goto _LL25;} _LL56: _temp55= _temp7.f2; if( _temp55 ==( void*) Cyc_Absyn_Abstract){
goto _LL24;} else{ goto _LL25;} _LL25: goto _LL26; _LL10: goto _LL8; _LL12: goto
_LL14; _LL14: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp59; _temp59.f1= s1; _temp59.f2= 0; _temp59;}); _LL16: s1= s0;
goto _LL8; _LL18: goto _LL8; _LL20: goto _LL8; _LL22: goto _LL8; _LL24: goto
_LL8; _LL26: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp60; _temp60.f1= s1; _temp60.f2= 0; _temp60;}); _LL8:;}
return({ struct _tuple3 _temp61; _temp61.f1= s1; _temp61.f2= 1; _temp61;});}
static int Cyc_Tcdecl_check_type( void* t0, void* t1){ return Cyc_Tcutil_unify(
t0, t1);} static unsigned int Cyc_Tcdecl_get_uint_const_value( struct Cyc_Absyn_Exp*
e){ void* _temp62=( void*) e->r; void* _temp68; int _temp70; void* _temp72;
_LL64: if(*(( int*) _temp62) == Cyc_Absyn_Const_e){ _LL69: _temp68=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp62)->f1; if(( unsigned int) _temp68 > 1u?*((
int*) _temp68) == Cyc_Absyn_Int_c: 0){ _LL73: _temp72=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp68)->f1; goto _LL71; _LL71: _temp70=(( struct Cyc_Absyn_Int_c_struct*)
_temp68)->f2; goto _LL65;} else{ goto _LL66;}} else{ goto _LL66;} _LL66: goto
_LL67; _LL65: return( unsigned int) _temp70; _LL67:( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp74=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp74[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp75; _temp75.tag= Cyc_Core_InvalidArg; _temp75.f1=
_tag_arr("Tcdecl::get_uint_const_value", sizeof( unsigned char), 29u); _temp75;});
_temp74;})); _LL63:;} inline static int Cyc_Tcdecl_check_tvs( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1, struct _tagged_arr t, struct _tagged_arr v,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( tvs0) !=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr _temp76= t; struct _tagged_arr _temp77= v; xprintf("%.*s %.*s has a different number of type parameters",
_get_arr_size( _temp76, 1u), _temp76.curr, _get_arr_size( _temp77, 1u), _temp77.curr);}));
return 0;}{ struct Cyc_List_List* _temp78= tvs0; struct Cyc_List_List* _temp79=
tvs1; for( 0; _temp78 != 0;( _temp78=(( struct Cyc_List_List*) _check_null(
_temp78))->tl, _temp79=(( struct Cyc_List_List*) _check_null( _temp79))->tl)){
struct Cyc_Absyn_Conref* _temp80=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp78))->hd)->kind); struct Cyc_Absyn_Conref* _temp81=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp79))->hd)->kind);
if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp80) !=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp81)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp82= t; struct
_tagged_arr _temp83= v; struct _tagged_arr _temp84= Cyc_Absynpp_ckind2string(
_temp80); struct _tagged_arr _temp85=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp78))->hd)->name; struct _tagged_arr _temp86= Cyc_Absynpp_ckind2string(
_temp81); xprintf("%.*s %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_get_arr_size( _temp82, 1u), _temp82.curr, _get_arr_size( _temp83, 1u), _temp83.curr,
_get_arr_size( _temp84, 1u), _temp84.curr, _get_arr_size( _temp85, 1u), _temp85.curr,
_get_arr_size( _temp86, 1u), _temp86.curr);})); return 0;}} return 1;}} inline
static int Cyc_Tcdecl_check_atts( struct Cyc_List_List* atts0, struct Cyc_List_List*
atts1, struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ if( ! Cyc_Tcutil_same_atts( atts0, atts1)){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp87= t; struct
_tagged_arr _temp88= v; xprintf("%.*s %.*s has different attributes",
_get_arr_size( _temp87, 1u), _temp87.curr, _get_arr_size( _temp88, 1u), _temp88.curr);}));
return 0;} else{ return 1;}} struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2;
} ; inline static struct Cyc_List_List* Cyc_Tcdecl_build_tvs_map( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1){ struct Cyc_List_List* _temp89= 0; for( 0;
tvs0 != 0;( tvs0=(( struct Cyc_List_List*) _check_null( tvs0))->tl, tvs1=((
struct Cyc_List_List*) _check_null( tvs1))->tl)){ _temp89=({ struct Cyc_List_List*
_temp90=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp90->hd=( void*)({ struct _tuple5* _temp91=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp91->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs1))->hd; _temp91->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp92=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp92[ 0]=({ struct Cyc_Absyn_VarType_struct _temp93; _temp93.tag= Cyc_Absyn_VarType;
_temp93.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs0))->hd;
_temp93;}); _temp92;}); _temp91;}); _temp90->tl= _temp89; _temp90;});} return
_temp89;} struct _tuple6{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
static struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_struct_or_union_decl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct
_tagged_arr t, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
struct _tagged_arr _temp94= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( d0->name))->v); int _temp95= 1; if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, _temp94, loc, msg)){ return 0;}{ int _temp98; void*
_temp100; struct _tuple3 _temp96= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*)
d1->sc, t, _temp94, loc, msg); _LL101: _temp100= _temp96.f1; goto _LL99; _LL99:
_temp98= _temp96.f2; goto _LL97; _LL97: if( ! _temp98){ _temp95= 0;} if( ! Cyc_Tcdecl_check_atts(
d0->attributes, d1->attributes, t, _temp94, loc, msg)){ _temp95= 0;}{ struct Cyc_Absyn_Structdecl*
d2;{ struct _tuple6 _temp103=({ struct _tuple6 _temp102; _temp102.f1= d0->fields;
_temp102.f2= d1->fields; _temp102;}); struct Cyc_Core_Opt* _temp111; struct Cyc_Core_Opt*
_temp113; struct Cyc_Core_Opt* _temp115; struct Cyc_Core_Opt* _temp117; struct
Cyc_Core_Opt* _temp119; struct Cyc_Core_Opt _temp121; struct Cyc_List_List*
_temp122; struct Cyc_Core_Opt* _temp124; struct Cyc_Core_Opt _temp126; struct
Cyc_List_List* _temp127; _LL105: _LL114: _temp113= _temp103.f1; goto _LL112;
_LL112: _temp111= _temp103.f2; if( _temp111 == 0){ goto _LL106;} else{ goto
_LL107;} _LL107: _LL118: _temp117= _temp103.f1; if( _temp117 == 0){ goto _LL116;}
else{ goto _LL109;} _LL116: _temp115= _temp103.f2; goto _LL108; _LL109: _LL125:
_temp124= _temp103.f1; if( _temp124 == 0){ goto _LL104;} else{ _temp126=*
_temp124; _LL128: _temp127=( struct Cyc_List_List*) _temp126.v; goto _LL120;}
_LL120: _temp119= _temp103.f2; if( _temp119 == 0){ goto _LL104;} else{ _temp121=*
_temp119; _LL123: _temp122=( struct Cyc_List_List*) _temp121.v; goto _LL110;}
_LL106: d2= d0; goto _LL104; _LL108: d2= d1; goto _LL104; _LL110: { struct Cyc_List_List*
_temp129= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); for( 0; _temp127 != 0?
_temp122 != 0: 0;( _temp127=(( struct Cyc_List_List*) _check_null( _temp127))->tl,
_temp122=(( struct Cyc_List_List*) _check_null( _temp122))->tl)){ struct Cyc_Absyn_Structfield
_temp132; struct Cyc_List_List* _temp133; struct Cyc_Core_Opt* _temp135; void*
_temp137; struct Cyc_Absyn_Tqual _temp139; struct _tagged_arr* _temp141; struct
Cyc_Absyn_Structfield* _temp130=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp127))->hd; _temp132=* _temp130; _LL142: _temp141= _temp132.name;
goto _LL140; _LL140: _temp139= _temp132.tq; goto _LL138; _LL138: _temp137=( void*)
_temp132.type; goto _LL136; _LL136: _temp135= _temp132.width; goto _LL134;
_LL134: _temp133= _temp132.attributes; goto _LL131; _LL131: { struct Cyc_Absyn_Structfield
_temp145; struct Cyc_List_List* _temp146; struct Cyc_Core_Opt* _temp148; void*
_temp150; struct Cyc_Absyn_Tqual _temp152; struct _tagged_arr* _temp154; struct
Cyc_Absyn_Structfield* _temp143=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp122))->hd; _temp145=* _temp143; _LL155: _temp154= _temp145.name;
goto _LL153; _LL153: _temp152= _temp145.tq; goto _LL151; _LL151: _temp150=( void*)
_temp145.type; goto _LL149; _LL149: _temp148= _temp145.width; goto _LL147;
_LL147: _temp146= _temp145.attributes; goto _LL144; _LL144: if( Cyc_String_zstrptrcmp(
_temp141, _temp154) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr _temp156= t; struct _tagged_arr _temp157= _temp94; struct
_tagged_arr _temp158=* _temp141; struct _tagged_arr _temp159=* _temp154; xprintf("%.*s %.*s : field name mismatch %.*s != %.*s",
_get_arr_size( _temp156, 1u), _temp156.curr, _get_arr_size( _temp157, 1u),
_temp157.curr, _get_arr_size( _temp158, 1u), _temp158.curr, _get_arr_size(
_temp159, 1u), _temp159.curr);})); return 0;} if( ! Cyc_Tcutil_same_atts(
_temp133, _temp146)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr _temp160= t; struct _tagged_arr _temp161= _temp94; struct
_tagged_arr _temp162=* _temp141; xprintf("%.*s %.*s : attribute mismatch on field %.*s",
_get_arr_size( _temp160, 1u), _temp160.curr, _get_arr_size( _temp161, 1u),
_temp161.curr, _get_arr_size( _temp162, 1u), _temp162.curr);})); _temp95= 0;}
if( ! Cyc_Tcutil_equal_tqual( _temp139, _temp152)){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct _tagged_arr _temp163= t; struct _tagged_arr
_temp164= _temp94; struct _tagged_arr _temp165=* _temp141; xprintf("%.*s %.*s : qualifier mismatch on field %.*s",
_get_arr_size( _temp163, 1u), _temp163.curr, _get_arr_size( _temp164, 1u),
_temp164.curr, _get_arr_size( _temp165, 1u), _temp165.curr);})); _temp95= 0;}
if(((( _temp135 != 0? _temp148 != 0: 0)? Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp135))->v) !=
Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp148))->v): 0)? 1:( _temp135 == 0? _temp148 != 0: 0))? 1:(
_temp135 != 0? _temp148 == 0: 0)){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr _temp166= t; struct _tagged_arr _temp167=
_temp94; struct _tagged_arr _temp168=* _temp141; xprintf("%.*s %.*s : bitfield mismatch on field %.*s",
_get_arr_size( _temp166, 1u), _temp166.curr, _get_arr_size( _temp167, 1u),
_temp167.curr, _get_arr_size( _temp168, 1u), _temp168.curr);})); _temp95= 0;}{
void* subst_t1= Cyc_Tcutil_substitute( _temp129, _temp150); if( ! Cyc_Tcdecl_check_type(
_temp137, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr _temp169= t; struct _tagged_arr _temp170= _temp94; struct
_tagged_arr _temp171=* _temp141; struct _tagged_arr _temp172= Cyc_Absynpp_typ2string(
_temp137); struct _tagged_arr _temp173= Cyc_Absynpp_typ2string( subst_t1);
xprintf("%.*s %.*s : type mismatch on field %.*s: %.*s != %.*s", _get_arr_size(
_temp169, 1u), _temp169.curr, _get_arr_size( _temp170, 1u), _temp170.curr,
_get_arr_size( _temp171, 1u), _temp171.curr, _get_arr_size( _temp172, 1u),
_temp172.curr, _get_arr_size( _temp173, 1u), _temp173.curr);})); _temp95= 0;}}}}
if( _temp127 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr _temp174= t; struct _tagged_arr _temp175= _temp94; struct
_tagged_arr _temp176=*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp127))->hd)->name; xprintf("%.*s %.*s is missing field %.*s",
_get_arr_size( _temp174, 1u), _temp174.curr, _get_arr_size( _temp175, 1u),
_temp175.curr, _get_arr_size( _temp176, 1u), _temp176.curr);})); _temp95= 0;}
if( _temp122 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr _temp177= t; struct _tagged_arr _temp178= _temp94; struct
_tagged_arr _temp179=*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp122))->hd)->name; xprintf("%.*s %.*s has extra field %.*s",
_get_arr_size( _temp177, 1u), _temp177.curr, _get_arr_size( _temp178, 1u),
_temp178.curr, _get_arr_size( _temp179, 1u), _temp179.curr);})); _temp95= 0;} d2=
d0; goto _LL104;} _LL104:;} if( ! _temp95){ return 0;} if( _temp100 ==( void*)
d2->sc){ return( struct Cyc_Absyn_Structdecl*) d2;} else{ d2=({ struct Cyc_Absyn_Structdecl*
_temp180=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp180[ 0]=* d2; _temp180;});( void*)( d2->sc=( void*) _temp100); return(
struct Cyc_Absyn_Structdecl*) d2;}}}} struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ return Cyc_Tcdecl_merge_struct_or_union_decl( d0,
d1, _tag_arr("struct", sizeof( unsigned char), 7u), loc, msg);} struct Cyc_Absyn_Uniondecl*
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
_tagged_arr _temp181= t; struct _tagged_arr _temp182= v; struct _tagged_arr
_temp183=*(* f1->name).f2; struct _tagged_arr _temp184=*(* f0->name).f2; xprintf("%.*s %.*s: field name mismatch %.*s != %.*s",
_get_arr_size( _temp181, 1u), _temp181.curr, _get_arr_size( _temp182, 1u),
_temp182.curr, _get_arr_size( _temp183, 1u), _temp183.curr, _get_arr_size(
_temp184, 1u), _temp184.curr);})); return 0;}{ struct _tagged_arr _temp185=*(*
f0->name).f2; int _temp190; void* _temp192; struct _tuple3 _temp188= Cyc_Tcdecl_merge_scope((
void*) f0->sc,( void*) f1->sc,( struct _tagged_arr)({ struct _tagged_arr
_temp186= t; struct _tagged_arr _temp187= v; xprintf("in %.*s %.*s, field",
_get_arr_size( _temp186, 1u), _temp186.curr, _get_arr_size( _temp187, 1u),
_temp187.curr);}), _temp185, loc, msg); _LL193: _temp192= _temp188.f1; goto
_LL191; _LL191: _temp190= _temp188.f2; goto _LL189; _LL189: { struct Cyc_List_List*
_temp194= f0->tvs; struct Cyc_List_List* _temp195= f1->tvs; if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp194) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp195)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr _temp196= t; struct _tagged_arr _temp197= v; struct
_tagged_arr _temp198= _temp185; xprintf("%.*s %.*s, field %.*s: type parameter number mismatch",
_get_arr_size( _temp196, 1u), _temp196.curr, _get_arr_size( _temp197, 1u),
_temp197.curr, _get_arr_size( _temp198, 1u), _temp198.curr);})); return 0;}{
struct Cyc_List_List* _temp199= Cyc_Tcdecl_build_tvs_map( _temp194, _temp195);
struct Cyc_List_List* _temp200= f0->typs; struct Cyc_List_List* _temp201= f1->typs;
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp200) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp201)){ Cyc_Tcdecl_merr( loc,
msg,( struct _tagged_arr)({ struct _tagged_arr _temp202= t; struct _tagged_arr
_temp203= v; struct _tagged_arr _temp204= _temp185; xprintf("%.*s %.*s, field %.*s: parameter number mismatch",
_get_arr_size( _temp202, 1u), _temp202.curr, _get_arr_size( _temp203, 1u),
_temp203.curr, _get_arr_size( _temp204, 1u), _temp204.curr);})); _temp190= 0;}
for( 0; _temp200 != 0;( _temp200=(( struct Cyc_List_List*) _check_null( _temp200))->tl,
_temp201=(( struct Cyc_List_List*) _check_null( _temp201))->tl)){ if( ! Cyc_Tcutil_equal_tqual((*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp200))->hd)).f1,(*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp201))->hd)).f1)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp205= t;
struct _tagged_arr _temp206= v; struct _tagged_arr _temp207= _temp185; xprintf("%.*s %.*s, field %.*s: parameter qualifier",
_get_arr_size( _temp205, 1u), _temp205.curr, _get_arr_size( _temp206, 1u),
_temp206.curr, _get_arr_size( _temp207, 1u), _temp207.curr);})); _temp190= 0;}{
void* subst_t1= Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp199, inst),(*(( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( _temp201))->hd)).f2); if( ! Cyc_Tcdecl_check_type((*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp200))->hd)).f2,
subst_t1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr
_temp208= t; struct _tagged_arr _temp209= v; struct _tagged_arr _temp210=
_temp185; struct _tagged_arr _temp211= Cyc_Absynpp_typ2string((*(( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( _temp200))->hd)).f2); struct
_tagged_arr _temp212= Cyc_Absynpp_typ2string( subst_t1); xprintf("%.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_get_arr_size( _temp208, 1u), _temp208.curr, _get_arr_size( _temp209, 1u),
_temp209.curr, _get_arr_size( _temp210, 1u), _temp210.curr, _get_arr_size(
_temp211, 1u), _temp211.curr, _get_arr_size( _temp212, 1u), _temp212.curr);}));
_temp190= 0;}}} if( ! _temp190){ return 0;} if(( void*) f0->sc != _temp192){
struct Cyc_Absyn_Tunionfield* _temp213=({ struct Cyc_Absyn_Tunionfield* _temp214=(
struct Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp214[ 0]=* f0; _temp214;});( void*)((( struct Cyc_Absyn_Tunionfield*)
_check_null( _temp213))->sc=( void*) _temp192); return _temp213;} else{ return(
struct Cyc_Absyn_Tunionfield*) f0;}}}}} static int Cyc_Tcdecl_substitute_tunionfield_f1(
struct _tagged_arr* name, struct Cyc_Absyn_Tvar* x){ return Cyc_String_strptrcmp(
x->name, name) == 0;} static struct _tuple7* Cyc_Tcdecl_substitute_tunionfield_f2(
struct Cyc_List_List* inst, struct _tuple7* x){ struct _tuple7 _temp217; void*
_temp218; struct Cyc_Absyn_Tqual _temp220; struct _tuple7* _temp215= x; _temp217=*
_temp215; _LL221: _temp220= _temp217.f1; goto _LL219; _LL219: _temp218= _temp217.f2;
goto _LL216; _LL216: return({ struct _tuple7* _temp222=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7)); _temp222->f1= _temp220; _temp222->f2= Cyc_Tcutil_substitute(
inst, _temp218); _temp222;});} static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List* d0tvs, struct Cyc_List_List* d1tvs, struct Cyc_List_List*
inst1, struct Cyc_Absyn_Tunionfield* f1){ struct Cyc_Absyn_Tunionfield* _temp223=({
struct Cyc_Absyn_Tunionfield* _temp231=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp231[ 0]=* f1; _temp231;});
struct Cyc_List_List* _temp224= 0;{ struct Cyc_List_List* _temp225= f1->tvs;
for( 0; _temp225 != 0; _temp225=(( struct Cyc_List_List*) _check_null( _temp225))->tl){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp225))->hd; if((( int(*)( int(* pred)( struct _tagged_arr*,
struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcdecl_substitute_tunionfield_f1, tv->name, d0tvs)? 1:(( int(*)( int(* pred)(
struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1, tv->name,
d1tvs)){ int i= 0; struct _tagged_arr base=* tv->name; struct _tagged_arr*
new_name; do { new_name=({ struct _tagged_arr* _temp226=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp226[ 0]=( struct _tagged_arr) Cyc_String_strconcat(
base,( struct _tagged_arr) Cyc_Core_string_of_int( i)); _temp226;}); i ++;} while (((
int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*), struct
_tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
new_name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_arr*, struct Cyc_Absyn_Tvar*),
struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)); tv=({ struct Cyc_Absyn_Tvar* _temp227=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp227[ 0]=* tv; _temp227;}); tv->name=
new_name;} _temp224=({ struct Cyc_List_List* _temp228=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp228->hd=( void*) tv; _temp228->tl=
_temp224; _temp228;});}} _temp224=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp224); _temp223->tvs= _temp224;{ struct Cyc_List_List*
_temp229= Cyc_Tcdecl_build_tvs_map( _temp224, f1->tvs); struct Cyc_List_List*
_temp230=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp229, inst1); _temp223->typs=(( struct Cyc_List_List*(*)(
struct _tuple7*(* f)( struct Cyc_List_List*, struct _tuple7*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcdecl_substitute_tunionfield_f2,
_temp230, f1->typs); return _temp223;}} static struct Cyc_List_List* Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1, int* res, int* incl,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp240=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp240[ 0]= 0; _temp240;}); struct Cyc_List_List** _temp232= f2sp; int cmp= -
1; for( 0; f0s != 0? f1s != 0: 0; f1s=(( struct Cyc_List_List*) _check_null( f1s))->tl){
while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)->name,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd)->name)) < 0: 0) { struct Cyc_List_List*
_temp233=({ struct Cyc_List_List* _temp234=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp234->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd); _temp234->tl= 0; _temp234;});*
_temp232= _temp233; _temp232=&(( struct Cyc_List_List*) _check_null( _temp233))->tl;
f0s=(( struct Cyc_List_List*) _check_null( f0s))->tl;} if( f0s == 0? 1: cmp > 0){*
incl= 0;{ struct Cyc_List_List* _temp235=({ struct Cyc_List_List* _temp236=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp236->hd=(
void*) Cyc_Tcdecl_substitute_tunionfield( tvs0, tvs1, inst,( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd); _temp236->tl= 0; _temp236;});*
_temp232= _temp235; _temp232=&(( struct Cyc_List_List*) _check_null( _temp235))->tl;}}
else{ struct Cyc_Absyn_Tunionfield* _temp237= Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f0s))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f1s))->hd,
inst, t, v, msg); if( _temp237 != 0){ if( _temp237 !=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f0s))->hd)){*
incl= 0;}{ struct Cyc_List_List* _temp238=({ struct Cyc_List_List* _temp239=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp239->hd=(
void*)(( struct Cyc_Absyn_Tunionfield*) _check_null( _temp237)); _temp239->tl= 0;
_temp239;});* _temp232= _temp238; _temp232=&(( struct Cyc_List_List*)
_check_null( _temp238))->tl;}} else{* res= 0;} f0s=(( struct Cyc_List_List*)
_check_null( f0s))->tl;}} if( f1s != 0){* incl= 0;* _temp232= f1s;} else{*
_temp232= f0s;} return* f2sp;} struct _tuple8{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; } ; static struct _tuple8 Cyc_Tcdecl_split( struct Cyc_List_List*
f){ if( f == 0){ return({ struct _tuple8 _temp241; _temp241.f1= 0; _temp241.f2=
0; _temp241;});} if((( struct Cyc_List_List*) _check_null( f))->tl == 0){ return({
struct _tuple8 _temp242; _temp242.f1= f; _temp242.f2= 0; _temp242;});}{ struct
Cyc_List_List* _temp245; struct Cyc_List_List* _temp247; struct _tuple8 _temp243=((
struct _tuple8(*)( struct Cyc_List_List* f)) Cyc_Tcdecl_split)((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( f))->tl))->tl); _LL248:
_temp247= _temp243.f1; goto _LL246; _LL246: _temp245= _temp243.f2; goto _LL244;
_LL244: return({ struct _tuple8 _temp249; _temp249.f1=({ struct Cyc_List_List*
_temp251=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp251->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( f))->hd);
_temp251->tl= _temp247; _temp251;}); _temp249.f2=({ struct Cyc_List_List*
_temp250=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp250->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( f))->tl))->hd); _temp250->tl= _temp245; _temp250;});
_temp249;});}} struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List*
f, int* res, struct _tagged_arr* v, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg){ struct Cyc_List_List* _temp254; struct Cyc_List_List*
_temp256; struct _tuple8 _temp252=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL257: _temp256= _temp252.f1; goto _LL255; _LL255:
_temp254= _temp252.f2; goto _LL253; _LL253: if( _temp256 != 0?(( struct Cyc_List_List*)
_check_null( _temp256))->tl != 0: 0){ _temp256= Cyc_Tcdecl_sort_xtunion_fields(
_temp256, res, v, loc, msg);} if( _temp254 != 0?(( struct Cyc_List_List*)
_check_null( _temp254))->tl != 0: 0){ _temp254= Cyc_Tcdecl_sort_xtunion_fields(
_temp254, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp256,
_temp254, 0, 0, 0, res,({ int* _temp258=( int*) GC_malloc_atomic( sizeof( int));
_temp258[ 0]= 1; _temp258;}), _tag_arr("xtunion", sizeof( unsigned char), 8u),*
v, loc, msg);} struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl( struct
Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp259= Cyc_Absynpp_qvar2string(
d0->name); struct _tagged_arr t= _tag_arr("[x]tunion", sizeof( unsigned char),
10u); int _temp260= 1; if( d0->is_xtunion != d1->is_xtunion){ Cyc_Tcdecl_merr(
loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp261= _temp259; struct
_tagged_arr _temp262= Cyc_Tcdecl_is_x2string( d0->is_xtunion); struct
_tagged_arr _temp263= Cyc_Tcdecl_is_x2string( d1->is_xtunion); xprintf("expected %.*s to be a %.*s instead of a %.*s",
_get_arr_size( _temp261, 1u), _temp261.curr, _get_arr_size( _temp262, 1u),
_temp262.curr, _get_arr_size( _temp263, 1u), _temp263.curr);})); _temp260= 0;}
else{ t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, _temp259, loc, msg)){ return 0;}{ int _temp266; void*
_temp268; struct _tuple3 _temp264= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*)
d1->sc, t, _temp259, loc, msg); _LL269: _temp268= _temp264.f1; goto _LL267;
_LL267: _temp266= _temp264.f2; goto _LL265; _LL265: if( ! _temp266){ _temp260= 0;}{
struct Cyc_Absyn_Tuniondecl* d2;{ struct _tuple6 _temp271=({ struct _tuple6
_temp270; _temp270.f1= d0->fields; _temp270.f2= d1->fields; _temp270;}); struct
Cyc_Core_Opt* _temp279; struct Cyc_Core_Opt* _temp281; struct Cyc_Core_Opt*
_temp283; struct Cyc_Core_Opt* _temp285; struct Cyc_Core_Opt* _temp287; struct
Cyc_Core_Opt _temp289; struct Cyc_List_List* _temp290; struct Cyc_Core_Opt*
_temp292; struct Cyc_Core_Opt _temp294; struct Cyc_List_List* _temp295; _LL273:
_LL282: _temp281= _temp271.f1; goto _LL280; _LL280: _temp279= _temp271.f2; if(
_temp279 == 0){ goto _LL274;} else{ goto _LL275;} _LL275: _LL286: _temp285=
_temp271.f1; if( _temp285 == 0){ goto _LL284;} else{ goto _LL277;} _LL284:
_temp283= _temp271.f2; goto _LL276; _LL277: _LL293: _temp292= _temp271.f1; if(
_temp292 == 0){ goto _LL272;} else{ _temp294=* _temp292; _LL296: _temp295=(
struct Cyc_List_List*) _temp294.v; goto _LL288;} _LL288: _temp287= _temp271.f2;
if( _temp287 == 0){ goto _LL272;} else{ _temp289=* _temp287; _LL291: _temp290=(
struct Cyc_List_List*) _temp289.v; goto _LL278;} _LL274: d2= d0; goto _LL272;
_LL276: d2= d1; goto _LL272; _LL278: { struct Cyc_List_List* _temp297= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); if( d0->is_xtunion){ int _temp298= 1; struct Cyc_List_List*
_temp299= Cyc_Tcdecl_merge_xtunion_fields( _temp295, _temp290, _temp297, d0->tvs,
d1->tvs,& _temp260,& _temp298, t, _temp259, loc, msg); if( _temp298){ d2= d0;}
else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp300=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp300[ 0]=* d0; _temp300;});(
void*)( d2->sc=( void*) _temp268); d2->fields=({ struct Cyc_Core_Opt* _temp301=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp301->v=(
void*) _temp299; _temp301;});}} else{ for( 0; _temp295 != 0? _temp290 != 0: 0;(
_temp295=(( struct Cyc_List_List*) _check_null( _temp295))->tl, _temp290=((
struct Cyc_List_List*) _check_null( _temp290))->tl)){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp295))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp290))->hd,
_temp297, t, _temp259, msg);} if( _temp295 != 0){ Cyc_Tcdecl_merr( loc, msg,(
struct _tagged_arr)({ struct _tagged_arr _temp302= t; struct _tagged_arr
_temp303= _temp259; struct _tagged_arr _temp304=*(*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp290))->hd)->name).f2; xprintf("%.*s %.*s has extra field %.*s",
_get_arr_size( _temp302, 1u), _temp302.curr, _get_arr_size( _temp303, 1u),
_temp303.curr, _get_arr_size( _temp304, 1u), _temp304.curr);})); _temp260= 0;}
if( _temp290 != 0){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr _temp305= t; struct _tagged_arr _temp306= _temp259; struct
_tagged_arr _temp307=*(*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp290))->hd)->name).f2; xprintf("%.*s %.*s is missing field %.*s",
_get_arr_size( _temp305, 1u), _temp305.curr, _get_arr_size( _temp306, 1u),
_temp306.curr, _get_arr_size( _temp307, 1u), _temp307.curr);})); _temp260= 0;}
d2= d0;} goto _LL272;} _LL272:;} if( ! _temp260){ return 0;} if( _temp268 ==(
void*) d2->sc){ return( struct Cyc_Absyn_Tuniondecl*) d2;} else{ d2=({ struct
Cyc_Absyn_Tuniondecl* _temp308=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp308[ 0]=* d2; _temp308;});( void*)(
d2->sc=( void*) _temp268); return( struct Cyc_Absyn_Tuniondecl*) d2;}}}} struct
Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl( struct Cyc_Absyn_Enumdecl* d0,
struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg){ struct _tagged_arr _temp309= Cyc_Absynpp_qvar2string( d0->name);
int _temp310= 1; int _temp313; void* _temp315; struct _tuple3 _temp311= Cyc_Tcdecl_merge_scope((
void*) d0->sc,( void*) d1->sc, _tag_arr("enum", sizeof( unsigned char), 5u),
_temp309, loc, msg); _LL316: _temp315= _temp311.f1; goto _LL314; _LL314:
_temp313= _temp311.f2; goto _LL312; _LL312: if( ! _temp313){ _temp310= 0;}{
struct Cyc_Absyn_Enumdecl* d2;{ struct _tuple6 _temp318=({ struct _tuple6
_temp317; _temp317.f1= d0->fields; _temp317.f2= d1->fields; _temp317;}); struct
Cyc_Core_Opt* _temp326; struct Cyc_Core_Opt* _temp328; struct Cyc_Core_Opt*
_temp330; struct Cyc_Core_Opt* _temp332; struct Cyc_Core_Opt* _temp334; struct
Cyc_Core_Opt _temp336; struct Cyc_List_List* _temp337; struct Cyc_Core_Opt*
_temp339; struct Cyc_Core_Opt _temp341; struct Cyc_List_List* _temp342; _LL320:
_LL329: _temp328= _temp318.f1; goto _LL327; _LL327: _temp326= _temp318.f2; if(
_temp326 == 0){ goto _LL321;} else{ goto _LL322;} _LL322: _LL333: _temp332=
_temp318.f1; if( _temp332 == 0){ goto _LL331;} else{ goto _LL324;} _LL331:
_temp330= _temp318.f2; goto _LL323; _LL324: _LL340: _temp339= _temp318.f1; if(
_temp339 == 0){ goto _LL319;} else{ _temp341=* _temp339; _LL343: _temp342=(
struct Cyc_List_List*) _temp341.v; goto _LL335;} _LL335: _temp334= _temp318.f2;
if( _temp334 == 0){ goto _LL319;} else{ _temp336=* _temp334; _LL338: _temp337=(
struct Cyc_List_List*) _temp336.v; goto _LL325;} _LL321: d2= d0; goto _LL319;
_LL323: d2= d1; goto _LL319; _LL325: for( 0; _temp342 != 0? _temp337 != 0: 0;(
_temp342=(( struct Cyc_List_List*) _check_null( _temp342))->tl, _temp337=((
struct Cyc_List_List*) _check_null( _temp337))->tl)){ struct Cyc_Absyn_Enumfield
_temp346; struct Cyc_Position_Segment* _temp347; struct Cyc_Absyn_Exp* _temp349;
struct _tuple0* _temp351; struct Cyc_Absyn_Enumfield* _temp344=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp342))->hd; _temp346=* _temp344; _LL352:
_temp351= _temp346.name; goto _LL350; _LL350: _temp349= _temp346.tag; goto
_LL348; _LL348: _temp347= _temp346.loc; goto _LL345; _LL345: { struct Cyc_Absyn_Enumfield
_temp355; struct Cyc_Position_Segment* _temp356; struct Cyc_Absyn_Exp* _temp358;
struct _tuple0* _temp360; struct Cyc_Absyn_Enumfield* _temp353=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( _temp337))->hd; _temp355=* _temp353; _LL361:
_temp360= _temp355.name; goto _LL359; _LL359: _temp358= _temp355.tag; goto
_LL357; _LL357: _temp356= _temp355.loc; goto _LL354; _LL354: if( Cyc_String_zstrptrcmp((*
_temp360).f2,(* _temp351).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr _temp362= _temp309; struct _tagged_arr
_temp363=*(* _temp360).f2; struct _tagged_arr _temp364=*(* _temp351).f2; xprintf("enum %.*s: field name mismatch %.*s != %.*s",
_get_arr_size( _temp362, 1u), _temp362.curr, _get_arr_size( _temp363, 1u),
_temp363.curr, _get_arr_size( _temp364, 1u), _temp364.curr);})); _temp310= 0;}
if( Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*) _check_null(
_temp349)) != Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)
_check_null( _temp358))){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({
struct _tagged_arr _temp365= _temp309; struct _tagged_arr _temp366=*(* _temp360).f2;
xprintf("enum %.*s, field %.*s, value mismatch", _get_arr_size( _temp365, 1u),
_temp365.curr, _get_arr_size( _temp366, 1u), _temp366.curr);})); _temp310= 0;}}}
d2= d0; goto _LL319; _LL319:;} if( ! _temp310){ return 0;} if(( void*) d2->sc ==
_temp315){ return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({ struct Cyc_Absyn_Enumdecl*
_temp367=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp367[ 0]=* d2; _temp367;});( void*)( d2->sc=( void*) _temp315); return(
struct Cyc_Absyn_Enumdecl*) d2;}}} inline static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void* sc0, void* t0, struct Cyc_Absyn_Tqual tq0, struct Cyc_List_List* atts0,
void* sc1, void* t1, struct Cyc_Absyn_Tqual tq1, struct Cyc_List_List* atts1,
struct _tagged_arr t, struct _tagged_arr v, struct Cyc_Position_Segment* loc,
struct _tagged_arr* msg){ int _temp368= 1; int _temp371; void* _temp373; struct
_tuple3 _temp369= Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg); _LL374:
_temp373= _temp369.f1; goto _LL372; _LL372: _temp371= _temp369.f2; goto _LL370;
_LL370: if( ! _temp371){ _temp368= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp375= t;
struct _tagged_arr _temp376= v; struct _tagged_arr _temp377= Cyc_Absynpp_typ2string(
t1); struct _tagged_arr _temp378= Cyc_Absynpp_typ2string( t0); xprintf("%.*s %.*s has type %.*s instead of %.*s",
_get_arr_size( _temp375, 1u), _temp375.curr, _get_arr_size( _temp376, 1u),
_temp376.curr, _get_arr_size( _temp377, 1u), _temp377.curr, _get_arr_size(
_temp378, 1u), _temp378.curr);})); _temp368= 0;} if( ! Cyc_Tcutil_equal_tqual(
tq0, tq1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr
_temp379= t; struct _tagged_arr _temp380= v; xprintf("%.*s %.*s has different type qualifiers",
_get_arr_size( _temp379, 1u), _temp379.curr, _get_arr_size( _temp380, 1u),
_temp380.curr);})); _temp368= 0;} if( ! Cyc_Tcutil_same_atts( atts0, atts1)){
Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr _temp381= t;
struct _tagged_arr _temp382= v; xprintf("%.*s %.*s has different attributes",
_get_arr_size( _temp381, 1u), _temp381.curr, _get_arr_size( _temp382, 1u),
_temp382.curr);})); _temp368= 0;} return({ struct _tuple3 _temp383; _temp383.f1=
_temp373; _temp383.f2= _temp368; _temp383;});} struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp384= Cyc_Absynpp_qvar2string(
d0->name); int _temp387; void* _temp389; struct _tuple3 _temp385= Cyc_Tcdecl_check_var_or_fn_decl((
void*) d0->sc,( void*) d0->type, d0->tq, d0->attributes,( void*) d1->sc,( void*)
d1->type, d1->tq, d1->attributes, _tag_arr("variable", sizeof( unsigned char), 9u),
_temp384, loc, msg); _LL390: _temp389= _temp385.f1; goto _LL388; _LL388:
_temp387= _temp385.f2; goto _LL386; _LL386: if( ! _temp387){ return 0;} if((
void*) d0->sc == _temp389){ return( struct Cyc_Absyn_Vardecl*) d0;} else{ struct
Cyc_Absyn_Vardecl* _temp391=({ struct Cyc_Absyn_Vardecl* _temp392=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp392[ 0]=* d0; _temp392;});(
void*)((( struct Cyc_Absyn_Vardecl*) _check_null( _temp391))->sc=( void*)
_temp389); return _temp391;}} struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg){ struct _tagged_arr _temp393= Cyc_Absynpp_qvar2string(
d0->name); if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs, _tag_arr("typedef",
sizeof( unsigned char), 8u), _temp393, loc, msg)){ return 0;}{ struct Cyc_List_List*
_temp394= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); void* subst_defn1= Cyc_Tcutil_substitute(
_temp394,( void*) d1->defn); if( ! Cyc_Tcdecl_check_type(( void*) d0->defn,
subst_defn1)){ Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct
_tagged_arr _temp395= _temp393; struct _tagged_arr _temp396= Cyc_Absynpp_typ2string(
subst_defn1); struct _tagged_arr _temp397= Cyc_Absynpp_typ2string(( void*) d0->defn);
xprintf("typedef %.*s does not refer to the same type: %.*s != %.*s",
_get_arr_size( _temp395, 1u), _temp395.curr, _get_arr_size( _temp396, 1u),
_temp396.curr, _get_arr_size( _temp397, 1u), _temp397.curr);})); return 0;}
return( struct Cyc_Absyn_Typedefdecl*) d0;}} void* Cyc_Tcdecl_merge_binding(
void* b0, void* b1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){
struct _tuple4 _temp399=({ struct _tuple4 _temp398; _temp398.f1= b0; _temp398.f2=
b1; _temp398;}); void* _temp413; void* _temp415; void* _temp417; struct Cyc_Absyn_Vardecl*
_temp419; void* _temp421; struct Cyc_Absyn_Vardecl* _temp423; void* _temp425;
struct Cyc_Absyn_Fndecl* _temp427; void* _temp429; struct Cyc_Absyn_Vardecl*
_temp431; void* _temp433; struct Cyc_Absyn_Fndecl* _temp435; void* _temp437;
struct Cyc_Absyn_Fndecl* _temp439; void* _temp441; struct Cyc_Absyn_Vardecl*
_temp443; void* _temp445; struct Cyc_Absyn_Fndecl* _temp447; _LL401: _LL416:
_temp415= _temp399.f1; if( _temp415 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL414;} else{ goto _LL403;} _LL414: _temp413= _temp399.f2; if( _temp413 ==(
void*) Cyc_Absyn_Unresolved_b){ goto _LL402;} else{ goto _LL403;} _LL403: _LL422:
_temp421= _temp399.f1; if(( unsigned int) _temp421 > 1u?*(( int*) _temp421) ==
Cyc_Absyn_Global_b: 0){ _LL424: _temp423=(( struct Cyc_Absyn_Global_b_struct*)
_temp421)->f1; goto _LL418;} else{ goto _LL405;} _LL418: _temp417= _temp399.f2;
if(( unsigned int) _temp417 > 1u?*(( int*) _temp417) == Cyc_Absyn_Global_b: 0){
_LL420: _temp419=(( struct Cyc_Absyn_Global_b_struct*) _temp417)->f1; goto
_LL404;} else{ goto _LL405;} _LL405: _LL430: _temp429= _temp399.f1; if((
unsigned int) _temp429 > 1u?*(( int*) _temp429) == Cyc_Absyn_Global_b: 0){
_LL432: _temp431=(( struct Cyc_Absyn_Global_b_struct*) _temp429)->f1; goto
_LL426;} else{ goto _LL407;} _LL426: _temp425= _temp399.f2; if(( unsigned int)
_temp425 > 1u?*(( int*) _temp425) == Cyc_Absyn_Funname_b: 0){ _LL428: _temp427=((
struct Cyc_Absyn_Funname_b_struct*) _temp425)->f1; goto _LL406;} else{ goto
_LL407;} _LL407: _LL438: _temp437= _temp399.f1; if(( unsigned int) _temp437 > 1u?*((
int*) _temp437) == Cyc_Absyn_Funname_b: 0){ _LL440: _temp439=(( struct Cyc_Absyn_Funname_b_struct*)
_temp437)->f1; goto _LL434;} else{ goto _LL409;} _LL434: _temp433= _temp399.f2;
if(( unsigned int) _temp433 > 1u?*(( int*) _temp433) == Cyc_Absyn_Funname_b: 0){
_LL436: _temp435=(( struct Cyc_Absyn_Funname_b_struct*) _temp433)->f1; goto
_LL408;} else{ goto _LL409;} _LL409: _LL446: _temp445= _temp399.f1; if((
unsigned int) _temp445 > 1u?*(( int*) _temp445) == Cyc_Absyn_Funname_b: 0){
_LL448: _temp447=(( struct Cyc_Absyn_Funname_b_struct*) _temp445)->f1; goto
_LL442;} else{ goto _LL411;} _LL442: _temp441= _temp399.f2; if(( unsigned int)
_temp441 > 1u?*(( int*) _temp441) == Cyc_Absyn_Global_b: 0){ _LL444: _temp443=((
struct Cyc_Absyn_Global_b_struct*) _temp441)->f1; goto _LL410;} else{ goto
_LL411;} _LL411: goto _LL412; _LL402: return( void*) Cyc_Absyn_Unresolved_b;
_LL404: { struct Cyc_Absyn_Vardecl* _temp449= Cyc_Tcdecl_merge_vardecl( _temp423,
_temp419, loc, msg); if( _temp449 == 0){ return( void*) Cyc_Absyn_Unresolved_b;}
if(( struct Cyc_Absyn_Vardecl*) _temp423 == _temp449){ return b0;} else{ if((
struct Cyc_Absyn_Vardecl*) _temp419 == _temp449){ return b1;} else{ return( void*)({
struct Cyc_Absyn_Global_b_struct* _temp450=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp450[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp451; _temp451.tag= Cyc_Absyn_Global_b; _temp451.f1=(
struct Cyc_Absyn_Vardecl*) _check_null( _temp449); _temp451;}); _temp450;});}}}
_LL406: { int _temp454; void* _temp456; struct _tuple3 _temp452= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp431->sc,( void*) _temp431->type, _temp431->tq, _temp431->attributes,(
void*) _temp427->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp427->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp427->attributes, _tag_arr("function", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp431->name), loc, msg); _LL457:
_temp456= _temp452.f1; goto _LL455; _LL455: _temp454= _temp452.f2; goto _LL453;
_LL453: if( ! _temp454){ return( void*) Cyc_Absyn_Unresolved_b;} return b1;}
_LL408: Cyc_Tcdecl_merr( loc, msg,( struct _tagged_arr)({ struct _tagged_arr
_temp458= Cyc_Absynpp_qvar2string( _temp435->name); xprintf("redefinition of function %.*s",
_get_arr_size( _temp458, 1u), _temp458.curr);})); return( void*) Cyc_Absyn_Unresolved_b;
_LL410: { int _temp461; void* _temp463; struct _tuple3 _temp459= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp447->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp447->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp447->attributes,( void*) _temp443->sc,( void*)
_temp443->type, _temp443->tq, _temp443->attributes, _tag_arr("variable", sizeof(
unsigned char), 9u), Cyc_Absynpp_qvar2string( _temp447->name), loc, msg); _LL464:
_temp463= _temp459.f1; goto _LL462; _LL462: _temp461= _temp459.f2; goto _LL460;
_LL460: if( ! _temp461){ return( void*) Cyc_Absyn_Unresolved_b;} return b0;}
_LL412:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp465=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp465[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp466; _temp466.tag= Cyc_Core_InvalidArg;
_temp466.f1= _tag_arr("Tcdecl::merge_binding", sizeof( unsigned char), 22u);
_temp466;}); _temp465;})); _LL400:;} struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl
_temp469; struct Cyc_Absyn_Tunionfield* _temp470; struct Cyc_Absyn_Tuniondecl*
_temp472; struct Cyc_Tcdecl_Xtunionfielddecl* _temp467= d0; _temp469=* _temp467;
_LL473: _temp472= _temp469.base; goto _LL471; _LL471: _temp470= _temp469.field;
goto _LL468; _LL468: { struct Cyc_Tcdecl_Xtunionfielddecl _temp476; struct Cyc_Absyn_Tunionfield*
_temp477; struct Cyc_Absyn_Tuniondecl* _temp479; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp474= d1; _temp476=* _temp474; _LL480: _temp479= _temp476.base; goto _LL478;
_LL478: _temp477= _temp476.field; goto _LL475; _LL475: { struct _tagged_arr
_temp481= Cyc_Absynpp_qvar2string( _temp470->name); if( Cyc_Absyn_qvar_cmp(
_temp472->name, _temp479->name) != 0){ Cyc_Tcdecl_merr( loc, msg,( struct
_tagged_arr)({ struct _tagged_arr _temp482= Cyc_Absynpp_qvar2string( _temp472->name);
struct _tagged_arr _temp483= Cyc_Absynpp_qvar2string( _temp479->name); struct
_tagged_arr _temp484= _temp481; xprintf("xtunions %.*s and %.*s have both a field named %.*s",
_get_arr_size( _temp482, 1u), _temp482.curr, _get_arr_size( _temp483, 1u),
_temp483.curr, _get_arr_size( _temp484, 1u), _temp484.curr);})); return 0;} if(
! Cyc_Tcdecl_check_tvs( _temp472->tvs, _temp479->tvs, _tag_arr("xtunion",
sizeof( unsigned char), 8u), Cyc_Absynpp_qvar2string( _temp472->name), loc, msg)){
return 0;}{ struct Cyc_List_List* _temp485= Cyc_Tcdecl_build_tvs_map( _temp472->tvs,
_temp479->tvs); struct Cyc_Absyn_Tunionfield* _temp486= Cyc_Tcdecl_merge_tunionfield(
_temp470, _temp477, _temp485, _tag_arr("xtunionfield", sizeof( unsigned char),
13u), _temp481, msg); if( _temp486 == 0){ return 0;} if( _temp486 ==( struct Cyc_Absyn_Tunionfield*)
_temp470){ return( struct Cyc_Tcdecl_Xtunionfielddecl*) d0;} else{ return({
struct Cyc_Tcdecl_Xtunionfielddecl* _temp487=( struct Cyc_Tcdecl_Xtunionfielddecl*)
GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp487->base=
_temp472; _temp487->field=( struct Cyc_Absyn_Tunionfield*) _check_null( _temp486);
_temp487;});}}}}}