 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
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
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_rappend( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern unsigned char Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth( struct
Cyc_List_List* x, int i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const
int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern void*
Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ( void* rgn); extern void*
Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct
_tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_zstrcmp(
struct _tagged_arr, struct _tagged_arr); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff); extern
void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* po); extern unsigned char Cyc_Tcutil_TypeErr[ 12u];
extern void* Cyc_Tcutil_impos( struct _tagged_arr); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable( void*); extern
int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral( struct
Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); struct
_tuple4{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ; struct _tuple5{
struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple4*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, void*);
extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*, struct
Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); struct _tuple6{ int f1; void* f2; } ; extern struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern int Cyc_Evexp_okay_szofarg( void* t); extern
void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int
new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*); extern
struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_arr, struct Cyc_Position_Segment*); extern void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part);
struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0; struct
Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF= 0;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
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
int tag; struct Cyc_Dict_Dict* f1; } ; extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; static void* Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr
msg){ Cyc_Tcutil_terr( loc, msg); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt*
_temp0=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp0->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp0;}));} static struct Cyc_Core_Opt
Cyc_Tcexp_rk={( void*)(( void*) 3u)}; static struct Cyc_Core_Opt Cyc_Tcexp_ak={(
void*)(( void*) 0u)}; static struct Cyc_Core_Opt Cyc_Tcexp_bk={( void*)(( void*)
2u)}; static struct Cyc_Core_Opt Cyc_Tcexp_mk={( void*)(( void*) 1u)}; static
void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tuple1* q){ struct _handler_cons _temp1; _push_handler(& _temp1);{
int _temp3= 0; if( setjmp( _temp1.handler)){ _temp3= 1;} if( ! _temp3){{ void*
_temp4= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void* _temp14; struct Cyc_Absyn_Enumfield*
_temp16; struct Cyc_Absyn_Enumdecl* _temp18; struct Cyc_Absyn_Tunionfield*
_temp20; struct Cyc_Absyn_Tuniondecl* _temp22; struct Cyc_Absyn_Structdecl*
_temp24; _LL6: if(*(( int*) _temp4) == Cyc_Tcenv_VarRes){ _LL15: _temp14=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp4)->f1; goto _LL7;} else{ goto _LL8;} _LL8:
if(*(( int*) _temp4) == Cyc_Tcenv_EnumRes){ _LL19: _temp18=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp4)->f1; goto _LL17; _LL17: _temp16=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp4)->f2; goto _LL9;} else{ goto _LL10;} _LL10: if(*(( int*) _temp4) == Cyc_Tcenv_TunionRes){
_LL23: _temp22=(( struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f1; goto _LL21;
_LL21: _temp20=(( struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f2; goto _LL11;}
else{ goto _LL12;} _LL12: if(*(( int*) _temp4) == Cyc_Tcenv_StructRes){ _LL25:
_temp24=(( struct Cyc_Tcenv_StructRes_struct*) _temp4)->f1; goto _LL13;} else{
goto _LL5;} _LL7:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp26=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp26[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp27; _temp27.tag= Cyc_Absyn_Var_e;
_temp27.f1= q; _temp27.f2=( void*) _temp14; _temp27;}); _temp26;}))); goto _LL5;
_LL9:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp28=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp28[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp29; _temp29.tag= Cyc_Absyn_Enum_e;
_temp29.f1= q; _temp29.f2=( struct Cyc_Absyn_Enumdecl*) _temp18; _temp29.f3=(
struct Cyc_Absyn_Enumfield*) _temp16; _temp29;}); _temp28;}))); goto _LL5; _LL11:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp30=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp30[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp31; _temp31.tag= Cyc_Absyn_Tunion_e;
_temp31.f1= 0; _temp31.f2= 0; _temp31.f3= 0; _temp31.f4= _temp22; _temp31.f5=
_temp20; _temp31;}); _temp30;}))); goto _LL5; _LL13: Cyc_Tcutil_terr( e->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp32= Cyc_Absynpp_qvar2string( q);
xprintf("bad occurrence of struct name %.*s", _get_arr_size( _temp32, 1u),
_temp32.curr);}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp33=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp33[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp34; _temp34.tag= Cyc_Absyn_Var_e;
_temp34.f1= q; _temp34.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp34;});
_temp33;}))); goto _LL5; _LL5:;}; _pop_handler();} else{ void* _temp2=( void*)
_exn_thrown; void* _temp36= _temp2; _LL38: if( _temp36 == Cyc_Dict_Absent){ goto
_LL39;} else{ goto _LL40;} _LL40: goto _LL41; _LL39:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp42=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp42[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp43; _temp43.tag= Cyc_Absyn_Var_e; _temp43.f1= q; _temp43.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp43;}); _temp42;}))); goto _LL37; _LL41:( void)
_throw( _temp36); _LL37:;}}} struct _tuple7{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static struct _tuple7* Cyc_Tcexp_make_struct_arg( struct
Cyc_Absyn_Exp* e){ return({ struct _tuple7* _temp44=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp44->f1= 0; _temp44->f2= e; _temp44;});} static
void Cyc_Tcexp_resolve_unknown_fn( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* es){ void* _temp45=( void*)
e1->r; struct _tuple1* _temp51; _LL47: if(*(( int*) _temp45) == Cyc_Absyn_UnknownId_e){
_LL52: _temp51=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp45)->f1; goto _LL48;}
else{ goto _LL49;} _LL49: goto _LL50; _LL48: { struct _handler_cons _temp53;
_push_handler(& _temp53);{ int _temp55= 0; if( setjmp( _temp53.handler)){
_temp55= 1;} if( ! _temp55){{ void* _temp56= Cyc_Tcenv_lookup_ordinary( te, e1->loc,
_temp51); void* _temp66; struct Cyc_Absyn_Tunionfield* _temp68; struct Cyc_Absyn_Tuniondecl*
_temp70; struct Cyc_Absyn_Structdecl* _temp72; struct Cyc_Absyn_Enumfield*
_temp74; struct Cyc_Absyn_Enumdecl* _temp76; _LL58: if(*(( int*) _temp56) == Cyc_Tcenv_VarRes){
_LL67: _temp66=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp56)->f1; goto
_LL59;} else{ goto _LL60;} _LL60: if(*(( int*) _temp56) == Cyc_Tcenv_TunionRes){
_LL71: _temp70=(( struct Cyc_Tcenv_TunionRes_struct*) _temp56)->f1; goto _LL69;
_LL69: _temp68=(( struct Cyc_Tcenv_TunionRes_struct*) _temp56)->f2; goto _LL61;}
else{ goto _LL62;} _LL62: if(*(( int*) _temp56) == Cyc_Tcenv_StructRes){ _LL73:
_temp72=(( struct Cyc_Tcenv_StructRes_struct*) _temp56)->f1; goto _LL63;} else{
goto _LL64;} _LL64: if(*(( int*) _temp56) == Cyc_Tcenv_EnumRes){ _LL77: _temp76=((
struct Cyc_Tcenv_EnumRes_struct*) _temp56)->f1; goto _LL75; _LL75: _temp74=((
struct Cyc_Tcenv_EnumRes_struct*) _temp56)->f2; goto _LL65;} else{ goto _LL57;}
_LL59:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp78=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp78[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp79; _temp79.tag= Cyc_Absyn_FnCall_e;
_temp79.f1= e1; _temp79.f2= es; _temp79.f3= 0; _temp79;}); _temp78;})));
_npop_handler( 0u); return; _LL61: if( _temp68->typs == 0){ Cyc_Tcutil_terr( e->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp80= Cyc_Absynpp_qvar2string(
_temp68->name); xprintf("%.*s is a constant, not a function", _get_arr_size(
_temp80, 1u), _temp80.curr);}));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct*
_temp81=( struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp81[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp82; _temp82.tag= Cyc_Absyn_Tunion_e;
_temp82.f1= 0; _temp82.f2= 0; _temp82.f3= es; _temp82.f4= _temp70; _temp82.f5=
_temp68; _temp82;}); _temp81;}))); _npop_handler( 0u); return; _npop_handler( 0u);
return; _LL63: { struct Cyc_List_List* _temp83=(( struct Cyc_List_List*(*)(
struct _tuple7*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcexp_make_struct_arg, es); if( _temp72->name != 0){( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp84=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp84[ 0]=({ struct Cyc_Absyn_Struct_e_struct
_temp85; _temp85.tag= Cyc_Absyn_Struct_e; _temp85.f1=( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( _temp72->name))->v; _temp85.f2= 0; _temp85.f3=
_temp83; _temp85.f4=( struct Cyc_Absyn_Structdecl*) _temp72; _temp85;}); _temp84;})));}
else{ Cyc_Tcutil_terr( e->loc, _tag_arr("missing struct name", sizeof(
unsigned char), 20u));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp86=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp86[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp87; _temp87.tag= Cyc_Absyn_Struct_e;
_temp87.f1= _temp51; _temp87.f2= 0; _temp87.f3= _temp83; _temp87.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp87;}); _temp86;})));} _npop_handler( 0u); return;} _LL65: Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp88= Cyc_Absynpp_qvar2string(
_temp51); xprintf("%.*s is an enum constructor, not a function", _get_arr_size(
_temp88, 1u), _temp88.curr);})); _npop_handler( 0u); return; _LL57:;};
_pop_handler();} else{ void* _temp54=( void*) _exn_thrown; void* _temp90=
_temp54; _LL92: if( _temp90 == Cyc_Dict_Absent){ goto _LL93;} else{ goto _LL94;}
_LL94: goto _LL95; _LL93: Cyc_Tcutil_terr( e1->loc,( struct _tagged_arr)({
struct _tagged_arr _temp96= Cyc_Absynpp_qvar2string( _temp51); xprintf("unbound identifier %.*s",
_get_arr_size( _temp96, 1u), _temp96.curr);}));( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp97=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp97[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp98; _temp98.tag= Cyc_Absyn_FnCall_e; _temp98.f1= e1; _temp98.f2= es;
_temp98.f3= 0; _temp98;}); _temp97;}))); return; _LL95:( void) _throw( _temp90);
_LL91:;}}} _LL50:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp99=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp99[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp100; _temp100.tag= Cyc_Absyn_FnCall_e;
_temp100.f1= e1; _temp100.f2= es; _temp100.f3= 0; _temp100;}); _temp99;})));
return; _LL46:;} static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp101=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp101[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp102; _temp102.tag= Cyc_Absyn_Array_e; _temp102.f1= des; _temp102;});
_temp101;}))); return;}{ void* t=*(( void**) _check_null( topt)); void* _temp103=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp113; struct Cyc_Absyn_Tqual
_temp115; void* _temp117; struct Cyc_Absyn_Structdecl** _temp119; struct Cyc_List_List*
_temp121; struct _tuple1* _temp123; struct Cyc_List_List* _temp125; _LL105: if((
unsigned int) _temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_ArrayType: 0){
_LL118: _temp117=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f1;
goto _LL116; _LL116: _temp115=(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f2;
goto _LL114; _LL114: _temp113=(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f3;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp103 > 4u?*((
int*) _temp103) == Cyc_Absyn_StructType: 0){ _LL124: _temp123=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f1; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f2; goto _LL120; _LL120: _temp119=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f3; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_AnonStructType: 0){ _LL126:
_temp125=(( struct Cyc_Absyn_AnonStructType_struct*) _temp103)->f1; goto _LL110;}
else{ goto _LL111;} _LL111: goto _LL112; _LL106:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp127=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp127[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp128; _temp128.tag= Cyc_Absyn_Array_e; _temp128.f1= des; _temp128;});
_temp127;}))); goto _LL104; _LL108: if( _temp119 == 0? 1: _temp123 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("Compiler bug: struct type not properly set", sizeof(
unsigned char), 43u));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp129=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp129[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp130; _temp130.tag= Cyc_Absyn_Struct_e;
_temp130.f1=( struct _tuple1*) _check_null( _temp123); _temp130.f2= 0; _temp130.f3=
des; _temp130.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp119)); _temp130;}); _temp129;}))); goto _LL104; _LL110:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp131=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp131[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp132; _temp132.tag= Cyc_Absyn_AnonStruct_e;
_temp132.f1=( void*) t; _temp132.f2= des; _temp132;}); _temp131;}))); goto
_LL104; _LL112:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp133=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp133[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp134; _temp134.tag= Cyc_Absyn_Array_e;
_temp134.f1= des; _temp134;}); _temp133;}))); goto _LL104; _LL104:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp135= msg_part; struct _tagged_arr _temp136= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_get_arr_size( _temp135, 1u), _temp135.curr, _get_arr_size( _temp136, 1u),
_temp136.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){
void* t;{ void* _temp137= c; unsigned char _temp153; void* _temp155; short
_temp157; void* _temp159; long long _temp161; void* _temp163; struct _tagged_arr
_temp165; int _temp167; void* _temp169; struct _tagged_arr _temp171; _LL139: if((
unsigned int) _temp137 > 1u?*(( int*) _temp137) == Cyc_Absyn_Char_c: 0){ _LL156:
_temp155=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp137)->f1; goto _LL154;
_LL154: _temp153=(( struct Cyc_Absyn_Char_c_struct*) _temp137)->f2; goto _LL140;}
else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 1u?*(( int*) _temp137)
== Cyc_Absyn_Short_c: 0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp137)->f1; goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_Short_c_struct*)
_temp137)->f2; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp137 > 1u?*(( int*) _temp137) == Cyc_Absyn_LongLong_c: 0){ _LL164: _temp163=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp137)->f1; goto _LL162; _LL162:
_temp161=(( struct Cyc_Absyn_LongLong_c_struct*) _temp137)->f2; goto _LL144;}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp137 > 1u?*(( int*) _temp137)
== Cyc_Absyn_Float_c: 0){ _LL166: _temp165=(( struct Cyc_Absyn_Float_c_struct*)
_temp137)->f1; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp137 > 1u?*(( int*) _temp137) == Cyc_Absyn_Int_c: 0){ _LL170: _temp169=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp137)->f1; goto _LL168; _LL168:
_temp167=(( struct Cyc_Absyn_Int_c_struct*) _temp137)->f2; goto _LL148;} else{
goto _LL149;} _LL149: if(( unsigned int) _temp137 > 1u?*(( int*) _temp137) ==
Cyc_Absyn_String_c: 0){ _LL172: _temp171=(( struct Cyc_Absyn_String_c_struct*)
_temp137)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if( _temp137 ==( void*)
Cyc_Absyn_Null_c){ goto _LL152;} else{ goto _LL138;} _LL140: t= _temp155 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL138;
_LL142: t= _temp159 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL138; _LL144: t= _temp163 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t:
Cyc_Absyn_slonglong_t; goto _LL138; _LL146: t= Cyc_Absyn_float_t; goto _LL138;
_LL148: if( topt == 0){ t= _temp169 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp173= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); void* _temp183; void* _temp185; void* _temp187; void*
_temp189; void* _temp191; void* _temp193; _LL175: if(( unsigned int) _temp173 >
4u?*(( int*) _temp173) == Cyc_Absyn_IntType: 0){ _LL186: _temp185=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp173)->f1; goto _LL184; _LL184: _temp183=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if( _temp183 ==( void*)
Cyc_Absyn_B1){ goto _LL176;} else{ goto _LL177;}} else{ goto _LL177;} _LL177:
if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType: 0){
_LL190: _temp189=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
goto _LL188; _LL188: _temp187=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp173)->f2; if( _temp187 ==( void*) Cyc_Absyn_B2){ goto _LL178;} else{ goto
_LL179;}} else{ goto _LL179;} _LL179: if(( unsigned int) _temp173 > 4u?*(( int*)
_temp173) == Cyc_Absyn_IntType: 0){ _LL194: _temp193=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp173)->f1; goto _LL192; _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp173)->f2; goto _LL180;} else{ goto _LL181;} _LL181: goto _LL182; _LL176: t=
_temp185 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp195=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp195[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp196; _temp196.tag= Cyc_Absyn_Const_e;
_temp196.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp197=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp197[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp198; _temp198.tag= Cyc_Absyn_Char_c;
_temp198.f1=( void*) _temp185; _temp198.f2=( unsigned char) _temp167; _temp198;});
_temp197;})); _temp196;}); _temp195;}))); goto _LL174; _LL178: t= _temp189 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp199=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp199[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp200; _temp200.tag= Cyc_Absyn_Const_e; _temp200.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp201=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp201[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp202; _temp202.tag= Cyc_Absyn_Short_c; _temp202.f1=( void*) _temp189;
_temp202.f2=( short) _temp167; _temp202;}); _temp201;})); _temp200;}); _temp199;})));
goto _LL174; _LL180: t= _temp193 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL174; _LL182: t= _temp169 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL174; _LL174:;} goto _LL138; _LL150: {
int len=( int) _get_arr_size( _temp171, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp203=({ struct Cyc_Absyn_Const_e_struct* _temp240=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp240[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp241; _temp241.tag= Cyc_Absyn_Const_e; _temp241.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp242=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp242[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp243; _temp243.tag= Cyc_Absyn_Int_c; _temp243.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp243.f2= len; _temp243;}); _temp242;})); _temp241;}); _temp240;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp203, loc); elen->topt=({
struct Cyc_Core_Opt* _temp204=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp204->v=( void*) Cyc_Absyn_uint_t; _temp204;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp205=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp205[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp206; _temp206.tag= Cyc_Absyn_Upper_b; _temp206.f1= elen; _temp206;});
_temp205;})); if( topt != 0){ void* _temp207= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp215; struct Cyc_Absyn_Tqual
_temp217; void* _temp219; struct Cyc_Absyn_PtrInfo _temp221; struct Cyc_Absyn_Conref*
_temp223; struct Cyc_Absyn_Tqual _temp225; struct Cyc_Absyn_Conref* _temp227;
void* _temp229; void* _temp231; _LL209: if(( unsigned int) _temp207 > 4u?*(( int*)
_temp207) == Cyc_Absyn_ArrayType: 0){ _LL220: _temp219=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp207)->f1; goto _LL218; _LL218: _temp217=(( struct Cyc_Absyn_ArrayType_struct*)
_temp207)->f2; goto _LL216; _LL216: _temp215=(( struct Cyc_Absyn_ArrayType_struct*)
_temp207)->f3; goto _LL210;} else{ goto _LL211;} _LL211: if(( unsigned int)
_temp207 > 4u?*(( int*) _temp207) == Cyc_Absyn_PointerType: 0){ _LL222: _temp221=((
struct Cyc_Absyn_PointerType_struct*) _temp207)->f1; _LL232: _temp231=( void*)
_temp221.elt_typ; goto _LL230; _LL230: _temp229=( void*) _temp221.rgn_typ; goto
_LL228; _LL228: _temp227= _temp221.nullable; goto _LL226; _LL226: _temp225=
_temp221.tq; goto _LL224; _LL224: _temp223= _temp221.bounds; goto _LL212;} else{
goto _LL213;} _LL213: goto _LL214; _LL210: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp233=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp233[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp234; _temp234.tag= Cyc_Absyn_ArrayType;
_temp234.f1=( void*) Cyc_Absyn_uchar_t; _temp234.f2= _temp217; _temp234.f3=(
struct Cyc_Absyn_Exp*) elen; _temp234;}); _temp233;}); _LL212: if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp235=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp235->v=( void*) t; _temp235;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp236=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp236->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp236;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp237=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp237[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp238; _temp238.tag= Cyc_Absyn_Upper_b; _temp238.f1= elen; _temp238;});
_temp237;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp239=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp239->v=( void*) t; _temp239;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL208; _LL214:
goto _LL208; _LL208:;} return t;} _LL152: { struct Cyc_List_List* _temp244= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp245=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp245[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp246; _temp246.tag= Cyc_Absyn_PointerType;
_temp246.f1=({ struct Cyc_Absyn_PtrInfo _temp247; _temp247.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp249=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp249->v=( void*)
_temp244; _temp249;})); _temp247.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp248=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp248->v=( void*) _temp244;
_temp248;})); _temp247.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp247.tq= Cyc_Absyn_empty_tqual(); _temp247.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp247;}); _temp246;}); _temp245;}); goto _LL138;}
_LL138:;} return t;} static void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tuple1* q, void* b){ void* _temp250= b;
struct Cyc_Absyn_Vardecl* _temp264; struct Cyc_Absyn_Fndecl* _temp266; struct
Cyc_Absyn_Vardecl* _temp268; struct Cyc_Absyn_Vardecl* _temp270; struct Cyc_Absyn_Vardecl*
_temp272; _LL252: if( _temp250 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL253;}
else{ goto _LL254;} _LL254: if(( unsigned int) _temp250 > 1u?*(( int*) _temp250)
== Cyc_Absyn_Global_b: 0){ _LL265: _temp264=(( struct Cyc_Absyn_Global_b_struct*)
_temp250)->f1; goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int)
_temp250 > 1u?*(( int*) _temp250) == Cyc_Absyn_Funname_b: 0){ _LL267: _temp266=((
struct Cyc_Absyn_Funname_b_struct*) _temp250)->f1; goto _LL257;} else{ goto
_LL258;} _LL258: if(( unsigned int) _temp250 > 1u?*(( int*) _temp250) == Cyc_Absyn_Pat_b:
0){ _LL269: _temp268=(( struct Cyc_Absyn_Pat_b_struct*) _temp250)->f1; goto
_LL259;} else{ goto _LL260;} _LL260: if(( unsigned int) _temp250 > 1u?*(( int*)
_temp250) == Cyc_Absyn_Local_b: 0){ _LL271: _temp270=(( struct Cyc_Absyn_Local_b_struct*)
_temp250)->f1; goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int)
_temp250 > 1u?*(( int*) _temp250) == Cyc_Absyn_Param_b: 0){ _LL273: _temp272=((
struct Cyc_Absyn_Param_b_struct*) _temp250)->f1; goto _LL263;} else{ goto _LL251;}
_LL253: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct
_tagged_arr _temp274= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_get_arr_size( _temp274, 1u), _temp274.curr);})); _LL255: q[
_check_known_subscript_notnull( 1u, 0)]=( _temp264->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp264->type; _LL257:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp266->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp266);
_LL259: _temp270= _temp268; goto _LL261; _LL261: _temp272= _temp270; goto _LL263;
_LL263:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp272->type; _LL251:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp275=( void*) fmt->r; void* _temp281; struct _tagged_arr
_temp283; _LL277: if(*(( int*) _temp275) == Cyc_Absyn_Const_e){ _LL282: _temp281=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp275)->f1; if(( unsigned int)
_temp281 > 1u?*(( int*) _temp281) == Cyc_Absyn_String_c: 0){ _LL284: _temp283=((
struct Cyc_Absyn_String_c_struct*) _temp281)->f1; goto _LL278;} else{ goto
_LL279;}} else{ goto _LL279;} _LL279: goto _LL280; _LL278: desc_types=
type_getter( te, _temp283, fmt->loc); goto _LL276; _LL280: Cyc_Tcutil_terr( fmt->loc,
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));
return; _LL276:;} for( 0; desc_types != 0? args != 0: 0;( desc_types=(( struct
Cyc_List_List*) _check_null( desc_types))->tl, args=(( struct Cyc_List_List*)
_check_null( args))->tl)){ void* t=( void*)(( struct Cyc_List_List*) _check_null(
desc_types))->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp285= Cyc_Absynpp_typ2string(
t); struct _tagged_arr _temp286= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("descriptor has type %.*s but argument has type %.*s",
_get_arr_size( _temp285, 1u), _temp285.curr, _get_arr_size( _temp286, 1u),
_temp286.curr);}));}} if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc, _tag_arr("too few arguments",
sizeof( unsigned char), 18u));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd)->loc, _tag_arr("too many arguments",
sizeof( unsigned char), 19u));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp287= p; _LL289: if( _temp287 ==( void*) Cyc_Absyn_Plus){ goto _LL290;}
else{ goto _LL291;} _LL291: if( _temp287 ==( void*) Cyc_Absyn_Minus){ goto
_LL292;} else{ goto _LL293;} _LL293: if( _temp287 ==( void*) Cyc_Absyn_Not){
goto _LL294;} else{ goto _LL295;} _LL295: if( _temp287 ==( void*) Cyc_Absyn_Bitnot){
goto _LL296;} else{ goto _LL297;} _LL297: if( _temp287 ==( void*) Cyc_Absyn_Size){
goto _LL298;} else{ goto _LL299;} _LL299: goto _LL300; _LL290: goto _LL292;
_LL292: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp301= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_get_arr_size( _temp301, 1u), _temp301.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL294: Cyc_Tcutil_check_contains_assign( e); if( !
Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp302= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_get_arr_size( _temp302, 1u), _temp302.curr);}));} return Cyc_Absyn_sint_t;
_LL296: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp303= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_get_arr_size( _temp303, 1u), _temp303.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL298:{ void* _temp304= t; struct Cyc_Absyn_Exp*
_temp312; struct Cyc_Absyn_Tqual _temp314; void* _temp316; struct Cyc_Absyn_PtrInfo
_temp318; struct Cyc_Absyn_Conref* _temp320; struct Cyc_Absyn_Tqual _temp322;
struct Cyc_Absyn_Conref* _temp324; void* _temp326; void* _temp328; _LL306: if((
unsigned int) _temp304 > 4u?*(( int*) _temp304) == Cyc_Absyn_ArrayType: 0){
_LL317: _temp316=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp304)->f1;
goto _LL315; _LL315: _temp314=(( struct Cyc_Absyn_ArrayType_struct*) _temp304)->f2;
goto _LL313; _LL313: _temp312=(( struct Cyc_Absyn_ArrayType_struct*) _temp304)->f3;
goto _LL307;} else{ goto _LL308;} _LL308: if(( unsigned int) _temp304 > 4u?*((
int*) _temp304) == Cyc_Absyn_PointerType: 0){ _LL319: _temp318=(( struct Cyc_Absyn_PointerType_struct*)
_temp304)->f1; _LL329: _temp328=( void*) _temp318.elt_typ; goto _LL327; _LL327:
_temp326=( void*) _temp318.rgn_typ; goto _LL325; _LL325: _temp324= _temp318.nullable;
goto _LL323; _LL323: _temp322= _temp318.tq; goto _LL321; _LL321: _temp320=
_temp318.bounds; goto _LL309;} else{ goto _LL310;} _LL310: goto _LL311; _LL307:
goto _LL305; _LL309: goto _LL305; _LL311: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp330= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_get_arr_size( _temp330, 1u), _temp330.curr);})); goto _LL305; _LL305:;} return
Cyc_Absyn_uint_t; _LL300: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("Non-unary primop", sizeof( unsigned char), 17u)); _LL288:;} static
void* Cyc_Tcexp_tcArithBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, int(* checker)( struct Cyc_Absyn_Exp*)){ if( !
checker( e1)){ Cyc_Tcutil_terr( e1->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp331= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); xprintf("type %.*s cannot be used here",
_get_arr_size( _temp331, 1u), _temp331.curr);})); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp332=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp332->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp332;}));}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp333= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); xprintf("type %.*s cannot be used here",
_get_arr_size( _temp333, 1u), _temp333.curr);})); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp334=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp334->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp334;}));}{
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type( t1, t2);}} static void* Cyc_Tcexp_tcPlus(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
void* _temp335= t1; struct Cyc_Absyn_PtrInfo _temp341; struct Cyc_Absyn_Conref*
_temp343; struct Cyc_Absyn_Tqual _temp345; struct Cyc_Absyn_Conref* _temp347;
void* _temp349; void* _temp351; _LL337: if(( unsigned int) _temp335 > 4u?*(( int*)
_temp335) == Cyc_Absyn_PointerType: 0){ _LL342: _temp341=(( struct Cyc_Absyn_PointerType_struct*)
_temp335)->f1; _LL352: _temp351=( void*) _temp341.elt_typ; goto _LL350; _LL350:
_temp349=( void*) _temp341.rgn_typ; goto _LL348; _LL348: _temp347= _temp341.nullable;
goto _LL346; _LL346: _temp345= _temp341.tq; goto _LL344; _LL344: _temp343=
_temp341.bounds; goto _LL338;} else{ goto _LL339;} _LL339: goto _LL340; _LL338:
if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp353= Cyc_Absynpp_typ2string( t2); xprintf("expecting int but found %.*s",
_get_arr_size( _temp353, 1u), _temp353.curr);}));} _temp343=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp343);{ void*
_temp354=( void*) _temp343->v; void* _temp362; void* _temp364; struct Cyc_Absyn_Exp*
_temp366; _LL356: if(( unsigned int) _temp354 > 1u?*(( int*) _temp354) == Cyc_Absyn_Eq_constr:
0){ _LL363: _temp362=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp354)->f1;
if( _temp362 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL357;} else{ goto _LL358;}}
else{ goto _LL358;} _LL358: if(( unsigned int) _temp354 > 1u?*(( int*) _temp354)
== Cyc_Absyn_Eq_constr: 0){ _LL365: _temp364=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp354)->f1; if(( unsigned int) _temp364 > 1u?*(( int*) _temp364) == Cyc_Absyn_Upper_b:
0){ _LL367: _temp366=(( struct Cyc_Absyn_Upper_b_struct*) _temp364)->f1; goto
_LL359;} else{ goto _LL360;}} else{ goto _LL360;} _LL360: goto _LL361; _LL357:
return t1; _LL359: { unsigned int _temp368= Cyc_Evexp_eval_const_uint_exp(
_temp366); _temp347=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp347);{ int is_nullable;{ void* _temp369=( void*)
_temp347->v; int _temp375; _LL371: if(( unsigned int) _temp369 > 1u?*(( int*)
_temp369) == Cyc_Absyn_Eq_constr: 0){ _LL376: _temp375=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp369)->f1; goto _LL372;} else{ goto _LL373;} _LL373: goto _LL374; _LL372:
if( _temp375){ struct Cyc_Absyn_PointerType_struct* _temp377=({ struct Cyc_Absyn_PointerType_struct*
_temp380=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp380[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp381; _temp381.tag= Cyc_Absyn_PointerType;
_temp381.f1=({ struct Cyc_Absyn_PtrInfo _temp382; _temp382.elt_typ=( void*)
_temp351; _temp382.rgn_typ=( void*) _temp349; _temp382.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp382.tq= _temp345; _temp382.bounds=
_temp343; _temp382;}); _temp381;}); _temp380;}); Cyc_Tcutil_warn( e1->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp378= Cyc_Absynpp_typ2string( t1);
struct _tagged_arr _temp379= Cyc_Absynpp_typ2string(( void*) _temp377); xprintf("coercing from %.*s to %.*s",
_get_arr_size( _temp378, 1u), _temp378.curr, _get_arr_size( _temp379, 1u),
_temp379.curr);})); Cyc_Tcutil_unchecked_cast( te, e1,( void*) _temp377);} goto
_LL370; _LL374:( void*)( _temp347->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp383=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp383[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp384; _temp384.tag= Cyc_Absyn_Eq_constr;
_temp384.f1=( void*) 0; _temp384;}); _temp383;}))); goto _LL370; _LL370:;} if(
Cyc_Tcutil_is_const_exp( te, e2)){ unsigned int _temp385= Cyc_Evexp_eval_const_uint_exp(
e2); if( _temp368 < _temp385){ _temp385= _temp368; Cyc_Tcutil_warn( e1->loc,(
struct _tagged_arr) xprintf("pointer arithmetic is out of bounds"));}{ struct
Cyc_Absyn_Exp* _temp386= Cyc_Absyn_uint_exp( _temp368 - _temp385, 0); _temp386->topt=({
struct Cyc_Core_Opt* _temp387=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp387->v=( void*) Cyc_Absyn_uint_t; _temp387;}); return( void*)({
struct Cyc_Absyn_PointerType_struct* _temp388=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp388[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp389; _temp389.tag= Cyc_Absyn_PointerType;
_temp389.f1=({ struct Cyc_Absyn_PtrInfo _temp390; _temp390.elt_typ=( void*)
_temp351; _temp390.rgn_typ=( void*) _temp349; _temp390.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp390.tq= _temp345; _temp390.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp391=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp391[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp392; _temp392.tag= Cyc_Absyn_Upper_b; _temp392.f1= _temp386; _temp392;});
_temp391;})); _temp390;}); _temp389;}); _temp388;});}} else{ Cyc_Tcutil_terr( e1->loc,
_tag_arr("cannot statically check pointer arithmetic.\n   suggest you cast the first expression to a ? type.",
sizeof( unsigned char), 98u)); return t1;}}} _LL361:( void*)( _temp343->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp393=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp393[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp394; _temp394.tag= Cyc_Absyn_Eq_constr; _temp394.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp394;}); _temp393;}))); return t1;
_LL355:;} _LL340: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL336:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp395= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct _tagged_arr _temp396=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_get_arr_size( _temp395, 1u), _temp395.curr, _get_arr_size( _temp396, 1u),
_temp396.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp397= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp398= Cyc_Absynpp_typ2string(
t2); xprintf("expecting either %.*s or int but found %.*s", _get_arr_size(
_temp397, 1u), _temp397.curr, _get_arr_size( _temp398, 1u), _temp398.curr);}));}
return t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp399=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp399->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp399;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp401=({ struct _tuple0 _temp400;
_temp400.f1= Cyc_Tcutil_compress( t1); _temp400.f2= Cyc_Tcutil_compress( t2);
_temp400;}); void* _temp407; struct Cyc_Absyn_PtrInfo _temp409; struct Cyc_Absyn_Conref*
_temp411; struct Cyc_Absyn_Tqual _temp413; struct Cyc_Absyn_Conref* _temp415;
void* _temp417; void* _temp419; void* _temp421; struct Cyc_Absyn_PtrInfo
_temp423; struct Cyc_Absyn_Conref* _temp425; struct Cyc_Absyn_Tqual _temp427;
struct Cyc_Absyn_Conref* _temp429; void* _temp431; void* _temp433; _LL403:
_LL422: _temp421= _temp401.f1; if(( unsigned int) _temp421 > 4u?*(( int*)
_temp421) == Cyc_Absyn_PointerType: 0){ _LL424: _temp423=(( struct Cyc_Absyn_PointerType_struct*)
_temp421)->f1; _LL434: _temp433=( void*) _temp423.elt_typ; goto _LL432; _LL432:
_temp431=( void*) _temp423.rgn_typ; goto _LL430; _LL430: _temp429= _temp423.nullable;
goto _LL428; _LL428: _temp427= _temp423.tq; goto _LL426; _LL426: _temp425=
_temp423.bounds; goto _LL408;} else{ goto _LL405;} _LL408: _temp407= _temp401.f2;
if(( unsigned int) _temp407 > 4u?*(( int*) _temp407) == Cyc_Absyn_PointerType: 0){
_LL410: _temp409=(( struct Cyc_Absyn_PointerType_struct*) _temp407)->f1; _LL420:
_temp419=( void*) _temp409.elt_typ; goto _LL418; _LL418: _temp417=( void*)
_temp409.rgn_typ; goto _LL416; _LL416: _temp415= _temp409.nullable; goto _LL414;
_LL414: _temp413= _temp409.tq; goto _LL412; _LL412: _temp411= _temp409.bounds;
goto _LL404;} else{ goto _LL405;} _LL405: goto _LL406; _LL404: if( Cyc_Tcutil_unify(
_temp433, _temp419)){ return Cyc_Absyn_sint_t;} goto _LL402; _LL406: goto _LL402;
_LL402:;} Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp435= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp436= Cyc_Absynpp_typ2string(
t2); xprintf("comparison not allowed between %.*s and %.*s", _get_arr_size(
_temp435, 1u), _temp435.curr, _get_arr_size( _temp436, 1u), _temp436.curr);}));
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp437=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp437->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp437;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp438= p; _LL440: if( _temp438 ==( void*)
Cyc_Absyn_Plus){ goto _LL441;} else{ goto _LL442;} _LL442: if( _temp438 ==( void*)
Cyc_Absyn_Minus){ goto _LL443;} else{ goto _LL444;} _LL444: if( _temp438 ==(
void*) Cyc_Absyn_Times){ goto _LL445;} else{ goto _LL446;} _LL446: if( _temp438
==( void*) Cyc_Absyn_Div){ goto _LL447;} else{ goto _LL448;} _LL448: if(
_temp438 ==( void*) Cyc_Absyn_Mod){ goto _LL449;} else{ goto _LL450;} _LL450:
if( _temp438 ==( void*) Cyc_Absyn_Bitand){ goto _LL451;} else{ goto _LL452;}
_LL452: if( _temp438 ==( void*) Cyc_Absyn_Bitor){ goto _LL453;} else{ goto
_LL454;} _LL454: if( _temp438 ==( void*) Cyc_Absyn_Bitxor){ goto _LL455;} else{
goto _LL456;} _LL456: if( _temp438 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL457;}
else{ goto _LL458;} _LL458: if( _temp438 ==( void*) Cyc_Absyn_Bitlrshift){ goto
_LL459;} else{ goto _LL460;} _LL460: if( _temp438 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL461;} else{ goto _LL462;} _LL462: if( _temp438 ==( void*) Cyc_Absyn_Eq){
goto _LL463;} else{ goto _LL464;} _LL464: if( _temp438 ==( void*) Cyc_Absyn_Neq){
goto _LL465;} else{ goto _LL466;} _LL466: if( _temp438 ==( void*) Cyc_Absyn_Gt){
goto _LL467;} else{ goto _LL468;} _LL468: if( _temp438 ==( void*) Cyc_Absyn_Lt){
goto _LL469;} else{ goto _LL470;} _LL470: if( _temp438 ==( void*) Cyc_Absyn_Gte){
goto _LL471;} else{ goto _LL472;} _LL472: if( _temp438 ==( void*) Cyc_Absyn_Lte){
goto _LL473;} else{ goto _LL474;} _LL474: goto _LL475; _LL441: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL443: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL445: goto _LL447;
_LL447: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL449: goto _LL451; _LL451: goto _LL453; _LL453: goto _LL455; _LL455: goto
_LL457; _LL457: goto _LL459; _LL459: goto _LL461; _LL461: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL463: goto _LL465; _LL465: goto _LL467;
_LL467: goto _LL469; _LL469: goto _LL471; _LL471: goto _LL473; _LL473: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL475:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp476=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp476[ 0]=({ struct Cyc_Core_Failure_struct
_temp477; _temp477.tag= Cyc_Core_Failure; _temp477.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp477;}); _temp476;})); _LL439:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp478= p; _LL480: if( _temp478
==( void*) Cyc_Absyn_Printf){ goto _LL481;} else{ goto _LL482;} _LL482: if(
_temp478 ==( void*) Cyc_Absyn_Fprintf){ goto _LL483;} else{ goto _LL484;} _LL484:
if( _temp478 ==( void*) Cyc_Absyn_Xprintf){ goto _LL485;} else{ goto _LL486;}
_LL486: if( _temp478 ==( void*) Cyc_Absyn_Scanf){ goto _LL487;} else{ goto
_LL488;} _LL488: if( _temp478 ==( void*) Cyc_Absyn_Fscanf){ goto _LL489;} else{
goto _LL490;} _LL490: if( _temp478 ==( void*) Cyc_Absyn_Sscanf){ goto _LL491;}
else{ goto _LL492;} _LL492: goto _LL493; _LL481: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("missing format string in printf", sizeof( unsigned char), 32u));}
Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd,(( struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs);
t=( void*) Cyc_Absyn_VoidType; goto _LL479; _LL483: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fprintf",
sizeof( unsigned char), 29u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp494=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _get_arr_size( _temp494, 1u),
_temp494.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType; goto _LL479;
_LL485: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in xprintf",
sizeof( unsigned char), 33u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_format_typs); t= Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt*
_temp495=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp495->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp495;}))); goto _LL479;
_LL487: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in scanf",
sizeof( unsigned char), 31u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto
_LL479; _LL489: if( es == 0? 1:(( struct Cyc_List_List*) _check_null( es))->tl
== 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fscanf",
sizeof( unsigned char), 28u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp496=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _get_arr_size( _temp496, 1u),
_temp496.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL479; _LL491: Cyc_Tcutil_terr(
loc, _tag_arr("sscanf is not supported", sizeof( unsigned char), 24u)); t= Cyc_Absyn_sint_t;
goto _LL479; _LL493: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ case 0: _LL497: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has 0 arguments",
sizeof( unsigned char), 35u)); case 1: _LL498: t= Cyc_Tcexp_tcUnPrimop( te, loc,
topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
break; case 2: _LL499: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd);
break; default: _LL500: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u));} goto _LL479; _LL479:;} return t;}} struct
_tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{ void* _temp502=( void*) e->r;
void* _temp526; struct Cyc_Absyn_Vardecl* _temp528; struct _tuple1* _temp530;
void* _temp532; struct Cyc_Absyn_Vardecl* _temp534; struct _tuple1* _temp536;
void* _temp538; struct Cyc_Absyn_Vardecl* _temp540; struct _tuple1* _temp542;
void* _temp544; struct Cyc_Absyn_Vardecl* _temp546; struct _tuple1* _temp548;
struct Cyc_Absyn_Exp* _temp550; struct Cyc_Absyn_Exp* _temp552; struct
_tagged_arr* _temp554; struct Cyc_Absyn_Exp* _temp556; struct _tagged_arr*
_temp558; struct Cyc_Absyn_Exp* _temp560; struct Cyc_Absyn_Exp* _temp562; struct
Cyc_Absyn_Exp* _temp564; struct Cyc_List_List* _temp566; struct Cyc_Absyn_Exp*
_temp568; _LL504: if(*(( int*) _temp502) == Cyc_Absyn_Var_e){ _LL531: _temp530=((
struct Cyc_Absyn_Var_e_struct*) _temp502)->f1; goto _LL527; _LL527: _temp526=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp502)->f2; if(( unsigned int)
_temp526 > 1u?*(( int*) _temp526) == Cyc_Absyn_Param_b: 0){ _LL529: _temp528=((
struct Cyc_Absyn_Param_b_struct*) _temp526)->f1; goto _LL505;} else{ goto _LL506;}}
else{ goto _LL506;} _LL506: if(*(( int*) _temp502) == Cyc_Absyn_Var_e){ _LL537:
_temp536=(( struct Cyc_Absyn_Var_e_struct*) _temp502)->f1; goto _LL533; _LL533:
_temp532=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp502)->f2; if((
unsigned int) _temp532 > 1u?*(( int*) _temp532) == Cyc_Absyn_Local_b: 0){ _LL535:
_temp534=(( struct Cyc_Absyn_Local_b_struct*) _temp532)->f1; goto _LL507;} else{
goto _LL508;}} else{ goto _LL508;} _LL508: if(*(( int*) _temp502) == Cyc_Absyn_Var_e){
_LL543: _temp542=(( struct Cyc_Absyn_Var_e_struct*) _temp502)->f1; goto _LL539;
_LL539: _temp538=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp502)->f2; if((
unsigned int) _temp538 > 1u?*(( int*) _temp538) == Cyc_Absyn_Pat_b: 0){ _LL541:
_temp540=(( struct Cyc_Absyn_Pat_b_struct*) _temp538)->f1; goto _LL509;} else{
goto _LL510;}} else{ goto _LL510;} _LL510: if(*(( int*) _temp502) == Cyc_Absyn_Var_e){
_LL549: _temp548=(( struct Cyc_Absyn_Var_e_struct*) _temp502)->f1; goto _LL545;
_LL545: _temp544=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp502)->f2; if((
unsigned int) _temp544 > 1u?*(( int*) _temp544) == Cyc_Absyn_Global_b: 0){
_LL547: _temp546=(( struct Cyc_Absyn_Global_b_struct*) _temp544)->f1; goto
_LL511;} else{ goto _LL512;}} else{ goto _LL512;} _LL512: if(*(( int*) _temp502)
== Cyc_Absyn_Subscript_e){ _LL553: _temp552=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp502)->f1; goto _LL551; _LL551: _temp550=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp502)->f2; goto _LL513;} else{ goto _LL514;} _LL514: if(*(( int*) _temp502)
== Cyc_Absyn_StructMember_e){ _LL557: _temp556=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp502)->f1; goto _LL555; _LL555: _temp554=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp502)->f2; goto _LL515;} else{ goto _LL516;} _LL516: if(*(( int*) _temp502)
== Cyc_Absyn_StructArrow_e){ _LL561: _temp560=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp502)->f1; goto _LL559; _LL559: _temp558=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp502)->f2; goto _LL517;} else{ goto _LL518;} _LL518: if(*(( int*) _temp502)
== Cyc_Absyn_Deref_e){ _LL563: _temp562=(( struct Cyc_Absyn_Deref_e_struct*)
_temp502)->f1; goto _LL519;} else{ goto _LL520;} _LL520: if(*(( int*) _temp502)
== Cyc_Absyn_NoInstantiate_e){ _LL565: _temp564=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp502)->f1; goto _LL521;} else{ goto _LL522;} _LL522: if(*(( int*) _temp502)
== Cyc_Absyn_Instantiate_e){ _LL569: _temp568=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp502)->f1; goto _LL567; _LL567: _temp566=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp502)->f2; goto _LL523;} else{ goto _LL524;} _LL524: goto _LL525; _LL505:
_temp534= _temp528; goto _LL507; _LL507: _temp540= _temp534; goto _LL509; _LL509:
_temp546= _temp540; goto _LL511; _LL511: if( !( _temp546->tq).q_const){ return;}
goto _LL503; _LL513:{ void* _temp570= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp552->topt))->v); struct Cyc_Absyn_PtrInfo _temp580; struct Cyc_Absyn_Conref*
_temp582; struct Cyc_Absyn_Tqual _temp584; struct Cyc_Absyn_Conref* _temp586;
void* _temp588; void* _temp590; struct Cyc_Absyn_Exp* _temp592; struct Cyc_Absyn_Tqual
_temp594; void* _temp596; struct Cyc_List_List* _temp598; _LL572: if((
unsigned int) _temp570 > 4u?*(( int*) _temp570) == Cyc_Absyn_PointerType: 0){
_LL581: _temp580=(( struct Cyc_Absyn_PointerType_struct*) _temp570)->f1; _LL591:
_temp590=( void*) _temp580.elt_typ; goto _LL589; _LL589: _temp588=( void*)
_temp580.rgn_typ; goto _LL587; _LL587: _temp586= _temp580.nullable; goto _LL585;
_LL585: _temp584= _temp580.tq; goto _LL583; _LL583: _temp582= _temp580.bounds;
goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int) _temp570 > 4u?*((
int*) _temp570) == Cyc_Absyn_ArrayType: 0){ _LL597: _temp596=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp570)->f1; goto _LL595; _LL595: _temp594=((
struct Cyc_Absyn_ArrayType_struct*) _temp570)->f2; goto _LL593; _LL593: _temp592=((
struct Cyc_Absyn_ArrayType_struct*) _temp570)->f3; goto _LL575;} else{ goto
_LL576;} _LL576: if(( unsigned int) _temp570 > 4u?*(( int*) _temp570) == Cyc_Absyn_TupleType:
0){ _LL599: _temp598=(( struct Cyc_Absyn_TupleType_struct*) _temp570)->f1; goto
_LL577;} else{ goto _LL578;} _LL578: goto _LL579; _LL573: _temp594= _temp584;
goto _LL575; _LL575: if( ! _temp594.q_const){ return;} goto _LL571; _LL577: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp550);{ struct _handler_cons
_temp600; _push_handler(& _temp600);{ int _temp602= 0; if( setjmp( _temp600.handler)){
_temp602= 1;} if( ! _temp602){{ struct _tuple8 _temp605; void* _temp606; struct
Cyc_Absyn_Tqual _temp608; struct _tuple8* _temp603=(( struct _tuple8*(*)( struct
Cyc_List_List* x, int i)) Cyc_List_nth)( _temp598,( int) i); _temp605=* _temp603;
_LL609: _temp608= _temp605.f1; goto _LL607; _LL607: _temp606= _temp605.f2; goto
_LL604; _LL604: if( ! _temp608.q_const){ _npop_handler( 0u); return;}};
_pop_handler();} else{ void* _temp601=( void*) _exn_thrown; void* _temp611=
_temp601; _LL613: if( _temp611 == Cyc_List_Nth){ goto _LL614;} else{ goto _LL615;}
_LL615: goto _LL616; _LL614: return; _LL616:( void) _throw( _temp611); _LL612:;}}}
goto _LL571;} _LL579: goto _LL571; _LL571:;} goto _LL503; _LL515:{ void*
_temp617= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp556->topt))->v); struct Cyc_Absyn_Structdecl** _temp629; struct Cyc_List_List*
_temp631; struct _tuple1* _temp633; struct Cyc_List_List* _temp635; struct Cyc_List_List*
_temp637; struct Cyc_Absyn_Uniondecl** _temp639; struct Cyc_List_List* _temp641;
struct _tuple1* _temp643; _LL619: if(( unsigned int) _temp617 > 4u?*(( int*)
_temp617) == Cyc_Absyn_StructType: 0){ _LL634: _temp633=(( struct Cyc_Absyn_StructType_struct*)
_temp617)->f1; goto _LL632; _LL632: _temp631=(( struct Cyc_Absyn_StructType_struct*)
_temp617)->f2; goto _LL630; _LL630: _temp629=(( struct Cyc_Absyn_StructType_struct*)
_temp617)->f3; goto _LL620;} else{ goto _LL621;} _LL621: if(( unsigned int)
_temp617 > 4u?*(( int*) _temp617) == Cyc_Absyn_AnonUnionType: 0){ _LL636:
_temp635=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp617)->f1; goto _LL622;}
else{ goto _LL623;} _LL623: if(( unsigned int) _temp617 > 4u?*(( int*) _temp617)
== Cyc_Absyn_AnonStructType: 0){ _LL638: _temp637=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp617)->f1; goto _LL624;} else{ goto _LL625;} _LL625: if(( unsigned int)
_temp617 > 4u?*(( int*) _temp617) == Cyc_Absyn_UnionType: 0){ _LL644: _temp643=((
struct Cyc_Absyn_UnionType_struct*) _temp617)->f1; goto _LL642; _LL642: _temp641=((
struct Cyc_Absyn_UnionType_struct*) _temp617)->f2; goto _LL640; _LL640: _temp639=((
struct Cyc_Absyn_UnionType_struct*) _temp617)->f3; goto _LL626;} else{ goto
_LL627;} _LL627: goto _LL628; _LL620: { struct Cyc_Absyn_Structfield* sf=
_temp629 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp629)), _temp554); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL618;} _LL622: _temp637= _temp635; goto _LL624; _LL624: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp637, _temp554); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL618;} _LL626: { struct Cyc_Absyn_Structfield*
sf= _temp639 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp639)), _temp554); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL618;} _LL628: goto _LL618; _LL618:;} goto _LL503; _LL517:{ void*
_temp645= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp560->topt))->v); struct Cyc_Absyn_PtrInfo _temp651; struct Cyc_Absyn_Conref*
_temp653; struct Cyc_Absyn_Tqual _temp655; struct Cyc_Absyn_Conref* _temp657;
void* _temp659; void* _temp661; _LL647: if(( unsigned int) _temp645 > 4u?*(( int*)
_temp645) == Cyc_Absyn_PointerType: 0){ _LL652: _temp651=(( struct Cyc_Absyn_PointerType_struct*)
_temp645)->f1; _LL662: _temp661=( void*) _temp651.elt_typ; goto _LL660; _LL660:
_temp659=( void*) _temp651.rgn_typ; goto _LL658; _LL658: _temp657= _temp651.nullable;
goto _LL656; _LL656: _temp655= _temp651.tq; goto _LL654; _LL654: _temp653=
_temp651.bounds; goto _LL648;} else{ goto _LL649;} _LL649: goto _LL650; _LL648:{
void* _temp663= Cyc_Tcutil_compress( _temp661); struct Cyc_Absyn_Structdecl**
_temp675; struct Cyc_List_List* _temp677; struct _tuple1* _temp679; struct Cyc_Absyn_Uniondecl**
_temp681; struct Cyc_List_List* _temp683; struct _tuple1* _temp685; struct Cyc_List_List*
_temp687; struct Cyc_List_List* _temp689; _LL665: if(( unsigned int) _temp663 >
4u?*(( int*) _temp663) == Cyc_Absyn_StructType: 0){ _LL680: _temp679=(( struct
Cyc_Absyn_StructType_struct*) _temp663)->f1; goto _LL678; _LL678: _temp677=((
struct Cyc_Absyn_StructType_struct*) _temp663)->f2; goto _LL676; _LL676:
_temp675=(( struct Cyc_Absyn_StructType_struct*) _temp663)->f3; goto _LL666;}
else{ goto _LL667;} _LL667: if(( unsigned int) _temp663 > 4u?*(( int*) _temp663)
== Cyc_Absyn_UnionType: 0){ _LL686: _temp685=(( struct Cyc_Absyn_UnionType_struct*)
_temp663)->f1; goto _LL684; _LL684: _temp683=(( struct Cyc_Absyn_UnionType_struct*)
_temp663)->f2; goto _LL682; _LL682: _temp681=(( struct Cyc_Absyn_UnionType_struct*)
_temp663)->f3; goto _LL668;} else{ goto _LL669;} _LL669: if(( unsigned int)
_temp663 > 4u?*(( int*) _temp663) == Cyc_Absyn_AnonStructType: 0){ _LL688:
_temp687=(( struct Cyc_Absyn_AnonStructType_struct*) _temp663)->f1; goto _LL670;}
else{ goto _LL671;} _LL671: if(( unsigned int) _temp663 > 4u?*(( int*) _temp663)
== Cyc_Absyn_AnonUnionType: 0){ _LL690: _temp689=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp663)->f1; goto _LL672;} else{ goto _LL673;} _LL673: goto _LL674; _LL666: {
struct Cyc_Absyn_Structfield* sf= _temp675 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp675)), _temp558); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL664;} _LL668: { struct Cyc_Absyn_Structfield*
sf= _temp681 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp681)), _temp558); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL664;} _LL670: _temp689= _temp687; goto _LL672; _LL672: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp689, _temp558); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL664;} _LL674: goto _LL664;
_LL664:;} goto _LL646; _LL650: goto _LL646; _LL646:;} goto _LL503; _LL519:{ void*
_temp691= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp562->topt))->v); struct Cyc_Absyn_PtrInfo _temp699; struct Cyc_Absyn_Conref*
_temp701; struct Cyc_Absyn_Tqual _temp703; struct Cyc_Absyn_Conref* _temp705;
void* _temp707; void* _temp709; struct Cyc_Absyn_Exp* _temp711; struct Cyc_Absyn_Tqual
_temp713; void* _temp715; _LL693: if(( unsigned int) _temp691 > 4u?*(( int*)
_temp691) == Cyc_Absyn_PointerType: 0){ _LL700: _temp699=(( struct Cyc_Absyn_PointerType_struct*)
_temp691)->f1; _LL710: _temp709=( void*) _temp699.elt_typ; goto _LL708; _LL708:
_temp707=( void*) _temp699.rgn_typ; goto _LL706; _LL706: _temp705= _temp699.nullable;
goto _LL704; _LL704: _temp703= _temp699.tq; goto _LL702; _LL702: _temp701=
_temp699.bounds; goto _LL694;} else{ goto _LL695;} _LL695: if(( unsigned int)
_temp691 > 4u?*(( int*) _temp691) == Cyc_Absyn_ArrayType: 0){ _LL716: _temp715=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp691)->f1; goto _LL714; _LL714:
_temp713=(( struct Cyc_Absyn_ArrayType_struct*) _temp691)->f2; goto _LL712;
_LL712: _temp711=(( struct Cyc_Absyn_ArrayType_struct*) _temp691)->f3; goto
_LL696;} else{ goto _LL697;} _LL697: goto _LL698; _LL694: _temp713= _temp703;
goto _LL696; _LL696: if( ! _temp713.q_const){ return;} goto _LL692; _LL698: goto
_LL692; _LL692:;} goto _LL503; _LL521: _temp568= _temp564; goto _LL523; _LL523:
Cyc_Tcexp_check_writable( te, _temp568); return; _LL525: goto _LL503; _LL503:;}
Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a read-only location",
sizeof( unsigned char), 38u));} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void*
i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("increment/decrement of non-lvalue",
sizeof( unsigned char), 34u));} Cyc_Tcexp_check_writable( te, e);{ void* t=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_is_numeric(
e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp717= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic or ? type but found %.*s",
_get_arr_size( _temp717, 1u), _temp717.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1, _tag_arr("conditional expression", sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp(
te, topt, e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp724=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp724->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp724;})); struct Cyc_List_List* _temp718=({
struct Cyc_List_List* _temp723=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp723->hd=( void*) e3; _temp723->tl= 0; _temp723;});
struct Cyc_List_List* _temp719=({ struct Cyc_List_List* _temp722=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp722->hd=( void*) e2; _temp722->tl=
_temp718; _temp722;}); if( ! Cyc_Tcutil_coerce_list( te, t, _temp719)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp720= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); struct _tagged_arr
_temp721= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
e3->topt))->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_get_arr_size( _temp720, 1u), _temp720.curr, _get_arr_size( _temp721, 1u),
_temp721.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te,
0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v)), e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ void* _temp725= Cyc_Tcutil_compress( t1); struct
Cyc_Absyn_Exp* _temp731; struct Cyc_Absyn_Tqual _temp733; void* _temp735; _LL727:
if(( unsigned int) _temp725 > 4u?*(( int*) _temp725) == Cyc_Absyn_ArrayType: 0){
_LL736: _temp735=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp725)->f1;
goto _LL734; _LL734: _temp733=(( struct Cyc_Absyn_ArrayType_struct*) _temp725)->f2;
goto _LL732; _LL732: _temp731=(( struct Cyc_Absyn_ArrayType_struct*) _temp725)->f3;
goto _LL728;} else{ goto _LL729;} _LL729: goto _LL730; _LL728: Cyc_Tcutil_terr(
loc, _tag_arr("cannot assign to an array", sizeof( unsigned char), 26u)); goto
_LL726; _LL730: goto _LL726; _LL726:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue", sizeof(
unsigned char), 25u));} Cyc_Tcexp_check_writable( te, e1); if( po == 0){ if( !
Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp737= Cyc_Absynpp_typ2string( t1);
struct _tagged_arr _temp738= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_get_arr_size( _temp737, 1u), _temp737.curr, _get_arr_size( _temp738, 1u),
_temp738.curr);}));}} else{ void* _temp739=( void*)(( struct Cyc_Core_Opt*)
_check_null( po))->v; void* _temp740= Cyc_Tcexp_tcBinPrimop( te, loc, 0,
_temp739, e1, e2); if( !( Cyc_Tcutil_unify( _temp740, t1)? 1: Cyc_Tcutil_coerceable(
_temp740))){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("Cannot use this operator in an assignment",
sizeof( unsigned char), 42u));} return _temp740;} return t2;}} static void* Cyc_Tcexp_tcSeqExp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, topt, e2); return( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;}
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; for( fields= fs; fields != 0;
fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ void* _temp743;
struct Cyc_Position_Segment* _temp745; struct Cyc_List_List* _temp747; struct
Cyc_List_List* _temp749; struct _tuple1* _temp751; struct Cyc_Absyn_Tunionfield
_temp741=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL752: _temp751= _temp741.name; goto _LL750; _LL750: _temp749=
_temp741.tvs; goto _LL748; _LL748: _temp747= _temp741.typs; goto _LL746; _LL746:
_temp745= _temp741.loc; goto _LL744; _LL744: _temp743=( void*) _temp741.sc; goto
_LL742; _LL742: if(( _temp747 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp747))->tl != 0)? 1: _temp749 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp747))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp755; struct Cyc_Position_Segment* _temp757; struct Cyc_List_List*
_temp759; struct Cyc_List_List* _temp761; struct _tuple1* _temp763; struct Cyc_Absyn_Tunionfield
_temp753=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL764: _temp763= _temp753.name; goto _LL762; _LL762: _temp761=
_temp753.tvs; goto _LL760; _LL760: _temp759= _temp753.typs; goto _LL758; _LL758:
_temp757= _temp753.loc; goto _LL756; _LL756: _temp755=( void*) _temp753.sc; goto
_LL754; _LL754: if(( _temp759 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp759))->tl != 0)? 1: _temp761 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp759))->hd)).f2);
if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd);}}} Cyc_Tcutil_terr( e->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp765= Cyc_Absynpp_typ2string( tu);
struct _tagged_arr _temp766= Cyc_Absynpp_typ2string( t1); xprintf("can't find a field in %.*s to inject a value of type %.*s",
_get_arr_size( _temp765, 1u), _temp765.curr, _get_arr_size( _temp766, 1u),
_temp766.curr);})); return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* es, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){
te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp767= t;
struct Cyc_Absyn_PtrInfo _temp773; struct Cyc_Absyn_Conref* _temp775; struct Cyc_Absyn_Tqual
_temp777; struct Cyc_Absyn_Conref* _temp779; void* _temp781; void* _temp783;
_LL769: if(( unsigned int) _temp767 > 4u?*(( int*) _temp767) == Cyc_Absyn_PointerType:
0){ _LL774: _temp773=(( struct Cyc_Absyn_PointerType_struct*) _temp767)->f1;
_LL784: _temp783=( void*) _temp773.elt_typ; goto _LL782; _LL782: _temp781=( void*)
_temp773.rgn_typ; goto _LL780; _LL780: _temp779= _temp773.nullable; goto _LL778;
_LL778: _temp777= _temp773.tq; goto _LL776; _LL776: _temp775= _temp773.bounds;
goto _LL770;} else{ goto _LL771;} _LL771: goto _LL772; _LL770: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp781); Cyc_Tcutil_check_nonzero_bound( loc, _temp775);{ void*
_temp785= Cyc_Tcutil_compress( _temp783); struct Cyc_Absyn_FnInfo _temp791;
struct Cyc_List_List* _temp793; struct Cyc_List_List* _temp795; struct Cyc_Absyn_VarargInfo*
_temp797; int _temp799; struct Cyc_List_List* _temp801; void* _temp803; struct
Cyc_Core_Opt* _temp805; struct Cyc_List_List* _temp807; _LL787: if((
unsigned int) _temp785 > 4u?*(( int*) _temp785) == Cyc_Absyn_FnType: 0){ _LL792:
_temp791=(( struct Cyc_Absyn_FnType_struct*) _temp785)->f1; _LL808: _temp807=
_temp791.tvars; goto _LL806; _LL806: _temp805= _temp791.effect; goto _LL804;
_LL804: _temp803=( void*) _temp791.ret_typ; goto _LL802; _LL802: _temp801=
_temp791.args; goto _LL800; _LL800: _temp799= _temp791.c_varargs; goto _LL798;
_LL798: _temp797= _temp791.cyc_varargs; goto _LL796; _LL796: _temp795= _temp791.rgn_po;
goto _LL794; _LL794: _temp793= _temp791.attributes; goto _LL788;} else{ goto
_LL789;} _LL789: goto _LL790; _LL788: if( topt != 0){ Cyc_Tcutil_unify( _temp803,*((
void**) _check_null( topt)));} while( es != 0? _temp801 != 0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void*
t2=(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp801))->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp809= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct _tagged_arr
_temp810= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_get_arr_size( _temp809, 1u), _temp809.curr, _get_arr_size( _temp810, 1u),
_temp810.curr);}));} es=(( struct Cyc_List_List*) _check_null( es))->tl;
_temp801=(( struct Cyc_List_List*) _check_null( _temp801))->tl;} if( _temp801 !=
0){ Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function", sizeof(
unsigned char), 31u));} else{ if( es != 0){ if( _temp799){ for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}} else{ if(
_temp797 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("too many arguments for function",
sizeof( unsigned char), 32u));} else{ int _temp813; void* _temp815; void*
_temp817; struct Cyc_Absyn_Tqual _temp819; struct Cyc_Core_Opt* _temp821; struct
Cyc_Absyn_VarargInfo _temp811=*(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp797)); _LL822: _temp821= _temp811.name; goto _LL820; _LL820: _temp819=
_temp811.tq; goto _LL818; _LL818: _temp817=( void*) _temp811.type; goto _LL816;
_LL816: _temp815=( void*) _temp811.rgn; goto _LL814; _LL814: _temp813= _temp811.inject;
goto _LL812; _LL812: { struct Cyc_Absyn_VarargCallInfo* _temp823=({ struct Cyc_Absyn_VarargCallInfo*
_temp850=( struct Cyc_Absyn_VarargCallInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp850->num_varargs= 0; _temp850->injectors= 0; _temp850->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp797); _temp850;});* vararg_call_info= _temp823; if( ! Cyc_Tcutil_unify(
Cyc_Tcenv_curr_rgn( te), _temp815)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp824= Cyc_Absynpp_typ2string( _temp815); struct
_tagged_arr _temp825= Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te)); xprintf("I have no idea how this could fail, but it did because the region for the varargs (%.*s) doesn't unify with the expected new_block region (%.*s).",
_get_arr_size( _temp824, 1u), _temp824.curr, _get_arr_size( _temp825, 1u),
_temp825.curr);}));} if( ! _temp813){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp823))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp817,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp817)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp826= Cyc_Absynpp_typ2string(
_temp817); struct _tagged_arr _temp827= Cyc_Absynpp_typ2string(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); xprintf("vararg requires type %.*s but argument has type %.*s",
_get_arr_size( _temp826, 1u), _temp826.curr, _get_arr_size( _temp827, 1u),
_temp827.curr);}));}}} else{ void* _temp828= Cyc_Tcutil_compress( _temp817);
struct Cyc_Absyn_TunionInfo _temp834; void* _temp836; struct Cyc_List_List*
_temp838; void* _temp840; struct Cyc_Absyn_Tuniondecl* _temp842; _LL830: if((
unsigned int) _temp828 > 4u?*(( int*) _temp828) == Cyc_Absyn_TunionType: 0){
_LL835: _temp834=(( struct Cyc_Absyn_TunionType_struct*) _temp828)->f1; _LL841:
_temp840=( void*) _temp834.tunion_info; if(*(( int*) _temp840) == Cyc_Absyn_KnownTunion){
_LL843: _temp842=(( struct Cyc_Absyn_KnownTunion_struct*) _temp840)->f1; goto
_LL839;} else{ goto _LL832;} _LL839: _temp838= _temp834.targs; goto _LL837;
_LL837: _temp836=( void*) _temp834.rgn; goto _LL831;} else{ goto _LL832;} _LL832:
goto _LL833; _LL831: { struct Cyc_Absyn_Tuniondecl* _temp844=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp842->name); if( ! Cyc_Tcutil_unify( _temp836, _temp815)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr) xprintf("bad region for injected varargs"));}{ struct
Cyc_List_List* fields= 0; if( _temp844->fields == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp845= Cyc_Absynpp_typ2string(
_temp817); xprintf("can't inject into %.*s", _get_arr_size( _temp845, 1u),
_temp845.curr);}));} else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp844->fields))->v;}{ struct _RegionHandle _temp846= _new_region();
struct _RegionHandle* rgn=& _temp846; _push_region( rgn);{ struct Cyc_List_List*
_temp847=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp844->tvs, _temp838); for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){(( struct Cyc_Absyn_VarargCallInfo*) _check_null(
_temp823))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp848= Cyc_Tcexp_tcInjection( te, e1, _temp817,
rgn, _temp847, fields); if( _temp848 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp823))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp823))->injectors,({ struct Cyc_List_List* _temp849=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp849->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp848)); _temp849->tl= 0;
_temp849;}));}}}}}; _pop_region( rgn);} goto _LL829;}} _LL833: Cyc_Tcutil_terr(
loc, _tag_arr("bad inject vararg type", sizeof( unsigned char), 23u)); goto
_LL829; _LL829:;}}}}}} Cyc_Tcenv_check_effect_accessible( te, loc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp805))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp795); return _temp803; _LL790: return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("expected pointer to function", sizeof( unsigned char), 29u)); _LL786:;}
_LL772: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u)); _LL768:;}} static void* Cyc_Tcexp_tcThrow( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( ! Cyc_Tcutil_coerce_use(
te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp851= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("expected xtunion exn but found %.*s",
_get_arr_size( _temp851, 1u), _temp851.curr);}));} return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp852=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp852->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp852;}));}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*)
Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v)));{ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp853= t1; struct Cyc_Absyn_PtrInfo
_temp859; struct Cyc_Absyn_Conref* _temp861; struct Cyc_Absyn_Tqual _temp863;
struct Cyc_Absyn_Conref* _temp865; void* _temp867; void* _temp869; _LL855: if((
unsigned int) _temp853 > 4u?*(( int*) _temp853) == Cyc_Absyn_PointerType: 0){
_LL860: _temp859=(( struct Cyc_Absyn_PointerType_struct*) _temp853)->f1; _LL870:
_temp869=( void*) _temp859.elt_typ; goto _LL868; _LL868: _temp867=( void*)
_temp859.rgn_typ; goto _LL866; _LL866: _temp865= _temp859.nullable; goto _LL864;
_LL864: _temp863= _temp859.tq; goto _LL862; _LL862: _temp861= _temp859.bounds;
goto _LL856;} else{ goto _LL857;} _LL857: goto _LL858; _LL856:{ void* _temp871=
Cyc_Tcutil_compress( _temp869); struct Cyc_Absyn_FnInfo _temp877; struct Cyc_List_List*
_temp879; struct Cyc_List_List* _temp881; struct Cyc_Absyn_VarargInfo* _temp883;
int _temp885; struct Cyc_List_List* _temp887; void* _temp889; struct Cyc_Core_Opt*
_temp891; struct Cyc_List_List* _temp893; _LL873: if(( unsigned int) _temp871 >
4u?*(( int*) _temp871) == Cyc_Absyn_FnType: 0){ _LL878: _temp877=(( struct Cyc_Absyn_FnType_struct*)
_temp871)->f1; _LL894: _temp893= _temp877.tvars; goto _LL892; _LL892: _temp891=
_temp877.effect; goto _LL890; _LL890: _temp889=( void*) _temp877.ret_typ; goto
_LL888; _LL888: _temp887= _temp877.args; goto _LL886; _LL886: _temp885= _temp877.c_varargs;
goto _LL884; _LL884: _temp883= _temp877.cyc_varargs; goto _LL882; _LL882:
_temp881= _temp877.rgn_po; goto _LL880; _LL880: _temp879= _temp877.attributes;
goto _LL874;} else{ goto _LL875;} _LL875: goto _LL876; _LL874: { struct Cyc_List_List*
instantiation= 0; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp893)){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u));}
for( 0; ts != 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, _temp893=((
struct Cyc_List_List*) _check_null( _temp893))->tl)){ void* k=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp893))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); instantiation=({ struct Cyc_List_List* _temp895=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp895->hd=( void*)({
struct _tuple5* _temp896=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp896->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp893))->hd; _temp896->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp896;}); _temp895->tl= instantiation; _temp895;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp900=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp900[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp901; _temp901.tag= Cyc_Absyn_FnType; _temp901.f1=({ struct Cyc_Absyn_FnInfo
_temp902; _temp902.tvars= 0; _temp902.effect= _temp891; _temp902.ret_typ=( void*)
_temp889; _temp902.args= _temp887; _temp902.c_varargs= _temp885; _temp902.cyc_varargs=
_temp883; _temp902.rgn_po= _temp881; _temp902.attributes= _temp879; _temp902;});
_temp901;}); _temp900;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp897=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp897[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp898; _temp898.tag= Cyc_Absyn_PointerType;
_temp898.f1=({ struct Cyc_Absyn_PtrInfo _temp899; _temp899.elt_typ=( void*)
new_fn_typ; _temp899.rgn_typ=( void*) _temp867; _temp899.nullable= _temp865;
_temp899.tq= _temp863; _temp899.bounds= _temp861; _temp899;}); _temp898;});
_temp897;}); return new_typ;}} _LL876: goto _LL872; _LL872:;} goto _LL854;
_LL858: goto _LL854; _LL854:;} return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp903= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_get_arr_size( _temp903, 1u), _temp903.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable(
te, loc, t2, t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp904= Cyc_Absynpp_typ2string(
t2); struct _tagged_arr _temp905= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_get_arr_size( _temp904, 1u), _temp904.curr, _get_arr_size( _temp905, 1u),
_temp905.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
void** _temp906= 0; struct Cyc_Absyn_Tqual _temp907= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp908= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp914; struct Cyc_Absyn_Conref* _temp916;
struct Cyc_Absyn_Tqual _temp918; struct Cyc_Absyn_Conref* _temp920; void*
_temp922; void* _temp924; _LL910: if(( unsigned int) _temp908 > 4u?*(( int*)
_temp908) == Cyc_Absyn_PointerType: 0){ _LL915: _temp914=(( struct Cyc_Absyn_PointerType_struct*)
_temp908)->f1; _LL925: _temp924=( void*) _temp914.elt_typ; goto _LL923; _LL923:
_temp922=( void*) _temp914.rgn_typ; goto _LL921; _LL921: _temp920= _temp914.nullable;
goto _LL919; _LL919: _temp918= _temp914.tq; goto _LL917; _LL917: _temp916=
_temp914.bounds; goto _LL911;} else{ goto _LL912;} _LL912: goto _LL913; _LL911:
_temp906=({ void** _temp926=( void**) GC_malloc( sizeof( void*)); _temp926[ 0]=
_temp924; _temp926;}); _temp907= _temp918; goto _LL909; _LL913: goto _LL909;
_LL909:;} Cyc_Tcexp_tcExpNoInst( te, _temp906, e);{ void* _temp927=( void*) e->r;
struct Cyc_Absyn_Structdecl* _temp935; struct Cyc_List_List* _temp937; struct
Cyc_Core_Opt* _temp939; struct _tuple1* _temp941; struct Cyc_List_List* _temp943;
_LL929: if(*(( int*) _temp927) == Cyc_Absyn_Struct_e){ _LL942: _temp941=((
struct Cyc_Absyn_Struct_e_struct*) _temp927)->f1; goto _LL940; _LL940: _temp939=((
struct Cyc_Absyn_Struct_e_struct*) _temp927)->f2; goto _LL938; _LL938: _temp937=((
struct Cyc_Absyn_Struct_e_struct*) _temp927)->f3; goto _LL936; _LL936: _temp935=((
struct Cyc_Absyn_Struct_e_struct*) _temp927)->f4; goto _LL930;} else{ goto
_LL931;} _LL931: if(*(( int*) _temp927) == Cyc_Absyn_Tuple_e){ _LL944: _temp943=((
struct Cyc_Absyn_Tuple_e_struct*) _temp927)->f1; goto _LL932;} else{ goto _LL933;}
_LL933: goto _LL934; _LL930: goto _LL932; _LL932: Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate",
sizeof( unsigned char), 19u));{ void*(* _temp945)( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp946= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp952; struct Cyc_Absyn_Conref*
_temp954; struct Cyc_Absyn_Tqual _temp956; struct Cyc_Absyn_Conref* _temp958;
void* _temp960; void* _temp962; _LL948: if(( unsigned int) _temp946 > 4u?*(( int*)
_temp946) == Cyc_Absyn_PointerType: 0){ _LL953: _temp952=(( struct Cyc_Absyn_PointerType_struct*)
_temp946)->f1; _LL963: _temp962=( void*) _temp952.elt_typ; goto _LL961; _LL961:
_temp960=( void*) _temp952.rgn_typ; goto _LL959; _LL959: _temp958= _temp952.nullable;
goto _LL957; _LL957: _temp956= _temp952.tq; goto _LL955; _LL955: _temp954=
_temp952.bounds; goto _LL949;} else{ goto _LL950;} _LL950: goto _LL951; _LL949:
if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp958)){
_temp945= Cyc_Absyn_star_typ;} goto _LL947; _LL951: goto _LL947; _LL947:;}
return _temp945(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp907);} _LL934: goto _LL928; _LL928:;}{ void*
_temp966; int _temp968; struct _tuple6 _temp964= Cyc_Tcutil_addressof_props( te,
e); _LL969: _temp968= _temp964.f1; goto _LL967; _LL967: _temp966= _temp964.f2;
goto _LL965; _LL965: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp968){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp966, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); if( ! Cyc_Evexp_okay_szofarg( t)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp970= Cyc_Absynpp_typ2string( t); xprintf("sizeof applied to type %.*s, which has unknown size here",
_get_arr_size( _temp970, 1u), _temp970.curr);}));} return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct _tagged_arr* n){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ int
bad_type= 1;{ void* _temp971= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp979; struct Cyc_List_List* _temp981; struct _tuple1* _temp983; struct Cyc_List_List*
_temp985; _LL973: if(( unsigned int) _temp971 > 4u?*(( int*) _temp971) == Cyc_Absyn_StructType:
0){ _LL984: _temp983=(( struct Cyc_Absyn_StructType_struct*) _temp971)->f1; goto
_LL982; _LL982: _temp981=(( struct Cyc_Absyn_StructType_struct*) _temp971)->f2;
goto _LL980; _LL980: _temp979=(( struct Cyc_Absyn_StructType_struct*) _temp971)->f3;
goto _LL974;} else{ goto _LL975;} _LL975: if(( unsigned int) _temp971 > 4u?*((
int*) _temp971) == Cyc_Absyn_AnonStructType: 0){ _LL986: _temp985=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp971)->f1; goto _LL976;} else{ goto _LL977;} _LL977: goto _LL978; _LL974:
if( _temp979 == 0){ return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("offsetof on unchecked StructType", sizeof( unsigned char), 33u));} if((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp979)))->fields == 0){ goto
_LL972;} goto _LL976; _LL976: bad_type= 0; goto _LL972; _LL978: goto _LL972;
_LL972:;} if( bad_type){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp987= Cyc_Absynpp_typ2string( t); xprintf("%.*s is not a known struct type",
_get_arr_size( _temp987, 1u), _temp987.curr);}));} return Cyc_Absyn_uint_t;}}
static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp988= t; struct Cyc_Absyn_PtrInfo _temp994; struct Cyc_Absyn_Conref*
_temp996; struct Cyc_Absyn_Tqual _temp998; struct Cyc_Absyn_Conref* _temp1000;
void* _temp1002; void* _temp1004; _LL990: if(( unsigned int) _temp988 > 4u?*((
int*) _temp988) == Cyc_Absyn_PointerType: 0){ _LL995: _temp994=(( struct Cyc_Absyn_PointerType_struct*)
_temp988)->f1; _LL1005: _temp1004=( void*) _temp994.elt_typ; goto _LL1003;
_LL1003: _temp1002=( void*) _temp994.rgn_typ; goto _LL1001; _LL1001: _temp1000=
_temp994.nullable; goto _LL999; _LL999: _temp998= _temp994.tq; goto _LL997;
_LL997: _temp996= _temp994.bounds; goto _LL991;} else{ goto _LL992;} _LL992:
goto _LL993; _LL991: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1002); Cyc_Tcutil_check_nonzero_bound(
loc, _temp996); return _temp1004; _LL993: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1006= Cyc_Absynpp_typ2string( t);
xprintf("expecting * or @ type but found %.*s", _get_arr_size( _temp1006, 1u),
_temp1006.curr);})); _LL989:;}} static void* Cyc_Tcexp_tcStructMember( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
outer_e, struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp1007= t; struct Cyc_Absyn_Structdecl**
_temp1023; struct Cyc_Absyn_Structdecl* _temp1025; struct Cyc_List_List*
_temp1026; struct _tuple1* _temp1028; struct Cyc_List_List* _temp1030; struct
Cyc_List_List* _temp1032; struct Cyc_Absyn_Uniondecl** _temp1034; struct Cyc_Absyn_Uniondecl*
_temp1036; struct Cyc_List_List* _temp1037; struct _tuple1* _temp1039; struct
Cyc_Absyn_Exp* _temp1042; struct Cyc_Absyn_Tqual _temp1044; void* _temp1046;
struct Cyc_Absyn_PtrInfo _temp1049; _LL1009: if(( unsigned int) _temp1007 > 4u?*((
int*) _temp1007) == Cyc_Absyn_StructType: 0){ _LL1029: _temp1028=(( struct Cyc_Absyn_StructType_struct*)
_temp1007)->f1; goto _LL1027; _LL1027: _temp1026=(( struct Cyc_Absyn_StructType_struct*)
_temp1007)->f2; goto _LL1024; _LL1024: _temp1023=(( struct Cyc_Absyn_StructType_struct*)
_temp1007)->f3; if( _temp1023 == 0){ goto _LL1011;} else{ _temp1025=* _temp1023;
goto _LL1010;}} else{ goto _LL1011;} _LL1011: if(( unsigned int) _temp1007 > 4u?*((
int*) _temp1007) == Cyc_Absyn_AnonStructType: 0){ _LL1031: _temp1030=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1007)->f1; goto _LL1012;} else{ goto
_LL1013;} _LL1013: if(( unsigned int) _temp1007 > 4u?*(( int*) _temp1007) == Cyc_Absyn_AnonUnionType:
0){ _LL1033: _temp1032=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1007)->f1;
goto _LL1014;} else{ goto _LL1015;} _LL1015: if(( unsigned int) _temp1007 > 4u?*((
int*) _temp1007) == Cyc_Absyn_UnionType: 0){ _LL1040: _temp1039=(( struct Cyc_Absyn_UnionType_struct*)
_temp1007)->f1; goto _LL1038; _LL1038: _temp1037=(( struct Cyc_Absyn_UnionType_struct*)
_temp1007)->f2; goto _LL1035; _LL1035: _temp1034=(( struct Cyc_Absyn_UnionType_struct*)
_temp1007)->f3; if( _temp1034 == 0){ goto _LL1017;} else{ _temp1036=* _temp1034;
goto _LL1016;}} else{ goto _LL1017;} _LL1017: if(( unsigned int) _temp1007 > 4u?*((
int*) _temp1007) == Cyc_Absyn_ArrayType: 0){ _LL1047: _temp1046=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1007)->f1; goto _LL1045; _LL1045:
_temp1044=(( struct Cyc_Absyn_ArrayType_struct*) _temp1007)->f2; goto _LL1043;
_LL1043: _temp1042=(( struct Cyc_Absyn_ArrayType_struct*) _temp1007)->f3; goto
_LL1041;} else{ goto _LL1019;} _LL1041: if( Cyc_String_zstrcmp(* f, _tag_arr("size",
sizeof( unsigned char), 5u)) == 0){ goto _LL1018;} else{ goto _LL1019;} _LL1019:
if(( unsigned int) _temp1007 > 4u?*(( int*) _temp1007) == Cyc_Absyn_PointerType:
0){ _LL1050: _temp1049=(( struct Cyc_Absyn_PointerType_struct*) _temp1007)->f1;
goto _LL1048;} else{ goto _LL1021;} _LL1048: if( Cyc_String_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) == 0){ goto _LL1020;} else{ goto
_LL1021;} _LL1021: goto _LL1022; _LL1010: if( _temp1028 == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)", sizeof(
unsigned char), 40u));}{ struct Cyc_Absyn_Structfield* _temp1051= Cyc_Absyn_lookup_struct_field(
_temp1025, f); if( _temp1051 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1052= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp1028)); struct _tagged_arr _temp1053=* f; xprintf("struct %.*s has no %.*s field",
_get_arr_size( _temp1052, 1u), _temp1052.curr, _get_arr_size( _temp1053, 1u),
_temp1053.curr);}));}{ void* t2;{ struct _RegionHandle _temp1054= _new_region();
struct _RegionHandle* rgn=& _temp1054; _push_region( rgn);{ struct Cyc_List_List*
_temp1055=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp1025->tvs, _temp1026); t2= Cyc_Tcutil_rsubstitute( rgn, _temp1055,(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1051))->type);};
_pop_region( rgn);} return t2;}} _LL1012: { struct Cyc_Absyn_Structfield*
_temp1056= Cyc_Absyn_lookup_field( _temp1030, f); if( _temp1056 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1057=*
f; xprintf("struct has no %.*s field", _get_arr_size( _temp1057, 1u), _temp1057.curr);}));}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1056))->type;}
_LL1014: { struct Cyc_Absyn_Structfield* _temp1058= Cyc_Absyn_lookup_field(
_temp1032, f); if( _temp1058 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1059=* f; xprintf("union has no %.*s field",
_get_arr_size( _temp1059, 1u), _temp1059.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1058))->type;} _LL1016: if( _temp1039 == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u));}{ struct Cyc_Absyn_Structfield* _temp1060= Cyc_Absyn_lookup_union_field(
_temp1036, f); if( _temp1060 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1061= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp1039)); struct _tagged_arr _temp1062=* f; xprintf("union %.*s has no %.*s field",
_get_arr_size( _temp1061, 1u), _temp1061.curr, _get_arr_size( _temp1062, 1u),
_temp1062.curr);}));}{ struct Cyc_List_List* _temp1063=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1036->tvs,
_temp1037); void* _temp1064= Cyc_Tcutil_substitute( _temp1063,( void*)(( struct
Cyc_Absyn_Structfield*) _check_null( _temp1060))->type); return _temp1064;}}
_LL1018:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp1065=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1065[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1066; _temp1066.tag= Cyc_Absyn_Primop_e;
_temp1066.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1066.f2=({ struct Cyc_List_List*
_temp1067=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1067->hd=( void*) e; _temp1067->tl= 0; _temp1067;}); _temp1066;});
_temp1065;}))); return Cyc_Absyn_uint_t; _LL1020:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1068=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1068[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1069; _temp1069.tag= Cyc_Absyn_Primop_e;
_temp1069.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1069.f2=({ struct Cyc_List_List*
_temp1070=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1070->hd=( void*) e; _temp1070->tl= 0; _temp1070;}); _temp1069;});
_temp1068;}))); return Cyc_Absyn_uint_t; _LL1022: if( Cyc_String_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1071= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct, union or array, found %.*s", _get_arr_size(
_temp1071, 1u), _temp1071.curr);}));} else{ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1072= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct or union, found %.*s", _get_arr_size( _temp1072, 1u),
_temp1072.curr);}));} _LL1008:;}} static void* Cyc_Tcexp_tcStructArrow( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e, struct _tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1073= t;
struct Cyc_Absyn_PtrInfo _temp1079; struct Cyc_Absyn_Conref* _temp1081; struct
Cyc_Absyn_Tqual _temp1083; struct Cyc_Absyn_Conref* _temp1085; void* _temp1087;
void* _temp1089; _LL1075: if(( unsigned int) _temp1073 > 4u?*(( int*) _temp1073)
== Cyc_Absyn_PointerType: 0){ _LL1080: _temp1079=(( struct Cyc_Absyn_PointerType_struct*)
_temp1073)->f1; _LL1090: _temp1089=( void*) _temp1079.elt_typ; goto _LL1088;
_LL1088: _temp1087=( void*) _temp1079.rgn_typ; goto _LL1086; _LL1086: _temp1085=
_temp1079.nullable; goto _LL1084; _LL1084: _temp1083= _temp1079.tq; goto _LL1082;
_LL1082: _temp1081= _temp1079.bounds; goto _LL1076;} else{ goto _LL1077;}
_LL1077: goto _LL1078; _LL1076: Cyc_Tcutil_check_nonzero_bound( loc, _temp1081);{
void* _temp1091= Cyc_Tcutil_compress( _temp1089); struct Cyc_Absyn_Structdecl**
_temp1103; struct Cyc_Absyn_Structdecl* _temp1105; struct Cyc_List_List*
_temp1106; struct _tuple1* _temp1108; struct Cyc_Absyn_Uniondecl** _temp1110;
struct Cyc_Absyn_Uniondecl* _temp1112; struct Cyc_List_List* _temp1113; struct
_tuple1* _temp1115; struct Cyc_List_List* _temp1117; struct Cyc_List_List*
_temp1119; _LL1093: if(( unsigned int) _temp1091 > 4u?*(( int*) _temp1091) ==
Cyc_Absyn_StructType: 0){ _LL1109: _temp1108=(( struct Cyc_Absyn_StructType_struct*)
_temp1091)->f1; goto _LL1107; _LL1107: _temp1106=(( struct Cyc_Absyn_StructType_struct*)
_temp1091)->f2; goto _LL1104; _LL1104: _temp1103=(( struct Cyc_Absyn_StructType_struct*)
_temp1091)->f3; if( _temp1103 == 0){ goto _LL1095;} else{ _temp1105=* _temp1103;
goto _LL1094;}} else{ goto _LL1095;} _LL1095: if(( unsigned int) _temp1091 > 4u?*((
int*) _temp1091) == Cyc_Absyn_UnionType: 0){ _LL1116: _temp1115=(( struct Cyc_Absyn_UnionType_struct*)
_temp1091)->f1; goto _LL1114; _LL1114: _temp1113=(( struct Cyc_Absyn_UnionType_struct*)
_temp1091)->f2; goto _LL1111; _LL1111: _temp1110=(( struct Cyc_Absyn_UnionType_struct*)
_temp1091)->f3; if( _temp1110 == 0){ goto _LL1097;} else{ _temp1112=* _temp1110;
goto _LL1096;}} else{ goto _LL1097;} _LL1097: if(( unsigned int) _temp1091 > 4u?*((
int*) _temp1091) == Cyc_Absyn_AnonStructType: 0){ _LL1118: _temp1117=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1091)->f1; goto _LL1098;} else{ goto
_LL1099;} _LL1099: if(( unsigned int) _temp1091 > 4u?*(( int*) _temp1091) == Cyc_Absyn_AnonUnionType:
0){ _LL1120: _temp1119=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1091)->f1;
goto _LL1100;} else{ goto _LL1101;} _LL1101: goto _LL1102; _LL1094: { struct Cyc_Absyn_Structfield*
_temp1121= Cyc_Absyn_lookup_struct_field( _temp1105, f); if( _temp1121 == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1122= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1108));
struct _tagged_arr _temp1123=* f; xprintf("struct %.*s has no %.*s field",
_get_arr_size( _temp1122, 1u), _temp1122.curr, _get_arr_size( _temp1123, 1u),
_temp1123.curr);}));}{ void* t3;{ struct _RegionHandle _temp1124= _new_region();
struct _RegionHandle* rgn=& _temp1124; _push_region( rgn);{ struct Cyc_List_List*
_temp1125=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp1105->tvs, _temp1106); t3= Cyc_Tcutil_rsubstitute( rgn, _temp1125,(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1121))->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1087); return
t3;}} _LL1096: { struct Cyc_Absyn_Structfield* _temp1126= Cyc_Absyn_lookup_union_field(
_temp1112, f); if( _temp1126 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1127= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp1115)); struct _tagged_arr _temp1128=* f; xprintf("union %.*s has no %.*s field",
_get_arr_size( _temp1127, 1u), _temp1127.curr, _get_arr_size( _temp1128, 1u),
_temp1128.curr);}));}{ struct Cyc_List_List* _temp1129=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1112->tvs,
_temp1113); void* _temp1130= Cyc_Tcutil_substitute( _temp1129,( void*)(( struct
Cyc_Absyn_Structfield*) _check_null( _temp1126))->type); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1087); return _temp1130;}} _LL1098: { struct Cyc_Absyn_Structfield*
_temp1131= Cyc_Absyn_lookup_field( _temp1117, f); if( _temp1131 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp1132=*
f; xprintf("struct has no %.*s field", _get_arr_size( _temp1132, 1u), _temp1132.curr);}));}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1131))->type;}
_LL1100: { struct Cyc_Absyn_Structfield* _temp1133= Cyc_Absyn_lookup_field(
_temp1119, f); if( _temp1133 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1134=* f; xprintf("union has no %.*s field",
_get_arr_size( _temp1134, 1u), _temp1134.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1133))->type;} _LL1102: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1135= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct pointer, found %.*s", _get_arr_size( _temp1135, 1u),
_temp1135.curr);})); _LL1092:;} _LL1078: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1136= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct pointer, found %.*s", _get_arr_size( _temp1136, 1u),
_temp1136.curr);})); _LL1074:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1137; _push_handler(& _temp1137);{ int _temp1139= 0; if(
setjmp( _temp1137.handler)){ _temp1139= 1;} if( ! _temp1139){{ void* _temp1140=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1140;}; _pop_handler();} else{ void* _temp1138=(
void*) _exn_thrown; void* _temp1142= _temp1138; _LL1144: if( _temp1142 == Cyc_List_Nth){
goto _LL1145;} else{ goto _LL1146;} _LL1146: goto _LL1147; _LL1145: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr) xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1147:(
void) _throw( _temp1142); _LL1143:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return
Cyc_Tcexp_expr_err( te, e2->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1148= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_get_arr_size( _temp1148, 1u), _temp1148.curr);}));}{ void* _temp1149= t1;
struct Cyc_Absyn_PtrInfo _temp1157; struct Cyc_Absyn_Conref* _temp1159; struct
Cyc_Absyn_Tqual _temp1161; struct Cyc_Absyn_Conref* _temp1163; void* _temp1165;
void* _temp1167; struct Cyc_List_List* _temp1169; _LL1151: if(( unsigned int)
_temp1149 > 4u?*(( int*) _temp1149) == Cyc_Absyn_PointerType: 0){ _LL1158:
_temp1157=(( struct Cyc_Absyn_PointerType_struct*) _temp1149)->f1; _LL1168:
_temp1167=( void*) _temp1157.elt_typ; goto _LL1166; _LL1166: _temp1165=( void*)
_temp1157.rgn_typ; goto _LL1164; _LL1164: _temp1163= _temp1157.nullable; goto
_LL1162; _LL1162: _temp1161= _temp1157.tq; goto _LL1160; _LL1160: _temp1159=
_temp1157.bounds; goto _LL1152;} else{ goto _LL1153;} _LL1153: if(( unsigned int)
_temp1149 > 4u?*(( int*) _temp1149) == Cyc_Absyn_TupleType: 0){ _LL1170:
_temp1169=(( struct Cyc_Absyn_TupleType_struct*) _temp1149)->f1; goto _LL1154;}
else{ goto _LL1155;} _LL1155: goto _LL1156; _LL1152: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp1159);} else{{ void* _temp1171=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1159))->v; void*
_temp1177; struct Cyc_Absyn_Exp* _temp1179; struct Cyc_Absyn_Exp _temp1181;
struct Cyc_Position_Segment* _temp1182; void* _temp1184; void* _temp1186; int
_temp1188; void* _temp1190; struct Cyc_Core_Opt* _temp1192; _LL1173: if((
unsigned int) _temp1171 > 1u?*(( int*) _temp1171) == Cyc_Absyn_Eq_constr: 0){
_LL1178: _temp1177=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1171)->f1;
if(( unsigned int) _temp1177 > 1u?*(( int*) _temp1177) == Cyc_Absyn_Upper_b: 0){
_LL1180: _temp1179=(( struct Cyc_Absyn_Upper_b_struct*) _temp1177)->f1;
_temp1181=* _temp1179; _LL1193: _temp1192= _temp1181.topt; goto _LL1185; _LL1185:
_temp1184=( void*) _temp1181.r; if(*(( int*) _temp1184) == Cyc_Absyn_Const_e){
_LL1187: _temp1186=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1184)->f1;
if(( unsigned int) _temp1186 > 1u?*(( int*) _temp1186) == Cyc_Absyn_Int_c: 0){
_LL1191: _temp1190=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1186)->f1;
if( _temp1190 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1189;} else{ goto _LL1175;}
_LL1189: _temp1188=(( struct Cyc_Absyn_Int_c_struct*) _temp1186)->f2; if(
_temp1188 == 1){ goto _LL1183;} else{ goto _LL1175;}} else{ goto _LL1175;}}
else{ goto _LL1175;} _LL1183: _temp1182= _temp1181.loc; goto _LL1174;} else{
goto _LL1175;}} else{ goto _LL1175;} _LL1175: goto _LL1176; _LL1174: Cyc_Tcutil_warn(
e1->loc, _tag_arr("subscript applied to pointer to one object", sizeof(
unsigned char), 43u)); goto _LL1172; _LL1176: goto _LL1172; _LL1172:;} Cyc_Tcutil_check_nonzero_bound(
loc, _temp1159);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1165); return
_temp1167; _LL1154: return Cyc_Tcexp_ithTupleType( te, loc, _temp1169, e2);
_LL1156: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1194= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_get_arr_size( _temp1194, 1u), _temp1194.curr);})); _LL1150:;}}} static void*
Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1195= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_List_List* _temp1201; _LL1197: if((
unsigned int) _temp1195 > 4u?*(( int*) _temp1195) == Cyc_Absyn_TupleType: 0){
_LL1202: _temp1201=(( struct Cyc_Absyn_TupleType_struct*) _temp1195)->f1; goto
_LL1198;} else{ goto _LL1199;} _LL1199: goto _LL1200; _LL1198: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1201) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc, _tag_arr("tuple expression has the wrong number of fields",
sizeof( unsigned char), 48u)); goto _LL1196;} for( 0; es != 0;( es=(( struct Cyc_List_List*)
_check_null( es))->tl, _temp1201=(( struct Cyc_List_List*) _check_null(
_temp1201))->tl)){ void* _temp1203=(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1201))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)&
_temp1203,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
fields=({ struct Cyc_List_List* _temp1204=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1204->hd=( void*)({ struct _tuple8*
_temp1205=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1205->f1=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1201))->hd)).f1;
_temp1205->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1205;});
_temp1204->tl= fields; _temp1204;});} done= 1; goto _LL1196; _LL1200: goto
_LL1196; _LL1196:;} if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); fields=({ struct Cyc_List_List*
_temp1206=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1206->hd=( void*)({ struct _tuple8* _temp1207=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1207->f1= Cyc_Absyn_empty_tqual(); _temp1207->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1207;}); _temp1206->tl=
fields; _temp1206;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1208=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1208[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1209; _temp1209.tag= Cyc_Absyn_TupleType;
_temp1209.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1209;}); _temp1208;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("tcCompoundLit", sizeof( unsigned char), 14u));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1229=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1229->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1229;})); struct Cyc_Absyn_Const_e_struct*
_temp1210=({ struct Cyc_Absyn_Const_e_struct* _temp1225=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1225[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1226; _temp1226.tag= Cyc_Absyn_Const_e; _temp1226.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1227=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1227[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1228; _temp1228.tag= Cyc_Absyn_Int_c; _temp1228.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1228.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1228;}); _temp1227;})); _temp1226;}); _temp1225;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1210, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1211=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1211->v=( void*) Cyc_Absyn_uint_t; _temp1211;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1223=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1223[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1224; _temp1224.tag= Cyc_Absyn_ArrayType;
_temp1224.f1=( void*) res; _temp1224.f2= Cyc_Absyn_empty_tqual(); _temp1224.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1224;}); _temp1223;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1212= Cyc_Absynpp_typ2string( res);
xprintf("elements of array do not all have the same type (%.*s)", _get_arr_size(
_temp1212, 1u), _temp1212.curr);}));}{ int offset= 0; for( 0; des != 0;( offset
++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){ struct Cyc_List_List*
ds=(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1;
if( ds != 0){ void* _temp1213=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct _tagged_arr* _temp1219; struct Cyc_Absyn_Exp* _temp1221; _LL1215: if(*((
int*) _temp1213) == Cyc_Absyn_FieldName){ _LL1220: _temp1219=(( struct Cyc_Absyn_FieldName_struct*)
_temp1213)->f1; goto _LL1216;} else{ goto _LL1217;} _LL1217: if(*(( int*)
_temp1213) == Cyc_Absyn_ArrayElement){ _LL1222: _temp1221=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1213)->f1; goto _LL1218;} else{ goto _LL1214;} _LL1216: Cyc_Tcutil_terr(
loc, _tag_arr("only array index designators are supported", sizeof(
unsigned char), 43u)); goto _LL1214; _LL1218: Cyc_Tcexp_tcExpInitializer( te, 0,
_temp1221);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1221); if( i !=
offset){ Cyc_Tcutil_terr( _temp1221->loc,( struct _tagged_arr) xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1214;} _LL1214:;}}} return res_t2;}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1230= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v); xprintf("expecting unsigned int, found %.*s",
_get_arr_size( _temp1230, 1u), _temp1230.curr);}));} if( !( vd->tq).q_const){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u));} if( te->le != 0){ te= Cyc_Tcenv_new_block( loc,
te); te= Cyc_Tcenv_add_local_var( loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp(
te, bound)){ Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant",
sizeof( unsigned char), 22u));} if( ! Cyc_Tcutil_is_const_exp( te, body)){ Cyc_Tcutil_terr(
bound->loc, _tag_arr("body is not constant", sizeof( unsigned char), 21u));}}{
struct Cyc_Absyn_PtrInfo pinfo; void** _temp1231= 0; struct Cyc_Absyn_Tqual*
_temp1232= 0; if( topt != 0){ void* _temp1233= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1241; struct Cyc_Absyn_Exp*
_temp1243; struct Cyc_Absyn_Tqual _temp1245; struct Cyc_Absyn_Tqual* _temp1247;
void* _temp1248; void** _temp1250; _LL1235: if(( unsigned int) _temp1233 > 4u?*((
int*) _temp1233) == Cyc_Absyn_PointerType: 0){ _LL1242: _temp1241=(( struct Cyc_Absyn_PointerType_struct*)
_temp1233)->f1; goto _LL1236;} else{ goto _LL1237;} _LL1237: if(( unsigned int)
_temp1233 > 4u?*(( int*) _temp1233) == Cyc_Absyn_ArrayType: 0){ _LL1249:
_temp1248=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1233)->f1;
_temp1250=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1233)->f1); goto
_LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_ArrayType_struct*) _temp1233)->f2;
_temp1247=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1233)->f2; goto _LL1244;
_LL1244: _temp1243=(( struct Cyc_Absyn_ArrayType_struct*) _temp1233)->f3; goto
_LL1238;} else{ goto _LL1239;} _LL1239: goto _LL1240; _LL1236: pinfo= _temp1241;
_temp1231=( void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1232=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1234; _LL1238: _temp1231=( void**) _temp1250; _temp1232=(
struct Cyc_Absyn_Tqual*) _temp1247; goto _LL1234; _LL1240: goto _LL1234; _LL1234:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1231, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1251=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1251[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1252; _temp1252.tag= Cyc_Absyn_ArrayType;
_temp1252.f1=( void*) t; _temp1252.f2= _temp1232 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1232)); _temp1252.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1252;}); _temp1251;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1253; _push_handler(&
_temp1253);{ int _temp1255= 0; if( setjmp( _temp1253.handler)){ _temp1255= 1;}
if( ! _temp1255){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1254=( void*) _exn_thrown; void* _temp1257= _temp1254; _LL1259:
if( _temp1257 == Cyc_Dict_Absent){ goto _LL1260;} else{ goto _LL1261;} _LL1261:
goto _LL1262; _LL1260: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1263= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_get_arr_size( _temp1263, 1u), _temp1263.curr);})); return topt != 0?*(( void**)
_check_null( topt)):( void*) Cyc_Absyn_VoidType; _LL1262:( void) _throw(
_temp1257); _LL1258:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!= 0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1264= _new_region(); struct _RegionHandle* rgn=&
_temp1264; _push_region( rgn);{ struct _tuple4 _temp1265=({ struct _tuple4
_temp1287; _temp1287.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1287.f2= rgn;
_temp1287;}); struct Cyc_List_List* _temp1266=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1265, sd->tvs); struct Cyc_List_List* _temp1267=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1266); struct Cyc_Absyn_StructType_struct*
_temp1268=({ struct Cyc_Absyn_StructType_struct* _temp1284=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1284[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1285; _temp1285.tag= Cyc_Absyn_StructType;
_temp1285.f1=( struct _tuple1*)* tn; _temp1285.f2= _temp1267; _temp1285.f3=({
struct Cyc_Absyn_Structdecl** _temp1286=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1286[ 0]= sd; _temp1286;});
_temp1285;}); _temp1284;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) _temp1268,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1269=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1269->v=( void*)
_temp1267; _temp1269;}); if( sd->fields == 0){ Cyc_Tcutil_terr( loc, _tag_arr("can't build abstract struct",
sizeof( unsigned char), 28u));{ void* _temp1270=( void*) _temp1268;
_npop_handler( 0u); return _temp1270;}}{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1273; struct Cyc_Absyn_Exp* _temp1274; struct Cyc_Absyn_Structfield*
_temp1276; struct _tuple9* _temp1271=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1273=* _temp1271; _LL1277: _temp1276= _temp1273.f1;
goto _LL1275; _LL1275: _temp1274= _temp1273.f2; goto _LL1272; _LL1272: { void*
_temp1278= Cyc_Tcutil_rsubstitute( rgn, _temp1266,( void*) _temp1276->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1278, _temp1274); if( ! Cyc_Tcutil_coerce_arg( te, _temp1274,
_temp1278)){ Cyc_Tcutil_terr( _temp1274->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1279=* _temp1276->name; struct _tagged_arr _temp1280= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_arr _temp1281= Cyc_Absynpp_typ2string( _temp1278); struct
_tagged_arr _temp1282= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1274->topt))->v); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_get_arr_size( _temp1279, 1u), _temp1279.curr, _get_arr_size( _temp1280, 1u),
_temp1280.curr, _get_arr_size( _temp1281, 1u), _temp1281.curr, _get_arr_size(
_temp1282, 1u), _temp1282.curr);}));}}}{ void* _temp1283=( void*) _temp1268;
_npop_handler( 0u); return _temp1283;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1288= _new_region(); struct
_RegionHandle* rgn=& _temp1288; _push_region( rgn);{ void* _temp1289= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1295; _LL1291: if(( unsigned int) _temp1289 > 4u?*((
int*) _temp1289) == Cyc_Absyn_AnonStructType: 0){ _LL1296: _temp1295=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1289)->f1; goto _LL1292;} else{ goto
_LL1293;} _LL1293: goto _LL1294; _LL1292: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1295); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1299; struct Cyc_Absyn_Exp*
_temp1300; struct Cyc_Absyn_Structfield* _temp1302; struct _tuple9* _temp1297=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1299=*
_temp1297; _LL1303: _temp1302= _temp1299.f1; goto _LL1301; _LL1301: _temp1300=
_temp1299.f2; goto _LL1298; _LL1298: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1302->type)), _temp1300); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1300,( void*) _temp1302->type)){ Cyc_Tcutil_terr( _temp1300->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1304=* _temp1302->name; struct
_tagged_arr _temp1305= Cyc_Absynpp_typ2string(( void*) _temp1302->type); struct
_tagged_arr _temp1306= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1300->topt))->v); xprintf("field %.*s of struct expects type %.*s != %.*s",
_get_arr_size( _temp1304, 1u), _temp1304.curr, _get_arr_size( _temp1305, 1u),
_temp1305.curr, _get_arr_size( _temp1306, 1u), _temp1306.curr);}));}} goto
_LL1290;} _LL1294:(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u)); goto _LL1290; _LL1290:;}; _pop_region( rgn);}
return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Core_Opt**
all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl*
tud, struct Cyc_Absyn_Tunionfield* tuf){ struct _RegionHandle _temp1307=
_new_region(); struct _RegionHandle* rgn=& _temp1307; _push_region( rgn);{
struct _tuple4 _temp1308=({ struct _tuple4 _temp1354; _temp1354.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1354.f2= rgn; _temp1354;}); struct Cyc_List_List* _temp1309=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1308, tud->tvs); struct Cyc_List_List*
_temp1310=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(*
f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp1308, tuf->tvs);
struct Cyc_List_List* _temp1311=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp1309); struct Cyc_List_List* _temp1312=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1310); struct Cyc_List_List* _temp1313=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1309, _temp1310);* all_ref=({
struct Cyc_Core_Opt* _temp1314=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1314->v=( void*) _temp1311; _temp1314;});* exist_ref=({
struct Cyc_Core_Opt* _temp1315=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1315->v=( void*) _temp1312; _temp1315;});{ void* res=( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1349=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1349[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1350; _temp1350.tag= Cyc_Absyn_TunionFieldType;
_temp1350.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1351; _temp1351.field_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp1352=( struct
Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct));
_temp1352[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct _temp1353; _temp1353.tag=
Cyc_Absyn_KnownTunionfield; _temp1353.f1= tud; _temp1353.f2= tuf; _temp1353;});
_temp1352;})); _temp1351.targs= _temp1311; _temp1351;}); _temp1350;}); _temp1349;});
if( topt != 0){ void* _temp1316= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_TunionFieldInfo _temp1324; struct Cyc_Absyn_TunionInfo
_temp1326; void* _temp1328; struct Cyc_List_List* _temp1330; void* _temp1332;
_LL1318: if(( unsigned int) _temp1316 > 4u?*(( int*) _temp1316) == Cyc_Absyn_TunionFieldType:
0){ _LL1325: _temp1324=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1316)->f1;
goto _LL1319;} else{ goto _LL1320;} _LL1320: if(( unsigned int) _temp1316 > 4u?*((
int*) _temp1316) == Cyc_Absyn_TunionType: 0){ _LL1327: _temp1326=(( struct Cyc_Absyn_TunionType_struct*)
_temp1316)->f1; _LL1333: _temp1332=( void*) _temp1326.tunion_info; goto _LL1331;
_LL1331: _temp1330= _temp1326.targs; goto _LL1329; _LL1329: _temp1328=( void*)
_temp1326.rgn; goto _LL1321;} else{ goto _LL1322;} _LL1322: goto _LL1323;
_LL1319: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res); goto _LL1317;
_LL1321:{ struct Cyc_List_List* a= _temp1311; for( 0; a != 0? _temp1330 != 0: 0;(
a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1330=(( struct Cyc_List_List*)
_check_null( _temp1330))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( a))->hd,( void*)(( struct Cyc_List_List*) _check_null( _temp1330))->hd);}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1334=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1334->v=(
void*) res; _temp1334;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1335=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1335[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1336; _temp1336.tag=
Cyc_Absyn_TunionType; _temp1336.f1=({ struct Cyc_Absyn_TunionInfo _temp1337;
_temp1337.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1338=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1338[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1339; _temp1339.tag=
Cyc_Absyn_KnownTunion; _temp1339.f1= tud; _temp1339;}); _temp1338;})); _temp1337.targs=
_temp1311; _temp1337.rgn=( void*) _temp1328; _temp1337;}); _temp1336;});
_temp1335;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1340= res;
_npop_handler( 0u); return _temp1340;}} goto _LL1317; _LL1323: goto _LL1317;
_LL1317:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0;(
es=(( struct Cyc_List_List*) _check_null( es))->tl, ts=(( struct Cyc_List_List*)
_check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; void* t= Cyc_Tcutil_rsubstitute(
rgn, _temp1313,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( ts))->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1341= Cyc_Absynpp_qvar2string( tuf->name); struct _tagged_arr _temp1342=
Cyc_Absynpp_typ2string( t); struct _tagged_arr _temp1343= e->topt == 0? _tag_arr("?",
sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_get_arr_size( _temp1341, 1u), _temp1341.curr, _get_arr_size( _temp1342, 1u),
_temp1342.curr, _get_arr_size( _temp1343, 1u), _temp1343.curr);}));}} if( es !=
0){ void* _temp1345= Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1344= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_get_arr_size( _temp1344, 1u), _temp1344.curr);})); _npop_handler( 0u); return
_temp1345;} if( ts != 0){ void* _temp1347= Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1346= Cyc_Absynpp_qvar2string( tuf->name);
xprintf("too few arguments for tunion constructor %.*s", _get_arr_size(
_temp1346, 1u), _temp1346.curr);})); _npop_handler( 0u); return _temp1347;}{
void* _temp1348= res; _npop_handler( 0u); return _temp1348;}}}}; _pop_region(
rgn);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( ropt != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1364=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1364[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1365; _temp1365.tag= Cyc_Absyn_RgnHandleType; _temp1365.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1366=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1366->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1366;})); _temp1365;}); _temp1364;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1355= Cyc_Tcutil_compress( handle_type); void*
_temp1361; _LL1357: if(( unsigned int) _temp1355 > 4u?*(( int*) _temp1355) ==
Cyc_Absyn_RgnHandleType: 0){ _LL1362: _temp1361=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1355)->f1; goto _LL1358;} else{ goto _LL1359;} _LL1359: goto _LL1360;
_LL1358: rgn= _temp1361; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1356; _LL1360: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null( ropt))->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1363= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _get_arr_size(
_temp1363, 1u), _temp1363.curr);})); goto _LL1356; _LL1356:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ void*
_temp1367= t; struct Cyc_Absyn_TunionFieldInfo _temp1373; struct Cyc_List_List*
_temp1375; void* _temp1377; struct Cyc_Absyn_Tunionfield* _temp1379; struct Cyc_Absyn_Tuniondecl*
_temp1381; _LL1369: if(( unsigned int) _temp1367 > 4u?*(( int*) _temp1367) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1374: _temp1373=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1367)->f1; _LL1378: _temp1377=( void*) _temp1373.field_info; if(*(( int*)
_temp1377) == Cyc_Absyn_KnownTunionfield){ _LL1382: _temp1381=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1377)->f1; goto _LL1380; _LL1380: _temp1379=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1377)->f2; goto _LL1376;} else{ goto _LL1371;} _LL1376: _temp1375=
_temp1373.targs; goto _LL1370;} else{ goto _LL1371;} _LL1371: goto _LL1372;
_LL1370: if( _temp1379->tvs != 0){ Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u));} goto _LL1368; _LL1372: goto _LL1368; _LL1368:;}{
void*(* _temp1383)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp1384= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1390; struct Cyc_Absyn_Conref* _temp1392;
struct Cyc_Absyn_Tqual _temp1394; struct Cyc_Absyn_Conref* _temp1396; void*
_temp1398; void* _temp1400; _LL1386: if(( unsigned int) _temp1384 > 4u?*(( int*)
_temp1384) == Cyc_Absyn_PointerType: 0){ _LL1391: _temp1390=(( struct Cyc_Absyn_PointerType_struct*)
_temp1384)->f1; _LL1401: _temp1400=( void*) _temp1390.elt_typ; goto _LL1399;
_LL1399: _temp1398=( void*) _temp1390.rgn_typ; goto _LL1397; _LL1397: _temp1396=
_temp1390.nullable; goto _LL1395; _LL1395: _temp1394= _temp1390.tq; goto _LL1393;
_LL1393: _temp1392= _temp1390.bounds; goto _LL1387;} else{ goto _LL1388;}
_LL1388: goto _LL1389; _LL1387: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1396)){ _temp1383= Cyc_Absyn_star_typ;} goto _LL1385; _LL1389: goto _LL1385;
_LL1385:;} return _temp1383( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1402=( void*) s->r; struct Cyc_Absyn_Exp* _temp1412;
struct Cyc_Absyn_Stmt* _temp1414; struct Cyc_Absyn_Stmt* _temp1416; struct Cyc_Absyn_Stmt*
_temp1418; struct Cyc_Absyn_Decl* _temp1420; _LL1404: if(( unsigned int)
_temp1402 > 1u?*(( int*) _temp1402) == Cyc_Absyn_Exp_s: 0){ _LL1413: _temp1412=((
struct Cyc_Absyn_Exp_s_struct*) _temp1402)->f1; goto _LL1405;} else{ goto
_LL1406;} _LL1406: if(( unsigned int) _temp1402 > 1u?*(( int*) _temp1402) == Cyc_Absyn_Seq_s:
0){ _LL1417: _temp1416=(( struct Cyc_Absyn_Seq_s_struct*) _temp1402)->f1; goto
_LL1415; _LL1415: _temp1414=(( struct Cyc_Absyn_Seq_s_struct*) _temp1402)->f2;
goto _LL1407;} else{ goto _LL1408;} _LL1408: if(( unsigned int) _temp1402 > 1u?*((
int*) _temp1402) == Cyc_Absyn_Decl_s: 0){ _LL1421: _temp1420=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1402)->f1; goto _LL1419; _LL1419: _temp1418=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1402)->f2; goto _LL1409;} else{ goto _LL1410;} _LL1410: goto _LL1411;
_LL1405: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1412->topt))->v;
_LL1407: s= _temp1414; continue; _LL1409: s= _temp1418; continue; _LL1411:
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u)); _LL1403:;}} static void* Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u));}
static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle
!= 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1431=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1431[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1432; _temp1432.tag= Cyc_Absyn_RgnHandleType; _temp1432.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1433=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1433->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1433;})); _temp1432;}); _temp1431;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle)); void* _temp1422= Cyc_Tcutil_compress( handle_type);
void* _temp1428; _LL1424: if(( unsigned int) _temp1422 > 4u?*(( int*) _temp1422)
== Cyc_Absyn_RgnHandleType: 0){ _LL1429: _temp1428=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1422)->f1; goto _LL1425;} else{ goto _LL1426;} _LL1426: goto _LL1427;
_LL1425: rgn= _temp1428; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1423; _LL1427: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null(
rgn_handle))->loc,( struct _tagged_arr)({ struct _tagged_arr _temp1430= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _get_arr_size(
_temp1430, 1u), _temp1430.curr);})); goto _LL1423; _LL1423:;}{ void* _temp1434=(
void*) e1->r; struct Cyc_Absyn_Exp* _temp1446; struct Cyc_Absyn_Exp* _temp1448;
struct Cyc_Absyn_Vardecl* _temp1450; struct Cyc_List_List* _temp1452; struct Cyc_Core_Opt*
_temp1454; struct Cyc_List_List* _temp1456; void* _temp1458; struct _tagged_arr
_temp1460; _LL1436: if(*(( int*) _temp1434) == Cyc_Absyn_Comprehension_e){
_LL1451: _temp1450=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1434)->f1;
goto _LL1449; _LL1449: _temp1448=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1434)->f2; goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1434)->f3; goto _LL1437;} else{ goto _LL1438;} _LL1438: if(*(( int*)
_temp1434) == Cyc_Absyn_UnresolvedMem_e){ _LL1455: _temp1454=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1434)->f1; goto _LL1453; _LL1453: _temp1452=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1434)->f2; goto _LL1439;} else{ goto _LL1440;} _LL1440: if(*(( int*)
_temp1434) == Cyc_Absyn_Array_e){ _LL1457: _temp1456=(( struct Cyc_Absyn_Array_e_struct*)
_temp1434)->f1; goto _LL1441;} else{ goto _LL1442;} _LL1442: if(*(( int*)
_temp1434) == Cyc_Absyn_Const_e){ _LL1459: _temp1458=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1434)->f1; if(( unsigned int) _temp1458 > 1u?*(( int*) _temp1458) == Cyc_Absyn_String_c:
0){ _LL1461: _temp1460=(( struct Cyc_Absyn_String_c_struct*) _temp1458)->f1;
goto _LL1443;} else{ goto _LL1444;}} else{ goto _LL1444;} _LL1444: goto _LL1445;
_LL1437: { void* _temp1462= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1463= Cyc_Tcutil_compress( _temp1462); struct Cyc_Absyn_Exp* _temp1469;
struct Cyc_Absyn_Tqual _temp1471; void* _temp1473; _LL1465: if(( unsigned int)
_temp1463 > 4u?*(( int*) _temp1463) == Cyc_Absyn_ArrayType: 0){ _LL1474:
_temp1473=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1463)->f1; goto
_LL1472; _LL1472: _temp1471=(( struct Cyc_Absyn_ArrayType_struct*) _temp1463)->f2;
goto _LL1470; _LL1470: _temp1469=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1463)->f3; goto _LL1466;} else{ goto _LL1467;} _LL1467: goto _LL1468;
_LL1466: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1448)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1479=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1479[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1480; _temp1480.tag= Cyc_Absyn_Upper_b; _temp1480.f1=
_temp1448; _temp1480;}); _temp1479;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1476=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1476[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1477; _temp1477.tag= Cyc_Absyn_PointerType;
_temp1477.f1=({ struct Cyc_Absyn_PtrInfo _temp1478; _temp1478.elt_typ=( void*)
_temp1473; _temp1478.rgn_typ=( void*) rgn; _temp1478.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1478.tq= _temp1471; _temp1478.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1478;});
_temp1477;}); _temp1476;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1475=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1475->v=(
void*) res_typ; _temp1475;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}
_LL1468: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u)); _LL1464:;} _LL1439:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1481=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1481[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1482; _temp1482.tag= Cyc_Absyn_Array_e; _temp1482.f1=
_temp1452; _temp1482;}); _temp1481;}))); _temp1456= _temp1452; goto _LL1441;
_LL1441: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1483= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1489; struct Cyc_Absyn_Conref*
_temp1491; struct Cyc_Absyn_Tqual _temp1493; struct Cyc_Absyn_Conref* _temp1495;
void* _temp1497; void* _temp1499; void** _temp1501; _LL1485: if(( unsigned int)
_temp1483 > 4u?*(( int*) _temp1483) == Cyc_Absyn_PointerType: 0){ _LL1490:
_temp1489=(( struct Cyc_Absyn_PointerType_struct*) _temp1483)->f1; _LL1500:
_temp1499=( void*) _temp1489.elt_typ; _temp1501=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1483)->f1).elt_typ; goto _LL1498; _LL1498: _temp1497=( void*) _temp1489.rgn_typ;
goto _LL1496; _LL1496: _temp1495= _temp1489.nullable; goto _LL1494; _LL1494:
_temp1493= _temp1489.tq; goto _LL1492; _LL1492: _temp1491= _temp1489.bounds;
goto _LL1486;} else{ goto _LL1487;} _LL1487: goto _LL1488; _LL1486: elt_typ_opt=(
void**) _temp1501; goto _LL1484; _LL1488: goto _LL1484; _LL1484:;}{ void*
_temp1502= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1503= Cyc_Tcutil_compress( _temp1502); struct Cyc_Absyn_Exp* _temp1509;
struct Cyc_Absyn_Tqual _temp1511; void* _temp1513; _LL1505: if(( unsigned int)
_temp1503 > 4u?*(( int*) _temp1503) == Cyc_Absyn_ArrayType: 0){ _LL1514:
_temp1513=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1503)->f1; goto
_LL1512; _LL1512: _temp1511=(( struct Cyc_Absyn_ArrayType_struct*) _temp1503)->f2;
goto _LL1510; _LL1510: _temp1509=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1503)->f3; goto _LL1506;} else{ goto _LL1507;} _LL1507: goto _LL1508;
_LL1506: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1515=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1515[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1516; _temp1516.tag=
Cyc_Absyn_PointerType; _temp1516.f1=({ struct Cyc_Absyn_PtrInfo _temp1517;
_temp1517.elt_typ=( void*) _temp1513; _temp1517.rgn_typ=( void*) rgn; _temp1517.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1517.tq=
_temp1511; _temp1517.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1518=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1518[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1519; _temp1519.tag= Cyc_Absyn_Upper_b; _temp1519.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1509); _temp1519;}); _temp1518;}));
_temp1517;}); _temp1516;}); _temp1515;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1520=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1520->v=( void*) res_typ; _temp1520;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1504; _LL1508: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("tcExpNoPromote on Array_e returned non-array type", sizeof(
unsigned char), 50u)); _LL1504:;} return res_typ;}} _LL1443: { void* _temp1521=
Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b);
void* _temp1522= Cyc_Tcexp_tcExp( te,( void**)& _temp1521, e1); return Cyc_Absyn_atb_typ(
_temp1522, rgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1523=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1523[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1524; _temp1524.tag= Cyc_Absyn_Upper_b;
_temp1524.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1524;}); _temp1523;}));} _LL1445: {
void** topt2= 0; if( topt != 0){ void* _temp1525= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1533; struct Cyc_Absyn_Conref*
_temp1535; struct Cyc_Absyn_Tqual _temp1537; struct Cyc_Absyn_Conref* _temp1539;
void* _temp1541; void* _temp1543; void** _temp1545; struct Cyc_Absyn_TunionInfo
_temp1546; _LL1527: if(( unsigned int) _temp1525 > 4u?*(( int*) _temp1525) ==
Cyc_Absyn_PointerType: 0){ _LL1534: _temp1533=(( struct Cyc_Absyn_PointerType_struct*)
_temp1525)->f1; _LL1544: _temp1543=( void*) _temp1533.elt_typ; _temp1545=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1525)->f1).elt_typ; goto _LL1542;
_LL1542: _temp1541=( void*) _temp1533.rgn_typ; goto _LL1540; _LL1540: _temp1539=
_temp1533.nullable; goto _LL1538; _LL1538: _temp1537= _temp1533.tq; goto _LL1536;
_LL1536: _temp1535= _temp1533.bounds; goto _LL1528;} else{ goto _LL1529;}
_LL1529: if(( unsigned int) _temp1525 > 4u?*(( int*) _temp1525) == Cyc_Absyn_TunionType:
0){ _LL1547: _temp1546=(( struct Cyc_Absyn_TunionType_struct*) _temp1525)->f1;
goto _LL1530;} else{ goto _LL1531;} _LL1531: goto _LL1532; _LL1528: topt2=( void**)
_temp1545; goto _LL1526; _LL1530: topt2=({ void** _temp1548=( void**) GC_malloc(
sizeof( void*)); _temp1548[ 0]=*(( void**) _check_null( topt)); _temp1548;});
goto _LL1526; _LL1532: goto _LL1526; _LL1526:;}{ void* _temp1549= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1551=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1551[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1552; _temp1552.tag=
Cyc_Absyn_PointerType; _temp1552.f1=({ struct Cyc_Absyn_PtrInfo _temp1553;
_temp1553.elt_typ=( void*) _temp1549; _temp1553.rgn_typ=( void*) rgn; _temp1553.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1553.tq= Cyc_Absyn_empty_tqual();
_temp1553.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1554=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1554[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1555; _temp1555.tag= Cyc_Absyn_Upper_b; _temp1555.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1555;}); _temp1554;})); _temp1553;}); _temp1552;});
_temp1551;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**) _check_null(
topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1550=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1550->v=( void*) res_typ;
_temp1550;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt)));
res_typ=*(( void**) _check_null( topt));}} return res_typ;}} _LL1435:;}} void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t= Cyc_Tcutil_compress( t);{
void* _temp1556= t; struct Cyc_Absyn_Exp* _temp1562; struct Cyc_Absyn_Tqual
_temp1564; void* _temp1566; _LL1558: if(( unsigned int) _temp1556 > 4u?*(( int*)
_temp1556) == Cyc_Absyn_ArrayType: 0){ _LL1567: _temp1566=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1556)->f1; goto _LL1565; _LL1565: _temp1564=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1556)->f2; goto _LL1563; _LL1563: _temp1562=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1556)->f3; goto _LL1559;} else{ goto _LL1560;} _LL1560: goto _LL1561;
_LL1559: { void* _temp1570; int _temp1572; struct _tuple6 _temp1568= Cyc_Tcutil_addressof_props(
te, e); _LL1573: _temp1572= _temp1568.f1; goto _LL1571; _LL1571: _temp1570=
_temp1568.f2; goto _LL1569; _LL1569: { void* _temp1574= _temp1562 == 0?( void*)
Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1575=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1575[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1576; _temp1576.tag= Cyc_Absyn_Upper_b;
_temp1576.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1562); _temp1576;});
_temp1575;}); t= Cyc_Absyn_atb_typ( _temp1566, _temp1570, _temp1564, _temp1574);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1561: return t; _LL1557:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1577=( void*) e->r; struct Cyc_List_List* _temp1587; struct
Cyc_Absyn_Exp* _temp1589; struct Cyc_Absyn_Exp* _temp1591; struct Cyc_Absyn_Vardecl*
_temp1593; void* _temp1595; struct _tagged_arr _temp1597; _LL1579: if(*(( int*)
_temp1577) == Cyc_Absyn_Array_e){ _LL1588: _temp1587=(( struct Cyc_Absyn_Array_e_struct*)
_temp1577)->f1; goto _LL1580;} else{ goto _LL1581;} _LL1581: if(*(( int*)
_temp1577) == Cyc_Absyn_Comprehension_e){ _LL1594: _temp1593=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1577)->f1; goto _LL1592; _LL1592: _temp1591=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1577)->f2; goto _LL1590; _LL1590: _temp1589=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1577)->f3; goto _LL1582;} else{ goto _LL1583;} _LL1583: if(*(( int*)
_temp1577) == Cyc_Absyn_Const_e){ _LL1596: _temp1595=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1577)->f1; if(( unsigned int) _temp1595 > 1u?*(( int*) _temp1595) == Cyc_Absyn_String_c:
0){ _LL1598: _temp1597=(( struct Cyc_Absyn_String_c_struct*) _temp1595)->f1;
goto _LL1584;} else{ goto _LL1585;}} else{ goto _LL1585;} _LL1585: goto _LL1586;
_LL1580: return t; _LL1582: return t; _LL1584: return t; _LL1586: t= Cyc_Tcutil_compress(
t);{ void* _temp1599= t; struct Cyc_Absyn_Exp* _temp1605; struct Cyc_Absyn_Tqual
_temp1607; void* _temp1609; _LL1601: if(( unsigned int) _temp1599 > 4u?*(( int*)
_temp1599) == Cyc_Absyn_ArrayType: 0){ _LL1610: _temp1609=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1599)->f1; goto _LL1608; _LL1608: _temp1607=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1599)->f2; goto _LL1606; _LL1606: _temp1605=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1599)->f3; goto _LL1602;} else{ goto _LL1603;} _LL1603: goto _LL1604;
_LL1602: { void* _temp1613; int _temp1615; struct _tuple6 _temp1611= Cyc_Tcutil_addressof_props(
te, e); _LL1616: _temp1615= _temp1611.f1; goto _LL1614; _LL1614: _temp1613=
_temp1611.f2; goto _LL1612; _LL1612: { void* b= _temp1605 == 0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1617=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1617[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1618; _temp1618.tag= Cyc_Absyn_Upper_b; _temp1618.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1605); _temp1618;}); _temp1617;}); t=
Cyc_Absyn_atb_typ( _temp1609, _temp1613, _temp1607, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1604: return t; _LL1600:;} _LL1578:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1619=( void*) e->r; struct Cyc_Absyn_Exp* _temp1625; _LL1621: if(*(( int*)
_temp1619) == Cyc_Absyn_NoInstantiate_e){ _LL1626: _temp1625=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1619)->f1; goto _LL1622;} else{ goto _LL1623;} _LL1623: goto _LL1624;
_LL1622: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1625);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1625->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1625->topt))->v))); e->topt=
_temp1625->topt; goto _LL1620; _LL1624: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1627= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1633; struct Cyc_Absyn_Conref*
_temp1635; struct Cyc_Absyn_Tqual _temp1637; struct Cyc_Absyn_Conref* _temp1639;
void* _temp1641; void* _temp1643; _LL1629: if(( unsigned int) _temp1627 > 4u?*((
int*) _temp1627) == Cyc_Absyn_PointerType: 0){ _LL1634: _temp1633=(( struct Cyc_Absyn_PointerType_struct*)
_temp1627)->f1; _LL1644: _temp1643=( void*) _temp1633.elt_typ; goto _LL1642;
_LL1642: _temp1641=( void*) _temp1633.rgn_typ; goto _LL1640; _LL1640: _temp1639=
_temp1633.nullable; goto _LL1638; _LL1638: _temp1637= _temp1633.tq; goto _LL1636;
_LL1636: _temp1635= _temp1633.bounds; goto _LL1630;} else{ goto _LL1631;}
_LL1631: goto _LL1632; _LL1630:{ void* _temp1645= Cyc_Tcutil_compress( _temp1643);
struct Cyc_Absyn_FnInfo _temp1651; struct Cyc_List_List* _temp1653; struct Cyc_List_List*
_temp1655; struct Cyc_Absyn_VarargInfo* _temp1657; int _temp1659; struct Cyc_List_List*
_temp1661; void* _temp1663; struct Cyc_Core_Opt* _temp1665; struct Cyc_List_List*
_temp1667; _LL1647: if(( unsigned int) _temp1645 > 4u?*(( int*) _temp1645) ==
Cyc_Absyn_FnType: 0){ _LL1652: _temp1651=(( struct Cyc_Absyn_FnType_struct*)
_temp1645)->f1; _LL1668: _temp1667= _temp1651.tvars; goto _LL1666; _LL1666:
_temp1665= _temp1651.effect; goto _LL1664; _LL1664: _temp1663=( void*) _temp1651.ret_typ;
goto _LL1662; _LL1662: _temp1661= _temp1651.args; goto _LL1660; _LL1660:
_temp1659= _temp1651.c_varargs; goto _LL1658; _LL1658: _temp1657= _temp1651.cyc_varargs;
goto _LL1656; _LL1656: _temp1655= _temp1651.rgn_po; goto _LL1654; _LL1654:
_temp1653= _temp1651.attributes; goto _LL1648;} else{ goto _LL1649;} _LL1649:
goto _LL1650; _LL1648: if( _temp1667 != 0){ struct _RegionHandle _temp1669=
_new_region(); struct _RegionHandle* rgn=& _temp1669; _push_region( rgn);{
struct _tuple4 _temp1670=({ struct _tuple4 _temp1680; _temp1680.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1680.f2= rgn; _temp1680;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1670, _temp1667); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1677=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1677[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1678; _temp1678.tag= Cyc_Absyn_FnType; _temp1678.f1=({ struct Cyc_Absyn_FnInfo
_temp1679; _temp1679.tvars= 0; _temp1679.effect= _temp1665; _temp1679.ret_typ=(
void*) _temp1663; _temp1679.args= _temp1661; _temp1679.c_varargs= _temp1659;
_temp1679.cyc_varargs= _temp1657; _temp1679.rgn_po= _temp1655; _temp1679.attributes=
_temp1653; _temp1679;}); _temp1678;}); _temp1677;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1674=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1674[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1675; _temp1675.tag= Cyc_Absyn_PointerType;
_temp1675.f1=({ struct Cyc_Absyn_PtrInfo _temp1676; _temp1676.elt_typ=( void*)
ftyp; _temp1676.rgn_typ=( void*) _temp1641; _temp1676.nullable= _temp1639;
_temp1676.tq= _temp1637; _temp1676.bounds= _temp1635; _temp1676;}); _temp1675;});
_temp1674;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1671=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1671[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1672; _temp1672.tag= Cyc_Absyn_Instantiate_e;
_temp1672.f1= inner; _temp1672.f2= ts; _temp1672;}); _temp1671;}))); e->topt=({
struct Cyc_Core_Opt* _temp1673=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1673->v=( void*) new_typ; _temp1673;});}; _pop_region( rgn);}
goto _LL1646; _LL1650: goto _LL1646; _LL1646:;} goto _LL1628; _LL1632: goto
_LL1628; _LL1628:;} goto _LL1620; _LL1620:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1681=( void*) e->r; struct Cyc_Absyn_Exp* _temp1755; struct
_tuple1* _temp1757; struct Cyc_List_List* _temp1759; struct Cyc_Absyn_Exp*
_temp1761; struct Cyc_List_List* _temp1763; struct Cyc_Core_Opt* _temp1765; void*
_temp1767; void* _temp1769; struct _tuple1* _temp1771; struct Cyc_List_List*
_temp1773; void* _temp1775; void* _temp1777; struct Cyc_Absyn_Exp* _temp1779;
struct Cyc_Absyn_Exp* _temp1781; struct Cyc_Core_Opt* _temp1783; struct Cyc_Absyn_Exp*
_temp1785; struct Cyc_Absyn_Exp* _temp1787; struct Cyc_Absyn_Exp* _temp1789;
struct Cyc_Absyn_Exp* _temp1791; struct Cyc_Absyn_Exp* _temp1793; struct Cyc_Absyn_Exp*
_temp1795; struct Cyc_Absyn_VarargCallInfo* _temp1797; struct Cyc_Absyn_VarargCallInfo**
_temp1799; struct Cyc_List_List* _temp1800; struct Cyc_Absyn_Exp* _temp1802;
struct Cyc_Absyn_Exp* _temp1804; struct Cyc_List_List* _temp1806; struct Cyc_Absyn_Exp*
_temp1808; struct Cyc_Absyn_Exp* _temp1810; void* _temp1812; struct Cyc_Absyn_Exp*
_temp1814; struct Cyc_Absyn_Exp* _temp1816; struct Cyc_Absyn_Exp* _temp1818;
struct Cyc_Absyn_Exp* _temp1820; void* _temp1822; struct _tagged_arr* _temp1824;
void* _temp1826; struct Cyc_Absyn_Exp* _temp1828; struct _tagged_arr* _temp1830;
struct Cyc_Absyn_Exp* _temp1832; struct _tagged_arr* _temp1834; struct Cyc_Absyn_Exp*
_temp1836; struct Cyc_Absyn_Exp* _temp1838; struct Cyc_Absyn_Exp* _temp1840;
struct Cyc_List_List* _temp1842; struct Cyc_List_List* _temp1844; struct _tuple2*
_temp1846; struct Cyc_List_List* _temp1848; struct Cyc_Absyn_Stmt* _temp1850;
struct Cyc_Absyn_Fndecl* _temp1852; struct Cyc_Absyn_Exp* _temp1854; struct Cyc_Absyn_Exp*
_temp1856; struct Cyc_Absyn_Exp* _temp1858; struct Cyc_Absyn_Vardecl* _temp1860;
struct Cyc_Absyn_Structdecl* _temp1862; struct Cyc_Absyn_Structdecl** _temp1864;
struct Cyc_List_List* _temp1865; struct Cyc_Core_Opt* _temp1867; struct Cyc_Core_Opt**
_temp1869; struct _tuple1* _temp1870; struct _tuple1** _temp1872; struct Cyc_List_List*
_temp1873; void* _temp1875; struct Cyc_Absyn_Tunionfield* _temp1877; struct Cyc_Absyn_Tuniondecl*
_temp1879; struct Cyc_List_List* _temp1881; struct Cyc_Core_Opt* _temp1883;
struct Cyc_Core_Opt** _temp1885; struct Cyc_Core_Opt* _temp1886; struct Cyc_Core_Opt**
_temp1888; struct Cyc_Absyn_Enumfield* _temp1889; struct Cyc_Absyn_Enumdecl*
_temp1891; struct _tuple1* _temp1893; struct _tuple1** _temp1895; void*
_temp1896; struct Cyc_Absyn_Exp* _temp1898; _LL1683: if(*(( int*) _temp1681) ==
Cyc_Absyn_NoInstantiate_e){ _LL1756: _temp1755=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1681)->f1; goto _LL1684;} else{ goto _LL1685;} _LL1685: if(*(( int*)
_temp1681) == Cyc_Absyn_UnknownId_e){ _LL1758: _temp1757=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1681)->f1; goto _LL1686;} else{ goto _LL1687;} _LL1687: if(*(( int*)
_temp1681) == Cyc_Absyn_UnknownCall_e){ _LL1762: _temp1761=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1681)->f1; goto _LL1760; _LL1760: _temp1759=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1681)->f2; goto _LL1688;} else{ goto _LL1689;} _LL1689: if(*(( int*)
_temp1681) == Cyc_Absyn_UnresolvedMem_e){ _LL1766: _temp1765=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1681)->f1; goto _LL1764; _LL1764: _temp1763=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1681)->f2; goto _LL1690;} else{ goto _LL1691;} _LL1691: if(*(( int*)
_temp1681) == Cyc_Absyn_Const_e){ _LL1768: _temp1767=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1681)->f1; goto _LL1692;} else{ goto _LL1693;} _LL1693: if(*(( int*)
_temp1681) == Cyc_Absyn_Var_e){ _LL1772: _temp1771=(( struct Cyc_Absyn_Var_e_struct*)
_temp1681)->f1; goto _LL1770; _LL1770: _temp1769=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1681)->f2; goto _LL1694;} else{ goto _LL1695;} _LL1695: if(*(( int*)
_temp1681) == Cyc_Absyn_Primop_e){ _LL1776: _temp1775=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1681)->f1; goto _LL1774; _LL1774: _temp1773=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1681)->f2; goto _LL1696;} else{ goto _LL1697;} _LL1697: if(*(( int*)
_temp1681) == Cyc_Absyn_Increment_e){ _LL1780: _temp1779=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1681)->f1; goto _LL1778; _LL1778: _temp1777=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1681)->f2; goto _LL1698;} else{ goto _LL1699;} _LL1699: if(*(( int*)
_temp1681) == Cyc_Absyn_AssignOp_e){ _LL1786: _temp1785=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1681)->f1; goto _LL1784; _LL1784: _temp1783=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1681)->f2; goto _LL1782; _LL1782: _temp1781=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1681)->f3; goto _LL1700;} else{ goto _LL1701;} _LL1701: if(*(( int*)
_temp1681) == Cyc_Absyn_Conditional_e){ _LL1792: _temp1791=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1681)->f1; goto _LL1790; _LL1790: _temp1789=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1681)->f2; goto _LL1788; _LL1788: _temp1787=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1681)->f3; goto _LL1702;} else{ goto _LL1703;} _LL1703: if(*(( int*)
_temp1681) == Cyc_Absyn_SeqExp_e){ _LL1796: _temp1795=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1681)->f1; goto _LL1794; _LL1794: _temp1793=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1681)->f2; goto _LL1704;} else{ goto _LL1705;} _LL1705: if(*(( int*)
_temp1681) == Cyc_Absyn_FnCall_e){ _LL1803: _temp1802=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1681)->f1; goto _LL1801; _LL1801: _temp1800=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1681)->f2; goto _LL1798; _LL1798: _temp1797=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1681)->f3; _temp1799=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1681)->f3;
goto _LL1706;} else{ goto _LL1707;} _LL1707: if(*(( int*) _temp1681) == Cyc_Absyn_Throw_e){
_LL1805: _temp1804=(( struct Cyc_Absyn_Throw_e_struct*) _temp1681)->f1; goto
_LL1708;} else{ goto _LL1709;} _LL1709: if(*(( int*) _temp1681) == Cyc_Absyn_Instantiate_e){
_LL1809: _temp1808=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1681)->f1;
goto _LL1807; _LL1807: _temp1806=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1681)->f2; goto _LL1710;} else{ goto _LL1711;} _LL1711: if(*(( int*)
_temp1681) == Cyc_Absyn_Cast_e){ _LL1813: _temp1812=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1681)->f1; goto _LL1811; _LL1811: _temp1810=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1681)->f2; goto _LL1712;} else{ goto _LL1713;} _LL1713: if(*(( int*)
_temp1681) == Cyc_Absyn_Address_e){ _LL1815: _temp1814=(( struct Cyc_Absyn_Address_e_struct*)
_temp1681)->f1; goto _LL1714;} else{ goto _LL1715;} _LL1715: if(*(( int*)
_temp1681) == Cyc_Absyn_New_e){ _LL1819: _temp1818=(( struct Cyc_Absyn_New_e_struct*)
_temp1681)->f1; goto _LL1817; _LL1817: _temp1816=(( struct Cyc_Absyn_New_e_struct*)
_temp1681)->f2; goto _LL1716;} else{ goto _LL1717;} _LL1717: if(*(( int*)
_temp1681) == Cyc_Absyn_Sizeofexp_e){ _LL1821: _temp1820=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1681)->f1; goto _LL1718;} else{ goto _LL1719;} _LL1719: if(*(( int*)
_temp1681) == Cyc_Absyn_Sizeoftyp_e){ _LL1823: _temp1822=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1681)->f1; goto _LL1720;} else{ goto _LL1721;} _LL1721: if(*(( int*)
_temp1681) == Cyc_Absyn_Offsetof_e){ _LL1827: _temp1826=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1681)->f1; goto _LL1825; _LL1825: _temp1824=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1681)->f2; goto _LL1722;} else{ goto _LL1723;} _LL1723: if(*(( int*)
_temp1681) == Cyc_Absyn_Deref_e){ _LL1829: _temp1828=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1681)->f1; goto _LL1724;} else{ goto _LL1725;} _LL1725: if(*(( int*)
_temp1681) == Cyc_Absyn_StructMember_e){ _LL1833: _temp1832=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1681)->f1; goto _LL1831; _LL1831: _temp1830=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1681)->f2; goto _LL1726;} else{ goto _LL1727;} _LL1727: if(*(( int*)
_temp1681) == Cyc_Absyn_StructArrow_e){ _LL1837: _temp1836=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1681)->f1; goto _LL1835; _LL1835: _temp1834=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1681)->f2; goto _LL1728;} else{ goto _LL1729;} _LL1729: if(*(( int*)
_temp1681) == Cyc_Absyn_Subscript_e){ _LL1841: _temp1840=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1681)->f1; goto _LL1839; _LL1839: _temp1838=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1681)->f2; goto _LL1730;} else{ goto _LL1731;} _LL1731: if(*(( int*)
_temp1681) == Cyc_Absyn_Tuple_e){ _LL1843: _temp1842=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1681)->f1; goto _LL1732;} else{ goto _LL1733;} _LL1733: if(*(( int*)
_temp1681) == Cyc_Absyn_CompoundLit_e){ _LL1847: _temp1846=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1681)->f1; goto _LL1845; _LL1845: _temp1844=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1681)->f2; goto _LL1734;} else{ goto _LL1735;} _LL1735: if(*(( int*)
_temp1681) == Cyc_Absyn_Array_e){ _LL1849: _temp1848=(( struct Cyc_Absyn_Array_e_struct*)
_temp1681)->f1; goto _LL1736;} else{ goto _LL1737;} _LL1737: if(*(( int*)
_temp1681) == Cyc_Absyn_StmtExp_e){ _LL1851: _temp1850=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1681)->f1; goto _LL1738;} else{ goto _LL1739;} _LL1739: if(*(( int*)
_temp1681) == Cyc_Absyn_Codegen_e){ _LL1853: _temp1852=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1681)->f1; goto _LL1740;} else{ goto _LL1741;} _LL1741: if(*(( int*)
_temp1681) == Cyc_Absyn_Fill_e){ _LL1855: _temp1854=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1681)->f1; goto _LL1742;} else{ goto _LL1743;} _LL1743: if(*(( int*)
_temp1681) == Cyc_Absyn_Comprehension_e){ _LL1861: _temp1860=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1681)->f1; goto _LL1859; _LL1859: _temp1858=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1681)->f2; goto _LL1857; _LL1857: _temp1856=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1681)->f3; goto _LL1744;} else{ goto _LL1745;} _LL1745: if(*(( int*)
_temp1681) == Cyc_Absyn_Struct_e){ _LL1871: _temp1870=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1681)->f1; _temp1872=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1681)->f1;
goto _LL1868; _LL1868: _temp1867=(( struct Cyc_Absyn_Struct_e_struct*) _temp1681)->f2;
_temp1869=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1681)->f2; goto _LL1866;
_LL1866: _temp1865=(( struct Cyc_Absyn_Struct_e_struct*) _temp1681)->f3; goto
_LL1863; _LL1863: _temp1862=(( struct Cyc_Absyn_Struct_e_struct*) _temp1681)->f4;
_temp1864=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1681)->f4; goto _LL1746;}
else{ goto _LL1747;} _LL1747: if(*(( int*) _temp1681) == Cyc_Absyn_AnonStruct_e){
_LL1876: _temp1875=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1681)->f1;
goto _LL1874; _LL1874: _temp1873=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1681)->f2; goto _LL1748;} else{ goto _LL1749;} _LL1749: if(*(( int*)
_temp1681) == Cyc_Absyn_Tunion_e){ _LL1887: _temp1886=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1681)->f1; _temp1888=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1681)->f1;
goto _LL1884; _LL1884: _temp1883=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1681)->f2;
_temp1885=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1681)->f2; goto _LL1882;
_LL1882: _temp1881=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1681)->f3; goto
_LL1880; _LL1880: _temp1879=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1681)->f4;
goto _LL1878; _LL1878: _temp1877=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1681)->f5;
goto _LL1750;} else{ goto _LL1751;} _LL1751: if(*(( int*) _temp1681) == Cyc_Absyn_Enum_e){
_LL1894: _temp1893=(( struct Cyc_Absyn_Enum_e_struct*) _temp1681)->f1; _temp1895=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1681)->f1; goto _LL1892; _LL1892:
_temp1891=(( struct Cyc_Absyn_Enum_e_struct*) _temp1681)->f2; goto _LL1890;
_LL1890: _temp1889=(( struct Cyc_Absyn_Enum_e_struct*) _temp1681)->f3; goto
_LL1752;} else{ goto _LL1753;} _LL1753: if(*(( int*) _temp1681) == Cyc_Absyn_Malloc_e){
_LL1899: _temp1898=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1681)->f1; goto
_LL1897; _LL1897: _temp1896=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1681)->f2; goto _LL1754;} else{ goto _LL1682;} _LL1684: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1755); return; _LL1686: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1757); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1688: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1761, _temp1759); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1690: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1763); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1692: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1767, e);
goto _LL1682; _LL1694: t= Cyc_Tcexp_tcVar( te, loc, _temp1771, _temp1769); goto
_LL1682; _LL1696: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1775, _temp1773);
goto _LL1682; _LL1698: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1779,
_temp1777); goto _LL1682; _LL1700: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1785, _temp1783, _temp1781); goto _LL1682; _LL1702: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1791, _temp1789, _temp1787); goto _LL1682; _LL1704: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1795, _temp1793); goto _LL1682; _LL1706: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1802, _temp1800, _temp1799); goto _LL1682; _LL1708: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1804); goto _LL1682; _LL1710: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1808, _temp1806); goto _LL1682; _LL1712: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1812, _temp1810); goto _LL1682; _LL1714: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1814); goto _LL1682; _LL1716: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1818, e, _temp1816); goto _LL1682; _LL1718: { void* _temp1900= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1820); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1900); goto
_LL1682;} _LL1720: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1822); goto
_LL1682; _LL1722: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1826, _temp1824);
goto _LL1682; _LL1724: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1828); goto
_LL1682; _LL1726: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1832,
_temp1830); goto _LL1682; _LL1728: t= Cyc_Tcexp_tcStructArrow( te, loc, topt,
_temp1836, _temp1834); goto _LL1682; _LL1730: t= Cyc_Tcexp_tcSubscript( te, loc,
topt, _temp1840, _temp1838); goto _LL1682; _LL1732: t= Cyc_Tcexp_tcTuple( te,
loc, topt, _temp1842); goto _LL1682; _LL1734: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1846, _temp1844); goto _LL1682; _LL1736: { void** elt_topt= 0;
if( topt != 0){ void* _temp1901= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_Exp* _temp1907; struct Cyc_Absyn_Tqual _temp1909; void*
_temp1911; void** _temp1913; _LL1903: if(( unsigned int) _temp1901 > 4u?*(( int*)
_temp1901) == Cyc_Absyn_ArrayType: 0){ _LL1912: _temp1911=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1901)->f1; _temp1913=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1901)->f1); goto _LL1910; _LL1910: _temp1909=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1901)->f2; goto _LL1908; _LL1908: _temp1907=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1901)->f3; goto _LL1904;} else{ goto _LL1905;} _LL1905: goto _LL1906;
_LL1904: elt_topt=( void**) _temp1913; goto _LL1902; _LL1906: goto _LL1902;
_LL1902:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1848); goto _LL1682;}
_LL1738: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1850); goto _LL1682;
_LL1740: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1852); goto _LL1682;
_LL1742: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1854); goto _LL1682; _LL1744:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1860, _temp1858, _temp1856);
goto _LL1682; _LL1746: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1872,
_temp1869, _temp1865, _temp1864); goto _LL1682; _LL1748: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1875, _temp1873); goto _LL1682; _LL1750: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1888, _temp1885, _temp1881, _temp1879, _temp1877); goto
_LL1682; _LL1752:* _temp1895=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1889))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1914=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1914[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1915; _temp1915.tag= Cyc_Absyn_EnumType;
_temp1915.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1891))->name;
_temp1915.f2= _temp1891; _temp1915;}); _temp1914;}); goto _LL1682; _LL1754: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1898, _temp1896); goto _LL1682; _LL1682:;}
e->topt=({ struct Cyc_Core_Opt* _temp1916=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1916->v=( void*) t; _temp1916;});}