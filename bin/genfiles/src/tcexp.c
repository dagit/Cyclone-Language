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
Cyc_List_List* x, int i); extern struct Cyc_List_List* Cyc_List_nth_tail( struct
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
Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct
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
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_String_zstrcmp( struct _tagged_arr,
struct _tagged_arr); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*);
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_arr); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_arr); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct
_tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
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
struct _tagged_arr)({ struct _tagged_arr(* _temp30)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp34= _tag_arr("bad occurrence of struct name %s",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp35; _temp35.tag=
Cyc_Stdio_String_pa; _temp35.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{
void* _temp33=( void*)& _temp35; void* _temp31[ 1u]={ _temp33}; struct
_tagged_arr _temp32={( void*) _temp31,( void*) _temp31,( void*)( _temp31 + 1u)};
_temp30( _temp34, _temp32);}}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp36=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp36[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp37; _temp37.tag= Cyc_Absyn_Var_e;
_temp37.f1= q; _temp37.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp37;});
_temp36;}))); goto _LL5; _LL5:;}; _pop_handler();} else{ void* _temp2=( void*)
_exn_thrown; void* _temp39= _temp2; _LL41: if( _temp39 == Cyc_Dict_Absent){ goto
_LL42;} else{ goto _LL43;} _LL43: goto _LL44; _LL42:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp45=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp45[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp46; _temp46.tag= Cyc_Absyn_Var_e; _temp46.f1= q; _temp46.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp46;}); _temp45;}))); goto _LL40; _LL44:( void)
_throw( _temp39); _LL40:;}}} struct _tuple7{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static struct _tuple7* Cyc_Tcexp_make_struct_arg( struct
Cyc_Absyn_Exp* e){ return({ struct _tuple7* _temp47=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp47->f1= 0; _temp47->f2= e; _temp47;});} static
void Cyc_Tcexp_resolve_unknown_fn( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* es){ void* _temp48=( void*)
e1->r; struct _tuple1* _temp54; _LL50: if(*(( int*) _temp48) == Cyc_Absyn_UnknownId_e){
_LL55: _temp54=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp48)->f1; goto _LL51;}
else{ goto _LL52;} _LL52: goto _LL53; _LL51: { struct _handler_cons _temp56;
_push_handler(& _temp56);{ int _temp58= 0; if( setjmp( _temp56.handler)){
_temp58= 1;} if( ! _temp58){{ void* _temp59= Cyc_Tcenv_lookup_ordinary( te, e1->loc,
_temp54); void* _temp69; struct Cyc_Absyn_Tunionfield* _temp71; struct Cyc_Absyn_Tuniondecl*
_temp73; struct Cyc_Absyn_Structdecl* _temp75; _LL61: if(*(( int*) _temp59) ==
Cyc_Tcenv_VarRes){ _LL70: _temp69=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp59)->f1; goto _LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp59) ==
Cyc_Tcenv_TunionRes){ _LL74: _temp73=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp59)->f1; goto _LL72; _LL72: _temp71=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp59)->f2; goto _LL64;} else{ goto _LL65;} _LL65: if(*(( int*) _temp59) ==
Cyc_Tcenv_StructRes){ _LL76: _temp75=(( struct Cyc_Tcenv_StructRes_struct*)
_temp59)->f1; goto _LL66;} else{ goto _LL67;} _LL67: if(*(( int*) _temp59) ==
Cyc_Tcenv_EnumRes){ goto _LL68;} else{ goto _LL60;} _LL62:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp77=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp77[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp78; _temp78.tag= Cyc_Absyn_FnCall_e; _temp78.f1= e1; _temp78.f2= es;
_temp78.f3= 0; _temp78;}); _temp77;}))); _npop_handler( 0u); return; _LL64: if(
_temp71->typs == 0){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp79)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp83= _tag_arr("%s is a constant, not a function", sizeof(
unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp84; _temp84.tag=
Cyc_Stdio_String_pa; _temp84.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp71->name);{ void* _temp82=( void*)& _temp84; void* _temp80[ 1u]={ _temp82};
struct _tagged_arr _temp81={( void*) _temp80,( void*) _temp80,( void*)( _temp80
+ 1u)}; _temp79( _temp83, _temp81);}}));}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp85=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp85[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp86; _temp86.tag= Cyc_Absyn_Tunion_e; _temp86.f1= 0; _temp86.f2= 0; _temp86.f3=
es; _temp86.f4= _temp73; _temp86.f5= _temp71; _temp86;}); _temp85;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL66: { struct Cyc_List_List*
_temp87=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp75->name != 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp88=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp88[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp89; _temp89.tag= Cyc_Absyn_Struct_e;
_temp89.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp75->name))->v;
_temp89.f2= 0; _temp89.f3= _temp87; _temp89.f4=( struct Cyc_Absyn_Structdecl*)
_temp75; _temp89;}); _temp88;})));} else{ Cyc_Tcutil_terr( e->loc, _tag_arr("missing struct name",
sizeof( unsigned char), 20u));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp90=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp90[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp91; _temp91.tag= Cyc_Absyn_Struct_e;
_temp91.f1= _temp54; _temp91.f2= 0; _temp91.f3= _temp87; _temp91.f4=( struct Cyc_Absyn_Structdecl*)
_temp75; _temp91;}); _temp90;})));} _npop_handler( 0u); return;} _LL68: Cyc_Tcutil_terr(
e->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp92)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp96=
_tag_arr("%s is an enum constructor, not a function", sizeof( unsigned char), 42u);
struct Cyc_Stdio_String_pa_struct _temp97; _temp97.tag= Cyc_Stdio_String_pa;
_temp97.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp54);{ void*
_temp95=( void*)& _temp97; void* _temp93[ 1u]={ _temp95}; struct _tagged_arr
_temp94={( void*) _temp93,( void*) _temp93,( void*)( _temp93 + 1u)}; _temp92(
_temp96, _temp94);}})); _npop_handler( 0u); return; _LL60:;}; _pop_handler();}
else{ void* _temp57=( void*) _exn_thrown; void* _temp99= _temp57; _LL101: if(
_temp99 == Cyc_Dict_Absent){ goto _LL102;} else{ goto _LL103;} _LL103: goto
_LL104; _LL102: Cyc_Tcutil_terr( e1->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp105)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp109= _tag_arr("unbound identifier %s", sizeof(
unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp110; _temp110.tag=
Cyc_Stdio_String_pa; _temp110.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp54);{ void* _temp108=( void*)& _temp110; void* _temp106[ 1u]={ _temp108};
struct _tagged_arr _temp107={( void*) _temp106,( void*) _temp106,( void*)(
_temp106 + 1u)}; _temp105( _temp109, _temp107);}}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp111=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp111[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp112; _temp112.tag= Cyc_Absyn_FnCall_e; _temp112.f1=
e1; _temp112.f2= es; _temp112.f3= 0; _temp112;}); _temp111;}))); return; _LL104:(
void) _throw( _temp99); _LL100:;}}} _LL53:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp113=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp113[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp114; _temp114.tag= Cyc_Absyn_FnCall_e; _temp114.f1=
e1; _temp114.f2= es; _temp114.f3= 0; _temp114;}); _temp113;}))); return; _LL49:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp115=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp115[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp116; _temp116.tag= Cyc_Absyn_Array_e;
_temp116.f1= des; _temp116;}); _temp115;}))); return;}{ void* t=*(( void**)
_check_null( topt)); void* _temp117= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual
_temp127; void* _temp129; struct Cyc_Absyn_Structdecl** _temp131; struct Cyc_List_List*
_temp133; struct _tuple1* _temp135; _LL119: if(( unsigned int) _temp117 > 4u?*((
int*) _temp117) == Cyc_Absyn_ArrayType: 0){ _LL130: _temp129=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp117)->f1; goto _LL128; _LL128: _temp127=((
struct Cyc_Absyn_ArrayType_struct*) _temp117)->f2; goto _LL120;} else{ goto
_LL121;} _LL121: if(( unsigned int) _temp117 > 4u?*(( int*) _temp117) == Cyc_Absyn_StructType:
0){ _LL136: _temp135=(( struct Cyc_Absyn_StructType_struct*) _temp117)->f1; goto
_LL134; _LL134: _temp133=(( struct Cyc_Absyn_StructType_struct*) _temp117)->f2;
goto _LL132; _LL132: _temp131=(( struct Cyc_Absyn_StructType_struct*) _temp117)->f3;
goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int) _temp117 > 4u?*((
int*) _temp117) == Cyc_Absyn_AnonStructType: 0){ goto _LL124;} else{ goto _LL125;}
_LL125: goto _LL126; _LL120:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp137=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp137[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp138; _temp138.tag= Cyc_Absyn_Array_e;
_temp138.f1= des; _temp138;}); _temp137;}))); goto _LL118; _LL122: if( _temp131
== 0? 1: _temp135 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("Compiler bug: struct type not properly set",
sizeof( unsigned char), 43u));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp139=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp139[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp140; _temp140.tag= Cyc_Absyn_Struct_e;
_temp140.f1=( struct _tuple1*) _check_null( _temp135); _temp140.f2= 0; _temp140.f3=
des; _temp140.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp131)); _temp140;}); _temp139;}))); goto _LL118; _LL124:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp141=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp141[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp142; _temp142.tag= Cyc_Absyn_AnonStruct_e;
_temp142.f1=( void*) t; _temp142.f2= des; _temp142;}); _temp141;}))); goto
_LL118; _LL126:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp143=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp143[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp144; _temp144.tag= Cyc_Absyn_Array_e;
_temp144.f1= des; _temp144;}); _temp143;}))); goto _LL118; _LL118:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp145)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp150= _tag_arr("test of %s has type %s instead of integral or * type",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp152;
_temp152.tag= Cyc_Stdio_String_pa; _temp152.f1=( struct _tagged_arr) msg_part;{
void* _temp148=( void*)& _temp152; struct Cyc_Stdio_String_pa_struct _temp151;
_temp151.tag= Cyc_Stdio_String_pa; _temp151.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp149=(
void*)& _temp151; void* _temp146[ 2u]={ _temp148, _temp149}; struct _tagged_arr
_temp147={( void*) _temp146,( void*) _temp146,( void*)( _temp146 + 2u)};
_temp145( _temp150, _temp147);}}}));}} static void* Cyc_Tcexp_tcConst( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void* c,
struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp153= c; void* _temp169; void*
_temp171; void* _temp173; int _temp175; void* _temp177; struct _tagged_arr
_temp179; _LL155: if(( unsigned int) _temp153 > 1u?*(( int*) _temp153) == Cyc_Absyn_Char_c:
0){ _LL170: _temp169=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp153)->f1;
goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int) _temp153 > 1u?*((
int*) _temp153) == Cyc_Absyn_Short_c: 0){ _LL172: _temp171=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp153)->f1; goto _LL158;} else{ goto _LL159;} _LL159: if(( unsigned int)
_temp153 > 1u?*(( int*) _temp153) == Cyc_Absyn_LongLong_c: 0){ _LL174: _temp173=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp153)->f1; goto _LL160;} else{
goto _LL161;} _LL161: if(( unsigned int) _temp153 > 1u?*(( int*) _temp153) ==
Cyc_Absyn_Float_c: 0){ goto _LL162;} else{ goto _LL163;} _LL163: if((
unsigned int) _temp153 > 1u?*(( int*) _temp153) == Cyc_Absyn_Int_c: 0){ _LL178:
_temp177=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp153)->f1; goto _LL176;
_LL176: _temp175=(( struct Cyc_Absyn_Int_c_struct*) _temp153)->f2; goto _LL164;}
else{ goto _LL165;} _LL165: if(( unsigned int) _temp153 > 1u?*(( int*) _temp153)
== Cyc_Absyn_String_c: 0){ _LL180: _temp179=(( struct Cyc_Absyn_String_c_struct*)
_temp153)->f1; goto _LL166;} else{ goto _LL167;} _LL167: if( _temp153 ==( void*)
Cyc_Absyn_Null_c){ goto _LL168;} else{ goto _LL154;} _LL156: t= _temp169 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL154;
_LL158: t= _temp171 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL154; _LL160: t= _temp173 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t:
Cyc_Absyn_slonglong_t; goto _LL154; _LL162: t= Cyc_Absyn_float_t; goto _LL154;
_LL164: if( topt == 0){ t= _temp177 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp181= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); void* _temp191; void* _temp193; void* _temp195; void*
_temp197; void* _temp199; _LL183: if(( unsigned int) _temp181 > 4u?*(( int*)
_temp181) == Cyc_Absyn_IntType: 0){ _LL194: _temp193=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f1; goto _LL192; _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f2; if( _temp191 ==( void*) Cyc_Absyn_B1){ goto _LL184;} else{ goto
_LL185;}} else{ goto _LL185;} _LL185: if(( unsigned int) _temp181 > 4u?*(( int*)
_temp181) == Cyc_Absyn_IntType: 0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f1; goto _LL196; _LL196: _temp195=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f2; if( _temp195 ==( void*) Cyc_Absyn_B2){ goto _LL186;} else{ goto
_LL187;}} else{ goto _LL187;} _LL187: if(( unsigned int) _temp181 > 4u?*(( int*)
_temp181) == Cyc_Absyn_IntType: 0){ _LL200: _temp199=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f1; goto _LL188;} else{ goto _LL189;} _LL189: goto _LL190; _LL184: t=
_temp193 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp201=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp201[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp202; _temp202.tag= Cyc_Absyn_Const_e;
_temp202.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp203=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp203[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp204; _temp204.tag= Cyc_Absyn_Char_c;
_temp204.f1=( void*) _temp193; _temp204.f2=( unsigned char) _temp175; _temp204;});
_temp203;})); _temp202;}); _temp201;}))); goto _LL182; _LL186: t= _temp197 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp205=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp205[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp206; _temp206.tag= Cyc_Absyn_Const_e; _temp206.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp207=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp207[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp208; _temp208.tag= Cyc_Absyn_Short_c; _temp208.f1=( void*) _temp197;
_temp208.f2=( short) _temp175; _temp208;}); _temp207;})); _temp206;}); _temp205;})));
goto _LL182; _LL188: t= _temp199 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL182; _LL190: t= _temp177 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL182; _LL182:;} goto _LL154; _LL166: {
int len=( int) _get_arr_size( _temp179, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp209=({ struct Cyc_Absyn_Const_e_struct* _temp246=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp246[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp247; _temp247.tag= Cyc_Absyn_Const_e; _temp247.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp248=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp248[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp249; _temp249.tag= Cyc_Absyn_Int_c; _temp249.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp249.f2= len; _temp249;}); _temp248;})); _temp247;}); _temp246;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp209, loc); elen->topt=({
struct Cyc_Core_Opt* _temp210=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp210->v=( void*) Cyc_Absyn_uint_t; _temp210;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp211=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp211[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp212; _temp212.tag= Cyc_Absyn_Upper_b; _temp212.f1= elen; _temp212;});
_temp211;})); if( topt != 0){ void* _temp213= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp221; struct Cyc_Absyn_Tqual
_temp223; void* _temp225; struct Cyc_Absyn_PtrInfo _temp227; struct Cyc_Absyn_Conref*
_temp229; struct Cyc_Absyn_Tqual _temp231; struct Cyc_Absyn_Conref* _temp233;
void* _temp235; void* _temp237; _LL215: if(( unsigned int) _temp213 > 4u?*(( int*)
_temp213) == Cyc_Absyn_ArrayType: 0){ _LL226: _temp225=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp213)->f1; goto _LL224; _LL224: _temp223=(( struct Cyc_Absyn_ArrayType_struct*)
_temp213)->f2; goto _LL222; _LL222: _temp221=(( struct Cyc_Absyn_ArrayType_struct*)
_temp213)->f3; goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int)
_temp213 > 4u?*(( int*) _temp213) == Cyc_Absyn_PointerType: 0){ _LL228: _temp227=((
struct Cyc_Absyn_PointerType_struct*) _temp213)->f1; _LL238: _temp237=( void*)
_temp227.elt_typ; goto _LL236; _LL236: _temp235=( void*) _temp227.rgn_typ; goto
_LL234; _LL234: _temp233= _temp227.nullable; goto _LL232; _LL232: _temp231=
_temp227.tq; goto _LL230; _LL230: _temp229= _temp227.bounds; goto _LL218;} else{
goto _LL219;} _LL219: goto _LL220; _LL216: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp239=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp239[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp240; _temp240.tag= Cyc_Absyn_ArrayType;
_temp240.f1=( void*) Cyc_Absyn_uchar_t; _temp240.f2= _temp223; _temp240.f3=(
struct Cyc_Absyn_Exp*) elen; _temp240;}); _temp239;}); _LL218: if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp241=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp241->v=( void*) t; _temp241;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp242=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp242->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp242;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp243=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp243[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp244; _temp244.tag= Cyc_Absyn_Upper_b; _temp244.f1= elen; _temp244;});
_temp243;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp245=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp245->v=( void*) t; _temp245;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL214; _LL220:
goto _LL214; _LL214:;} return t;} _LL168: { struct Cyc_List_List* _temp250= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp251=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp251[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp252; _temp252.tag= Cyc_Absyn_PointerType;
_temp252.f1=({ struct Cyc_Absyn_PtrInfo _temp253; _temp253.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp255=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp255->v=( void*)
_temp250; _temp255;})); _temp253.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp254=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp254->v=( void*) _temp250;
_temp254;})); _temp253.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp253.tq= Cyc_Absyn_empty_tqual(); _temp253.bounds= Cyc_Absyn_empty_conref();
_temp253;}); _temp252;}); _temp251;}); goto _LL154;} _LL154:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp256= b; struct Cyc_Absyn_Vardecl*
_temp270; struct Cyc_Absyn_Fndecl* _temp272; struct Cyc_Absyn_Vardecl* _temp274;
struct Cyc_Absyn_Vardecl* _temp276; struct Cyc_Absyn_Vardecl* _temp278; _LL258:
if( _temp256 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL259;} else{ goto _LL260;}
_LL260: if(( unsigned int) _temp256 > 1u?*(( int*) _temp256) == Cyc_Absyn_Global_b:
0){ _LL271: _temp270=(( struct Cyc_Absyn_Global_b_struct*) _temp256)->f1; goto
_LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp256 > 1u?*(( int*)
_temp256) == Cyc_Absyn_Funname_b: 0){ _LL273: _temp272=(( struct Cyc_Absyn_Funname_b_struct*)
_temp256)->f1; goto _LL263;} else{ goto _LL264;} _LL264: if(( unsigned int)
_temp256 > 1u?*(( int*) _temp256) == Cyc_Absyn_Pat_b: 0){ _LL275: _temp274=((
struct Cyc_Absyn_Pat_b_struct*) _temp256)->f1; goto _LL265;} else{ goto _LL266;}
_LL266: if(( unsigned int) _temp256 > 1u?*(( int*) _temp256) == Cyc_Absyn_Local_b:
0){ _LL277: _temp276=(( struct Cyc_Absyn_Local_b_struct*) _temp256)->f1; goto
_LL267;} else{ goto _LL268;} _LL268: if(( unsigned int) _temp256 > 1u?*(( int*)
_temp256) == Cyc_Absyn_Param_b: 0){ _LL279: _temp278=(( struct Cyc_Absyn_Param_b_struct*)
_temp256)->f1; goto _LL269;} else{ goto _LL257;} _LL259: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp280)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp284= _tag_arr("unbound identifier %s", sizeof( unsigned char), 22u); struct
Cyc_Stdio_String_pa_struct _temp285; _temp285.tag= Cyc_Stdio_String_pa; _temp285.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{ void* _temp283=( void*)&
_temp285; void* _temp281[ 1u]={ _temp283}; struct _tagged_arr _temp282={( void*)
_temp281,( void*) _temp281,( void*)( _temp281 + 1u)}; _temp280( _temp284,
_temp282);}})); _LL261: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp270->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp270->type; _LL263:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp272->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp272);
_LL265: _temp276= _temp274; goto _LL267; _LL267: _temp278= _temp276; goto _LL269;
_LL269:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp278->type; _LL257:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_Core_Opt* opt_args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp286=( void*) fmt->r; void* _temp294; struct _tagged_arr
_temp296; struct Cyc_Absyn_Exp* _temp298; struct Cyc_Absyn_Exp _temp300; void*
_temp301; void* _temp303; struct _tagged_arr _temp305; _LL288: if(*(( int*)
_temp286) == Cyc_Absyn_Const_e){ _LL295: _temp294=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp286)->f1; if(( unsigned int) _temp294 > 1u?*(( int*) _temp294) == Cyc_Absyn_String_c:
0){ _LL297: _temp296=(( struct Cyc_Absyn_String_c_struct*) _temp294)->f1; goto
_LL289;} else{ goto _LL290;}} else{ goto _LL290;} _LL290: if(*(( int*) _temp286)
== Cyc_Absyn_Cast_e){ _LL299: _temp298=(( struct Cyc_Absyn_Cast_e_struct*)
_temp286)->f2; _temp300=* _temp298; _LL302: _temp301=( void*) _temp300.r; if(*((
int*) _temp301) == Cyc_Absyn_Const_e){ _LL304: _temp303=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp301)->f1; if(( unsigned int) _temp303 > 1u?*(( int*) _temp303) == Cyc_Absyn_String_c:
0){ _LL306: _temp305=(( struct Cyc_Absyn_String_c_struct*) _temp303)->f1; goto
_LL291;} else{ goto _LL292;}} else{ goto _LL292;}} else{ goto _LL292;} _LL292:
goto _LL293; _LL289: _temp305= _temp296; goto _LL291; _LL291: desc_types=
type_getter( te, _temp305, fmt->loc); goto _LL287; _LL293: return; _LL287:;} if(
opt_args != 0){ struct Cyc_List_List* _temp307=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( opt_args))->v; for( 0; desc_types != 0? _temp307 !=
0: 0;( desc_types=(( struct Cyc_List_List*) _check_null( desc_types))->tl,
_temp307=(( struct Cyc_List_List*) _check_null( _temp307))->tl)){ void* t=( void*)((
struct Cyc_List_List*) _check_null( desc_types))->hd; struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp307))->hd; if(
! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp308)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp313= _tag_arr("descriptor has type %s but argument has type %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp315;
_temp315.tag= Cyc_Stdio_String_pa; _temp315.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp311=( void*)& _temp315; struct Cyc_Stdio_String_pa_struct
_temp314; _temp314.tag= Cyc_Stdio_String_pa; _temp314.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{
void* _temp312=( void*)& _temp314; void* _temp309[ 2u]={ _temp311, _temp312};
struct _tagged_arr _temp310={( void*) _temp309,( void*) _temp309,( void*)(
_temp309 + 2u)}; _temp308( _temp313, _temp310);}}}));}} if( desc_types != 0){
Cyc_Tcutil_terr( fmt->loc, _tag_arr("too few arguments", sizeof( unsigned char),
18u));} if( _temp307 != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp307))->hd)->loc, _tag_arr("too many arguments",
sizeof( unsigned char), 19u));}}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp316= p; _LL318: if( _temp316 ==( void*) Cyc_Absyn_Plus){ goto _LL319;}
else{ goto _LL320;} _LL320: if( _temp316 ==( void*) Cyc_Absyn_Minus){ goto
_LL321;} else{ goto _LL322;} _LL322: if( _temp316 ==( void*) Cyc_Absyn_Not){
goto _LL323;} else{ goto _LL324;} _LL324: if( _temp316 ==( void*) Cyc_Absyn_Bitnot){
goto _LL325;} else{ goto _LL326;} _LL326: if( _temp316 ==( void*) Cyc_Absyn_Size){
goto _LL327;} else{ goto _LL328;} _LL328: goto _LL329; _LL319: goto _LL321;
_LL321: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp330)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp334= _tag_arr("expecting arithmetic type but found %s",
sizeof( unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp335;
_temp335.tag= Cyc_Stdio_String_pa; _temp335.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp333=( void*)& _temp335; void* _temp331[ 1u]={ _temp333}; struct
_tagged_arr _temp332={( void*) _temp331,( void*) _temp331,( void*)( _temp331 + 1u)};
_temp330( _temp334, _temp332);}}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL323: Cyc_Tcutil_check_contains_assign( e); if( !
Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp336)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp340= _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp341;
_temp341.tag= Cyc_Stdio_String_pa; _temp341.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp339=( void*)& _temp341; void* _temp337[ 1u]={ _temp339}; struct
_tagged_arr _temp338={( void*) _temp337,( void*) _temp337,( void*)( _temp337 + 1u)};
_temp336( _temp340, _temp338);}}));} return Cyc_Absyn_sint_t; _LL325: if( ! Cyc_Tcutil_is_integral(
e)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp342)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp346= _tag_arr("expecting integral type but found %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp347; _temp347.tag=
Cyc_Stdio_String_pa; _temp347.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp345=( void*)& _temp347; void* _temp343[ 1u]={ _temp345}; struct
_tagged_arr _temp344={( void*) _temp343,( void*) _temp343,( void*)( _temp343 + 1u)};
_temp342( _temp346, _temp344);}}));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL327:{ void* _temp348= t; struct Cyc_Absyn_PtrInfo
_temp356; _LL350: if(( unsigned int) _temp348 > 4u?*(( int*) _temp348) == Cyc_Absyn_ArrayType:
0){ goto _LL351;} else{ goto _LL352;} _LL352: if(( unsigned int) _temp348 > 4u?*((
int*) _temp348) == Cyc_Absyn_PointerType: 0){ _LL357: _temp356=(( struct Cyc_Absyn_PointerType_struct*)
_temp348)->f1; goto _LL353;} else{ goto _LL354;} _LL354: goto _LL355; _LL351:
goto _LL349; _LL353: goto _LL349; _LL355: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp358)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp362= _tag_arr("size requires pointer or array type, not %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp363;
_temp363.tag= Cyc_Stdio_String_pa; _temp363.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp361=( void*)& _temp363; void* _temp359[ 1u]={ _temp361}; struct
_tagged_arr _temp360={( void*) _temp359,( void*) _temp359,( void*)( _temp359 + 1u)};
_temp358( _temp362, _temp360);}})); goto _LL349; _LL349:;} return Cyc_Absyn_uint_t;
_LL329: return Cyc_Tcutil_impos( _tag_arr("Non-unary primop", sizeof(
unsigned char), 17u)); _LL317:;} static void* Cyc_Tcexp_tcArithBinop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, int(* checker)( struct
Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr( e1->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp364)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp368= _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp369;
_temp369.tag= Cyc_Stdio_String_pa; _temp369.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp367=(
void*)& _temp369; void* _temp365[ 1u]={ _temp367}; struct _tagged_arr _temp366={(
void*) _temp365,( void*) _temp365,( void*)( _temp365 + 1u)}; _temp364( _temp368,
_temp366);}})); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp370=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp370->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp370;}));} if( ! checker( e2)){ Cyc_Tcutil_terr(
e2->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp371)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp375= _tag_arr("type %s cannot be used here", sizeof( unsigned char), 28u);
struct Cyc_Stdio_String_pa_struct _temp376; _temp376.tag= Cyc_Stdio_String_pa;
_temp376.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v);{ void* _temp374=( void*)& _temp376; void* _temp372[
1u]={ _temp374}; struct _tagged_arr _temp373={( void*) _temp372,( void*)
_temp372,( void*)( _temp372 + 1u)}; _temp371( _temp375, _temp373);}})); return
Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp377=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp377->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp377;}));}{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp378= t1; struct
Cyc_Absyn_PtrInfo _temp384; struct Cyc_Absyn_Conref* _temp386; struct Cyc_Absyn_Tqual
_temp388; struct Cyc_Absyn_Conref* _temp390; void* _temp392; void* _temp394;
_LL380: if(( unsigned int) _temp378 > 4u?*(( int*) _temp378) == Cyc_Absyn_PointerType:
0){ _LL385: _temp384=(( struct Cyc_Absyn_PointerType_struct*) _temp378)->f1;
_LL395: _temp394=( void*) _temp384.elt_typ; goto _LL393; _LL393: _temp392=( void*)
_temp384.rgn_typ; goto _LL391; _LL391: _temp390= _temp384.nullable; goto _LL389;
_LL389: _temp388= _temp384.tq; goto _LL387; _LL387: _temp386= _temp384.bounds;
goto _LL381;} else{ goto _LL382;} _LL382: goto _LL383; _LL381: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp396)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp400= _tag_arr("expecting int but found %s", sizeof(
unsigned char), 27u); struct Cyc_Stdio_String_pa_struct _temp401; _temp401.tag=
Cyc_Stdio_String_pa; _temp401.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp399=( void*)& _temp401; void* _temp397[ 1u]={ _temp399}; struct
_tagged_arr _temp398={( void*) _temp397,( void*) _temp397,( void*)( _temp397 + 1u)};
_temp396( _temp400, _temp398);}}));} _temp386= Cyc_Absyn_compress_conref(
_temp386);{ void* _temp402=( void*) _temp386->v; void* _temp410; void* _temp412;
struct Cyc_Absyn_Exp* _temp414; _LL404: if(( unsigned int) _temp402 > 1u?*(( int*)
_temp402) == Cyc_Absyn_Eq_constr: 0){ _LL411: _temp410=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp402)->f1; if( _temp410 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL405;} else{
goto _LL406;}} else{ goto _LL406;} _LL406: if(( unsigned int) _temp402 > 1u?*((
int*) _temp402) == Cyc_Absyn_Eq_constr: 0){ _LL413: _temp412=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp402)->f1; if(( unsigned int) _temp412 > 1u?*((
int*) _temp412) == Cyc_Absyn_Upper_b: 0){ _LL415: _temp414=(( struct Cyc_Absyn_Upper_b_struct*)
_temp412)->f1; goto _LL407;} else{ goto _LL408;}} else{ goto _LL408;} _LL408:
goto _LL409; _LL405: return t1; _LL407: { struct Cyc_Absyn_PointerType_struct*
_temp416=({ struct Cyc_Absyn_PointerType_struct* _temp417=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp417[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp418; _temp418.tag= Cyc_Absyn_PointerType;
_temp418.f1=({ struct Cyc_Absyn_PtrInfo _temp419; _temp419.elt_typ=( void*)
_temp394; _temp419.rgn_typ=( void*) _temp392; _temp419.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp419.tq= _temp388; _temp419.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp419;}); _temp418;}); _temp417;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp416); return( void*) _temp416;} _LL409:( void*)( _temp386->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp420=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp420[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp421; _temp421.tag= Cyc_Absyn_Eq_constr; _temp421.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp421;}); _temp420;}))); return t1;
_LL403:;} _LL383: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL379:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp422)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp427= _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u); struct Cyc_Stdio_String_pa_struct _temp429;
_temp429.tag= Cyc_Stdio_String_pa; _temp429.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp425=(
void*)& _temp429; struct Cyc_Stdio_String_pa_struct _temp428; _temp428.tag= Cyc_Stdio_String_pa;
_temp428.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v);{ void* _temp426=( void*)& _temp428; void* _temp423[
2u]={ _temp425, _temp426}; struct _tagged_arr _temp424={( void*) _temp423,( void*)
_temp423,( void*)( _temp423 + 2u)}; _temp422( _temp427, _temp424);}}}));} return
Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr(
e2->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp430)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp435= _tag_arr("expecting either %s or int but found %s", sizeof(
unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp437; _temp437.tag=
Cyc_Stdio_String_pa; _temp437.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp433=( void*)& _temp437; struct Cyc_Stdio_String_pa_struct
_temp436; _temp436.tag= Cyc_Stdio_String_pa; _temp436.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp434=( void*)& _temp436; void* _temp431[
2u]={ _temp433, _temp434}; struct _tagged_arr _temp432={( void*) _temp431,( void*)
_temp431,( void*)( _temp431 + 2u)}; _temp430( _temp435, _temp432);}}}));} return
t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp438=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp438->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp438;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp440=({ struct _tuple0 _temp439;
_temp439.f1= Cyc_Tcutil_compress( t1); _temp439.f2= Cyc_Tcutil_compress( t2);
_temp439;}); void* _temp446; struct Cyc_Absyn_PtrInfo _temp448; void* _temp450;
void* _temp452; struct Cyc_Absyn_PtrInfo _temp454; void* _temp456; _LL442:
_LL453: _temp452= _temp440.f1; if(( unsigned int) _temp452 > 4u?*(( int*)
_temp452) == Cyc_Absyn_PointerType: 0){ _LL455: _temp454=(( struct Cyc_Absyn_PointerType_struct*)
_temp452)->f1; _LL457: _temp456=( void*) _temp454.elt_typ; goto _LL447;} else{
goto _LL444;} _LL447: _temp446= _temp440.f2; if(( unsigned int) _temp446 > 4u?*((
int*) _temp446) == Cyc_Absyn_PointerType: 0){ _LL449: _temp448=(( struct Cyc_Absyn_PointerType_struct*)
_temp446)->f1; _LL451: _temp450=( void*) _temp448.elt_typ; goto _LL443;} else{
goto _LL444;} _LL444: goto _LL445; _LL443: if( Cyc_Tcutil_unify( _temp456,
_temp450)){ return Cyc_Absyn_sint_t;} goto _LL441; _LL445: goto _LL441; _LL441:;}
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp458)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp463= _tag_arr("comparison not allowed between %s and %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp465;
_temp465.tag= Cyc_Stdio_String_pa; _temp465.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp461=( void*)& _temp465; struct Cyc_Stdio_String_pa_struct
_temp464; _temp464.tag= Cyc_Stdio_String_pa; _temp464.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp462=( void*)& _temp464; void* _temp459[
2u]={ _temp461, _temp462}; struct _tagged_arr _temp460={( void*) _temp459,( void*)
_temp459,( void*)( _temp459 + 2u)}; _temp458( _temp463, _temp460);}}})); return
Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp466=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp466->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp466;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp467= p; _LL469: if( _temp467 ==( void*)
Cyc_Absyn_Plus){ goto _LL470;} else{ goto _LL471;} _LL471: if( _temp467 ==( void*)
Cyc_Absyn_Minus){ goto _LL472;} else{ goto _LL473;} _LL473: if( _temp467 ==(
void*) Cyc_Absyn_Times){ goto _LL474;} else{ goto _LL475;} _LL475: if( _temp467
==( void*) Cyc_Absyn_Div){ goto _LL476;} else{ goto _LL477;} _LL477: if(
_temp467 ==( void*) Cyc_Absyn_Mod){ goto _LL478;} else{ goto _LL479;} _LL479:
if( _temp467 ==( void*) Cyc_Absyn_Bitand){ goto _LL480;} else{ goto _LL481;}
_LL481: if( _temp467 ==( void*) Cyc_Absyn_Bitor){ goto _LL482;} else{ goto
_LL483;} _LL483: if( _temp467 ==( void*) Cyc_Absyn_Bitxor){ goto _LL484;} else{
goto _LL485;} _LL485: if( _temp467 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL486;}
else{ goto _LL487;} _LL487: if( _temp467 ==( void*) Cyc_Absyn_Bitlrshift){ goto
_LL488;} else{ goto _LL489;} _LL489: if( _temp467 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL490;} else{ goto _LL491;} _LL491: if( _temp467 ==( void*) Cyc_Absyn_Eq){
goto _LL492;} else{ goto _LL493;} _LL493: if( _temp467 ==( void*) Cyc_Absyn_Neq){
goto _LL494;} else{ goto _LL495;} _LL495: if( _temp467 ==( void*) Cyc_Absyn_Gt){
goto _LL496;} else{ goto _LL497;} _LL497: if( _temp467 ==( void*) Cyc_Absyn_Lt){
goto _LL498;} else{ goto _LL499;} _LL499: if( _temp467 ==( void*) Cyc_Absyn_Gte){
goto _LL500;} else{ goto _LL501;} _LL501: if( _temp467 ==( void*) Cyc_Absyn_Lte){
goto _LL502;} else{ goto _LL503;} _LL503: goto _LL504; _LL470: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL472: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL474: goto _LL476;
_LL476: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL478: goto _LL480; _LL480: goto _LL482; _LL482: goto _LL484; _LL484: goto
_LL486; _LL486: goto _LL488; _LL488: goto _LL490; _LL490: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL492: goto _LL494; _LL494: goto _LL496;
_LL496: goto _LL498; _LL498: goto _LL500; _LL500: goto _LL502; _LL502: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL504:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp505=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp505[ 0]=({ struct Cyc_Core_Failure_struct
_temp506; _temp506.tag= Cyc_Core_Failure; _temp506.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp506;}); _temp505;})); _LL468:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp507= p; _LL509: goto _LL510;
_LL510: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){ case
0: _LL511: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has 0 arguments",
sizeof( unsigned char), 35u)); case 1: _LL512: t= Cyc_Tcexp_tcUnPrimop( te, loc,
topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
break; case 2: _LL513: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd);
break; default: _LL514: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u));} goto _LL508; _LL508:;} return t;}} struct
_tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{ void* _temp516=( void*) e->r;
void* _temp540; struct Cyc_Absyn_Vardecl* _temp542; void* _temp544; struct Cyc_Absyn_Vardecl*
_temp546; void* _temp548; struct Cyc_Absyn_Vardecl* _temp550; void* _temp552;
struct Cyc_Absyn_Vardecl* _temp554; struct Cyc_Absyn_Exp* _temp556; struct Cyc_Absyn_Exp*
_temp558; struct _tagged_arr* _temp560; struct Cyc_Absyn_Exp* _temp562; struct
_tagged_arr* _temp564; struct Cyc_Absyn_Exp* _temp566; struct Cyc_Absyn_Exp*
_temp568; struct Cyc_Absyn_Exp* _temp570; struct Cyc_Absyn_Exp* _temp572; _LL518:
if(*(( int*) _temp516) == Cyc_Absyn_Var_e){ _LL541: _temp540=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp516)->f2; if(( unsigned int) _temp540 > 1u?*(( int*)
_temp540) == Cyc_Absyn_Param_b: 0){ _LL543: _temp542=(( struct Cyc_Absyn_Param_b_struct*)
_temp540)->f1; goto _LL519;} else{ goto _LL520;}} else{ goto _LL520;} _LL520:
if(*(( int*) _temp516) == Cyc_Absyn_Var_e){ _LL545: _temp544=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp516)->f2; if(( unsigned int) _temp544 > 1u?*(( int*)
_temp544) == Cyc_Absyn_Local_b: 0){ _LL547: _temp546=(( struct Cyc_Absyn_Local_b_struct*)
_temp544)->f1; goto _LL521;} else{ goto _LL522;}} else{ goto _LL522;} _LL522:
if(*(( int*) _temp516) == Cyc_Absyn_Var_e){ _LL549: _temp548=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp516)->f2; if(( unsigned int) _temp548 > 1u?*(( int*)
_temp548) == Cyc_Absyn_Pat_b: 0){ _LL551: _temp550=(( struct Cyc_Absyn_Pat_b_struct*)
_temp548)->f1; goto _LL523;} else{ goto _LL524;}} else{ goto _LL524;} _LL524:
if(*(( int*) _temp516) == Cyc_Absyn_Var_e){ _LL553: _temp552=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp516)->f2; if(( unsigned int) _temp552 > 1u?*(( int*)
_temp552) == Cyc_Absyn_Global_b: 0){ _LL555: _temp554=(( struct Cyc_Absyn_Global_b_struct*)
_temp552)->f1; goto _LL525;} else{ goto _LL526;}} else{ goto _LL526;} _LL526:
if(*(( int*) _temp516) == Cyc_Absyn_Subscript_e){ _LL559: _temp558=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp516)->f1; goto _LL557; _LL557: _temp556=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp516)->f2; goto _LL527;} else{ goto _LL528;} _LL528: if(*(( int*) _temp516)
== Cyc_Absyn_StructMember_e){ _LL563: _temp562=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp516)->f1; goto _LL561; _LL561: _temp560=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp516)->f2; goto _LL529;} else{ goto _LL530;} _LL530: if(*(( int*) _temp516)
== Cyc_Absyn_StructArrow_e){ _LL567: _temp566=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp516)->f1; goto _LL565; _LL565: _temp564=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp516)->f2; goto _LL531;} else{ goto _LL532;} _LL532: if(*(( int*) _temp516)
== Cyc_Absyn_Deref_e){ _LL569: _temp568=(( struct Cyc_Absyn_Deref_e_struct*)
_temp516)->f1; goto _LL533;} else{ goto _LL534;} _LL534: if(*(( int*) _temp516)
== Cyc_Absyn_NoInstantiate_e){ _LL571: _temp570=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp516)->f1; goto _LL535;} else{ goto _LL536;} _LL536: if(*(( int*) _temp516)
== Cyc_Absyn_Instantiate_e){ _LL573: _temp572=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp516)->f1; goto _LL537;} else{ goto _LL538;} _LL538: goto _LL539; _LL519:
_temp546= _temp542; goto _LL521; _LL521: _temp550= _temp546; goto _LL523; _LL523:
_temp554= _temp550; goto _LL525; _LL525: if( !( _temp554->tq).q_const){ return;}
goto _LL517; _LL527:{ void* _temp574= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp558->topt))->v); struct Cyc_Absyn_PtrInfo _temp584; struct Cyc_Absyn_Tqual
_temp586; struct Cyc_Absyn_Tqual _temp588; struct Cyc_List_List* _temp590;
_LL576: if(( unsigned int) _temp574 > 4u?*(( int*) _temp574) == Cyc_Absyn_PointerType:
0){ _LL585: _temp584=(( struct Cyc_Absyn_PointerType_struct*) _temp574)->f1;
_LL587: _temp586= _temp584.tq; goto _LL577;} else{ goto _LL578;} _LL578: if((
unsigned int) _temp574 > 4u?*(( int*) _temp574) == Cyc_Absyn_ArrayType: 0){
_LL589: _temp588=(( struct Cyc_Absyn_ArrayType_struct*) _temp574)->f2; goto
_LL579;} else{ goto _LL580;} _LL580: if(( unsigned int) _temp574 > 4u?*(( int*)
_temp574) == Cyc_Absyn_TupleType: 0){ _LL591: _temp590=(( struct Cyc_Absyn_TupleType_struct*)
_temp574)->f1; goto _LL581;} else{ goto _LL582;} _LL582: goto _LL583; _LL577:
_temp588= _temp586; goto _LL579; _LL579: if( ! _temp588.q_const){ return;} goto
_LL575; _LL581: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp556);{
struct _handler_cons _temp592; _push_handler(& _temp592);{ int _temp594= 0; if(
setjmp( _temp592.handler)){ _temp594= 1;} if( ! _temp594){{ struct _tuple8
_temp597; struct Cyc_Absyn_Tqual _temp598; struct _tuple8* _temp595=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( _temp590,( int) i);
_temp597=* _temp595; _LL599: _temp598= _temp597.f1; goto _LL596; _LL596: if( !
_temp598.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp593=( void*) _exn_thrown; void* _temp601= _temp593; _LL603: if( _temp601 ==
Cyc_List_Nth){ goto _LL604;} else{ goto _LL605;} _LL605: goto _LL606; _LL604:
return; _LL606:( void) _throw( _temp601); _LL602:;}}} goto _LL575;} _LL583: goto
_LL575; _LL575:;} goto _LL517; _LL529:{ void* _temp607= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp562->topt))->v); struct Cyc_Absyn_Structdecl**
_temp619; struct Cyc_List_List* _temp621; struct Cyc_List_List* _temp623; struct
Cyc_Absyn_Uniondecl** _temp625; _LL609: if(( unsigned int) _temp607 > 4u?*(( int*)
_temp607) == Cyc_Absyn_StructType: 0){ _LL620: _temp619=(( struct Cyc_Absyn_StructType_struct*)
_temp607)->f3; goto _LL610;} else{ goto _LL611;} _LL611: if(( unsigned int)
_temp607 > 4u?*(( int*) _temp607) == Cyc_Absyn_AnonUnionType: 0){ _LL622:
_temp621=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp607)->f1; goto _LL612;}
else{ goto _LL613;} _LL613: if(( unsigned int) _temp607 > 4u?*(( int*) _temp607)
== Cyc_Absyn_AnonStructType: 0){ _LL624: _temp623=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp607)->f1; goto _LL614;} else{ goto _LL615;} _LL615: if(( unsigned int)
_temp607 > 4u?*(( int*) _temp607) == Cyc_Absyn_UnionType: 0){ _LL626: _temp625=((
struct Cyc_Absyn_UnionType_struct*) _temp607)->f3; goto _LL616;} else{ goto
_LL617;} _LL617: goto _LL618; _LL610: { struct Cyc_Absyn_Structfield* sf=
_temp619 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp619)), _temp560); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL608;} _LL612: _temp623= _temp621; goto _LL614; _LL614: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp623, _temp560); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL608;} _LL616: { struct Cyc_Absyn_Structfield*
sf= _temp625 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp625)), _temp560); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL608;} _LL618: goto _LL608; _LL608:;} goto _LL517; _LL531:{ void*
_temp627= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp566->topt))->v); struct Cyc_Absyn_PtrInfo _temp633; void* _temp635; _LL629:
if(( unsigned int) _temp627 > 4u?*(( int*) _temp627) == Cyc_Absyn_PointerType: 0){
_LL634: _temp633=(( struct Cyc_Absyn_PointerType_struct*) _temp627)->f1; _LL636:
_temp635=( void*) _temp633.elt_typ; goto _LL630;} else{ goto _LL631;} _LL631:
goto _LL632; _LL630:{ void* _temp637= Cyc_Tcutil_compress( _temp635); struct Cyc_Absyn_Structdecl**
_temp649; struct Cyc_Absyn_Uniondecl** _temp651; struct Cyc_List_List* _temp653;
struct Cyc_List_List* _temp655; _LL639: if(( unsigned int) _temp637 > 4u?*(( int*)
_temp637) == Cyc_Absyn_StructType: 0){ _LL650: _temp649=(( struct Cyc_Absyn_StructType_struct*)
_temp637)->f3; goto _LL640;} else{ goto _LL641;} _LL641: if(( unsigned int)
_temp637 > 4u?*(( int*) _temp637) == Cyc_Absyn_UnionType: 0){ _LL652: _temp651=((
struct Cyc_Absyn_UnionType_struct*) _temp637)->f3; goto _LL642;} else{ goto
_LL643;} _LL643: if(( unsigned int) _temp637 > 4u?*(( int*) _temp637) == Cyc_Absyn_AnonStructType:
0){ _LL654: _temp653=(( struct Cyc_Absyn_AnonStructType_struct*) _temp637)->f1;
goto _LL644;} else{ goto _LL645;} _LL645: if(( unsigned int) _temp637 > 4u?*((
int*) _temp637) == Cyc_Absyn_AnonUnionType: 0){ _LL656: _temp655=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp637)->f1; goto _LL646;} else{ goto _LL647;} _LL647: goto _LL648; _LL640: {
struct Cyc_Absyn_Structfield* sf= _temp649 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp649)), _temp564); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL638;} _LL642: { struct Cyc_Absyn_Structfield*
sf= _temp651 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp651)), _temp564); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL638;} _LL644: _temp655= _temp653; goto _LL646; _LL646: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp655, _temp564); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL638;} _LL648: goto _LL638;
_LL638:;} goto _LL628; _LL632: goto _LL628; _LL628:;} goto _LL517; _LL533:{ void*
_temp657= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp568->topt))->v); struct Cyc_Absyn_PtrInfo _temp665; struct Cyc_Absyn_Tqual
_temp667; struct Cyc_Absyn_Tqual _temp669; _LL659: if(( unsigned int) _temp657 >
4u?*(( int*) _temp657) == Cyc_Absyn_PointerType: 0){ _LL666: _temp665=(( struct
Cyc_Absyn_PointerType_struct*) _temp657)->f1; _LL668: _temp667= _temp665.tq;
goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int) _temp657 > 4u?*((
int*) _temp657) == Cyc_Absyn_ArrayType: 0){ _LL670: _temp669=(( struct Cyc_Absyn_ArrayType_struct*)
_temp657)->f2; goto _LL662;} else{ goto _LL663;} _LL663: goto _LL664; _LL660:
_temp669= _temp667; goto _LL662; _LL662: if( ! _temp669.q_const){ return;} goto
_LL658; _LL664: goto _LL658; _LL658:;} goto _LL517; _LL535: _temp572= _temp570;
goto _LL537; _LL537: Cyc_Tcexp_check_writable( te, _temp572); return; _LL539:
goto _LL517; _LL517:;} Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a read-only location",
sizeof( unsigned char), 38u));} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void*
i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("increment/decrement of non-lvalue",
sizeof( unsigned char), 34u));} Cyc_Tcexp_check_writable( te, e);{ void* t=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_is_numeric(
e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){ Cyc_Tcutil_terr( e->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp671)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp675= _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp676;
_temp676.tag= Cyc_Stdio_String_pa; _temp676.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp674=( void*)& _temp676; void* _temp672[ 1u]={ _temp674}; struct
_tagged_arr _temp673={( void*) _temp672,( void*) _temp672,( void*)( _temp672 + 1u)};
_temp671( _temp675, _temp673);}}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1, _tag_arr("conditional expression", sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp(
te, topt, e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp689=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp689->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp689;})); struct Cyc_List_List* _temp677=({
struct Cyc_List_List* _temp688=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp688->hd=( void*) e3; _temp688->tl= 0; _temp688;});
struct Cyc_List_List* _temp678=({ struct Cyc_List_List* _temp687=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp687->hd=( void*) e2; _temp687->tl=
_temp677; _temp687;}); if( ! Cyc_Tcutil_coerce_list( te, t, _temp678)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp679)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp684=
_tag_arr("conditional clause types do not match: %s != %s", sizeof(
unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp686; _temp686.tag=
Cyc_Stdio_String_pa; _temp686.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp682=(
void*)& _temp686; struct Cyc_Stdio_String_pa_struct _temp685; _temp685.tag= Cyc_Stdio_String_pa;
_temp685.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ void* _temp683=( void*)& _temp685; void* _temp680[
2u]={ _temp682, _temp683}; struct _tagged_arr _temp681={( void*) _temp680,( void*)
_temp680,( void*)( _temp680 + 2u)}; _temp679( _temp684, _temp681);}}}));} return
t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp*
e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void**)((
void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)), e2);{ void* t1=(
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void* t2=( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp690= Cyc_Tcutil_compress(
t1); _LL692: if(( unsigned int) _temp690 > 4u?*(( int*) _temp690) == Cyc_Absyn_ArrayType:
0){ goto _LL693;} else{ goto _LL694;} _LL694: goto _LL695; _LL693: Cyc_Tcutil_terr(
loc, _tag_arr("cannot assign to an array", sizeof( unsigned char), 26u)); goto
_LL691; _LL695: goto _LL691; _LL691:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return
Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue", sizeof(
unsigned char), 25u));} Cyc_Tcexp_check_writable( te, e1); if( po == 0){ if( !
Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp696)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp701= _tag_arr("type mismatch: %s != %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp703;
_temp703.tag= Cyc_Stdio_String_pa; _temp703.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp699=( void*)& _temp703; struct Cyc_Stdio_String_pa_struct
_temp702; _temp702.tag= Cyc_Stdio_String_pa; _temp702.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp700=( void*)& _temp702; void* _temp697[
2u]={ _temp699, _temp700}; struct _tagged_arr _temp698={( void*) _temp697,( void*)
_temp697,( void*)( _temp697 + 2u)}; _temp696( _temp701, _temp698);}}}));}} else{
void* _temp704=( void*)(( struct Cyc_Core_Opt*) _check_null( po))->v; void*
_temp705= Cyc_Tcexp_tcBinPrimop( te, loc, 0, _temp704, e1, e2); if( !( Cyc_Tcutil_unify(
_temp705, t1)? 1: Cyc_Tcutil_coerceable( _temp705))){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("Cannot use this operator in an assignment", sizeof(
unsigned char), 42u));} return _temp705;} return t2;}} static void* Cyc_Tcexp_tcSeqExp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, topt, e2); return( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;}
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;{ void* _temp706= Cyc_Tcutil_compress(
t1); void* _temp716; void* _temp718; _LL708: if( _temp706 ==( void*) Cyc_Absyn_FloatType){
goto _LL709;} else{ goto _LL710;} _LL710: if(( unsigned int) _temp706 > 4u?*((
int*) _temp706) == Cyc_Absyn_IntType: 0){ _LL717: _temp716=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp706)->f2; if( _temp716 ==( void*) Cyc_Absyn_B1){ goto _LL711;} else{ goto
_LL712;}} else{ goto _LL712;} _LL712: if(( unsigned int) _temp706 > 4u?*(( int*)
_temp706) == Cyc_Absyn_IntType: 0){ _LL719: _temp718=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp706)->f2; if( _temp718 ==( void*) Cyc_Absyn_B2){ goto _LL713;} else{ goto
_LL714;}} else{ goto _LL714;} _LL714: goto _LL715; _LL709: Cyc_Tcutil_unchecked_cast(
te, e,( void*) Cyc_Absyn_DoubleType); t1=( void*) Cyc_Absyn_DoubleType; goto
_LL707; _LL711: goto _LL713; _LL713: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_sint_t);
t1= Cyc_Absyn_sint_t; goto _LL707; _LL715: goto _LL707; _LL707:;} for( fields=
fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
void* _temp722; struct Cyc_Position_Segment* _temp724; struct Cyc_List_List*
_temp726; struct Cyc_List_List* _temp728; struct _tuple1* _temp730; struct Cyc_Absyn_Tunionfield
_temp720=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL731: _temp730= _temp720.name; goto _LL729; _LL729: _temp728=
_temp720.tvs; goto _LL727; _LL727: _temp726= _temp720.typs; goto _LL725; _LL725:
_temp724= _temp720.loc; goto _LL723; _LL723: _temp722=( void*) _temp720.sc; goto
_LL721; _LL721: if(( _temp726 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp726))->tl != 0)? 1: _temp728 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp726))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp734; struct Cyc_Position_Segment* _temp736; struct Cyc_List_List*
_temp738; struct Cyc_List_List* _temp740; struct _tuple1* _temp742; struct Cyc_Absyn_Tunionfield
_temp732=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL743: _temp742= _temp732.name; goto _LL741; _LL741: _temp740=
_temp732.tvs; goto _LL739; _LL739: _temp738= _temp732.typs; goto _LL737; _LL737:
_temp736= _temp732.loc; goto _LL735; _LL735: _temp734=( void*) _temp732.sc; goto
_LL733; _LL733: if(( _temp738 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp738))->tl != 0)? 1: _temp740 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp738))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp744)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp749= _tag_arr("can't find a field in %s to inject a value of type %s",
sizeof( unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp751;
_temp751.tag= Cyc_Stdio_String_pa; _temp751.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
tu);{ void* _temp747=( void*)& _temp751; struct Cyc_Stdio_String_pa_struct
_temp750; _temp750.tag= Cyc_Stdio_String_pa; _temp750.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp748=( void*)& _temp750; void* _temp745[
2u]={ _temp747, _temp748}; struct _tagged_arr _temp746={( void*) _temp745,( void*)
_temp745,( void*)( _temp745 + 2u)}; _temp744( _temp749, _temp746);}}})); return
0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* args, struct
Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List* _temp752=
args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp753= t; struct Cyc_Absyn_PtrInfo _temp759; struct Cyc_Absyn_Conref*
_temp761; struct Cyc_Absyn_Tqual _temp763; struct Cyc_Absyn_Conref* _temp765;
void* _temp767; void* _temp769; _LL755: if(( unsigned int) _temp753 > 4u?*(( int*)
_temp753) == Cyc_Absyn_PointerType: 0){ _LL760: _temp759=(( struct Cyc_Absyn_PointerType_struct*)
_temp753)->f1; _LL770: _temp769=( void*) _temp759.elt_typ; goto _LL768; _LL768:
_temp767=( void*) _temp759.rgn_typ; goto _LL766; _LL766: _temp765= _temp759.nullable;
goto _LL764; _LL764: _temp763= _temp759.tq; goto _LL762; _LL762: _temp761=
_temp759.bounds; goto _LL756;} else{ goto _LL757;} _LL757: goto _LL758; _LL756:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp767); Cyc_Tcutil_check_nonzero_bound(
loc, _temp761);{ void* _temp771= Cyc_Tcutil_compress( _temp769); struct Cyc_Absyn_FnInfo
_temp777; struct Cyc_List_List* _temp779; struct Cyc_List_List* _temp781; struct
Cyc_Absyn_VarargInfo* _temp783; int _temp785; struct Cyc_List_List* _temp787;
void* _temp789; struct Cyc_Core_Opt* _temp791; struct Cyc_List_List* _temp793;
_LL773: if(( unsigned int) _temp771 > 4u?*(( int*) _temp771) == Cyc_Absyn_FnType:
0){ _LL778: _temp777=(( struct Cyc_Absyn_FnType_struct*) _temp771)->f1; _LL794:
_temp793= _temp777.tvars; goto _LL792; _LL792: _temp791= _temp777.effect; goto
_LL790; _LL790: _temp789=( void*) _temp777.ret_typ; goto _LL788; _LL788:
_temp787= _temp777.args; goto _LL786; _LL786: _temp785= _temp777.c_varargs; goto
_LL784; _LL784: _temp783= _temp777.cyc_varargs; goto _LL782; _LL782: _temp781=
_temp777.rgn_po; goto _LL780; _LL780: _temp779= _temp777.attributes; goto _LL774;}
else{ goto _LL775;} _LL775: goto _LL776; _LL774: if( topt != 0){ Cyc_Tcutil_unify(
_temp789,*(( void**) _check_null( topt)));} while( _temp752 != 0? _temp787 != 0:
0) { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp752))->hd; void* t2=(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp787))->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( !
Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp795)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp800= _tag_arr("actual argument has type %s but formal has type %s",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp802;
_temp802.tag= Cyc_Stdio_String_pa; _temp802.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp798=(
void*)& _temp802; struct Cyc_Stdio_String_pa_struct _temp801; _temp801.tag= Cyc_Stdio_String_pa;
_temp801.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp799=(
void*)& _temp801; void* _temp796[ 2u]={ _temp798, _temp799}; struct _tagged_arr
_temp797={( void*) _temp796,( void*) _temp796,( void*)( _temp796 + 2u)};
_temp795( _temp800, _temp797);}}}));} _temp752=(( struct Cyc_List_List*)
_check_null( _temp752))->tl; _temp787=(( struct Cyc_List_List*) _check_null(
_temp787))->tl;} if( _temp787 != 0){ Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u));} else{ if(( _temp752 != 0? 1: _temp785)? 1:
_temp783 != 0){ if( _temp785){ for( 0; _temp752 != 0; _temp752=(( struct Cyc_List_List*)
_check_null( _temp752))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp752))->hd);}} else{ if( _temp783 == 0){
Cyc_Tcutil_terr( loc, _tag_arr("too many arguments for function", sizeof(
unsigned char), 32u));} else{ int _temp805; void* _temp807; void* _temp809;
struct Cyc_Absyn_VarargInfo _temp803=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp783)); _LL810: _temp809=( void*) _temp803.type; goto _LL808;
_LL808: _temp807=( void*) _temp803.rgn; goto _LL806; _LL806: _temp805= _temp803.inject;
goto _LL804; _LL804: { struct Cyc_Absyn_VarargCallInfo* _temp811=({ struct Cyc_Absyn_VarargCallInfo*
_temp859=( struct Cyc_Absyn_VarargCallInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp859->num_varargs= 0; _temp859->injectors= 0; _temp859->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp783); _temp859;});* vararg_call_info= _temp811; if( ! Cyc_Tcutil_unify(
Cyc_Tcenv_curr_rgn( te), _temp807)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp812)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp817= _tag_arr("I have no idea how this could fail, but it did because the region for the varargs (%s) doesn't unify with the expected new_block region (%s).",
sizeof( unsigned char), 142u); struct Cyc_Stdio_String_pa_struct _temp819;
_temp819.tag= Cyc_Stdio_String_pa; _temp819.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp807);{ void* _temp815=( void*)& _temp819; struct Cyc_Stdio_String_pa_struct
_temp818; _temp818.tag= Cyc_Stdio_String_pa; _temp818.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te));{ void* _temp816=( void*)&
_temp818; void* _temp813[ 2u]={ _temp815, _temp816}; struct _tagged_arr _temp814={(
void*) _temp813,( void*) _temp813,( void*)( _temp813 + 2u)}; _temp812( _temp817,
_temp814);}}}));} if( ! _temp805){ for( 0; _temp752 != 0; _temp752=(( struct Cyc_List_List*)
_check_null( _temp752))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp752))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp811))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp809,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp809)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp820)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp825= _tag_arr("vararg requires type %s but argument has type %s",
sizeof( unsigned char), 49u); struct Cyc_Stdio_String_pa_struct _temp827;
_temp827.tag= Cyc_Stdio_String_pa; _temp827.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp809);{ void* _temp823=( void*)& _temp827; struct Cyc_Stdio_String_pa_struct
_temp826; _temp826.tag= Cyc_Stdio_String_pa; _temp826.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{
void* _temp824=( void*)& _temp826; void* _temp821[ 2u]={ _temp823, _temp824};
struct _tagged_arr _temp822={( void*) _temp821,( void*) _temp821,( void*)(
_temp821 + 2u)}; _temp820( _temp825, _temp822);}}}));}}} else{ void* _temp828=
Cyc_Tcutil_compress( _temp809); struct Cyc_Absyn_TunionInfo _temp834; void*
_temp836; struct Cyc_List_List* _temp838; void* _temp840; struct Cyc_Absyn_Tuniondecl*
_temp842; _LL830: if(( unsigned int) _temp828 > 4u?*(( int*) _temp828) == Cyc_Absyn_TunionType:
0){ _LL835: _temp834=(( struct Cyc_Absyn_TunionType_struct*) _temp828)->f1;
_LL841: _temp840=( void*) _temp834.tunion_info; if(*(( int*) _temp840) == Cyc_Absyn_KnownTunion){
_LL843: _temp842=(( struct Cyc_Absyn_KnownTunion_struct*) _temp840)->f1; goto
_LL839;} else{ goto _LL832;} _LL839: _temp838= _temp834.targs; goto _LL837;
_LL837: _temp836=( void*) _temp834.rgn; goto _LL831;} else{ goto _LL832;} _LL832:
goto _LL833; _LL831: { struct Cyc_Absyn_Tuniondecl* _temp844=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp842->name); if( ! Cyc_Tcutil_unify( _temp836, _temp807)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp845)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp848=
_tag_arr("bad region for injected varargs", sizeof( unsigned char), 32u); void*
_temp846[ 0u]={}; struct _tagged_arr _temp847={( void*) _temp846,( void*)
_temp846,( void*)( _temp846 + 0u)}; _temp845( _temp848, _temp847);}));}{ struct
Cyc_List_List* fields= 0; if( _temp844->fields == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp849)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp853= _tag_arr("can't inject into %s",
sizeof( unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp854;
_temp854.tag= Cyc_Stdio_String_pa; _temp854.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp809);{ void* _temp852=( void*)& _temp854; void* _temp850[ 1u]={ _temp852};
struct _tagged_arr _temp851={( void*) _temp850,( void*) _temp850,( void*)(
_temp850 + 1u)}; _temp849( _temp853, _temp851);}}));} else{ fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp844->fields))->v;}{ struct _RegionHandle
_temp855= _new_region(); struct _RegionHandle* rgn=& _temp855; _push_region( rgn);{
struct Cyc_List_List* _temp856=(( struct Cyc_List_List*(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y))
Cyc_List_rzip)( rgn, rgn, _temp844->tvs, _temp838); for( 0; _temp752 != 0;
_temp752=(( struct Cyc_List_List*) _check_null( _temp752))->tl){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp811))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp752))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp857= Cyc_Tcexp_tcInjection( te, e1, _temp809,
rgn, _temp856, fields); if( _temp857 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp811))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp811))->injectors,({ struct Cyc_List_List* _temp858=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp858->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp857)); _temp858->tl= 0;
_temp858;}));}}}}}; _pop_region( rgn);} goto _LL829;}} _LL833: Cyc_Tcutil_terr(
loc, _tag_arr("bad inject vararg type", sizeof( unsigned char), 23u)); goto
_LL829; _LL829:;}}}}}}{ struct Cyc_List_List* a= _temp779; for( 0; a != 0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ void* _temp860=( void*)(( struct
Cyc_List_List*) _check_null( a))->hd; int _temp866; int _temp868; void* _temp870;
_LL862: if(( unsigned int) _temp860 > 16u?*(( int*) _temp860) == Cyc_Absyn_Format_att:
0){ _LL871: _temp870=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp860)->f1;
goto _LL869; _LL869: _temp868=(( struct Cyc_Absyn_Format_att_struct*) _temp860)->f2;
goto _LL867; _LL867: _temp866=(( struct Cyc_Absyn_Format_att_struct*) _temp860)->f3;
goto _LL863;} else{ goto _LL864;} _LL864: goto _LL865; _LL863:{ struct
_handler_cons _temp872; _push_handler(& _temp872);{ int _temp874= 0; if( setjmp(
_temp872.handler)){ _temp874= 1;} if( ! _temp874){{ struct Cyc_Absyn_Exp*
_temp875=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
args, _temp868 - 1); struct Cyc_Core_Opt* fmt_args; if( _temp866 == 0){ fmt_args=
0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp876=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp876->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp866 - 1);
_temp876;});}{ void* _temp877= _temp870; _LL879: if( _temp877 ==( void*) Cyc_Absyn_Printf_ft){
goto _LL880;} else{ goto _LL881;} _LL881: if( _temp877 ==( void*) Cyc_Absyn_Scanf_ft){
goto _LL882;} else{ goto _LL878;} _LL880: Cyc_Tcexp_check_format_args( te,
_temp875, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL878; _LL882: Cyc_Tcexp_check_format_args(
te, _temp875, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL878; _LL878:;}};
_pop_handler();} else{ void* _temp873=( void*) _exn_thrown; void* _temp884=
_temp873; _LL886: if( _temp884 == Cyc_List_Nth){ goto _LL887;} else{ goto _LL888;}
_LL888: goto _LL889; _LL887: Cyc_Tcutil_terr( loc, _tag_arr("bad format arguments",
sizeof( unsigned char), 21u)); goto _LL885; _LL889:( void) _throw( _temp884);
_LL885:;}}} goto _LL861; _LL865: goto _LL861; _LL861:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp791))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp781); return _temp789; _LL776: return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("expected pointer to function", sizeof( unsigned char), 29u)); _LL772:;}
_LL758: return Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u)); _LL754:;}} static void* Cyc_Tcexp_tcThrow( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( ! Cyc_Tcutil_coerce_use(
te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp890)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp894= _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u); struct Cyc_Stdio_String_pa_struct _temp895;
_temp895.tag= Cyc_Stdio_String_pa; _temp895.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp893=(
void*)& _temp895; void* _temp891[ 1u]={ _temp893}; struct _tagged_arr _temp892={(
void*) _temp891,( void*) _temp891,( void*)( _temp891 + 1u)}; _temp890( _temp894,
_temp892);}}));} return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp896=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp896->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp896;}));} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{
void* _temp897= t1; struct Cyc_Absyn_PtrInfo _temp903; struct Cyc_Absyn_Conref*
_temp905; struct Cyc_Absyn_Tqual _temp907; struct Cyc_Absyn_Conref* _temp909;
void* _temp911; void* _temp913; _LL899: if(( unsigned int) _temp897 > 4u?*(( int*)
_temp897) == Cyc_Absyn_PointerType: 0){ _LL904: _temp903=(( struct Cyc_Absyn_PointerType_struct*)
_temp897)->f1; _LL914: _temp913=( void*) _temp903.elt_typ; goto _LL912; _LL912:
_temp911=( void*) _temp903.rgn_typ; goto _LL910; _LL910: _temp909= _temp903.nullable;
goto _LL908; _LL908: _temp907= _temp903.tq; goto _LL906; _LL906: _temp905=
_temp903.bounds; goto _LL900;} else{ goto _LL901;} _LL901: goto _LL902; _LL900:{
void* _temp915= Cyc_Tcutil_compress( _temp913); struct Cyc_Absyn_FnInfo _temp921;
struct Cyc_List_List* _temp923; struct Cyc_List_List* _temp925; struct Cyc_Absyn_VarargInfo*
_temp927; int _temp929; struct Cyc_List_List* _temp931; void* _temp933; struct
Cyc_Core_Opt* _temp935; struct Cyc_List_List* _temp937; _LL917: if((
unsigned int) _temp915 > 4u?*(( int*) _temp915) == Cyc_Absyn_FnType: 0){ _LL922:
_temp921=(( struct Cyc_Absyn_FnType_struct*) _temp915)->f1; _LL938: _temp937=
_temp921.tvars; goto _LL936; _LL936: _temp935= _temp921.effect; goto _LL934;
_LL934: _temp933=( void*) _temp921.ret_typ; goto _LL932; _LL932: _temp931=
_temp921.args; goto _LL930; _LL930: _temp929= _temp921.c_varargs; goto _LL928;
_LL928: _temp927= _temp921.cyc_varargs; goto _LL926; _LL926: _temp925= _temp921.rgn_po;
goto _LL924; _LL924: _temp923= _temp921.attributes; goto _LL918;} else{ goto
_LL919;} _LL919: goto _LL920; _LL918: { struct Cyc_List_List* instantiation= 0;
if( Cyc_List_length( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp937)){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("type instantiation mismatch",
sizeof( unsigned char), 28u));} for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, _temp937=(( struct Cyc_List_List*) _check_null( _temp937))->tl)){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp937))->hd)->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); instantiation=({
struct Cyc_List_List* _temp939=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp939->hd=( void*)({ struct _tuple5* _temp940=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp940->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp937))->hd; _temp940->f2=( void*)((
struct Cyc_List_List*) _check_null( ts))->hd; _temp940;}); _temp939->tl=
instantiation; _temp939;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp944=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp944[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp945; _temp945.tag= Cyc_Absyn_FnType; _temp945.f1=({ struct Cyc_Absyn_FnInfo
_temp946; _temp946.tvars= 0; _temp946.effect= _temp935; _temp946.ret_typ=( void*)
_temp933; _temp946.args= _temp931; _temp946.c_varargs= _temp929; _temp946.cyc_varargs=
_temp927; _temp946.rgn_po= _temp925; _temp946.attributes= _temp923; _temp946;});
_temp945;}); _temp944;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp941=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp941[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp942; _temp942.tag= Cyc_Absyn_PointerType;
_temp942.f1=({ struct Cyc_Absyn_PtrInfo _temp943; _temp943.elt_typ=( void*)
new_fn_typ; _temp943.rgn_typ=( void*) _temp911; _temp943.nullable= _temp909;
_temp943.tq= _temp907; _temp943.bounds= _temp905; _temp943;}); _temp942;});
_temp941;}); return new_typ;}} _LL920: goto _LL916; _LL916:;} goto _LL898;
_LL902: goto _LL898; _LL898:;} return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp947)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp951= _tag_arr("expecting polymorphic type but found %s",
sizeof( unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp952;
_temp952.tag= Cyc_Stdio_String_pa; _temp952.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp950=( void*)& _temp952; void* _temp948[ 1u]={ _temp950}; struct
_tagged_arr _temp949={( void*) _temp948,( void*) _temp948,( void*)( _temp948 + 1u)};
_temp947( _temp951, _temp949);}}));}} static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t, struct Cyc_Absyn_Exp*
e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? !
Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp953)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp958= _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp960;
_temp960.tag= Cyc_Stdio_String_pa; _temp960.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp956=( void*)& _temp960; struct Cyc_Stdio_String_pa_struct
_temp959; _temp959.tag= Cyc_Stdio_String_pa; _temp959.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp957=( void*)& _temp959; void* _temp954[
2u]={ _temp956, _temp957}; struct _tagged_arr _temp955={( void*) _temp954,( void*)
_temp954,( void*)( _temp954 + 2u)}; _temp953( _temp958, _temp955);}}}));} return
t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ void** _temp961= 0; struct Cyc_Absyn_Tqual
_temp962= Cyc_Absyn_empty_tqual(); if( topt != 0){ void* _temp963= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp969; struct Cyc_Absyn_Tqual
_temp971; void* _temp973; _LL965: if(( unsigned int) _temp963 > 4u?*(( int*)
_temp963) == Cyc_Absyn_PointerType: 0){ _LL970: _temp969=(( struct Cyc_Absyn_PointerType_struct*)
_temp963)->f1; _LL974: _temp973=( void*) _temp969.elt_typ; goto _LL972; _LL972:
_temp971= _temp969.tq; goto _LL966;} else{ goto _LL967;} _LL967: goto _LL968;
_LL966: _temp961=({ void** _temp975=( void**) GC_malloc( sizeof( void*));
_temp975[ 0]= _temp973; _temp975;}); _temp962= _temp971; goto _LL964; _LL968:
goto _LL964; _LL964:;} Cyc_Tcexp_tcExpNoInst( te, _temp961, e);{ void* _temp976=(
void*) e->r; _LL978: if(*(( int*) _temp976) == Cyc_Absyn_Struct_e){ goto _LL979;}
else{ goto _LL980;} _LL980: if(*(( int*) _temp976) == Cyc_Absyn_Tuple_e){ goto
_LL981;} else{ goto _LL982;} _LL982: goto _LL983; _LL979: goto _LL981; _LL981:
Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate", sizeof( unsigned char), 19u));{
void*(* _temp984)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp985= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp991; struct Cyc_Absyn_Conref* _temp993;
_LL987: if(( unsigned int) _temp985 > 4u?*(( int*) _temp985) == Cyc_Absyn_PointerType:
0){ _LL992: _temp991=(( struct Cyc_Absyn_PointerType_struct*) _temp985)->f1;
_LL994: _temp993= _temp991.nullable; goto _LL988;} else{ goto _LL989;} _LL989:
goto _LL990; _LL988: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp993)){ _temp984= Cyc_Absyn_star_typ;} goto _LL986; _LL990: goto _LL986;
_LL986:;} return _temp984(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp962);} _LL983: goto _LL977; _LL977:;}{ void*
_temp997; int _temp999; struct _tuple6 _temp995= Cyc_Tcutil_addressof_props( te,
e); _LL1000: _temp999= _temp995.f1; goto _LL998; _LL998: _temp997= _temp995.f2;
goto _LL996; _LL996: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp999){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp997, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); if( ! Cyc_Evexp_okay_szofarg( t)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp1001)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp1005= _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u); struct Cyc_Stdio_String_pa_struct _temp1006;
_temp1006.tag= Cyc_Stdio_String_pa; _temp1006.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1004=( void*)& _temp1006; void* _temp1002[ 1u]={ _temp1004};
struct _tagged_arr _temp1003={( void*) _temp1002,( void*) _temp1002,( void*)(
_temp1002 + 1u)}; _temp1001( _temp1005, _temp1003);}}));} return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct _tagged_arr* n){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ int
bad_type= 1;{ void* _temp1007= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp1015; _LL1009: if(( unsigned int) _temp1007 > 4u?*(( int*) _temp1007) ==
Cyc_Absyn_StructType: 0){ _LL1016: _temp1015=(( struct Cyc_Absyn_StructType_struct*)
_temp1007)->f3; goto _LL1010;} else{ goto _LL1011;} _LL1011: if(( unsigned int)
_temp1007 > 4u?*(( int*) _temp1007) == Cyc_Absyn_AnonStructType: 0){ goto
_LL1012;} else{ goto _LL1013;} _LL1013: goto _LL1014; _LL1010: if( _temp1015 ==
0){ return Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u));} if((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp1015)))->fields == 0){ goto _LL1008;} goto _LL1012; _LL1012:
bad_type= 0; goto _LL1008; _LL1014: goto _LL1008; _LL1008:;} if( bad_type){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1017)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1021=
_tag_arr("%s is not a known struct type", sizeof( unsigned char), 30u); struct
Cyc_Stdio_String_pa_struct _temp1022; _temp1022.tag= Cyc_Stdio_String_pa;
_temp1022.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1020=(
void*)& _temp1022; void* _temp1018[ 1u]={ _temp1020}; struct _tagged_arr
_temp1019={( void*) _temp1018,( void*) _temp1018,( void*)( _temp1018 + 1u)};
_temp1017( _temp1021, _temp1019);}}));} return Cyc_Absyn_uint_t;}} static void*
Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1023= t;
struct Cyc_Absyn_PtrInfo _temp1029; struct Cyc_Absyn_Conref* _temp1031; void*
_temp1033; void* _temp1035; _LL1025: if(( unsigned int) _temp1023 > 4u?*(( int*)
_temp1023) == Cyc_Absyn_PointerType: 0){ _LL1030: _temp1029=(( struct Cyc_Absyn_PointerType_struct*)
_temp1023)->f1; _LL1036: _temp1035=( void*) _temp1029.elt_typ; goto _LL1034;
_LL1034: _temp1033=( void*) _temp1029.rgn_typ; goto _LL1032; _LL1032: _temp1031=
_temp1029.bounds; goto _LL1026;} else{ goto _LL1027;} _LL1027: goto _LL1028;
_LL1026: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1033); Cyc_Tcutil_check_nonzero_bound(
loc, _temp1031); return _temp1035; _LL1028: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp1037)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1041= _tag_arr("expecting * or @ type but found %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp1042;
_temp1042.tag= Cyc_Stdio_String_pa; _temp1042.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1040=( void*)& _temp1042; void* _temp1038[ 1u]={ _temp1040};
struct _tagged_arr _temp1039={( void*) _temp1038,( void*) _temp1038,( void*)(
_temp1038 + 1u)}; _temp1037( _temp1041, _temp1039);}})); _LL1024:;}} static void*
Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1043= t;
struct Cyc_Absyn_Structdecl** _temp1059; struct Cyc_Absyn_Structdecl* _temp1061;
struct Cyc_List_List* _temp1062; struct _tuple1* _temp1064; struct Cyc_List_List*
_temp1066; struct Cyc_List_List* _temp1068; struct Cyc_Absyn_Uniondecl**
_temp1070; struct Cyc_Absyn_Uniondecl* _temp1072; struct Cyc_List_List*
_temp1073; struct _tuple1* _temp1075; _LL1045: if(( unsigned int) _temp1043 > 4u?*((
int*) _temp1043) == Cyc_Absyn_StructType: 0){ _LL1065: _temp1064=(( struct Cyc_Absyn_StructType_struct*)
_temp1043)->f1; goto _LL1063; _LL1063: _temp1062=(( struct Cyc_Absyn_StructType_struct*)
_temp1043)->f2; goto _LL1060; _LL1060: _temp1059=(( struct Cyc_Absyn_StructType_struct*)
_temp1043)->f3; if( _temp1059 == 0){ goto _LL1047;} else{ _temp1061=* _temp1059;
goto _LL1046;}} else{ goto _LL1047;} _LL1047: if(( unsigned int) _temp1043 > 4u?*((
int*) _temp1043) == Cyc_Absyn_AnonStructType: 0){ _LL1067: _temp1066=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1043)->f1; goto _LL1048;} else{ goto
_LL1049;} _LL1049: if(( unsigned int) _temp1043 > 4u?*(( int*) _temp1043) == Cyc_Absyn_AnonUnionType:
0){ _LL1069: _temp1068=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1043)->f1;
goto _LL1050;} else{ goto _LL1051;} _LL1051: if(( unsigned int) _temp1043 > 4u?*((
int*) _temp1043) == Cyc_Absyn_UnionType: 0){ _LL1076: _temp1075=(( struct Cyc_Absyn_UnionType_struct*)
_temp1043)->f1; goto _LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_UnionType_struct*)
_temp1043)->f2; goto _LL1071; _LL1071: _temp1070=(( struct Cyc_Absyn_UnionType_struct*)
_temp1043)->f3; if( _temp1070 == 0){ goto _LL1053;} else{ _temp1072=* _temp1070;
goto _LL1052;}} else{ goto _LL1053;} _LL1053: if(( unsigned int) _temp1043 > 4u?*((
int*) _temp1043) == Cyc_Absyn_ArrayType: 0){ goto _LL1077;} else{ goto _LL1055;}
_LL1077: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
== 0){ goto _LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int) _temp1043
> 4u?*(( int*) _temp1043) == Cyc_Absyn_PointerType: 0){ goto _LL1078;} else{
goto _LL1057;} _LL1078: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) == 0){ goto _LL1056;} else{ goto _LL1057;} _LL1057: goto
_LL1058; _LL1046: if( _temp1064 == 0){ return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("unresolved struct type (COMPILER ERROR)", sizeof( unsigned char), 40u));}{
struct Cyc_Absyn_Structfield* _temp1079= Cyc_Absyn_lookup_struct_field(
_temp1061, f); if( _temp1079 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1080)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1085= _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp1087;
_temp1087.tag= Cyc_Stdio_String_pa; _temp1087.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1064));{ void* _temp1083=( void*)& _temp1087;
struct Cyc_Stdio_String_pa_struct _temp1086; _temp1086.tag= Cyc_Stdio_String_pa;
_temp1086.f1=( struct _tagged_arr)* f;{ void* _temp1084=( void*)& _temp1086;
void* _temp1081[ 2u]={ _temp1083, _temp1084}; struct _tagged_arr _temp1082={(
void*) _temp1081,( void*) _temp1081,( void*)( _temp1081 + 2u)}; _temp1080(
_temp1085, _temp1082);}}}));}{ void* t2;{ struct _RegionHandle _temp1088=
_new_region(); struct _RegionHandle* rgn=& _temp1088; _push_region( rgn);{
struct Cyc_List_List* _temp1089=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1061->tvs, _temp1062); t2= Cyc_Tcutil_rsubstitute(
rgn, _temp1089,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1079))->type);};
_pop_region( rgn);} return t2;}} _LL1048: { struct Cyc_Absyn_Structfield*
_temp1090= Cyc_Absyn_lookup_field( _temp1066, f); if( _temp1090 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1091)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1095= _tag_arr("struct has no %s field", sizeof(
unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp1096; _temp1096.tag=
Cyc_Stdio_String_pa; _temp1096.f1=( struct _tagged_arr)* f;{ void* _temp1094=(
void*)& _temp1096; void* _temp1092[ 1u]={ _temp1094}; struct _tagged_arr
_temp1093={( void*) _temp1092,( void*) _temp1092,( void*)( _temp1092 + 1u)};
_temp1091( _temp1095, _temp1093);}}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1090))->type;} _LL1050: { struct Cyc_Absyn_Structfield*
_temp1097= Cyc_Absyn_lookup_field( _temp1068, f); if( _temp1097 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1098)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1102= _tag_arr("union has no %s field", sizeof(
unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp1103; _temp1103.tag=
Cyc_Stdio_String_pa; _temp1103.f1=( struct _tagged_arr)* f;{ void* _temp1101=(
void*)& _temp1103; void* _temp1099[ 1u]={ _temp1101}; struct _tagged_arr
_temp1100={( void*) _temp1099,( void*) _temp1099,( void*)( _temp1099 + 1u)};
_temp1098( _temp1102, _temp1100);}}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1097))->type;} _LL1052: if( _temp1075 == 0){ return Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u));}{ struct Cyc_Absyn_Structfield* _temp1104= Cyc_Absyn_lookup_union_field(
_temp1072, f); if( _temp1104 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1105)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1110= _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp1112;
_temp1112.tag= Cyc_Stdio_String_pa; _temp1112.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1075));{ void* _temp1108=( void*)& _temp1112;
struct Cyc_Stdio_String_pa_struct _temp1111; _temp1111.tag= Cyc_Stdio_String_pa;
_temp1111.f1=( struct _tagged_arr)* f;{ void* _temp1109=( void*)& _temp1111;
void* _temp1106[ 2u]={ _temp1108, _temp1109}; struct _tagged_arr _temp1107={(
void*) _temp1106,( void*) _temp1106,( void*)( _temp1106 + 2u)}; _temp1105(
_temp1110, _temp1107);}}}));}{ struct Cyc_List_List* _temp1113=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1072->tvs,
_temp1073); void* _temp1114= Cyc_Tcutil_substitute( _temp1113,( void*)(( struct
Cyc_Absyn_Structfield*) _check_null( _temp1104))->type); return _temp1114;}}
_LL1054:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp1115=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1115[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1116; _temp1116.tag= Cyc_Absyn_Primop_e;
_temp1116.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1116.f2=({ struct Cyc_List_List*
_temp1117=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1117->hd=( void*) e; _temp1117->tl= 0; _temp1117;}); _temp1116;});
_temp1115;}))); return Cyc_Absyn_uint_t; _LL1056:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1118=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1118[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1119; _temp1119.tag= Cyc_Absyn_Primop_e;
_temp1119.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1119.f2=({ struct Cyc_List_List*
_temp1120=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1120->hd=( void*) e; _temp1120->tl= 0; _temp1120;}); _temp1119;});
_temp1118;}))); return Cyc_Absyn_uint_t; _LL1058: if( Cyc_String_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1121)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1125= _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp1126; _temp1126.tag=
Cyc_Stdio_String_pa; _temp1126.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1124=( void*)& _temp1126; void* _temp1122[ 1u]={ _temp1124};
struct _tagged_arr _temp1123={( void*) _temp1122,( void*) _temp1122,( void*)(
_temp1122 + 1u)}; _temp1121( _temp1125, _temp1123);}}));} else{ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1127)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1131= _tag_arr("expecting struct or union, found %s", sizeof( unsigned char),
36u); struct Cyc_Stdio_String_pa_struct _temp1132; _temp1132.tag= Cyc_Stdio_String_pa;
_temp1132.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1130=(
void*)& _temp1132; void* _temp1128[ 1u]={ _temp1130}; struct _tagged_arr
_temp1129={( void*) _temp1128,( void*) _temp1128,( void*)( _temp1128 + 1u)};
_temp1127( _temp1131, _temp1129);}}));} _LL1044:;}} static void* Cyc_Tcexp_tcStructArrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp1133= t; struct Cyc_Absyn_PtrInfo _temp1139; struct Cyc_Absyn_Conref*
_temp1141; void* _temp1143; void* _temp1145; _LL1135: if(( unsigned int)
_temp1133 > 4u?*(( int*) _temp1133) == Cyc_Absyn_PointerType: 0){ _LL1140:
_temp1139=(( struct Cyc_Absyn_PointerType_struct*) _temp1133)->f1; _LL1146:
_temp1145=( void*) _temp1139.elt_typ; goto _LL1144; _LL1144: _temp1143=( void*)
_temp1139.rgn_typ; goto _LL1142; _LL1142: _temp1141= _temp1139.bounds; goto
_LL1136;} else{ goto _LL1137;} _LL1137: goto _LL1138; _LL1136: Cyc_Tcutil_check_nonzero_bound(
loc, _temp1141);{ void* _temp1147= Cyc_Tcutil_compress( _temp1145); struct Cyc_Absyn_Structdecl**
_temp1159; struct Cyc_Absyn_Structdecl* _temp1161; struct Cyc_List_List*
_temp1162; struct _tuple1* _temp1164; struct Cyc_Absyn_Uniondecl** _temp1166;
struct Cyc_Absyn_Uniondecl* _temp1168; struct Cyc_List_List* _temp1169; struct
_tuple1* _temp1171; struct Cyc_List_List* _temp1173; struct Cyc_List_List*
_temp1175; _LL1149: if(( unsigned int) _temp1147 > 4u?*(( int*) _temp1147) ==
Cyc_Absyn_StructType: 0){ _LL1165: _temp1164=(( struct Cyc_Absyn_StructType_struct*)
_temp1147)->f1; goto _LL1163; _LL1163: _temp1162=(( struct Cyc_Absyn_StructType_struct*)
_temp1147)->f2; goto _LL1160; _LL1160: _temp1159=(( struct Cyc_Absyn_StructType_struct*)
_temp1147)->f3; if( _temp1159 == 0){ goto _LL1151;} else{ _temp1161=* _temp1159;
goto _LL1150;}} else{ goto _LL1151;} _LL1151: if(( unsigned int) _temp1147 > 4u?*((
int*) _temp1147) == Cyc_Absyn_UnionType: 0){ _LL1172: _temp1171=(( struct Cyc_Absyn_UnionType_struct*)
_temp1147)->f1; goto _LL1170; _LL1170: _temp1169=(( struct Cyc_Absyn_UnionType_struct*)
_temp1147)->f2; goto _LL1167; _LL1167: _temp1166=(( struct Cyc_Absyn_UnionType_struct*)
_temp1147)->f3; if( _temp1166 == 0){ goto _LL1153;} else{ _temp1168=* _temp1166;
goto _LL1152;}} else{ goto _LL1153;} _LL1153: if(( unsigned int) _temp1147 > 4u?*((
int*) _temp1147) == Cyc_Absyn_AnonStructType: 0){ _LL1174: _temp1173=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1147)->f1; goto _LL1154;} else{ goto
_LL1155;} _LL1155: if(( unsigned int) _temp1147 > 4u?*(( int*) _temp1147) == Cyc_Absyn_AnonUnionType:
0){ _LL1176: _temp1175=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1147)->f1;
goto _LL1156;} else{ goto _LL1157;} _LL1157: goto _LL1158; _LL1150: { struct Cyc_Absyn_Structfield*
_temp1177= Cyc_Absyn_lookup_struct_field( _temp1161, f); if( _temp1177 == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1178)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1183= _tag_arr("struct %s has no %s field", sizeof(
unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp1185; _temp1185.tag=
Cyc_Stdio_String_pa; _temp1185.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1164));{ void* _temp1181=( void*)& _temp1185;
struct Cyc_Stdio_String_pa_struct _temp1184; _temp1184.tag= Cyc_Stdio_String_pa;
_temp1184.f1=( struct _tagged_arr)* f;{ void* _temp1182=( void*)& _temp1184;
void* _temp1179[ 2u]={ _temp1181, _temp1182}; struct _tagged_arr _temp1180={(
void*) _temp1179,( void*) _temp1179,( void*)( _temp1179 + 2u)}; _temp1178(
_temp1183, _temp1180);}}}));}{ void* t3;{ struct _RegionHandle _temp1186=
_new_region(); struct _RegionHandle* rgn=& _temp1186; _push_region( rgn);{
struct Cyc_List_List* _temp1187=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1161->tvs, _temp1162); t3= Cyc_Tcutil_rsubstitute(
rgn, _temp1187,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1177))->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1143); return
t3;}} _LL1152: { struct Cyc_Absyn_Structfield* _temp1188= Cyc_Absyn_lookup_union_field(
_temp1168, f); if( _temp1188 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1189)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1194= _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp1196;
_temp1196.tag= Cyc_Stdio_String_pa; _temp1196.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1171));{ void* _temp1192=( void*)& _temp1196;
struct Cyc_Stdio_String_pa_struct _temp1195; _temp1195.tag= Cyc_Stdio_String_pa;
_temp1195.f1=( struct _tagged_arr)* f;{ void* _temp1193=( void*)& _temp1195;
void* _temp1190[ 2u]={ _temp1192, _temp1193}; struct _tagged_arr _temp1191={(
void*) _temp1190,( void*) _temp1190,( void*)( _temp1190 + 2u)}; _temp1189(
_temp1194, _temp1191);}}}));}{ struct Cyc_List_List* _temp1197=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1168->tvs,
_temp1169); void* _temp1198= Cyc_Tcutil_substitute( _temp1197,( void*)(( struct
Cyc_Absyn_Structfield*) _check_null( _temp1188))->type); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1143); return _temp1198;}} _LL1154: { struct Cyc_Absyn_Structfield*
_temp1199= Cyc_Absyn_lookup_field( _temp1173, f); if( _temp1199 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1200)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1204= _tag_arr("struct has no %s field", sizeof(
unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp1205; _temp1205.tag=
Cyc_Stdio_String_pa; _temp1205.f1=( struct _tagged_arr)* f;{ void* _temp1203=(
void*)& _temp1205; void* _temp1201[ 1u]={ _temp1203}; struct _tagged_arr
_temp1202={( void*) _temp1201,( void*) _temp1201,( void*)( _temp1201 + 1u)};
_temp1200( _temp1204, _temp1202);}}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1199))->type;} _LL1156: { struct Cyc_Absyn_Structfield*
_temp1206= Cyc_Absyn_lookup_field( _temp1175, f); if( _temp1206 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1207)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1211= _tag_arr("union has no %s field", sizeof(
unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp1212; _temp1212.tag=
Cyc_Stdio_String_pa; _temp1212.f1=( struct _tagged_arr)* f;{ void* _temp1210=(
void*)& _temp1212; void* _temp1208[ 1u]={ _temp1210}; struct _tagged_arr
_temp1209={( void*) _temp1208,( void*) _temp1208,( void*)( _temp1208 + 1u)};
_temp1207( _temp1211, _temp1209);}}));} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1206))->type;} _LL1158: return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp1213)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1217= _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp1218;
_temp1218.tag= Cyc_Stdio_String_pa; _temp1218.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1216=( void*)& _temp1218; void* _temp1214[ 1u]={ _temp1216};
struct _tagged_arr _temp1215={( void*) _temp1214,( void*) _temp1214,( void*)(
_temp1214 + 1u)}; _temp1213( _temp1217, _temp1215);}})); _LL1148:;} _LL1138:
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1219)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1223= _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp1224;
_temp1224.tag= Cyc_Stdio_String_pa; _temp1224.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1222=( void*)& _temp1224; void* _temp1220[ 1u]={ _temp1222};
struct _tagged_arr _temp1221={( void*) _temp1220,( void*) _temp1220,( void*)(
_temp1220 + 1u)}; _temp1219( _temp1223, _temp1221);}})); _LL1134:;}} static void*
Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1225; _push_handler(& _temp1225);{ int
_temp1227= 0; if( setjmp( _temp1225.handler)){ _temp1227= 1;} if( ! _temp1227){{
void* _temp1228=(*(( struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1228;}; _pop_handler();} else{
void* _temp1226=( void*) _exn_thrown; void* _temp1230= _temp1226; _LL1232: if(
_temp1230 == Cyc_List_Nth){ goto _LL1233;} else{ goto _LL1234;} _LL1234: goto
_LL1235; _LL1233: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp1236)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp1241= _tag_arr("index is %d but tuple has only %d fields",
sizeof( unsigned char), 41u); struct Cyc_Stdio_Int_pa_struct _temp1243;
_temp1243.tag= Cyc_Stdio_Int_pa; _temp1243.f1=( int) i;{ void* _temp1239=( void*)&
_temp1243; struct Cyc_Stdio_Int_pa_struct _temp1242; _temp1242.tag= Cyc_Stdio_Int_pa;
_temp1242.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ts));{ void* _temp1240=( void*)& _temp1242; void* _temp1237[ 2u]={ _temp1239,
_temp1240}; struct _tagged_arr _temp1238={( void*) _temp1237,( void*) _temp1237,(
void*)( _temp1237 + 2u)}; _temp1236( _temp1241, _temp1238);}}})); _LL1235:( void)
_throw( _temp1230); _LL1231:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return
Cyc_Tcexp_expr_err( te, e2->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1244)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1248= _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u); struct Cyc_Stdio_String_pa_struct _temp1249;
_temp1249.tag= Cyc_Stdio_String_pa; _temp1249.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp1247=( void*)& _temp1249; void* _temp1245[ 1u]={ _temp1247};
struct _tagged_arr _temp1246={( void*) _temp1245,( void*) _temp1245,( void*)(
_temp1245 + 1u)}; _temp1244( _temp1248, _temp1246);}}));}{ void* _temp1250= t1;
struct Cyc_Absyn_PtrInfo _temp1258; struct Cyc_Absyn_Conref* _temp1260; struct
Cyc_Absyn_Tqual _temp1262; void* _temp1264; void* _temp1266; struct Cyc_List_List*
_temp1268; _LL1252: if(( unsigned int) _temp1250 > 4u?*(( int*) _temp1250) ==
Cyc_Absyn_PointerType: 0){ _LL1259: _temp1258=(( struct Cyc_Absyn_PointerType_struct*)
_temp1250)->f1; _LL1267: _temp1266=( void*) _temp1258.elt_typ; goto _LL1265;
_LL1265: _temp1264=( void*) _temp1258.rgn_typ; goto _LL1263; _LL1263: _temp1262=
_temp1258.tq; goto _LL1261; _LL1261: _temp1260= _temp1258.bounds; goto _LL1253;}
else{ goto _LL1254;} _LL1254: if(( unsigned int) _temp1250 > 4u?*(( int*)
_temp1250) == Cyc_Absyn_TupleType: 0){ _LL1269: _temp1268=(( struct Cyc_Absyn_TupleType_struct*)
_temp1250)->f1; goto _LL1255;} else{ goto _LL1256;} _LL1256: goto _LL1257;
_LL1253: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1260);} else{{ void* _temp1270=( void*)( Cyc_Absyn_compress_conref(
_temp1260))->v; void* _temp1276; struct Cyc_Absyn_Exp* _temp1278; struct Cyc_Absyn_Exp
_temp1280; void* _temp1281; void* _temp1283; int _temp1285; void* _temp1287;
_LL1272: if(( unsigned int) _temp1270 > 1u?*(( int*) _temp1270) == Cyc_Absyn_Eq_constr:
0){ _LL1277: _temp1276=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1270)->f1;
if(( unsigned int) _temp1276 > 1u?*(( int*) _temp1276) == Cyc_Absyn_Upper_b: 0){
_LL1279: _temp1278=(( struct Cyc_Absyn_Upper_b_struct*) _temp1276)->f1;
_temp1280=* _temp1278; _LL1282: _temp1281=( void*) _temp1280.r; if(*(( int*)
_temp1281) == Cyc_Absyn_Const_e){ _LL1284: _temp1283=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1281)->f1; if(( unsigned int) _temp1283 > 1u?*(( int*) _temp1283) == Cyc_Absyn_Int_c:
0){ _LL1288: _temp1287=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1283)->f1;
if( _temp1287 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1286;} else{ goto _LL1274;}
_LL1286: _temp1285=(( struct Cyc_Absyn_Int_c_struct*) _temp1283)->f2; if(
_temp1285 == 1){ goto _LL1273;} else{ goto _LL1274;}} else{ goto _LL1274;}}
else{ goto _LL1274;}} else{ goto _LL1274;}} else{ goto _LL1274;} _LL1274: goto
_LL1275; _LL1273: Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u)); goto _LL1271; _LL1275: goto _LL1271; _LL1271:;}
Cyc_Tcutil_check_nonzero_bound( loc, _temp1260);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1264); return _temp1266; _LL1255: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1268, e2); _LL1257: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1289)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1293= _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp1294;
_temp1294.tag= Cyc_Stdio_String_pa; _temp1294.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1292=( void*)& _temp1294; void* _temp1290[ 1u]={ _temp1292};
struct _tagged_arr _temp1291={( void*) _temp1290,( void*) _temp1290,( void*)(
_temp1290 + 1u)}; _temp1289( _temp1293, _temp1291);}})); _LL1251:;}}} static
void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1295= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_List_List* _temp1301; _LL1297: if((
unsigned int) _temp1295 > 4u?*(( int*) _temp1295) == Cyc_Absyn_TupleType: 0){
_LL1302: _temp1301=(( struct Cyc_Absyn_TupleType_struct*) _temp1295)->f1; goto
_LL1298;} else{ goto _LL1299;} _LL1299: goto _LL1300; _LL1298: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1301) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc, _tag_arr("tuple expression has the wrong number of fields",
sizeof( unsigned char), 48u)); goto _LL1296;} for( 0; es != 0;( es=(( struct Cyc_List_List*)
_check_null( es))->tl, _temp1301=(( struct Cyc_List_List*) _check_null(
_temp1301))->tl)){ void* _temp1303=(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp1301))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)&
_temp1303,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
fields=({ struct Cyc_List_List* _temp1304=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1304->hd=( void*)({ struct _tuple8*
_temp1305=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1305->f1=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1301))->hd)).f1;
_temp1305->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1305;});
_temp1304->tl= fields; _temp1304;});} done= 1; goto _LL1296; _LL1300: goto
_LL1296; _LL1296:;} if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); fields=({ struct Cyc_List_List*
_temp1306=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1306->hd=( void*)({ struct _tuple8* _temp1307=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1307->f1= Cyc_Absyn_empty_tqual(); _temp1307->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1307;}); _temp1306->tl=
fields; _temp1306;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1308=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1308[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1309; _temp1309.tag= Cyc_Absyn_TupleType;
_temp1309.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1309;}); _temp1308;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,
_tag_arr("tcCompoundLit", sizeof( unsigned char), 14u));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1340=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1340->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1340;})); struct Cyc_Absyn_Const_e_struct*
_temp1310=({ struct Cyc_Absyn_Const_e_struct* _temp1336=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1336[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1337; _temp1337.tag= Cyc_Absyn_Const_e; _temp1337.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1338=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1338[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1339; _temp1339.tag= Cyc_Absyn_Int_c; _temp1339.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1339.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1339;}); _temp1338;})); _temp1337;}); _temp1336;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1310, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1311=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1311->v=( void*) Cyc_Absyn_uint_t; _temp1311;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1334=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1334[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1335; _temp1335.tag= Cyc_Absyn_ArrayType;
_temp1335.f1=( void*) res; _temp1335.f2= Cyc_Absyn_empty_tqual(); _temp1335.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1335;}); _temp1334;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp1312)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1316= _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp1317;
_temp1317.tag= Cyc_Stdio_String_pa; _temp1317.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
res);{ void* _temp1315=( void*)& _temp1317; void* _temp1313[ 1u]={ _temp1315};
struct _tagged_arr _temp1314={( void*) _temp1313,( void*) _temp1313,( void*)(
_temp1313 + 1u)}; _temp1312( _temp1316, _temp1314);}}));}{ int offset= 0; for( 0;
des != 0;( offset ++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){
struct Cyc_List_List* ds=(*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1; if( ds != 0){ void* _temp1318=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp1324; _LL1320: if(*(( int*)
_temp1318) == Cyc_Absyn_FieldName){ goto _LL1321;} else{ goto _LL1322;} _LL1322:
if(*(( int*) _temp1318) == Cyc_Absyn_ArrayElement){ _LL1325: _temp1324=(( struct
Cyc_Absyn_ArrayElement_struct*) _temp1318)->f1; goto _LL1323;} else{ goto
_LL1319;} _LL1321: Cyc_Tcutil_terr( loc, _tag_arr("only array index designators are supported",
sizeof( unsigned char), 43u)); goto _LL1319; _LL1323: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp1324);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1324);
if( i != offset){ Cyc_Tcutil_terr( _temp1324->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp1326)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1331= _tag_arr("expecting index designator %d but found %d",
sizeof( unsigned char), 43u); struct Cyc_Stdio_Int_pa_struct _temp1333;
_temp1333.tag= Cyc_Stdio_Int_pa; _temp1333.f1=( int)(( unsigned int) offset);{
void* _temp1329=( void*)& _temp1333; struct Cyc_Stdio_Int_pa_struct _temp1332;
_temp1332.tag= Cyc_Stdio_Int_pa; _temp1332.f1=( int) i;{ void* _temp1330=( void*)&
_temp1332; void* _temp1327[ 2u]={ _temp1329, _temp1330}; struct _tagged_arr
_temp1328={( void*) _temp1327,( void*) _temp1327,( void*)( _temp1327 + 2u)};
_temp1326( _temp1331, _temp1328);}}}));} goto _LL1319;} _LL1319:;}}} return
res_t2;}} static void* Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0,
bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){ Cyc_Tcutil_terr( bound->loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp1341)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1345= _tag_arr("expecting unsigned int, found %s",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp1346;
_temp1346.tag= Cyc_Stdio_String_pa; _temp1346.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( bound->topt))->v);{ void* _temp1344=(
void*)& _temp1346; void* _temp1342[ 1u]={ _temp1344}; struct _tagged_arr
_temp1343={( void*) _temp1342,( void*) _temp1342,( void*)( _temp1342 + 1u)};
_temp1341( _temp1345, _temp1343);}}));} if( !( vd->tq).q_const){(( int(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u));} if( te->le != 0){ te= Cyc_Tcenv_new_block( loc,
te); te= Cyc_Tcenv_add_local_var( loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp(
te, bound)){ Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant",
sizeof( unsigned char), 22u));} if( ! Cyc_Tcutil_is_const_exp( te, body)){ Cyc_Tcutil_terr(
bound->loc, _tag_arr("body is not constant", sizeof( unsigned char), 21u));}}{
struct Cyc_Absyn_PtrInfo pinfo; void** _temp1347= 0; struct Cyc_Absyn_Tqual*
_temp1348= 0; if( topt != 0){ void* _temp1349= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1357; struct Cyc_Absyn_Exp*
_temp1359; struct Cyc_Absyn_Tqual _temp1361; struct Cyc_Absyn_Tqual* _temp1363;
void* _temp1364; void** _temp1366; _LL1351: if(( unsigned int) _temp1349 > 4u?*((
int*) _temp1349) == Cyc_Absyn_PointerType: 0){ _LL1358: _temp1357=(( struct Cyc_Absyn_PointerType_struct*)
_temp1349)->f1; goto _LL1352;} else{ goto _LL1353;} _LL1353: if(( unsigned int)
_temp1349 > 4u?*(( int*) _temp1349) == Cyc_Absyn_ArrayType: 0){ _LL1365:
_temp1364=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1349)->f1;
_temp1366=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1349)->f1); goto
_LL1362; _LL1362: _temp1361=(( struct Cyc_Absyn_ArrayType_struct*) _temp1349)->f2;
_temp1363=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1349)->f2; goto _LL1360;
_LL1360: _temp1359=(( struct Cyc_Absyn_ArrayType_struct*) _temp1349)->f3; goto
_LL1354;} else{ goto _LL1355;} _LL1355: goto _LL1356; _LL1352: pinfo= _temp1357;
_temp1347=( void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1348=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1350; _LL1354: _temp1347=( void**) _temp1366; _temp1348=(
struct Cyc_Absyn_Tqual*) _temp1363; goto _LL1350; _LL1356: goto _LL1350; _LL1350:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1347, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1367=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1367[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1368; _temp1368.tag= Cyc_Absyn_ArrayType;
_temp1368.f1=( void*) t; _temp1368.f2= _temp1348 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1348)); _temp1368.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1368;}); _temp1367;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1369; _push_handler(&
_temp1369);{ int _temp1371= 0; if( setjmp( _temp1369.handler)){ _temp1371= 1;}
if( ! _temp1371){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1370=( void*) _exn_thrown; void* _temp1373= _temp1370; _LL1375:
if( _temp1373 == Cyc_Dict_Absent){ goto _LL1376;} else{ goto _LL1377;} _LL1377:
goto _LL1378; _LL1376: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp1379)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1383= _tag_arr("unbound struct name %s", sizeof(
unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp1384; _temp1384.tag=
Cyc_Stdio_String_pa; _temp1384.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1382=( void*)& _temp1384; void* _temp1380[ 1u]={ _temp1382};
struct _tagged_arr _temp1381={( void*) _temp1380,( void*) _temp1380,( void*)(
_temp1380 + 1u)}; _temp1379( _temp1383, _temp1381);}})); return topt != 0?*((
void**) _check_null( topt)):( void*) Cyc_Absyn_VoidType; _LL1378:( void) _throw(
_temp1373); _LL1374:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!= 0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1385= _new_region(); struct _RegionHandle* rgn=&
_temp1385; _push_region( rgn);{ struct _tuple4 _temp1386=({ struct _tuple4
_temp1416; _temp1416.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1416.f2= rgn;
_temp1416;}); struct Cyc_List_List* _temp1387=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1386, sd->tvs); struct Cyc_List_List* _temp1388=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1387); struct Cyc_Absyn_StructType_struct*
_temp1389=({ struct Cyc_Absyn_StructType_struct* _temp1413=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1413[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1414; _temp1414.tag= Cyc_Absyn_StructType;
_temp1414.f1=( struct _tuple1*)* tn; _temp1414.f2= _temp1388; _temp1414.f3=({
struct Cyc_Absyn_Structdecl** _temp1415=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1415[ 0]= sd; _temp1415;});
_temp1414;}); _temp1413;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) _temp1389,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1390=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1390->v=( void*)
_temp1388; _temp1390;}); if( sd->fields == 0){ Cyc_Tcutil_terr( loc, _tag_arr("can't build abstract struct",
sizeof( unsigned char), 28u));{ void* _temp1391=( void*) _temp1389;
_npop_handler( 0u); return _temp1391;}}{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1394; struct Cyc_Absyn_Exp* _temp1395; struct Cyc_Absyn_Structfield*
_temp1397; struct _tuple9* _temp1392=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1394=* _temp1392; _LL1398: _temp1397= _temp1394.f1;
goto _LL1396; _LL1396: _temp1395= _temp1394.f2; goto _LL1393; _LL1393: { void*
_temp1399= Cyc_Tcutil_rsubstitute( rgn, _temp1387,( void*) _temp1397->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1399, _temp1395); if( ! Cyc_Tcutil_coerce_arg( te, _temp1395,
_temp1399)){ Cyc_Tcutil_terr( _temp1395->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp1400)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1407= _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp1411;
_temp1411.tag= Cyc_Stdio_String_pa; _temp1411.f1=( struct _tagged_arr)*
_temp1397->name;{ void* _temp1403=( void*)& _temp1411; struct Cyc_Stdio_String_pa_struct
_temp1410; _temp1410.tag= Cyc_Stdio_String_pa; _temp1410.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(* tn);{ void* _temp1404=( void*)& _temp1410; struct Cyc_Stdio_String_pa_struct
_temp1409; _temp1409.tag= Cyc_Stdio_String_pa; _temp1409.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp1399);{ void* _temp1405=( void*)& _temp1409; struct
Cyc_Stdio_String_pa_struct _temp1408; _temp1408.tag= Cyc_Stdio_String_pa;
_temp1408.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1395->topt))->v);{ void* _temp1406=( void*)& _temp1408; void*
_temp1401[ 4u]={ _temp1403, _temp1404, _temp1405, _temp1406}; struct _tagged_arr
_temp1402={( void*) _temp1401,( void*) _temp1401,( void*)( _temp1401 + 4u)};
_temp1400( _temp1407, _temp1402);}}}}}));}}}{ void* _temp1412=( void*) _temp1389;
_npop_handler( 0u); return _temp1412;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1417= _new_region(); struct
_RegionHandle* rgn=& _temp1417; _push_region( rgn);{ void* _temp1418= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1424; _LL1420: if(( unsigned int) _temp1418 > 4u?*((
int*) _temp1418) == Cyc_Absyn_AnonStructType: 0){ _LL1425: _temp1424=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1418)->f1; goto _LL1421;} else{ goto
_LL1422;} _LL1422: goto _LL1423; _LL1421: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1424); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1428; struct Cyc_Absyn_Exp*
_temp1429; struct Cyc_Absyn_Structfield* _temp1431; struct _tuple9* _temp1426=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1428=*
_temp1426; _LL1432: _temp1431= _temp1428.f1; goto _LL1430; _LL1430: _temp1429=
_temp1428.f2; goto _LL1427; _LL1427: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1431->type)), _temp1429); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1429,( void*) _temp1431->type)){ Cyc_Tcutil_terr( _temp1429->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp1433)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1439= _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp1442;
_temp1442.tag= Cyc_Stdio_String_pa; _temp1442.f1=( struct _tagged_arr)*
_temp1431->name;{ void* _temp1436=( void*)& _temp1442; struct Cyc_Stdio_String_pa_struct
_temp1441; _temp1441.tag= Cyc_Stdio_String_pa; _temp1441.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*) _temp1431->type);{ void* _temp1437=( void*)&
_temp1441; struct Cyc_Stdio_String_pa_struct _temp1440; _temp1440.tag= Cyc_Stdio_String_pa;
_temp1440.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1429->topt))->v);{ void* _temp1438=( void*)& _temp1440; void*
_temp1434[ 3u]={ _temp1436, _temp1437, _temp1438}; struct _tagged_arr _temp1435={(
void*) _temp1434,( void*) _temp1434,( void*)( _temp1434 + 3u)}; _temp1433(
_temp1439, _temp1435);}}}}));}} goto _LL1419;} _LL1423:(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u)); goto _LL1419; _LL1419:;}; _pop_region( rgn);}
return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Core_Opt**
all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl*
tud, struct Cyc_Absyn_Tunionfield* tuf){ struct _RegionHandle _temp1443=
_new_region(); struct _RegionHandle* rgn=& _temp1443; _push_region( rgn);{
struct _tuple4 _temp1444=({ struct _tuple4 _temp1503; _temp1503.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1503.f2= rgn; _temp1503;}); struct Cyc_List_List* _temp1445=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1444, tud->tvs); struct Cyc_List_List*
_temp1446=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(*
f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp1444, tuf->tvs);
struct Cyc_List_List* _temp1447=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp1445); struct Cyc_List_List* _temp1448=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1446); struct Cyc_List_List* _temp1449=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1445, _temp1446);* all_ref=({
struct Cyc_Core_Opt* _temp1450=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1450->v=( void*) _temp1447; _temp1450;});* exist_ref=({
struct Cyc_Core_Opt* _temp1451=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1451->v=( void*) _temp1448; _temp1451;});{ void* res=( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1498=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1498[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1499; _temp1499.tag= Cyc_Absyn_TunionFieldType;
_temp1499.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1500; _temp1500.field_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp1501=( struct
Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct));
_temp1501[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct _temp1502; _temp1502.tag=
Cyc_Absyn_KnownTunionfield; _temp1502.f1= tud; _temp1502.f2= tuf; _temp1502;});
_temp1501;})); _temp1500.targs= _temp1447; _temp1500;}); _temp1499;}); _temp1498;});
if( topt != 0){ void* _temp1452= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_TunionInfo _temp1460; void* _temp1462; struct Cyc_List_List*
_temp1464; _LL1454: if(( unsigned int) _temp1452 > 4u?*(( int*) _temp1452) ==
Cyc_Absyn_TunionFieldType: 0){ goto _LL1455;} else{ goto _LL1456;} _LL1456: if((
unsigned int) _temp1452 > 4u?*(( int*) _temp1452) == Cyc_Absyn_TunionType: 0){
_LL1461: _temp1460=(( struct Cyc_Absyn_TunionType_struct*) _temp1452)->f1;
_LL1465: _temp1464= _temp1460.targs; goto _LL1463; _LL1463: _temp1462=( void*)
_temp1460.rgn; goto _LL1457;} else{ goto _LL1458;} _LL1458: goto _LL1459;
_LL1455: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res); goto _LL1453;
_LL1457:{ struct Cyc_List_List* a= _temp1447; for( 0; a != 0? _temp1464 != 0: 0;(
a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1464=(( struct Cyc_List_List*)
_check_null( _temp1464))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( a))->hd,( void*)(( struct Cyc_List_List*) _check_null( _temp1464))->hd);}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1466=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1466->v=(
void*) res; _temp1466;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1467=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1467[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1468; _temp1468.tag=
Cyc_Absyn_TunionType; _temp1468.f1=({ struct Cyc_Absyn_TunionInfo _temp1469;
_temp1469.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1470=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1470[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1471; _temp1471.tag=
Cyc_Absyn_KnownTunion; _temp1471.f1= tud; _temp1471;}); _temp1470;})); _temp1469.targs=
_temp1447; _temp1469.rgn=( void*) _temp1462; _temp1469;}); _temp1468;});
_temp1467;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1472= res;
_npop_handler( 0u); return _temp1472;}} goto _LL1453; _LL1459: goto _LL1453;
_LL1453:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0;(
es=(( struct Cyc_List_List*) _check_null( es))->tl, ts=(( struct Cyc_List_List*)
_check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; void* t= Cyc_Tcutil_rsubstitute(
rgn, _temp1449,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( ts))->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){ Cyc_Tcutil_terr( e->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1473)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1479= _tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u); struct Cyc_Stdio_String_pa_struct _temp1482;
_temp1482.tag= Cyc_Stdio_String_pa; _temp1482.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1476=( void*)& _temp1482; struct Cyc_Stdio_String_pa_struct
_temp1481; _temp1481.tag= Cyc_Stdio_String_pa; _temp1481.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1477=( void*)& _temp1481; struct Cyc_Stdio_String_pa_struct
_temp1480; _temp1480.tag= Cyc_Stdio_String_pa; _temp1480.f1=( struct _tagged_arr)(
e->topt == 0? _tag_arr("?", sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1478=(
void*)& _temp1480; void* _temp1474[ 3u]={ _temp1476, _temp1477, _temp1478};
struct _tagged_arr _temp1475={( void*) _temp1474,( void*) _temp1474,( void*)(
_temp1474 + 3u)}; _temp1473( _temp1479, _temp1475);}}}}));}} if( es != 0){ void*
_temp1489= Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1483)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1487= _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp1488;
_temp1488.tag= Cyc_Stdio_String_pa; _temp1488.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1486=( void*)& _temp1488; void* _temp1484[ 1u]={
_temp1486}; struct _tagged_arr _temp1485={( void*) _temp1484,( void*) _temp1484,(
void*)( _temp1484 + 1u)}; _temp1483( _temp1487, _temp1485);}})); _npop_handler(
0u); return _temp1489;} if( ts != 0){ void* _temp1496= Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1490)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1494=
_tag_arr("too few arguments for tunion constructor %s", sizeof( unsigned char),
44u); struct Cyc_Stdio_String_pa_struct _temp1495; _temp1495.tag= Cyc_Stdio_String_pa;
_temp1495.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void*
_temp1493=( void*)& _temp1495; void* _temp1491[ 1u]={ _temp1493}; struct
_tagged_arr _temp1492={( void*) _temp1491,( void*) _temp1491,( void*)( _temp1491
+ 1u)}; _temp1490( _temp1494, _temp1492);}})); _npop_handler( 0u); return
_temp1496;}{ void* _temp1497= res; _npop_handler( 0u); return _temp1497;}}}};
_pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt, void*
t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1518=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1518[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1519; _temp1519.tag= Cyc_Absyn_RgnHandleType;
_temp1519.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp1520=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1520->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1520;}));
_temp1519;}); _temp1518;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( ropt)); void* _temp1504= Cyc_Tcutil_compress(
handle_type); void* _temp1510; _LL1506: if(( unsigned int) _temp1504 > 4u?*((
int*) _temp1504) == Cyc_Absyn_RgnHandleType: 0){ _LL1511: _temp1510=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp1504)->f1; goto _LL1507;} else{
goto _LL1508;} _LL1508: goto _LL1509; _LL1507: rgn= _temp1510; Cyc_Tcenv_check_rgn_accessible(
te, loc, rgn); goto _LL1505; _LL1509: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1512)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp1516= _tag_arr("expecting region_t type but found %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp1517; _temp1517.tag=
Cyc_Stdio_String_pa; _temp1517.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1515=( void*)& _temp1517; void* _temp1513[ 1u]={
_temp1515}; struct _tagged_arr _temp1514={( void*) _temp1513,( void*) _temp1513,(
void*)( _temp1513 + 1u)}; _temp1512( _temp1516, _temp1514);}})); goto _LL1505;
_LL1505:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_MemKind, t);{ void* _temp1521= t; struct Cyc_Absyn_TunionFieldInfo
_temp1527; void* _temp1529; struct Cyc_Absyn_Tunionfield* _temp1531; _LL1523:
if(( unsigned int) _temp1521 > 4u?*(( int*) _temp1521) == Cyc_Absyn_TunionFieldType:
0){ _LL1528: _temp1527=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1521)->f1;
_LL1530: _temp1529=( void*) _temp1527.field_info; if(*(( int*) _temp1529) == Cyc_Absyn_KnownTunionfield){
_LL1532: _temp1531=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1529)->f2;
goto _LL1524;} else{ goto _LL1525;}} else{ goto _LL1525;} _LL1525: goto _LL1526;
_LL1524: if( _temp1531->tvs != 0){ Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u));} goto _LL1522; _LL1526: goto _LL1522; _LL1522:;}{
void*(* _temp1533)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp1534= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1540; struct Cyc_Absyn_Conref* _temp1542;
_LL1536: if(( unsigned int) _temp1534 > 4u?*(( int*) _temp1534) == Cyc_Absyn_PointerType:
0){ _LL1541: _temp1540=(( struct Cyc_Absyn_PointerType_struct*) _temp1534)->f1;
_LL1543: _temp1542= _temp1540.nullable; goto _LL1537;} else{ goto _LL1538;}
_LL1538: goto _LL1539; _LL1537: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1542)){ _temp1533= Cyc_Absyn_star_typ;} goto _LL1535; _LL1539: goto _LL1535;
_LL1535:;} return _temp1533( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1544=( void*) s->r; struct Cyc_Absyn_Exp* _temp1554;
struct Cyc_Absyn_Stmt* _temp1556; struct Cyc_Absyn_Stmt* _temp1558; struct Cyc_Absyn_Stmt*
_temp1560; struct Cyc_Absyn_Decl* _temp1562; _LL1546: if(( unsigned int)
_temp1544 > 1u?*(( int*) _temp1544) == Cyc_Absyn_Exp_s: 0){ _LL1555: _temp1554=((
struct Cyc_Absyn_Exp_s_struct*) _temp1544)->f1; goto _LL1547;} else{ goto
_LL1548;} _LL1548: if(( unsigned int) _temp1544 > 1u?*(( int*) _temp1544) == Cyc_Absyn_Seq_s:
0){ _LL1559: _temp1558=(( struct Cyc_Absyn_Seq_s_struct*) _temp1544)->f1; goto
_LL1557; _LL1557: _temp1556=(( struct Cyc_Absyn_Seq_s_struct*) _temp1544)->f2;
goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( unsigned int) _temp1544 > 1u?*((
int*) _temp1544) == Cyc_Absyn_Decl_s: 0){ _LL1563: _temp1562=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1544)->f1; goto _LL1561; _LL1561: _temp1560=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1544)->f2; goto _LL1551;} else{ goto _LL1552;} _LL1552: goto _LL1553;
_LL1547: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1554->topt))->v;
_LL1549: s= _temp1556; continue; _LL1551: s= _temp1560; continue; _LL1553:
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u)); _LL1545:;}} static void* Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u));}
static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle
!= 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1578=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1578[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1579; _temp1579.tag= Cyc_Absyn_RgnHandleType; _temp1579.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1580=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1580->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1580;})); _temp1579;}); _temp1578;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle)); void* _temp1564= Cyc_Tcutil_compress( handle_type);
void* _temp1570; _LL1566: if(( unsigned int) _temp1564 > 4u?*(( int*) _temp1564)
== Cyc_Absyn_RgnHandleType: 0){ _LL1571: _temp1570=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1564)->f1; goto _LL1567;} else{ goto _LL1568;} _LL1568: goto _LL1569;
_LL1567: rgn= _temp1570; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1565; _LL1569: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null(
rgn_handle))->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1572)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp1576= _tag_arr("expecting region_t type but found %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp1577; _temp1577.tag=
Cyc_Stdio_String_pa; _temp1577.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1575=( void*)& _temp1577; void* _temp1573[ 1u]={
_temp1575}; struct _tagged_arr _temp1574={( void*) _temp1573,( void*) _temp1573,(
void*)( _temp1573 + 1u)}; _temp1572( _temp1576, _temp1574);}})); goto _LL1565;
_LL1565:;}{ void* _temp1581=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1593;
struct Cyc_Absyn_Exp* _temp1595; struct Cyc_Absyn_Vardecl* _temp1597; struct Cyc_List_List*
_temp1599; struct Cyc_Core_Opt* _temp1601; struct Cyc_List_List* _temp1603; void*
_temp1605; struct _tagged_arr _temp1607; _LL1583: if(*(( int*) _temp1581) == Cyc_Absyn_Comprehension_e){
_LL1598: _temp1597=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1581)->f1;
goto _LL1596; _LL1596: _temp1595=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1581)->f2; goto _LL1594; _LL1594: _temp1593=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1581)->f3; goto _LL1584;} else{ goto _LL1585;} _LL1585: if(*(( int*)
_temp1581) == Cyc_Absyn_UnresolvedMem_e){ _LL1602: _temp1601=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1581)->f1; goto _LL1600; _LL1600: _temp1599=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1581)->f2; goto _LL1586;} else{ goto _LL1587;} _LL1587: if(*(( int*)
_temp1581) == Cyc_Absyn_Array_e){ _LL1604: _temp1603=(( struct Cyc_Absyn_Array_e_struct*)
_temp1581)->f1; goto _LL1588;} else{ goto _LL1589;} _LL1589: if(*(( int*)
_temp1581) == Cyc_Absyn_Const_e){ _LL1606: _temp1605=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1581)->f1; if(( unsigned int) _temp1605 > 1u?*(( int*) _temp1605) == Cyc_Absyn_String_c:
0){ _LL1608: _temp1607=(( struct Cyc_Absyn_String_c_struct*) _temp1605)->f1;
goto _LL1590;} else{ goto _LL1591;}} else{ goto _LL1591;} _LL1591: goto _LL1592;
_LL1584: { void* _temp1609= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1610= Cyc_Tcutil_compress( _temp1609); struct Cyc_Absyn_Tqual _temp1616;
void* _temp1618; _LL1612: if(( unsigned int) _temp1610 > 4u?*(( int*) _temp1610)
== Cyc_Absyn_ArrayType: 0){ _LL1619: _temp1618=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1610)->f1; goto _LL1617; _LL1617: _temp1616=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1610)->f2; goto _LL1613;} else{ goto _LL1614;} _LL1614: goto _LL1615;
_LL1613: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1595)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1624=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1624[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1625; _temp1625.tag= Cyc_Absyn_Upper_b; _temp1625.f1=
_temp1595; _temp1625;}); _temp1624;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1621=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1621[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1622; _temp1622.tag= Cyc_Absyn_PointerType;
_temp1622.f1=({ struct Cyc_Absyn_PtrInfo _temp1623; _temp1623.elt_typ=( void*)
_temp1618; _temp1623.rgn_typ=( void*) rgn; _temp1623.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1623.tq= _temp1616; _temp1623.bounds= Cyc_Absyn_new_conref(
b); _temp1623;}); _temp1622;}); _temp1621;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1620=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1620->v=( void*) res_typ; _temp1620;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return
res_typ;} _LL1615: return Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u)); _LL1611:;} _LL1586:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1626=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1626[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1627; _temp1627.tag= Cyc_Absyn_Array_e; _temp1627.f1=
_temp1599; _temp1627;}); _temp1626;}))); _temp1603= _temp1599; goto _LL1588;
_LL1588: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1628= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1634; struct Cyc_Absyn_Tqual
_temp1636; void* _temp1638; void** _temp1640; _LL1630: if(( unsigned int)
_temp1628 > 4u?*(( int*) _temp1628) == Cyc_Absyn_PointerType: 0){ _LL1635:
_temp1634=(( struct Cyc_Absyn_PointerType_struct*) _temp1628)->f1; _LL1639:
_temp1638=( void*) _temp1634.elt_typ; _temp1640=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1628)->f1).elt_typ; goto _LL1637; _LL1637: _temp1636= _temp1634.tq; goto
_LL1631;} else{ goto _LL1632;} _LL1632: goto _LL1633; _LL1631: elt_typ_opt=(
void**) _temp1640; goto _LL1629; _LL1633: goto _LL1629; _LL1629:;}{ void*
_temp1641= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1642= Cyc_Tcutil_compress( _temp1641); struct Cyc_Absyn_Exp* _temp1648;
struct Cyc_Absyn_Tqual _temp1650; void* _temp1652; _LL1644: if(( unsigned int)
_temp1642 > 4u?*(( int*) _temp1642) == Cyc_Absyn_ArrayType: 0){ _LL1653:
_temp1652=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1642)->f1; goto
_LL1651; _LL1651: _temp1650=(( struct Cyc_Absyn_ArrayType_struct*) _temp1642)->f2;
goto _LL1649; _LL1649: _temp1648=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1642)->f3; goto _LL1645;} else{ goto _LL1646;} _LL1646: goto _LL1647;
_LL1645: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1654=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1654[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1655; _temp1655.tag=
Cyc_Absyn_PointerType; _temp1655.f1=({ struct Cyc_Absyn_PtrInfo _temp1656;
_temp1656.elt_typ=( void*) _temp1652; _temp1656.rgn_typ=( void*) rgn; _temp1656.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1656.tq=
_temp1650; _temp1656.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1657=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1657[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1658; _temp1658.tag= Cyc_Absyn_Upper_b;
_temp1658.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1648); _temp1658;});
_temp1657;})); _temp1656;}); _temp1655;}); _temp1654;}); if( topt != 0){ if( !
Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1659=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1659->v=( void*) res_typ; _temp1659;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1643; _LL1647: return Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u)); _LL1643:;} return res_typ;}} _LL1590: { void*
_temp1660= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),(
void*) Cyc_Absyn_Unknown_b); void* _temp1661= Cyc_Tcexp_tcExp( te,( void**)&
_temp1660, e1); return Cyc_Absyn_atb_typ( _temp1661, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1662=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1662[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1663; _temp1663.tag= Cyc_Absyn_Upper_b; _temp1663.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1663;}); _temp1662;}));} _LL1592: { void** topt2=
0; if( topt != 0){ void* _temp1664= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1672; struct Cyc_Absyn_Conref* _temp1674;
void* _temp1676; void** _temp1678; _LL1666: if(( unsigned int) _temp1664 > 4u?*((
int*) _temp1664) == Cyc_Absyn_PointerType: 0){ _LL1673: _temp1672=(( struct Cyc_Absyn_PointerType_struct*)
_temp1664)->f1; _LL1677: _temp1676=( void*) _temp1672.elt_typ; _temp1678=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1664)->f1).elt_typ; goto _LL1675;
_LL1675: _temp1674= _temp1672.nullable; goto _LL1667;} else{ goto _LL1668;}
_LL1668: if(( unsigned int) _temp1664 > 4u?*(( int*) _temp1664) == Cyc_Absyn_TunionType:
0){ goto _LL1669;} else{ goto _LL1670;} _LL1670: goto _LL1671; _LL1667: topt2=(
void**) _temp1678; goto _LL1665; _LL1669: topt2=({ void** _temp1679=( void**)
GC_malloc( sizeof( void*)); _temp1679[ 0]=*(( void**) _check_null( topt));
_temp1679;}); goto _LL1665; _LL1671: goto _LL1665; _LL1665:;}{ void* _temp1680=
Cyc_Tcexp_tcExp( te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1682=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1682[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1683; _temp1683.tag=
Cyc_Absyn_PointerType; _temp1683.f1=({ struct Cyc_Absyn_PtrInfo _temp1684;
_temp1684.elt_typ=( void*) _temp1680; _temp1684.rgn_typ=( void*) rgn; _temp1684.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1684.tq= Cyc_Absyn_empty_tqual();
_temp1684.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1685=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1685[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1686; _temp1686.tag= Cyc_Absyn_Upper_b;
_temp1686.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1686;}); _temp1685;})); _temp1684;});
_temp1683;}); _temp1682;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1681=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1681->v=(
void*) res_typ; _temp1681;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}}
_LL1582:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t=
Cyc_Tcutil_compress( t);{ void* _temp1687= t; struct Cyc_Absyn_Exp* _temp1693;
struct Cyc_Absyn_Tqual _temp1695; void* _temp1697; _LL1689: if(( unsigned int)
_temp1687 > 4u?*(( int*) _temp1687) == Cyc_Absyn_ArrayType: 0){ _LL1698:
_temp1697=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1687)->f1; goto
_LL1696; _LL1696: _temp1695=(( struct Cyc_Absyn_ArrayType_struct*) _temp1687)->f2;
goto _LL1694; _LL1694: _temp1693=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1687)->f3; goto _LL1690;} else{ goto _LL1691;} _LL1691: goto _LL1692;
_LL1690: { void* _temp1701; struct _tuple6 _temp1699= Cyc_Tcutil_addressof_props(
te, e); _LL1702: _temp1701= _temp1699.f2; goto _LL1700; _LL1700: { void*
_temp1703= _temp1693 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1704=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1704[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1705; _temp1705.tag= Cyc_Absyn_Upper_b;
_temp1705.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1693); _temp1705;});
_temp1704;}); t= Cyc_Absyn_atb_typ( _temp1697, _temp1701, _temp1695, _temp1703);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1692: return t; _LL1688:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1706=( void*) e->r; void* _temp1716; _LL1708: if(*(( int*)
_temp1706) == Cyc_Absyn_Array_e){ goto _LL1709;} else{ goto _LL1710;} _LL1710:
if(*(( int*) _temp1706) == Cyc_Absyn_Comprehension_e){ goto _LL1711;} else{ goto
_LL1712;} _LL1712: if(*(( int*) _temp1706) == Cyc_Absyn_Const_e){ _LL1717:
_temp1716=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1706)->f1; if((
unsigned int) _temp1716 > 1u?*(( int*) _temp1716) == Cyc_Absyn_String_c: 0){
goto _LL1713;} else{ goto _LL1714;}} else{ goto _LL1714;} _LL1714: goto _LL1715;
_LL1709: return t; _LL1711: return t; _LL1713: return t; _LL1715: t= Cyc_Tcutil_compress(
t);{ void* _temp1718= t; struct Cyc_Absyn_Exp* _temp1724; struct Cyc_Absyn_Tqual
_temp1726; void* _temp1728; _LL1720: if(( unsigned int) _temp1718 > 4u?*(( int*)
_temp1718) == Cyc_Absyn_ArrayType: 0){ _LL1729: _temp1728=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1718)->f1; goto _LL1727; _LL1727: _temp1726=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1718)->f2; goto _LL1725; _LL1725: _temp1724=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1718)->f3; goto _LL1721;} else{ goto _LL1722;} _LL1722: goto _LL1723;
_LL1721: { void* _temp1732; struct _tuple6 _temp1730= Cyc_Tcutil_addressof_props(
te, e); _LL1733: _temp1732= _temp1730.f2; goto _LL1731; _LL1731: { void* b=
_temp1724 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1734=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1734[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1735; _temp1735.tag= Cyc_Absyn_Upper_b;
_temp1735.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1724); _temp1735;});
_temp1734;}); t= Cyc_Absyn_atb_typ( _temp1728, _temp1732, _temp1726, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1723: return t; _LL1719:;} _LL1707:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1736=( void*) e->r; struct Cyc_Absyn_Exp* _temp1742; _LL1738: if(*(( int*)
_temp1736) == Cyc_Absyn_NoInstantiate_e){ _LL1743: _temp1742=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1736)->f1; goto _LL1739;} else{ goto _LL1740;} _LL1740: goto _LL1741;
_LL1739: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1742);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1742->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1742->topt))->v))); e->topt=
_temp1742->topt; goto _LL1737; _LL1741: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1744= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1750; struct Cyc_Absyn_Conref*
_temp1752; struct Cyc_Absyn_Tqual _temp1754; struct Cyc_Absyn_Conref* _temp1756;
void* _temp1758; void* _temp1760; _LL1746: if(( unsigned int) _temp1744 > 4u?*((
int*) _temp1744) == Cyc_Absyn_PointerType: 0){ _LL1751: _temp1750=(( struct Cyc_Absyn_PointerType_struct*)
_temp1744)->f1; _LL1761: _temp1760=( void*) _temp1750.elt_typ; goto _LL1759;
_LL1759: _temp1758=( void*) _temp1750.rgn_typ; goto _LL1757; _LL1757: _temp1756=
_temp1750.nullable; goto _LL1755; _LL1755: _temp1754= _temp1750.tq; goto _LL1753;
_LL1753: _temp1752= _temp1750.bounds; goto _LL1747;} else{ goto _LL1748;}
_LL1748: goto _LL1749; _LL1747:{ void* _temp1762= Cyc_Tcutil_compress( _temp1760);
struct Cyc_Absyn_FnInfo _temp1768; struct Cyc_List_List* _temp1770; struct Cyc_List_List*
_temp1772; struct Cyc_Absyn_VarargInfo* _temp1774; int _temp1776; struct Cyc_List_List*
_temp1778; void* _temp1780; struct Cyc_Core_Opt* _temp1782; struct Cyc_List_List*
_temp1784; _LL1764: if(( unsigned int) _temp1762 > 4u?*(( int*) _temp1762) ==
Cyc_Absyn_FnType: 0){ _LL1769: _temp1768=(( struct Cyc_Absyn_FnType_struct*)
_temp1762)->f1; _LL1785: _temp1784= _temp1768.tvars; goto _LL1783; _LL1783:
_temp1782= _temp1768.effect; goto _LL1781; _LL1781: _temp1780=( void*) _temp1768.ret_typ;
goto _LL1779; _LL1779: _temp1778= _temp1768.args; goto _LL1777; _LL1777:
_temp1776= _temp1768.c_varargs; goto _LL1775; _LL1775: _temp1774= _temp1768.cyc_varargs;
goto _LL1773; _LL1773: _temp1772= _temp1768.rgn_po; goto _LL1771; _LL1771:
_temp1770= _temp1768.attributes; goto _LL1765;} else{ goto _LL1766;} _LL1766:
goto _LL1767; _LL1765: if( _temp1784 != 0){ struct _RegionHandle _temp1786=
_new_region(); struct _RegionHandle* rgn=& _temp1786; _push_region( rgn);{
struct _tuple4 _temp1787=({ struct _tuple4 _temp1797; _temp1797.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1797.f2= rgn; _temp1797;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1787, _temp1784); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1794=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1794[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1795; _temp1795.tag= Cyc_Absyn_FnType; _temp1795.f1=({ struct Cyc_Absyn_FnInfo
_temp1796; _temp1796.tvars= 0; _temp1796.effect= _temp1782; _temp1796.ret_typ=(
void*) _temp1780; _temp1796.args= _temp1778; _temp1796.c_varargs= _temp1776;
_temp1796.cyc_varargs= _temp1774; _temp1796.rgn_po= _temp1772; _temp1796.attributes=
_temp1770; _temp1796;}); _temp1795;}); _temp1794;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1791=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1791[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1792; _temp1792.tag= Cyc_Absyn_PointerType;
_temp1792.f1=({ struct Cyc_Absyn_PtrInfo _temp1793; _temp1793.elt_typ=( void*)
ftyp; _temp1793.rgn_typ=( void*) _temp1758; _temp1793.nullable= _temp1756;
_temp1793.tq= _temp1754; _temp1793.bounds= _temp1752; _temp1793;}); _temp1792;});
_temp1791;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1788=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1788[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1789; _temp1789.tag= Cyc_Absyn_Instantiate_e;
_temp1789.f1= inner; _temp1789.f2= ts; _temp1789;}); _temp1788;}))); e->topt=({
struct Cyc_Core_Opt* _temp1790=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1790->v=( void*) new_typ; _temp1790;});}; _pop_region( rgn);}
goto _LL1763; _LL1767: goto _LL1763; _LL1763:;} goto _LL1745; _LL1749: goto
_LL1745; _LL1745:;} goto _LL1737; _LL1737:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1798=( void*) e->r; struct Cyc_Absyn_Exp* _temp1872; struct
_tuple1* _temp1874; struct Cyc_List_List* _temp1876; struct Cyc_Absyn_Exp*
_temp1878; struct Cyc_List_List* _temp1880; struct Cyc_Core_Opt* _temp1882; void*
_temp1884; void* _temp1886; struct _tuple1* _temp1888; struct Cyc_List_List*
_temp1890; void* _temp1892; void* _temp1894; struct Cyc_Absyn_Exp* _temp1896;
struct Cyc_Absyn_Exp* _temp1898; struct Cyc_Core_Opt* _temp1900; struct Cyc_Absyn_Exp*
_temp1902; struct Cyc_Absyn_Exp* _temp1904; struct Cyc_Absyn_Exp* _temp1906;
struct Cyc_Absyn_Exp* _temp1908; struct Cyc_Absyn_Exp* _temp1910; struct Cyc_Absyn_Exp*
_temp1912; struct Cyc_Absyn_VarargCallInfo* _temp1914; struct Cyc_Absyn_VarargCallInfo**
_temp1916; struct Cyc_List_List* _temp1917; struct Cyc_Absyn_Exp* _temp1919;
struct Cyc_Absyn_Exp* _temp1921; struct Cyc_List_List* _temp1923; struct Cyc_Absyn_Exp*
_temp1925; struct Cyc_Absyn_Exp* _temp1927; void* _temp1929; struct Cyc_Absyn_Exp*
_temp1931; struct Cyc_Absyn_Exp* _temp1933; struct Cyc_Absyn_Exp* _temp1935;
struct Cyc_Absyn_Exp* _temp1937; void* _temp1939; struct _tagged_arr* _temp1941;
void* _temp1943; struct Cyc_Absyn_Exp* _temp1945; struct _tagged_arr* _temp1947;
struct Cyc_Absyn_Exp* _temp1949; struct _tagged_arr* _temp1951; struct Cyc_Absyn_Exp*
_temp1953; struct Cyc_Absyn_Exp* _temp1955; struct Cyc_Absyn_Exp* _temp1957;
struct Cyc_List_List* _temp1959; struct Cyc_List_List* _temp1961; struct _tuple2*
_temp1963; struct Cyc_List_List* _temp1965; struct Cyc_Absyn_Stmt* _temp1967;
struct Cyc_Absyn_Fndecl* _temp1969; struct Cyc_Absyn_Exp* _temp1971; struct Cyc_Absyn_Exp*
_temp1973; struct Cyc_Absyn_Exp* _temp1975; struct Cyc_Absyn_Vardecl* _temp1977;
struct Cyc_Absyn_Structdecl* _temp1979; struct Cyc_Absyn_Structdecl** _temp1981;
struct Cyc_List_List* _temp1982; struct Cyc_Core_Opt* _temp1984; struct Cyc_Core_Opt**
_temp1986; struct _tuple1* _temp1987; struct _tuple1** _temp1989; struct Cyc_List_List*
_temp1990; void* _temp1992; struct Cyc_Absyn_Tunionfield* _temp1994; struct Cyc_Absyn_Tuniondecl*
_temp1996; struct Cyc_List_List* _temp1998; struct Cyc_Core_Opt* _temp2000;
struct Cyc_Core_Opt** _temp2002; struct Cyc_Core_Opt* _temp2003; struct Cyc_Core_Opt**
_temp2005; struct Cyc_Absyn_Enumfield* _temp2006; struct Cyc_Absyn_Enumdecl*
_temp2008; struct _tuple1* _temp2010; struct _tuple1** _temp2012; void*
_temp2013; struct Cyc_Absyn_Exp* _temp2015; _LL1800: if(*(( int*) _temp1798) ==
Cyc_Absyn_NoInstantiate_e){ _LL1873: _temp1872=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1798)->f1; goto _LL1801;} else{ goto _LL1802;} _LL1802: if(*(( int*)
_temp1798) == Cyc_Absyn_UnknownId_e){ _LL1875: _temp1874=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1798)->f1; goto _LL1803;} else{ goto _LL1804;} _LL1804: if(*(( int*)
_temp1798) == Cyc_Absyn_UnknownCall_e){ _LL1879: _temp1878=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1798)->f1; goto _LL1877; _LL1877: _temp1876=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1798)->f2; goto _LL1805;} else{ goto _LL1806;} _LL1806: if(*(( int*)
_temp1798) == Cyc_Absyn_UnresolvedMem_e){ _LL1883: _temp1882=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1798)->f1; goto _LL1881; _LL1881: _temp1880=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1798)->f2; goto _LL1807;} else{ goto _LL1808;} _LL1808: if(*(( int*)
_temp1798) == Cyc_Absyn_Const_e){ _LL1885: _temp1884=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1798)->f1; goto _LL1809;} else{ goto _LL1810;} _LL1810: if(*(( int*)
_temp1798) == Cyc_Absyn_Var_e){ _LL1889: _temp1888=(( struct Cyc_Absyn_Var_e_struct*)
_temp1798)->f1; goto _LL1887; _LL1887: _temp1886=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1798)->f2; goto _LL1811;} else{ goto _LL1812;} _LL1812: if(*(( int*)
_temp1798) == Cyc_Absyn_Primop_e){ _LL1893: _temp1892=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1798)->f1; goto _LL1891; _LL1891: _temp1890=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1798)->f2; goto _LL1813;} else{ goto _LL1814;} _LL1814: if(*(( int*)
_temp1798) == Cyc_Absyn_Increment_e){ _LL1897: _temp1896=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1798)->f1; goto _LL1895; _LL1895: _temp1894=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1798)->f2; goto _LL1815;} else{ goto _LL1816;} _LL1816: if(*(( int*)
_temp1798) == Cyc_Absyn_AssignOp_e){ _LL1903: _temp1902=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1798)->f1; goto _LL1901; _LL1901: _temp1900=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1798)->f2; goto _LL1899; _LL1899: _temp1898=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1798)->f3; goto _LL1817;} else{ goto _LL1818;} _LL1818: if(*(( int*)
_temp1798) == Cyc_Absyn_Conditional_e){ _LL1909: _temp1908=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1798)->f1; goto _LL1907; _LL1907: _temp1906=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1798)->f2; goto _LL1905; _LL1905: _temp1904=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1798)->f3; goto _LL1819;} else{ goto _LL1820;} _LL1820: if(*(( int*)
_temp1798) == Cyc_Absyn_SeqExp_e){ _LL1913: _temp1912=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1798)->f1; goto _LL1911; _LL1911: _temp1910=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1798)->f2; goto _LL1821;} else{ goto _LL1822;} _LL1822: if(*(( int*)
_temp1798) == Cyc_Absyn_FnCall_e){ _LL1920: _temp1919=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1798)->f1; goto _LL1918; _LL1918: _temp1917=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1798)->f2; goto _LL1915; _LL1915: _temp1914=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1798)->f3; _temp1916=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1798)->f3;
goto _LL1823;} else{ goto _LL1824;} _LL1824: if(*(( int*) _temp1798) == Cyc_Absyn_Throw_e){
_LL1922: _temp1921=(( struct Cyc_Absyn_Throw_e_struct*) _temp1798)->f1; goto
_LL1825;} else{ goto _LL1826;} _LL1826: if(*(( int*) _temp1798) == Cyc_Absyn_Instantiate_e){
_LL1926: _temp1925=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1798)->f1;
goto _LL1924; _LL1924: _temp1923=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1798)->f2; goto _LL1827;} else{ goto _LL1828;} _LL1828: if(*(( int*)
_temp1798) == Cyc_Absyn_Cast_e){ _LL1930: _temp1929=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1798)->f1; goto _LL1928; _LL1928: _temp1927=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1798)->f2; goto _LL1829;} else{ goto _LL1830;} _LL1830: if(*(( int*)
_temp1798) == Cyc_Absyn_Address_e){ _LL1932: _temp1931=(( struct Cyc_Absyn_Address_e_struct*)
_temp1798)->f1; goto _LL1831;} else{ goto _LL1832;} _LL1832: if(*(( int*)
_temp1798) == Cyc_Absyn_New_e){ _LL1936: _temp1935=(( struct Cyc_Absyn_New_e_struct*)
_temp1798)->f1; goto _LL1934; _LL1934: _temp1933=(( struct Cyc_Absyn_New_e_struct*)
_temp1798)->f2; goto _LL1833;} else{ goto _LL1834;} _LL1834: if(*(( int*)
_temp1798) == Cyc_Absyn_Sizeofexp_e){ _LL1938: _temp1937=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1798)->f1; goto _LL1835;} else{ goto _LL1836;} _LL1836: if(*(( int*)
_temp1798) == Cyc_Absyn_Sizeoftyp_e){ _LL1940: _temp1939=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1798)->f1; goto _LL1837;} else{ goto _LL1838;} _LL1838: if(*(( int*)
_temp1798) == Cyc_Absyn_Offsetof_e){ _LL1944: _temp1943=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1798)->f1; goto _LL1942; _LL1942: _temp1941=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1798)->f2; goto _LL1839;} else{ goto _LL1840;} _LL1840: if(*(( int*)
_temp1798) == Cyc_Absyn_Deref_e){ _LL1946: _temp1945=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1798)->f1; goto _LL1841;} else{ goto _LL1842;} _LL1842: if(*(( int*)
_temp1798) == Cyc_Absyn_StructMember_e){ _LL1950: _temp1949=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1798)->f1; goto _LL1948; _LL1948: _temp1947=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1798)->f2; goto _LL1843;} else{ goto _LL1844;} _LL1844: if(*(( int*)
_temp1798) == Cyc_Absyn_StructArrow_e){ _LL1954: _temp1953=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1798)->f1; goto _LL1952; _LL1952: _temp1951=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1798)->f2; goto _LL1845;} else{ goto _LL1846;} _LL1846: if(*(( int*)
_temp1798) == Cyc_Absyn_Subscript_e){ _LL1958: _temp1957=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1798)->f1; goto _LL1956; _LL1956: _temp1955=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1798)->f2; goto _LL1847;} else{ goto _LL1848;} _LL1848: if(*(( int*)
_temp1798) == Cyc_Absyn_Tuple_e){ _LL1960: _temp1959=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1798)->f1; goto _LL1849;} else{ goto _LL1850;} _LL1850: if(*(( int*)
_temp1798) == Cyc_Absyn_CompoundLit_e){ _LL1964: _temp1963=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1798)->f1; goto _LL1962; _LL1962: _temp1961=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1798)->f2; goto _LL1851;} else{ goto _LL1852;} _LL1852: if(*(( int*)
_temp1798) == Cyc_Absyn_Array_e){ _LL1966: _temp1965=(( struct Cyc_Absyn_Array_e_struct*)
_temp1798)->f1; goto _LL1853;} else{ goto _LL1854;} _LL1854: if(*(( int*)
_temp1798) == Cyc_Absyn_StmtExp_e){ _LL1968: _temp1967=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1798)->f1; goto _LL1855;} else{ goto _LL1856;} _LL1856: if(*(( int*)
_temp1798) == Cyc_Absyn_Codegen_e){ _LL1970: _temp1969=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1798)->f1; goto _LL1857;} else{ goto _LL1858;} _LL1858: if(*(( int*)
_temp1798) == Cyc_Absyn_Fill_e){ _LL1972: _temp1971=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1798)->f1; goto _LL1859;} else{ goto _LL1860;} _LL1860: if(*(( int*)
_temp1798) == Cyc_Absyn_Comprehension_e){ _LL1978: _temp1977=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1798)->f1; goto _LL1976; _LL1976: _temp1975=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1798)->f2; goto _LL1974; _LL1974: _temp1973=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1798)->f3; goto _LL1861;} else{ goto _LL1862;} _LL1862: if(*(( int*)
_temp1798) == Cyc_Absyn_Struct_e){ _LL1988: _temp1987=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1798)->f1; _temp1989=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f1;
goto _LL1985; _LL1985: _temp1984=(( struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f2;
_temp1986=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f2; goto _LL1983;
_LL1983: _temp1982=(( struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f3; goto
_LL1980; _LL1980: _temp1979=(( struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f4;
_temp1981=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f4; goto _LL1863;}
else{ goto _LL1864;} _LL1864: if(*(( int*) _temp1798) == Cyc_Absyn_AnonStruct_e){
_LL1993: _temp1992=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1798)->f1;
goto _LL1991; _LL1991: _temp1990=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1798)->f2; goto _LL1865;} else{ goto _LL1866;} _LL1866: if(*(( int*)
_temp1798) == Cyc_Absyn_Tunion_e){ _LL2004: _temp2003=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1798)->f1; _temp2005=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f1;
goto _LL2001; _LL2001: _temp2000=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f2;
_temp2002=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f2; goto _LL1999;
_LL1999: _temp1998=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f3; goto
_LL1997; _LL1997: _temp1996=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f4;
goto _LL1995; _LL1995: _temp1994=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f5;
goto _LL1867;} else{ goto _LL1868;} _LL1868: if(*(( int*) _temp1798) == Cyc_Absyn_Enum_e){
_LL2011: _temp2010=(( struct Cyc_Absyn_Enum_e_struct*) _temp1798)->f1; _temp2012=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1798)->f1; goto _LL2009; _LL2009:
_temp2008=(( struct Cyc_Absyn_Enum_e_struct*) _temp1798)->f2; goto _LL2007;
_LL2007: _temp2006=(( struct Cyc_Absyn_Enum_e_struct*) _temp1798)->f3; goto
_LL1869;} else{ goto _LL1870;} _LL1870: if(*(( int*) _temp1798) == Cyc_Absyn_Malloc_e){
_LL2016: _temp2015=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1798)->f1; goto
_LL2014; _LL2014: _temp2013=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1798)->f2; goto _LL1871;} else{ goto _LL1799;} _LL1801: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1872); return; _LL1803: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1874); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1805: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1878, _temp1876); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1807: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1880); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1809: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1884, e);
goto _LL1799; _LL1811: t= Cyc_Tcexp_tcVar( te, loc, _temp1888, _temp1886); goto
_LL1799; _LL1813: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1892, _temp1890);
goto _LL1799; _LL1815: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1896,
_temp1894); goto _LL1799; _LL1817: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1902, _temp1900, _temp1898); goto _LL1799; _LL1819: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1908, _temp1906, _temp1904); goto _LL1799; _LL1821: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1912, _temp1910); goto _LL1799; _LL1823: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1919, _temp1917, _temp1916); goto _LL1799; _LL1825: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1921); goto _LL1799; _LL1827: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1925, _temp1923); goto _LL1799; _LL1829: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1929, _temp1927); goto _LL1799; _LL1831: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1931); goto _LL1799; _LL1833: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1935, e, _temp1933); goto _LL1799; _LL1835: { void* _temp2017= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1937); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp2017); goto
_LL1799;} _LL1837: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1939); goto
_LL1799; _LL1839: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1943, _temp1941);
goto _LL1799; _LL1841: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1945); goto
_LL1799; _LL1843: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1949,
_temp1947); goto _LL1799; _LL1845: t= Cyc_Tcexp_tcStructArrow( te, loc, topt,
_temp1953, _temp1951); goto _LL1799; _LL1847: t= Cyc_Tcexp_tcSubscript( te, loc,
topt, _temp1957, _temp1955); goto _LL1799; _LL1849: t= Cyc_Tcexp_tcTuple( te,
loc, topt, _temp1959); goto _LL1799; _LL1851: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1963, _temp1961); goto _LL1799; _LL1853: { void** elt_topt= 0;
if( topt != 0){ void* _temp2018= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); void* _temp2024; void** _temp2026; _LL2020: if(( unsigned int)
_temp2018 > 4u?*(( int*) _temp2018) == Cyc_Absyn_ArrayType: 0){ _LL2025:
_temp2024=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2018)->f1;
_temp2026=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2018)->f1); goto
_LL2021;} else{ goto _LL2022;} _LL2022: goto _LL2023; _LL2021: elt_topt=( void**)
_temp2026; goto _LL2019; _LL2023: goto _LL2019; _LL2019:;} t= Cyc_Tcexp_tcArray(
te, loc, elt_topt, _temp1965); goto _LL1799;} _LL1855: t= Cyc_Tcexp_tcStmtExp(
te, loc, topt, _temp1967); goto _LL1799; _LL1857: t= Cyc_Tcexp_tcCodegen( te,
loc, topt, _temp1969); goto _LL1799; _LL1859: t= Cyc_Tcexp_tcFill( te, loc, topt,
_temp1971); goto _LL1799; _LL1861: t= Cyc_Tcexp_tcComprehension( te, loc, topt,
_temp1977, _temp1975, _temp1973); goto _LL1799; _LL1863: t= Cyc_Tcexp_tcStruct(
te, loc, topt, _temp1989, _temp1986, _temp1982, _temp1981); goto _LL1799;
_LL1865: t= Cyc_Tcexp_tcAnonStruct( te, loc, _temp1992, _temp1990); goto _LL1799;
_LL1867: t= Cyc_Tcexp_tcTunion( te, loc, topt, e, _temp2005, _temp2002,
_temp1998, _temp1996, _temp1994); goto _LL1799; _LL1869:* _temp2012=(( struct
Cyc_Absyn_Enumfield*) _check_null( _temp2006))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2027=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2027[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2028; _temp2028.tag= Cyc_Absyn_EnumType;
_temp2028.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2008))->name;
_temp2028.f2= _temp2008; _temp2028;}); _temp2027;}); goto _LL1799; _LL1871: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp2015, _temp2013); goto _LL1799; _LL1799:;}
e->topt=({ struct Cyc_Core_Opt* _temp2029=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2029->v=( void*) t; _temp2029;});}