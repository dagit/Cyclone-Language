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
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(*
f)( void*, void*), void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth( struct Cyc_List_List*
x, int i); extern struct Cyc_List_List* Cyc_List_nth_tail( struct Cyc_List_List*
x, int i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rzip( struct
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
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Stdio_vraprintf(
struct _RegionHandle* r1, struct _tagged_arr fmt, struct _tagged_arr ap); static
const int Cyc_Stdio_ShortPtr_sa= 0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag;
short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct
Cyc_Stdio_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa=
3; struct Cyc_Stdio_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static
const int Cyc_Stdio_StringPtr_sa= 4; struct Cyc_Stdio_StringPtr_sa_struct{ int
tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_DoublePtr_sa= 5;
struct Cyc_Stdio_DoublePtr_sa_struct{ int tag; double* f1; } ; static const int
Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{ int tag; float*
f1; } ; extern int Cyc_String_zstrcmp( struct _tagged_arr, struct _tagged_arr);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
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
_tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
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
msg, struct _tagged_arr ap){({ void* _temp0[ 0u]={}; Cyc_Tcutil_terr( loc,(
struct _tagged_arr) Cyc_Stdio_vraprintf( Cyc_Core_heap_region, msg, ap),
_tag_arr( _temp0, sizeof( void*), 0u));}); return Cyc_Absyn_wildtyp(({ struct
Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1;}));} static struct
Cyc_Core_Opt Cyc_Tcexp_rk={( void*)(( void*) 3u)}; static struct Cyc_Core_Opt
Cyc_Tcexp_ak={( void*)(( void*) 0u)}; static struct Cyc_Core_Opt Cyc_Tcexp_bk={(
void*)(( void*) 2u)}; static struct Cyc_Core_Opt Cyc_Tcexp_mk={( void*)(( void*)
1u)}; static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, struct _tuple1* q){ struct _handler_cons _temp2;
_push_handler(& _temp2);{ int _temp4= 0; if( setjmp( _temp2.handler)){ _temp4= 1;}
if( ! _temp4){{ void* _temp5= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void*
_temp15; struct Cyc_Absyn_Enumfield* _temp17; struct Cyc_Absyn_Enumdecl* _temp19;
struct Cyc_Absyn_Tunionfield* _temp21; struct Cyc_Absyn_Tuniondecl* _temp23;
_LL7: if(*(( int*) _temp5) == Cyc_Tcenv_VarRes){ _LL16: _temp15=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp5)->f1; goto _LL8;} else{ goto _LL9;} _LL9:
if(*(( int*) _temp5) == Cyc_Tcenv_EnumRes){ _LL20: _temp19=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp5)->f1; goto _LL18; _LL18: _temp17=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp5)->f2; goto _LL10;} else{ goto _LL11;} _LL11: if(*(( int*) _temp5) == Cyc_Tcenv_TunionRes){
_LL24: _temp23=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f1; goto _LL22;
_LL22: _temp21=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f2; goto _LL12;}
else{ goto _LL13;} _LL13: if(*(( int*) _temp5) == Cyc_Tcenv_StructRes){ goto
_LL14;} else{ goto _LL6;} _LL8:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp25=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp25[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp26; _temp26.tag= Cyc_Absyn_Var_e;
_temp26.f1= q; _temp26.f2=( void*) _temp15; _temp26;}); _temp25;}))); goto _LL6;
_LL10:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp27=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp27[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp28; _temp28.tag= Cyc_Absyn_Enum_e;
_temp28.f1= q; _temp28.f2=( struct Cyc_Absyn_Enumdecl*) _temp19; _temp28.f3=(
struct Cyc_Absyn_Enumfield*) _temp17; _temp28;}); _temp27;}))); goto _LL6; _LL12:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp29=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp29[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp30; _temp30.tag= Cyc_Absyn_Tunion_e;
_temp30.f1= 0; _temp30.f2= 0; _temp30.f3= 0; _temp30.f4= _temp23; _temp30.f5=
_temp21; _temp30;}); _temp29;}))); goto _LL6; _LL14:({ struct Cyc_Stdio_String_pa_struct
_temp32; _temp32.tag= Cyc_Stdio_String_pa; _temp32.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp31[ 1u]={& _temp32}; Cyc_Tcutil_terr( e->loc, _tag_arr("bad occurrence of struct name %s",
sizeof( unsigned char), 33u), _tag_arr( _temp31, sizeof( void*), 1u));}});( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp33=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp33[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp34; _temp34.tag= Cyc_Absyn_Var_e; _temp34.f1= q; _temp34.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp34;}); _temp33;}))); goto _LL6; _LL6:;};
_pop_handler();} else{ void* _temp3=( void*) _exn_thrown; void* _temp36= _temp3;
_LL38: if( _temp36 == Cyc_Dict_Absent){ goto _LL39;} else{ goto _LL40;} _LL40:
goto _LL41; _LL39:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp42=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp42[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp43; _temp43.tag= Cyc_Absyn_Var_e;
_temp43.f1= q; _temp43.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp43;});
_temp42;}))); goto _LL37; _LL41:( void) _throw( _temp36); _LL37:;}}} struct
_tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple7* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({ struct
_tuple7* _temp44=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp44->f1=
0; _temp44->f2= e; _temp44;});} static void Cyc_Tcexp_resolve_unknown_fn( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1, struct
Cyc_List_List* es){ void* _temp45=( void*) e1->r; struct _tuple1* _temp51; _LL47:
if(*(( int*) _temp45) == Cyc_Absyn_UnknownId_e){ _LL52: _temp51=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp45)->f1; goto _LL48;} else{ goto _LL49;} _LL49: goto _LL50; _LL48: { struct
_handler_cons _temp53; _push_handler(& _temp53);{ int _temp55= 0; if( setjmp(
_temp53.handler)){ _temp55= 1;} if( ! _temp55){{ void* _temp56= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp51); void* _temp66; struct Cyc_Absyn_Tunionfield* _temp68;
struct Cyc_Absyn_Tuniondecl* _temp70; struct Cyc_Absyn_Structdecl* _temp72;
_LL58: if(*(( int*) _temp56) == Cyc_Tcenv_VarRes){ _LL67: _temp66=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp56)->f1; goto _LL59;} else{ goto _LL60;}
_LL60: if(*(( int*) _temp56) == Cyc_Tcenv_TunionRes){ _LL71: _temp70=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f1; goto _LL69; _LL69: _temp68=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f2; goto _LL61;} else{ goto _LL62;} _LL62:
if(*(( int*) _temp56) == Cyc_Tcenv_StructRes){ _LL73: _temp72=(( struct Cyc_Tcenv_StructRes_struct*)
_temp56)->f1; goto _LL63;} else{ goto _LL64;} _LL64: if(*(( int*) _temp56) ==
Cyc_Tcenv_EnumRes){ goto _LL65;} else{ goto _LL57;} _LL59:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp74=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp74[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp75; _temp75.tag= Cyc_Absyn_FnCall_e; _temp75.f1= e1; _temp75.f2= es;
_temp75.f3= 0; _temp75;}); _temp74;}))); _npop_handler( 0u); return; _LL61: if(
_temp68->typs == 0){({ struct Cyc_Stdio_String_pa_struct _temp77; _temp77.tag=
Cyc_Stdio_String_pa; _temp77.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp68->name);{ void* _temp76[ 1u]={& _temp77}; Cyc_Tcutil_terr( e->loc,
_tag_arr("%s is a constant, not a function", sizeof( unsigned char), 33u),
_tag_arr( _temp76, sizeof( void*), 1u));}});}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp78=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp78[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp79; _temp79.tag= Cyc_Absyn_Tunion_e; _temp79.f1= 0; _temp79.f2= 0; _temp79.f3=
es; _temp79.f4= _temp70; _temp79.f5= _temp68; _temp79;}); _temp78;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL63: { struct Cyc_List_List*
_temp80=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp72->name != 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp81=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp81[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp82; _temp82.tag= Cyc_Absyn_Struct_e;
_temp82.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp72->name))->v;
_temp82.f2= 0; _temp82.f3= _temp80; _temp82.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp82;}); _temp81;})));} else{({ void* _temp83[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("missing struct name", sizeof( unsigned char), 20u), _tag_arr(
_temp83, sizeof( void*), 0u));});( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp84=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp84[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp85; _temp85.tag= Cyc_Absyn_Struct_e;
_temp85.f1= _temp51; _temp85.f2= 0; _temp85.f3= _temp80; _temp85.f4=( struct Cyc_Absyn_Structdecl*)
_temp72; _temp85;}); _temp84;})));} _npop_handler( 0u); return;} _LL65:({ struct
Cyc_Stdio_String_pa_struct _temp87; _temp87.tag= Cyc_Stdio_String_pa; _temp87.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp51);{ void* _temp86[ 1u]={&
_temp87}; Cyc_Tcutil_terr( e->loc, _tag_arr("%s is an enum constructor, not a function",
sizeof( unsigned char), 42u), _tag_arr( _temp86, sizeof( void*), 1u));}});
_npop_handler( 0u); return; _LL57:;}; _pop_handler();} else{ void* _temp54=(
void*) _exn_thrown; void* _temp89= _temp54; _LL91: if( _temp89 == Cyc_Dict_Absent){
goto _LL92;} else{ goto _LL93;} _LL93: goto _LL94; _LL92:({ struct Cyc_Stdio_String_pa_struct
_temp96; _temp96.tag= Cyc_Stdio_String_pa; _temp96.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp51);{ void* _temp95[ 1u]={& _temp96}; Cyc_Tcutil_terr( e1->loc, _tag_arr("unbound identifier %s",
sizeof( unsigned char), 22u), _tag_arr( _temp95, sizeof( void*), 1u));}});( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp97=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp97[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp98; _temp98.tag= Cyc_Absyn_FnCall_e; _temp98.f1= e1; _temp98.f2= es;
_temp98.f3= 0; _temp98;}); _temp97;}))); return; _LL94:( void) _throw( _temp89);
_LL90:;}}} _LL50:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
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
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual _temp113; void* _temp115; struct
Cyc_Absyn_Structdecl** _temp117; struct Cyc_List_List* _temp119; struct _tuple1*
_temp121; _LL105: if(( unsigned int) _temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_ArrayType:
0){ _LL116: _temp115=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f1;
goto _LL114; _LL114: _temp113=(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f2;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp103 > 4u?*((
int*) _temp103) == Cyc_Absyn_StructType: 0){ _LL122: _temp121=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f1; goto _LL120; _LL120: _temp119=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f2; goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f3; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_AnonStructType: 0){ goto _LL110;}
else{ goto _LL111;} _LL111: goto _LL112; _LL106:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp123=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp123[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp124; _temp124.tag= Cyc_Absyn_Array_e; _temp124.f1= des; _temp124;});
_temp123;}))); goto _LL104; _LL108: if( _temp117 == 0? 1: _temp121 == 0){({ void*
_temp125[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("Compiler bug: struct type not properly set",
sizeof( unsigned char), 43u), _tag_arr( _temp125, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp126=(
struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp126[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp127; _temp127.tag= Cyc_Absyn_Struct_e;
_temp127.f1=( struct _tuple1*) _check_null( _temp121); _temp127.f2= 0; _temp127.f3=
des; _temp127.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp117)); _temp127;}); _temp126;}))); goto _LL104; _LL110:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp128=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp128[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp129; _temp129.tag= Cyc_Absyn_AnonStruct_e;
_temp129.f1=( void*) t; _temp129.f2= des; _temp129;}); _temp128;}))); goto
_LL104; _LL112:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp130=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp130[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp131; _temp131.tag= Cyc_Absyn_Array_e;
_temp131.f1= des; _temp131;}); _temp130;}))); goto _LL104; _LL104:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){({ struct Cyc_Stdio_String_pa_struct _temp134; _temp134.tag= Cyc_Stdio_String_pa;
_temp134.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Stdio_String_pa_struct _temp133;
_temp133.tag= Cyc_Stdio_String_pa; _temp133.f1=( struct _tagged_arr) msg_part;{
void* _temp132[ 2u]={& _temp133,& _temp134}; Cyc_Tcutil_terr( e->loc, _tag_arr("test of %s has type %s instead of integral or * type",
sizeof( unsigned char), 53u), _tag_arr( _temp132, sizeof( void*), 2u));}}});}}
static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp135= c;
void* _temp151; void* _temp153; void* _temp155; int _temp157; void* _temp159;
struct _tagged_arr _temp161; _LL137: if(( unsigned int) _temp135 > 1u?*(( int*)
_temp135) == Cyc_Absyn_Char_c: 0){ _LL152: _temp151=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp135)->f1; goto _LL138;} else{ goto _LL139;} _LL139: if(( unsigned int)
_temp135 > 1u?*(( int*) _temp135) == Cyc_Absyn_Short_c: 0){ _LL154: _temp153=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp135)->f1; goto _LL140;} else{
goto _LL141;} _LL141: if(( unsigned int) _temp135 > 1u?*(( int*) _temp135) ==
Cyc_Absyn_LongLong_c: 0){ _LL156: _temp155=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp135)->f1; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp135 > 1u?*(( int*) _temp135) == Cyc_Absyn_Float_c: 0){ goto _LL144;} else{
goto _LL145;} _LL145: if(( unsigned int) _temp135 > 1u?*(( int*) _temp135) ==
Cyc_Absyn_Int_c: 0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp135)->f1; goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_Int_c_struct*)
_temp135)->f2; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp135 > 1u?*(( int*) _temp135) == Cyc_Absyn_String_c: 0){ _LL162: _temp161=((
struct Cyc_Absyn_String_c_struct*) _temp135)->f1; goto _LL148;} else{ goto
_LL149;} _LL149: if( _temp135 ==( void*) Cyc_Absyn_Null_c){ goto _LL150;} else{
goto _LL136;} _LL138: t= _temp151 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL136; _LL140: t= _temp153 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL136; _LL142: t= _temp155 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto
_LL136; _LL144: t= Cyc_Absyn_float_t; goto _LL136; _LL146: if( topt == 0){ t=
_temp159 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp163= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); void*
_temp173; void* _temp175; void* _temp177; void* _temp179; void* _temp181; _LL165:
if(( unsigned int) _temp163 > 4u?*(( int*) _temp163) == Cyc_Absyn_IntType: 0){
_LL176: _temp175=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp163)->f1;
goto _LL174; _LL174: _temp173=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp173 ==( void*) Cyc_Absyn_B1){ goto _LL166;} else{ goto
_LL167;}} else{ goto _LL167;} _LL167: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_IntType: 0){ _LL180: _temp179=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL178; _LL178: _temp177=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp177 ==( void*) Cyc_Absyn_B2){ goto _LL168;} else{ goto
_LL169;}} else{ goto _LL169;} _LL169: if(( unsigned int) _temp163 > 4u?*(( int*)
_temp163) == Cyc_Absyn_IntType: 0){ _LL182: _temp181=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL170;} else{ goto _LL171;} _LL171: goto _LL172; _LL166: t=
_temp175 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp183=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp183[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp184; _temp184.tag= Cyc_Absyn_Const_e;
_temp184.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp185=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp185[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp186; _temp186.tag= Cyc_Absyn_Char_c;
_temp186.f1=( void*) _temp175; _temp186.f2=( unsigned char) _temp157; _temp186;});
_temp185;})); _temp184;}); _temp183;}))); goto _LL164; _LL168: t= _temp179 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp187=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp187[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp188; _temp188.tag= Cyc_Absyn_Const_e; _temp188.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp189=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp189[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp190; _temp190.tag= Cyc_Absyn_Short_c; _temp190.f1=( void*) _temp179;
_temp190.f2=( short) _temp157; _temp190;}); _temp189;})); _temp188;}); _temp187;})));
goto _LL164; _LL170: t= _temp181 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL164; _LL172: t= _temp159 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL164; _LL164:;} goto _LL136; _LL148: {
int len=( int) _get_arr_size( _temp161, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp191=({ struct Cyc_Absyn_Const_e_struct* _temp228=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp228[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp229; _temp229.tag= Cyc_Absyn_Const_e; _temp229.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp230=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp230[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp231; _temp231.tag= Cyc_Absyn_Int_c; _temp231.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp231.f2= len; _temp231;}); _temp230;})); _temp229;}); _temp228;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp191, loc); elen->topt=({
struct Cyc_Core_Opt* _temp192=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp192->v=( void*) Cyc_Absyn_uint_t; _temp192;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp193=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp193[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp194; _temp194.tag= Cyc_Absyn_Upper_b; _temp194.f1= elen; _temp194;});
_temp193;})); if( topt != 0){ void* _temp195= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp203; struct Cyc_Absyn_Tqual
_temp205; void* _temp207; struct Cyc_Absyn_PtrInfo _temp209; struct Cyc_Absyn_Conref*
_temp211; struct Cyc_Absyn_Tqual _temp213; struct Cyc_Absyn_Conref* _temp215;
void* _temp217; void* _temp219; _LL197: if(( unsigned int) _temp195 > 4u?*(( int*)
_temp195) == Cyc_Absyn_ArrayType: 0){ _LL208: _temp207=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp195)->f1; goto _LL206; _LL206: _temp205=(( struct Cyc_Absyn_ArrayType_struct*)
_temp195)->f2; goto _LL204; _LL204: _temp203=(( struct Cyc_Absyn_ArrayType_struct*)
_temp195)->f3; goto _LL198;} else{ goto _LL199;} _LL199: if(( unsigned int)
_temp195 > 4u?*(( int*) _temp195) == Cyc_Absyn_PointerType: 0){ _LL210: _temp209=((
struct Cyc_Absyn_PointerType_struct*) _temp195)->f1; _LL220: _temp219=( void*)
_temp209.elt_typ; goto _LL218; _LL218: _temp217=( void*) _temp209.rgn_typ; goto
_LL216; _LL216: _temp215= _temp209.nullable; goto _LL214; _LL214: _temp213=
_temp209.tq; goto _LL212; _LL212: _temp211= _temp209.bounds; goto _LL200;} else{
goto _LL201;} _LL201: goto _LL202; _LL198: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp221=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp221[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp222; _temp222.tag= Cyc_Absyn_ArrayType;
_temp222.f1=( void*) Cyc_Absyn_uchar_t; _temp222.f2= _temp205; _temp222.f3=(
struct Cyc_Absyn_Exp*) elen; _temp222;}); _temp221;}); _LL200: if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp223=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp223->v=( void*) t; _temp223;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp224=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp224->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp224;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp225=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp225[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp226; _temp226.tag= Cyc_Absyn_Upper_b; _temp226.f1= elen; _temp226;});
_temp225;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp227=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp227->v=( void*) t; _temp227;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL196; _LL202:
goto _LL196; _LL196:;} return t;} _LL150: { struct Cyc_List_List* _temp232= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp233=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp233[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp234; _temp234.tag= Cyc_Absyn_PointerType;
_temp234.f1=({ struct Cyc_Absyn_PtrInfo _temp235; _temp235.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp237=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp237->v=( void*)
_temp232; _temp237;})); _temp235.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp236=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp236->v=( void*) _temp232;
_temp236;})); _temp235.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp235.tq= Cyc_Absyn_empty_tqual(); _temp235.bounds= Cyc_Absyn_empty_conref();
_temp235;}); _temp234;}); _temp233;}); goto _LL136;} _LL136:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp238= b; struct Cyc_Absyn_Vardecl*
_temp252; struct Cyc_Absyn_Fndecl* _temp254; struct Cyc_Absyn_Vardecl* _temp256;
struct Cyc_Absyn_Vardecl* _temp258; struct Cyc_Absyn_Vardecl* _temp260; _LL240:
if( _temp238 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL241;} else{ goto _LL242;}
_LL242: if(( unsigned int) _temp238 > 1u?*(( int*) _temp238) == Cyc_Absyn_Global_b:
0){ _LL253: _temp252=(( struct Cyc_Absyn_Global_b_struct*) _temp238)->f1; goto
_LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp238 > 1u?*(( int*)
_temp238) == Cyc_Absyn_Funname_b: 0){ _LL255: _temp254=(( struct Cyc_Absyn_Funname_b_struct*)
_temp238)->f1; goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int)
_temp238 > 1u?*(( int*) _temp238) == Cyc_Absyn_Pat_b: 0){ _LL257: _temp256=((
struct Cyc_Absyn_Pat_b_struct*) _temp238)->f1; goto _LL247;} else{ goto _LL248;}
_LL248: if(( unsigned int) _temp238 > 1u?*(( int*) _temp238) == Cyc_Absyn_Local_b:
0){ _LL259: _temp258=(( struct Cyc_Absyn_Local_b_struct*) _temp238)->f1; goto
_LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp238 > 1u?*(( int*)
_temp238) == Cyc_Absyn_Param_b: 0){ _LL261: _temp260=(( struct Cyc_Absyn_Param_b_struct*)
_temp238)->f1; goto _LL251;} else{ goto _LL239;} _LL241: return({ struct Cyc_Stdio_String_pa_struct
_temp263; _temp263.tag= Cyc_Stdio_String_pa; _temp263.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( q);{ void* _temp262[ 1u]={& _temp263}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unbound identifier %s", sizeof( unsigned char), 22u),
_tag_arr( _temp262, sizeof( void*), 1u));}}); _LL243: q[
_check_known_subscript_notnull( 1u, 0)]=( _temp252->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp252->type; _LL245:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp254->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp254);
_LL247: _temp258= _temp256; goto _LL249; _LL249: _temp260= _temp258; goto _LL251;
_LL251:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp260->type; _LL239:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_Core_Opt* opt_args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp264=( void*) fmt->r; void* _temp272; struct _tagged_arr
_temp274; struct Cyc_Absyn_Exp* _temp276; struct Cyc_Absyn_Exp _temp278; void*
_temp279; void* _temp281; struct _tagged_arr _temp283; _LL266: if(*(( int*)
_temp264) == Cyc_Absyn_Const_e){ _LL273: _temp272=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp264)->f1; if(( unsigned int) _temp272 > 1u?*(( int*) _temp272) == Cyc_Absyn_String_c:
0){ _LL275: _temp274=(( struct Cyc_Absyn_String_c_struct*) _temp272)->f1; goto
_LL267;} else{ goto _LL268;}} else{ goto _LL268;} _LL268: if(*(( int*) _temp264)
== Cyc_Absyn_Cast_e){ _LL277: _temp276=(( struct Cyc_Absyn_Cast_e_struct*)
_temp264)->f2; _temp278=* _temp276; _LL280: _temp279=( void*) _temp278.r; if(*((
int*) _temp279) == Cyc_Absyn_Const_e){ _LL282: _temp281=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp279)->f1; if(( unsigned int) _temp281 > 1u?*(( int*) _temp281) == Cyc_Absyn_String_c:
0){ _LL284: _temp283=(( struct Cyc_Absyn_String_c_struct*) _temp281)->f1; goto
_LL269;} else{ goto _LL270;}} else{ goto _LL270;}} else{ goto _LL270;} _LL270:
goto _LL271; _LL267: _temp283= _temp274; goto _LL269; _LL269: desc_types=
type_getter( te, _temp283, fmt->loc); goto _LL265; _LL271: return; _LL265:;} if(
opt_args != 0){ struct Cyc_List_List* _temp285=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( opt_args))->v; for( 0; desc_types != 0? _temp285 !=
0: 0;( desc_types=(( struct Cyc_List_List*) _check_null( desc_types))->tl,
_temp285=(( struct Cyc_List_List*) _check_null( _temp285))->tl)){ void* t=( void*)((
struct Cyc_List_List*) _check_null( desc_types))->hd; struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp285))->hd; if(
! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct Cyc_Stdio_String_pa_struct
_temp288; _temp288.tag= Cyc_Stdio_String_pa; _temp288.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{
struct Cyc_Stdio_String_pa_struct _temp287; _temp287.tag= Cyc_Stdio_String_pa;
_temp287.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp286[ 2u]={&
_temp287,& _temp288}; Cyc_Tcutil_terr( e->loc, _tag_arr("descriptor has type %s but argument has type %s",
sizeof( unsigned char), 48u), _tag_arr( _temp286, sizeof( void*), 2u));}}});}}
if( desc_types != 0){({ void* _temp289[ 0u]={}; Cyc_Tcutil_terr( fmt->loc,
_tag_arr("too few arguments", sizeof( unsigned char), 18u), _tag_arr( _temp289,
sizeof( void*), 0u));});} if( _temp285 != 0){({ void* _temp290[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp285))->hd)->loc,
_tag_arr("too many arguments", sizeof( unsigned char), 19u), _tag_arr( _temp290,
sizeof( void*), 0u));});}}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp291= p; _LL293: if( _temp291 ==( void*) Cyc_Absyn_Plus){ goto _LL294;}
else{ goto _LL295;} _LL295: if( _temp291 ==( void*) Cyc_Absyn_Minus){ goto
_LL296;} else{ goto _LL297;} _LL297: if( _temp291 ==( void*) Cyc_Absyn_Not){
goto _LL298;} else{ goto _LL299;} _LL299: if( _temp291 ==( void*) Cyc_Absyn_Bitnot){
goto _LL300;} else{ goto _LL301;} _LL301: if( _temp291 ==( void*) Cyc_Absyn_Size){
goto _LL302;} else{ goto _LL303;} _LL303: goto _LL304; _LL294: goto _LL296;
_LL296: if( ! Cyc_Tcutil_is_numeric( e)){({ struct Cyc_Stdio_String_pa_struct
_temp306; _temp306.tag= Cyc_Stdio_String_pa; _temp306.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp305[ 1u]={& _temp306}; Cyc_Tcutil_terr(
loc, _tag_arr("expecting arithmetic type but found %s", sizeof( unsigned char),
39u), _tag_arr( _temp305, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL298: Cyc_Tcutil_check_contains_assign( e); if( !
Cyc_Tcutil_coerce_to_bool( te, e)){({ struct Cyc_Stdio_String_pa_struct _temp308;
_temp308.tag= Cyc_Stdio_String_pa; _temp308.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp307[ 1u]={& _temp308}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u), _tag_arr( _temp307, sizeof( void*), 1u));}});}
return Cyc_Absyn_sint_t; _LL300: if( ! Cyc_Tcutil_is_integral( e)){({ struct Cyc_Stdio_String_pa_struct
_temp310; _temp310.tag= Cyc_Stdio_String_pa; _temp310.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp309[ 1u]={& _temp310}; Cyc_Tcutil_terr(
loc, _tag_arr("expecting integral type but found %s", sizeof( unsigned char), 37u),
_tag_arr( _temp309, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL302:{ void* _temp311= t; struct Cyc_Absyn_PtrInfo
_temp319; _LL313: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_ArrayType:
0){ goto _LL314;} else{ goto _LL315;} _LL315: if(( unsigned int) _temp311 > 4u?*((
int*) _temp311) == Cyc_Absyn_PointerType: 0){ _LL320: _temp319=(( struct Cyc_Absyn_PointerType_struct*)
_temp311)->f1; goto _LL316;} else{ goto _LL317;} _LL317: goto _LL318; _LL314:
goto _LL312; _LL316: goto _LL312; _LL318:({ struct Cyc_Stdio_String_pa_struct
_temp322; _temp322.tag= Cyc_Stdio_String_pa; _temp322.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp321[ 1u]={& _temp322}; Cyc_Tcutil_terr(
loc, _tag_arr("size requires pointer or array type, not %s", sizeof(
unsigned char), 44u), _tag_arr( _temp321, sizeof( void*), 1u));}}); goto _LL312;
_LL312:;} return Cyc_Absyn_uint_t; _LL304: return({ void* _temp323[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("Non-unary primop", sizeof( unsigned char), 17u), _tag_arr( _temp323,
sizeof( void*), 0u));}); _LL292:;} static void* Cyc_Tcexp_tcArithBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, int(*
checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){({ struct Cyc_Stdio_String_pa_struct
_temp325; _temp325.tag= Cyc_Stdio_String_pa; _temp325.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{
void* _temp324[ 1u]={& _temp325}; Cyc_Tcutil_terr( e1->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp324, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp326=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp326->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp326;}));} if( ! checker( e2)){({ struct Cyc_Stdio_String_pa_struct
_temp328; _temp328.tag= Cyc_Stdio_String_pa; _temp328.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{
void* _temp327[ 1u]={& _temp328}; Cyc_Tcutil_terr( e2->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp327, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp329=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp329->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp329;}));}{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp330= t1; struct
Cyc_Absyn_PtrInfo _temp336; struct Cyc_Absyn_Conref* _temp338; struct Cyc_Absyn_Tqual
_temp340; struct Cyc_Absyn_Conref* _temp342; void* _temp344; void* _temp346;
_LL332: if(( unsigned int) _temp330 > 4u?*(( int*) _temp330) == Cyc_Absyn_PointerType:
0){ _LL337: _temp336=(( struct Cyc_Absyn_PointerType_struct*) _temp330)->f1;
_LL347: _temp346=( void*) _temp336.elt_typ; goto _LL345; _LL345: _temp344=( void*)
_temp336.rgn_typ; goto _LL343; _LL343: _temp342= _temp336.nullable; goto _LL341;
_LL341: _temp340= _temp336.tq; goto _LL339; _LL339: _temp338= _temp336.bounds;
goto _LL333;} else{ goto _LL334;} _LL334: goto _LL335; _LL333: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){({ struct Cyc_Stdio_String_pa_struct _temp349; _temp349.tag= Cyc_Stdio_String_pa;
_temp349.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp348[
1u]={& _temp349}; Cyc_Tcutil_terr( e2->loc, _tag_arr("expecting int but found %s",
sizeof( unsigned char), 27u), _tag_arr( _temp348, sizeof( void*), 1u));}});}
_temp338= Cyc_Absyn_compress_conref( _temp338);{ void* _temp350=( void*)
_temp338->v; void* _temp358; void* _temp360; struct Cyc_Absyn_Exp* _temp362;
_LL352: if(( unsigned int) _temp350 > 1u?*(( int*) _temp350) == Cyc_Absyn_Eq_constr:
0){ _LL359: _temp358=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp350)->f1;
if( _temp358 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL353;} else{ goto _LL354;}}
else{ goto _LL354;} _LL354: if(( unsigned int) _temp350 > 1u?*(( int*) _temp350)
== Cyc_Absyn_Eq_constr: 0){ _LL361: _temp360=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp350)->f1; if(( unsigned int) _temp360 > 1u?*(( int*) _temp360) == Cyc_Absyn_Upper_b:
0){ _LL363: _temp362=(( struct Cyc_Absyn_Upper_b_struct*) _temp360)->f1; goto
_LL355;} else{ goto _LL356;}} else{ goto _LL356;} _LL356: goto _LL357; _LL353:
return t1; _LL355: { struct Cyc_Absyn_PointerType_struct* _temp364=({ struct Cyc_Absyn_PointerType_struct*
_temp365=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp365[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp366; _temp366.tag= Cyc_Absyn_PointerType;
_temp366.f1=({ struct Cyc_Absyn_PtrInfo _temp367; _temp367.elt_typ=( void*)
_temp346; _temp367.rgn_typ=( void*) _temp344; _temp367.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp367.tq= _temp340; _temp367.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp367;}); _temp366;}); _temp365;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp364); return( void*) _temp364;} _LL357:( void*)( _temp338->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp368=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp368[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp369; _temp369.tag= Cyc_Absyn_Eq_constr; _temp369.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp369;}); _temp368;}))); return t1;
_LL351:;} _LL335: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL331:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){({ struct Cyc_Stdio_String_pa_struct
_temp372; _temp372.tag= Cyc_Stdio_String_pa; _temp372.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{
struct Cyc_Stdio_String_pa_struct _temp371; _temp371.tag= Cyc_Stdio_String_pa;
_temp371.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v);{ void* _temp370[ 2u]={& _temp371,& _temp372}; Cyc_Tcutil_terr(
e1->loc, _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u), _tag_arr( _temp370, sizeof( void*), 2u));}}});}
return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){({
struct Cyc_Stdio_String_pa_struct _temp375; _temp375.tag= Cyc_Stdio_String_pa;
_temp375.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Stdio_String_pa_struct
_temp374; _temp374.tag= Cyc_Stdio_String_pa; _temp374.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp373[ 2u]={& _temp374,& _temp375}; Cyc_Tcutil_terr(
e2->loc, _tag_arr("expecting either %s or int but found %s", sizeof(
unsigned char), 40u), _tag_arr( _temp373, sizeof( void*), 2u));}}});} return t1;}}
else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp376=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp376->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp376;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp378=({ struct _tuple0 _temp377;
_temp377.f1= Cyc_Tcutil_compress( t1); _temp377.f2= Cyc_Tcutil_compress( t2);
_temp377;}); void* _temp384; struct Cyc_Absyn_PtrInfo _temp386; void* _temp388;
void* _temp390; struct Cyc_Absyn_PtrInfo _temp392; void* _temp394; _LL380:
_LL391: _temp390= _temp378.f1; if(( unsigned int) _temp390 > 4u?*(( int*)
_temp390) == Cyc_Absyn_PointerType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_PointerType_struct*)
_temp390)->f1; _LL395: _temp394=( void*) _temp392.elt_typ; goto _LL385;} else{
goto _LL382;} _LL385: _temp384= _temp378.f2; if(( unsigned int) _temp384 > 4u?*((
int*) _temp384) == Cyc_Absyn_PointerType: 0){ _LL387: _temp386=(( struct Cyc_Absyn_PointerType_struct*)
_temp384)->f1; _LL389: _temp388=( void*) _temp386.elt_typ; goto _LL381;} else{
goto _LL382;} _LL382: goto _LL383; _LL381: if( Cyc_Tcutil_unify( _temp394,
_temp388)){ return Cyc_Absyn_sint_t;} goto _LL379; _LL383: goto _LL379; _LL379:;}({
struct Cyc_Stdio_String_pa_struct _temp398; _temp398.tag= Cyc_Stdio_String_pa;
_temp398.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Stdio_String_pa_struct
_temp397; _temp397.tag= Cyc_Stdio_String_pa; _temp397.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp396[ 2u]={& _temp397,& _temp398}; Cyc_Tcutil_terr(
loc, _tag_arr("comparison not allowed between %s and %s", sizeof( unsigned char),
41u), _tag_arr( _temp396, sizeof( void*), 2u));}}}); return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp399=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp399->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp399;}));}}}}}
static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp400= p; _LL402: if( _temp400 ==( void*) Cyc_Absyn_Plus){ goto _LL403;}
else{ goto _LL404;} _LL404: if( _temp400 ==( void*) Cyc_Absyn_Minus){ goto
_LL405;} else{ goto _LL406;} _LL406: if( _temp400 ==( void*) Cyc_Absyn_Times){
goto _LL407;} else{ goto _LL408;} _LL408: if( _temp400 ==( void*) Cyc_Absyn_Div){
goto _LL409;} else{ goto _LL410;} _LL410: if( _temp400 ==( void*) Cyc_Absyn_Mod){
goto _LL411;} else{ goto _LL412;} _LL412: if( _temp400 ==( void*) Cyc_Absyn_Bitand){
goto _LL413;} else{ goto _LL414;} _LL414: if( _temp400 ==( void*) Cyc_Absyn_Bitor){
goto _LL415;} else{ goto _LL416;} _LL416: if( _temp400 ==( void*) Cyc_Absyn_Bitxor){
goto _LL417;} else{ goto _LL418;} _LL418: if( _temp400 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL419;} else{ goto _LL420;} _LL420: if( _temp400 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL421;} else{ goto _LL422;} _LL422: if( _temp400 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL423;} else{ goto _LL424;} _LL424: if( _temp400 ==( void*) Cyc_Absyn_Eq){
goto _LL425;} else{ goto _LL426;} _LL426: if( _temp400 ==( void*) Cyc_Absyn_Neq){
goto _LL427;} else{ goto _LL428;} _LL428: if( _temp400 ==( void*) Cyc_Absyn_Gt){
goto _LL429;} else{ goto _LL430;} _LL430: if( _temp400 ==( void*) Cyc_Absyn_Lt){
goto _LL431;} else{ goto _LL432;} _LL432: if( _temp400 ==( void*) Cyc_Absyn_Gte){
goto _LL433;} else{ goto _LL434;} _LL434: if( _temp400 ==( void*) Cyc_Absyn_Lte){
goto _LL435;} else{ goto _LL436;} _LL436: goto _LL437; _LL403: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL405: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL407: goto _LL409;
_LL409: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL411: goto _LL413; _LL413: goto _LL415; _LL415: goto _LL417; _LL417: goto
_LL419; _LL419: goto _LL421; _LL421: goto _LL423; _LL423: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL425: goto _LL427; _LL427: goto _LL429;
_LL429: goto _LL431; _LL431: goto _LL433; _LL433: goto _LL435; _LL435: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL437:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp438=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp438[ 0]=({ struct Cyc_Core_Failure_struct
_temp439; _temp439.tag= Cyc_Core_Failure; _temp439.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp439;}); _temp438;})); _LL401:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t; switch((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ case 0: _LL440: return({ void* _temp442[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("primitive operator has 0 arguments", sizeof( unsigned char),
35u), _tag_arr( _temp442, sizeof( void*), 0u));}); case 1: _LL441: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
es))->hd); break; case 2: _LL443: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd); break; default: _LL444: return({ void* _temp446[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u), _tag_arr( _temp446, sizeof( void*), 0u));});}
return t;}} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp447=( void*) e->r; void* _temp471; struct Cyc_Absyn_Vardecl* _temp473;
void* _temp475; struct Cyc_Absyn_Vardecl* _temp477; void* _temp479; struct Cyc_Absyn_Vardecl*
_temp481; void* _temp483; struct Cyc_Absyn_Vardecl* _temp485; struct Cyc_Absyn_Exp*
_temp487; struct Cyc_Absyn_Exp* _temp489; struct _tagged_arr* _temp491; struct
Cyc_Absyn_Exp* _temp493; struct _tagged_arr* _temp495; struct Cyc_Absyn_Exp*
_temp497; struct Cyc_Absyn_Exp* _temp499; struct Cyc_Absyn_Exp* _temp501; struct
Cyc_Absyn_Exp* _temp503; _LL449: if(*(( int*) _temp447) == Cyc_Absyn_Var_e){
_LL472: _temp471=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp471 > 1u?*(( int*) _temp471) == Cyc_Absyn_Param_b: 0){ _LL474:
_temp473=(( struct Cyc_Absyn_Param_b_struct*) _temp471)->f1; goto _LL450;} else{
goto _LL451;}} else{ goto _LL451;} _LL451: if(*(( int*) _temp447) == Cyc_Absyn_Var_e){
_LL476: _temp475=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp475 > 1u?*(( int*) _temp475) == Cyc_Absyn_Local_b: 0){ _LL478:
_temp477=(( struct Cyc_Absyn_Local_b_struct*) _temp475)->f1; goto _LL452;} else{
goto _LL453;}} else{ goto _LL453;} _LL453: if(*(( int*) _temp447) == Cyc_Absyn_Var_e){
_LL480: _temp479=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp479 > 1u?*(( int*) _temp479) == Cyc_Absyn_Pat_b: 0){ _LL482:
_temp481=(( struct Cyc_Absyn_Pat_b_struct*) _temp479)->f1; goto _LL454;} else{
goto _LL455;}} else{ goto _LL455;} _LL455: if(*(( int*) _temp447) == Cyc_Absyn_Var_e){
_LL484: _temp483=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp483 > 1u?*(( int*) _temp483) == Cyc_Absyn_Global_b: 0){
_LL486: _temp485=(( struct Cyc_Absyn_Global_b_struct*) _temp483)->f1; goto
_LL456;} else{ goto _LL457;}} else{ goto _LL457;} _LL457: if(*(( int*) _temp447)
== Cyc_Absyn_Subscript_e){ _LL490: _temp489=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp447)->f1; goto _LL488; _LL488: _temp487=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp447)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if(*(( int*) _temp447)
== Cyc_Absyn_StructMember_e){ _LL494: _temp493=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp447)->f1; goto _LL492; _LL492: _temp491=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp447)->f2; goto _LL460;} else{ goto _LL461;} _LL461: if(*(( int*) _temp447)
== Cyc_Absyn_StructArrow_e){ _LL498: _temp497=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp447)->f1; goto _LL496; _LL496: _temp495=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp447)->f2; goto _LL462;} else{ goto _LL463;} _LL463: if(*(( int*) _temp447)
== Cyc_Absyn_Deref_e){ _LL500: _temp499=(( struct Cyc_Absyn_Deref_e_struct*)
_temp447)->f1; goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp447)
== Cyc_Absyn_NoInstantiate_e){ _LL502: _temp501=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp447)->f1; goto _LL466;} else{ goto _LL467;} _LL467: if(*(( int*) _temp447)
== Cyc_Absyn_Instantiate_e){ _LL504: _temp503=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp447)->f1; goto _LL468;} else{ goto _LL469;} _LL469: goto _LL470; _LL450:
_temp477= _temp473; goto _LL452; _LL452: _temp481= _temp477; goto _LL454; _LL454:
_temp485= _temp481; goto _LL456; _LL456: if( !( _temp485->tq).q_const){ return;}
goto _LL448; _LL458:{ void* _temp505= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp489->topt))->v); struct Cyc_Absyn_PtrInfo _temp515; struct Cyc_Absyn_Tqual
_temp517; struct Cyc_Absyn_Tqual _temp519; struct Cyc_List_List* _temp521;
_LL507: if(( unsigned int) _temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_PointerType:
0){ _LL516: _temp515=(( struct Cyc_Absyn_PointerType_struct*) _temp505)->f1;
_LL518: _temp517= _temp515.tq; goto _LL508;} else{ goto _LL509;} _LL509: if((
unsigned int) _temp505 > 4u?*(( int*) _temp505) == Cyc_Absyn_ArrayType: 0){
_LL520: _temp519=(( struct Cyc_Absyn_ArrayType_struct*) _temp505)->f2; goto
_LL510;} else{ goto _LL511;} _LL511: if(( unsigned int) _temp505 > 4u?*(( int*)
_temp505) == Cyc_Absyn_TupleType: 0){ _LL522: _temp521=(( struct Cyc_Absyn_TupleType_struct*)
_temp505)->f1; goto _LL512;} else{ goto _LL513;} _LL513: goto _LL514; _LL508:
_temp519= _temp517; goto _LL510; _LL510: if( ! _temp519.q_const){ return;} goto
_LL506; _LL512: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp487);{
struct _handler_cons _temp523; _push_handler(& _temp523);{ int _temp525= 0; if(
setjmp( _temp523.handler)){ _temp525= 1;} if( ! _temp525){{ struct _tuple8
_temp528; struct Cyc_Absyn_Tqual _temp529; struct _tuple8* _temp526=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( _temp521,( int) i);
_temp528=* _temp526; _LL530: _temp529= _temp528.f1; goto _LL527; _LL527: if( !
_temp529.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp524=( void*) _exn_thrown; void* _temp532= _temp524; _LL534: if( _temp532 ==
Cyc_List_Nth){ goto _LL535;} else{ goto _LL536;} _LL536: goto _LL537; _LL535:
return; _LL537:( void) _throw( _temp532); _LL533:;}}} goto _LL506;} _LL514: goto
_LL506; _LL506:;} goto _LL448; _LL460:{ void* _temp538= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp493->topt))->v); struct Cyc_Absyn_Structdecl**
_temp550; struct Cyc_List_List* _temp552; struct Cyc_List_List* _temp554; struct
Cyc_Absyn_Uniondecl** _temp556; _LL540: if(( unsigned int) _temp538 > 4u?*(( int*)
_temp538) == Cyc_Absyn_StructType: 0){ _LL551: _temp550=(( struct Cyc_Absyn_StructType_struct*)
_temp538)->f3; goto _LL541;} else{ goto _LL542;} _LL542: if(( unsigned int)
_temp538 > 4u?*(( int*) _temp538) == Cyc_Absyn_AnonUnionType: 0){ _LL553:
_temp552=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp538)->f1; goto _LL543;}
else{ goto _LL544;} _LL544: if(( unsigned int) _temp538 > 4u?*(( int*) _temp538)
== Cyc_Absyn_AnonStructType: 0){ _LL555: _temp554=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp538)->f1; goto _LL545;} else{ goto _LL546;} _LL546: if(( unsigned int)
_temp538 > 4u?*(( int*) _temp538) == Cyc_Absyn_UnionType: 0){ _LL557: _temp556=((
struct Cyc_Absyn_UnionType_struct*) _temp538)->f3; goto _LL547;} else{ goto
_LL548;} _LL548: goto _LL549; _LL541: { struct Cyc_Absyn_Structfield* sf=
_temp550 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp550)), _temp491); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL539;} _LL543: _temp554= _temp552; goto _LL545; _LL545: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp554, _temp491); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL539;} _LL547: { struct Cyc_Absyn_Structfield*
sf= _temp556 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp556)), _temp491); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL539;} _LL549: goto _LL539; _LL539:;} goto _LL448; _LL462:{ void*
_temp558= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp497->topt))->v); struct Cyc_Absyn_PtrInfo _temp564; void* _temp566; _LL560:
if(( unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_PointerType: 0){
_LL565: _temp564=(( struct Cyc_Absyn_PointerType_struct*) _temp558)->f1; _LL567:
_temp566=( void*) _temp564.elt_typ; goto _LL561;} else{ goto _LL562;} _LL562:
goto _LL563; _LL561:{ void* _temp568= Cyc_Tcutil_compress( _temp566); struct Cyc_Absyn_Structdecl**
_temp580; struct Cyc_Absyn_Uniondecl** _temp582; struct Cyc_List_List* _temp584;
struct Cyc_List_List* _temp586; _LL570: if(( unsigned int) _temp568 > 4u?*(( int*)
_temp568) == Cyc_Absyn_StructType: 0){ _LL581: _temp580=(( struct Cyc_Absyn_StructType_struct*)
_temp568)->f3; goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int)
_temp568 > 4u?*(( int*) _temp568) == Cyc_Absyn_UnionType: 0){ _LL583: _temp582=((
struct Cyc_Absyn_UnionType_struct*) _temp568)->f3; goto _LL573;} else{ goto
_LL574;} _LL574: if(( unsigned int) _temp568 > 4u?*(( int*) _temp568) == Cyc_Absyn_AnonStructType:
0){ _LL585: _temp584=(( struct Cyc_Absyn_AnonStructType_struct*) _temp568)->f1;
goto _LL575;} else{ goto _LL576;} _LL576: if(( unsigned int) _temp568 > 4u?*((
int*) _temp568) == Cyc_Absyn_AnonUnionType: 0){ _LL587: _temp586=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp568)->f1; goto _LL577;} else{ goto _LL578;} _LL578: goto _LL579; _LL571: {
struct Cyc_Absyn_Structfield* sf= _temp580 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp580)), _temp495); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL569;} _LL573: { struct Cyc_Absyn_Structfield*
sf= _temp582 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp582)), _temp495); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL569;} _LL575: _temp586= _temp584; goto _LL577; _LL577: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp586, _temp495); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL569;} _LL579: goto _LL569;
_LL569:;} goto _LL559; _LL563: goto _LL559; _LL559:;} goto _LL448; _LL464:{ void*
_temp588= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp499->topt))->v); struct Cyc_Absyn_PtrInfo _temp596; struct Cyc_Absyn_Tqual
_temp598; struct Cyc_Absyn_Tqual _temp600; _LL590: if(( unsigned int) _temp588 >
4u?*(( int*) _temp588) == Cyc_Absyn_PointerType: 0){ _LL597: _temp596=(( struct
Cyc_Absyn_PointerType_struct*) _temp588)->f1; _LL599: _temp598= _temp596.tq;
goto _LL591;} else{ goto _LL592;} _LL592: if(( unsigned int) _temp588 > 4u?*((
int*) _temp588) == Cyc_Absyn_ArrayType: 0){ _LL601: _temp600=(( struct Cyc_Absyn_ArrayType_struct*)
_temp588)->f2; goto _LL593;} else{ goto _LL594;} _LL594: goto _LL595; _LL591:
_temp600= _temp598; goto _LL593; _LL593: if( ! _temp600.q_const){ return;} goto
_LL589; _LL595: goto _LL589; _LL589:;} goto _LL448; _LL466: _temp503= _temp501;
goto _LL468; _LL468: Cyc_Tcexp_check_writable( te, _temp503); return; _LL470:
goto _LL448; _LL448:;}({ void* _temp602[ 0u]={}; Cyc_Tcutil_terr( e->loc,
_tag_arr("attempt to write a read-only location", sizeof( unsigned char), 38u),
_tag_arr( _temp602, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return({ void* _temp603[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("increment/decrement of non-lvalue",
sizeof( unsigned char), 34u), _tag_arr( _temp603, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e);{ void* t=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if(
! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){({
struct Cyc_Stdio_String_pa_struct _temp605; _temp605.tag= Cyc_Stdio_String_pa;
_temp605.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp604[ 1u]={&
_temp605}; Cyc_Tcutil_terr( e->loc, _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u), _tag_arr( _temp604, sizeof( void*), 1u));}});}
return t;}} static void* Cyc_Tcexp_tcConditional( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest( te, e1, _tag_arr("conditional expression",
sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp( te, topt, e2); Cyc_Tcexp_tcExp(
te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({
struct Cyc_Core_Opt* _temp613=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp613->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp613;}));
struct Cyc_List_List* _temp606=({ struct Cyc_List_List* _temp612=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp612->hd=( void*) e3; _temp612->tl=
0; _temp612;}); struct Cyc_List_List* _temp607=({ struct Cyc_List_List* _temp611=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp611->hd=(
void*) e2; _temp611->tl= _temp606; _temp611;}); if( ! Cyc_Tcutil_coerce_list( te,
t, _temp607)){({ struct Cyc_Stdio_String_pa_struct _temp610; _temp610.tag= Cyc_Stdio_String_pa;
_temp610.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ struct Cyc_Stdio_String_pa_struct _temp609;
_temp609.tag= Cyc_Stdio_String_pa; _temp609.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp608[ 2u]={&
_temp609,& _temp610}; Cyc_Tcutil_terr( loc, _tag_arr("conditional clause types do not match: %s != %s",
sizeof( unsigned char), 48u), _tag_arr( _temp608, sizeof( void*), 2u));}}});}
return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp(
te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)),
e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp614=
Cyc_Tcutil_compress( t1); _LL616: if(( unsigned int) _temp614 > 4u?*(( int*)
_temp614) == Cyc_Absyn_ArrayType: 0){ goto _LL617;} else{ goto _LL618;} _LL618:
goto _LL619; _LL617:({ void* _temp620[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("cannot assign to an array",
sizeof( unsigned char), 26u), _tag_arr( _temp620, sizeof( void*), 0u));}); goto
_LL615; _LL619: goto _LL615; _LL615:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return({
void* _temp621[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue",
sizeof( unsigned char), 25u), _tag_arr( _temp621, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return({
struct Cyc_Stdio_String_pa_struct _temp624; _temp624.tag= Cyc_Stdio_String_pa;
_temp624.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Stdio_String_pa_struct
_temp623; _temp623.tag= Cyc_Stdio_String_pa; _temp623.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp622[ 2u]={& _temp623,& _temp624}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type mismatch: %s != %s", sizeof( unsigned char), 24u),
_tag_arr( _temp622, sizeof( void*), 2u));}}});}} else{ void* _temp625=( void*)((
struct Cyc_Core_Opt*) _check_null( po))->v; void* _temp626= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp625, e1, e2); if( !( Cyc_Tcutil_unify( _temp626, t1)? 1: Cyc_Tcutil_coerceable(
_temp626))){ return({ void* _temp627[ 0u]={}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("Cannot use this operator in an assignment", sizeof( unsigned char), 42u),
_tag_arr( _temp627, sizeof( void*), 0u));});} return _temp626;} return t2;}}
static void* Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;} static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* tu, struct
_RegionHandle* r, struct Cyc_List_List* inst, struct Cyc_List_List* fs){ struct
Cyc_List_List* fields; void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;{
void* _temp628= Cyc_Tcutil_compress( t1); void* _temp638; void* _temp640; _LL630:
if( _temp628 ==( void*) Cyc_Absyn_FloatType){ goto _LL631;} else{ goto _LL632;}
_LL632: if(( unsigned int) _temp628 > 4u?*(( int*) _temp628) == Cyc_Absyn_IntType:
0){ _LL639: _temp638=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp628)->f2;
if( _temp638 ==( void*) Cyc_Absyn_B1){ goto _LL633;} else{ goto _LL634;}} else{
goto _LL634;} _LL634: if(( unsigned int) _temp628 > 4u?*(( int*) _temp628) ==
Cyc_Absyn_IntType: 0){ _LL641: _temp640=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp628)->f2; if( _temp640 ==( void*) Cyc_Absyn_B2){ goto _LL635;} else{ goto
_LL636;}} else{ goto _LL636;} _LL636: goto _LL637; _LL631: Cyc_Tcutil_unchecked_cast(
te, e,( void*) Cyc_Absyn_DoubleType); t1=( void*) Cyc_Absyn_DoubleType; goto
_LL629; _LL633: goto _LL635; _LL635: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_sint_t);
t1= Cyc_Absyn_sint_t; goto _LL629; _LL637: goto _LL629; _LL629:;} for( fields=
fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
void* _temp644; struct Cyc_Position_Segment* _temp646; struct Cyc_List_List*
_temp648; struct Cyc_List_List* _temp650; struct _tuple1* _temp652; struct Cyc_Absyn_Tunionfield
_temp642=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL653: _temp652= _temp642.name; goto _LL651; _LL651: _temp650=
_temp642.tvs; goto _LL649; _LL649: _temp648= _temp642.typs; goto _LL647; _LL647:
_temp646= _temp642.loc; goto _LL645; _LL645: _temp644=( void*) _temp642.sc; goto
_LL643; _LL643: if(( _temp648 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp648))->tl != 0)? 1: _temp650 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp648))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp656; struct Cyc_Position_Segment* _temp658; struct Cyc_List_List*
_temp660; struct Cyc_List_List* _temp662; struct _tuple1* _temp664; struct Cyc_Absyn_Tunionfield
_temp654=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL665: _temp664= _temp654.name; goto _LL663; _LL663: _temp662=
_temp654.tvs; goto _LL661; _LL661: _temp660= _temp654.typs; goto _LL659; _LL659:
_temp658= _temp654.loc; goto _LL657; _LL657: _temp656=( void*) _temp654.sc; goto
_LL655; _LL655: if(( _temp660 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp660))->tl != 0)? 1: _temp662 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp660))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}({
struct Cyc_Stdio_String_pa_struct _temp668; _temp668.tag= Cyc_Stdio_String_pa;
_temp668.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ struct Cyc_Stdio_String_pa_struct
_temp667; _temp667.tag= Cyc_Stdio_String_pa; _temp667.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( tu);{ void* _temp666[ 2u]={& _temp667,& _temp668}; Cyc_Tcutil_terr(
e->loc, _tag_arr("can't find a field in %s to inject a value of type %s",
sizeof( unsigned char), 54u), _tag_arr( _temp666, sizeof( void*), 2u));}}});
return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
args, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List*
_temp669= args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp670= t; struct Cyc_Absyn_PtrInfo _temp676; struct Cyc_Absyn_Conref*
_temp678; struct Cyc_Absyn_Tqual _temp680; struct Cyc_Absyn_Conref* _temp682;
void* _temp684; void* _temp686; _LL672: if(( unsigned int) _temp670 > 4u?*(( int*)
_temp670) == Cyc_Absyn_PointerType: 0){ _LL677: _temp676=(( struct Cyc_Absyn_PointerType_struct*)
_temp670)->f1; _LL687: _temp686=( void*) _temp676.elt_typ; goto _LL685; _LL685:
_temp684=( void*) _temp676.rgn_typ; goto _LL683; _LL683: _temp682= _temp676.nullable;
goto _LL681; _LL681: _temp680= _temp676.tq; goto _LL679; _LL679: _temp678=
_temp676.bounds; goto _LL673;} else{ goto _LL674;} _LL674: goto _LL675; _LL673:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp684); Cyc_Tcutil_check_nonzero_bound(
loc, _temp678);{ void* _temp688= Cyc_Tcutil_compress( _temp686); struct Cyc_Absyn_FnInfo
_temp694; struct Cyc_List_List* _temp696; struct Cyc_List_List* _temp698; struct
Cyc_Absyn_VarargInfo* _temp700; int _temp702; struct Cyc_List_List* _temp704;
void* _temp706; struct Cyc_Core_Opt* _temp708; struct Cyc_List_List* _temp710;
_LL690: if(( unsigned int) _temp688 > 4u?*(( int*) _temp688) == Cyc_Absyn_FnType:
0){ _LL695: _temp694=(( struct Cyc_Absyn_FnType_struct*) _temp688)->f1; _LL711:
_temp710= _temp694.tvars; goto _LL709; _LL709: _temp708= _temp694.effect; goto
_LL707; _LL707: _temp706=( void*) _temp694.ret_typ; goto _LL705; _LL705:
_temp704= _temp694.args; goto _LL703; _LL703: _temp702= _temp694.c_varargs; goto
_LL701; _LL701: _temp700= _temp694.cyc_varargs; goto _LL699; _LL699: _temp698=
_temp694.rgn_po; goto _LL697; _LL697: _temp696= _temp694.attributes; goto _LL691;}
else{ goto _LL692;} _LL692: goto _LL693; _LL691: if( topt != 0){ Cyc_Tcutil_unify(
_temp706,*(( void**) _check_null( topt)));} while( _temp669 != 0? _temp704 != 0:
0) { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp669))->hd; void* t2=(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp704))->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( !
Cyc_Tcutil_coerce_arg( te, e1, t2)){({ struct Cyc_Stdio_String_pa_struct
_temp714; _temp714.tag= Cyc_Stdio_String_pa; _temp714.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Stdio_String_pa_struct _temp713;
_temp713.tag= Cyc_Stdio_String_pa; _temp713.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp712[ 2u]={&
_temp713,& _temp714}; Cyc_Tcutil_terr( loc, _tag_arr("actual argument has type %s but formal has type %s",
sizeof( unsigned char), 51u), _tag_arr( _temp712, sizeof( void*), 2u));}}});}
_temp669=(( struct Cyc_List_List*) _check_null( _temp669))->tl; _temp704=((
struct Cyc_List_List*) _check_null( _temp704))->tl;} if( _temp704 != 0){({ void*
_temp715[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u), _tag_arr( _temp715, sizeof( void*), 0u));});}
else{ if(( _temp669 != 0? 1: _temp702)? 1: _temp700 != 0){ if( _temp702){ for( 0;
_temp669 != 0; _temp669=(( struct Cyc_List_List*) _check_null( _temp669))->tl){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp669))->hd);}} else{ if( _temp700 == 0){({ void* _temp716[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("too many arguments for function", sizeof(
unsigned char), 32u), _tag_arr( _temp716, sizeof( void*), 0u));});} else{ int
_temp719; void* _temp721; void* _temp723; struct Cyc_Absyn_VarargInfo _temp717=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp700)); _LL724: _temp723=( void*)
_temp717.type; goto _LL722; _LL722: _temp721=( void*) _temp717.rgn; goto _LL720;
_LL720: _temp719= _temp717.inject; goto _LL718; _LL718: { struct Cyc_Absyn_VarargCallInfo*
_temp725=({ struct Cyc_Absyn_VarargCallInfo* _temp757=( struct Cyc_Absyn_VarargCallInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo)); _temp757->num_varargs= 0;
_temp757->injectors= 0; _temp757->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp700); _temp757;});* vararg_call_info= _temp725; if( ! Cyc_Tcutil_unify(
Cyc_Tcenv_curr_rgn( te), _temp721)){({ struct Cyc_Stdio_String_pa_struct
_temp728; _temp728.tag= Cyc_Stdio_String_pa; _temp728.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te));{ struct Cyc_Stdio_String_pa_struct
_temp727; _temp727.tag= Cyc_Stdio_String_pa; _temp727.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp721);{ void* _temp726[ 2u]={& _temp727,& _temp728};
Cyc_Tcutil_terr( loc, _tag_arr("I have no idea how this could fail, but it did because the region for the varargs (%s) doesn't unify with the expected new_block region (%s).",
sizeof( unsigned char), 142u), _tag_arr( _temp726, sizeof( void*), 2u));}}});}
if( ! _temp719){ for( 0; _temp669 != 0; _temp669=(( struct Cyc_List_List*)
_check_null( _temp669))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp669))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp725))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp723,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp723)){({ struct Cyc_Stdio_String_pa_struct
_temp731; _temp731.tag= Cyc_Stdio_String_pa; _temp731.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{
struct Cyc_Stdio_String_pa_struct _temp730; _temp730.tag= Cyc_Stdio_String_pa;
_temp730.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp723);{ void*
_temp729[ 2u]={& _temp730,& _temp731}; Cyc_Tcutil_terr( loc, _tag_arr("vararg requires type %s but argument has type %s",
sizeof( unsigned char), 49u), _tag_arr( _temp729, sizeof( void*), 2u));}}});}}}
else{ void* _temp732= Cyc_Tcutil_compress( _temp723); struct Cyc_Absyn_TunionInfo
_temp738; void* _temp740; struct Cyc_List_List* _temp742; void* _temp744; struct
Cyc_Absyn_Tuniondecl* _temp746; _LL734: if(( unsigned int) _temp732 > 4u?*(( int*)
_temp732) == Cyc_Absyn_TunionType: 0){ _LL739: _temp738=(( struct Cyc_Absyn_TunionType_struct*)
_temp732)->f1; _LL745: _temp744=( void*) _temp738.tunion_info; if(*(( int*)
_temp744) == Cyc_Absyn_KnownTunion){ _LL747: _temp746=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp744)->f1; goto _LL743;} else{ goto _LL736;} _LL743: _temp742= _temp738.targs;
goto _LL741; _LL741: _temp740=( void*) _temp738.rgn; goto _LL735;} else{ goto
_LL736;} _LL736: goto _LL737; _LL735: { struct Cyc_Absyn_Tuniondecl* _temp748=*
Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp746->name); if( ! Cyc_Tcutil_unify(
_temp740, _temp721)){({ void* _temp749[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad region for injected varargs",
sizeof( unsigned char), 32u), _tag_arr( _temp749, sizeof( void*), 0u));});}{
struct Cyc_List_List* fields= 0; if( _temp748->fields == 0){({ struct Cyc_Stdio_String_pa_struct
_temp751; _temp751.tag= Cyc_Stdio_String_pa; _temp751.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp723);{ void* _temp750[ 1u]={& _temp751}; Cyc_Tcutil_terr(
loc, _tag_arr("can't inject into %s", sizeof( unsigned char), 21u), _tag_arr(
_temp750, sizeof( void*), 1u));}});} else{ fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp748->fields))->v;}{ struct _RegionHandle
_temp752= _new_region(); struct _RegionHandle* rgn=& _temp752; _push_region( rgn);{
struct Cyc_List_List* _temp753=(( struct Cyc_List_List*(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y))
Cyc_List_rzip)( rgn, rgn, _temp748->tvs, _temp742); for( 0; _temp669 != 0;
_temp669=(( struct Cyc_List_List*) _check_null( _temp669))->tl){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp725))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp669))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp754= Cyc_Tcexp_tcInjection( te, e1, _temp723,
rgn, _temp753, fields); if( _temp754 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp725))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp725))->injectors,({ struct Cyc_List_List* _temp755=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp755->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp754)); _temp755->tl= 0;
_temp755;}));}}}}}; _pop_region( rgn);} goto _LL733;}} _LL737:({ void* _temp756[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad inject vararg type", sizeof(
unsigned char), 23u), _tag_arr( _temp756, sizeof( void*), 0u));}); goto _LL733;
_LL733:;}}}}}}{ struct Cyc_List_List* a= _temp696; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ void* _temp758=( void*)(( struct Cyc_List_List*)
_check_null( a))->hd; int _temp764; int _temp766; void* _temp768; _LL760: if((
unsigned int) _temp758 > 16u?*(( int*) _temp758) == Cyc_Absyn_Format_att: 0){
_LL769: _temp768=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp758)->f1;
goto _LL767; _LL767: _temp766=(( struct Cyc_Absyn_Format_att_struct*) _temp758)->f2;
goto _LL765; _LL765: _temp764=(( struct Cyc_Absyn_Format_att_struct*) _temp758)->f3;
goto _LL761;} else{ goto _LL762;} _LL762: goto _LL763; _LL761:{ struct
_handler_cons _temp770; _push_handler(& _temp770);{ int _temp772= 0; if( setjmp(
_temp770.handler)){ _temp772= 1;} if( ! _temp772){{ struct Cyc_Absyn_Exp*
_temp773=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
args, _temp766 - 1); struct Cyc_Core_Opt* fmt_args; if( _temp764 == 0){ fmt_args=
0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp774=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp774->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp764 - 1);
_temp774;});}{ void* _temp775= _temp768; _LL777: if( _temp775 ==( void*) Cyc_Absyn_Printf_ft){
goto _LL778;} else{ goto _LL779;} _LL779: if( _temp775 ==( void*) Cyc_Absyn_Scanf_ft){
goto _LL780;} else{ goto _LL776;} _LL778: Cyc_Tcexp_check_format_args( te,
_temp773, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL776; _LL780: Cyc_Tcexp_check_format_args(
te, _temp773, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL776; _LL776:;}};
_pop_handler();} else{ void* _temp771=( void*) _exn_thrown; void* _temp782=
_temp771; _LL784: if( _temp782 == Cyc_List_Nth){ goto _LL785;} else{ goto _LL786;}
_LL786: goto _LL787; _LL785:({ void* _temp788[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bad format arguments", sizeof( unsigned char), 21u), _tag_arr(
_temp788, sizeof( void*), 0u));}); goto _LL783; _LL787:( void) _throw( _temp782);
_LL783:;}}} goto _LL759; _LL763: goto _LL759; _LL759:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp708))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp698); return _temp706; _LL693: return({ void* _temp789[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function", sizeof(
unsigned char), 29u), _tag_arr( _temp789, sizeof( void*), 0u));}); _LL689:;}
_LL675: return({ void* _temp790[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u), _tag_arr( _temp790, sizeof( void*), 0u));});
_LL671:;}} static void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){({ struct Cyc_Stdio_String_pa_struct
_temp792; _temp792.tag= Cyc_Stdio_String_pa; _temp792.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{
void* _temp791[ 1u]={& _temp792}; Cyc_Tcutil_terr( loc, _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp791, sizeof( void*), 1u));}});}
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp793=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp793->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp793;}));} static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp794= t1;
struct Cyc_Absyn_PtrInfo _temp800; struct Cyc_Absyn_Conref* _temp802; struct Cyc_Absyn_Tqual
_temp804; struct Cyc_Absyn_Conref* _temp806; void* _temp808; void* _temp810;
_LL796: if(( unsigned int) _temp794 > 4u?*(( int*) _temp794) == Cyc_Absyn_PointerType:
0){ _LL801: _temp800=(( struct Cyc_Absyn_PointerType_struct*) _temp794)->f1;
_LL811: _temp810=( void*) _temp800.elt_typ; goto _LL809; _LL809: _temp808=( void*)
_temp800.rgn_typ; goto _LL807; _LL807: _temp806= _temp800.nullable; goto _LL805;
_LL805: _temp804= _temp800.tq; goto _LL803; _LL803: _temp802= _temp800.bounds;
goto _LL797;} else{ goto _LL798;} _LL798: goto _LL799; _LL797:{ void* _temp812=
Cyc_Tcutil_compress( _temp810); struct Cyc_Absyn_FnInfo _temp818; struct Cyc_List_List*
_temp820; struct Cyc_List_List* _temp822; struct Cyc_Absyn_VarargInfo* _temp824;
int _temp826; struct Cyc_List_List* _temp828; void* _temp830; struct Cyc_Core_Opt*
_temp832; struct Cyc_List_List* _temp834; _LL814: if(( unsigned int) _temp812 >
4u?*(( int*) _temp812) == Cyc_Absyn_FnType: 0){ _LL819: _temp818=(( struct Cyc_Absyn_FnType_struct*)
_temp812)->f1; _LL835: _temp834= _temp818.tvars; goto _LL833; _LL833: _temp832=
_temp818.effect; goto _LL831; _LL831: _temp830=( void*) _temp818.ret_typ; goto
_LL829; _LL829: _temp828= _temp818.args; goto _LL827; _LL827: _temp826= _temp818.c_varargs;
goto _LL825; _LL825: _temp824= _temp818.cyc_varargs; goto _LL823; _LL823:
_temp822= _temp818.rgn_po; goto _LL821; _LL821: _temp820= _temp818.attributes;
goto _LL815;} else{ goto _LL816;} _LL816: goto _LL817; _LL815: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) !=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp834)){ return({ void* _temp836[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u),
_tag_arr( _temp836, sizeof( void*), 0u));});} for( 0; ts != 0;( ts=(( struct Cyc_List_List*)
_check_null( ts))->tl, _temp834=(( struct Cyc_List_List*) _check_null( _temp834))->tl)){
void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp834))->hd)->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); instantiation=({
struct Cyc_List_List* _temp837=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp837->hd=( void*)({ struct _tuple5* _temp838=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp838->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp834))->hd; _temp838->f2=( void*)((
struct Cyc_List_List*) _check_null( ts))->hd; _temp838;}); _temp837->tl=
instantiation; _temp837;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp842=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp842[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp843; _temp843.tag= Cyc_Absyn_FnType; _temp843.f1=({ struct Cyc_Absyn_FnInfo
_temp844; _temp844.tvars= 0; _temp844.effect= _temp832; _temp844.ret_typ=( void*)
_temp830; _temp844.args= _temp828; _temp844.c_varargs= _temp826; _temp844.cyc_varargs=
_temp824; _temp844.rgn_po= _temp822; _temp844.attributes= _temp820; _temp844;});
_temp843;}); _temp842;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp839=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp839[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp840; _temp840.tag= Cyc_Absyn_PointerType;
_temp840.f1=({ struct Cyc_Absyn_PtrInfo _temp841; _temp841.elt_typ=( void*)
new_fn_typ; _temp841.rgn_typ=( void*) _temp808; _temp841.nullable= _temp806;
_temp841.tq= _temp804; _temp841.bounds= _temp802; _temp841;}); _temp840;});
_temp839;}); return new_typ;}} _LL817: goto _LL813; _LL813:;} goto _LL795;
_LL799: goto _LL795; _LL795:;} return({ struct Cyc_Stdio_String_pa_struct
_temp846; _temp846.tag= Cyc_Stdio_String_pa; _temp846.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp845[ 1u]={& _temp846}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting polymorphic type but found %s", sizeof(
unsigned char), 40u), _tag_arr( _temp845, sizeof( void*), 1u));}});}} static
void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ return({ struct Cyc_Stdio_String_pa_struct _temp849; _temp849.tag=
Cyc_Stdio_String_pa; _temp849.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Stdio_String_pa_struct _temp848; _temp848.tag= Cyc_Stdio_String_pa;
_temp848.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp847[
2u]={& _temp848,& _temp849}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u), _tag_arr( _temp847, sizeof( void*), 2u));}}});}
return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ void**
_temp850= 0; struct Cyc_Absyn_Tqual _temp851= Cyc_Absyn_empty_tqual(); if( topt
!= 0){ void* _temp852= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp858; struct Cyc_Absyn_Tqual _temp860; void*
_temp862; _LL854: if(( unsigned int) _temp852 > 4u?*(( int*) _temp852) == Cyc_Absyn_PointerType:
0){ _LL859: _temp858=(( struct Cyc_Absyn_PointerType_struct*) _temp852)->f1;
_LL863: _temp862=( void*) _temp858.elt_typ; goto _LL861; _LL861: _temp860=
_temp858.tq; goto _LL855;} else{ goto _LL856;} _LL856: goto _LL857; _LL855:
_temp850=({ void** _temp864=( void**) GC_malloc( sizeof( void*)); _temp864[ 0]=
_temp862; _temp864;}); _temp851= _temp860; goto _LL853; _LL857: goto _LL853;
_LL853:;} Cyc_Tcexp_tcExpNoInst( te, _temp850, e);{ void* _temp865=( void*) e->r;
_LL867: if(*(( int*) _temp865) == Cyc_Absyn_Struct_e){ goto _LL868;} else{ goto
_LL869;} _LL869: if(*(( int*) _temp865) == Cyc_Absyn_Tuple_e){ goto _LL870;}
else{ goto _LL871;} _LL871: goto _LL872; _LL868: goto _LL870; _LL870:({ void*
_temp873[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate", sizeof(
unsigned char), 19u), _tag_arr( _temp873, sizeof( void*), 0u));});{ void*(*
_temp874)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if(
topt != 0){ void* _temp875= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp881; struct Cyc_Absyn_Conref* _temp883; _LL877:
if(( unsigned int) _temp875 > 4u?*(( int*) _temp875) == Cyc_Absyn_PointerType: 0){
_LL882: _temp881=(( struct Cyc_Absyn_PointerType_struct*) _temp875)->f1; _LL884:
_temp883= _temp881.nullable; goto _LL878;} else{ goto _LL879;} _LL879: goto
_LL880; _LL878: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp883)){ _temp874= Cyc_Absyn_star_typ;} goto _LL876; _LL880: goto _LL876;
_LL876:;} return _temp874(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp851);} _LL872: goto _LL866; _LL866:;}{ void*
_temp887; int _temp889; struct _tuple6 _temp885= Cyc_Tcutil_addressof_props( te,
e); _LL890: _temp889= _temp885.f1; goto _LL888; _LL888: _temp887= _temp885.f2;
goto _LL886; _LL886: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp889){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp887, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); if( ! Cyc_Evexp_okay_szofarg( t)){({ struct Cyc_Stdio_String_pa_struct
_temp892; _temp892.tag= Cyc_Stdio_String_pa; _temp892.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp891[ 1u]={& _temp892}; Cyc_Tcutil_terr(
loc, _tag_arr("sizeof applied to type %s, which has unknown size here", sizeof(
unsigned char), 55u), _tag_arr( _temp891, sizeof( void*), 1u));}});} return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct _tagged_arr* n){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ int
bad_type= 1;{ void* _temp893= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp901; _LL895: if(( unsigned int) _temp893 > 4u?*(( int*) _temp893) == Cyc_Absyn_StructType:
0){ _LL902: _temp901=(( struct Cyc_Absyn_StructType_struct*) _temp893)->f3; goto
_LL896;} else{ goto _LL897;} _LL897: if(( unsigned int) _temp893 > 4u?*(( int*)
_temp893) == Cyc_Absyn_AnonStructType: 0){ goto _LL898;} else{ goto _LL899;}
_LL899: goto _LL900; _LL896: if( _temp901 == 0){ return({ void* _temp903[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType", sizeof(
unsigned char), 33u), _tag_arr( _temp903, sizeof( void*), 0u));});} if((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp901)))->fields == 0){ goto
_LL894;} goto _LL898; _LL898: bad_type= 0; goto _LL894; _LL900: goto _LL894;
_LL894:;} if( bad_type){({ struct Cyc_Stdio_String_pa_struct _temp905; _temp905.tag=
Cyc_Stdio_String_pa; _temp905.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp904[ 1u]={& _temp905}; Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct type",
sizeof( unsigned char), 30u), _tag_arr( _temp904, sizeof( void*), 1u));}});}
return Cyc_Absyn_uint_t;}} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp906= t; struct Cyc_Absyn_PtrInfo _temp912;
struct Cyc_Absyn_Conref* _temp914; void* _temp916; void* _temp918; _LL908: if((
unsigned int) _temp906 > 4u?*(( int*) _temp906) == Cyc_Absyn_PointerType: 0){
_LL913: _temp912=(( struct Cyc_Absyn_PointerType_struct*) _temp906)->f1; _LL919:
_temp918=( void*) _temp912.elt_typ; goto _LL917; _LL917: _temp916=( void*)
_temp912.rgn_typ; goto _LL915; _LL915: _temp914= _temp912.bounds; goto _LL909;}
else{ goto _LL910;} _LL910: goto _LL911; _LL909: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp916); Cyc_Tcutil_check_nonzero_bound( loc, _temp914); return
_temp918; _LL911: return({ struct Cyc_Stdio_String_pa_struct _temp921; _temp921.tag=
Cyc_Stdio_String_pa; _temp921.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp920[ 1u]={& _temp921}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting * or @ type but found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp920, sizeof( void*), 1u));}});
_LL907:;}} static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* outer_e,
struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0,
e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); void* _temp922= t; struct Cyc_Absyn_Structdecl** _temp938; struct
Cyc_Absyn_Structdecl* _temp940; struct Cyc_List_List* _temp941; struct _tuple1*
_temp943; struct Cyc_List_List* _temp945; struct Cyc_List_List* _temp947; struct
Cyc_Absyn_Uniondecl** _temp949; struct Cyc_Absyn_Uniondecl* _temp951; struct Cyc_List_List*
_temp952; struct _tuple1* _temp954; _LL924: if(( unsigned int) _temp922 > 4u?*((
int*) _temp922) == Cyc_Absyn_StructType: 0){ _LL944: _temp943=(( struct Cyc_Absyn_StructType_struct*)
_temp922)->f1; goto _LL942; _LL942: _temp941=(( struct Cyc_Absyn_StructType_struct*)
_temp922)->f2; goto _LL939; _LL939: _temp938=(( struct Cyc_Absyn_StructType_struct*)
_temp922)->f3; if( _temp938 == 0){ goto _LL926;} else{ _temp940=* _temp938; goto
_LL925;}} else{ goto _LL926;} _LL926: if(( unsigned int) _temp922 > 4u?*(( int*)
_temp922) == Cyc_Absyn_AnonStructType: 0){ _LL946: _temp945=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp922)->f1; goto _LL927;} else{ goto _LL928;} _LL928: if(( unsigned int)
_temp922 > 4u?*(( int*) _temp922) == Cyc_Absyn_AnonUnionType: 0){ _LL948:
_temp947=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp922)->f1; goto _LL929;}
else{ goto _LL930;} _LL930: if(( unsigned int) _temp922 > 4u?*(( int*) _temp922)
== Cyc_Absyn_UnionType: 0){ _LL955: _temp954=(( struct Cyc_Absyn_UnionType_struct*)
_temp922)->f1; goto _LL953; _LL953: _temp952=(( struct Cyc_Absyn_UnionType_struct*)
_temp922)->f2; goto _LL950; _LL950: _temp949=(( struct Cyc_Absyn_UnionType_struct*)
_temp922)->f3; if( _temp949 == 0){ goto _LL932;} else{ _temp951=* _temp949; goto
_LL931;}} else{ goto _LL932;} _LL932: if(( unsigned int) _temp922 > 4u?*(( int*)
_temp922) == Cyc_Absyn_ArrayType: 0){ goto _LL956;} else{ goto _LL934;} _LL956:
if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u)) == 0){
goto _LL933;} else{ goto _LL934;} _LL934: if(( unsigned int) _temp922 > 4u?*((
int*) _temp922) == Cyc_Absyn_PointerType: 0){ goto _LL957;} else{ goto _LL936;}
_LL957: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
== 0){ goto _LL935;} else{ goto _LL936;} _LL936: goto _LL937; _LL925: if(
_temp943 == 0){ return({ void* _temp958[ 0u]={}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("unresolved struct type (COMPILER ERROR)", sizeof( unsigned char), 40u),
_tag_arr( _temp958, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp959= Cyc_Absyn_lookup_struct_field( _temp940, f); if( _temp959 == 0){
return({ struct Cyc_Stdio_String_pa_struct _temp962; _temp962.tag= Cyc_Stdio_String_pa;
_temp962.f1=( struct _tagged_arr)* f;{ struct Cyc_Stdio_String_pa_struct
_temp961; _temp961.tag= Cyc_Stdio_String_pa; _temp961.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp943));{ void*
_temp960[ 2u]={& _temp961,& _temp962}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp960, sizeof( void*), 2u));}}});}{
void* t2;{ struct _RegionHandle _temp963= _new_region(); struct _RegionHandle*
rgn=& _temp963; _push_region( rgn);{ struct Cyc_List_List* _temp964=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp940->tvs,
_temp941); t2= Cyc_Tcutil_rsubstitute( rgn, _temp964,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp959))->type);}; _pop_region( rgn);} return t2;}} _LL927: {
struct Cyc_Absyn_Structfield* _temp965= Cyc_Absyn_lookup_field( _temp945, f);
if( _temp965 == 0){ return({ struct Cyc_Stdio_String_pa_struct _temp967;
_temp967.tag= Cyc_Stdio_String_pa; _temp967.f1=( struct _tagged_arr)* f;{ void*
_temp966[ 1u]={& _temp967}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct has no %s field",
sizeof( unsigned char), 23u), _tag_arr( _temp966, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp965))->type;}
_LL929: { struct Cyc_Absyn_Structfield* _temp968= Cyc_Absyn_lookup_field(
_temp947, f); if( _temp968 == 0){ return({ struct Cyc_Stdio_String_pa_struct
_temp970; _temp970.tag= Cyc_Stdio_String_pa; _temp970.f1=( struct _tagged_arr)*
f;{ void* _temp969[ 1u]={& _temp970}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp969, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp968))->type;}
_LL931: if( _temp954 == 0){ return({ void* _temp971[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u), _tag_arr( _temp971, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp972= Cyc_Absyn_lookup_union_field( _temp951, f); if( _temp972 == 0){ return({
struct Cyc_Stdio_String_pa_struct _temp975; _temp975.tag= Cyc_Stdio_String_pa;
_temp975.f1=( struct _tagged_arr)* f;{ struct Cyc_Stdio_String_pa_struct
_temp974; _temp974.tag= Cyc_Stdio_String_pa; _temp974.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp954));{ void*
_temp973[ 2u]={& _temp974,& _temp975}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp973, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp976=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp951->tvs, _temp952); void*
_temp977= Cyc_Tcutil_substitute( _temp976,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp972))->type); return _temp977;}} _LL933:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp978=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp978[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp979; _temp979.tag= Cyc_Absyn_Primop_e; _temp979.f1=(
void*)(( void*) Cyc_Absyn_Size); _temp979.f2=({ struct Cyc_List_List* _temp980=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp980->hd=(
void*) e; _temp980->tl= 0; _temp980;}); _temp979;}); _temp978;}))); return Cyc_Absyn_uint_t;
_LL935:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp981=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp981[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp982; _temp982.tag= Cyc_Absyn_Primop_e;
_temp982.f1=( void*)(( void*) Cyc_Absyn_Size); _temp982.f2=({ struct Cyc_List_List*
_temp983=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp983->hd=( void*) e; _temp983->tl= 0; _temp983;}); _temp982;}); _temp981;})));
return Cyc_Absyn_uint_t; _LL937: if( Cyc_String_zstrcmp(* f, _tag_arr("size",
sizeof( unsigned char), 5u)) == 0){ return({ struct Cyc_Stdio_String_pa_struct
_temp985; _temp985.tag= Cyc_Stdio_String_pa; _temp985.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp984[ 1u]={& _temp985}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u), _tag_arr( _temp984, sizeof( void*), 1u));}});} else{
return({ struct Cyc_Stdio_String_pa_struct _temp987; _temp987.tag= Cyc_Stdio_String_pa;
_temp987.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp986[ 1u]={&
_temp987}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp986, sizeof( void*), 1u));}});}
_LL923:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp988= t;
struct Cyc_Absyn_PtrInfo _temp994; struct Cyc_Absyn_Conref* _temp996; void*
_temp998; void* _temp1000; _LL990: if(( unsigned int) _temp988 > 4u?*(( int*)
_temp988) == Cyc_Absyn_PointerType: 0){ _LL995: _temp994=(( struct Cyc_Absyn_PointerType_struct*)
_temp988)->f1; _LL1001: _temp1000=( void*) _temp994.elt_typ; goto _LL999; _LL999:
_temp998=( void*) _temp994.rgn_typ; goto _LL997; _LL997: _temp996= _temp994.bounds;
goto _LL991;} else{ goto _LL992;} _LL992: goto _LL993; _LL991: Cyc_Tcutil_check_nonzero_bound(
loc, _temp996);{ void* _temp1002= Cyc_Tcutil_compress( _temp1000); struct Cyc_Absyn_Structdecl**
_temp1014; struct Cyc_Absyn_Structdecl* _temp1016; struct Cyc_List_List*
_temp1017; struct _tuple1* _temp1019; struct Cyc_Absyn_Uniondecl** _temp1021;
struct Cyc_Absyn_Uniondecl* _temp1023; struct Cyc_List_List* _temp1024; struct
_tuple1* _temp1026; struct Cyc_List_List* _temp1028; struct Cyc_List_List*
_temp1030; _LL1004: if(( unsigned int) _temp1002 > 4u?*(( int*) _temp1002) ==
Cyc_Absyn_StructType: 0){ _LL1020: _temp1019=(( struct Cyc_Absyn_StructType_struct*)
_temp1002)->f1; goto _LL1018; _LL1018: _temp1017=(( struct Cyc_Absyn_StructType_struct*)
_temp1002)->f2; goto _LL1015; _LL1015: _temp1014=(( struct Cyc_Absyn_StructType_struct*)
_temp1002)->f3; if( _temp1014 == 0){ goto _LL1006;} else{ _temp1016=* _temp1014;
goto _LL1005;}} else{ goto _LL1006;} _LL1006: if(( unsigned int) _temp1002 > 4u?*((
int*) _temp1002) == Cyc_Absyn_UnionType: 0){ _LL1027: _temp1026=(( struct Cyc_Absyn_UnionType_struct*)
_temp1002)->f1; goto _LL1025; _LL1025: _temp1024=(( struct Cyc_Absyn_UnionType_struct*)
_temp1002)->f2; goto _LL1022; _LL1022: _temp1021=(( struct Cyc_Absyn_UnionType_struct*)
_temp1002)->f3; if( _temp1021 == 0){ goto _LL1008;} else{ _temp1023=* _temp1021;
goto _LL1007;}} else{ goto _LL1008;} _LL1008: if(( unsigned int) _temp1002 > 4u?*((
int*) _temp1002) == Cyc_Absyn_AnonStructType: 0){ _LL1029: _temp1028=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1002)->f1; goto _LL1009;} else{ goto
_LL1010;} _LL1010: if(( unsigned int) _temp1002 > 4u?*(( int*) _temp1002) == Cyc_Absyn_AnonUnionType:
0){ _LL1031: _temp1030=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1002)->f1;
goto _LL1011;} else{ goto _LL1012;} _LL1012: goto _LL1013; _LL1005: { struct Cyc_Absyn_Structfield*
_temp1032= Cyc_Absyn_lookup_struct_field( _temp1016, f); if( _temp1032 == 0){
return({ struct Cyc_Stdio_String_pa_struct _temp1035; _temp1035.tag= Cyc_Stdio_String_pa;
_temp1035.f1=( struct _tagged_arr)* f;{ struct Cyc_Stdio_String_pa_struct
_temp1034; _temp1034.tag= Cyc_Stdio_String_pa; _temp1034.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1019));{ void*
_temp1033[ 2u]={& _temp1034,& _temp1035}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp1033, sizeof( void*), 2u));}}});}{
void* t3;{ struct _RegionHandle _temp1036= _new_region(); struct _RegionHandle*
rgn=& _temp1036; _push_region( rgn);{ struct Cyc_List_List* _temp1037=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1016->tvs,
_temp1017); t3= Cyc_Tcutil_rsubstitute( rgn, _temp1037,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1032))->type);}; _pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp998); return t3;}} _LL1007: { struct Cyc_Absyn_Structfield*
_temp1038= Cyc_Absyn_lookup_union_field( _temp1023, f); if( _temp1038 == 0){
return({ struct Cyc_Stdio_String_pa_struct _temp1041; _temp1041.tag= Cyc_Stdio_String_pa;
_temp1041.f1=( struct _tagged_arr)* f;{ struct Cyc_Stdio_String_pa_struct
_temp1040; _temp1040.tag= Cyc_Stdio_String_pa; _temp1040.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1026));{ void*
_temp1039[ 2u]={& _temp1040,& _temp1041}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp1039, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp1042=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1023->tvs, _temp1024); void*
_temp1043= Cyc_Tcutil_substitute( _temp1042,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1038))->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp998); return _temp1043;}} _LL1009: { struct Cyc_Absyn_Structfield*
_temp1044= Cyc_Absyn_lookup_field( _temp1028, f); if( _temp1044 == 0){ return({
struct Cyc_Stdio_String_pa_struct _temp1046; _temp1046.tag= Cyc_Stdio_String_pa;
_temp1046.f1=( struct _tagged_arr)* f;{ void* _temp1045[ 1u]={& _temp1046}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1045, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1044))->type;} _LL1011: { struct Cyc_Absyn_Structfield*
_temp1047= Cyc_Absyn_lookup_field( _temp1030, f); if( _temp1047 == 0){ return({
struct Cyc_Stdio_String_pa_struct _temp1049; _temp1049.tag= Cyc_Stdio_String_pa;
_temp1049.f1=( struct _tagged_arr)* f;{ void* _temp1048[ 1u]={& _temp1049}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union has no %s field", sizeof( unsigned char), 22u),
_tag_arr( _temp1048, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1047))->type;} _LL1013: return({ struct Cyc_Stdio_String_pa_struct
_temp1051; _temp1051.tag= Cyc_Stdio_String_pa; _temp1051.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1050[ 1u]={& _temp1051}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct pointer, found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1050, sizeof( void*), 1u));}}); _LL1003:;} _LL993: return({
struct Cyc_Stdio_String_pa_struct _temp1053; _temp1053.tag= Cyc_Stdio_String_pa;
_temp1053.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1052[
1u]={& _temp1053}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1052, sizeof( void*), 1u));}});
_LL989:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1054; _push_handler(& _temp1054);{ int _temp1056= 0; if(
setjmp( _temp1054.handler)){ _temp1056= 1;} if( ! _temp1056){{ void* _temp1057=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1057;}; _pop_handler();} else{ void* _temp1055=(
void*) _exn_thrown; void* _temp1059= _temp1055; _LL1061: if( _temp1059 == Cyc_List_Nth){
goto _LL1062;} else{ goto _LL1063;} _LL1063: goto _LL1064; _LL1062: return({
struct Cyc_Stdio_Int_pa_struct _temp1067; _temp1067.tag= Cyc_Stdio_Int_pa;
_temp1067.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ts));{ struct Cyc_Stdio_Int_pa_struct _temp1066; _temp1066.tag= Cyc_Stdio_Int_pa;
_temp1066.f1=( int) i;{ void* _temp1065[ 2u]={& _temp1066,& _temp1067}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("index is %d but tuple has only %d fields", sizeof(
unsigned char), 41u), _tag_arr( _temp1065, sizeof( void*), 2u));}}}); _LL1064:(
void) _throw( _temp1059); _LL1060:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
struct Cyc_Stdio_String_pa_struct _temp1069; _temp1069.tag= Cyc_Stdio_String_pa;
_temp1069.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp1068[
1u]={& _temp1069}; Cyc_Tcexp_expr_err( te, e2->loc, _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp1068, sizeof( void*), 1u));}});}{
void* _temp1070= t1; struct Cyc_Absyn_PtrInfo _temp1078; struct Cyc_Absyn_Conref*
_temp1080; struct Cyc_Absyn_Tqual _temp1082; void* _temp1084; void* _temp1086;
struct Cyc_List_List* _temp1088; _LL1072: if(( unsigned int) _temp1070 > 4u?*((
int*) _temp1070) == Cyc_Absyn_PointerType: 0){ _LL1079: _temp1078=(( struct Cyc_Absyn_PointerType_struct*)
_temp1070)->f1; _LL1087: _temp1086=( void*) _temp1078.elt_typ; goto _LL1085;
_LL1085: _temp1084=( void*) _temp1078.rgn_typ; goto _LL1083; _LL1083: _temp1082=
_temp1078.tq; goto _LL1081; _LL1081: _temp1080= _temp1078.bounds; goto _LL1073;}
else{ goto _LL1074;} _LL1074: if(( unsigned int) _temp1070 > 4u?*(( int*)
_temp1070) == Cyc_Absyn_TupleType: 0){ _LL1089: _temp1088=(( struct Cyc_Absyn_TupleType_struct*)
_temp1070)->f1; goto _LL1075;} else{ goto _LL1076;} _LL1076: goto _LL1077;
_LL1073: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1080);} else{{ void* _temp1090=( void*)( Cyc_Absyn_compress_conref(
_temp1080))->v; void* _temp1096; struct Cyc_Absyn_Exp* _temp1098; struct Cyc_Absyn_Exp
_temp1100; void* _temp1101; void* _temp1103; int _temp1105; void* _temp1107;
_LL1092: if(( unsigned int) _temp1090 > 1u?*(( int*) _temp1090) == Cyc_Absyn_Eq_constr:
0){ _LL1097: _temp1096=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1090)->f1;
if(( unsigned int) _temp1096 > 1u?*(( int*) _temp1096) == Cyc_Absyn_Upper_b: 0){
_LL1099: _temp1098=(( struct Cyc_Absyn_Upper_b_struct*) _temp1096)->f1;
_temp1100=* _temp1098; _LL1102: _temp1101=( void*) _temp1100.r; if(*(( int*)
_temp1101) == Cyc_Absyn_Const_e){ _LL1104: _temp1103=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1101)->f1; if(( unsigned int) _temp1103 > 1u?*(( int*) _temp1103) == Cyc_Absyn_Int_c:
0){ _LL1108: _temp1107=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1103)->f1;
if( _temp1107 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1106;} else{ goto _LL1094;}
_LL1106: _temp1105=(( struct Cyc_Absyn_Int_c_struct*) _temp1103)->f2; if(
_temp1105 == 1){ goto _LL1093;} else{ goto _LL1094;}} else{ goto _LL1094;}}
else{ goto _LL1094;}} else{ goto _LL1094;}} else{ goto _LL1094;} _LL1094: goto
_LL1095; _LL1093:({ void* _temp1109[ 0u]={}; Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u), _tag_arr( _temp1109, sizeof( void*), 0u));}); goto
_LL1091; _LL1095: goto _LL1091; _LL1091:;} Cyc_Tcutil_check_nonzero_bound( loc,
_temp1080);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1084); return
_temp1086; _LL1075: return Cyc_Tcexp_ithTupleType( te, loc, _temp1088, e2);
_LL1077: return({ struct Cyc_Stdio_String_pa_struct _temp1111; _temp1111.tag=
Cyc_Stdio_String_pa; _temp1111.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1110[ 1u]={& _temp1111}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1110, sizeof( void*), 1u));}});
_LL1071:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt != 0){ void* _temp1112= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_List_List* _temp1118; _LL1114: if((
unsigned int) _temp1112 > 4u?*(( int*) _temp1112) == Cyc_Absyn_TupleType: 0){
_LL1119: _temp1118=(( struct Cyc_Absyn_TupleType_struct*) _temp1112)->f1; goto
_LL1115;} else{ goto _LL1116;} _LL1116: goto _LL1117; _LL1115: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1118) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){({ void* _temp1120[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("tuple expression has the wrong number of fields", sizeof(
unsigned char), 48u), _tag_arr( _temp1120, sizeof( void*), 0u));}); goto _LL1113;}
for( 0; es != 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl, _temp1118=((
struct Cyc_List_List*) _check_null( _temp1118))->tl)){ void* _temp1121=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1118))->hd)).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1121,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1122=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1122->hd=( void*)({ struct
_tuple8* _temp1123=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1123->f1=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1118))->hd)).f1; _temp1123->f2=( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1123;}); _temp1122->tl= fields; _temp1122;});} done= 1; goto _LL1113;
_LL1117: goto _LL1113; _LL1113:;} if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); fields=({ struct Cyc_List_List*
_temp1124=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1124->hd=( void*)({ struct _tuple8* _temp1125=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1125->f1= Cyc_Absyn_empty_tqual(); _temp1125->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1125;}); _temp1124->tl=
fields; _temp1124;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1126=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1126[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1127; _temp1127.tag= Cyc_Absyn_TupleType;
_temp1127.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1127;}); _temp1126;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void* _temp1128[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("tcCompoundLit", sizeof( unsigned char), 14u), _tag_arr(
_temp1128, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1151=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1151->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1151;})); struct Cyc_Absyn_Const_e_struct* _temp1129=({ struct Cyc_Absyn_Const_e_struct*
_temp1147=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1147[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1148; _temp1148.tag= Cyc_Absyn_Const_e;
_temp1148.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1149=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1149[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1150; _temp1150.tag= Cyc_Absyn_Int_c;
_temp1150.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1150.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1150;}); _temp1149;}));
_temp1148;}); _temp1147;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) _temp1129, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1130=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1130->v=( void*)
Cyc_Absyn_uint_t; _temp1130;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1145=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1145[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1146; _temp1146.tag= Cyc_Absyn_ArrayType;
_temp1146.f1=( void*) res; _temp1146.f2= Cyc_Absyn_empty_tqual(); _temp1146.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1146;}); _temp1145;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){({ struct
Cyc_Stdio_String_pa_struct _temp1132; _temp1132.tag= Cyc_Stdio_String_pa;
_temp1132.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( res);{ void*
_temp1131[ 1u]={& _temp1132}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc, _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp1131, sizeof( void*), 1u));}});}{
int offset= 0; for( 0; des != 0;( offset ++, des=(( struct Cyc_List_List*)
_check_null( des))->tl)){ struct Cyc_List_List* ds=(*(( struct _tuple7*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1; if( ds != 0){ void*
_temp1133=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp*
_temp1139; _LL1135: if(*(( int*) _temp1133) == Cyc_Absyn_FieldName){ goto
_LL1136;} else{ goto _LL1137;} _LL1137: if(*(( int*) _temp1133) == Cyc_Absyn_ArrayElement){
_LL1140: _temp1139=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1133)->f1;
goto _LL1138;} else{ goto _LL1134;} _LL1136:({ void* _temp1141[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("only array index designators are supported", sizeof(
unsigned char), 43u), _tag_arr( _temp1141, sizeof( void*), 0u));}); goto _LL1134;
_LL1138: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1139);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1139); if( i != offset){({ struct Cyc_Stdio_Int_pa_struct _temp1144;
_temp1144.tag= Cyc_Stdio_Int_pa; _temp1144.f1=( int) i;{ struct Cyc_Stdio_Int_pa_struct
_temp1143; _temp1143.tag= Cyc_Stdio_Int_pa; _temp1143.f1=( int)(( unsigned int)
offset);{ void* _temp1142[ 2u]={& _temp1143,& _temp1144}; Cyc_Tcutil_terr(
_temp1139->loc, _tag_arr("expecting index designator %d but found %d", sizeof(
unsigned char), 43u), _tag_arr( _temp1142, sizeof( void*), 2u));}}});} goto
_LL1134;} _LL1134:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({
struct Cyc_Stdio_String_pa_struct _temp1153; _temp1153.tag= Cyc_Stdio_String_pa;
_temp1153.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v);{ void* _temp1152[ 1u]={& _temp1153}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("expecting unsigned int, found %s", sizeof( unsigned char),
33u), _tag_arr( _temp1152, sizeof( void*), 1u));}});} if( !( vd->tq).q_const){({
void* _temp1154[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u), _tag_arr( _temp1154, sizeof( void*), 0u));});} if(
te->le != 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){({ void*
_temp1155[ 0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant",
sizeof( unsigned char), 22u), _tag_arr( _temp1155, sizeof( void*), 0u));});} if(
! Cyc_Tcutil_is_const_exp( te, body)){({ void* _temp1156[ 0u]={}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("body is not constant", sizeof( unsigned char), 21u),
_tag_arr( _temp1156, sizeof( void*), 0u));});}}{ struct Cyc_Absyn_PtrInfo pinfo;
void** _temp1157= 0; struct Cyc_Absyn_Tqual* _temp1158= 0; if( topt != 0){ void*
_temp1159= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1167; struct Cyc_Absyn_Exp* _temp1169; struct Cyc_Absyn_Tqual _temp1171;
struct Cyc_Absyn_Tqual* _temp1173; void* _temp1174; void** _temp1176; _LL1161:
if(( unsigned int) _temp1159 > 4u?*(( int*) _temp1159) == Cyc_Absyn_PointerType:
0){ _LL1168: _temp1167=(( struct Cyc_Absyn_PointerType_struct*) _temp1159)->f1;
goto _LL1162;} else{ goto _LL1163;} _LL1163: if(( unsigned int) _temp1159 > 4u?*((
int*) _temp1159) == Cyc_Absyn_ArrayType: 0){ _LL1175: _temp1174=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1159)->f1; _temp1176=&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1159)->f1); goto _LL1172; _LL1172:
_temp1171=(( struct Cyc_Absyn_ArrayType_struct*) _temp1159)->f2; _temp1173=&((
struct Cyc_Absyn_ArrayType_struct*) _temp1159)->f2; goto _LL1170; _LL1170:
_temp1169=(( struct Cyc_Absyn_ArrayType_struct*) _temp1159)->f3; goto _LL1164;}
else{ goto _LL1165;} _LL1165: goto _LL1166; _LL1162: pinfo= _temp1167; _temp1157=(
void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1158=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1160; _LL1164: _temp1157=( void**) _temp1176; _temp1158=(
struct Cyc_Absyn_Tqual*) _temp1173; goto _LL1160; _LL1166: goto _LL1160; _LL1160:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1157, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1177=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1177[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1178; _temp1178.tag= Cyc_Absyn_ArrayType;
_temp1178.f1=( void*) t; _temp1178.f2= _temp1158 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1158)); _temp1178.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1178;}); _temp1177;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1179; _push_handler(&
_temp1179);{ int _temp1181= 0; if( setjmp( _temp1179.handler)){ _temp1181= 1;}
if( ! _temp1181){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1180=( void*) _exn_thrown; void* _temp1183= _temp1180; _LL1185:
if( _temp1183 == Cyc_Dict_Absent){ goto _LL1186;} else{ goto _LL1187;} _LL1187:
goto _LL1188; _LL1186:({ struct Cyc_Stdio_String_pa_struct _temp1190; _temp1190.tag=
Cyc_Stdio_String_pa; _temp1190.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1189[ 1u]={& _temp1190}; Cyc_Tcutil_terr( loc, _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1189, sizeof( void*), 1u));}});
return topt != 0?*(( void**) _check_null( topt)):( void*) Cyc_Absyn_VoidType;
_LL1188:( void) _throw( _temp1183); _LL1184:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*)
sd; if( sd->name != 0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v;}}{ struct _RegionHandle _temp1191= _new_region();
struct _RegionHandle* rgn=& _temp1191; _push_region( rgn);{ struct _tuple4
_temp1192=({ struct _tuple4 _temp1216; _temp1216.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1216.f2= rgn; _temp1216;}); struct Cyc_List_List* _temp1193=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1192, sd->tvs); struct Cyc_List_List*
_temp1194=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1193); struct
Cyc_Absyn_StructType_struct* _temp1195=({ struct Cyc_Absyn_StructType_struct*
_temp1213=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1213[ 0]=({ struct Cyc_Absyn_StructType_struct _temp1214; _temp1214.tag=
Cyc_Absyn_StructType; _temp1214.f1=( struct _tuple1*)* tn; _temp1214.f2=
_temp1194; _temp1214.f3=({ struct Cyc_Absyn_Structdecl** _temp1215=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1215[ 0]= sd; _temp1215;});
_temp1214;}); _temp1213;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) _temp1195,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1196=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1196->v=( void*)
_temp1194; _temp1196;}); if( sd->fields == 0){({ void* _temp1197[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u),
_tag_arr( _temp1197, sizeof( void*), 0u));});{ void* _temp1198=( void*)
_temp1195; _npop_handler( 0u); return _temp1198;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1201; struct Cyc_Absyn_Exp* _temp1202; struct Cyc_Absyn_Structfield*
_temp1204; struct _tuple9* _temp1199=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1201=* _temp1199; _LL1205: _temp1204= _temp1201.f1;
goto _LL1203; _LL1203: _temp1202= _temp1201.f2; goto _LL1200; _LL1200: { void*
_temp1206= Cyc_Tcutil_rsubstitute( rgn, _temp1193,( void*) _temp1204->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1206, _temp1202); if( ! Cyc_Tcutil_coerce_arg( te, _temp1202,
_temp1206)){({ struct Cyc_Stdio_String_pa_struct _temp1211; _temp1211.tag= Cyc_Stdio_String_pa;
_temp1211.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1202->topt))->v);{ struct Cyc_Stdio_String_pa_struct _temp1210;
_temp1210.tag= Cyc_Stdio_String_pa; _temp1210.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1206);{ struct Cyc_Stdio_String_pa_struct _temp1209; _temp1209.tag= Cyc_Stdio_String_pa;
_temp1209.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(* tn);{ struct Cyc_Stdio_String_pa_struct
_temp1208; _temp1208.tag= Cyc_Stdio_String_pa; _temp1208.f1=( struct _tagged_arr)*
_temp1204->name;{ void* _temp1207[ 4u]={& _temp1208,& _temp1209,& _temp1210,&
_temp1211}; Cyc_Tcutil_terr( _temp1202->loc, _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1207, sizeof( void*), 4u));}}}}});}}}{
void* _temp1212=( void*) _temp1195; _npop_handler( 0u); return _temp1212;}}};
_pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1217= _new_region(); struct _RegionHandle* rgn=&
_temp1217; _push_region( rgn);{ void* _temp1218= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1224; _LL1220: if(( unsigned int) _temp1218 > 4u?*((
int*) _temp1218) == Cyc_Absyn_AnonStructType: 0){ _LL1225: _temp1224=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1218)->f1; goto _LL1221;} else{ goto
_LL1222;} _LL1222: goto _LL1223; _LL1221: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1224); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1228; struct Cyc_Absyn_Exp*
_temp1229; struct Cyc_Absyn_Structfield* _temp1231; struct _tuple9* _temp1226=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1228=*
_temp1226; _LL1232: _temp1231= _temp1228.f1; goto _LL1230; _LL1230: _temp1229=
_temp1228.f2; goto _LL1227; _LL1227: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1231->type)), _temp1229); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1229,( void*) _temp1231->type)){({ struct Cyc_Stdio_String_pa_struct
_temp1236; _temp1236.tag= Cyc_Stdio_String_pa; _temp1236.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1229->topt))->v);{
struct Cyc_Stdio_String_pa_struct _temp1235; _temp1235.tag= Cyc_Stdio_String_pa;
_temp1235.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*) _temp1231->type);{
struct Cyc_Stdio_String_pa_struct _temp1234; _temp1234.tag= Cyc_Stdio_String_pa;
_temp1234.f1=( struct _tagged_arr)* _temp1231->name;{ void* _temp1233[ 3u]={&
_temp1234,& _temp1235,& _temp1236}; Cyc_Tcutil_terr( _temp1229->loc, _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1233, sizeof( void*), 3u));}}}});}}
goto _LL1219;} _LL1223:({ void* _temp1237[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u), _tag_arr( _temp1237, sizeof( void*), 0u));}); goto
_LL1219; _LL1219:;}; _pop_region( rgn);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1238= _new_region(); struct _RegionHandle* rgn=&
_temp1238; _push_region( rgn);{ struct _tuple4 _temp1239=({ struct _tuple4
_temp1284; _temp1284.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1284.f2= rgn;
_temp1284;}); struct Cyc_List_List* _temp1240=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1239, tud->tvs); struct Cyc_List_List* _temp1241=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1239, tuf->tvs); struct Cyc_List_List* _temp1242=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1240); struct Cyc_List_List* _temp1243=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1241); struct Cyc_List_List*
_temp1244=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1240, _temp1241);*
all_ref=({ struct Cyc_Core_Opt* _temp1245=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1245->v=( void*) _temp1242; _temp1245;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1246=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1246->v=( void*) _temp1243; _temp1246;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1279=( struct
Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1279[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1280; _temp1280.tag=
Cyc_Absyn_TunionFieldType; _temp1280.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1281; _temp1281.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1282=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1282[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1283; _temp1283.tag= Cyc_Absyn_KnownTunionfield; _temp1283.f1= tud;
_temp1283.f2= tuf; _temp1283;}); _temp1282;})); _temp1281.targs= _temp1242;
_temp1281;}); _temp1280;}); _temp1279;}); if( topt != 0){ void* _temp1247= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionInfo _temp1255; void*
_temp1257; struct Cyc_List_List* _temp1259; _LL1249: if(( unsigned int)
_temp1247 > 4u?*(( int*) _temp1247) == Cyc_Absyn_TunionFieldType: 0){ goto
_LL1250;} else{ goto _LL1251;} _LL1251: if(( unsigned int) _temp1247 > 4u?*((
int*) _temp1247) == Cyc_Absyn_TunionType: 0){ _LL1256: _temp1255=(( struct Cyc_Absyn_TunionType_struct*)
_temp1247)->f1; _LL1260: _temp1259= _temp1255.targs; goto _LL1258; _LL1258:
_temp1257=( void*) _temp1255.rgn; goto _LL1252;} else{ goto _LL1253;} _LL1253:
goto _LL1254; _LL1250: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res);
goto _LL1248; _LL1252:{ struct Cyc_List_List* a= _temp1242; for( 0; a != 0?
_temp1259 != 0: 0;( a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1259=((
struct Cyc_List_List*) _check_null( _temp1259))->tl)){ Cyc_Tcutil_unify(( void*)((
struct Cyc_List_List*) _check_null( a))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp1259))->hd);}} if( tuf->typs == 0? es == 0: 0){ e->topt=({
struct Cyc_Core_Opt* _temp1261=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1261->v=( void*) res; _temp1261;}); res=( void*)({ struct
Cyc_Absyn_TunionType_struct* _temp1262=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1262[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1263; _temp1263.tag= Cyc_Absyn_TunionType;
_temp1263.f1=({ struct Cyc_Absyn_TunionInfo _temp1264; _temp1264.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1265=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1265[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1266; _temp1266.tag= Cyc_Absyn_KnownTunion;
_temp1266.f1= tud; _temp1266;}); _temp1265;})); _temp1264.targs= _temp1242;
_temp1264.rgn=( void*) _temp1257; _temp1264;}); _temp1263;}); _temp1262;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1267= res; _npop_handler( 0u); return _temp1267;}} goto
_LL1248; _LL1254: goto _LL1248; _LL1248:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es != 0? ts != 0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1244,(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct Cyc_Stdio_String_pa_struct
_temp1271; _temp1271.tag= Cyc_Stdio_String_pa; _temp1271.f1=( struct _tagged_arr)(
e->topt == 0? _tag_arr("?", sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ struct Cyc_Stdio_String_pa_struct
_temp1270; _temp1270.tag= Cyc_Stdio_String_pa; _temp1270.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Stdio_String_pa_struct _temp1269;
_temp1269.tag= Cyc_Stdio_String_pa; _temp1269.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1268[ 3u]={& _temp1269,& _temp1270,& _temp1271}; Cyc_Tcutil_terr(
e->loc, _tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u), _tag_arr( _temp1268, sizeof( void*), 3u));}}}});}}
if( es != 0){ void* _temp1274=({ struct Cyc_Stdio_String_pa_struct _temp1273;
_temp1273.tag= Cyc_Stdio_String_pa; _temp1273.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1272[ 1u]={& _temp1273}; Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,
_tag_arr("too many arguments for tunion constructor %s", sizeof( unsigned char),
45u), _tag_arr( _temp1272, sizeof( void*), 1u));}}); _npop_handler( 0u); return
_temp1274;} if( ts != 0){ void* _temp1277=({ struct Cyc_Stdio_String_pa_struct
_temp1276; _temp1276.tag= Cyc_Stdio_String_pa; _temp1276.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1275[ 1u]={& _temp1276}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("too few arguments for tunion constructor %s", sizeof(
unsigned char), 44u), _tag_arr( _temp1275, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1277;}{ void* _temp1278= res; _npop_handler( 0u);
return _temp1278;}}}}; _pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt !=
0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1295=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1295[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1296; _temp1296.tag= Cyc_Absyn_RgnHandleType; _temp1296.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1297=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1297->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1297;})); _temp1296;}); _temp1295;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1285= Cyc_Tcutil_compress( handle_type); void*
_temp1291; _LL1287: if(( unsigned int) _temp1285 > 4u?*(( int*) _temp1285) ==
Cyc_Absyn_RgnHandleType: 0){ _LL1292: _temp1291=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1285)->f1; goto _LL1288;} else{ goto _LL1289;} _LL1289: goto _LL1290;
_LL1288: rgn= _temp1291; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1286; _LL1290:({ struct Cyc_Stdio_String_pa_struct _temp1294; _temp1294.tag=
Cyc_Stdio_String_pa; _temp1294.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1293[ 1u]={& _temp1294}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1293, sizeof( void*), 1u));}});
goto _LL1286; _LL1286:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t);{ void* _temp1298= t; struct Cyc_Absyn_TunionFieldInfo
_temp1304; void* _temp1306; struct Cyc_Absyn_Tunionfield* _temp1308; _LL1300:
if(( unsigned int) _temp1298 > 4u?*(( int*) _temp1298) == Cyc_Absyn_TunionFieldType:
0){ _LL1305: _temp1304=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1298)->f1;
_LL1307: _temp1306=( void*) _temp1304.field_info; if(*(( int*) _temp1306) == Cyc_Absyn_KnownTunionfield){
_LL1309: _temp1308=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1306)->f2;
goto _LL1301;} else{ goto _LL1302;}} else{ goto _LL1302;} _LL1302: goto _LL1303;
_LL1301: if( _temp1308->tvs != 0){({ void* _temp1310[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("malloc with existential types not yet implemented", sizeof(
unsigned char), 50u), _tag_arr( _temp1310, sizeof( void*), 0u));});} goto
_LL1299; _LL1303: goto _LL1299; _LL1299:;}{ void*(* _temp1311)( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt != 0){ void*
_temp1312= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1318; struct Cyc_Absyn_Conref* _temp1320; _LL1314: if(( unsigned int)
_temp1312 > 4u?*(( int*) _temp1312) == Cyc_Absyn_PointerType: 0){ _LL1319:
_temp1318=(( struct Cyc_Absyn_PointerType_struct*) _temp1312)->f1; _LL1321:
_temp1320= _temp1318.nullable; goto _LL1315;} else{ goto _LL1316;} _LL1316: goto
_LL1317; _LL1315: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1320)){ _temp1311= Cyc_Absyn_star_typ;} goto _LL1313; _LL1317: goto _LL1313;
_LL1313:;} return _temp1311( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1322=( void*) s->r; struct Cyc_Absyn_Exp* _temp1332;
struct Cyc_Absyn_Stmt* _temp1334; struct Cyc_Absyn_Stmt* _temp1336; struct Cyc_Absyn_Stmt*
_temp1338; struct Cyc_Absyn_Decl* _temp1340; _LL1324: if(( unsigned int)
_temp1322 > 1u?*(( int*) _temp1322) == Cyc_Absyn_Exp_s: 0){ _LL1333: _temp1332=((
struct Cyc_Absyn_Exp_s_struct*) _temp1322)->f1; goto _LL1325;} else{ goto
_LL1326;} _LL1326: if(( unsigned int) _temp1322 > 1u?*(( int*) _temp1322) == Cyc_Absyn_Seq_s:
0){ _LL1337: _temp1336=(( struct Cyc_Absyn_Seq_s_struct*) _temp1322)->f1; goto
_LL1335; _LL1335: _temp1334=(( struct Cyc_Absyn_Seq_s_struct*) _temp1322)->f2;
goto _LL1327;} else{ goto _LL1328;} _LL1328: if(( unsigned int) _temp1322 > 1u?*((
int*) _temp1322) == Cyc_Absyn_Decl_s: 0){ _LL1341: _temp1340=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1322)->f1; goto _LL1339; _LL1339: _temp1338=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1322)->f2; goto _LL1329;} else{ goto _LL1330;} _LL1330: goto _LL1331;
_LL1325: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1332->topt))->v;
_LL1327: s= _temp1334; continue; _LL1329: s= _temp1338; continue; _LL1331:
return({ void* _temp1342[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u), _tag_arr( _temp1342, sizeof( void*), 0u));});
_LL1323:;}} static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({
void* _temp1343[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u), _tag_arr( _temp1343, sizeof( void*), 0u));});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void* _temp1344[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u),
_tag_arr( _temp1344, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle != 0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1355=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1355[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1356; _temp1356.tag= Cyc_Absyn_RgnHandleType;
_temp1356.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp1357=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1357->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1357;}));
_temp1356;}); _temp1355;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void*
_temp1345= Cyc_Tcutil_compress( handle_type); void* _temp1351; _LL1347: if((
unsigned int) _temp1345 > 4u?*(( int*) _temp1345) == Cyc_Absyn_RgnHandleType: 0){
_LL1352: _temp1351=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1345)->f1;
goto _LL1348;} else{ goto _LL1349;} _LL1349: goto _LL1350; _LL1348: rgn=
_temp1351; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto _LL1346; _LL1350:({
struct Cyc_Stdio_String_pa_struct _temp1354; _temp1354.tag= Cyc_Stdio_String_pa;
_temp1354.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( handle_type);{ void*
_temp1353[ 1u]={& _temp1354}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1353, sizeof( void*), 1u));}});
goto _LL1346; _LL1346:;}{ void* _temp1358=( void*) e1->r; struct Cyc_Absyn_Exp*
_temp1370; struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Absyn_Vardecl* _temp1374;
struct Cyc_List_List* _temp1376; struct Cyc_Core_Opt* _temp1378; struct Cyc_List_List*
_temp1380; void* _temp1382; struct _tagged_arr _temp1384; _LL1360: if(*(( int*)
_temp1358) == Cyc_Absyn_Comprehension_e){ _LL1375: _temp1374=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1358)->f1; goto _LL1373; _LL1373: _temp1372=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1358)->f2; goto _LL1371; _LL1371: _temp1370=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1358)->f3; goto _LL1361;} else{ goto _LL1362;} _LL1362: if(*(( int*)
_temp1358) == Cyc_Absyn_UnresolvedMem_e){ _LL1379: _temp1378=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1358)->f1; goto _LL1377; _LL1377: _temp1376=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1358)->f2; goto _LL1363;} else{ goto _LL1364;} _LL1364: if(*(( int*)
_temp1358) == Cyc_Absyn_Array_e){ _LL1381: _temp1380=(( struct Cyc_Absyn_Array_e_struct*)
_temp1358)->f1; goto _LL1365;} else{ goto _LL1366;} _LL1366: if(*(( int*)
_temp1358) == Cyc_Absyn_Const_e){ _LL1383: _temp1382=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1358)->f1; if(( unsigned int) _temp1382 > 1u?*(( int*) _temp1382) == Cyc_Absyn_String_c:
0){ _LL1385: _temp1384=(( struct Cyc_Absyn_String_c_struct*) _temp1382)->f1;
goto _LL1367;} else{ goto _LL1368;}} else{ goto _LL1368;} _LL1368: goto _LL1369;
_LL1361: { void* _temp1386= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1387= Cyc_Tcutil_compress( _temp1386); struct Cyc_Absyn_Tqual _temp1393;
void* _temp1395; _LL1389: if(( unsigned int) _temp1387 > 4u?*(( int*) _temp1387)
== Cyc_Absyn_ArrayType: 0){ _LL1396: _temp1395=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1387)->f1; goto _LL1394; _LL1394: _temp1393=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1387)->f2; goto _LL1390;} else{ goto _LL1391;} _LL1391: goto _LL1392;
_LL1390: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1372)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1401=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1401[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1402; _temp1402.tag= Cyc_Absyn_Upper_b; _temp1402.f1=
_temp1372; _temp1402;}); _temp1401;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1398=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1398[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1399; _temp1399.tag= Cyc_Absyn_PointerType;
_temp1399.f1=({ struct Cyc_Absyn_PtrInfo _temp1400; _temp1400.elt_typ=( void*)
_temp1395; _temp1400.rgn_typ=( void*) rgn; _temp1400.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1400.tq= _temp1393; _temp1400.bounds= Cyc_Absyn_new_conref(
b); _temp1400;}); _temp1399;}); _temp1398;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1397=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1397->v=( void*) res_typ; _temp1397;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return
res_typ;} _LL1392: return({ void* _temp1403[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u), _tag_arr( _temp1403, sizeof( void*), 0u));});
_LL1388:;} _LL1363:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1404=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1404[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1405; _temp1405.tag= Cyc_Absyn_Array_e;
_temp1405.f1= _temp1376; _temp1405;}); _temp1404;}))); _temp1380= _temp1376;
goto _LL1365; _LL1365: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1406=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1412; struct Cyc_Absyn_Tqual _temp1414; void* _temp1416; void** _temp1418;
_LL1408: if(( unsigned int) _temp1406 > 4u?*(( int*) _temp1406) == Cyc_Absyn_PointerType:
0){ _LL1413: _temp1412=(( struct Cyc_Absyn_PointerType_struct*) _temp1406)->f1;
_LL1417: _temp1416=( void*) _temp1412.elt_typ; _temp1418=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1406)->f1).elt_typ; goto _LL1415; _LL1415: _temp1414= _temp1412.tq; goto
_LL1409;} else{ goto _LL1410;} _LL1410: goto _LL1411; _LL1409: elt_typ_opt=(
void**) _temp1418; goto _LL1407; _LL1411: goto _LL1407; _LL1407:;}{ void*
_temp1419= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1420= Cyc_Tcutil_compress( _temp1419); struct Cyc_Absyn_Exp* _temp1426;
struct Cyc_Absyn_Tqual _temp1428; void* _temp1430; _LL1422: if(( unsigned int)
_temp1420 > 4u?*(( int*) _temp1420) == Cyc_Absyn_ArrayType: 0){ _LL1431:
_temp1430=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1420)->f1; goto
_LL1429; _LL1429: _temp1428=(( struct Cyc_Absyn_ArrayType_struct*) _temp1420)->f2;
goto _LL1427; _LL1427: _temp1426=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1420)->f3; goto _LL1423;} else{ goto _LL1424;} _LL1424: goto _LL1425;
_LL1423: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1432=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1432[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1433; _temp1433.tag=
Cyc_Absyn_PointerType; _temp1433.f1=({ struct Cyc_Absyn_PtrInfo _temp1434;
_temp1434.elt_typ=( void*) _temp1430; _temp1434.rgn_typ=( void*) rgn; _temp1434.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1434.tq=
_temp1428; _temp1434.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1435=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1435[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1436; _temp1436.tag= Cyc_Absyn_Upper_b;
_temp1436.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1426); _temp1436;});
_temp1435;})); _temp1434;}); _temp1433;}); _temp1432;}); if( topt != 0){ if( !
Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1437=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1437->v=( void*) res_typ; _temp1437;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1421; _LL1425: return({ void* _temp1438[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u), _tag_arr( _temp1438, sizeof( void*), 0u));});
_LL1421:;} return res_typ;}} _LL1367: { void* _temp1439= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b); void* _temp1440= Cyc_Tcexp_tcExp(
te,( void**)& _temp1439, e1); return Cyc_Absyn_atb_typ( _temp1440, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1441=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1441[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1442; _temp1442.tag= Cyc_Absyn_Upper_b; _temp1442.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1442;}); _temp1441;}));} _LL1369: { void** topt2=
0; if( topt != 0){ void* _temp1443= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1451; struct Cyc_Absyn_Conref* _temp1453;
void* _temp1455; void** _temp1457; _LL1445: if(( unsigned int) _temp1443 > 4u?*((
int*) _temp1443) == Cyc_Absyn_PointerType: 0){ _LL1452: _temp1451=(( struct Cyc_Absyn_PointerType_struct*)
_temp1443)->f1; _LL1456: _temp1455=( void*) _temp1451.elt_typ; _temp1457=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1443)->f1).elt_typ; goto _LL1454;
_LL1454: _temp1453= _temp1451.nullable; goto _LL1446;} else{ goto _LL1447;}
_LL1447: if(( unsigned int) _temp1443 > 4u?*(( int*) _temp1443) == Cyc_Absyn_TunionType:
0){ goto _LL1448;} else{ goto _LL1449;} _LL1449: goto _LL1450; _LL1446: topt2=(
void**) _temp1457; goto _LL1444; _LL1448: topt2=({ void** _temp1458=( void**)
GC_malloc( sizeof( void*)); _temp1458[ 0]=*(( void**) _check_null( topt));
_temp1458;}); goto _LL1444; _LL1450: goto _LL1444; _LL1444:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1460=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1460[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1461; _temp1461.tag=
Cyc_Absyn_PointerType; _temp1461.f1=({ struct Cyc_Absyn_PtrInfo _temp1462;
_temp1462.elt_typ=( void*) telt; _temp1462.rgn_typ=( void*) rgn; _temp1462.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1462.tq= Cyc_Absyn_empty_tqual();
_temp1462.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1463=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1463[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1464; _temp1464.tag= Cyc_Absyn_Upper_b;
_temp1464.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1464;}); _temp1463;})); _temp1462;});
_temp1461;}); _temp1460;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1459=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1459->v=(
void*) res_typ; _temp1459;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}}
_LL1359:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress( Cyc_Tcexp_tcExpNoPromote(
te, topt, e)); void* _temp1465= t; struct Cyc_Absyn_Exp* _temp1471; struct Cyc_Absyn_Tqual
_temp1473; void* _temp1475; _LL1467: if(( unsigned int) _temp1465 > 4u?*(( int*)
_temp1465) == Cyc_Absyn_ArrayType: 0){ _LL1476: _temp1475=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1465)->f1; goto _LL1474; _LL1474: _temp1473=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1465)->f2; goto _LL1472; _LL1472: _temp1471=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1465)->f3; goto _LL1468;} else{ goto _LL1469;} _LL1469: goto _LL1470;
_LL1468: { void* _temp1479; struct _tuple6 _temp1477= Cyc_Tcutil_addressof_props(
te, e); _LL1480: _temp1479= _temp1477.f2; goto _LL1478; _LL1478: { void*
_temp1481= _temp1471 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1482=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1482[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1483; _temp1483.tag= Cyc_Absyn_Upper_b;
_temp1483.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1471); _temp1483;});
_temp1482;}); t= Cyc_Absyn_atb_typ( _temp1475, _temp1479, _temp1473, _temp1481);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1470: return t; _LL1466:;} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1484=( void*) e->r; void* _temp1494; _LL1486: if(*(( int*)
_temp1484) == Cyc_Absyn_Array_e){ goto _LL1487;} else{ goto _LL1488;} _LL1488:
if(*(( int*) _temp1484) == Cyc_Absyn_Comprehension_e){ goto _LL1489;} else{ goto
_LL1490;} _LL1490: if(*(( int*) _temp1484) == Cyc_Absyn_Const_e){ _LL1495:
_temp1494=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1484)->f1; if((
unsigned int) _temp1494 > 1u?*(( int*) _temp1494) == Cyc_Absyn_String_c: 0){
goto _LL1491;} else{ goto _LL1492;}} else{ goto _LL1492;} _LL1492: goto _LL1493;
_LL1487: return t; _LL1489: return t; _LL1491: return t; _LL1493: t= Cyc_Tcutil_compress(
t);{ void* _temp1496= t; struct Cyc_Absyn_Exp* _temp1502; struct Cyc_Absyn_Tqual
_temp1504; void* _temp1506; _LL1498: if(( unsigned int) _temp1496 > 4u?*(( int*)
_temp1496) == Cyc_Absyn_ArrayType: 0){ _LL1507: _temp1506=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1496)->f1; goto _LL1505; _LL1505: _temp1504=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1496)->f2; goto _LL1503; _LL1503: _temp1502=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1496)->f3; goto _LL1499;} else{ goto _LL1500;} _LL1500: goto _LL1501;
_LL1499: { void* _temp1510; struct _tuple6 _temp1508= Cyc_Tcutil_addressof_props(
te, e); _LL1511: _temp1510= _temp1508.f2; goto _LL1509; _LL1509: { void* b=
_temp1502 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1512=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1512[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1513; _temp1513.tag= Cyc_Absyn_Upper_b;
_temp1513.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1502); _temp1513;});
_temp1512;}); t= Cyc_Absyn_atb_typ( _temp1506, _temp1510, _temp1504, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1501: return t; _LL1497:;} _LL1485:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1514=( void*) e->r; struct Cyc_Absyn_Exp* _temp1520; _LL1516: if(*(( int*)
_temp1514) == Cyc_Absyn_NoInstantiate_e){ _LL1521: _temp1520=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1514)->f1; goto _LL1517;} else{ goto _LL1518;} _LL1518: goto _LL1519;
_LL1517: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1520);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1520->topt))->v=( void*) Cyc_Absyn_pointer_expand(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1520->topt))->v)); e->topt= _temp1520->topt;
goto _LL1515; _LL1519: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1522=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1528; struct Cyc_Absyn_Conref* _temp1530; struct
Cyc_Absyn_Tqual _temp1532; struct Cyc_Absyn_Conref* _temp1534; void* _temp1536;
void* _temp1538; _LL1524: if(( unsigned int) _temp1522 > 4u?*(( int*) _temp1522)
== Cyc_Absyn_PointerType: 0){ _LL1529: _temp1528=(( struct Cyc_Absyn_PointerType_struct*)
_temp1522)->f1; _LL1539: _temp1538=( void*) _temp1528.elt_typ; goto _LL1537;
_LL1537: _temp1536=( void*) _temp1528.rgn_typ; goto _LL1535; _LL1535: _temp1534=
_temp1528.nullable; goto _LL1533; _LL1533: _temp1532= _temp1528.tq; goto _LL1531;
_LL1531: _temp1530= _temp1528.bounds; goto _LL1525;} else{ goto _LL1526;}
_LL1526: goto _LL1527; _LL1525:{ void* _temp1540= Cyc_Tcutil_compress( _temp1538);
struct Cyc_Absyn_FnInfo _temp1546; struct Cyc_List_List* _temp1548; struct Cyc_List_List*
_temp1550; struct Cyc_Absyn_VarargInfo* _temp1552; int _temp1554; struct Cyc_List_List*
_temp1556; void* _temp1558; struct Cyc_Core_Opt* _temp1560; struct Cyc_List_List*
_temp1562; _LL1542: if(( unsigned int) _temp1540 > 4u?*(( int*) _temp1540) ==
Cyc_Absyn_FnType: 0){ _LL1547: _temp1546=(( struct Cyc_Absyn_FnType_struct*)
_temp1540)->f1; _LL1563: _temp1562= _temp1546.tvars; goto _LL1561; _LL1561:
_temp1560= _temp1546.effect; goto _LL1559; _LL1559: _temp1558=( void*) _temp1546.ret_typ;
goto _LL1557; _LL1557: _temp1556= _temp1546.args; goto _LL1555; _LL1555:
_temp1554= _temp1546.c_varargs; goto _LL1553; _LL1553: _temp1552= _temp1546.cyc_varargs;
goto _LL1551; _LL1551: _temp1550= _temp1546.rgn_po; goto _LL1549; _LL1549:
_temp1548= _temp1546.attributes; goto _LL1543;} else{ goto _LL1544;} _LL1544:
goto _LL1545; _LL1543: if( _temp1562 != 0){ struct _RegionHandle _temp1564=
_new_region(); struct _RegionHandle* rgn=& _temp1564; _push_region( rgn);{
struct _tuple4 _temp1565=({ struct _tuple4 _temp1575; _temp1575.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1575.f2= rgn; _temp1575;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1565, _temp1562); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1572=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1572[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1573; _temp1573.tag= Cyc_Absyn_FnType; _temp1573.f1=({ struct Cyc_Absyn_FnInfo
_temp1574; _temp1574.tvars= 0; _temp1574.effect= _temp1560; _temp1574.ret_typ=(
void*) _temp1558; _temp1574.args= _temp1556; _temp1574.c_varargs= _temp1554;
_temp1574.cyc_varargs= _temp1552; _temp1574.rgn_po= _temp1550; _temp1574.attributes=
_temp1548; _temp1574;}); _temp1573;}); _temp1572;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1569=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1569[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1570; _temp1570.tag= Cyc_Absyn_PointerType;
_temp1570.f1=({ struct Cyc_Absyn_PtrInfo _temp1571; _temp1571.elt_typ=( void*)
ftyp; _temp1571.rgn_typ=( void*) _temp1536; _temp1571.nullable= _temp1534;
_temp1571.tq= _temp1532; _temp1571.bounds= _temp1530; _temp1571;}); _temp1570;});
_temp1569;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1566=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1566[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1567; _temp1567.tag= Cyc_Absyn_Instantiate_e;
_temp1567.f1= inner; _temp1567.f2= ts; _temp1567;}); _temp1566;}))); e->topt=({
struct Cyc_Core_Opt* _temp1568=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1568->v=( void*) new_typ; _temp1568;});}; _pop_region( rgn);}
goto _LL1541; _LL1545: goto _LL1541; _LL1541:;} goto _LL1523; _LL1527: goto
_LL1523; _LL1523:;} goto _LL1515; _LL1515:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1576=( void*) e->r; struct Cyc_Absyn_Exp* _temp1650; struct
_tuple1* _temp1652; struct Cyc_List_List* _temp1654; struct Cyc_Absyn_Exp*
_temp1656; struct Cyc_List_List* _temp1658; struct Cyc_Core_Opt* _temp1660; void*
_temp1662; void* _temp1664; struct _tuple1* _temp1666; struct Cyc_List_List*
_temp1668; void* _temp1670; void* _temp1672; struct Cyc_Absyn_Exp* _temp1674;
struct Cyc_Absyn_Exp* _temp1676; struct Cyc_Core_Opt* _temp1678; struct Cyc_Absyn_Exp*
_temp1680; struct Cyc_Absyn_Exp* _temp1682; struct Cyc_Absyn_Exp* _temp1684;
struct Cyc_Absyn_Exp* _temp1686; struct Cyc_Absyn_Exp* _temp1688; struct Cyc_Absyn_Exp*
_temp1690; struct Cyc_Absyn_VarargCallInfo* _temp1692; struct Cyc_Absyn_VarargCallInfo**
_temp1694; struct Cyc_List_List* _temp1695; struct Cyc_Absyn_Exp* _temp1697;
struct Cyc_Absyn_Exp* _temp1699; struct Cyc_List_List* _temp1701; struct Cyc_Absyn_Exp*
_temp1703; struct Cyc_Absyn_Exp* _temp1705; void* _temp1707; struct Cyc_Absyn_Exp*
_temp1709; struct Cyc_Absyn_Exp* _temp1711; struct Cyc_Absyn_Exp* _temp1713;
struct Cyc_Absyn_Exp* _temp1715; void* _temp1717; struct _tagged_arr* _temp1719;
void* _temp1721; struct Cyc_Absyn_Exp* _temp1723; struct _tagged_arr* _temp1725;
struct Cyc_Absyn_Exp* _temp1727; struct _tagged_arr* _temp1729; struct Cyc_Absyn_Exp*
_temp1731; struct Cyc_Absyn_Exp* _temp1733; struct Cyc_Absyn_Exp* _temp1735;
struct Cyc_List_List* _temp1737; struct Cyc_List_List* _temp1739; struct _tuple2*
_temp1741; struct Cyc_List_List* _temp1743; struct Cyc_Absyn_Stmt* _temp1745;
struct Cyc_Absyn_Fndecl* _temp1747; struct Cyc_Absyn_Exp* _temp1749; struct Cyc_Absyn_Exp*
_temp1751; struct Cyc_Absyn_Exp* _temp1753; struct Cyc_Absyn_Vardecl* _temp1755;
struct Cyc_Absyn_Structdecl* _temp1757; struct Cyc_Absyn_Structdecl** _temp1759;
struct Cyc_List_List* _temp1760; struct Cyc_Core_Opt* _temp1762; struct Cyc_Core_Opt**
_temp1764; struct _tuple1* _temp1765; struct _tuple1** _temp1767; struct Cyc_List_List*
_temp1768; void* _temp1770; struct Cyc_Absyn_Tunionfield* _temp1772; struct Cyc_Absyn_Tuniondecl*
_temp1774; struct Cyc_List_List* _temp1776; struct Cyc_Core_Opt* _temp1778;
struct Cyc_Core_Opt** _temp1780; struct Cyc_Core_Opt* _temp1781; struct Cyc_Core_Opt**
_temp1783; struct Cyc_Absyn_Enumfield* _temp1784; struct Cyc_Absyn_Enumdecl*
_temp1786; struct _tuple1* _temp1788; struct _tuple1** _temp1790; void*
_temp1791; struct Cyc_Absyn_Exp* _temp1793; _LL1578: if(*(( int*) _temp1576) ==
Cyc_Absyn_NoInstantiate_e){ _LL1651: _temp1650=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1576)->f1; goto _LL1579;} else{ goto _LL1580;} _LL1580: if(*(( int*)
_temp1576) == Cyc_Absyn_UnknownId_e){ _LL1653: _temp1652=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1576)->f1; goto _LL1581;} else{ goto _LL1582;} _LL1582: if(*(( int*)
_temp1576) == Cyc_Absyn_UnknownCall_e){ _LL1657: _temp1656=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1576)->f1; goto _LL1655; _LL1655: _temp1654=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1576)->f2; goto _LL1583;} else{ goto _LL1584;} _LL1584: if(*(( int*)
_temp1576) == Cyc_Absyn_UnresolvedMem_e){ _LL1661: _temp1660=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1576)->f1; goto _LL1659; _LL1659: _temp1658=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1576)->f2; goto _LL1585;} else{ goto _LL1586;} _LL1586: if(*(( int*)
_temp1576) == Cyc_Absyn_Const_e){ _LL1663: _temp1662=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1576)->f1; goto _LL1587;} else{ goto _LL1588;} _LL1588: if(*(( int*)
_temp1576) == Cyc_Absyn_Var_e){ _LL1667: _temp1666=(( struct Cyc_Absyn_Var_e_struct*)
_temp1576)->f1; goto _LL1665; _LL1665: _temp1664=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1576)->f2; goto _LL1589;} else{ goto _LL1590;} _LL1590: if(*(( int*)
_temp1576) == Cyc_Absyn_Primop_e){ _LL1671: _temp1670=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1576)->f1; goto _LL1669; _LL1669: _temp1668=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1576)->f2; goto _LL1591;} else{ goto _LL1592;} _LL1592: if(*(( int*)
_temp1576) == Cyc_Absyn_Increment_e){ _LL1675: _temp1674=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1576)->f1; goto _LL1673; _LL1673: _temp1672=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1576)->f2; goto _LL1593;} else{ goto _LL1594;} _LL1594: if(*(( int*)
_temp1576) == Cyc_Absyn_AssignOp_e){ _LL1681: _temp1680=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1576)->f1; goto _LL1679; _LL1679: _temp1678=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1576)->f2; goto _LL1677; _LL1677: _temp1676=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1576)->f3; goto _LL1595;} else{ goto _LL1596;} _LL1596: if(*(( int*)
_temp1576) == Cyc_Absyn_Conditional_e){ _LL1687: _temp1686=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1576)->f1; goto _LL1685; _LL1685: _temp1684=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1576)->f2; goto _LL1683; _LL1683: _temp1682=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1576)->f3; goto _LL1597;} else{ goto _LL1598;} _LL1598: if(*(( int*)
_temp1576) == Cyc_Absyn_SeqExp_e){ _LL1691: _temp1690=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1576)->f1; goto _LL1689; _LL1689: _temp1688=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1576)->f2; goto _LL1599;} else{ goto _LL1600;} _LL1600: if(*(( int*)
_temp1576) == Cyc_Absyn_FnCall_e){ _LL1698: _temp1697=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1576)->f1; goto _LL1696; _LL1696: _temp1695=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1576)->f2; goto _LL1693; _LL1693: _temp1692=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1576)->f3; _temp1694=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1576)->f3;
goto _LL1601;} else{ goto _LL1602;} _LL1602: if(*(( int*) _temp1576) == Cyc_Absyn_Throw_e){
_LL1700: _temp1699=(( struct Cyc_Absyn_Throw_e_struct*) _temp1576)->f1; goto
_LL1603;} else{ goto _LL1604;} _LL1604: if(*(( int*) _temp1576) == Cyc_Absyn_Instantiate_e){
_LL1704: _temp1703=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1576)->f1;
goto _LL1702; _LL1702: _temp1701=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1576)->f2; goto _LL1605;} else{ goto _LL1606;} _LL1606: if(*(( int*)
_temp1576) == Cyc_Absyn_Cast_e){ _LL1708: _temp1707=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1576)->f1; goto _LL1706; _LL1706: _temp1705=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1576)->f2; goto _LL1607;} else{ goto _LL1608;} _LL1608: if(*(( int*)
_temp1576) == Cyc_Absyn_Address_e){ _LL1710: _temp1709=(( struct Cyc_Absyn_Address_e_struct*)
_temp1576)->f1; goto _LL1609;} else{ goto _LL1610;} _LL1610: if(*(( int*)
_temp1576) == Cyc_Absyn_New_e){ _LL1714: _temp1713=(( struct Cyc_Absyn_New_e_struct*)
_temp1576)->f1; goto _LL1712; _LL1712: _temp1711=(( struct Cyc_Absyn_New_e_struct*)
_temp1576)->f2; goto _LL1611;} else{ goto _LL1612;} _LL1612: if(*(( int*)
_temp1576) == Cyc_Absyn_Sizeofexp_e){ _LL1716: _temp1715=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1576)->f1; goto _LL1613;} else{ goto _LL1614;} _LL1614: if(*(( int*)
_temp1576) == Cyc_Absyn_Sizeoftyp_e){ _LL1718: _temp1717=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1576)->f1; goto _LL1615;} else{ goto _LL1616;} _LL1616: if(*(( int*)
_temp1576) == Cyc_Absyn_Offsetof_e){ _LL1722: _temp1721=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1576)->f1; goto _LL1720; _LL1720: _temp1719=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1576)->f2; goto _LL1617;} else{ goto _LL1618;} _LL1618: if(*(( int*)
_temp1576) == Cyc_Absyn_Deref_e){ _LL1724: _temp1723=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1576)->f1; goto _LL1619;} else{ goto _LL1620;} _LL1620: if(*(( int*)
_temp1576) == Cyc_Absyn_StructMember_e){ _LL1728: _temp1727=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1576)->f1; goto _LL1726; _LL1726: _temp1725=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1576)->f2; goto _LL1621;} else{ goto _LL1622;} _LL1622: if(*(( int*)
_temp1576) == Cyc_Absyn_StructArrow_e){ _LL1732: _temp1731=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1576)->f1; goto _LL1730; _LL1730: _temp1729=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1576)->f2; goto _LL1623;} else{ goto _LL1624;} _LL1624: if(*(( int*)
_temp1576) == Cyc_Absyn_Subscript_e){ _LL1736: _temp1735=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1576)->f1; goto _LL1734; _LL1734: _temp1733=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1576)->f2; goto _LL1625;} else{ goto _LL1626;} _LL1626: if(*(( int*)
_temp1576) == Cyc_Absyn_Tuple_e){ _LL1738: _temp1737=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1576)->f1; goto _LL1627;} else{ goto _LL1628;} _LL1628: if(*(( int*)
_temp1576) == Cyc_Absyn_CompoundLit_e){ _LL1742: _temp1741=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1576)->f1; goto _LL1740; _LL1740: _temp1739=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1576)->f2; goto _LL1629;} else{ goto _LL1630;} _LL1630: if(*(( int*)
_temp1576) == Cyc_Absyn_Array_e){ _LL1744: _temp1743=(( struct Cyc_Absyn_Array_e_struct*)
_temp1576)->f1; goto _LL1631;} else{ goto _LL1632;} _LL1632: if(*(( int*)
_temp1576) == Cyc_Absyn_StmtExp_e){ _LL1746: _temp1745=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1576)->f1; goto _LL1633;} else{ goto _LL1634;} _LL1634: if(*(( int*)
_temp1576) == Cyc_Absyn_Codegen_e){ _LL1748: _temp1747=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1576)->f1; goto _LL1635;} else{ goto _LL1636;} _LL1636: if(*(( int*)
_temp1576) == Cyc_Absyn_Fill_e){ _LL1750: _temp1749=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1576)->f1; goto _LL1637;} else{ goto _LL1638;} _LL1638: if(*(( int*)
_temp1576) == Cyc_Absyn_Comprehension_e){ _LL1756: _temp1755=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1576)->f1; goto _LL1754; _LL1754: _temp1753=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1576)->f2; goto _LL1752; _LL1752: _temp1751=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1576)->f3; goto _LL1639;} else{ goto _LL1640;} _LL1640: if(*(( int*)
_temp1576) == Cyc_Absyn_Struct_e){ _LL1766: _temp1765=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1576)->f1; _temp1767=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1576)->f1;
goto _LL1763; _LL1763: _temp1762=(( struct Cyc_Absyn_Struct_e_struct*) _temp1576)->f2;
_temp1764=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1576)->f2; goto _LL1761;
_LL1761: _temp1760=(( struct Cyc_Absyn_Struct_e_struct*) _temp1576)->f3; goto
_LL1758; _LL1758: _temp1757=(( struct Cyc_Absyn_Struct_e_struct*) _temp1576)->f4;
_temp1759=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1576)->f4; goto _LL1641;}
else{ goto _LL1642;} _LL1642: if(*(( int*) _temp1576) == Cyc_Absyn_AnonStruct_e){
_LL1771: _temp1770=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1576)->f1;
goto _LL1769; _LL1769: _temp1768=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1576)->f2; goto _LL1643;} else{ goto _LL1644;} _LL1644: if(*(( int*)
_temp1576) == Cyc_Absyn_Tunion_e){ _LL1782: _temp1781=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1576)->f1; _temp1783=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1576)->f1;
goto _LL1779; _LL1779: _temp1778=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1576)->f2;
_temp1780=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1576)->f2; goto _LL1777;
_LL1777: _temp1776=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1576)->f3; goto
_LL1775; _LL1775: _temp1774=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1576)->f4;
goto _LL1773; _LL1773: _temp1772=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1576)->f5;
goto _LL1645;} else{ goto _LL1646;} _LL1646: if(*(( int*) _temp1576) == Cyc_Absyn_Enum_e){
_LL1789: _temp1788=(( struct Cyc_Absyn_Enum_e_struct*) _temp1576)->f1; _temp1790=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1576)->f1; goto _LL1787; _LL1787:
_temp1786=(( struct Cyc_Absyn_Enum_e_struct*) _temp1576)->f2; goto _LL1785;
_LL1785: _temp1784=(( struct Cyc_Absyn_Enum_e_struct*) _temp1576)->f3; goto
_LL1647;} else{ goto _LL1648;} _LL1648: if(*(( int*) _temp1576) == Cyc_Absyn_Malloc_e){
_LL1794: _temp1793=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1576)->f1; goto
_LL1792; _LL1792: _temp1791=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1576)->f2; goto _LL1649;} else{ goto _LL1577;} _LL1579: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1650); return; _LL1581: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1652); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1583: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1656, _temp1654); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1585: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1658); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1587: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1662, e);
goto _LL1577; _LL1589: t= Cyc_Tcexp_tcVar( te, loc, _temp1666, _temp1664); goto
_LL1577; _LL1591: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1670, _temp1668);
goto _LL1577; _LL1593: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1674,
_temp1672); goto _LL1577; _LL1595: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1680, _temp1678, _temp1676); goto _LL1577; _LL1597: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1686, _temp1684, _temp1682); goto _LL1577; _LL1599: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1690, _temp1688); goto _LL1577; _LL1601: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1697, _temp1695, _temp1694); goto _LL1577; _LL1603: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1699); goto _LL1577; _LL1605: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1703, _temp1701); goto _LL1577; _LL1607: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1707, _temp1705); goto _LL1577; _LL1609: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1709); goto _LL1577; _LL1611: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1713, e, _temp1711); goto _LL1577; _LL1613: { void* _temp1795= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1715); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1795); goto
_LL1577;} _LL1615: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1717); goto
_LL1577; _LL1617: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1721, _temp1719);
goto _LL1577; _LL1619: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1723); goto
_LL1577; _LL1621: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1727,
_temp1725); goto _LL1577; _LL1623: t= Cyc_Tcexp_tcStructArrow( te, loc, topt,
_temp1731, _temp1729); goto _LL1577; _LL1625: t= Cyc_Tcexp_tcSubscript( te, loc,
topt, _temp1735, _temp1733); goto _LL1577; _LL1627: t= Cyc_Tcexp_tcTuple( te,
loc, topt, _temp1737); goto _LL1577; _LL1629: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1741, _temp1739); goto _LL1577; _LL1631: { void** elt_topt= 0;
if( topt != 0){ void* _temp1796= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); void* _temp1802; void** _temp1804; _LL1798: if(( unsigned int)
_temp1796 > 4u?*(( int*) _temp1796) == Cyc_Absyn_ArrayType: 0){ _LL1803:
_temp1802=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1796)->f1;
_temp1804=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1796)->f1); goto
_LL1799;} else{ goto _LL1800;} _LL1800: goto _LL1801; _LL1799: elt_topt=( void**)
_temp1804; goto _LL1797; _LL1801: goto _LL1797; _LL1797:;} t= Cyc_Tcexp_tcArray(
te, loc, elt_topt, _temp1743); goto _LL1577;} _LL1633: t= Cyc_Tcexp_tcStmtExp(
te, loc, topt, _temp1745); goto _LL1577; _LL1635: t= Cyc_Tcexp_tcCodegen( te,
loc, topt, _temp1747); goto _LL1577; _LL1637: t= Cyc_Tcexp_tcFill( te, loc, topt,
_temp1749); goto _LL1577; _LL1639: t= Cyc_Tcexp_tcComprehension( te, loc, topt,
_temp1755, _temp1753, _temp1751); goto _LL1577; _LL1641: t= Cyc_Tcexp_tcStruct(
te, loc, topt, _temp1767, _temp1764, _temp1760, _temp1759); goto _LL1577;
_LL1643: t= Cyc_Tcexp_tcAnonStruct( te, loc, _temp1770, _temp1768); goto _LL1577;
_LL1645: t= Cyc_Tcexp_tcTunion( te, loc, topt, e, _temp1783, _temp1780,
_temp1776, _temp1774, _temp1772); goto _LL1577; _LL1647:* _temp1790=(( struct
Cyc_Absyn_Enumfield*) _check_null( _temp1784))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp1805=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1805[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1806; _temp1806.tag= Cyc_Absyn_EnumType;
_temp1806.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1786))->name;
_temp1806.f2= _temp1786; _temp1806;}); _temp1805;}); goto _LL1577; _LL1649: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1793, _temp1791); goto _LL1577; _LL1577:;}
e->topt=({ struct Cyc_Core_Opt* _temp1807=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1807->v=( void*) t; _temp1807;});}
