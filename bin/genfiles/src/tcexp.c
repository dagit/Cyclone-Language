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
_temp20; struct Cyc_Absyn_Tuniondecl* _temp22; _LL6: if(*(( int*) _temp4) == Cyc_Tcenv_VarRes){
_LL15: _temp14=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp4)->f1; goto
_LL7;} else{ goto _LL8;} _LL8: if(*(( int*) _temp4) == Cyc_Tcenv_EnumRes){ _LL19:
_temp18=(( struct Cyc_Tcenv_EnumRes_struct*) _temp4)->f1; goto _LL17; _LL17:
_temp16=(( struct Cyc_Tcenv_EnumRes_struct*) _temp4)->f2; goto _LL9;} else{ goto
_LL10;} _LL10: if(*(( int*) _temp4) == Cyc_Tcenv_TunionRes){ _LL23: _temp22=((
struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f1; goto _LL21; _LL21: _temp20=((
struct Cyc_Tcenv_TunionRes_struct*) _temp4)->f2; goto _LL11;} else{ goto _LL12;}
_LL12: if(*(( int*) _temp4) == Cyc_Tcenv_StructRes){ goto _LL13;} else{ goto
_LL5;} _LL7:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp24=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp24[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp25; _temp25.tag= Cyc_Absyn_Var_e;
_temp25.f1= q; _temp25.f2=( void*) _temp14; _temp25;}); _temp24;}))); goto _LL5;
_LL9:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp26=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp26[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp27; _temp27.tag= Cyc_Absyn_Enum_e;
_temp27.f1= q; _temp27.f2=( struct Cyc_Absyn_Enumdecl*) _temp18; _temp27.f3=(
struct Cyc_Absyn_Enumfield*) _temp16; _temp27;}); _temp26;}))); goto _LL5; _LL11:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp28=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp28[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp29; _temp29.tag= Cyc_Absyn_Tunion_e;
_temp29.f1= 0; _temp29.f2= 0; _temp29.f3= 0; _temp29.f4= _temp22; _temp29.f5=
_temp20; _temp29;}); _temp28;}))); goto _LL5; _LL13: Cyc_Tcutil_terr( e->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp30= Cyc_Absynpp_qvar2string( q);
xprintf("bad occurrence of struct name %.*s", _get_arr_size( _temp30, 1u),
_temp30.curr);}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp31=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp31[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp32; _temp32.tag= Cyc_Absyn_Var_e;
_temp32.f1= q; _temp32.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp32;});
_temp31;}))); goto _LL5; _LL5:;}; _pop_handler();} else{ void* _temp2=( void*)
_exn_thrown; void* _temp34= _temp2; _LL36: if( _temp34 == Cyc_Dict_Absent){ goto
_LL37;} else{ goto _LL38;} _LL38: goto _LL39; _LL37:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp40=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp40[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp41; _temp41.tag= Cyc_Absyn_Var_e; _temp41.f1= q; _temp41.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp41;}); _temp40;}))); goto _LL35; _LL39:( void)
_throw( _temp34); _LL35:;}}} struct _tuple7{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static struct _tuple7* Cyc_Tcexp_make_struct_arg( struct
Cyc_Absyn_Exp* e){ return({ struct _tuple7* _temp42=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp42->f1= 0; _temp42->f2= e; _temp42;});} static
void Cyc_Tcexp_resolve_unknown_fn( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* es){ void* _temp43=( void*)
e1->r; struct _tuple1* _temp49; _LL45: if(*(( int*) _temp43) == Cyc_Absyn_UnknownId_e){
_LL50: _temp49=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp43)->f1; goto _LL46;}
else{ goto _LL47;} _LL47: goto _LL48; _LL46: { struct _handler_cons _temp51;
_push_handler(& _temp51);{ int _temp53= 0; if( setjmp( _temp51.handler)){
_temp53= 1;} if( ! _temp53){{ void* _temp54= Cyc_Tcenv_lookup_ordinary( te, e1->loc,
_temp49); void* _temp64; struct Cyc_Absyn_Tunionfield* _temp66; struct Cyc_Absyn_Tuniondecl*
_temp68; struct Cyc_Absyn_Structdecl* _temp70; _LL56: if(*(( int*) _temp54) ==
Cyc_Tcenv_VarRes){ _LL65: _temp64=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp54)->f1; goto _LL57;} else{ goto _LL58;} _LL58: if(*(( int*) _temp54) ==
Cyc_Tcenv_TunionRes){ _LL69: _temp68=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp54)->f1; goto _LL67; _LL67: _temp66=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp54)->f2; goto _LL59;} else{ goto _LL60;} _LL60: if(*(( int*) _temp54) ==
Cyc_Tcenv_StructRes){ _LL71: _temp70=(( struct Cyc_Tcenv_StructRes_struct*)
_temp54)->f1; goto _LL61;} else{ goto _LL62;} _LL62: if(*(( int*) _temp54) ==
Cyc_Tcenv_EnumRes){ goto _LL63;} else{ goto _LL55;} _LL57:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp72=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp72[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp73; _temp73.tag= Cyc_Absyn_FnCall_e; _temp73.f1= e1; _temp73.f2= es;
_temp73.f3= 0; _temp73;}); _temp72;}))); _npop_handler( 0u); return; _LL59: if(
_temp66->typs == 0){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp74= Cyc_Absynpp_qvar2string( _temp66->name); xprintf("%.*s is a constant, not a function",
_get_arr_size( _temp74, 1u), _temp74.curr);}));}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp75=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp75[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp76; _temp76.tag= Cyc_Absyn_Tunion_e; _temp76.f1= 0; _temp76.f2= 0; _temp76.f3=
es; _temp76.f4= _temp68; _temp76.f5= _temp66; _temp76;}); _temp75;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL61: { struct Cyc_List_List*
_temp77=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp70->name != 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp78=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp78[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp79; _temp79.tag= Cyc_Absyn_Struct_e;
_temp79.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp70->name))->v;
_temp79.f2= 0; _temp79.f3= _temp77; _temp79.f4=( struct Cyc_Absyn_Structdecl*)
_temp70; _temp79;}); _temp78;})));} else{ Cyc_Tcutil_terr( e->loc, _tag_arr("missing struct name",
sizeof( unsigned char), 20u));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp80=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp80[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp81; _temp81.tag= Cyc_Absyn_Struct_e;
_temp81.f1= _temp49; _temp81.f2= 0; _temp81.f3= _temp77; _temp81.f4=( struct Cyc_Absyn_Structdecl*)
_temp70; _temp81;}); _temp80;})));} _npop_handler( 0u); return;} _LL63: Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp82= Cyc_Absynpp_qvar2string(
_temp49); xprintf("%.*s is an enum constructor, not a function", _get_arr_size(
_temp82, 1u), _temp82.curr);})); _npop_handler( 0u); return; _LL55:;};
_pop_handler();} else{ void* _temp52=( void*) _exn_thrown; void* _temp84=
_temp52; _LL86: if( _temp84 == Cyc_Dict_Absent){ goto _LL87;} else{ goto _LL88;}
_LL88: goto _LL89; _LL87: Cyc_Tcutil_terr( e1->loc,( struct _tagged_arr)({
struct _tagged_arr _temp90= Cyc_Absynpp_qvar2string( _temp49); xprintf("unbound identifier %.*s",
_get_arr_size( _temp90, 1u), _temp90.curr);}));( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp91=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp91[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp92; _temp92.tag= Cyc_Absyn_FnCall_e; _temp92.f1= e1; _temp92.f2= es;
_temp92.f3= 0; _temp92;}); _temp91;}))); return; _LL89:( void) _throw( _temp84);
_LL85:;}}} _LL48:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp93=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp93[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp94; _temp94.tag= Cyc_Absyn_FnCall_e;
_temp94.f1= e1; _temp94.f2= es; _temp94.f3= 0; _temp94;}); _temp93;}))); return;
_LL44:;} static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
Cyc_List_List* des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Array_e_struct* _temp95=( struct Cyc_Absyn_Array_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Array_e_struct)); _temp95[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp96; _temp96.tag= Cyc_Absyn_Array_e; _temp96.f1= des; _temp96;}); _temp95;})));
return;}{ void* t=*(( void**) _check_null( topt)); void* _temp97= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tqual _temp107; void* _temp109; struct Cyc_Absyn_Structdecl**
_temp111; struct Cyc_List_List* _temp113; struct _tuple1* _temp115; _LL99: if((
unsigned int) _temp97 > 4u?*(( int*) _temp97) == Cyc_Absyn_ArrayType: 0){ _LL110:
_temp109=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp97)->f1; goto
_LL108; _LL108: _temp107=(( struct Cyc_Absyn_ArrayType_struct*) _temp97)->f2;
goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int) _temp97 > 4u?*((
int*) _temp97) == Cyc_Absyn_StructType: 0){ _LL116: _temp115=(( struct Cyc_Absyn_StructType_struct*)
_temp97)->f1; goto _LL114; _LL114: _temp113=(( struct Cyc_Absyn_StructType_struct*)
_temp97)->f2; goto _LL112; _LL112: _temp111=(( struct Cyc_Absyn_StructType_struct*)
_temp97)->f3; goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int)
_temp97 > 4u?*(( int*) _temp97) == Cyc_Absyn_AnonStructType: 0){ goto _LL104;}
else{ goto _LL105;} _LL105: goto _LL106; _LL100:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp117=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp117[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp118; _temp118.tag= Cyc_Absyn_Array_e; _temp118.f1= des; _temp118;});
_temp117;}))); goto _LL98; _LL102: if( _temp111 == 0? 1: _temp115 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("Compiler bug: struct type not properly set", sizeof(
unsigned char), 43u));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp119=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp119[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp120; _temp120.tag= Cyc_Absyn_Struct_e;
_temp120.f1=( struct _tuple1*) _check_null( _temp115); _temp120.f2= 0; _temp120.f3=
des; _temp120.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp111)); _temp120;}); _temp119;}))); goto _LL98; _LL104:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp121=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp121[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp122; _temp122.tag= Cyc_Absyn_AnonStruct_e;
_temp122.f1=( void*) t; _temp122.f2= des; _temp122;}); _temp121;}))); goto _LL98;
_LL106:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp123=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp123[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp124; _temp124.tag= Cyc_Absyn_Array_e;
_temp124.f1= des; _temp124;}); _temp123;}))); goto _LL98; _LL98:;}} static void
Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp125= msg_part; struct _tagged_arr _temp126= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_get_arr_size( _temp125, 1u), _temp125.curr, _get_arr_size( _temp126, 1u),
_temp126.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){
void* t;{ void* _temp127= c; void* _temp143; void* _temp145; void* _temp147; int
_temp149; void* _temp151; struct _tagged_arr _temp153; _LL129: if(( unsigned int)
_temp127 > 1u?*(( int*) _temp127) == Cyc_Absyn_Char_c: 0){ _LL144: _temp143=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp127)->f1; goto _LL130;} else{
goto _LL131;} _LL131: if(( unsigned int) _temp127 > 1u?*(( int*) _temp127) ==
Cyc_Absyn_Short_c: 0){ _LL146: _temp145=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp127)->f1; goto _LL132;} else{ goto _LL133;} _LL133: if(( unsigned int)
_temp127 > 1u?*(( int*) _temp127) == Cyc_Absyn_LongLong_c: 0){ _LL148: _temp147=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp127)->f1; goto _LL134;} else{
goto _LL135;} _LL135: if(( unsigned int) _temp127 > 1u?*(( int*) _temp127) ==
Cyc_Absyn_Float_c: 0){ goto _LL136;} else{ goto _LL137;} _LL137: if((
unsigned int) _temp127 > 1u?*(( int*) _temp127) == Cyc_Absyn_Int_c: 0){ _LL152:
_temp151=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp127)->f1; goto _LL150;
_LL150: _temp149=(( struct Cyc_Absyn_Int_c_struct*) _temp127)->f2; goto _LL138;}
else{ goto _LL139;} _LL139: if(( unsigned int) _temp127 > 1u?*(( int*) _temp127)
== Cyc_Absyn_String_c: 0){ _LL154: _temp153=(( struct Cyc_Absyn_String_c_struct*)
_temp127)->f1; goto _LL140;} else{ goto _LL141;} _LL141: if( _temp127 ==( void*)
Cyc_Absyn_Null_c){ goto _LL142;} else{ goto _LL128;} _LL130: t= _temp143 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL128;
_LL132: t= _temp145 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL128; _LL134: t= _temp147 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t:
Cyc_Absyn_slonglong_t; goto _LL128; _LL136: t= Cyc_Absyn_float_t; goto _LL128;
_LL138: if( topt == 0){ t= _temp151 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp155= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); void* _temp165; void* _temp167; void* _temp169; void*
_temp171; void* _temp173; _LL157: if(( unsigned int) _temp155 > 4u?*(( int*)
_temp155) == Cyc_Absyn_IntType: 0){ _LL168: _temp167=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp155)->f1; goto _LL166; _LL166: _temp165=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp155)->f2; if( _temp165 ==( void*) Cyc_Absyn_B1){ goto _LL158;} else{ goto
_LL159;}} else{ goto _LL159;} _LL159: if(( unsigned int) _temp155 > 4u?*(( int*)
_temp155) == Cyc_Absyn_IntType: 0){ _LL172: _temp171=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp155)->f1; goto _LL170; _LL170: _temp169=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp155)->f2; if( _temp169 ==( void*) Cyc_Absyn_B2){ goto _LL160;} else{ goto
_LL161;}} else{ goto _LL161;} _LL161: if(( unsigned int) _temp155 > 4u?*(( int*)
_temp155) == Cyc_Absyn_IntType: 0){ _LL174: _temp173=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp155)->f1; goto _LL162;} else{ goto _LL163;} _LL163: goto _LL164; _LL158: t=
_temp167 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp175=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp175[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp176; _temp176.tag= Cyc_Absyn_Const_e;
_temp176.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp177=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp177[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp178; _temp178.tag= Cyc_Absyn_Char_c;
_temp178.f1=( void*) _temp167; _temp178.f2=( unsigned char) _temp149; _temp178;});
_temp177;})); _temp176;}); _temp175;}))); goto _LL156; _LL160: t= _temp171 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp179=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp179[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp180; _temp180.tag= Cyc_Absyn_Const_e; _temp180.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp181=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp181[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp182; _temp182.tag= Cyc_Absyn_Short_c; _temp182.f1=( void*) _temp171;
_temp182.f2=( short) _temp149; _temp182;}); _temp181;})); _temp180;}); _temp179;})));
goto _LL156; _LL162: t= _temp173 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL156; _LL164: t= _temp151 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL156; _LL156:;} goto _LL128; _LL140: {
int len=( int) _get_arr_size( _temp153, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp183=({ struct Cyc_Absyn_Const_e_struct* _temp220=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp220[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp221; _temp221.tag= Cyc_Absyn_Const_e; _temp221.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp222=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp222[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp223; _temp223.tag= Cyc_Absyn_Int_c; _temp223.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp223.f2= len; _temp223;}); _temp222;})); _temp221;}); _temp220;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp183, loc); elen->topt=({
struct Cyc_Core_Opt* _temp184=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp184->v=( void*) Cyc_Absyn_uint_t; _temp184;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp185=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp185[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp186; _temp186.tag= Cyc_Absyn_Upper_b; _temp186.f1= elen; _temp186;});
_temp185;})); if( topt != 0){ void* _temp187= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp195; struct Cyc_Absyn_Tqual
_temp197; void* _temp199; struct Cyc_Absyn_PtrInfo _temp201; struct Cyc_Absyn_Conref*
_temp203; struct Cyc_Absyn_Tqual _temp205; struct Cyc_Absyn_Conref* _temp207;
void* _temp209; void* _temp211; _LL189: if(( unsigned int) _temp187 > 4u?*(( int*)
_temp187) == Cyc_Absyn_ArrayType: 0){ _LL200: _temp199=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp187)->f1; goto _LL198; _LL198: _temp197=(( struct Cyc_Absyn_ArrayType_struct*)
_temp187)->f2; goto _LL196; _LL196: _temp195=(( struct Cyc_Absyn_ArrayType_struct*)
_temp187)->f3; goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int)
_temp187 > 4u?*(( int*) _temp187) == Cyc_Absyn_PointerType: 0){ _LL202: _temp201=((
struct Cyc_Absyn_PointerType_struct*) _temp187)->f1; _LL212: _temp211=( void*)
_temp201.elt_typ; goto _LL210; _LL210: _temp209=( void*) _temp201.rgn_typ; goto
_LL208; _LL208: _temp207= _temp201.nullable; goto _LL206; _LL206: _temp205=
_temp201.tq; goto _LL204; _LL204: _temp203= _temp201.bounds; goto _LL192;} else{
goto _LL193;} _LL193: goto _LL194; _LL190: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp213=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp213[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp214; _temp214.tag= Cyc_Absyn_ArrayType;
_temp214.f1=( void*) Cyc_Absyn_uchar_t; _temp214.f2= _temp197; _temp214.f3=(
struct Cyc_Absyn_Exp*) elen; _temp214;}); _temp213;}); _LL192: if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp215=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp215->v=( void*) t; _temp215;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp216=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp216->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp216;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp217=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp217[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp218; _temp218.tag= Cyc_Absyn_Upper_b; _temp218.f1= elen; _temp218;});
_temp217;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp219=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp219->v=( void*) t; _temp219;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL188; _LL194:
goto _LL188; _LL188:;} return t;} _LL142: { struct Cyc_List_List* _temp224= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp225=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp225[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp226; _temp226.tag= Cyc_Absyn_PointerType;
_temp226.f1=({ struct Cyc_Absyn_PtrInfo _temp227; _temp227.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp229=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp229->v=( void*)
_temp224; _temp229;})); _temp227.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp228=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp228->v=( void*) _temp224;
_temp228;})); _temp227.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp227.tq= Cyc_Absyn_empty_tqual(); _temp227.bounds= Cyc_Absyn_empty_conref();
_temp227;}); _temp226;}); _temp225;}); goto _LL128;} _LL128:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp230= b; struct Cyc_Absyn_Vardecl*
_temp244; struct Cyc_Absyn_Fndecl* _temp246; struct Cyc_Absyn_Vardecl* _temp248;
struct Cyc_Absyn_Vardecl* _temp250; struct Cyc_Absyn_Vardecl* _temp252; _LL232:
if( _temp230 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL233;} else{ goto _LL234;}
_LL234: if(( unsigned int) _temp230 > 1u?*(( int*) _temp230) == Cyc_Absyn_Global_b:
0){ _LL245: _temp244=(( struct Cyc_Absyn_Global_b_struct*) _temp230)->f1; goto
_LL235;} else{ goto _LL236;} _LL236: if(( unsigned int) _temp230 > 1u?*(( int*)
_temp230) == Cyc_Absyn_Funname_b: 0){ _LL247: _temp246=(( struct Cyc_Absyn_Funname_b_struct*)
_temp230)->f1; goto _LL237;} else{ goto _LL238;} _LL238: if(( unsigned int)
_temp230 > 1u?*(( int*) _temp230) == Cyc_Absyn_Pat_b: 0){ _LL249: _temp248=((
struct Cyc_Absyn_Pat_b_struct*) _temp230)->f1; goto _LL239;} else{ goto _LL240;}
_LL240: if(( unsigned int) _temp230 > 1u?*(( int*) _temp230) == Cyc_Absyn_Local_b:
0){ _LL251: _temp250=(( struct Cyc_Absyn_Local_b_struct*) _temp230)->f1; goto
_LL241;} else{ goto _LL242;} _LL242: if(( unsigned int) _temp230 > 1u?*(( int*)
_temp230) == Cyc_Absyn_Param_b: 0){ _LL253: _temp252=(( struct Cyc_Absyn_Param_b_struct*)
_temp230)->f1; goto _LL243;} else{ goto _LL231;} _LL233: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp254= Cyc_Absynpp_qvar2string(
q); xprintf("unbound identifier %.*s", _get_arr_size( _temp254, 1u), _temp254.curr);}));
_LL235: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp244->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp244->type; _LL237:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp246->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp246);
_LL239: _temp250= _temp248; goto _LL241; _LL241: _temp252= _temp250; goto _LL243;
_LL243:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp252->type; _LL231:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp255=( void*) fmt->r; void* _temp261; struct _tagged_arr
_temp263; _LL257: if(*(( int*) _temp255) == Cyc_Absyn_Const_e){ _LL262: _temp261=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp255)->f1; if(( unsigned int)
_temp261 > 1u?*(( int*) _temp261) == Cyc_Absyn_String_c: 0){ _LL264: _temp263=((
struct Cyc_Absyn_String_c_struct*) _temp261)->f1; goto _LL258;} else{ goto
_LL259;}} else{ goto _LL259;} _LL259: goto _LL260; _LL258: desc_types=
type_getter( te, _temp263, fmt->loc); goto _LL256; _LL260: Cyc_Tcutil_terr( fmt->loc,
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));
return; _LL256:;} for( 0; desc_types != 0? args != 0: 0;( desc_types=(( struct
Cyc_List_List*) _check_null( desc_types))->tl, args=(( struct Cyc_List_List*)
_check_null( args))->tl)){ void* t=( void*)(( struct Cyc_List_List*) _check_null(
desc_types))->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp265= Cyc_Absynpp_typ2string(
t); struct _tagged_arr _temp266= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("descriptor has type %.*s but argument has type %.*s",
_get_arr_size( _temp265, 1u), _temp265.curr, _get_arr_size( _temp266, 1u),
_temp266.curr);}));}} if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc, _tag_arr("too few arguments",
sizeof( unsigned char), 18u));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd)->loc, _tag_arr("too many arguments",
sizeof( unsigned char), 19u));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp267= p; _LL269: if( _temp267 ==( void*) Cyc_Absyn_Plus){ goto _LL270;}
else{ goto _LL271;} _LL271: if( _temp267 ==( void*) Cyc_Absyn_Minus){ goto
_LL272;} else{ goto _LL273;} _LL273: if( _temp267 ==( void*) Cyc_Absyn_Not){
goto _LL274;} else{ goto _LL275;} _LL275: if( _temp267 ==( void*) Cyc_Absyn_Bitnot){
goto _LL276;} else{ goto _LL277;} _LL277: if( _temp267 ==( void*) Cyc_Absyn_Size){
goto _LL278;} else{ goto _LL279;} _LL279: goto _LL280; _LL270: goto _LL272;
_LL272: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp281= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_get_arr_size( _temp281, 1u), _temp281.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL274: Cyc_Tcutil_check_contains_assign( e); if( !
Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp282= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_get_arr_size( _temp282, 1u), _temp282.curr);}));} return Cyc_Absyn_sint_t;
_LL276: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp283= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_get_arr_size( _temp283, 1u), _temp283.curr);}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL278:{ void* _temp284= t; struct Cyc_Absyn_PtrInfo
_temp292; _LL286: if(( unsigned int) _temp284 > 4u?*(( int*) _temp284) == Cyc_Absyn_ArrayType:
0){ goto _LL287;} else{ goto _LL288;} _LL288: if(( unsigned int) _temp284 > 4u?*((
int*) _temp284) == Cyc_Absyn_PointerType: 0){ _LL293: _temp292=(( struct Cyc_Absyn_PointerType_struct*)
_temp284)->f1; goto _LL289;} else{ goto _LL290;} _LL290: goto _LL291; _LL287:
goto _LL285; _LL289: goto _LL285; _LL291: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp294= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_get_arr_size( _temp294, 1u), _temp294.curr);})); goto _LL285; _LL285:;} return
Cyc_Absyn_uint_t; _LL280: return Cyc_Tcutil_impos( _tag_arr("Non-unary primop",
sizeof( unsigned char), 17u)); _LL268:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr(
e1->loc,( struct _tagged_arr)({ struct _tagged_arr _temp295= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); xprintf("type %.*s cannot be used here",
_get_arr_size( _temp295, 1u), _temp295.curr);})); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp296=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp296->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp296;}));}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp297= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); xprintf("type %.*s cannot be used here",
_get_arr_size( _temp297, 1u), _temp297.curr);})); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp298=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp298->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp298;}));}{
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type( t1, t2);}} static void* Cyc_Tcexp_tcPlus(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
void* _temp299= t1; struct Cyc_Absyn_PtrInfo _temp305; struct Cyc_Absyn_Conref*
_temp307; struct Cyc_Absyn_Tqual _temp309; struct Cyc_Absyn_Conref* _temp311;
void* _temp313; void* _temp315; _LL301: if(( unsigned int) _temp299 > 4u?*(( int*)
_temp299) == Cyc_Absyn_PointerType: 0){ _LL306: _temp305=(( struct Cyc_Absyn_PointerType_struct*)
_temp299)->f1; _LL316: _temp315=( void*) _temp305.elt_typ; goto _LL314; _LL314:
_temp313=( void*) _temp305.rgn_typ; goto _LL312; _LL312: _temp311= _temp305.nullable;
goto _LL310; _LL310: _temp309= _temp305.tq; goto _LL308; _LL308: _temp307=
_temp305.bounds; goto _LL302;} else{ goto _LL303;} _LL303: goto _LL304; _LL302:
if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp317= Cyc_Absynpp_typ2string( t2); xprintf("expecting int but found %.*s",
_get_arr_size( _temp317, 1u), _temp317.curr);}));} _temp307= Cyc_Absyn_compress_conref(
_temp307);{ void* _temp318=( void*) _temp307->v; void* _temp326; void* _temp328;
struct Cyc_Absyn_Exp* _temp330; _LL320: if(( unsigned int) _temp318 > 1u?*(( int*)
_temp318) == Cyc_Absyn_Eq_constr: 0){ _LL327: _temp326=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp318)->f1; if( _temp326 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL321;} else{
goto _LL322;}} else{ goto _LL322;} _LL322: if(( unsigned int) _temp318 > 1u?*((
int*) _temp318) == Cyc_Absyn_Eq_constr: 0){ _LL329: _temp328=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp318)->f1; if(( unsigned int) _temp328 > 1u?*((
int*) _temp328) == Cyc_Absyn_Upper_b: 0){ _LL331: _temp330=(( struct Cyc_Absyn_Upper_b_struct*)
_temp328)->f1; goto _LL323;} else{ goto _LL324;}} else{ goto _LL324;} _LL324:
goto _LL325; _LL321: return t1; _LL323: { struct Cyc_Absyn_PointerType_struct*
_temp332=({ struct Cyc_Absyn_PointerType_struct* _temp333=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp333[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp334; _temp334.tag= Cyc_Absyn_PointerType;
_temp334.f1=({ struct Cyc_Absyn_PtrInfo _temp335; _temp335.elt_typ=( void*)
_temp315; _temp335.rgn_typ=( void*) _temp313; _temp335.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp335.tq= _temp309; _temp335.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp335;}); _temp334;}); _temp333;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp332); return( void*) _temp332;} _LL325:( void*)( _temp307->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp336=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp336[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp337; _temp337.tag= Cyc_Absyn_Eq_constr; _temp337.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp337;}); _temp336;}))); return t1;
_LL319:;} _LL304: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL300:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp338= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct _tagged_arr _temp339=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_get_arr_size( _temp338, 1u), _temp338.curr, _get_arr_size( _temp339, 1u),
_temp339.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp340= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp341= Cyc_Absynpp_typ2string(
t2); xprintf("expecting either %.*s or int but found %.*s", _get_arr_size(
_temp340, 1u), _temp340.curr, _get_arr_size( _temp341, 1u), _temp341.curr);}));}
return t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp342=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp342->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp342;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp344=({ struct _tuple0 _temp343;
_temp343.f1= Cyc_Tcutil_compress( t1); _temp343.f2= Cyc_Tcutil_compress( t2);
_temp343;}); void* _temp350; struct Cyc_Absyn_PtrInfo _temp352; void* _temp354;
void* _temp356; struct Cyc_Absyn_PtrInfo _temp358; void* _temp360; _LL346:
_LL357: _temp356= _temp344.f1; if(( unsigned int) _temp356 > 4u?*(( int*)
_temp356) == Cyc_Absyn_PointerType: 0){ _LL359: _temp358=(( struct Cyc_Absyn_PointerType_struct*)
_temp356)->f1; _LL361: _temp360=( void*) _temp358.elt_typ; goto _LL351;} else{
goto _LL348;} _LL351: _temp350= _temp344.f2; if(( unsigned int) _temp350 > 4u?*((
int*) _temp350) == Cyc_Absyn_PointerType: 0){ _LL353: _temp352=(( struct Cyc_Absyn_PointerType_struct*)
_temp350)->f1; _LL355: _temp354=( void*) _temp352.elt_typ; goto _LL347;} else{
goto _LL348;} _LL348: goto _LL349; _LL347: if( Cyc_Tcutil_unify( _temp360,
_temp354)){ return Cyc_Absyn_sint_t;} goto _LL345; _LL349: goto _LL345; _LL345:;}
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp362= Cyc_Absynpp_typ2string(
t1); struct _tagged_arr _temp363= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_get_arr_size( _temp362, 1u), _temp362.curr, _get_arr_size( _temp363, 1u),
_temp363.curr);})); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp364=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp364->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp364;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* _temp365= p;
_LL367: if( _temp365 ==( void*) Cyc_Absyn_Plus){ goto _LL368;} else{ goto _LL369;}
_LL369: if( _temp365 ==( void*) Cyc_Absyn_Minus){ goto _LL370;} else{ goto
_LL371;} _LL371: if( _temp365 ==( void*) Cyc_Absyn_Times){ goto _LL372;} else{
goto _LL373;} _LL373: if( _temp365 ==( void*) Cyc_Absyn_Div){ goto _LL374;}
else{ goto _LL375;} _LL375: if( _temp365 ==( void*) Cyc_Absyn_Mod){ goto _LL376;}
else{ goto _LL377;} _LL377: if( _temp365 ==( void*) Cyc_Absyn_Bitand){ goto
_LL378;} else{ goto _LL379;} _LL379: if( _temp365 ==( void*) Cyc_Absyn_Bitor){
goto _LL380;} else{ goto _LL381;} _LL381: if( _temp365 ==( void*) Cyc_Absyn_Bitxor){
goto _LL382;} else{ goto _LL383;} _LL383: if( _temp365 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL384;} else{ goto _LL385;} _LL385: if( _temp365 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL386;} else{ goto _LL387;} _LL387: if( _temp365 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL388;} else{ goto _LL389;} _LL389: if( _temp365 ==( void*) Cyc_Absyn_Eq){
goto _LL390;} else{ goto _LL391;} _LL391: if( _temp365 ==( void*) Cyc_Absyn_Neq){
goto _LL392;} else{ goto _LL393;} _LL393: if( _temp365 ==( void*) Cyc_Absyn_Gt){
goto _LL394;} else{ goto _LL395;} _LL395: if( _temp365 ==( void*) Cyc_Absyn_Lt){
goto _LL396;} else{ goto _LL397;} _LL397: if( _temp365 ==( void*) Cyc_Absyn_Gte){
goto _LL398;} else{ goto _LL399;} _LL399: if( _temp365 ==( void*) Cyc_Absyn_Lte){
goto _LL400;} else{ goto _LL401;} _LL401: goto _LL402; _LL368: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL370: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL372: goto _LL374;
_LL374: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL376: goto _LL378; _LL378: goto _LL380; _LL380: goto _LL382; _LL382: goto
_LL384; _LL384: goto _LL386; _LL386: goto _LL388; _LL388: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL390: goto _LL392; _LL392: goto _LL394;
_LL394: goto _LL396; _LL396: goto _LL398; _LL398: goto _LL400; _LL400: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL402:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp403=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp403[ 0]=({ struct Cyc_Core_Failure_struct
_temp404; _temp404.tag= Cyc_Core_Failure; _temp404.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp404;}); _temp403;})); _LL366:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp405= p; _LL407: if( _temp405
==( void*) Cyc_Absyn_Printf){ goto _LL408;} else{ goto _LL409;} _LL409: if(
_temp405 ==( void*) Cyc_Absyn_Fprintf){ goto _LL410;} else{ goto _LL411;} _LL411:
if( _temp405 ==( void*) Cyc_Absyn_Xprintf){ goto _LL412;} else{ goto _LL413;}
_LL413: if( _temp405 ==( void*) Cyc_Absyn_Scanf){ goto _LL414;} else{ goto
_LL415;} _LL415: if( _temp405 ==( void*) Cyc_Absyn_Fscanf){ goto _LL416;} else{
goto _LL417;} _LL417: if( _temp405 ==( void*) Cyc_Absyn_Sscanf){ goto _LL418;}
else{ goto _LL419;} _LL419: goto _LL420; _LL408: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("missing format string in printf", sizeof( unsigned char), 32u));}
Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd,(( struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs);
t=( void*) Cyc_Absyn_VoidType; goto _LL406; _LL410: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fprintf",
sizeof( unsigned char), 29u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp421=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _get_arr_size( _temp421, 1u),
_temp421.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType; goto _LL406;
_LL412: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in xprintf",
sizeof( unsigned char), 33u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_format_typs); t= Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt*
_temp422=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp422->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp422;}))); goto _LL406;
_LL414: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("missing format string in scanf",
sizeof( unsigned char), 31u));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto
_LL406; _LL416: if( es == 0? 1:(( struct Cyc_List_List*) _check_null( es))->tl
== 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments to fscanf",
sizeof( unsigned char), 28u));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp423=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v);
xprintf("expecting FILE @ but found %.*s", _get_arr_size( _temp423, 1u),
_temp423.curr);}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd,((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->tl,
Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL406; _LL418: Cyc_Tcutil_terr(
loc, _tag_arr("sscanf is not supported", sizeof( unsigned char), 24u)); t= Cyc_Absyn_sint_t;
goto _LL406; _LL420: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ case 0: _LL424: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has 0 arguments",
sizeof( unsigned char), 35u)); case 1: _LL425: t= Cyc_Tcexp_tcUnPrimop( te, loc,
topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
break; case 2: _LL426: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd);
break; default: _LL427: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u));} goto _LL406; _LL406:;} return t;}} struct
_tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{ void* _temp429=( void*) e->r;
void* _temp453; struct Cyc_Absyn_Vardecl* _temp455; void* _temp457; struct Cyc_Absyn_Vardecl*
_temp459; void* _temp461; struct Cyc_Absyn_Vardecl* _temp463; void* _temp465;
struct Cyc_Absyn_Vardecl* _temp467; struct Cyc_Absyn_Exp* _temp469; struct Cyc_Absyn_Exp*
_temp471; struct _tagged_arr* _temp473; struct Cyc_Absyn_Exp* _temp475; struct
_tagged_arr* _temp477; struct Cyc_Absyn_Exp* _temp479; struct Cyc_Absyn_Exp*
_temp481; struct Cyc_Absyn_Exp* _temp483; struct Cyc_Absyn_Exp* _temp485; _LL431:
if(*(( int*) _temp429) == Cyc_Absyn_Var_e){ _LL454: _temp453=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp429)->f2; if(( unsigned int) _temp453 > 1u?*(( int*)
_temp453) == Cyc_Absyn_Param_b: 0){ _LL456: _temp455=(( struct Cyc_Absyn_Param_b_struct*)
_temp453)->f1; goto _LL432;} else{ goto _LL433;}} else{ goto _LL433;} _LL433:
if(*(( int*) _temp429) == Cyc_Absyn_Var_e){ _LL458: _temp457=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp429)->f2; if(( unsigned int) _temp457 > 1u?*(( int*)
_temp457) == Cyc_Absyn_Local_b: 0){ _LL460: _temp459=(( struct Cyc_Absyn_Local_b_struct*)
_temp457)->f1; goto _LL434;} else{ goto _LL435;}} else{ goto _LL435;} _LL435:
if(*(( int*) _temp429) == Cyc_Absyn_Var_e){ _LL462: _temp461=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp429)->f2; if(( unsigned int) _temp461 > 1u?*(( int*)
_temp461) == Cyc_Absyn_Pat_b: 0){ _LL464: _temp463=(( struct Cyc_Absyn_Pat_b_struct*)
_temp461)->f1; goto _LL436;} else{ goto _LL437;}} else{ goto _LL437;} _LL437:
if(*(( int*) _temp429) == Cyc_Absyn_Var_e){ _LL466: _temp465=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp429)->f2; if(( unsigned int) _temp465 > 1u?*(( int*)
_temp465) == Cyc_Absyn_Global_b: 0){ _LL468: _temp467=(( struct Cyc_Absyn_Global_b_struct*)
_temp465)->f1; goto _LL438;} else{ goto _LL439;}} else{ goto _LL439;} _LL439:
if(*(( int*) _temp429) == Cyc_Absyn_Subscript_e){ _LL472: _temp471=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp429)->f1; goto _LL470; _LL470: _temp469=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp429)->f2; goto _LL440;} else{ goto _LL441;} _LL441: if(*(( int*) _temp429)
== Cyc_Absyn_StructMember_e){ _LL476: _temp475=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp429)->f1; goto _LL474; _LL474: _temp473=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp429)->f2; goto _LL442;} else{ goto _LL443;} _LL443: if(*(( int*) _temp429)
== Cyc_Absyn_StructArrow_e){ _LL480: _temp479=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp429)->f1; goto _LL478; _LL478: _temp477=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp429)->f2; goto _LL444;} else{ goto _LL445;} _LL445: if(*(( int*) _temp429)
== Cyc_Absyn_Deref_e){ _LL482: _temp481=(( struct Cyc_Absyn_Deref_e_struct*)
_temp429)->f1; goto _LL446;} else{ goto _LL447;} _LL447: if(*(( int*) _temp429)
== Cyc_Absyn_NoInstantiate_e){ _LL484: _temp483=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp429)->f1; goto _LL448;} else{ goto _LL449;} _LL449: if(*(( int*) _temp429)
== Cyc_Absyn_Instantiate_e){ _LL486: _temp485=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp429)->f1; goto _LL450;} else{ goto _LL451;} _LL451: goto _LL452; _LL432:
_temp459= _temp455; goto _LL434; _LL434: _temp463= _temp459; goto _LL436; _LL436:
_temp467= _temp463; goto _LL438; _LL438: if( !( _temp467->tq).q_const){ return;}
goto _LL430; _LL440:{ void* _temp487= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp471->topt))->v); struct Cyc_Absyn_PtrInfo _temp497; struct Cyc_Absyn_Tqual
_temp499; struct Cyc_Absyn_Tqual _temp501; struct Cyc_List_List* _temp503;
_LL489: if(( unsigned int) _temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_PointerType:
0){ _LL498: _temp497=(( struct Cyc_Absyn_PointerType_struct*) _temp487)->f1;
_LL500: _temp499= _temp497.tq; goto _LL490;} else{ goto _LL491;} _LL491: if((
unsigned int) _temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_ArrayType: 0){
_LL502: _temp501=(( struct Cyc_Absyn_ArrayType_struct*) _temp487)->f2; goto
_LL492;} else{ goto _LL493;} _LL493: if(( unsigned int) _temp487 > 4u?*(( int*)
_temp487) == Cyc_Absyn_TupleType: 0){ _LL504: _temp503=(( struct Cyc_Absyn_TupleType_struct*)
_temp487)->f1; goto _LL494;} else{ goto _LL495;} _LL495: goto _LL496; _LL490:
_temp501= _temp499; goto _LL492; _LL492: if( ! _temp501.q_const){ return;} goto
_LL488; _LL494: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp469);{
struct _handler_cons _temp505; _push_handler(& _temp505);{ int _temp507= 0; if(
setjmp( _temp505.handler)){ _temp507= 1;} if( ! _temp507){{ struct _tuple8
_temp510; struct Cyc_Absyn_Tqual _temp511; struct _tuple8* _temp508=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( _temp503,( int) i);
_temp510=* _temp508; _LL512: _temp511= _temp510.f1; goto _LL509; _LL509: if( !
_temp511.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp506=( void*) _exn_thrown; void* _temp514= _temp506; _LL516: if( _temp514 ==
Cyc_List_Nth){ goto _LL517;} else{ goto _LL518;} _LL518: goto _LL519; _LL517:
return; _LL519:( void) _throw( _temp514); _LL515:;}}} goto _LL488;} _LL496: goto
_LL488; _LL488:;} goto _LL430; _LL442:{ void* _temp520= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp475->topt))->v); struct Cyc_Absyn_Structdecl**
_temp532; struct Cyc_List_List* _temp534; struct Cyc_List_List* _temp536; struct
Cyc_Absyn_Uniondecl** _temp538; _LL522: if(( unsigned int) _temp520 > 4u?*(( int*)
_temp520) == Cyc_Absyn_StructType: 0){ _LL533: _temp532=(( struct Cyc_Absyn_StructType_struct*)
_temp520)->f3; goto _LL523;} else{ goto _LL524;} _LL524: if(( unsigned int)
_temp520 > 4u?*(( int*) _temp520) == Cyc_Absyn_AnonUnionType: 0){ _LL535:
_temp534=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp520)->f1; goto _LL525;}
else{ goto _LL526;} _LL526: if(( unsigned int) _temp520 > 4u?*(( int*) _temp520)
== Cyc_Absyn_AnonStructType: 0){ _LL537: _temp536=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp520)->f1; goto _LL527;} else{ goto _LL528;} _LL528: if(( unsigned int)
_temp520 > 4u?*(( int*) _temp520) == Cyc_Absyn_UnionType: 0){ _LL539: _temp538=((
struct Cyc_Absyn_UnionType_struct*) _temp520)->f3; goto _LL529;} else{ goto
_LL530;} _LL530: goto _LL531; _LL523: { struct Cyc_Absyn_Structfield* sf=
_temp532 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp532)), _temp473); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL521;} _LL525: _temp536= _temp534; goto _LL527; _LL527: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp536, _temp473); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL521;} _LL529: { struct Cyc_Absyn_Structfield*
sf= _temp538 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp538)), _temp473); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL521;} _LL531: goto _LL521; _LL521:;} goto _LL430; _LL444:{ void*
_temp540= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp479->topt))->v); struct Cyc_Absyn_PtrInfo _temp546; void* _temp548; _LL542:
if(( unsigned int) _temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_PointerType: 0){
_LL547: _temp546=(( struct Cyc_Absyn_PointerType_struct*) _temp540)->f1; _LL549:
_temp548=( void*) _temp546.elt_typ; goto _LL543;} else{ goto _LL544;} _LL544:
goto _LL545; _LL543:{ void* _temp550= Cyc_Tcutil_compress( _temp548); struct Cyc_Absyn_Structdecl**
_temp562; struct Cyc_Absyn_Uniondecl** _temp564; struct Cyc_List_List* _temp566;
struct Cyc_List_List* _temp568; _LL552: if(( unsigned int) _temp550 > 4u?*(( int*)
_temp550) == Cyc_Absyn_StructType: 0){ _LL563: _temp562=(( struct Cyc_Absyn_StructType_struct*)
_temp550)->f3; goto _LL553;} else{ goto _LL554;} _LL554: if(( unsigned int)
_temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_UnionType: 0){ _LL565: _temp564=((
struct Cyc_Absyn_UnionType_struct*) _temp550)->f3; goto _LL555;} else{ goto
_LL556;} _LL556: if(( unsigned int) _temp550 > 4u?*(( int*) _temp550) == Cyc_Absyn_AnonStructType:
0){ _LL567: _temp566=(( struct Cyc_Absyn_AnonStructType_struct*) _temp550)->f1;
goto _LL557;} else{ goto _LL558;} _LL558: if(( unsigned int) _temp550 > 4u?*((
int*) _temp550) == Cyc_Absyn_AnonUnionType: 0){ _LL569: _temp568=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp550)->f1; goto _LL559;} else{ goto _LL560;} _LL560: goto _LL561; _LL553: {
struct Cyc_Absyn_Structfield* sf= _temp562 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp562)), _temp477); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL551;} _LL555: { struct Cyc_Absyn_Structfield*
sf= _temp564 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp564)), _temp477); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL551;} _LL557: _temp568= _temp566; goto _LL559; _LL559: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp568, _temp477); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL551;} _LL561: goto _LL551;
_LL551:;} goto _LL541; _LL545: goto _LL541; _LL541:;} goto _LL430; _LL446:{ void*
_temp570= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp481->topt))->v); struct Cyc_Absyn_PtrInfo _temp578; struct Cyc_Absyn_Tqual
_temp580; struct Cyc_Absyn_Tqual _temp582; _LL572: if(( unsigned int) _temp570 >
4u?*(( int*) _temp570) == Cyc_Absyn_PointerType: 0){ _LL579: _temp578=(( struct
Cyc_Absyn_PointerType_struct*) _temp570)->f1; _LL581: _temp580= _temp578.tq;
goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int) _temp570 > 4u?*((
int*) _temp570) == Cyc_Absyn_ArrayType: 0){ _LL583: _temp582=(( struct Cyc_Absyn_ArrayType_struct*)
_temp570)->f2; goto _LL575;} else{ goto _LL576;} _LL576: goto _LL577; _LL573:
_temp582= _temp580; goto _LL575; _LL575: if( ! _temp582.q_const){ return;} goto
_LL571; _LL577: goto _LL571; _LL571:;} goto _LL430; _LL448: _temp485= _temp483;
goto _LL450; _LL450: Cyc_Tcexp_check_writable( te, _temp485); return; _LL452:
goto _LL430; _LL430:;} Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a read-only location",
sizeof( unsigned char), 38u));} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void*
i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("increment/decrement of non-lvalue",
sizeof( unsigned char), 34u));} Cyc_Tcexp_check_writable( te, e);{ void* t=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_is_numeric(
e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp584= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic or ? type but found %.*s",
_get_arr_size( _temp584, 1u), _temp584.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1, _tag_arr("conditional expression", sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp(
te, topt, e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp591=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp591->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp591;})); struct Cyc_List_List* _temp585=({
struct Cyc_List_List* _temp590=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp590->hd=( void*) e3; _temp590->tl= 0; _temp590;});
struct Cyc_List_List* _temp586=({ struct Cyc_List_List* _temp589=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp589->hd=( void*) e2; _temp589->tl=
_temp585; _temp589;}); if( ! Cyc_Tcutil_coerce_list( te, t, _temp586)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp587= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); struct _tagged_arr
_temp588= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
e3->topt))->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_get_arr_size( _temp587, 1u), _temp587.curr, _get_arr_size( _temp588, 1u),
_temp588.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te,
0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v)), e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ void* _temp592= Cyc_Tcutil_compress( t1); _LL594:
if(( unsigned int) _temp592 > 4u?*(( int*) _temp592) == Cyc_Absyn_ArrayType: 0){
goto _LL595;} else{ goto _LL596;} _LL596: goto _LL597; _LL595: Cyc_Tcutil_terr(
loc, _tag_arr("cannot assign to an array", sizeof( unsigned char), 26u)); goto
_LL593; _LL597: goto _LL593; _LL593:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue", sizeof(
unsigned char), 25u));} Cyc_Tcexp_check_writable( te, e1); if( po == 0){ if( !
Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp598= Cyc_Absynpp_typ2string( t1);
struct _tagged_arr _temp599= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_get_arr_size( _temp598, 1u), _temp598.curr, _get_arr_size( _temp599, 1u),
_temp599.curr);}));}} else{ void* _temp600=( void*)(( struct Cyc_Core_Opt*)
_check_null( po))->v; void* _temp601= Cyc_Tcexp_tcBinPrimop( te, loc, 0,
_temp600, e1, e2); if( !( Cyc_Tcutil_unify( _temp601, t1)? 1: Cyc_Tcutil_coerceable(
_temp601))){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("Cannot use this operator in an assignment",
sizeof( unsigned char), 42u));} return _temp601;} return t2;}} static void* Cyc_Tcexp_tcSeqExp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, topt, e2); return( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;}
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; for( fields= fs; fields != 0;
fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ void* _temp604;
struct Cyc_Position_Segment* _temp606; struct Cyc_List_List* _temp608; struct
Cyc_List_List* _temp610; struct _tuple1* _temp612; struct Cyc_Absyn_Tunionfield
_temp602=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL613: _temp612= _temp602.name; goto _LL611; _LL611: _temp610=
_temp602.tvs; goto _LL609; _LL609: _temp608= _temp602.typs; goto _LL607; _LL607:
_temp606= _temp602.loc; goto _LL605; _LL605: _temp604=( void*) _temp602.sc; goto
_LL603; _LL603: if(( _temp608 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp608))->tl != 0)? 1: _temp610 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp608))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp616; struct Cyc_Position_Segment* _temp618; struct Cyc_List_List*
_temp620; struct Cyc_List_List* _temp622; struct _tuple1* _temp624; struct Cyc_Absyn_Tunionfield
_temp614=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL625: _temp624= _temp614.name; goto _LL623; _LL623: _temp622=
_temp614.tvs; goto _LL621; _LL621: _temp620= _temp614.typs; goto _LL619; _LL619:
_temp618= _temp614.loc; goto _LL617; _LL617: _temp616=( void*) _temp614.sc; goto
_LL615; _LL615: if(( _temp620 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp620))->tl != 0)? 1: _temp622 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp620))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr _temp626= Cyc_Absynpp_typ2string(
tu); struct _tagged_arr _temp627= Cyc_Absynpp_typ2string( t1); xprintf("can't find a field in %.*s to inject a value of type %.*s",
_get_arr_size( _temp626, 1u), _temp626.curr, _get_arr_size( _temp627, 1u),
_temp627.curr);})); return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* es, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){
te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp628= t;
struct Cyc_Absyn_PtrInfo _temp634; struct Cyc_Absyn_Conref* _temp636; struct Cyc_Absyn_Tqual
_temp638; struct Cyc_Absyn_Conref* _temp640; void* _temp642; void* _temp644;
_LL630: if(( unsigned int) _temp628 > 4u?*(( int*) _temp628) == Cyc_Absyn_PointerType:
0){ _LL635: _temp634=(( struct Cyc_Absyn_PointerType_struct*) _temp628)->f1;
_LL645: _temp644=( void*) _temp634.elt_typ; goto _LL643; _LL643: _temp642=( void*)
_temp634.rgn_typ; goto _LL641; _LL641: _temp640= _temp634.nullable; goto _LL639;
_LL639: _temp638= _temp634.tq; goto _LL637; _LL637: _temp636= _temp634.bounds;
goto _LL631;} else{ goto _LL632;} _LL632: goto _LL633; _LL631: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp642); Cyc_Tcutil_check_nonzero_bound( loc, _temp636);{ void*
_temp646= Cyc_Tcutil_compress( _temp644); struct Cyc_Absyn_FnInfo _temp652;
struct Cyc_List_List* _temp654; struct Cyc_List_List* _temp656; struct Cyc_Absyn_VarargInfo*
_temp658; int _temp660; struct Cyc_List_List* _temp662; void* _temp664; struct
Cyc_Core_Opt* _temp666; struct Cyc_List_List* _temp668; _LL648: if((
unsigned int) _temp646 > 4u?*(( int*) _temp646) == Cyc_Absyn_FnType: 0){ _LL653:
_temp652=(( struct Cyc_Absyn_FnType_struct*) _temp646)->f1; _LL669: _temp668=
_temp652.tvars; goto _LL667; _LL667: _temp666= _temp652.effect; goto _LL665;
_LL665: _temp664=( void*) _temp652.ret_typ; goto _LL663; _LL663: _temp662=
_temp652.args; goto _LL661; _LL661: _temp660= _temp652.c_varargs; goto _LL659;
_LL659: _temp658= _temp652.cyc_varargs; goto _LL657; _LL657: _temp656= _temp652.rgn_po;
goto _LL655; _LL655: _temp654= _temp652.attributes; goto _LL649;} else{ goto
_LL650;} _LL650: goto _LL651; _LL649: if( topt != 0){ Cyc_Tcutil_unify( _temp664,*((
void**) _check_null( topt)));} while( es != 0? _temp662 != 0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void*
t2=(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp662))->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp670= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct _tagged_arr
_temp671= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_get_arr_size( _temp670, 1u), _temp670.curr, _get_arr_size( _temp671, 1u),
_temp671.curr);}));} es=(( struct Cyc_List_List*) _check_null( es))->tl;
_temp662=(( struct Cyc_List_List*) _check_null( _temp662))->tl;} if( _temp662 !=
0){ Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function", sizeof(
unsigned char), 31u));} else{ if( es != 0){ if( _temp660){ for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}} else{ if(
_temp658 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("too many arguments for function",
sizeof( unsigned char), 32u));} else{ int _temp674; void* _temp676; void*
_temp678; struct Cyc_Absyn_VarargInfo _temp672=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp658)); _LL679: _temp678=( void*) _temp672.type; goto _LL677;
_LL677: _temp676=( void*) _temp672.rgn; goto _LL675; _LL675: _temp674= _temp672.inject;
goto _LL673; _LL673: { struct Cyc_Absyn_VarargCallInfo* _temp680=({ struct Cyc_Absyn_VarargCallInfo*
_temp707=( struct Cyc_Absyn_VarargCallInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp707->num_varargs= 0; _temp707->injectors= 0; _temp707->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp658); _temp707;});* vararg_call_info= _temp680; if( ! Cyc_Tcutil_unify(
Cyc_Tcenv_curr_rgn( te), _temp676)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp681= Cyc_Absynpp_typ2string( _temp676); struct
_tagged_arr _temp682= Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te)); xprintf("I have no idea how this could fail, but it did because the region for the varargs (%.*s) doesn't unify with the expected new_block region (%.*s).",
_get_arr_size( _temp681, 1u), _temp681.curr, _get_arr_size( _temp682, 1u),
_temp682.curr);}));} if( ! _temp674){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp680))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp678,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp678)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp683= Cyc_Absynpp_typ2string(
_temp678); struct _tagged_arr _temp684= Cyc_Absynpp_typ2string(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); xprintf("vararg requires type %.*s but argument has type %.*s",
_get_arr_size( _temp683, 1u), _temp683.curr, _get_arr_size( _temp684, 1u),
_temp684.curr);}));}}} else{ void* _temp685= Cyc_Tcutil_compress( _temp678);
struct Cyc_Absyn_TunionInfo _temp691; void* _temp693; struct Cyc_List_List*
_temp695; void* _temp697; struct Cyc_Absyn_Tuniondecl* _temp699; _LL687: if((
unsigned int) _temp685 > 4u?*(( int*) _temp685) == Cyc_Absyn_TunionType: 0){
_LL692: _temp691=(( struct Cyc_Absyn_TunionType_struct*) _temp685)->f1; _LL698:
_temp697=( void*) _temp691.tunion_info; if(*(( int*) _temp697) == Cyc_Absyn_KnownTunion){
_LL700: _temp699=(( struct Cyc_Absyn_KnownTunion_struct*) _temp697)->f1; goto
_LL696;} else{ goto _LL689;} _LL696: _temp695= _temp691.targs; goto _LL694;
_LL694: _temp693=( void*) _temp691.rgn; goto _LL688;} else{ goto _LL689;} _LL689:
goto _LL690; _LL688: { struct Cyc_Absyn_Tuniondecl* _temp701=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp699->name); if( ! Cyc_Tcutil_unify( _temp693, _temp676)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr) xprintf("bad region for injected varargs"));}{ struct
Cyc_List_List* fields= 0; if( _temp701->fields == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp702= Cyc_Absynpp_typ2string(
_temp678); xprintf("can't inject into %.*s", _get_arr_size( _temp702, 1u),
_temp702.curr);}));} else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp701->fields))->v;}{ struct _RegionHandle _temp703= _new_region();
struct _RegionHandle* rgn=& _temp703; _push_region( rgn);{ struct Cyc_List_List*
_temp704=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp701->tvs, _temp695); for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){(( struct Cyc_Absyn_VarargCallInfo*) _check_null(
_temp680))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp705= Cyc_Tcexp_tcInjection( te, e1, _temp678,
rgn, _temp704, fields); if( _temp705 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp680))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp680))->injectors,({ struct Cyc_List_List* _temp706=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp706->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp705)); _temp706->tl= 0;
_temp706;}));}}}}}; _pop_region( rgn);} goto _LL686;}} _LL690: Cyc_Tcutil_terr(
loc, _tag_arr("bad inject vararg type", sizeof( unsigned char), 23u)); goto
_LL686; _LL686:;}}}}}} Cyc_Tcenv_check_effect_accessible( te, loc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp666))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp656); return _temp664; _LL651: return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("expected pointer to function", sizeof( unsigned char), 29u)); _LL647:;}
_LL633: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u)); _LL629:;}} static void* Cyc_Tcexp_tcThrow( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( ! Cyc_Tcutil_coerce_use(
te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp708= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("expected xtunion exn but found %.*s",
_get_arr_size( _temp708, 1u), _temp708.curr);}));} return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp709=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp709->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp709;}));}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*)
Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v)));{ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp710= t1; struct Cyc_Absyn_PtrInfo
_temp716; struct Cyc_Absyn_Conref* _temp718; struct Cyc_Absyn_Tqual _temp720;
struct Cyc_Absyn_Conref* _temp722; void* _temp724; void* _temp726; _LL712: if((
unsigned int) _temp710 > 4u?*(( int*) _temp710) == Cyc_Absyn_PointerType: 0){
_LL717: _temp716=(( struct Cyc_Absyn_PointerType_struct*) _temp710)->f1; _LL727:
_temp726=( void*) _temp716.elt_typ; goto _LL725; _LL725: _temp724=( void*)
_temp716.rgn_typ; goto _LL723; _LL723: _temp722= _temp716.nullable; goto _LL721;
_LL721: _temp720= _temp716.tq; goto _LL719; _LL719: _temp718= _temp716.bounds;
goto _LL713;} else{ goto _LL714;} _LL714: goto _LL715; _LL713:{ void* _temp728=
Cyc_Tcutil_compress( _temp726); struct Cyc_Absyn_FnInfo _temp734; struct Cyc_List_List*
_temp736; struct Cyc_List_List* _temp738; struct Cyc_Absyn_VarargInfo* _temp740;
int _temp742; struct Cyc_List_List* _temp744; void* _temp746; struct Cyc_Core_Opt*
_temp748; struct Cyc_List_List* _temp750; _LL730: if(( unsigned int) _temp728 >
4u?*(( int*) _temp728) == Cyc_Absyn_FnType: 0){ _LL735: _temp734=(( struct Cyc_Absyn_FnType_struct*)
_temp728)->f1; _LL751: _temp750= _temp734.tvars; goto _LL749; _LL749: _temp748=
_temp734.effect; goto _LL747; _LL747: _temp746=( void*) _temp734.ret_typ; goto
_LL745; _LL745: _temp744= _temp734.args; goto _LL743; _LL743: _temp742= _temp734.c_varargs;
goto _LL741; _LL741: _temp740= _temp734.cyc_varargs; goto _LL739; _LL739:
_temp738= _temp734.rgn_po; goto _LL737; _LL737: _temp736= _temp734.attributes;
goto _LL731;} else{ goto _LL732;} _LL732: goto _LL733; _LL731: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) !=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp750)){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("type instantiation mismatch",
sizeof( unsigned char), 28u));} for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, _temp750=(( struct Cyc_List_List*) _check_null( _temp750))->tl)){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp750))->hd)->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); instantiation=({
struct Cyc_List_List* _temp752=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp752->hd=( void*)({ struct _tuple5* _temp753=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp753->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp750))->hd; _temp753->f2=( void*)((
struct Cyc_List_List*) _check_null( ts))->hd; _temp753;}); _temp752->tl=
instantiation; _temp752;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp757=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp757[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp758; _temp758.tag= Cyc_Absyn_FnType; _temp758.f1=({ struct Cyc_Absyn_FnInfo
_temp759; _temp759.tvars= 0; _temp759.effect= _temp748; _temp759.ret_typ=( void*)
_temp746; _temp759.args= _temp744; _temp759.c_varargs= _temp742; _temp759.cyc_varargs=
_temp740; _temp759.rgn_po= _temp738; _temp759.attributes= _temp736; _temp759;});
_temp758;}); _temp757;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp754=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp754[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp755; _temp755.tag= Cyc_Absyn_PointerType;
_temp755.f1=({ struct Cyc_Absyn_PtrInfo _temp756; _temp756.elt_typ=( void*)
new_fn_typ; _temp756.rgn_typ=( void*) _temp724; _temp756.nullable= _temp722;
_temp756.tq= _temp720; _temp756.bounds= _temp718; _temp756;}); _temp755;});
_temp754;}); return new_typ;}} _LL733: goto _LL729; _LL729:;} goto _LL711;
_LL715: goto _LL711; _LL711:;} return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp760= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_get_arr_size( _temp760, 1u), _temp760.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable(
te, loc, t2, t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp761= Cyc_Absynpp_typ2string(
t2); struct _tagged_arr _temp762= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_get_arr_size( _temp761, 1u), _temp761.curr, _get_arr_size( _temp762, 1u),
_temp762.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
void** _temp763= 0; struct Cyc_Absyn_Tqual _temp764= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp765= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp771; struct Cyc_Absyn_Tqual _temp773;
void* _temp775; _LL767: if(( unsigned int) _temp765 > 4u?*(( int*) _temp765) ==
Cyc_Absyn_PointerType: 0){ _LL772: _temp771=(( struct Cyc_Absyn_PointerType_struct*)
_temp765)->f1; _LL776: _temp775=( void*) _temp771.elt_typ; goto _LL774; _LL774:
_temp773= _temp771.tq; goto _LL768;} else{ goto _LL769;} _LL769: goto _LL770;
_LL768: _temp763=({ void** _temp777=( void**) GC_malloc( sizeof( void*));
_temp777[ 0]= _temp775; _temp777;}); _temp764= _temp773; goto _LL766; _LL770:
goto _LL766; _LL766:;} Cyc_Tcexp_tcExpNoInst( te, _temp763, e);{ void* _temp778=(
void*) e->r; _LL780: if(*(( int*) _temp778) == Cyc_Absyn_Struct_e){ goto _LL781;}
else{ goto _LL782;} _LL782: if(*(( int*) _temp778) == Cyc_Absyn_Tuple_e){ goto
_LL783;} else{ goto _LL784;} _LL784: goto _LL785; _LL781: goto _LL783; _LL783:
Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate", sizeof( unsigned char), 19u));{
void*(* _temp786)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp787= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp793; struct Cyc_Absyn_Conref* _temp795;
_LL789: if(( unsigned int) _temp787 > 4u?*(( int*) _temp787) == Cyc_Absyn_PointerType:
0){ _LL794: _temp793=(( struct Cyc_Absyn_PointerType_struct*) _temp787)->f1;
_LL796: _temp795= _temp793.nullable; goto _LL790;} else{ goto _LL791;} _LL791:
goto _LL792; _LL790: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp795)){ _temp786= Cyc_Absyn_star_typ;} goto _LL788; _LL792: goto _LL788;
_LL788:;} return _temp786(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp764);} _LL785: goto _LL779; _LL779:;}{ void*
_temp799; int _temp801; struct _tuple6 _temp797= Cyc_Tcutil_addressof_props( te,
e); _LL802: _temp801= _temp797.f1; goto _LL800; _LL800: _temp799= _temp797.f2;
goto _LL798; _LL798: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp801){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp799, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); if( ! Cyc_Evexp_okay_szofarg( t)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp803= Cyc_Absynpp_typ2string( t); xprintf("sizeof applied to type %.*s, which has unknown size here",
_get_arr_size( _temp803, 1u), _temp803.curr);}));} return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct _tagged_arr* n){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ int
bad_type= 1;{ void* _temp804= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp812; _LL806: if(( unsigned int) _temp804 > 4u?*(( int*) _temp804) == Cyc_Absyn_StructType:
0){ _LL813: _temp812=(( struct Cyc_Absyn_StructType_struct*) _temp804)->f3; goto
_LL807;} else{ goto _LL808;} _LL808: if(( unsigned int) _temp804 > 4u?*(( int*)
_temp804) == Cyc_Absyn_AnonStructType: 0){ goto _LL809;} else{ goto _LL810;}
_LL810: goto _LL811; _LL807: if( _temp812 == 0){ return Cyc_Tcutil_impos(
_tag_arr("offsetof on unchecked StructType", sizeof( unsigned char), 33u));} if((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp812)))->fields == 0){ goto
_LL805;} goto _LL809; _LL809: bad_type= 0; goto _LL805; _LL811: goto _LL805;
_LL805:;} if( bad_type){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp814= Cyc_Absynpp_typ2string( t); xprintf("%.*s is not a known struct type",
_get_arr_size( _temp814, 1u), _temp814.curr);}));} return Cyc_Absyn_uint_t;}}
static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp815= t; struct Cyc_Absyn_PtrInfo _temp821; struct Cyc_Absyn_Conref*
_temp823; void* _temp825; void* _temp827; _LL817: if(( unsigned int) _temp815 >
4u?*(( int*) _temp815) == Cyc_Absyn_PointerType: 0){ _LL822: _temp821=(( struct
Cyc_Absyn_PointerType_struct*) _temp815)->f1; _LL828: _temp827=( void*) _temp821.elt_typ;
goto _LL826; _LL826: _temp825=( void*) _temp821.rgn_typ; goto _LL824; _LL824:
_temp823= _temp821.bounds; goto _LL818;} else{ goto _LL819;} _LL819: goto _LL820;
_LL818: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp825); Cyc_Tcutil_check_nonzero_bound(
loc, _temp823); return _temp827; _LL820: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp829= Cyc_Absynpp_typ2string( t);
xprintf("expecting * or @ type but found %.*s", _get_arr_size( _temp829, 1u),
_temp829.curr);})); _LL816:;}} static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* outer_e,
struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0,
e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); void* _temp830= t; struct Cyc_Absyn_Structdecl** _temp846; struct
Cyc_Absyn_Structdecl* _temp848; struct Cyc_List_List* _temp849; struct _tuple1*
_temp851; struct Cyc_List_List* _temp853; struct Cyc_List_List* _temp855; struct
Cyc_Absyn_Uniondecl** _temp857; struct Cyc_Absyn_Uniondecl* _temp859; struct Cyc_List_List*
_temp860; struct _tuple1* _temp862; _LL832: if(( unsigned int) _temp830 > 4u?*((
int*) _temp830) == Cyc_Absyn_StructType: 0){ _LL852: _temp851=(( struct Cyc_Absyn_StructType_struct*)
_temp830)->f1; goto _LL850; _LL850: _temp849=(( struct Cyc_Absyn_StructType_struct*)
_temp830)->f2; goto _LL847; _LL847: _temp846=(( struct Cyc_Absyn_StructType_struct*)
_temp830)->f3; if( _temp846 == 0){ goto _LL834;} else{ _temp848=* _temp846; goto
_LL833;}} else{ goto _LL834;} _LL834: if(( unsigned int) _temp830 > 4u?*(( int*)
_temp830) == Cyc_Absyn_AnonStructType: 0){ _LL854: _temp853=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp830)->f1; goto _LL835;} else{ goto _LL836;} _LL836: if(( unsigned int)
_temp830 > 4u?*(( int*) _temp830) == Cyc_Absyn_AnonUnionType: 0){ _LL856:
_temp855=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp830)->f1; goto _LL837;}
else{ goto _LL838;} _LL838: if(( unsigned int) _temp830 > 4u?*(( int*) _temp830)
== Cyc_Absyn_UnionType: 0){ _LL863: _temp862=(( struct Cyc_Absyn_UnionType_struct*)
_temp830)->f1; goto _LL861; _LL861: _temp860=(( struct Cyc_Absyn_UnionType_struct*)
_temp830)->f2; goto _LL858; _LL858: _temp857=(( struct Cyc_Absyn_UnionType_struct*)
_temp830)->f3; if( _temp857 == 0){ goto _LL840;} else{ _temp859=* _temp857; goto
_LL839;}} else{ goto _LL840;} _LL840: if(( unsigned int) _temp830 > 4u?*(( int*)
_temp830) == Cyc_Absyn_ArrayType: 0){ goto _LL864;} else{ goto _LL842;} _LL864:
if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u)) == 0){
goto _LL841;} else{ goto _LL842;} _LL842: if(( unsigned int) _temp830 > 4u?*((
int*) _temp830) == Cyc_Absyn_PointerType: 0){ goto _LL865;} else{ goto _LL844;}
_LL865: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
== 0){ goto _LL843;} else{ goto _LL844;} _LL844: goto _LL845; _LL833: if(
_temp851 == 0){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)",
sizeof( unsigned char), 40u));}{ struct Cyc_Absyn_Structfield* _temp866= Cyc_Absyn_lookup_struct_field(
_temp848, f); if( _temp866 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp867= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp851)); struct _tagged_arr _temp868=* f; xprintf("struct %.*s has no %.*s field",
_get_arr_size( _temp867, 1u), _temp867.curr, _get_arr_size( _temp868, 1u),
_temp868.curr);}));}{ void* t2;{ struct _RegionHandle _temp869= _new_region();
struct _RegionHandle* rgn=& _temp869; _push_region( rgn);{ struct Cyc_List_List*
_temp870=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp848->tvs, _temp849); t2= Cyc_Tcutil_rsubstitute( rgn, _temp870,(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp866))->type);};
_pop_region( rgn);} return t2;}} _LL835: { struct Cyc_Absyn_Structfield*
_temp871= Cyc_Absyn_lookup_field( _temp853, f); if( _temp871 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp872=* f; xprintf("struct has no %.*s field",
_get_arr_size( _temp872, 1u), _temp872.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp871))->type;} _LL837: { struct Cyc_Absyn_Structfield* _temp873=
Cyc_Absyn_lookup_field( _temp855, f); if( _temp873 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp874=* f; xprintf("union has no %.*s field",
_get_arr_size( _temp874, 1u), _temp874.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp873))->type;} _LL839: if( _temp862 == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u));}{ struct Cyc_Absyn_Structfield* _temp875= Cyc_Absyn_lookup_union_field(
_temp859, f); if( _temp875 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp876= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp862)); struct _tagged_arr _temp877=* f; xprintf("union %.*s has no %.*s field",
_get_arr_size( _temp876, 1u), _temp876.curr, _get_arr_size( _temp877, 1u),
_temp877.curr);}));}{ struct Cyc_List_List* _temp878=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp859->tvs,
_temp860); void* _temp879= Cyc_Tcutil_substitute( _temp878,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp875))->type); return _temp879;}} _LL841:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp880=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp880[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp881; _temp881.tag= Cyc_Absyn_Primop_e; _temp881.f1=(
void*)(( void*) Cyc_Absyn_Size); _temp881.f2=({ struct Cyc_List_List* _temp882=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp882->hd=(
void*) e; _temp882->tl= 0; _temp882;}); _temp881;}); _temp880;}))); return Cyc_Absyn_uint_t;
_LL843:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp883=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp883[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp884; _temp884.tag= Cyc_Absyn_Primop_e;
_temp884.f1=( void*)(( void*) Cyc_Absyn_Size); _temp884.f2=({ struct Cyc_List_List*
_temp885=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp885->hd=( void*) e; _temp885->tl= 0; _temp885;}); _temp884;}); _temp883;})));
return Cyc_Absyn_uint_t; _LL845: if( Cyc_String_zstrcmp(* f, _tag_arr("size",
sizeof( unsigned char), 5u)) == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp886= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_get_arr_size( _temp886, 1u), _temp886.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp887= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct or union, found %.*s", _get_arr_size( _temp887, 1u),
_temp887.curr);}));} _LL831:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct _tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp888= t;
struct Cyc_Absyn_PtrInfo _temp894; struct Cyc_Absyn_Conref* _temp896; void*
_temp898; void* _temp900; _LL890: if(( unsigned int) _temp888 > 4u?*(( int*)
_temp888) == Cyc_Absyn_PointerType: 0){ _LL895: _temp894=(( struct Cyc_Absyn_PointerType_struct*)
_temp888)->f1; _LL901: _temp900=( void*) _temp894.elt_typ; goto _LL899; _LL899:
_temp898=( void*) _temp894.rgn_typ; goto _LL897; _LL897: _temp896= _temp894.bounds;
goto _LL891;} else{ goto _LL892;} _LL892: goto _LL893; _LL891: Cyc_Tcutil_check_nonzero_bound(
loc, _temp896);{ void* _temp902= Cyc_Tcutil_compress( _temp900); struct Cyc_Absyn_Structdecl**
_temp914; struct Cyc_Absyn_Structdecl* _temp916; struct Cyc_List_List* _temp917;
struct _tuple1* _temp919; struct Cyc_Absyn_Uniondecl** _temp921; struct Cyc_Absyn_Uniondecl*
_temp923; struct Cyc_List_List* _temp924; struct _tuple1* _temp926; struct Cyc_List_List*
_temp928; struct Cyc_List_List* _temp930; _LL904: if(( unsigned int) _temp902 >
4u?*(( int*) _temp902) == Cyc_Absyn_StructType: 0){ _LL920: _temp919=(( struct
Cyc_Absyn_StructType_struct*) _temp902)->f1; goto _LL918; _LL918: _temp917=((
struct Cyc_Absyn_StructType_struct*) _temp902)->f2; goto _LL915; _LL915:
_temp914=(( struct Cyc_Absyn_StructType_struct*) _temp902)->f3; if( _temp914 ==
0){ goto _LL906;} else{ _temp916=* _temp914; goto _LL905;}} else{ goto _LL906;}
_LL906: if(( unsigned int) _temp902 > 4u?*(( int*) _temp902) == Cyc_Absyn_UnionType:
0){ _LL927: _temp926=(( struct Cyc_Absyn_UnionType_struct*) _temp902)->f1; goto
_LL925; _LL925: _temp924=(( struct Cyc_Absyn_UnionType_struct*) _temp902)->f2;
goto _LL922; _LL922: _temp921=(( struct Cyc_Absyn_UnionType_struct*) _temp902)->f3;
if( _temp921 == 0){ goto _LL908;} else{ _temp923=* _temp921; goto _LL907;}}
else{ goto _LL908;} _LL908: if(( unsigned int) _temp902 > 4u?*(( int*) _temp902)
== Cyc_Absyn_AnonStructType: 0){ _LL929: _temp928=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp902)->f1; goto _LL909;} else{ goto _LL910;} _LL910: if(( unsigned int)
_temp902 > 4u?*(( int*) _temp902) == Cyc_Absyn_AnonUnionType: 0){ _LL931:
_temp930=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp902)->f1; goto _LL911;}
else{ goto _LL912;} _LL912: goto _LL913; _LL905: { struct Cyc_Absyn_Structfield*
_temp932= Cyc_Absyn_lookup_struct_field( _temp916, f); if( _temp932 == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr
_temp933= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp919));
struct _tagged_arr _temp934=* f; xprintf("struct %.*s has no %.*s field",
_get_arr_size( _temp933, 1u), _temp933.curr, _get_arr_size( _temp934, 1u),
_temp934.curr);}));}{ void* t3;{ struct _RegionHandle _temp935= _new_region();
struct _RegionHandle* rgn=& _temp935; _push_region( rgn);{ struct Cyc_List_List*
_temp936=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp916->tvs, _temp917); t3= Cyc_Tcutil_rsubstitute( rgn, _temp936,(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp932))->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp898); return
t3;}} _LL907: { struct Cyc_Absyn_Structfield* _temp937= Cyc_Absyn_lookup_union_field(
_temp923, f); if( _temp937 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp938= Cyc_Absynpp_qvar2string(( struct
_tuple1*) _check_null( _temp926)); struct _tagged_arr _temp939=* f; xprintf("union %.*s has no %.*s field",
_get_arr_size( _temp938, 1u), _temp938.curr, _get_arr_size( _temp939, 1u),
_temp939.curr);}));}{ struct Cyc_List_List* _temp940=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp923->tvs,
_temp924); void* _temp941= Cyc_Tcutil_substitute( _temp940,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp937))->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp898); return _temp941;}} _LL909: { struct Cyc_Absyn_Structfield* _temp942=
Cyc_Absyn_lookup_field( _temp928, f); if( _temp942 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp943=* f; xprintf("struct has no %.*s field",
_get_arr_size( _temp943, 1u), _temp943.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp942))->type;} _LL911: { struct Cyc_Absyn_Structfield* _temp944=
Cyc_Absyn_lookup_field( _temp930, f); if( _temp944 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr _temp945=* f; xprintf("union has no %.*s field",
_get_arr_size( _temp945, 1u), _temp945.curr);}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp944))->type;} _LL913: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp946= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct pointer, found %.*s", _get_arr_size( _temp946, 1u),
_temp946.curr);})); _LL903:;} _LL893: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr _temp947= Cyc_Absynpp_typ2string( t);
xprintf("expecting struct pointer, found %.*s", _get_arr_size( _temp947, 1u),
_temp947.curr);})); _LL889:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp948; _push_handler(& _temp948);{ int _temp950= 0; if( setjmp(
_temp948.handler)){ _temp950= 1;} if( ! _temp950){{ void* _temp951=(*(( struct
_tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp951;}; _pop_handler();} else{ void* _temp949=(
void*) _exn_thrown; void* _temp953= _temp949; _LL955: if( _temp953 == Cyc_List_Nth){
goto _LL956;} else{ goto _LL957;} _LL957: goto _LL958; _LL956: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr) xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL958:(
void) _throw( _temp953); _LL954:;}}} static void* Cyc_Tcexp_tcSubscript( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return
Cyc_Tcexp_expr_err( te, e2->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp959= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_get_arr_size( _temp959, 1u), _temp959.curr);}));}{ void* _temp960= t1; struct
Cyc_Absyn_PtrInfo _temp968; struct Cyc_Absyn_Conref* _temp970; struct Cyc_Absyn_Tqual
_temp972; void* _temp974; void* _temp976; struct Cyc_List_List* _temp978; _LL962:
if(( unsigned int) _temp960 > 4u?*(( int*) _temp960) == Cyc_Absyn_PointerType: 0){
_LL969: _temp968=(( struct Cyc_Absyn_PointerType_struct*) _temp960)->f1; _LL977:
_temp976=( void*) _temp968.elt_typ; goto _LL975; _LL975: _temp974=( void*)
_temp968.rgn_typ; goto _LL973; _LL973: _temp972= _temp968.tq; goto _LL971;
_LL971: _temp970= _temp968.bounds; goto _LL963;} else{ goto _LL964;} _LL964: if((
unsigned int) _temp960 > 4u?*(( int*) _temp960) == Cyc_Absyn_TupleType: 0){
_LL979: _temp978=(( struct Cyc_Absyn_TupleType_struct*) _temp960)->f1; goto
_LL965;} else{ goto _LL966;} _LL966: goto _LL967; _LL963: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp970);} else{{ void* _temp980=( void*)( Cyc_Absyn_compress_conref( _temp970))->v;
void* _temp986; struct Cyc_Absyn_Exp* _temp988; struct Cyc_Absyn_Exp _temp990;
void* _temp991; void* _temp993; int _temp995; void* _temp997; _LL982: if((
unsigned int) _temp980 > 1u?*(( int*) _temp980) == Cyc_Absyn_Eq_constr: 0){
_LL987: _temp986=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp980)->f1;
if(( unsigned int) _temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Upper_b: 0){
_LL989: _temp988=(( struct Cyc_Absyn_Upper_b_struct*) _temp986)->f1; _temp990=*
_temp988; _LL992: _temp991=( void*) _temp990.r; if(*(( int*) _temp991) == Cyc_Absyn_Const_e){
_LL994: _temp993=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp991)->f1; if((
unsigned int) _temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Int_c: 0){ _LL998:
_temp997=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp993)->f1; if( _temp997
==( void*) Cyc_Absyn_Unsigned){ goto _LL996;} else{ goto _LL984;} _LL996:
_temp995=(( struct Cyc_Absyn_Int_c_struct*) _temp993)->f2; if( _temp995 == 1){
goto _LL983;} else{ goto _LL984;}} else{ goto _LL984;}} else{ goto _LL984;}}
else{ goto _LL984;}} else{ goto _LL984;} _LL984: goto _LL985; _LL983: Cyc_Tcutil_warn(
e1->loc, _tag_arr("subscript applied to pointer to one object", sizeof(
unsigned char), 43u)); goto _LL981; _LL985: goto _LL981; _LL981:;} Cyc_Tcutil_check_nonzero_bound(
loc, _temp970);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp974); return
_temp976; _LL965: return Cyc_Tcexp_ithTupleType( te, loc, _temp978, e2); _LL967:
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr
_temp999= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_get_arr_size( _temp999, 1u), _temp999.curr);})); _LL961:;}}} static void* Cyc_Tcexp_tcTuple(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_List_List* es){ int done= 0; struct Cyc_List_List* fields= 0; if( topt != 0){
void* _temp1000= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct
Cyc_List_List* _temp1006; _LL1002: if(( unsigned int) _temp1000 > 4u?*(( int*)
_temp1000) == Cyc_Absyn_TupleType: 0){ _LL1007: _temp1006=(( struct Cyc_Absyn_TupleType_struct*)
_temp1000)->f1; goto _LL1003;} else{ goto _LL1004;} _LL1004: goto _LL1005;
_LL1003: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1006) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,
_tag_arr("tuple expression has the wrong number of fields", sizeof(
unsigned char), 48u)); goto _LL1001;} for( 0; es != 0;( es=(( struct Cyc_List_List*)
_check_null( es))->tl, _temp1006=(( struct Cyc_List_List*) _check_null(
_temp1006))->tl)){ void* _temp1008=(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1006))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)&
_temp1008,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
fields=({ struct Cyc_List_List* _temp1009=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1009->hd=( void*)({ struct _tuple8*
_temp1010=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1010->f1=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1006))->hd)).f1;
_temp1010->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1010;});
_temp1009->tl= fields; _temp1009;});} done= 1; goto _LL1001; _LL1005: goto
_LL1001; _LL1001:;} if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); fields=({ struct Cyc_List_List*
_temp1011=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1011->hd=( void*)({ struct _tuple8* _temp1012=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1012->f1= Cyc_Absyn_empty_tqual(); _temp1012->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1012;}); _temp1011->tl=
fields; _temp1011;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1013=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1013[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1014; _temp1014.tag= Cyc_Absyn_TupleType;
_temp1014.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1014;}); _temp1013;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("tcCompoundLit", sizeof( unsigned char), 14u));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1032=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1032->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1032;})); struct Cyc_Absyn_Const_e_struct*
_temp1015=({ struct Cyc_Absyn_Const_e_struct* _temp1028=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1028[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1029; _temp1029.tag= Cyc_Absyn_Const_e; _temp1029.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1030=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1030[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1031; _temp1031.tag= Cyc_Absyn_Int_c; _temp1031.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1031.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1031;}); _temp1030;})); _temp1029;}); _temp1028;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1015, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1016=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1016->v=( void*) Cyc_Absyn_uint_t; _temp1016;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1026=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1026[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1027; _temp1027.tag= Cyc_Absyn_ArrayType;
_temp1027.f1=( void*) res; _temp1027.f2= Cyc_Absyn_empty_tqual(); _temp1027.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1027;}); _temp1026;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1017= Cyc_Absynpp_typ2string( res);
xprintf("elements of array do not all have the same type (%.*s)", _get_arr_size(
_temp1017, 1u), _temp1017.curr);}));}{ int offset= 0; for( 0; des != 0;( offset
++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){ struct Cyc_List_List*
ds=(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1;
if( ds != 0){ void* _temp1018=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct Cyc_Absyn_Exp* _temp1024; _LL1020: if(*(( int*) _temp1018) == Cyc_Absyn_FieldName){
goto _LL1021;} else{ goto _LL1022;} _LL1022: if(*(( int*) _temp1018) == Cyc_Absyn_ArrayElement){
_LL1025: _temp1024=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1018)->f1;
goto _LL1023;} else{ goto _LL1019;} _LL1021: Cyc_Tcutil_terr( loc, _tag_arr("only array index designators are supported",
sizeof( unsigned char), 43u)); goto _LL1019; _LL1023: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp1024);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1024);
if( i != offset){ Cyc_Tcutil_terr( _temp1024->loc,( struct _tagged_arr) xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1019;} _LL1019:;}}} return res_t2;}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1033= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v); xprintf("expecting unsigned int, found %.*s",
_get_arr_size( _temp1033, 1u), _temp1033.curr);}));} if( !( vd->tq).q_const){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u));} if( te->le != 0){ te= Cyc_Tcenv_new_block( loc,
te); te= Cyc_Tcenv_add_local_var( loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp(
te, bound)){ Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant",
sizeof( unsigned char), 22u));} if( ! Cyc_Tcutil_is_const_exp( te, body)){ Cyc_Tcutil_terr(
bound->loc, _tag_arr("body is not constant", sizeof( unsigned char), 21u));}}{
struct Cyc_Absyn_PtrInfo pinfo; void** _temp1034= 0; struct Cyc_Absyn_Tqual*
_temp1035= 0; if( topt != 0){ void* _temp1036= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1044; struct Cyc_Absyn_Exp*
_temp1046; struct Cyc_Absyn_Tqual _temp1048; struct Cyc_Absyn_Tqual* _temp1050;
void* _temp1051; void** _temp1053; _LL1038: if(( unsigned int) _temp1036 > 4u?*((
int*) _temp1036) == Cyc_Absyn_PointerType: 0){ _LL1045: _temp1044=(( struct Cyc_Absyn_PointerType_struct*)
_temp1036)->f1; goto _LL1039;} else{ goto _LL1040;} _LL1040: if(( unsigned int)
_temp1036 > 4u?*(( int*) _temp1036) == Cyc_Absyn_ArrayType: 0){ _LL1052:
_temp1051=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1036)->f1;
_temp1053=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1036)->f1); goto
_LL1049; _LL1049: _temp1048=(( struct Cyc_Absyn_ArrayType_struct*) _temp1036)->f2;
_temp1050=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1036)->f2; goto _LL1047;
_LL1047: _temp1046=(( struct Cyc_Absyn_ArrayType_struct*) _temp1036)->f3; goto
_LL1041;} else{ goto _LL1042;} _LL1042: goto _LL1043; _LL1039: pinfo= _temp1044;
_temp1034=( void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1035=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1037; _LL1041: _temp1034=( void**) _temp1053; _temp1035=(
struct Cyc_Absyn_Tqual*) _temp1050; goto _LL1037; _LL1043: goto _LL1037; _LL1037:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1034, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1054=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1054[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1055; _temp1055.tag= Cyc_Absyn_ArrayType;
_temp1055.f1=( void*) t; _temp1055.f2= _temp1035 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1035)); _temp1055.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1055;}); _temp1054;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1056; _push_handler(&
_temp1056);{ int _temp1058= 0; if( setjmp( _temp1056.handler)){ _temp1058= 1;}
if( ! _temp1058){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1057=( void*) _exn_thrown; void* _temp1060= _temp1057; _LL1062:
if( _temp1060 == Cyc_Dict_Absent){ goto _LL1063;} else{ goto _LL1064;} _LL1064:
goto _LL1065; _LL1063: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1066= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_get_arr_size( _temp1066, 1u), _temp1066.curr);})); return topt != 0?*(( void**)
_check_null( topt)):( void*) Cyc_Absyn_VoidType; _LL1065:( void) _throw(
_temp1060); _LL1061:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!= 0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1067= _new_region(); struct _RegionHandle* rgn=&
_temp1067; _push_region( rgn);{ struct _tuple4 _temp1068=({ struct _tuple4
_temp1090; _temp1090.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1090.f2= rgn;
_temp1090;}); struct Cyc_List_List* _temp1069=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1068, sd->tvs); struct Cyc_List_List* _temp1070=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1069); struct Cyc_Absyn_StructType_struct*
_temp1071=({ struct Cyc_Absyn_StructType_struct* _temp1087=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1087[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1088; _temp1088.tag= Cyc_Absyn_StructType;
_temp1088.f1=( struct _tuple1*)* tn; _temp1088.f2= _temp1070; _temp1088.f3=({
struct Cyc_Absyn_Structdecl** _temp1089=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1089[ 0]= sd; _temp1089;});
_temp1088;}); _temp1087;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) _temp1071,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1072=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1072->v=( void*)
_temp1070; _temp1072;}); if( sd->fields == 0){ Cyc_Tcutil_terr( loc, _tag_arr("can't build abstract struct",
sizeof( unsigned char), 28u));{ void* _temp1073=( void*) _temp1071;
_npop_handler( 0u); return _temp1073;}}{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1076; struct Cyc_Absyn_Exp* _temp1077; struct Cyc_Absyn_Structfield*
_temp1079; struct _tuple9* _temp1074=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1076=* _temp1074; _LL1080: _temp1079= _temp1076.f1;
goto _LL1078; _LL1078: _temp1077= _temp1076.f2; goto _LL1075; _LL1075: { void*
_temp1081= Cyc_Tcutil_rsubstitute( rgn, _temp1069,( void*) _temp1079->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1081, _temp1077); if( ! Cyc_Tcutil_coerce_arg( te, _temp1077,
_temp1081)){ Cyc_Tcutil_terr( _temp1077->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1082=* _temp1079->name; struct _tagged_arr _temp1083= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_arr _temp1084= Cyc_Absynpp_typ2string( _temp1081); struct
_tagged_arr _temp1085= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1077->topt))->v); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_get_arr_size( _temp1082, 1u), _temp1082.curr, _get_arr_size( _temp1083, 1u),
_temp1083.curr, _get_arr_size( _temp1084, 1u), _temp1084.curr, _get_arr_size(
_temp1085, 1u), _temp1085.curr);}));}}}{ void* _temp1086=( void*) _temp1071;
_npop_handler( 0u); return _temp1086;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1091= _new_region(); struct
_RegionHandle* rgn=& _temp1091; _push_region( rgn);{ void* _temp1092= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1098; _LL1094: if(( unsigned int) _temp1092 > 4u?*((
int*) _temp1092) == Cyc_Absyn_AnonStructType: 0){ _LL1099: _temp1098=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1092)->f1; goto _LL1095;} else{ goto
_LL1096;} _LL1096: goto _LL1097; _LL1095: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1098); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1102; struct Cyc_Absyn_Exp*
_temp1103; struct Cyc_Absyn_Structfield* _temp1105; struct _tuple9* _temp1100=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1102=*
_temp1100; _LL1106: _temp1105= _temp1102.f1; goto _LL1104; _LL1104: _temp1103=
_temp1102.f2; goto _LL1101; _LL1101: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1105->type)), _temp1103); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1103,( void*) _temp1105->type)){ Cyc_Tcutil_terr( _temp1103->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1107=* _temp1105->name; struct
_tagged_arr _temp1108= Cyc_Absynpp_typ2string(( void*) _temp1105->type); struct
_tagged_arr _temp1109= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1103->topt))->v); xprintf("field %.*s of struct expects type %.*s != %.*s",
_get_arr_size( _temp1107, 1u), _temp1107.curr, _get_arr_size( _temp1108, 1u),
_temp1108.curr, _get_arr_size( _temp1109, 1u), _temp1109.curr);}));}} goto
_LL1093;} _LL1097:(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u)); goto _LL1093; _LL1093:;}; _pop_region( rgn);}
return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Core_Opt**
all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl*
tud, struct Cyc_Absyn_Tunionfield* tuf){ struct _RegionHandle _temp1110=
_new_region(); struct _RegionHandle* rgn=& _temp1110; _push_region( rgn);{
struct _tuple4 _temp1111=({ struct _tuple4 _temp1153; _temp1153.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1153.f2= rgn; _temp1153;}); struct Cyc_List_List* _temp1112=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1111, tud->tvs); struct Cyc_List_List*
_temp1113=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(*
f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp1111, tuf->tvs);
struct Cyc_List_List* _temp1114=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp1112); struct Cyc_List_List* _temp1115=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1113); struct Cyc_List_List* _temp1116=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1112, _temp1113);* all_ref=({
struct Cyc_Core_Opt* _temp1117=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1117->v=( void*) _temp1114; _temp1117;});* exist_ref=({
struct Cyc_Core_Opt* _temp1118=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1118->v=( void*) _temp1115; _temp1118;});{ void* res=( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1148=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1148[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1149; _temp1149.tag= Cyc_Absyn_TunionFieldType;
_temp1149.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1150; _temp1150.field_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp1151=( struct
Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct));
_temp1151[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct _temp1152; _temp1152.tag=
Cyc_Absyn_KnownTunionfield; _temp1152.f1= tud; _temp1152.f2= tuf; _temp1152;});
_temp1151;})); _temp1150.targs= _temp1114; _temp1150;}); _temp1149;}); _temp1148;});
if( topt != 0){ void* _temp1119= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_TunionInfo _temp1127; void* _temp1129; struct Cyc_List_List*
_temp1131; _LL1121: if(( unsigned int) _temp1119 > 4u?*(( int*) _temp1119) ==
Cyc_Absyn_TunionFieldType: 0){ goto _LL1122;} else{ goto _LL1123;} _LL1123: if((
unsigned int) _temp1119 > 4u?*(( int*) _temp1119) == Cyc_Absyn_TunionType: 0){
_LL1128: _temp1127=(( struct Cyc_Absyn_TunionType_struct*) _temp1119)->f1;
_LL1132: _temp1131= _temp1127.targs; goto _LL1130; _LL1130: _temp1129=( void*)
_temp1127.rgn; goto _LL1124;} else{ goto _LL1125;} _LL1125: goto _LL1126;
_LL1122: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res); goto _LL1120;
_LL1124:{ struct Cyc_List_List* a= _temp1114; for( 0; a != 0? _temp1131 != 0: 0;(
a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1131=(( struct Cyc_List_List*)
_check_null( _temp1131))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( a))->hd,( void*)(( struct Cyc_List_List*) _check_null( _temp1131))->hd);}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1133=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1133->v=(
void*) res; _temp1133;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1134=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1134[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1135; _temp1135.tag=
Cyc_Absyn_TunionType; _temp1135.f1=({ struct Cyc_Absyn_TunionInfo _temp1136;
_temp1136.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1137=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1137[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1138; _temp1138.tag=
Cyc_Absyn_KnownTunion; _temp1138.f1= tud; _temp1138;}); _temp1137;})); _temp1136.targs=
_temp1114; _temp1136.rgn=( void*) _temp1129; _temp1136;}); _temp1135;});
_temp1134;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1139= res;
_npop_handler( 0u); return _temp1139;}} goto _LL1120; _LL1126: goto _LL1120;
_LL1120:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0;(
es=(( struct Cyc_List_List*) _check_null( es))->tl, ts=(( struct Cyc_List_List*)
_check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; void* t= Cyc_Tcutil_rsubstitute(
rgn, _temp1116,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( ts))->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1140= Cyc_Absynpp_qvar2string( tuf->name); struct _tagged_arr _temp1141=
Cyc_Absynpp_typ2string( t); struct _tagged_arr _temp1142= e->topt == 0? _tag_arr("?",
sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_get_arr_size( _temp1140, 1u), _temp1140.curr, _get_arr_size( _temp1141, 1u),
_temp1141.curr, _get_arr_size( _temp1142, 1u), _temp1142.curr);}));}} if( es !=
0){ void* _temp1144= Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp1143= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_get_arr_size( _temp1143, 1u), _temp1143.curr);})); _npop_handler( 0u); return
_temp1144;} if( ts != 0){ void* _temp1146= Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr _temp1145= Cyc_Absynpp_qvar2string( tuf->name);
xprintf("too few arguments for tunion constructor %.*s", _get_arr_size(
_temp1145, 1u), _temp1145.curr);})); _npop_handler( 0u); return _temp1146;}{
void* _temp1147= res; _npop_handler( 0u); return _temp1147;}}}}; _pop_region(
rgn);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( ropt != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1163=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1163[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1164; _temp1164.tag= Cyc_Absyn_RgnHandleType; _temp1164.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1165=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1165->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1165;})); _temp1164;}); _temp1163;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1154= Cyc_Tcutil_compress( handle_type); void*
_temp1160; _LL1156: if(( unsigned int) _temp1154 > 4u?*(( int*) _temp1154) ==
Cyc_Absyn_RgnHandleType: 0){ _LL1161: _temp1160=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1154)->f1; goto _LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159;
_LL1157: rgn= _temp1160; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1155; _LL1159: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null( ropt))->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp1162= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _get_arr_size(
_temp1162, 1u), _temp1162.curr);})); goto _LL1155; _LL1155:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ void*
_temp1166= t; struct Cyc_Absyn_TunionFieldInfo _temp1172; void* _temp1174;
struct Cyc_Absyn_Tunionfield* _temp1176; _LL1168: if(( unsigned int) _temp1166 >
4u?*(( int*) _temp1166) == Cyc_Absyn_TunionFieldType: 0){ _LL1173: _temp1172=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1166)->f1; _LL1175: _temp1174=(
void*) _temp1172.field_info; if(*(( int*) _temp1174) == Cyc_Absyn_KnownTunionfield){
_LL1177: _temp1176=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1174)->f2;
goto _LL1169;} else{ goto _LL1170;}} else{ goto _LL1170;} _LL1170: goto _LL1171;
_LL1169: if( _temp1176->tvs != 0){ Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u));} goto _LL1167; _LL1171: goto _LL1167; _LL1167:;}{
void*(* _temp1178)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp1179= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1185; struct Cyc_Absyn_Conref* _temp1187;
_LL1181: if(( unsigned int) _temp1179 > 4u?*(( int*) _temp1179) == Cyc_Absyn_PointerType:
0){ _LL1186: _temp1185=(( struct Cyc_Absyn_PointerType_struct*) _temp1179)->f1;
_LL1188: _temp1187= _temp1185.nullable; goto _LL1182;} else{ goto _LL1183;}
_LL1183: goto _LL1184; _LL1182: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1187)){ _temp1178= Cyc_Absyn_star_typ;} goto _LL1180; _LL1184: goto _LL1180;
_LL1180:;} return _temp1178( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1189=( void*) s->r; struct Cyc_Absyn_Exp* _temp1199;
struct Cyc_Absyn_Stmt* _temp1201; struct Cyc_Absyn_Stmt* _temp1203; struct Cyc_Absyn_Stmt*
_temp1205; struct Cyc_Absyn_Decl* _temp1207; _LL1191: if(( unsigned int)
_temp1189 > 1u?*(( int*) _temp1189) == Cyc_Absyn_Exp_s: 0){ _LL1200: _temp1199=((
struct Cyc_Absyn_Exp_s_struct*) _temp1189)->f1; goto _LL1192;} else{ goto
_LL1193;} _LL1193: if(( unsigned int) _temp1189 > 1u?*(( int*) _temp1189) == Cyc_Absyn_Seq_s:
0){ _LL1204: _temp1203=(( struct Cyc_Absyn_Seq_s_struct*) _temp1189)->f1; goto
_LL1202; _LL1202: _temp1201=(( struct Cyc_Absyn_Seq_s_struct*) _temp1189)->f2;
goto _LL1194;} else{ goto _LL1195;} _LL1195: if(( unsigned int) _temp1189 > 1u?*((
int*) _temp1189) == Cyc_Absyn_Decl_s: 0){ _LL1208: _temp1207=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1189)->f1; goto _LL1206; _LL1206: _temp1205=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1189)->f2; goto _LL1196;} else{ goto _LL1197;} _LL1197: goto _LL1198;
_LL1192: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1199->topt))->v;
_LL1194: s= _temp1201; continue; _LL1196: s= _temp1205; continue; _LL1198:
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u)); _LL1190:;}} static void* Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u));}
static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle
!= 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1218=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1218[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1219; _temp1219.tag= Cyc_Absyn_RgnHandleType; _temp1219.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1220=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1220->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1220;})); _temp1219;}); _temp1218;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle)); void* _temp1209= Cyc_Tcutil_compress( handle_type);
void* _temp1215; _LL1211: if(( unsigned int) _temp1209 > 4u?*(( int*) _temp1209)
== Cyc_Absyn_RgnHandleType: 0){ _LL1216: _temp1215=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1209)->f1; goto _LL1212;} else{ goto _LL1213;} _LL1213: goto _LL1214;
_LL1212: rgn= _temp1215; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1210; _LL1214: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null(
rgn_handle))->loc,( struct _tagged_arr)({ struct _tagged_arr _temp1217= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _get_arr_size(
_temp1217, 1u), _temp1217.curr);})); goto _LL1210; _LL1210:;}{ void* _temp1221=(
void*) e1->r; struct Cyc_Absyn_Exp* _temp1233; struct Cyc_Absyn_Exp* _temp1235;
struct Cyc_Absyn_Vardecl* _temp1237; struct Cyc_List_List* _temp1239; struct Cyc_Core_Opt*
_temp1241; struct Cyc_List_List* _temp1243; void* _temp1245; struct _tagged_arr
_temp1247; _LL1223: if(*(( int*) _temp1221) == Cyc_Absyn_Comprehension_e){
_LL1238: _temp1237=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1221)->f1;
goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1221)->f2; goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1221)->f3; goto _LL1224;} else{ goto _LL1225;} _LL1225: if(*(( int*)
_temp1221) == Cyc_Absyn_UnresolvedMem_e){ _LL1242: _temp1241=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1221)->f1; goto _LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1221)->f2; goto _LL1226;} else{ goto _LL1227;} _LL1227: if(*(( int*)
_temp1221) == Cyc_Absyn_Array_e){ _LL1244: _temp1243=(( struct Cyc_Absyn_Array_e_struct*)
_temp1221)->f1; goto _LL1228;} else{ goto _LL1229;} _LL1229: if(*(( int*)
_temp1221) == Cyc_Absyn_Const_e){ _LL1246: _temp1245=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1221)->f1; if(( unsigned int) _temp1245 > 1u?*(( int*) _temp1245) == Cyc_Absyn_String_c:
0){ _LL1248: _temp1247=(( struct Cyc_Absyn_String_c_struct*) _temp1245)->f1;
goto _LL1230;} else{ goto _LL1231;}} else{ goto _LL1231;} _LL1231: goto _LL1232;
_LL1224: { void* _temp1249= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1250= Cyc_Tcutil_compress( _temp1249); struct Cyc_Absyn_Tqual _temp1256;
void* _temp1258; _LL1252: if(( unsigned int) _temp1250 > 4u?*(( int*) _temp1250)
== Cyc_Absyn_ArrayType: 0){ _LL1259: _temp1258=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1250)->f1; goto _LL1257; _LL1257: _temp1256=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1250)->f2; goto _LL1253;} else{ goto _LL1254;} _LL1254: goto _LL1255;
_LL1253: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1235)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1264=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1264[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1265; _temp1265.tag= Cyc_Absyn_Upper_b; _temp1265.f1=
_temp1235; _temp1265;}); _temp1264;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1261=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1261[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1262; _temp1262.tag= Cyc_Absyn_PointerType;
_temp1262.f1=({ struct Cyc_Absyn_PtrInfo _temp1263; _temp1263.elt_typ=( void*)
_temp1258; _temp1263.rgn_typ=( void*) rgn; _temp1263.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1263.tq= _temp1256; _temp1263.bounds= Cyc_Absyn_new_conref(
b); _temp1263;}); _temp1262;}); _temp1261;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1260=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1260->v=( void*) res_typ; _temp1260;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return
res_typ;} _LL1255: return Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u)); _LL1251:;} _LL1226:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1266=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1266[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1267; _temp1267.tag= Cyc_Absyn_Array_e; _temp1267.f1=
_temp1239; _temp1267;}); _temp1266;}))); _temp1243= _temp1239; goto _LL1228;
_LL1228: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1268= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1274; struct Cyc_Absyn_Tqual
_temp1276; void* _temp1278; void** _temp1280; _LL1270: if(( unsigned int)
_temp1268 > 4u?*(( int*) _temp1268) == Cyc_Absyn_PointerType: 0){ _LL1275:
_temp1274=(( struct Cyc_Absyn_PointerType_struct*) _temp1268)->f1; _LL1279:
_temp1278=( void*) _temp1274.elt_typ; _temp1280=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1268)->f1).elt_typ; goto _LL1277; _LL1277: _temp1276= _temp1274.tq; goto
_LL1271;} else{ goto _LL1272;} _LL1272: goto _LL1273; _LL1271: elt_typ_opt=(
void**) _temp1280; goto _LL1269; _LL1273: goto _LL1269; _LL1269:;}{ void*
_temp1281= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1282= Cyc_Tcutil_compress( _temp1281); struct Cyc_Absyn_Exp* _temp1288;
struct Cyc_Absyn_Tqual _temp1290; void* _temp1292; _LL1284: if(( unsigned int)
_temp1282 > 4u?*(( int*) _temp1282) == Cyc_Absyn_ArrayType: 0){ _LL1293:
_temp1292=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1282)->f1; goto
_LL1291; _LL1291: _temp1290=(( struct Cyc_Absyn_ArrayType_struct*) _temp1282)->f2;
goto _LL1289; _LL1289: _temp1288=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1282)->f3; goto _LL1285;} else{ goto _LL1286;} _LL1286: goto _LL1287;
_LL1285: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1294=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1294[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1295; _temp1295.tag=
Cyc_Absyn_PointerType; _temp1295.f1=({ struct Cyc_Absyn_PtrInfo _temp1296;
_temp1296.elt_typ=( void*) _temp1292; _temp1296.rgn_typ=( void*) rgn; _temp1296.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1296.tq=
_temp1290; _temp1296.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1297=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1297[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1298; _temp1298.tag= Cyc_Absyn_Upper_b;
_temp1298.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1288); _temp1298;});
_temp1297;})); _temp1296;}); _temp1295;}); _temp1294;}); if( topt != 0){ if( !
Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1299=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1299->v=( void*) res_typ; _temp1299;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1283; _LL1287: return Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u)); _LL1283:;} return res_typ;}} _LL1230: { void*
_temp1300= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),(
void*) Cyc_Absyn_Unknown_b); void* _temp1301= Cyc_Tcexp_tcExp( te,( void**)&
_temp1300, e1); return Cyc_Absyn_atb_typ( _temp1301, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1302=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1302[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1303; _temp1303.tag= Cyc_Absyn_Upper_b; _temp1303.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1303;}); _temp1302;}));} _LL1232: { void** topt2=
0; if( topt != 0){ void* _temp1304= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1312; struct Cyc_Absyn_Conref* _temp1314;
void* _temp1316; void** _temp1318; _LL1306: if(( unsigned int) _temp1304 > 4u?*((
int*) _temp1304) == Cyc_Absyn_PointerType: 0){ _LL1313: _temp1312=(( struct Cyc_Absyn_PointerType_struct*)
_temp1304)->f1; _LL1317: _temp1316=( void*) _temp1312.elt_typ; _temp1318=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1304)->f1).elt_typ; goto _LL1315;
_LL1315: _temp1314= _temp1312.nullable; goto _LL1307;} else{ goto _LL1308;}
_LL1308: if(( unsigned int) _temp1304 > 4u?*(( int*) _temp1304) == Cyc_Absyn_TunionType:
0){ goto _LL1309;} else{ goto _LL1310;} _LL1310: goto _LL1311; _LL1307: topt2=(
void**) _temp1318; goto _LL1305; _LL1309: topt2=({ void** _temp1319=( void**)
GC_malloc( sizeof( void*)); _temp1319[ 0]=*(( void**) _check_null( topt));
_temp1319;}); goto _LL1305; _LL1311: goto _LL1305; _LL1305:;}{ void* _temp1320=
Cyc_Tcexp_tcExp( te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1322=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1322[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1323; _temp1323.tag=
Cyc_Absyn_PointerType; _temp1323.f1=({ struct Cyc_Absyn_PtrInfo _temp1324;
_temp1324.elt_typ=( void*) _temp1320; _temp1324.rgn_typ=( void*) rgn; _temp1324.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1324.tq= Cyc_Absyn_empty_tqual();
_temp1324.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1325=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1325[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1326; _temp1326.tag= Cyc_Absyn_Upper_b;
_temp1326.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1326;}); _temp1325;})); _temp1324;});
_temp1323;}); _temp1322;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1321=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1321->v=(
void*) res_typ; _temp1321;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}}
_LL1222:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t=
Cyc_Tcutil_compress( t);{ void* _temp1327= t; struct Cyc_Absyn_Exp* _temp1333;
struct Cyc_Absyn_Tqual _temp1335; void* _temp1337; _LL1329: if(( unsigned int)
_temp1327 > 4u?*(( int*) _temp1327) == Cyc_Absyn_ArrayType: 0){ _LL1338:
_temp1337=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1327)->f1; goto
_LL1336; _LL1336: _temp1335=(( struct Cyc_Absyn_ArrayType_struct*) _temp1327)->f2;
goto _LL1334; _LL1334: _temp1333=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1327)->f3; goto _LL1330;} else{ goto _LL1331;} _LL1331: goto _LL1332;
_LL1330: { void* _temp1341; struct _tuple6 _temp1339= Cyc_Tcutil_addressof_props(
te, e); _LL1342: _temp1341= _temp1339.f2; goto _LL1340; _LL1340: { void*
_temp1343= _temp1333 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1344=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1344[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1345; _temp1345.tag= Cyc_Absyn_Upper_b;
_temp1345.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1333); _temp1345;});
_temp1344;}); t= Cyc_Absyn_atb_typ( _temp1337, _temp1341, _temp1335, _temp1343);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1332: return t; _LL1328:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1346=( void*) e->r; void* _temp1356; _LL1348: if(*(( int*)
_temp1346) == Cyc_Absyn_Array_e){ goto _LL1349;} else{ goto _LL1350;} _LL1350:
if(*(( int*) _temp1346) == Cyc_Absyn_Comprehension_e){ goto _LL1351;} else{ goto
_LL1352;} _LL1352: if(*(( int*) _temp1346) == Cyc_Absyn_Const_e){ _LL1357:
_temp1356=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1346)->f1; if((
unsigned int) _temp1356 > 1u?*(( int*) _temp1356) == Cyc_Absyn_String_c: 0){
goto _LL1353;} else{ goto _LL1354;}} else{ goto _LL1354;} _LL1354: goto _LL1355;
_LL1349: return t; _LL1351: return t; _LL1353: return t; _LL1355: t= Cyc_Tcutil_compress(
t);{ void* _temp1358= t; struct Cyc_Absyn_Exp* _temp1364; struct Cyc_Absyn_Tqual
_temp1366; void* _temp1368; _LL1360: if(( unsigned int) _temp1358 > 4u?*(( int*)
_temp1358) == Cyc_Absyn_ArrayType: 0){ _LL1369: _temp1368=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1358)->f1; goto _LL1367; _LL1367: _temp1366=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1358)->f2; goto _LL1365; _LL1365: _temp1364=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1358)->f3; goto _LL1361;} else{ goto _LL1362;} _LL1362: goto _LL1363;
_LL1361: { void* _temp1372; struct _tuple6 _temp1370= Cyc_Tcutil_addressof_props(
te, e); _LL1373: _temp1372= _temp1370.f2; goto _LL1371; _LL1371: { void* b=
_temp1364 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1374=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1374[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1375; _temp1375.tag= Cyc_Absyn_Upper_b;
_temp1375.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1364); _temp1375;});
_temp1374;}); t= Cyc_Absyn_atb_typ( _temp1368, _temp1372, _temp1366, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1363: return t; _LL1359:;} _LL1347:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1376=( void*) e->r; struct Cyc_Absyn_Exp* _temp1382; _LL1378: if(*(( int*)
_temp1376) == Cyc_Absyn_NoInstantiate_e){ _LL1383: _temp1382=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1376)->f1; goto _LL1379;} else{ goto _LL1380;} _LL1380: goto _LL1381;
_LL1379: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1382);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1382->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1382->topt))->v))); e->topt=
_temp1382->topt; goto _LL1377; _LL1381: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1384= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1390; struct Cyc_Absyn_Conref*
_temp1392; struct Cyc_Absyn_Tqual _temp1394; struct Cyc_Absyn_Conref* _temp1396;
void* _temp1398; void* _temp1400; _LL1386: if(( unsigned int) _temp1384 > 4u?*((
int*) _temp1384) == Cyc_Absyn_PointerType: 0){ _LL1391: _temp1390=(( struct Cyc_Absyn_PointerType_struct*)
_temp1384)->f1; _LL1401: _temp1400=( void*) _temp1390.elt_typ; goto _LL1399;
_LL1399: _temp1398=( void*) _temp1390.rgn_typ; goto _LL1397; _LL1397: _temp1396=
_temp1390.nullable; goto _LL1395; _LL1395: _temp1394= _temp1390.tq; goto _LL1393;
_LL1393: _temp1392= _temp1390.bounds; goto _LL1387;} else{ goto _LL1388;}
_LL1388: goto _LL1389; _LL1387:{ void* _temp1402= Cyc_Tcutil_compress( _temp1400);
struct Cyc_Absyn_FnInfo _temp1408; struct Cyc_List_List* _temp1410; struct Cyc_List_List*
_temp1412; struct Cyc_Absyn_VarargInfo* _temp1414; int _temp1416; struct Cyc_List_List*
_temp1418; void* _temp1420; struct Cyc_Core_Opt* _temp1422; struct Cyc_List_List*
_temp1424; _LL1404: if(( unsigned int) _temp1402 > 4u?*(( int*) _temp1402) ==
Cyc_Absyn_FnType: 0){ _LL1409: _temp1408=(( struct Cyc_Absyn_FnType_struct*)
_temp1402)->f1; _LL1425: _temp1424= _temp1408.tvars; goto _LL1423; _LL1423:
_temp1422= _temp1408.effect; goto _LL1421; _LL1421: _temp1420=( void*) _temp1408.ret_typ;
goto _LL1419; _LL1419: _temp1418= _temp1408.args; goto _LL1417; _LL1417:
_temp1416= _temp1408.c_varargs; goto _LL1415; _LL1415: _temp1414= _temp1408.cyc_varargs;
goto _LL1413; _LL1413: _temp1412= _temp1408.rgn_po; goto _LL1411; _LL1411:
_temp1410= _temp1408.attributes; goto _LL1405;} else{ goto _LL1406;} _LL1406:
goto _LL1407; _LL1405: if( _temp1424 != 0){ struct _RegionHandle _temp1426=
_new_region(); struct _RegionHandle* rgn=& _temp1426; _push_region( rgn);{
struct _tuple4 _temp1427=({ struct _tuple4 _temp1437; _temp1437.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1437.f2= rgn; _temp1437;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1427, _temp1424); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1434=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1434[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1435; _temp1435.tag= Cyc_Absyn_FnType; _temp1435.f1=({ struct Cyc_Absyn_FnInfo
_temp1436; _temp1436.tvars= 0; _temp1436.effect= _temp1422; _temp1436.ret_typ=(
void*) _temp1420; _temp1436.args= _temp1418; _temp1436.c_varargs= _temp1416;
_temp1436.cyc_varargs= _temp1414; _temp1436.rgn_po= _temp1412; _temp1436.attributes=
_temp1410; _temp1436;}); _temp1435;}); _temp1434;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1431=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1431[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1432; _temp1432.tag= Cyc_Absyn_PointerType;
_temp1432.f1=({ struct Cyc_Absyn_PtrInfo _temp1433; _temp1433.elt_typ=( void*)
ftyp; _temp1433.rgn_typ=( void*) _temp1398; _temp1433.nullable= _temp1396;
_temp1433.tq= _temp1394; _temp1433.bounds= _temp1392; _temp1433;}); _temp1432;});
_temp1431;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1428=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1428[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1429; _temp1429.tag= Cyc_Absyn_Instantiate_e;
_temp1429.f1= inner; _temp1429.f2= ts; _temp1429;}); _temp1428;}))); e->topt=({
struct Cyc_Core_Opt* _temp1430=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1430->v=( void*) new_typ; _temp1430;});}; _pop_region( rgn);}
goto _LL1403; _LL1407: goto _LL1403; _LL1403:;} goto _LL1385; _LL1389: goto
_LL1385; _LL1385:;} goto _LL1377; _LL1377:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1438=( void*) e->r; struct Cyc_Absyn_Exp* _temp1512; struct
_tuple1* _temp1514; struct Cyc_List_List* _temp1516; struct Cyc_Absyn_Exp*
_temp1518; struct Cyc_List_List* _temp1520; struct Cyc_Core_Opt* _temp1522; void*
_temp1524; void* _temp1526; struct _tuple1* _temp1528; struct Cyc_List_List*
_temp1530; void* _temp1532; void* _temp1534; struct Cyc_Absyn_Exp* _temp1536;
struct Cyc_Absyn_Exp* _temp1538; struct Cyc_Core_Opt* _temp1540; struct Cyc_Absyn_Exp*
_temp1542; struct Cyc_Absyn_Exp* _temp1544; struct Cyc_Absyn_Exp* _temp1546;
struct Cyc_Absyn_Exp* _temp1548; struct Cyc_Absyn_Exp* _temp1550; struct Cyc_Absyn_Exp*
_temp1552; struct Cyc_Absyn_VarargCallInfo* _temp1554; struct Cyc_Absyn_VarargCallInfo**
_temp1556; struct Cyc_List_List* _temp1557; struct Cyc_Absyn_Exp* _temp1559;
struct Cyc_Absyn_Exp* _temp1561; struct Cyc_List_List* _temp1563; struct Cyc_Absyn_Exp*
_temp1565; struct Cyc_Absyn_Exp* _temp1567; void* _temp1569; struct Cyc_Absyn_Exp*
_temp1571; struct Cyc_Absyn_Exp* _temp1573; struct Cyc_Absyn_Exp* _temp1575;
struct Cyc_Absyn_Exp* _temp1577; void* _temp1579; struct _tagged_arr* _temp1581;
void* _temp1583; struct Cyc_Absyn_Exp* _temp1585; struct _tagged_arr* _temp1587;
struct Cyc_Absyn_Exp* _temp1589; struct _tagged_arr* _temp1591; struct Cyc_Absyn_Exp*
_temp1593; struct Cyc_Absyn_Exp* _temp1595; struct Cyc_Absyn_Exp* _temp1597;
struct Cyc_List_List* _temp1599; struct Cyc_List_List* _temp1601; struct _tuple2*
_temp1603; struct Cyc_List_List* _temp1605; struct Cyc_Absyn_Stmt* _temp1607;
struct Cyc_Absyn_Fndecl* _temp1609; struct Cyc_Absyn_Exp* _temp1611; struct Cyc_Absyn_Exp*
_temp1613; struct Cyc_Absyn_Exp* _temp1615; struct Cyc_Absyn_Vardecl* _temp1617;
struct Cyc_Absyn_Structdecl* _temp1619; struct Cyc_Absyn_Structdecl** _temp1621;
struct Cyc_List_List* _temp1622; struct Cyc_Core_Opt* _temp1624; struct Cyc_Core_Opt**
_temp1626; struct _tuple1* _temp1627; struct _tuple1** _temp1629; struct Cyc_List_List*
_temp1630; void* _temp1632; struct Cyc_Absyn_Tunionfield* _temp1634; struct Cyc_Absyn_Tuniondecl*
_temp1636; struct Cyc_List_List* _temp1638; struct Cyc_Core_Opt* _temp1640;
struct Cyc_Core_Opt** _temp1642; struct Cyc_Core_Opt* _temp1643; struct Cyc_Core_Opt**
_temp1645; struct Cyc_Absyn_Enumfield* _temp1646; struct Cyc_Absyn_Enumdecl*
_temp1648; struct _tuple1* _temp1650; struct _tuple1** _temp1652; void*
_temp1653; struct Cyc_Absyn_Exp* _temp1655; _LL1440: if(*(( int*) _temp1438) ==
Cyc_Absyn_NoInstantiate_e){ _LL1513: _temp1512=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1438)->f1; goto _LL1441;} else{ goto _LL1442;} _LL1442: if(*(( int*)
_temp1438) == Cyc_Absyn_UnknownId_e){ _LL1515: _temp1514=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1438)->f1; goto _LL1443;} else{ goto _LL1444;} _LL1444: if(*(( int*)
_temp1438) == Cyc_Absyn_UnknownCall_e){ _LL1519: _temp1518=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1438)->f1; goto _LL1517; _LL1517: _temp1516=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1438)->f2; goto _LL1445;} else{ goto _LL1446;} _LL1446: if(*(( int*)
_temp1438) == Cyc_Absyn_UnresolvedMem_e){ _LL1523: _temp1522=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1438)->f1; goto _LL1521; _LL1521: _temp1520=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1438)->f2; goto _LL1447;} else{ goto _LL1448;} _LL1448: if(*(( int*)
_temp1438) == Cyc_Absyn_Const_e){ _LL1525: _temp1524=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1438)->f1; goto _LL1449;} else{ goto _LL1450;} _LL1450: if(*(( int*)
_temp1438) == Cyc_Absyn_Var_e){ _LL1529: _temp1528=(( struct Cyc_Absyn_Var_e_struct*)
_temp1438)->f1; goto _LL1527; _LL1527: _temp1526=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1438)->f2; goto _LL1451;} else{ goto _LL1452;} _LL1452: if(*(( int*)
_temp1438) == Cyc_Absyn_Primop_e){ _LL1533: _temp1532=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1438)->f1; goto _LL1531; _LL1531: _temp1530=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1438)->f2; goto _LL1453;} else{ goto _LL1454;} _LL1454: if(*(( int*)
_temp1438) == Cyc_Absyn_Increment_e){ _LL1537: _temp1536=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1438)->f1; goto _LL1535; _LL1535: _temp1534=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1438)->f2; goto _LL1455;} else{ goto _LL1456;} _LL1456: if(*(( int*)
_temp1438) == Cyc_Absyn_AssignOp_e){ _LL1543: _temp1542=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1438)->f1; goto _LL1541; _LL1541: _temp1540=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1438)->f2; goto _LL1539; _LL1539: _temp1538=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1438)->f3; goto _LL1457;} else{ goto _LL1458;} _LL1458: if(*(( int*)
_temp1438) == Cyc_Absyn_Conditional_e){ _LL1549: _temp1548=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1438)->f1; goto _LL1547; _LL1547: _temp1546=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1438)->f2; goto _LL1545; _LL1545: _temp1544=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1438)->f3; goto _LL1459;} else{ goto _LL1460;} _LL1460: if(*(( int*)
_temp1438) == Cyc_Absyn_SeqExp_e){ _LL1553: _temp1552=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1438)->f1; goto _LL1551; _LL1551: _temp1550=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1438)->f2; goto _LL1461;} else{ goto _LL1462;} _LL1462: if(*(( int*)
_temp1438) == Cyc_Absyn_FnCall_e){ _LL1560: _temp1559=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1438)->f1; goto _LL1558; _LL1558: _temp1557=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1438)->f2; goto _LL1555; _LL1555: _temp1554=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1438)->f3; _temp1556=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1438)->f3;
goto _LL1463;} else{ goto _LL1464;} _LL1464: if(*(( int*) _temp1438) == Cyc_Absyn_Throw_e){
_LL1562: _temp1561=(( struct Cyc_Absyn_Throw_e_struct*) _temp1438)->f1; goto
_LL1465;} else{ goto _LL1466;} _LL1466: if(*(( int*) _temp1438) == Cyc_Absyn_Instantiate_e){
_LL1566: _temp1565=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1438)->f1;
goto _LL1564; _LL1564: _temp1563=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1438)->f2; goto _LL1467;} else{ goto _LL1468;} _LL1468: if(*(( int*)
_temp1438) == Cyc_Absyn_Cast_e){ _LL1570: _temp1569=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1438)->f1; goto _LL1568; _LL1568: _temp1567=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1438)->f2; goto _LL1469;} else{ goto _LL1470;} _LL1470: if(*(( int*)
_temp1438) == Cyc_Absyn_Address_e){ _LL1572: _temp1571=(( struct Cyc_Absyn_Address_e_struct*)
_temp1438)->f1; goto _LL1471;} else{ goto _LL1472;} _LL1472: if(*(( int*)
_temp1438) == Cyc_Absyn_New_e){ _LL1576: _temp1575=(( struct Cyc_Absyn_New_e_struct*)
_temp1438)->f1; goto _LL1574; _LL1574: _temp1573=(( struct Cyc_Absyn_New_e_struct*)
_temp1438)->f2; goto _LL1473;} else{ goto _LL1474;} _LL1474: if(*(( int*)
_temp1438) == Cyc_Absyn_Sizeofexp_e){ _LL1578: _temp1577=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1438)->f1; goto _LL1475;} else{ goto _LL1476;} _LL1476: if(*(( int*)
_temp1438) == Cyc_Absyn_Sizeoftyp_e){ _LL1580: _temp1579=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1438)->f1; goto _LL1477;} else{ goto _LL1478;} _LL1478: if(*(( int*)
_temp1438) == Cyc_Absyn_Offsetof_e){ _LL1584: _temp1583=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1438)->f1; goto _LL1582; _LL1582: _temp1581=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1438)->f2; goto _LL1479;} else{ goto _LL1480;} _LL1480: if(*(( int*)
_temp1438) == Cyc_Absyn_Deref_e){ _LL1586: _temp1585=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1438)->f1; goto _LL1481;} else{ goto _LL1482;} _LL1482: if(*(( int*)
_temp1438) == Cyc_Absyn_StructMember_e){ _LL1590: _temp1589=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1438)->f1; goto _LL1588; _LL1588: _temp1587=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1438)->f2; goto _LL1483;} else{ goto _LL1484;} _LL1484: if(*(( int*)
_temp1438) == Cyc_Absyn_StructArrow_e){ _LL1594: _temp1593=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1438)->f1; goto _LL1592; _LL1592: _temp1591=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1438)->f2; goto _LL1485;} else{ goto _LL1486;} _LL1486: if(*(( int*)
_temp1438) == Cyc_Absyn_Subscript_e){ _LL1598: _temp1597=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1438)->f1; goto _LL1596; _LL1596: _temp1595=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1438)->f2; goto _LL1487;} else{ goto _LL1488;} _LL1488: if(*(( int*)
_temp1438) == Cyc_Absyn_Tuple_e){ _LL1600: _temp1599=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1438)->f1; goto _LL1489;} else{ goto _LL1490;} _LL1490: if(*(( int*)
_temp1438) == Cyc_Absyn_CompoundLit_e){ _LL1604: _temp1603=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1438)->f1; goto _LL1602; _LL1602: _temp1601=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1438)->f2; goto _LL1491;} else{ goto _LL1492;} _LL1492: if(*(( int*)
_temp1438) == Cyc_Absyn_Array_e){ _LL1606: _temp1605=(( struct Cyc_Absyn_Array_e_struct*)
_temp1438)->f1; goto _LL1493;} else{ goto _LL1494;} _LL1494: if(*(( int*)
_temp1438) == Cyc_Absyn_StmtExp_e){ _LL1608: _temp1607=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1438)->f1; goto _LL1495;} else{ goto _LL1496;} _LL1496: if(*(( int*)
_temp1438) == Cyc_Absyn_Codegen_e){ _LL1610: _temp1609=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1438)->f1; goto _LL1497;} else{ goto _LL1498;} _LL1498: if(*(( int*)
_temp1438) == Cyc_Absyn_Fill_e){ _LL1612: _temp1611=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1438)->f1; goto _LL1499;} else{ goto _LL1500;} _LL1500: if(*(( int*)
_temp1438) == Cyc_Absyn_Comprehension_e){ _LL1618: _temp1617=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1438)->f1; goto _LL1616; _LL1616: _temp1615=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1438)->f2; goto _LL1614; _LL1614: _temp1613=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1438)->f3; goto _LL1501;} else{ goto _LL1502;} _LL1502: if(*(( int*)
_temp1438) == Cyc_Absyn_Struct_e){ _LL1628: _temp1627=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1438)->f1; _temp1629=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1438)->f1;
goto _LL1625; _LL1625: _temp1624=(( struct Cyc_Absyn_Struct_e_struct*) _temp1438)->f2;
_temp1626=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1438)->f2; goto _LL1623;
_LL1623: _temp1622=(( struct Cyc_Absyn_Struct_e_struct*) _temp1438)->f3; goto
_LL1620; _LL1620: _temp1619=(( struct Cyc_Absyn_Struct_e_struct*) _temp1438)->f4;
_temp1621=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1438)->f4; goto _LL1503;}
else{ goto _LL1504;} _LL1504: if(*(( int*) _temp1438) == Cyc_Absyn_AnonStruct_e){
_LL1633: _temp1632=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1438)->f1;
goto _LL1631; _LL1631: _temp1630=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1438)->f2; goto _LL1505;} else{ goto _LL1506;} _LL1506: if(*(( int*)
_temp1438) == Cyc_Absyn_Tunion_e){ _LL1644: _temp1643=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1438)->f1; _temp1645=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1438)->f1;
goto _LL1641; _LL1641: _temp1640=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1438)->f2;
_temp1642=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1438)->f2; goto _LL1639;
_LL1639: _temp1638=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1438)->f3; goto
_LL1637; _LL1637: _temp1636=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1438)->f4;
goto _LL1635; _LL1635: _temp1634=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1438)->f5;
goto _LL1507;} else{ goto _LL1508;} _LL1508: if(*(( int*) _temp1438) == Cyc_Absyn_Enum_e){
_LL1651: _temp1650=(( struct Cyc_Absyn_Enum_e_struct*) _temp1438)->f1; _temp1652=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1438)->f1; goto _LL1649; _LL1649:
_temp1648=(( struct Cyc_Absyn_Enum_e_struct*) _temp1438)->f2; goto _LL1647;
_LL1647: _temp1646=(( struct Cyc_Absyn_Enum_e_struct*) _temp1438)->f3; goto
_LL1509;} else{ goto _LL1510;} _LL1510: if(*(( int*) _temp1438) == Cyc_Absyn_Malloc_e){
_LL1656: _temp1655=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1438)->f1; goto
_LL1654; _LL1654: _temp1653=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1438)->f2; goto _LL1511;} else{ goto _LL1439;} _LL1441: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1512); return; _LL1443: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1514); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1445: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1518, _temp1516); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1447: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1520); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1449: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1524, e);
goto _LL1439; _LL1451: t= Cyc_Tcexp_tcVar( te, loc, _temp1528, _temp1526); goto
_LL1439; _LL1453: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1532, _temp1530);
goto _LL1439; _LL1455: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1536,
_temp1534); goto _LL1439; _LL1457: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1542, _temp1540, _temp1538); goto _LL1439; _LL1459: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1548, _temp1546, _temp1544); goto _LL1439; _LL1461: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1552, _temp1550); goto _LL1439; _LL1463: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1559, _temp1557, _temp1556); goto _LL1439; _LL1465: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1561); goto _LL1439; _LL1467: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1565, _temp1563); goto _LL1439; _LL1469: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1569, _temp1567); goto _LL1439; _LL1471: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1571); goto _LL1439; _LL1473: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1575, e, _temp1573); goto _LL1439; _LL1475: { void* _temp1657= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1577); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1657); goto
_LL1439;} _LL1477: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1579); goto
_LL1439; _LL1479: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1583, _temp1581);
goto _LL1439; _LL1481: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1585); goto
_LL1439; _LL1483: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1589,
_temp1587); goto _LL1439; _LL1485: t= Cyc_Tcexp_tcStructArrow( te, loc, topt,
_temp1593, _temp1591); goto _LL1439; _LL1487: t= Cyc_Tcexp_tcSubscript( te, loc,
topt, _temp1597, _temp1595); goto _LL1439; _LL1489: t= Cyc_Tcexp_tcTuple( te,
loc, topt, _temp1599); goto _LL1439; _LL1491: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1603, _temp1601); goto _LL1439; _LL1493: { void** elt_topt= 0;
if( topt != 0){ void* _temp1658= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); void* _temp1664; void** _temp1666; _LL1660: if(( unsigned int)
_temp1658 > 4u?*(( int*) _temp1658) == Cyc_Absyn_ArrayType: 0){ _LL1665:
_temp1664=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1658)->f1;
_temp1666=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1658)->f1); goto
_LL1661;} else{ goto _LL1662;} _LL1662: goto _LL1663; _LL1661: elt_topt=( void**)
_temp1666; goto _LL1659; _LL1663: goto _LL1659; _LL1659:;} t= Cyc_Tcexp_tcArray(
te, loc, elt_topt, _temp1605); goto _LL1439;} _LL1495: t= Cyc_Tcexp_tcStmtExp(
te, loc, topt, _temp1607); goto _LL1439; _LL1497: t= Cyc_Tcexp_tcCodegen( te,
loc, topt, _temp1609); goto _LL1439; _LL1499: t= Cyc_Tcexp_tcFill( te, loc, topt,
_temp1611); goto _LL1439; _LL1501: t= Cyc_Tcexp_tcComprehension( te, loc, topt,
_temp1617, _temp1615, _temp1613); goto _LL1439; _LL1503: t= Cyc_Tcexp_tcStruct(
te, loc, topt, _temp1629, _temp1626, _temp1622, _temp1621); goto _LL1439;
_LL1505: t= Cyc_Tcexp_tcAnonStruct( te, loc, _temp1632, _temp1630); goto _LL1439;
_LL1507: t= Cyc_Tcexp_tcTunion( te, loc, topt, e, _temp1645, _temp1642,
_temp1638, _temp1636, _temp1634); goto _LL1439; _LL1509:* _temp1652=(( struct
Cyc_Absyn_Enumfield*) _check_null( _temp1646))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp1667=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1667[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1668; _temp1668.tag= Cyc_Absyn_EnumType;
_temp1668.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1648))->name;
_temp1668.f2= _temp1648; _temp1668;}); _temp1667;}); goto _LL1439; _LL1511: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1655, _temp1653); goto _LL1439; _LL1439:;}
e->topt=({ struct Cyc_Core_Opt* _temp1669=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1669->v=( void*) t; _temp1669;});}